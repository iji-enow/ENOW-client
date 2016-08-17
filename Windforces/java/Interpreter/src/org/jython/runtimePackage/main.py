'''
Created on 2016. 8. 15.

@author: jeasungpark
'''

from org.jython.runtimePackage import runtime
from org.jython.runtimePackage import sourceControl

class main:
    def __init__(self, _source, _parameter, _payload):
        self.source = _source
        self.parameter = _parameter
        self.payload = _payload
        self.runtime = None
        self.sourceControl = None
        
    def controllSource(self):
        self.sourceControl = sourceControl(body=self.source)
        
    def run(self):
        self.runtime = runtime()
        self.runtime.run(_args=self.parameter,
                         _payloads = self.payload)
        

