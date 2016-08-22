import something

def eventHandler(event, context, callback):
	event["something"] = "value"
	print("contents")
	raise Exception
	callback["returned"] = "value"