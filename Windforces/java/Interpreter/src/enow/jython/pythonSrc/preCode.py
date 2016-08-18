import sys
import thread
import json
import logging
import os
from enow.jython.pythonSrc.body import eventHandler
from enow.jython.pythonSrc.postCode import postProcess
from enow.jython.pythonSrc.StreamToLogger import StreamToLogger
'''
List : Global Variables
    Descriptions : 
        threadExit : A variable for detecting whether a thread executing the body source has exited"
        loggerStdout : A variable logging the stream passed out on STDOUT
        loggerStderr : A variable logging the stream passed out on STDERR
        lock = A semaphore assigned from thread
'''
threadExit = False
loggerStdout = 0
loggerStderr = 0
lock = 0

def kilobytes(megabytes):
    return megabytes * 1024 * 1024

def eventHandlerFacade(_event, _context, _callback):
    global loggerStdout
    global loggerStderr
    global threadExit
    global lock
    old_stdout = sys.stdout
    old_stderr = sys.stderr
    
    logger = StreamToLogger(loggerStdout, logging.DEBUG)
    sys.stdout = logger
    
    logger = StreamToLogger(loggerStderr, logging.ERROR)
    sys.stderr = logger
    
    eventHandler(_event, _context, _callback)
    
    sys.stdout = old_stdout
    sys.stderr = old_stderr
    
    lock.acquire()
    threadExit = True
    lock.release()
    
    
def Main():
    
    jsonDump = ""
    parameterDump = ""
    while True:
        string = sys.stdin.readline()
        
        if not string:
            break
        
        if string == "end\n":
            break
        
        jsonDump += str(string)
        
    while True:
        string = sys.stdin.readline()
        
        if not string:
            break
        
        if string == "end\n":
            break
        
        parameterDump += str(string)
    
    _event = json.loads(jsonDump)
    _context = dict()
    _callback = dict()
    """
    context object written in json
    ATTRIBUTES:
        * function_name
        * function_version
        * invoked_ERN
        * memory_limit_in_mb
    """
    _context["function_name"] = ""
    _context["function_version"] = ""
    _context["invoked_ERN"] = ""
    _context["memory_limit_in_mb"] = 0
    _context["topicName"] = ""
    _context["deviceID"] = ""
    _context["parameter"] = parameterDump
    
    """
    setting up a thread for executing a body code
    """
    global lock
    lock = thread.allocate_lock()
    global threadExit
    
    stackSize = []
    stackSize.append(kilobytes(_context["memory_limit_in_mb"]))
    thread.stack_size(stackSize)
    
    """
    setting up a logger for debugging
    """
    formatDebug = '%(levelname)s:%(message)s'
    formatError = '%(levelname)s:%(module)s:%(funcName)s in line no (%(lineno)s) : %(message)s'
    
    global loggerStdout
    global loggerStderr
    
    loggerStdout = logging.getLogger("loggerDebug")
    loggerStderr = logging.getLogger("loggerError")
    
    CURRENT_DIR = os.path.abspath(os.path.dirname(__file__))
    
    loggerStdoutFilePath = os.path.join(CURRENT_DIR, 'log')
    loggerStderrFilePath = os.path.join(CURRENT_DIR, 'log')
    
    debugFileHandler = logging.FileHandler(str(loggerStdoutFilePath) + "/stdout")
    debugFileHandler.setLevel(logging.DEBUG)
    errorFileHandler = logging.FileHandler(str(loggerStderrFilePath) + "/stderr")
    errorFileHandler.setLevel(logging.ERROR)
    
    formatterDebug = logging.Formatter(formatDebug)
    formatterError = logging.Formatter(formatError)
    debugFileHandler.setFormatter(formatterDebug)
    errorFileHandler.setFormatter(formatterError)
    
    loggerStdout.addHandler(debugFileHandler)
    loggerStderr.addHandler(errorFileHandler)
    
    try:
        thread.start_new_thread(eventHandlerFacade, (_event, _context, _callback))
    except:
        print("Unable to start thread")
    
    while True:
        lock.acquire()
        if threadExit == True:
            lock.release()
            break
        lock.release()
    
    postProcess(_event, _context, _callback)
    
if __name__ == "__main__":
    sys.stderr.write("preCode.py : running")
    sys.stderr.flush()
    Main()
    sys.stderr.write("preCode.py : exiting")
    sys.stderr.flush()