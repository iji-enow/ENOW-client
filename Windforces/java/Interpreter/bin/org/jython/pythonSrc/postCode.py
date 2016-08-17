import json
import sys
from json.decoder import JSONObject

'''
A function wraps up the whole process
    Description : 
        Dumps event, context and callback parameters to a json object and stringify!
'''

def postProcess(event, context, callback):
    jsonEventDump = json.dumps(event)
    jsonContextDump = json.dumps(context)
    jsonCallbackDump = json.dumps(callback)
    
    jsonPayload = {
                   "event" : jsonEventDump,
                   "context" : jsonContextDump,
                   "callback" : jsonCallbackDump}
    
    jsonPayloadDump = json.dumps(jsonPayload)
    
    sys.stdout.writelines(jsonPayloadDump)
    