import RPi.GPIO as GPIO
import dht11
import time
import datetime
import json
import sysv_ipc

# initialize GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.cleanup()

# read data using pin 14
instance = dht11.DHT11(pin=17)
_key = 9012
_mode = 0600
_size = 8192
sharedMemory = sysv_ipc.SharedMemory(key = _key, mode = _mode, size = _size)

while True:
    result = instance.read()
    if result.is_valid():
		l_result_dict = { "temparature" : str(result.temperature), "humidity" : str(result.humidity) }
		l_result_json_str = json.dumps(l_result_dict)
		l_result_json_str_byte = l_result_json_str.decode("utf-8").encode("ascii")

		sharedMemory.write(l_result_json_str_byte, offset = 1)
		sharedMemory.write("?", offset = 0)
		print("Last valid input: " + str(datetime.datetime.now()))
		print("Temperature: %d C" % result.temperature)
		print("Humidity: %d %%" % result.humidity)

		while True:
			l_read_byte = sharedMemory.read(byte_count = 1, offset = 0)
			if l_read_byte == b"!":
				break


    time.sleep(1)
