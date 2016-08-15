#include <iostream>
#include <nlohmann/json.hpp>
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
}
#include <string>
#include "./header/MQTTClient.hpp"

using namespace std;
using json = nlohmann::json;

static int verbose_flag;

int main(int argc, char **argv){
	string address,\
		clientID,\
		topic,\
		payload;

	bool addressFlag = false,\
					   idFlag = false,\
					   topicFlag = false;

	char *p_address = NULL,\
					  *p_id = NULL,\
					  *p_topic = NULL;

	int c = 0;
	opterr = 0;
	json j;

	while(1){
		static struct option long_options[] = {
			{"address", required_argument, 0, 'a'},
			{"topic", required_argument, 0, 't'},
			{"clientID", required_argument, 0, 'i'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long(argc,\
				argv,\
				"a:t:i:",\
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
			case 't':
				printf("topic : %s\n", optarg);
				p_topic = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
				memcpy(p_topic, optarg, strlen(optarg) + 1);
				break;
			case 'i':
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
	topic += p_topic;

	j["ID"] = "Windforces";

	j["PASSWORD"] = "success";
	payload = j.dump();
	const char *p_payload = payload.c_str();

	objMQTTClient *client = new objMQTTClient();
	client->createClient(address, clientID);
	client->setPersistenceConnection();
	client->setConnectionInterval(20);
	client->clientConnect();
	client->setPayload(payload.length(),\
			p_payload);
	client->publish(topic,\
			10000L);

	client->listen(topic.c_str(),\
			1);

	return 0;
}
