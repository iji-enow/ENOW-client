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
#include <pthread.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "header/KISA_SHA256.h"
}
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <boost/locale.hpp>
#include "./header/MQTTClientPool.hpp"
#include "./json/json.hh"

using namespace std;
using namespace JSON;

static int verbose_flag;

static const char *p_arch;
static const char *p_macAddress;
static int endianess;
static mutex pool_mutex;
static objMQTTClientPool *p_pool;
static objMQTTClient *leaderObject = NULL;
static string address, clientID;
static key_t globalKey;

/*
 * A function for checking system-wise data, like
 * 	System architecture
 * 	Endianess
 * 	Mac address for active network interfaces
 */

void systemCheck(void){
	int filedes = 0,\
				  endian_t = 0,\
				  family_t = 0,\
				  index_t = 0,\
				  status_t = 0;
	unsigned int rx_t = 0,\
						tx_t = 0;
	char *p_token = NULL,\
					*p_arch_t = NULL,\
					*p_macAddress_t = NULL;
	char host[NI_MAXHOST],\
		interface[BUFSIZ],\
		path[BUFSIZ],\
		p_buffer[BUFSIZ];

	p_arch_t = (char *)malloc(sizeof(char) * BUFSIZ);
	p_macAddress_t = (char *)malloc(sizeof(char) * BUFSIZ);

	memset(p_arch_t, 0, BUFSIZ);
	memset(p_macAddress_t, 0, BUFSIZ);
	memset(p_buffer, 0, BUFSIZ);

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
	strcpy(p_macAddress_t, p_token);
	p_macAddress = p_macAddress_t;
}

/*
 * A function for converting ASCII string to UTF-8 string
 * The function uses Boost library, so you probably need to install it.
 */

static std::string fromLocale(const std::string &localeString){
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);
	std::locale locale = generator(boost::locale::util::get_system_locale());
	return boost::locale::conv::to_utf<char>(localeString, locale);
}

/*
   A function for setting up connection between feedback and alive topic.
*/

void preprocess(const string &major_topic){

	objMQTTClient *p_client_feedback = NULL;

	pool_mutex.lock();
	if((p_client_feedback = p_pool->findClient(major_topic)) == NULL){
		pool_mutex.unlock();
		string initial_str;
		char feedback_str[BUFSIZ] = "/feedback";
		char alive_str[BUFSIZ] = "/alive/request";
		char *const subtopic_list[2] = {feedback_str,\
			alive_str\
		};

		p_client_feedback = new objMQTTClient();

		MQTTClient_message msg_feedback_t = MQTTClient_message_initializer;
		MQTTClient_message msg_alive_t = MQTTClient_message_initializer;
		initial_str = "Initial Payload";

		p_client_feedback->setTopic(major_topic);
		p_client_feedback->setPayload(msg_feedback_t,\
				strlen(initial_str.c_str()),\
				initial_str.c_str(),\
				1,\
				0,\
				0);
		p_client_feedback->setPayload(msg_alive_t,\
				strlen(initial_str.c_str()),\
				initial_str.c_str(),\
				1,\
				0,\
				0);
		p_client_feedback->publish(msg_feedback_t,\
				string("/feedback"),\
				30);
		p_client_feedback->publish(msg_alive_t,\
				string("/alive/request"),\
				30);
		p_client_feedback->listenMany(subtopic_list,\
				2);

		pool_mutex.lock();
		p_pool->insertClient(p_client_feedback);
		pool_mutex.unlock();
	}
}

/*
   A function communicating with another process sharing the same memory region
*/

static void shareMemory(string _payload, string _topic, long long int _key) {
	objMQTTClient *p_client = NULL;
	ostringstream stream;

	char c,\
		buffer[BUFSIZ];
	int sharedMemoryID = 0,
		index = 0;
	key_t key = (key_t)_key;
	void *sharedMemoryRegion = NULL;
	char *currentAddress = NULL;
	string payload,\
		   payload_utf8;
	Value json = parse_string(_payload);

	pool_mutex.lock();
	if((p_client = p_pool->findClient(_topic)) != NULL){
		pool_mutex.unlock();

		if((sharedMemoryID = shmget(key,\
						BUFSIZ,\
						IPC_CREAT | 0600)) < 0){
			perror("shmget");
		}
		else{
			if((sharedMemoryRegion = shmat(sharedMemoryID,\
							(void *)0,\
							0)) == (void *)-1){
				perror("shmat");
			}else{
				currentAddress = (char *)sharedMemoryRegion;
				*currentAddress = '!';

				while(true){
					index = 0;
					stream.str("");
					stream.clear();
					payload.clear();
					payload_utf8.clear();

					cout << "Sending a message to broker" << endl;
					cout << "	TOPIC : " << _topic << endl;
				
					currentAddress = (char *)sharedMemoryRegion;

					while(*currentAddress != '?')
						sleep(1);

					currentAddress = (char *)sharedMemoryRegion;
					currentAddress++;
					while(*currentAddress != '\0'){
						buffer[index] = (char)*currentAddress;
						index++;
						currentAddress++;
					}

					currentAddress = (char *)sharedMemoryRegion;
					*currentAddress = '!';

					Value object = parse_string(string(buffer));

					json["payload"] = object;
					stream << json;
					payload = stream.str();
					payload_utf8 = fromLocale(payload);

					cout << "	PAYLOAD : " << payload_utf8 << endl;
					MQTTClient_message msg_t = MQTTClient_message_initializer;

					p_client->setPayload(msg_t,\
							strlen(payload_utf8.c_str()),\
							payload_utf8.c_str(),\
							1,\
							false,\
							true);

					p_client->publish(msg_t,\
							string("/status"),\
							30);

				}
			}
		}
	}
	else{
		pool_mutex.unlock();
		fprintf(stderr, "Sharing memory space has failed\n");
	}
}

