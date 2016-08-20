from org.jython.temp import BuildingType

class Building(BuildingType):
    def __init__(self):
        self.integer = None
    
    def set(self, _integer):
        self.integer = _integer
    
    def get(self):
        self.integer += 1
        return self.integer
    
    
            