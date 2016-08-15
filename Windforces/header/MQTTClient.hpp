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
}

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#define QOS 1
#define TIMEOUT 10000L

class objMQTTClient{
	private:
		static MQTTClient m_client;
		MQTTClient_connectOptions m_conn_opts;
		MQTTClient_message m_pubmsg;

		static MQTTClient_deliveryToken m_token;
		volatile MQTTClient_deliveryToken m_token_d;

		string m_address;
		string m_clientID;
		set<string> m_TOPIC;
		static priority_queue<string> m_queue;

		bool m_pubmsgSet;
		bool m_clientCreated;
		bool m_clientConnected;

		pthread_t m_thread;
		pthread_mutex_t m_lock;
		pthread_attr_t m_attr;

	public:
		objMQTTClient() : m_conn_opts(MQTTClient_connectOptions_initializer), m_pubmsg(MQTTClient_message_initializer){
			m_pubmsgSet = false;
			m_clientCreated = false;
			m_clientConnected = false;
		}
		~objMQTTClient(void);

		int createClient(string _address, string _clientID);

		void setConnectOptions(const MQTTClient_connectOptions &_conn_opts);
		MQTTClient_connectOptions getConnectOptions(void);

		void setPersistenceConnection(void);
		void setConnectionInterval(const int _interval);
		bool setLWT(void);

		bool setPayload(const int _payloadlen,\
				const char *_payload,\
				int _qos = 1,\
				int _retained = true,\
				int _dup = false);

		bool clientConnect(void);
		bool publish(string _TOPIC,\
				unsigned long timeOut);

		static void delivered(void *_context,\
				MQTTClient_deliveryToken _token_d);
		static int messageArrived(void *_context,\
				char *_topicName,\
				int _topicLen,\
				MQTTClient_message *_message);
		static void connectionLost(void *_context,\
				char *_cause);

		void listen(const char *_topic,\
				int qos = 1);
		static void *routine(void *_param);

};

#endif
