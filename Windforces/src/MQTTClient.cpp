#include "../header/MQTTClient.hpp"

#define ON 1
#define OFF 0

typedef struct param{
	char *topic;
	int qos;
};

MQTTClient objMQTTClient::m_client;
MQTTClient_deliveryToken objMQTTClient::m_token;
priority_queue<string> objMQTTClient::m_queue;


#ifndef MQTTClient_willOptions_initializer
	#define MQTTClient_willOptions_initializer {{'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 1, 1}
#endif

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

	wiringPiSetup();

	m_clientCreated = true;
	return result;
}

/* void setConnectOptions(const MQTTClient_connectOptions *_conn_opts);
 *
 * setter method for connection options
 * DETAILS:
 * 	This setter method is only for setting variables.
 * 	You should've set constant fields when creating this object.
 */

void objMQTTClient::setConnectOptions(const MQTTClient_connectOptions &_conn_opts){
	free(&m_conn_opts);
	m_conn_opts.keepAliveInterval = _conn_opts.keepAliveInterval;
	m_conn_opts.cleansession = _conn_opts.cleansession;
	m_conn_opts.reliable = _conn_opts.reliable;
	m_conn_opts.will = _conn_opts.will;
	m_conn_opts.connectTimeout = _conn_opts.connectTimeout;
	m_conn_opts.retryInterval = _conn_opts.retryInterval;
	m_conn_opts.ssl = _conn_opts.ssl;
	m_conn_opts.serverURIcount = _conn_opts.serverURIcount;
	m_conn_opts.MQTTVersion = _conn_opts.MQTTVersion;
}

/*
 * getter method for connection options
 * devs need to verify the connection options on their own
 */

MQTTClient_connectOptions objMQTTClient::getConnectOptions(void){
	return m_conn_opts;
}

/* 
 * for setting continuous connection between broker and client
 */

void objMQTTClient::setPersistenceConnection(void){
	m_conn_opts.cleansession = 0;
}

/*
 * for setting period of the connection at the time
 */

