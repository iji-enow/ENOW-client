/*Copyright [2016] [JeaSung Park]

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License
*/
#include "../header/MQTTClient.hpp"

priority_queue<string> objMQTTClient::m_queue;
mutex objMQTTClient::m_transfer_mutex;
MQTTClient objMQTTClient::m_client = NULL;
MQTTClient_connectOptions objMQTTClient::m_option = MQTTClient_connectOptions_initializer;
MQTTClient_willOptions objMQTTClient::m_will = MQTTClient_willOptions_initializer;
MQTTClient_SSLOptions objMQTTClient::m_ssl = MQTTClient_SSLOptions_initializer;
string objMQTTClient::m_address;
string objMQTTClient::m_clientID;
bool objMQTTClient::m_clientCreated = false;
bool objMQTTClient::m_clientConnected = false;
bool objMQTTClient::m_onTransfer;
void *objMQTTClient::m_sharedMemoryRegion = NULL;
int objMQTTClient::m_sharedMemoryID = 0;
char *objMQTTClient::m_currentAddress = NULL;
key_t objMQTTClient::m_sharedKey = (key_t)0;

static char fromDecToHex(char input){
	if(input >= 0 && input <= 9)
		return input + 48;
	else
		return input + 87;
}

static std::string fromLocale(const std::string &localeString){
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);
	std::locale locale = generator(boost::locale::util::get_system_locale());
	return boost::locale::conv::to_utf<char>(localeString, locale);
}

/* int createClient(void)
 *
 * for creating client object
 * if succeed, the function returns 0
 * if not, it returns an error code 
 */

