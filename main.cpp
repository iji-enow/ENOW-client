#include <iostream>
#include <json.hpp>
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "header/KISA_SHA256.h"
}
#include <string>
#include <boost/locale.hpp>
#include "./header/MQTTClientPool.hpp"

#define BUFSIZ 4096

using namespace std;
using json = nlohmann::json;

static int verbose_flag;

static string fromLocale(const string &localeString){
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);
	std::locale locale = generator(boost::locale::util::get_system_locale());
	return boost::locale::conv::to_utf<char>(localeString, locale);
}

int main(int argc, char **argv){
	string address,\
		clientID;

	bool addressFlag = false,\
					   idFlag = false,\
					   topicFlag = false;

	char *p_address = NULL,\
					  *p_id = NULL,\
					  *p_topic = NULL,\
					  *p_SHA256_token = NULL,\
					  *p_SHA256 = NULL,\
					  *p_token = NULL;

	objMQTTClientPool *p_pool = NULL;

	int c = 0;
	opterr = 0;
	json j;

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

	while(true){
		int index = 0;
		char linebuffer[BUFSIZ];
		

		memset(linebuffer, 0, sizeof(linebuffer));
		while((linebuffer[index++] = cin.get()) != EOF){

		}
		auto jsonTerminal = json::parse(linebuffer);
		
		auto topic = jsonTerminal["topic"].get<string>();
		auto payload = jsonTerminal["payload"].get<string>();
		string topic_utf8;

		if(jsonTerminal.find("configuration") != jsonTerminal.end()){
			auto configuration = jsonTerminal["configuration"].get<object>();
		}

		p_topic = topic.c_str();
		memset(p_SHA256_token, 0, BUFSIZ);
		memset(p_SHA256, 0, 4 * BUFSIZ);

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
		objMQTTClient client_t;

		topic.clear();
		topic += p_SHA256;
		topic_utf8 = fromLocale(topic);

		string myLWT = "My LWT";
		string myLWTUTF8 = fromLocale(myLWT);

		client_t.createClient(address, clientID);
		client_t.setConnectOptions(60,\
				false,\
				true,\
				30);

		client_t.setLWT(myLWTUTF8.c_str(),\
				1,\
				1);
		client_t.
	}

	topic += p_SHA256;
	topic_utf8 = fromLocale(topic);

	objMQTTClient *client = new objMQTTClient();
	client->createClient(address, clientID);
	client->setPersistenceConnection();
	client->setConnectionInterval(20);
	client->clientConnect();
	client->setPayload(payload.length(),\
			p_payload);
	client->publish(topic_utf8,\
			10000L);

	client->listen(topic_utf8.c_str(),\
			1);

	return 0;
}
