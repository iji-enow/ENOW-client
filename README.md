# enow-api
  A project aiming to replicate AWS Lambda in a manner of IoT.

## Prerequisite

- Eclipse
[here](http://www.eclipse.org/home/index.php)

  Basically, this project is done under Eclipse IDE. So you may use it or get another decent IDE that can show this whole project.

- Java JDK
[Java SE Development Kit 8u101](http://www.oracle.com/technetwork/java/javase/downloads/index.html)

  This project is also based on JDK 8 update 101. Unless there is a specific reason why you should use different version of JDK, please get the JDK installed on your machine.

- Install [json-simple](https://code.google.com/archive/p/json-simple/)

  There are hundreds of thousands of libraries for JSON nowadays. We chose to work with __json-simple__.

- Install
[Jython](http://www.jython.org)

  Another great feature of this project is that you can use python in java context. In order to do so, __Jython__ is the only feasible answer. So you need to get this liberary on your own project.

## To Do list

- [ ]This project aims to run python script on java runtime.

- [ ]There are _2_ necessary __key : 'value'__  pairs and _1_ optional __key : 'value'__ pair on the json payload inside your MQTT request.

  ```
  {
  "PARAMETER" : "-ls -t",
  "SOURCE" : "def eventHandler(event, context, callback):\n\tevent[\"identification\"] = \"modified\"\n\tprint(\"succeed\")",
  "PAYLOAD" : {"identification" : "original"}
  }
  ```
  __REQUIRED__ : "PAYLOAD", "SOURCE"
  __OPTIONAL__ : "PARAMETER"

- [ ]All you need to do is simple. _Load the pairs and run!_

## Usage

### entry

- The first thing you need to do is to implement an entry program in java. Below is a sample program.

```
package enow.jython.interfaces;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import enow.jython.interfaces.BuildingType;
import enow.jython.interfaces.JythonObjectFactory;

/*
 * Class : entry
 * 	class where the main function is located.
 * 	implemented to show an example about how to use the whole api
 */

public class entry{
	public static void main(String[] args){
		JythonObjectFactory factory = JythonObjectFactory.getInstance();
		@SuppressWarnings("static-access")
		BuildingType building = (BuildingType)factory.createObject(BuildingType.class, "Building");

		JSONParser parser = new JSONParser();
		String result = new String();

		try{
			String cwd = System.getProperty("user.dir");
			cwd += "/src/enow/jython/interfaces/example.json";
			Object obj = parser.parse(new FileReader(cwd));

			JSONObject jsonObject = (JSONObject)obj;

			String PARAMETER = (String)jsonObject.get("PARAMETER");
			String SOURCE = (String)jsonObject.get("SOURCE");
			JSONObject PAYLOAD = (JSONObject)jsonObject.get("PAYLOAD");
			String PAYLOAD_STR = PAYLOAD.toString();

			building.setParameter(PARAMETER);
			building.setcode(SOURCE);
			building.setPayload(PAYLOAD_STR);

			result = building.run();
		}
		catch(FileNotFoundException e){
			e.printStackTrace();
		}
		catch(IOException e){
			e.printStackTrace();
		} catch (org.json.simple.parser.ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		System.out.println(result);
	}
}
```

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
