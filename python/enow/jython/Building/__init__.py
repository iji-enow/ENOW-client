import sys
import time
'''
Class : Building
    Description :
     Actual class implementing interface of BuildingType
'''
from enow.jython.runtimePackage.runtimeMain import runtimeMain


class Building():
    main = None
    result = None
    code = None
    parameter = None
    payload = None
    previousData = None

    def __init__(self):
        time.sleep(0.000001)
#        print("Initializing interface")

    def getcode(self):
        if self.code is not None:
            return self.code
        else:
            return ""

    def getParameter(self):
        if self.parameter is not None:
            return self.parameter
        else:
            return ""

    def getPayload(self):
        if self.payload is not None:
            return self.payload
        else:
            return ""

    def getPreviousData(self):
        if self.previousData is not None:
            return self.previousData
        else:
            return ""

    def setcode(self, _code):
        self.code = _code

    def setParameter(self, _parameter):
        self.parameter = _parameter

    def setPayload(self, _payload):
        self.payload = _payload

    def setPreviousData(self, _previousData):
        self.previousData = _previousData

    def run(self):
        '''
        Function : run
            Description :
             A bridge connecting the actual part of implementation and the method shown outside
        '''
        if self.parameter is not None and self.code is not None and self.payload is not None:
            #             print("Running Jython program")
            self.main = runtimeMain(_source=self.code,
                                    _parameter=self.parameter,
                                    _payload=self.payload,
                                    _previousData = self.previousData)
            self.main.controllSource()
            self.result = self.main.run()
            return self.result
#        else:
#             print("At least one of the items are not set\n")
