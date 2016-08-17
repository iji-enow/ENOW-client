package org.jython.interfaces;

import java.util.logging.Level;
import java.util.logging.Logger;
import org.python.util.PythonInterpreter;
import org.python.core.PyObject;

public class JythonObjectFactory{
	private static JythonObjectFactory instance = null;
	private static PyObject pyObject = null;
	
	protected JythonObjectFactory(){
		
	}
	
	public static JythonObjectFactory getInstance(){
		if(instance == null){
			instance = new JythonObjectFactory();
		}
		return instance;
	}
	
	public static Object createObject(Object interfaceType, String moduleName){
		Object javaInt = null;
		PythonInterpreter interpreter = new PythonInterpreter();
		interpreter.exec("from " + moduleName + " import " + moduleName);
		
		pyObject = interpreter.get(moduleName);
		
		try{
			PyObject newObj = pyObject.__call__();
			javaInt = newObj.__tojava__(Class.forName(interfaceType.toString().substring(
                    interfaceType.toString().indexOf(" ")+1,
                    interfaceType.toString().length())));
		}catch (ClassNotFoundException ex) {
			// TODO: handle exception
			Logger.getLogger(JythonObjectFactory.class.getName()).log(Level.SEVERE, null, ex);
		}
		return javaInt;
	}
}