from org.jython.interfaces import BuildingType
from org.jython.runtimePackage import main

'''
Class : Building
    Description : 
     Actual class implementing interface of BuildingType
'''

class Building(BuildingType):
    def __init__(self):
        self.source = None
        self.parameter = None
        self.payload = None
        self.main = None
    
    def getSource(self):
        if self.source is not None:
            return self.source
    
    def getParameter(self):
        if self.parameter is not None:
            return self.parameter
    
    def getPayload(self):
        if self.payload is not None:
            return self.payload
    
    def setSource(self, _source):
        self.source = str(_source)
    
    def setParameter(self, _parameter):
        self.parameter = str(_parameter)
        
    def setPayload(self, _payload):
        self.payload = str(_payload) 
    
    def run(self):
        '''
        Function : run
            Description : 
             A bridge connecting the actual part of implementation and the method shown outside
        '''
        if self.parameter is not None and self.payload is not None and self.payload is not None:
            self.main = main(_source=self.source,
                             _parameter=self.parameter,
                             _payload=self.payload)
            self.controlSource()
            self.main.run()
        else:
            print("At least one of the items are not set\n")
        
    
            