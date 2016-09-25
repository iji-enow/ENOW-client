import json
def eventHandler(event, context, callback):
	jsonString = json.dumps(event)
	print(jsonString)
	if event["present"] == "person":
		print("OK")
	else:
		print("None")