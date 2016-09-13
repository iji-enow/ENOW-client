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
#include <sstream>
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <wiringPi.h>
#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include "KISA_SHA256.h"
}

#include "../json/json.hh"
#include <boost/locale.hpp>

using namespace std;
using namespace JSON;

#define QOS 1
#define TIMEOUT 10000L

class objMQTTClient{
	private:
		static MQTTClient m_client;
		static MQTTClient_connectOptions m_option;
		static MQTTClient_willOptions m_will;
		static MQTTClient_SSLOptions m_ssl;

		static string m_address;
		string m_topic;
		static string m_clientID;
		static priority_queue<string> m_queue;

		static bool m_clientCreated;
		static bool m_clientConnected;
		bool m_topicSet;
		bool m_listening;

	public:
		objMQTTClient()\
			: m_option(MQTTClient_connectOptions_initializer),\
			m_will(MQTTClient_willOptions_initializer),\
			m_ssl(MQTTClient_SSLOptions_initializer){
				m_clientCreated = false;
				m_clientConnected = false;
				m_topicSet = false;
				m_listening = false;
			}
		~objMQTTClient(void);

		static int createClient(string _address, string _clientID);
		void setTopic(string _topic);
		string getTopic(void);

		static void setConnectOptions(const int _keepAliverInterval,\
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
				int _dup = true);

		static bool clientConnect(void);
		bool publish(MQTTClient_message &m_pubmsg,\
				const string sub_topic,\
				unsigned long timeOut);

		static void delivered(void *_context,\
				MQTTClient_deliveryToken _token_d);
		static int messageArrived(void *_context,\
				char *_topicName,\
				int _topicLen,\
				MQTTClient_message *_message);
		static void connectionLost(void *_context,\
				char *_cause);

		bool listen(const string sub_topic,\
				int _qos = 1);
};

#endif

