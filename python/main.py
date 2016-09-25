from executingBolt import ExecutingBolt
import json
def Main():
    bolt = ExecutingBolt()
    t_json_string = '''{
    "topic":"enow/serverId1/brokerId1/deviceId1",
    "roadMapId":"2",
    "mapId":"1",
    "incomingNode":["2", "4"],
    "outingNode":["11", "13"],
    "previousData":{"2" : "value1", "4" : "value2"},
    "payload": {
        "present" : "person"
    },
    "lastNode":false,
    "order":false,
    "verified":false,
    "ack" : true
    }'''
    t_element_json = json.loads(t_json_string)
    t_parameter_tuple = (t_element_json)
    t_result_json = bolt.process(t_parameter_tuple)
    print(json.dumps(t_result_json))
if __name__ == "__main__":
    Main()
