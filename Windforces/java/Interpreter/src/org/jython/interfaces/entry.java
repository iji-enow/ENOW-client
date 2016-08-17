package org.jython.interfaces;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.jython.interfaces.BuildingType;
import org.jython.interfaces.JythonObjectFactory;

/*
 * Class : entry
 * 	class where the main function is located.
 * 	implemented to show an example about how to use the whole api
 */

public class entry{
	public static void key(String[] args){
		JythonObjectFactory factory = JythonObjectFactory.getInstance();
		
		BuildingType building = (BuildingType)factory.createObject(BuildingType.class, "Building");
		building.setSource(new String(""));
		building.setPayload(new String(""));
		building.setParameter(new String(""));
		
		System.out.println(building.getParameter() +
				" " + 
				building.getPayload() + 
				" " + 
				building.getSource());
	}
}