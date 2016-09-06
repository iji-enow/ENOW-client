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

#include "../header/MQTTClientPool.hpp"

set<objMQTTClient *, CompFunc> objMQTTClientPool::m_clientPool;

static bool isTopicEqual(objMQTTClient *lhs, objMQTTClient *rhs){
	return (lhs->getTopic() == rhs->getTopic());
}

void objMQTTClientPool::insertClient(objMQTTClient *_client) {
	pair<set<objMQTTClient *, CompFunc>::iterator, bool> code;

	code = m_clientPool.insert(_client);
	if(code.second == false){
		fprintf(stderr, "The element already has existed\n");
		return;
	}
}

objMQTTClient* objMQTTClientPool::findClient(string _topic) {
	set<objMQTTClient *, CompFunc>::iterator iterator;

	objMQTTClient *dummy = new objMQTTClient();
	dummy->setTopic(_topic);

	iterator = search_n(m_clientPool.begin(),\
			m_clientPool.end(),\
			1,\
			dummy,\
			isTopicEqual);
	if(iterator != m_clientPool.end())
		printf("Found one.\n");
	else{
		fprintf(stderr, "No such client that has the topic\n");
		delete dummy;
		return NULL;
	}
	delete dummy;
	return *iterator;
}




