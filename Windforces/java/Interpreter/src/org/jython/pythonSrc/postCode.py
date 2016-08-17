import json
import paho.mqtt.client as mqtt

topicToRegister = ""

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topicToRegister)
    
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    
def on_disconnect(client, userdata, rc):
    print("Now Client is closing\n")

def postProcess(event, context, callback):
    
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    
    global topicToRegister
    topicToRegister = context["deviceID"] + "/" + context["topic"]
    jsonDumpCallback = json.dumps(callback)
    jsonDumpEvent = json.dumps(event)
    
    client.connect("192.168.2.2", 1883, 60)
    client.publish(topicToRegister + "/event", jsonDumpEvent, 1, True)
    client.publish(topicToRegister + "/callback", jsonDumpCallback, 1, True)
    client.disconnect()
    
    