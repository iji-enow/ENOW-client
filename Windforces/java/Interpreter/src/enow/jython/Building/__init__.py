from enow.jython.interfaces import BuildingType
from enow.jython.runtimePackage.runtimeMain import runtimeMain
import sys
'''
Class : Building
    Description : 
     Actual class implementing interface of BuildingType
'''
class Building(BuildingType):
    main = None
    result = None
    code = ""
    parameter = ""
    payload = ""
    
    def __init__(self):
        print("Initializing interface")
        
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
    
    def setcode(self, _code):
        self.code = _code
    
    def setParameter(self, _parameter):
        self.parameter = _parameter
        
    def setPayload(self, _payload):
        self.payload = _payload
    
    def run(self):
        '''
        Function : run
            Description : 
             A bridge connecting the actual part of implementation and the method shown outside
        '''
        if self.parameter is not None and self.code is not None and self.payload is not None:
            self.main = runtimeMain(_source=self.code,
                             _parameter=self.parameter,
                             _payload=self.payload)
            
            self.controlSource()
            self.result = self.main.run()
            return self.result
        else:
            print("At least one of the items are not set\n")
        
    
            