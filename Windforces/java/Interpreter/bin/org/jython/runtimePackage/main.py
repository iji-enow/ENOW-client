'''
Created on 2016. 8. 15.

@author: jeasungpark
'''

from org.jython.runtimePackage import runtime
from org.jython.runtimePackage import sourceControl

'''
Class : main
    Description : 
    Actual class controlling a pipeline for the whole program.
'''

class main:
    def __init__(self, _source, _parameter, _payload):
        self.source = _source
        self.parameter = _parameter
        self.payload = _payload
        self.runtime = None
        self.sourceControl = None
    '''
    Function : controllSource()
        Description : 
         Instantiate a sourceControl object and assign it on the sourceControl member variable.
    '''
    def controllSource(self):
        self.sourceControl = sourceControl(body=self.source)
    '''
    Function : run()
        Description : 
         Instantiate a runtime object and run the source code received.
    '''    
    def run(self):
        self.runtime = runtime()
        self.runtime.run(_args=self.parameter,
                         _payloads = self.payload)
        

