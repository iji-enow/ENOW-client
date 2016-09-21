MQTTClient
=========================

Todo List
---------
These are the features planned to implement

- [ ] ~~Manage construction and destruction of a client in client pool in a manner of [RAII](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)

- [ ] ~~Sending subscriptions to each program connected to the client(The way it treats subscriptions now is to broadcast to the single shared memory region).

- [ ] ~~Sending logs generated while the program has executed to the broker.

Prerequisite
==========

- Paho MQTT C [here](https://eclipse.org/paho/clients/c/)

  AFAIK, there are many open-source program handling __MQTT__ protocol out there. We chose to work with __PAHO MQTT C__ client. It supports many features such as __SSL/TLS__ connection, automatic reconnection, non-blocking api and so on.

- Boost C++ Libraries [here](http://www.boost.org/users/download/)

  When it comes to lower level of network communication, _encoding_ is the essence of handling data between client and server and the __Boost C++ Libraries__ supports a variety of functions converting some encoding to others. So we used this to solve the encoding issue.

- CMake [here]

How to use it
==========

  The client program is