int objMQTTClient::createClient(string _address, string _clientID){
	int result = 0;

	m_address = _address;
	m_clientID = _clientID;

	cout << m_address << endl;
	cout << m_clientID << endl;
	if((result = MQTTClient_create(\
					&m_client,\
					m_address.c_str(),\
					m_clientID.c_str(),\
					MQTTCLIENT_PERSISTENCE_NONE,\
					NULL)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Connection Failed with Exitcode %i\n", result);
	}

	//wiringPiSetup();

	m_clientCreated = true;
	return result;
}

void objMQTTClient::setTopic(string _topic){
	m_topic = _topic;
	m_topicSet = true;
}

string objMQTTClient::getTopic(void){
	return m_topic;
}

/* void setConnectOptions(const MQTTClient_connectOptions *_conn_opts);
 *
 * setter method for connection options
 * DETAILS:
 * 	This setter method is only for setting variables.
 * 	You should've set constant fields when creating this object.
 */

void objMQTTClient::setConnectOptions(const int _keepAliverInterval,\
		const int _cleansession,\ 
		const int _reliable,\
		const int _connectTimeout){
	m_option.keepAliveInterval = _keepAliverInterval;
	m_option.cleansession = _cleansession;
	m_option.reliable = _reliable;
	m_option.connectTimeout = _connectTimeout;
	/*m_option.ssl = &m_ssl;
	  m_option.will = &m_will;*/
}

void objMQTTClient::setSSLOptions(const char *_trustStore,\
		const char *_keyStore,\
		const char *_privateKey){

	m_ssl.trustStore = _trustStore;
	m_ssl.keyStore = _keyStore;
	m_ssl.privateKey = _privateKey;

	m_option.ssl = &m_ssl;
}

/*
 * for setting a structure of the last will statement
 * RETURNED VALUE:
 * 	If succeed, the function returns true.
 * 	Else, it returns false.
 * DETAILS:
 * 	_topicName is a string of characters representing the topic itself published on the broker
 * 	the object checks whether the topic is published or not.
 * 	If topic existed, the object should set the last will statement properly.
 * 	_qos is set 1 for reliable connection. 	
 */

bool objMQTTClient::setLWT(const char *_message,\
		const int _retained,\
		const char _qos) {
	if(!m_topicSet){
		fprintf(stderr, "A topic related to this client is not set\n");
		return false;
	}
	m_will.topicName = (const char *)m_topic.c_str();
	m_will.message = (const char *)_message;
	m_will.retained = _retained;
	m_will.qos = _qos;

	m_option.will = &m_will;
	return true;
}

/* for setting payload of the message which is going to be published soon.
 * RETURNED VALUE:
 * 	If succeed, the function returns true.
 * 	Else, it returns false.
 * DETAILS:
 * The program first checks whether the given parameter, _payloadlen, matches the length of the content in _payload.
 * If both of them didn't match, the function would print error message notifying both of them don't match.
 * _retained is set true for maintaining the content on the broker.
 * _dup is set false, meaning the message is not something duplicated.
 */

bool objMQTTClient::setPayload(MQTTClient_message &m_pubmsg,\
		const int _payloadlen,\
		const char *_payload,\
		int _qos,\
		int _retained,\
		int _dup){

	if(_payloadlen == 0 ||\
			_payload == NULL){
		fprintf(stderr, "Neither payload length nor payload are empty\n");
		return false;
	}

	m_pubmsg = MQTTClient_message_initializer;

	if(strlen(_payload) != (size_t)_payloadlen)
		fprintf(stderr, "Length of the content in _payload(parameter) and _payloadlen(parameter) doesn't match. Be advised!\n");

	m_pubmsg.payloadlen = _payloadlen;
	m_pubmsg.payload = (void *)_payload;
	m_pubmsg.qos = _qos;
	m_pubmsg.retained = _retained;
	m_pubmsg.dup = _dup;

	return true;
}

/*
 * for connecting to the broker
 */

bool objMQTTClient::clientConnect(void){
	int result = 0;

	if(!m_clientCreated){
		fprintf(stderr, "Client is not created.\n");
		return false;
	}
	if(m_clientConnected){
		fprintf(stderr, "Client is already connected\n");
		return false;
	}

	if((result = MQTTClient_setCallbacks(m_client,\
					NULL,\
					&objMQTTClient::connectionLost,\
					&objMQTTClient::messageArrived,\
					&objMQTTClient::delivered)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Failed to set callbacks for subscribing messages.\n");
		return result;
	}

	if((result = MQTTClient_connect(m_client,\
					&m_option)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Failed to connect, return code %d\n", result);
		exit(1);
	}

	m_clientConnected = true;

	return result;
}

/*
 * for transferring the message to the broker
 * 	RETURNED VALUE:
 * 	If the client succeed to deliver message to the broker, the function returns true.
 * 	Else, it returns false.
 * 	DETAILS:
 * 	In the beginning, the function checks whether the client object, message to be published and connection status is set. Unless all of them were set properly, the function should not publish the message to the broker. 
 * 	After all of them are set, the function then publish the message to the broker and waits for response. If the response is successfully delivered to the client, it returns the status of it. 
 */

bool objMQTTClient::publish(MQTTClient_message &m_pubmsg,\
		const string sub_topic,\
		unsigned long timeOut){
	int result = 0;
	int result_t = 0;
	MQTTClient_deliveryToken m_token;
	string topic_SHA256_utf8 = fromLocale(m_topic + sub_topic);

	/*makeTopic(m_topic,\
	  sub_topic,\
	  topic_SHA256_utf8);
	 */

	if(!m_topicSet){
		fprintf(stderr, "A topic related to this client is not set\n");
		return false;	
	}
	if(!m_clientCreated){
		fprintf(stderr, "Client is not created.\n");
		return false;
	}
	if(!m_clientConnected){
		fprintf(stderr, "Client is not connected to the broker\n");
		return false;
	}

	if((result = MQTTClient_publishMessage(m_client,\
					topic_SHA256_utf8.c_str(),\
					&m_pubmsg,\
					&m_token)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Something went wrong while transferring the message.\n");
		return result;
	}

	result_t = MQTTClient_waitForCompletion(m_client,\
			m_token,\
			timeOut);

	printf("Message with delivery token %d delivered\n", m_token);

	return result_t;	
}

/*
 * for creating a thread listening messages from the broker
 * DETAILS:
 * The function gets both topic and level of qos.
 * At first, it sets the attribute of a thread detachable.
 * Next, it creates a thread for listening the messages.
 * At last, it doesn't wait for the thread to end.
 */

bool objMQTTClient::listen(const string sub_topic,\
		int _qos){
	if(m_listening)
		return false;
	if(!m_topicSet){
		fprintf(stderr, "A topic related to this client is not set\n");
		return false;
	}

	int result = 0;
	string w_topic_str = fromLocale(m_topic + sub_topic);

	/*makeTopic(m_topic,\
	  sub_topic,\
	  topic_SHA256_utf8);
	 */

	cout << "Listening topic" << endl;
	cout << "\tTOPIC : " << m_topic << sub_topic << endl;
	cout << "\tQOS : " << _qos << endl;

	if((result = MQTTClient_subscribe(m_client,\
					w_topic_str.c_str(),\
					_qos)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "A procedure listening topic was not created with exitcode %d\n", result);
	}

	m_listening = true;
	return true;
}

bool objMQTTClient::listenMany(char *const* _topic,\
		int _size,\
		int _qos){
	int *qos_list = (int *)malloc(sizeof(int) * _size);

	if(m_listening)
		return false;
	if(!m_topicSet){
		fprintf(stderr, "A topic related to this client is not set\n");
		return false;
	}

	int result = 0,\
				 index = 0;

	for(int i = 0;i < _size;i++) {
		qos_list[index] = _qos;

		char single_topic_backup[BUFSIZ];
		strcpy(single_topic_backup, _topic[index]);
		strcpy(_topic[index], m_topic.c_str());
		strcat(_topic[index], single_topic_backup);

		cout << "Listening topic" << endl;
		cout << "\tTOPIC : " << string(_topic[index]) << endl;
		cout << "\tQOS : " << _qos << endl;

		index++;
	}

	/*makeTopic(m_topic,\
	  sub_topic,\
	  topic_SHA256_utf8);
	 */
	if((result = MQTTClient_subscribeMany(m_client,\
					_size,\
					_topic,\
					qos_list)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "A procedure listening topic was not created with exitcode %d\n", result);
	}

	m_listening = true;
	return true;
}

void objMQTTClient::makeTopic(const string &major_topic,\
		const string &minor_topic,
		string &result){
	char p_major_topic[BUFSIZ],\
		p_minor_topic[BUFSIZ],\
		p_result[BUFSIZ],\
		p_current[BUFSIZ];
	char *token = NULL;
	char upper_bit_mask = 0b11110000;
	char lower_bit_mask = 0b00001111;

	int w_index = 0,\
				  index = 0;

	strcpy(p_major_topic, major_topic.c_str());
	strcpy(p_minor_topic, minor_topic.c_str());
	strcat(p_major_topic, p_minor_topic);
	cout << "TOPIC to hash" << endl;
	cout << "	" << string(p_major_topic) << endl;
	memset(p_result, 0, BUFSIZ);
	p_result[w_index++] = '/';

	token = strtok(p_major_topic, "/");
	while(token != NULL){
		memset(p_current, 0, BUFSIZ);
		index = 0;

		SHA256_Encrpyt((const BYTE *)token,\
				(UINT)strlen(token),\
				(BYTE *)p_current);
		while(p_current[index] != 0) {
			char upper = p_current[index] & upper_bit_mask;
			char lower = p_current[index] & lower_bit_mask;
			upper = upper >> 4;

			p_result[w_index++] = fromDecToHex(upper);
			p_result[w_index++] = fromDecToHex(lower);

			index++;
		}
		p_result[w_index++] = '/';
		token = strtok(NULL, "/");
	}

	cout << "TOPIC hashed" << endl;
	cout << "	" << string(p_result) << endl;

	result = fromLocale(string(p_result));
}

/*
 * for checking whether the message is properly delivered.
 */
void objMQTTClient::delivered(void *_context,\
		MQTTClient_deliveryToken _token_d){
	printf("Message with token value %d delivery confirmed.\n", _token_d);
}

int objMQTTClient::messageArrived(void *_context,\
		char *_topicName,\
		int _topicLen,\
		MQTTClient_message *_message){
	char *p_payload = NULL,\
					  *p_topic_prev = NULL,\
					  *p_topic_prev_prev = NULL,\
					  *p_topic_w = NULL;

	int code = 0;
	string topic_str,\
		request_str_SHA256_utf8,\
		response_str_SHA256_utf8,\
		feedback_str_SHA256_utf8,\
		message;

	/*
	 * Confirming the contents of a message
	 */

	printf("Message arrived.\n");
	printf("	topic : %s\n", _topicName);
	p_payload = (char *)_message->payload;
	p_payload[_message->payloadlen] = 0;
	message += p_payload;

	/*
	 * Verify the topic of a message
	 */
	p_topic_w = (char *)malloc(sizeof(char) * (strlen(_topicName) + 2));
	strcpy(p_topic_w, _topicName);
	p_topic_prev = strtok(p_topic_w, "/");
	while(p_topic_prev != NULL) {
		p_topic_prev_prev = p_topic_prev;
		p_topic_prev = strtok(NULL, "/");
	}

	/*
	 * checks whether it is the status topic
	 */

	if( strcmp(p_topic_prev_prev, "request") == 0 ){

		MQTTClient_message m_pubmsg = MQTTClient_message_initializer;
		MQTTClient_deliveryToken m_token;

		/*
		   make a message to return
		*/

		m_pubmsg.payload = (void *)p_payload;
		m_pubmsg.payloadlen = strlen(p_payload);
		m_pubmsg.qos = 1;
			
		/*
		 * create a topic name for response
		 */

		strcpy(p_topic_w, _topicName);
		strcpy(&(p_topic_w[strlen(_topicName) - 7]), "response");
		p_topic_w[strlen(_topicName) + 1] = 0;
		response_str_SHA256_utf8 = fromLocale(string(p_topic_w));

		/*
		 *  publish the message to the response topic
		 * and then, wait.
		 */

		printf("Responding to status topic\n");
		if((code = MQTTClient_publishMessage(m_client,\
						response_str_SHA256_utf8.c_str(),\
						&m_pubmsg,\
						&m_token)) != MQTTCLIENT_SUCCESS){
			fprintf(stderr, "Error while updating the topic \"status\" with error %d\n", code);
		}
		code = MQTTClient_waitForCompletion(m_client,\
				m_token,\
				TIMEOUT);
		printf("Message with delivery token %d delivered\n", m_token);
	}
	else if( strcmp(p_topic_prev_prev, "feed") == 0 ) {
		printf("Acquired feedback\n");
		m_transfer_mutex.lock();
		if(m_onTransfer){
			m_transfer_mutex.unlock();
		}else {
			m_onTransfer = true;
			m_transfer_mutex.unlock();

			m_currentAddress = (char *)m_sharedMemoryRegion;
			m_currentAddress++;

			memcpy(m_currentAddress, p_payload, strlen(p_payload) + 1);

			m_currentAddress = (char *)m_sharedMemoryRegion;
			*m_currentAddress = '?';

			while(1){
				if(*m_currentAddress == '!')
					break;
				sleep(1);
			}

			m_transfer_mutex.lock();
			m_onTransfer = false;
			m_transfer_mutex.unlock();
		}
		printf("Feedback processing completed\n");		
	}

	MQTTClient_freeMessage(&_message);
	MQTTClient_free(_topicName);
	free(p_topic_w);
	return 1;
}

/*
 * for checking the connection lost
 */

void objMQTTClient::connectionLost( void *_context,\
		char *_cause ){
	fprintf(stderr, "\nConnection lost\n");
	fprintf(stderr, "	cause : %s\n", _cause);
}

/*
 * for destructing the object
 */

void objMQTTClient::disconnect(void){
	if(m_clientConnected)
		MQTTClient_disconnect(m_client, 10000);
	if(m_clientCreated)
		MQTTClient_destroy(&m_client);
}

objMQTTClient::~objMQTTClient(void){

}
