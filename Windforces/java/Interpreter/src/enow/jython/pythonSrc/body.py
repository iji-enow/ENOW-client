def eventHandler(event, context, callback):
    event["identification"] = "modified"
    print(context["parameter"])