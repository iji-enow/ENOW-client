from subprocess import PIPE, Popen

class runtime:
    
    def __init__(self):
        self.fStdout.seek(0)
        self.fStderr.seek(0)    
    
    def run(self, _args, _payloads):
        
        jythonPath = "/Users/jeasungpark/jython2.7.0/bin/jython"
        command = []
        command.append(jythonPath)
        command.append("-u")
        command.append("./pythonSrc/preCode.py")
        
        process = Popen(args=command,
                    stdin=PIPE,
                    bufsize=1)
        
        print >> process.stdin, _payloads
        process.stdin.close()