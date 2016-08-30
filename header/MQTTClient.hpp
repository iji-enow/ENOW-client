/*
Copyright [2016] [JeaSung Park]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <functional>
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>

#include <wiringPi.h>
#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include "KISA_SHA256.h"
}

#include <nlohmann/json.hpp>
#include <boost/locale.hpp>

using namespace std;
using json = nlohmann::json;

#define QOS 1
#define TIMEOUT 10000L

class objMQTTClient{
	private:
		MQTTClient m_client;
		MQTTClient_connectOptions m_option;
		MQTTClient_willOptions m_will;
		MQTTClient_SSLOptions m_ssl;

		string m_address;
		string m_topic;
		string m_clientID;
		static priority_queue<string> m_queue;

		bool m_clientCreated;
		bool m_clientConnected;
		bool m_topicSet;

		pthread_t m_thread;
		static pthread_mutex_t m_lock;
		pthread_attr_t m_attr;

	public:
		objMQTTClient()\
		: m_option(MQTTClient_connectOptions_initializer),\
	   m_will(MQTTClient_willOptions_initializer),\
	   m_ssl(MQTTClient_SSLOptions_initializer){
		   m_clientCreated = false;
		   m_clientConnected = false;
		   m_topicSet = false;
	   }
		~objMQTTClient(void);

		int createClient(string _address, string _clientID);
		void setTopic(string _topic);
		string getTopic(void);

		void setConnectOptions(const int _keepAliverInterval,\
				const int _cleansession,\
				const int _reliable,\
				const int _connectTimeout);
		bool setLWT(const char *_message = NULL,\
				const int _retained = 1,\
				const char _qos =  1);

		bool setPayload(MQTTClient_message &_message,\
				const int _payloadlen,\
				const char *_payload,\
				int _qos = 1,\
				int _retained = true,\
				int _dup = false);

		bool clientConnect(void);
		bool publish(MQTTClient_message &m_pubmsg,\
				unsigned long timeOut);

		static void delivered(void *_context,\
				MQTTClient_deliveryToken _token_d);
		static int messageArrived(void *_context,\
				char *_topicName,\
				int _topicLen,\
				MQTTClient_message *_message);
		static void connectionLost(void *_context,\
				char *_cause);

		void listen(int _qos = 1);
		static void *routine(void *_param);

};

#endif

