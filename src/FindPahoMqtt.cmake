set(PC_LIBMQTT_LIBDIR /usr/local/lib)
set(PC_LIBMQTT_INCLUDEDIR /usr/local/include)

find_library(PAHO_MQTT_C_LIBRARIES NAMES libpaho-mqtt3cs paho-mqtt3cs
	HINTS ${PC_LIBMQTT_LIBDIR})

find_path(MQTT_CLIENT_INCLUDE_DIRS MQTTClient.h MQTTClientPersistence.h
	HINTS ${PC_LIBMQTT_INCLUDEDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(paho_mqtt DEFAULT_MSG
	PAHO_MQTT_C_LIBRARIES MQTT_CLIENT_INCLUDE_DIRS)

set(LIBMQTT_LIBRARIES ${PAHO_MQTT_C_LIBRARIES})
set(LIBMQTT_INCLUDE_DIRS ${MQTT_CLIENT_INCLUDE_DIRS})
