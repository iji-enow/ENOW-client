#include <iostream>
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "header/KISA_SHA256.h"
}
#include <string>
#include <boost/locale.hpp>
#include "./header/MQTTClientPool.hpp"
#include "./header/json.hpp"

using namespace std;
using json = nlohmann::json;

static int verbose_flag;

static const char *p_arch;
static const char *p_macAddress;
static int endianess;

void systemCheck(void){
	int filedes = 0,\
				  endian_t = 0,\
				  family_t = 0,\
				  index_t = 0,\
				  status_t = 0;
	unsigned int rx_t = 0,\
				  tx_t = 0;
	char *p_buffer = (char *)malloc(sizeof(char) * BUFSIZ);
	char *p_token = NULL,\
					*p_arch_t = NULL,\
					*p_macAddress_t = NULL;
	char host[NI_MAXHOST];
	char interface[BUFSIZ],\
		path[BUFSIZ];

	memset(p_buffer, 0, sizeof(BUFSIZ));

	struct ifaddrs *p_ifaddr = NULL,\
							   *p_ifa = NULL;

	if((filedes = open("/proc/cpuinfo", O_RDONLY)) == -1){
		fprintf(stderr, "Error while opening /proc/cpuinfo, consider giving the program root privilege\n");
		exit(1);
	}
	
	if(read(filedes, p_buffer, BUFSIZ) == -1){
		fprintf(stderr, "Error while reading /proc/cpuinfo, consider giving the program root privilege\n");
		exit(1);
	}

	close(filedes);
	
	p_token = strtok(p_buffer, ":\t\n");
	while(p_token != NULL){
		if(strcmp(p_token, "model name") == 0){
			while(1){
				p_token = strtok(NULL, ":\t\n");
				if(strlen(p_token) != 0){
					p_arch_t = (char *)malloc(BUFSIZ * sizeof(char));
					memset(p_arch_t, 0, BUFSIZ);
					strcpy(p_arch_t, p_token);
					p_arch = p_arch_t;
					break;
				}
			}
			break;
		}
		p_token = strtok(NULL, ":\t\n");
	}
	
	endian_t = 1;
	if(*(char *)&endian_t == 1)
		endianess = 1;
	else
		endianess = 2;

	if(getifaddrs(&p_ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}

	for(p_ifa = p_ifaddr, index_t = 0;\
			p_ifa != NULL;\
			p_ifa = p_ifa->ifa_next, index_t++){

		if(p_ifa->ifa_addr == NULL)
			continue;
		family_t = p_ifa->ifa_addr->sa_family;

		if(family_t == AF_PACKET &&\
				p_ifa->ifa_data != NULL){
			struct rtnl_link_stats *p_stats = (struct rtnl_link_stats *)p_ifa->ifa_data;

			if(p_stats->tx_bytes > tx_t ||
					p_stats->rx_bytes > rx_t){
				tx_t = p_stats->tx_bytes;
				rx_t = p_stats->rx_bytes;
				
				memset(interface, 0, BUFSIZ);
				sprintf(interface, "%s", p_ifa->ifa_name);
			}
		}
	}
	
	memset(path, 0, BUFSIZ);
	strcat(path, "/sys/class/net/");
	strcat(path, interface);
	strcat(path, "/address");

	if((filedes = open(path, O_RDONLY)) == -1){
		fprintf(stderr, "Error while opening network interface, consider giving the program root privi    lege\n");
		exit(1);
	}

	if(read(filedes, p_buffer, BUFSIZ) == -1){
		fprintf(stderr, "Error while reading network interface, consider giving the program root privi    lege\n");
		exit(1);
	}

	close(filedes);
	
	p_token = strtok(p_buffer, "\n");
	p_macAddress_t = (char *)malloc(BUFSIZ * sizeof(char));
	strcpy(p_macAddress_t, p_token);
	p_macAddress = p_macAddress_t;
}


static string fromLocale(const string &localeString){
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);
	std::locale locale = generator(boost::locale::util::get_system_locale());
	return boost::locale::conv::to_utf<char>(localeString, locale);
}

