{
  "roadMapId" : "2",
  "mapIds" : {
          "1" : {
              "parameter" : [
                  "-l"
              ],
              "code" : "import json\ndef eventHandler(event, context, callback):\n\tjsonString = json.dumps(event)\n\tprint(jsonString)\n\tif event[\"present\"] == \"person\":\n\t\tprint(\"OK\")\n\telse:\n\t\tprint(\"None\")",
              "deviceId" : "deviceId1",
              "serverId" : "serverId1",
              "brokerId" : "brokerId1"
          }
    }
}
