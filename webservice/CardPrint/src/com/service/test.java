package com.service;

import org.codehaus.xfire.client.XFireProxyFactory;
import org.codehaus.xfire.service.Service;
import org.codehaus.xfire.service.binding.ObjectServiceFactory;

public class test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int status = -1;
		String serviceUrl = "http://localhost:8080/CardPrintService/services/CardPrint";
		Service serviceModel = new ObjectServiceFactory().create(CardPrint.class, null, "http://localhost:8080/CardPrintService/services/CardPrint?wsdl", null);
		XFireProxyFactory serviceFactory = new XFireProxyFactory();
		try
		{
			CardPrint service = (CardPrint)serviceFactory.create(serviceModel,serviceUrl);
			status = service.returnPrintResult(true, "too big", "ksbm", "atr", "yhkh");
		        System.out.println(status);
		}
		catch(Exception e)
		{
		    e.printStackTrace();
		}

	}

}