void objMQTTClient::setConnectionInterval(const int _interval){
	m_conn_opts.keepAliveInterval = _interval;
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
 * PROGRAMMING GUIDE:
 * 	Before you use this function, you should have declared a macro named,
 * 		MQTTClient_willOptions_initializer
 * 	with 
 * 		{{'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 1, 1}
 * 	in its content. That means you should write something like below
 * 		#define MQTTClient_willOptions_initializer {{'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 1, 1}
 * 	The first field containing NULL pointer should points a character array describing a topic name you want to leave a LWT and the second also points to a character array showing a message of the LWT to the broker.
 *
 * 	
 */

bool objMQTTClient::setLWT(void){
	MQTTClient_willOptions current = MQTTClient_willOptions_initializer;

	m_conn_opts.will = &current;

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

bool objMQTTClient::setPayload(const int _payloadlen,\
	   	const char *_payload,\
	   	int _qos,\
	   	int _retained,\
	   	int _dup){

	if(_payloadlen == 0 ||\
			_payload == NULL){
		fprintf(stderr, "Neither payload length nor payload are empty\n");
		return false;
	}

	sprintf(m_pubmsg.struct_id, "MQTM");
	m_pubmsg.struct_version = 0;
	if(strlen(_payload) != (size_t)_payloadlen)
		fprintf(stderr, "Length of the content in _payload(parameter) and _payloadlen(parameter) doesn't match. Be advised!\n");

	m_pubmsg.payloadlen = _payloadlen;
	m_pubmsg.payload = (void *)_payload;
	m_pubmsg.qos = _qos;
	m_pubmsg.retained = _retained;
	m_pubmsg.dup = _dup;

	m_pubmsgSet = true;

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

	if((result = MQTTClient_setCallbacks(m_client,\
					NULL,\
					objMQTTClient::connectionLost,\
					objMQTTClient::messageArrived,\
					objMQTTClient::delivered)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Failed to set callbacks for subscribing messages.\n");
		return result;
	}

	if((result = MQTTClient_connect(m_client,\
					&m_conn_opts)) != MQTTCLIENT_SUCCESS){
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

bool objMQTTClient::publish(string _TOPIC,\
		unsigned long timeOut){
	int result = 0;
	int result_t = 0;

	if(!m_clientCreated){
		fprintf(stderr, "Client is not created.\n");
		return false;
	}
	if(!m_pubmsgSet){
		fprintf(stderr, "Message to be published is not set\n");
		return false;		
	}
	if(!m_clientConnected){
		fprintf(stderr, "Client is not connected to the broker\n");
		return false;
	}

	if((result = MQTTClient_publishMessage(m_client,\
					_TOPIC.c_str(),\
					&m_pubmsg,\
					&m_token)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "Something went wrong while transferring the message.\n");
		return result;
	}

	result_t = MQTTClient_waitForCompletion(m_client,\
			m_token,\
			timeOut);

	printf("Message with delivery token %d delivered\n", m_token);

	m_TOPIC.insert(_TOPIC);
	m_pubmsgSet = false;

	return result_t;	
}

/*
 * routine for subscribing messages
 */

void *objMQTTClient::routine(void *_param){
	param *p_param = (param *)_param;
	char ch;
	int result;
	printf("Listening topic\n\tTOPIC : %s\n\tQOS : %d\n", p_param->topic, p_param->qos);
	if((result = MQTTClient_subscribe(m_client,\
				p_param->topic,\
				p_param->qos)) != MQTTCLIENT_SUCCESS){
		fprintf(stderr, "A procedure listening topic was not created with exitcode %d\n", result);
		pthread_exit(NULL);
	}
	do{
		ch = getchar();
	}while(ch != 'Q' &&\
			ch != 'q');
	pthread_exit(NULL);
}

/*
 * for creating a thread listening messages from the broker
 * DETAILS:
 * The function gets both topic and level of qos.
 * At first, it sets the attribute of a thread detachable.
 * Next, it creates a thread for listening the messages.
 * At last, it doesn't wait for the thread to end.
 */

void objMQTTClient::listen(const char *_topic,\
		int _qos){
	param *_param = (param *)malloc(sizeof(param));
	_param->topic = (char *)malloc(strlen(_topic) + 1);
	strcpy(_param->topic, _topic);
	_param->qos = _qos;

	pthread_attr_init(&m_attr);
	if(pthread_attr_setdetachstate(&m_attr,\
				PTHREAD_CREATE_JOINABLE) != 0){
		fprintf(stderr, "A thread listening the topics is not detachable\n");
		exit(1);
	}

	if(pthread_create(&m_thread,\
				&m_attr,\
				objMQTTClient::routine,\
				_param) != 0){
		fprintf(stderr, "Thread is not created\n");
		exit(1);
	}

	pthread_join(m_thread, NULL);

	pthread_attr_destroy(&m_attr);
}

/*
 * for checking whether the message is properly delivered.
 */

void objMQTTClient::delivered(void *_context,\
		MQTTClient_deliveryToken _token_d){
	printf("Message with token value %d delivery confirmed.\n", _token_d);
	m_token = _token_d;
}

/*
 * for queueing several message
 */ 

static int ledControl(int gpio, bool flag){
	int i;

	pinMode(gpio, OUTPUT);

	/*for(i = 0;i < 5;i++){
		digitalWrite(gpio, HIGH);
		delay(1000);
		digitalWrite(gpio, LOW);
		delay(1000);
	}
	*/

	if(flag == ON)
		digitalWrite(gpio, HIGH);
	else if(flag == OFF)
		digitalWrite(gpio, LOW);

	return 0;
}

int objMQTTClient::messageArrived(void *_context,\
		char *_topicName,\
		int _topicLen,\
		MQTTClient_message *_message){
	char *p_payload;

	string message;
	printf("Message arrived.\n");
	printf("	topic : %s\n", _topicName);

	p_payload = (char *)_message->payload;
	for(int i = 0;i < _message->payloadlen;i++){
		message.push_back(*p_payload++);
	}

	m_queue.push(message);

	json jsonObject = json::parse(message);
	
	for(json::iterator it = jsonObject.begin();it != jsonObject.end();it++){
		string value = it.value().get<std::string>();

		cout << value << endl;
		if(value.compare("ON") == 0 ||\
				value.compare("on") == 0){
			ledControl(1, ON);
		}
		else if(value.compare("OFF") == 0 ||\
				value.compare("off") == 0){
			ledControl(1, OFF);
		}

		//cout << it.key() << " : " << it.value() << endl;
	}

	MQTTClient_freeMessage(&_message);
	MQTTClient_free(_topicName);
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

objMQTTClient::~objMQTTClient(void){
	free(m_conn_opts.will);
	MQTTClient_disconnect(m_client, 10000);
	MQTTClient_destroy(&m_client);
}


