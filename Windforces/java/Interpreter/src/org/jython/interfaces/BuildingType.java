package org.jython.interfaces;

public interface BuildingType{
	public String getSource();
	public String getParameter();
	public String getPayload();
	
	public void setParameter(String _parameter);
	public void setSource(String _source);
	public void setPayload(String _payload);
	public void run();
}