int main(int argc, char **argv){
	string address,\
		clientID,\
		topic,\
		topic_utf8,\
		jsonTerminal_str,\
		payload,\
		w_payload,\
		payload_utf8,\
		myLWT,\
		myLWT_utf8;

	bool addressFlag = false,\
					   idFlag = false,\
					   topicFlag = false;

	char *p_address = NULL,\
					  *p_id = NULL,\
					  *p_topic = NULL,\
					  *p_SHA256_token = NULL,\
					  *p_SHA256 = NULL,\
					  *p_token = NULL,\
					  *p_linebuffer = NULL;

	objMQTTClientPool *p_pool = NULL;

	int c = 0;
	opterr = 0;
	json j;

	systemCheck();

	while(1){
		static struct option long_options[] = {
			{"address", required_argument, 0, 'a'},
			//			{"topic", required_argument, 0, 't'},
			{"clientID", required_argument, 0, 'i'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long(argc,\
				argv,\
				"a:i:",\
				long_options,\
				&option_index);

		if(c == -1)
			break;

		switch(c){
			case 0:
				if(long_options[option_index].flag != 0)
					break;
				printf("option %s", long_options[option_index].name);
				if(optarg)
					printf(" with arg %s", optarg);
				printf("\n");
				break;
			case 'a':
				printf("address : %s\n", optarg);
				p_address = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
				memcpy(p_address, optarg, strlen(optarg) + 1);
				break;
				/*			case 't':
							printf("topic : %s\n", optarg);
							p_topic = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
							memcpy(p_topic, optarg, strlen(optarg) + 1);
							break;
							*/			case 'i':
				printf("client id : %s\n", optarg);
				p_id = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
				memcpy(p_id, optarg, strlen(optarg) + 1);
				break;
			case '?':
				break;
			default:
				abort();
		}
	}

	address += p_address;
	clientID += p_id;

	p_pool = new objMQTTClientPool();
	p_SHA256_token = (char *)malloc(sizeof(char) * BUFSIZ);
	memset(p_SHA256_token, 0, BUFSIZ);
	p_SHA256 = (char *)malloc(sizeof(char) * BUFSIZ * 4);
	memset(p_SHA256, 0, BUFSIZ);
	p_linebuffer = (char *)malloc(sizeof(char) * BUFSIZ);
	memset(p_linebuffer, 0, BUFSIZ);
	p_topic = (char *)malloc(BUFSIZ * sizeof(char));
	memset(p_topic, 0, BUFSIZ);

	while(true){
		memset(p_linebuffer, 0, BUFSIZ);
		memset(p_SHA256, 0, BUFSIZ * 4);
		memset(p_SHA256_token, 0, BUFSIZ);
		memset(p_topic, 0, BUFSIZ);
		topic.clear();
		payload.clear();
		payload_utf8.clear();
		jsonTerminal_str.clear();
		w_payload.clear();
		myLWT.clear();
		myLWT_utf8.clear();

		if(fgets(p_linebuffer, BUFSIZ, stdin) == NULL){
			perror("Error reading input");
			exit(1);
		}
		p_linebuffer[strlen(p_linebuffer) - 1] = 0;

		json jsonTerminal = json::parse(p_linebuffer);

		topic = jsonTerminal["topic"].get<string>();
		payload = jsonTerminal["payload"].get<string>();

		if(jsonTerminal.find("metadata") == jsonTerminal.end()){
			jsonTerminal["metadata"]["arch"] = string(p_arch);
			jsonTerminal["metadata"]["mac_address"] = string(p_macAddress);
			jsonTerminal["metadata"]["endian"] = endianess;
		}

		jsonTerminal_str = jsonTerminal.dump();

		strcpy(p_topic, topic.c_str());
		p_token = strtok(p_topic, "/");
		while(p_token != NULL){
			if(strlen(p_token) != 0){
				SHA256_Encrpyt((const BYTE *)p_token, (UINT)strlen(p_token) + 1, (BYTE *)p_SHA256_token);	
				strcat(p_SHA256, "/");
				strcat(p_SHA256, p_SHA256_token);
				memset(p_SHA256_token, 0, BUFSIZ);
			}
			p_token = strtok(NULL, "/");
		}
		objMQTTClient *p_client;

		topic += p_SHA256;
		topic_utf8 = fromLocale(topic);
		w_payload = jsonTerminal.dump();
		payload_utf8 = fromLocale(w_payload);

		if((p_client = p_pool->findClient(topic_utf8)) == NULL){
			p_client = new objMQTTClient();
			MQTTClient_message msg_t = MQTTClient_message_initializer;
			myLWT = "My LWT";
			myLWT_utf8 = fromLocale(myLWT);
			p_client->setTopic(topic_utf8);
			p_client->createClient(address, clientID);
			p_client->clientConnect();
			p_client->setConnectOptions(60,\
					false,\
					true,\
					30);
			p_client->setLWT(myLWT_utf8.c_str(),\
					1,\
					1);
			p_client->setPayload(msg_t,\
					strlen(payload_utf8.c_str()),\
					payload_utf8.c_str());

			p_client->publish(msg_t,\
					30);
			sleep(1.0);
			p_client->listen();
			
			p_pool->insertClient(p_client);
		}
		else{
			MQTTClient_message msg_t = MQTTClient_message_initializer;
			p_client->clientConnect();
			p_client->setPayload(msg_t,\
					strlen(payload_utf8.c_str()),\
					payload_utf8.c_str());
			p_client->publish(msg_t,\
					30);
		}
	}

	free(p_address);
	free(p_id);
	free(p_topic);
	free(p_SHA256_token);
	free(p_SHA256);
	free(p_linebuffer);
	return 0;
}
