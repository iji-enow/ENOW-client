package org.jython.interfaces;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.jython.interfaces.BuildingType;
import org.jython.interfaces.JythonObjectFactory;


/*
 * Class : entry
 * 	class where the main function is located.
 * 	implemented to show an example about how to use the whole api
 */

public class entry{
	public static void main(String[] args){
		JythonObjectFactory factory = JythonObjectFactory.getInstance();
		
		BuildingType building = (BuildingType)factory.createObject(BuildingType.class, "Building");
		
		JSONParser parser = new JSONParser();
		String result = new String();
		
		try{
			Object obj = parser.parse(new FileReader("./example.json"));
			
			JSONObject jsonObject = (JSONObject)obj;
			
			String PARAMETER = (String)jsonObject.get("PARAMETER");
			String SOURCE = (String)jsonObject.get("SOURCE");
			String PAYLOAD = (String)jsonObject.get("PAYLOAD");
			
			building.setParameter(PARAMETER);
			building.setSource(SOURCE);
			building.setPayload(PAYLOAD);
			
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