import os
'''
Class : sourceControl
    Description : 
    A class processing file IO on the source received.
'''
class sourceControl:
    preCode = None
    body = None
    postCode = None
    '''
    Function : __init__(...)
        Description : 
        A function that initializes the class
        Parameters : 
            preCode : A source executed prior to the body source
            body : A source executed in the middle. User input from the dashboard is required.
            postCode : A source that wraps up the whole process.
            
        BE ADVISED : 
        Unless there is a reason to modify both the preCode and postCode,
        do not put anything on both the preCode and postCode parameter.
    '''
    def __init__(self, preCode = None, body, postCode = None):
        self.preCode = preCode
        self.body = body
        self.postCode = postCode
        
        self.storeSources()
    '''
    Function : storeSources()
        Descriptions :
        A function that stores the sources received.
    '''  
    def storeSources(self):
        if not self.preCode or not self.body or self.postCode:
            print("At least one of the source is empty\n")
            
        CURRENT_DIR = os.path.abspath(os.path.dirname(__file__))
        
        pathPreCode = os.path.join(CURRENT_DIR + '..' + 'pythonSrc', 'preCode.py')
        pathbodyCode = os.path.join(CURRENT_DIR + '..' + 'pythonSrc', 'body.py')
        pathPostCode = os.path.join(CURRENT_DIR + '..' + 'pythonSrc', 'postCode.py')
        fPreCode = open(str(pathPreCode), "r+")
        fBodyCode = open(str(pathbodyCode), "r+")
        fPostCode = open(str(pathPostCode), "r+")
        
        fPreCode.seek(0)
        fBodyCode.seek(0)
        fPostCode.seek(0)
        
        if self.preCode:
            fPreCode.writelines(self.preCode)
        fBodyCode.writelines(self.body)
        if self.postCode:
            fPostCode.writelines(self.postCode)
        
        fPreCode.close()
        fBodyCode.close()
        fPostCode.close()
        
        