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

/*
========================================
	Class : objMQTTClientPool
========================================
	Description :
		  A class managing list of objMQTTClients
		 The class supports query function to retrieve objMQTTClient in the list
*/

#ifndef _MQTT_CLIENT_POOL_
#define _MQTT_CLIENT_POOL_

#include <utility>
#include <algorithm>
#include <atomic>

#include "MQTTClient.hpp"

class CompFunc {
	public:
		bool operator() (objMQTTClient *lhs, objMQTTClient *rhs) {
			if(lhs->getTopic() < rhs->getTopic())
				return true;
			else
				return false;
		}
};

class objMQTTClientPool {
private:
	static set<objMQTTClient *, CompFunc> m_clientPool;
	static bool m_initialized;
public:
	objMQTTClientPool(){
		m_initialized = false;
	}

	bool initialize(void);
	void insertClient(objMQTTClient *_client);
	objMQTTClient* findClient(string _topic);
};

#endif
