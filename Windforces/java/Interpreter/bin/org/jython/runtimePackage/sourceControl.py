
class sourceControl:
    preCode = ""
    body = ""
    postCode = ""
    def __init__(self, preCode, body, postCode):
        self.preCode = preCode
        self.body = body
        self.postCode = postCode
        
        self.storeSources()
        
    def storeSources(self):
        if not self.preCode or not self.body or self.postCode:
            print("At least one of the source is empty\n")
        
        fPreCode = open("./pythonSrc/preCode.py", "r+")
        fBodyCode = open("./pythonSrc/body.py", "r+")
        fPostCode = open("./pythonSrc/postCode.py", "r+")
        
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
        
        