package org.jython.temp;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.jython.temp.BuildingType;
import org.jython.temp.JythonObjectFactory;

public class entry{
	public static void main(String[] args){
		JythonObjectFactory factory = JythonObjectFactory.getInstance();
		int result = 0;
		
		BuildingType building = (BuildingType)factory.createObject(BuildingType.class, "Building");
		building.set(1);
		result = building.get();
		
		System.out.println("Result : " + result + "\n");
	}
}