static void signalHandler(int signo) {
	if(signo == SIGINT) {
		if(leaderObject != NULL) {
			leaderObject->disconnect();
			delete leaderObject;
		}
	}
}

/*
 * An entry for the whole program
 * There are 3 main features of this program
 *	1. Connects the device to any remote broker
 *	2. Hashes topics and data for security purpose
 *	3. Creates a thread pool for managing each MQTTClient
 */

int main(int argc, char **argv){
	string topic,\
		jsonTerminal_str;

	bool addressFlag = false,\
					   idFlag = false,\
					   topicFlag = false;

	char *p_address = NULL,\
					  *p_id = NULL,\
					  *p_token = NULL,\
					  *p_token_prev = NULL;
	char p_SHA256[BUFSIZ],\
		p_linebuffer[BUFSIZ],\
		p_topic[BUFSIZ];

	const char *trustStore = "/etc/pki/tls/certs/lesstif-rootca.crt";
	const char *keyStore = "/etc/pki/tls/certs/lesstif.com.crt";
	const char *passPhrase = "Poiu8756.";

	const rlim_t kernelStackSize = 16L * 1024L * 1024L;
	struct rlimit limit;
	int c = 0,\
			result = 0;
	long long int key = 0;
	opterr = 0;
	key = -1;
	/*
	 * A routine for system proprietary data
	 */
	systemCheck();
	/*
	 * A routine for setting up a stack size for the program
	 */

	if((result = getrlimit(RLIMIT_STACK, &limit)) == 0) {
		if(limit.rlim_cur < kernelStackSize) {
			limit.rlim_cur = kernelStackSize;
			if((result = setrlimit(RLIMIT_STACK, &limit)) != 0){
				fprintf(stderr, "Acquiring the size of a stack for the program failed with exit code %d\n", result);
			}
		}
	}

	while(1){
		static struct option long_options[] = {
			{"address", required_argument, 0, 'a'},
			{"key", required_argument, 0, 'k'},
			{"clientID", required_argument, 0, 'i'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long(argc,\
				argv,\
				"a:i:k:",\
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
			case 'k':
				printf("key : %s\n", optarg);
				globalKey = (key_t)atoi(optarg);
				break;
			case 'i':
				printf("client id : %s\n", optarg);
				p_id = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
				memcpy(p_id, optarg, strlen(optarg) + 1);
				break;
			case '?':
				break;
			default:
				fprintf(stderr, "Please enter any parameter\n");
				exit(1);
		}
	}

	/*
	 * A routine for sharing memory region among processes
	 * 	Host program : this
	 * 	Client program : example program
	 * 		i.e) human face detection program in OpenCV
	 */

	address += p_address;
	clientID += p_id;

	p_pool = new objMQTTClientPool();
	memset(p_SHA256, 0, BUFSIZ);
	memset(p_linebuffer, 0, BUFSIZ);
	memset(p_topic, 0, BUFSIZ);

	if(setvbuf(stdin, NULL, _IONBF, 4096) != 0){
		perror("Error setting buffer");
		exit(1);
	}

	signal(SIGINT, signalHandler);

	while(true){
		memset(p_linebuffer, 0, BUFSIZ);
		memset(p_SHA256, 0, BUFSIZ);
		memset(p_topic, 0, BUFSIZ);
		topic.clear();
		jsonTerminal_str.clear();

		if(ferror(stdin) != 0){
			perror("Error indicator is set on stdin");
			exit(1);
		}

		if(!fgets(p_linebuffer, BUFSIZ, stdin)){
			perror("Error reading input");
			exit(1);
		}
		p_linebuffer[strlen(p_linebuffer) - 1] = 0;

		if(fflush(stdin) != 0){
			perror("Error flushing stdin");
			exit(1);
		}

		Value json = parse_string(p_linebuffer);

		topic = (string)json["topic"];
		key = (long long int)json["key"];

		Object object = (Object)json;
		ostringstream stream;
		string payload;

		if(object.find("metadata") == object.end()){
			Object metadata;
			json["metadata"] = metadata;
			json["metadata"]["arch"] = string(p_arch);
			json["metadata"]["mac_address"] = string(p_macAddress);
			json["metadata"]["endian"] = endianess;
		}

		stream << json;
		payload = stream.str();

		if(p_pool->initialize()){
			leaderObject = new objMQTTClient(globalKey);
			leaderObject->createClient(address, clientID);
			leaderObject->setTopic(string(""));
			leaderObject->setConnectOptions(90,\
					true,\
					false,\
					60);
			leaderObject->setSSLOptions(trustStore,\
					keyStore,\
					passPhrase);
			leaderObject->clientConnect();
		}

		thread preprocess_thread{preprocess, ref(topic)};
		preprocess_thread.join();
		thread client_thread {shareMemory, payload, topic, key};
		client_thread.detach();
	}

	free(p_address);
	free(p_id);
	return 0;
}
