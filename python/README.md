# enow-api
  A project aiming to replicate AWS Lambda in a manner of IoT.

## Prerequisite

- Python
[here](https://www.python.org/downloads/)

  Basically we need to choose a language that processes a source submitted by user. So the language should not be something that compiles first and run later. If there were any compilation stage in the whole process, then the users who implements a program with our platform should specify many flags which makes the program harder to execute.

  __Python__ is one of the languages that doesn't need any compilation stage. The way it works is to read source and interpret it. __NODE.js__ was one of the languages that we took it under consideration but the way it approaches to the lower level of __Linux__ system is much more complicated. So that is why we chose to work with __Python__.

- setuptools and pip
[here](https://pypi.python.org/pypi/setuptools) and
[here](https://pypi.python.org/pypi/pip)

  In order to install additional package, python package index is needed. So you need to install it to make use of __Python__.

- pyenv(_optional_)
[here](https://github.com/yyuu/pyenv)

  This software is meant to be embedded in Apache Storm. So if you wanted to use it on your own project, you would have to install and configure your own virtual environment for __Python__. I would recommend __Pyenv__.

## To Do list

- [ ] SSL/TLS connection in order to guarantee safety of our service

- [ ] Configure platform-independent environment using __pip__ and __Debian__ package.

- [ ] Hosting additional service such as Amazon Alexa.

## Usage

### entry

- There are _2_ necessary __key : 'value'__  pairs and _1_ optional __key : 'value'__ pair on the json payload inside your MQTT request.

  ```
  {
  "PARAMETER" : "-ls -t",
  "SOURCE" : "def eventHandler(event, context, callback):\n\tevent[\"identification\"] = \"modified\"\n\tprint(\"succeed\")",
  "PAYLOAD" : {"identification" : "original"}
  }
  ```
  __REQUIRED__ : "PAYLOAD", "SOURCE"
  __OPTIONAL__ : "PARAMETER"


### eventHandler
- The basic form of *eventHandler* is as follow
  ```
  import something

  def eventHandler(event, context, callback):
     event["something"] = "value"
     print("contents")
     raise Exception
     callback["returned"] = "value"
  ```

- The eventHandler supports following 3 features.
  * import packages

    There are one and only package name where the eventHandler is located. Tis called __pythonSrc__. If you wanted another source inside the package, there would be no feasible way to do so, because it would cause cyclic import. Other than that, it is ok.

  * getting payload

    Another great thing you can do in the eventHandler is that you can get a value of the __PAYLOAD__ key in the payload you've sent eariler and use it as you wish. If you got something to return to other program, all you need to do is to set a key and value pair on callback parameter. The rest of the jobs to send them to anywhere is on us.

  * print to _STDIN_ and _STDERR_

    Getting what you've printed out is one of the coolest features we've ever implemented. In order to do so, you just __print__ everything and get the result later on __pythonSrc/log/log.txt__ file.


### References
[https://code.google.com/archive/p/json-simple/]

[http://jythonbook-ko.readthedocs.io/en/latest/]

__사이먼, 몽. (2015.1.5). In 송 성. (Ed.), 라즈베리 파이 쿡북 [200여 가지 레시피로 라즈베리 파이 완전 분석] (경. 박 Trans.). (초판 2쇄 ed.). 한빛미디어(주): O'REILLY.__

__이강성. (2013.10.1). In 최홍석 이. (Ed.), 파이썬 3 바이블 [스크립트 언어의 지존을 만나다!] (초판 ed.). 최홍석: 주식회사 프리렉.__
