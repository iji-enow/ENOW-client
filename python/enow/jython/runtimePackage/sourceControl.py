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
    def __init__(self, preCode = None, body = None, postCode = None):
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
        '''
        if not self.preCode or not self.body or self.postCode:
            print("At least one of the source is empty\n")
        '''

        fileDir = os.path.dirname(os.path.realpath('__file__'))

        pathPreCode = os.path.join(fileDir, 'enow/jython/pythonSrc/preCode.py')
        pathbodyCode = os.path.join(fileDir, 'enow/jython/pythonSrc/body.py')
        pathPostCode = os.path.join(fileDir, 'enow/jython/pythonSrc/preCode.py')
#         fPreCode = open(pathPreCode, "wb")
        fBodyCode = open(pathbodyCode, "wb")
#         fPostCode = open(pathPostCode, "wb")

#         fPreCode.seek(0)
        fBodyCode.seek(0)
#         fPostCode.seek(0)

        fBodyCode.truncate()
        fBodyCode.seek(0)

#         if self.preCode:
#             fPreCode.writelines(self.preCode)
        fBodyCode.write(self.body)
#         if self.postCode:
#             fPostCode.writelines(self.postCode)

#         fPreCode.close()
        fBodyCode.close()
#         fPostCode.close()
