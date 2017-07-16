package com.service;

import java.io.*;
import java.net.*;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;
import java.util.Random;

import javax.jws.WebParam;
import javax.servlet.http.HttpServletRequest;
import javax.sql.DataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.dbcp2.BasicDataSourceFactory;
import org.codehaus.xfire.transport.http.XFireServletController;

import org.codehaus.xfire.client.XFireProxyFactory;
import org.codehaus.xfire.service.Service;
import org.codehaus.xfire.service.binding.ObjectServiceFactory;


import org.apache.log4j.FileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.Level;
import org.apache.log4j.SimpleLayout;

class  Msg_Info
{
/* 给终端客户端发待打印的任务消息，首先传递消息头部信息，包括长度等信息
 * 首先向终端客户端发送任务消息对应的任务id和消息长度
 * 
typedef struct _msg_info
{		
	int type;
	int len;
}msg_info;
*/
	public static int length = 8;
	private byte[] buf = new byte[4+4]; 
	
	
	public int type = 0;
	public int len = 0;
	
	/*整型转换为字节数组*/
	private static byte[] toLH(int n) {
	    byte[] b = new byte[4];
	    b[0] = (byte) (n & 0xff);
	    b[1] = (byte) (n >> 8 & 0xff);
	    b[2] = (byte) (n >> 16 & 0xff);
	    b[3] = (byte) (n >> 24 & 0xff);
	    return b;
	}
	/*构造并转换*/
	public Msg_Info(int type,int len) {
		
		this.type = type;
		this.len = len;
	 
		byte[] temp;
	
		temp = toLH(type);
		System.arraycopy(temp, 0, buf, 0, temp.length);
		
		temp = toLH(len);
		System.arraycopy(temp, 0, buf, 4, temp.length);
	}

	/* 返回要发送的数组*/
	public byte[] getBuf() {
		return buf;
	}
};

public class CardPrintImpl implements CardPrint {
	
	public static  int WorkStationPort = 6833;//打印客户端的监听端口 
	public static String WorkStationIP; //打印客户端的监听IP
	public static Socket client_socket = null; 
		
	public static final int RETURN_OK = 0;
	public static final int RETURN_FAIL = -1;
	
	public static long  swiftNumber = 1000000000;   /*任务唯一标识*/
	public static int  Locale = 0x10100001;   /*流水号*/
	
	public static final Logger logger = Logger.getLogger(CardPrintImpl.class);
	
	static {   	
        SimpleLayout layout = new SimpleLayout();
	    /*FileAppender appender = null;
	    try {
	         appender = new FileAppender(layout, "myLog.log", false);
		    } catch (Exception e) {
		    }
		    logger.addAppender(appender);*/
		    logger.setLevel(Level.ERROR);
		    
	    }
	
	
	/*数据库连接池设置*/
	private static final Log log = LogFactory.getLog(CardPrintImpl.class);
	private static final String configFile = "dbcp.properties";
	private static DataSource dataSource;
	static {
	  Properties dbProperties = new Properties();
	  try {
	   dbProperties.load(CardPrintImpl.class.getClassLoader()
	     .getResourceAsStream(configFile));
	   dataSource = BasicDataSourceFactory.createDataSource(dbProperties);
	   Connection conn = getConn();
	   DatabaseMetaData mdm = conn.getMetaData();
	   log.info("Connected to " + mdm.getDatabaseProductName() + " "
	     + mdm.getDatabaseProductVersion());
	   if (conn != null) {
	    conn.close();
	   }
	  } catch (Exception e) {
	   log.error("初始化连接池失败：" + e);
	   
	  }
	}
	
	
    
	 
	 public static final Connection getConn() {
		  Connection conn = null;
		  try {
		   conn = dataSource.getConnection();
		  } catch (SQLException e) {
		   log.error("获取数据库连接失败：" + e);
		  }
		  return conn;
		 }
		 
	 public static void closeConn(Connection conn) {
	  try 
	  {
		   if (conn != null && !conn.isClosed()) {
		    conn.setAutoCommit(true);
		    conn.close();
		   }
	  } 
	  catch (SQLException e) {
	   log.error("关闭数据库连接失败：" + e);
	  }
	 }
	
	public String getversion() {
		return "CardPrint v1.0";
	}
	
	/*终端打印设备注册*/
	public int cardDeviceRegister(
			String workStationIP,    /*终端IP*/
			String workStationPort,   /*终端Port*/
			String cardDeviceName,	/*打印机名称*/
			String encryptIndex) throws SQLException	/*加密因子*/
			
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		
		Date today = new java.util.Date();  
        Timestamp time; 
		
		System.out.println(workStationIP);
		System.out.println(workStationPort);
		
		System.out.println(cardDeviceName);
		System.out.println(encryptIndex);
		
		if (encryptIndex == null || encryptIndex == "")
		{
			return RETURN_FAIL;			
		}
	
		
		
        
        conn=CardPrintImpl.getConn();
        sql = "select id from deviceinfo where encryptIndex = '" +encryptIndex+ "'";
        System.out.println(sql);
        stmt = conn.createStatement();
        rs = stmt.executeQuery(sql);
        if (rs.next())
        {
        	System.out.println("has register");
        	sql = "delete from deviceinfo where encryptIndex = ?";
            System.out.println(sql);
            prest = conn.prepareStatement(sql);
            prest.setString(1, encryptIndex);
            rst = prest.execute();
        }
        
			
		/*获取访问该webservice的客户端*/			
		try {
			HttpServletRequest request = XFireServletController.getRequest();
		    ip = request.getRemoteAddr();			
		} catch (Exception e) {
				e.printStackTrace();
				ip = "";
		}
			
		try 
		{
			time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO deviceInfo"
            	+ "(actionIP,workStationIP,workStationPort,cardDeviceName,encryptIndex,regTime) VALUES"
            	+ "(?,?,?,?,?,?)";
            
            prest = conn.prepareStatement(insertTableSQL);
            prest.setString(1, ip);
            prest.setString(2, workStationIP);
            prest.setString(3,workStationPort);	            
            prest.setString(4, cardDeviceName);
            prest.setString(5,encryptIndex);
            prest.setTimestamp(6,time);
                      
            rst = prest.execute();
           
            CardPrintImpl.closeConn(conn);
		}        
        catch (SQLException e) {
			e.printStackTrace();
			CardPrintImpl.closeConn(conn);
			logger.error("card print register failed"+workStationIP+workStationPort);
			return RETURN_FAIL;
		}			
            
        CardPrintImpl.closeConn(conn);
		return RETURN_OK;
		

	}
	
	/*终端打印机返回打印结果*/
	public int returnPrintResult(
			int isOK,    /*是否成功*/
			String SBYY,   /*失败原因*/
			String KSBM,		/*卡识别码*/
			String ATR,	/*ATR*/
			String yhkh, /*银行卡号*/
			int taskSessionID) throws Exception		/*任务sessionID*/
	{
		String taskID;  /*自动生成的taskID,构成为GY+机具加密因子（10位，加密因子不足由0补齐）+10位流水号*/
	
		Connection sqlConn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		int status;
		Date today = new java.util.Date();  
        Timestamp time; 
				
		sqlConn=CardPrintImpl.getConn();
        try 
        {
			stmt = sqlConn.createStatement();
			if (isOK == 1)
			{
				sql = "update task_info set status = 2 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);				
			}
			else if (isOK == 0)
			{
				sql = "update task_info set status = -1 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);
			    
			    /*打印结果如果为失败时记录失败信息*/
			    time = new Timestamp(today.getTime());
                insertTableSQL = "INSERT INTO ws_error_log"
                	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
                	+ "(?,?,?,?,?)";
                
                prest = sqlConn.prepareStatement(insertTableSQL);
                prest.setInt(1, taskSessionID);
                prest.setString(2, "returnPrintResult");
                prest.setInt(3, isOK);
                prest.setString(4, SBYY);
                prest.setTimestamp(5,time);
                          
                rst = prest.execute();
				
			}
			else if (isOK == 2) /*打印机忙*/
			{
				sql = "update task_info set status = 100 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);	
			    CardPrintImpl.closeConn(sqlConn);
			    logger.error("printer busy taskSessionID:"+taskSessionID);
				return 0;
			}
			else if (isOK == 3) /*收到就回*/
			{
				sql = "update task_info set status = 1 where  id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);	
			    CardPrintImpl.closeConn(sqlConn);
			    
				return 0;
			}
		        
		} catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			CardPrintImpl.closeConn(sqlConn);
			logger.error("returnPrintResult:database error");
			return -1;
		}
		
		CardPrintImpl.closeConn(sqlConn);
		
		/*读取社保方的webservice url和访问的用户名和密码，为调用社保方的回写webservice接口做准备*/
		Properties configProperties = new Properties(); 
		String proFilePath = System.getProperty("user.dir") + "\\guanyuan_config.properties";  
		System.out.println(proFilePath);
		InputStream inputStream;
		try {
			inputStream = new BufferedInputStream(new FileInputStream(proFilePath));
			
			try {   
				configProperties.load(inputStream);   
			  } catch (IOException e1) {   
			   e1.printStackTrace();  
			   logger.error("returnPrintResult:read config file,ioexception");
			   return -1;
			  }   
			  
		} catch (FileNotFoundException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
			logger.error("returnPrintResult:read config file,ioexception");
			return -1;
		}  
		 
		  
	    URL wsUrl = new URL(configProperties.getProperty("yinhai_url"));
	    HttpURLConnection conn;
	    System.out.println(configProperties.getProperty("yinhai_url"));
        
        
        try {
        	conn = (HttpURLConnection) wsUrl.openConnection();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			logger.error("returnPrintResult:can not connect to yinhai webservice");
			return -1;
		}
		
        String jyls;
        String jyyzm;
        String inputstr = "";
	        	        
        conn.setDoInput(true);
        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "text/xml;charset=UTF-8");
        conn.setRequestProperty("SOAPAction", "");
        OutputStream os;
		
		
		try {
			os = conn.getOutputStream();
		} catch (IOException e3) {
			// TODO Auto-generated catch block
			e3.printStackTrace();
			logger.error("returnPrintResult:webservice outputstream error");
			return -1;
		}
  
		System.out.println("username:"+configProperties.getProperty("username")+",password:"+configProperties.getProperty("password"));
		String username = configProperties.getProperty("username");
		String password = configProperties.getProperty("password");
		
        //请求体，获取交易流水号和验证码
		String soap="<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		+"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
		+"<soap:Header>"
			+"<in:system xmlns:in=\"http://yinhai.com\">" +
			"<userName>"+username+"</userName>" +
			"<passWord>"+password+"</passWord>" 
			+"<jylsh>0</jylsh>" +
			"<jyyzm>0</jyyzm>" +
			"<jybh>MakeCardResult</jybh>" +
			"</in:system>"
		+"</soap:Header>" +
		"<soap:Body>" +
			"<ns2:getJylsh xmlns:ns2=\"http://webservice.common.yinhai.com/\">"+
			"</ns2:getJylsh>" +
		"</soap:Body>" +
		"</soap:Envelope>;";
		
		System.out.println("first  soap is"+soap);

        os.write(soap.getBytes());
        InputStream is = conn.getInputStream();
        byte[] b = new byte[1024];
        int len = 0;
        String s = "";
        while((len = is.read(b)) != -1){
            String ss = new String(b,0,len,"UTF-8");
            s += ss;
        }
        s=s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "");
        System.out.println(s);
        is.close();
        os.close();
        conn.disconnect();
        
        if (s == "")
        {
        	logger.error("returnPrintResult:get yinhai yhlsh webservice error");
        	return -1;
        }
   
        /*获取返回的流水交易号和验证码*/
        int startIndex = s.indexOf("<jylsh>")+"<jylsh>".length();
        int endIndex = s.indexOf("</jylsh>");
      
        jyls = s.substring(startIndex, endIndex);
        System.out.println(jyls);
        
        startIndex = s.indexOf("<jyyzm>")+"<jyyzm>".length();
        endIndex = s.indexOf("</jyyzm>");
      
        jyyzm = s.substring(startIndex, endIndex);
        System.out.println(jyyzm);
        
        /*获取到流水号和验证码后，再次调用社保方的MakeCardResult接口来回写结果*/
        wsUrl = new URL(configProperties.getProperty("yinhai_url"));
        conn = (HttpURLConnection) wsUrl.openConnection();
        conn.setDoInput(true);
        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "text/xml;charset=UTF-8");
        conn.setRequestProperty("SOAPAction", "");
	        
		os = conn.getOutputStream();
        
		/*获取taskID*/
		sqlConn=CardPrintImpl.getConn();
		stmt = sqlConn.createStatement();
		sql = "select taskID from task_info where id = "+taskSessionID;
		System.out.println(sql);
        //rs = stmt.executeQuery(sql);
        
        try 
        {
        	rs = stmt.executeQuery(sql);
        }
        catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			CardPrintImpl.closeConn(sqlConn);
			logger.error("mysql operate error");
			return -1;
		}
        
        if (rs.next()) 
        {                
            taskID = rs.getString(1);
            System.out.println("taskID");
            System.out.println(taskID);
        }
        else
        {
        	/*找不到taskID时记录失败信息*/
		    time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            prest = sqlConn.prepareStatement(insertTableSQL);
            prest.setInt(1, taskSessionID);
            prest.setString(2, "returnPrintResult");
            prest.setInt(3, isOK);
            prest.setString(4, SBYY);
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
            CardPrintImpl.closeConn(sqlConn);
            
            logger.error("returnPrintResult:cannot find taskID,taskSession:"+taskSessionID);
        	return -1;
        }
        
        CardPrintImpl.closeConn(sqlConn);
			
			
			
		if (isOK == 1)
		{
			inputstr ="<input>" +
			"<isok>1</isok>" +
			"<sbyy></sbyy>" +
			"<taskid>"+taskID+"</taskid>" +
			"<ksbm>"+KSBM+"</ksbm>" +
			"<atr>"+ATR+"</atr>" +
			"<yhkh>"+yhkh+"</yhkh>" +
			"</input>" ;	
		}
		else
		{	
			startIndex = 0;
			endIndex = SBYY.indexOf("|");	        
			if (endIndex > 0)
			{
				SBYY = SBYY.substring(startIndex, endIndex); 
			}
	        
			inputstr ="<input>" +
			"<isok>0</isok>" +
			"<sbyy>"+SBYY+"</sbyy>" +
			"<taskid></taskid>" +
			"<ksbm></ksbm>" +
			"<atr></atr>" +
			"<yhkh></yhkh>" +
			"</input>" ;
		}
		
		inputstr = inputstr.replace("<","&lt;").replace(">","&gt;");
		System.out.println("inputstr is "+inputstr);
			
		soap="<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			+"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
			+"<soap:Header>"
				+"<in:system xmlns:in=\"http://yinhai.com\">" +
				"<userName>"+username+"</userName>" +
				"<passWord>"+password+"</passWord>" 
				+"<jylsh>"+jyls+"</jylsh>" +
				"<jyyzm>"+jyyzm+"</jyyzm>" +
				"<jybh>MakeCardResult</jybh>" +
				"</in:system>"
			+"</soap:Header>" +
			"<soap:Body>" +
				"<ns2:callBusiness xmlns:ns2=\"http://webservice.common.yinhai.com/\">"+"<inputxml>" +
				inputstr+
				"</inputxml>" +
				
				"</ns2:callBusiness>" +
			"</soap:Body>" +
			"</soap:Envelope>;";
  
		
		
		//soap.getBytes("utf-8");
		System.out.println("second soap is"+soap);

        os.write(soap.getBytes("utf-8"));
        is = conn.getInputStream();
        b = new byte[1024];
        len = 0;
        s = "";
        while((len = is.read(b)) != -1){
            String ss = new String(b,0,len,"UTF-8");
            s += ss;
        }
        //System.out.println(s);
        //System.out.println(s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", ""));
        s=s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "");
        System.out.println(s);
        
        if (s == "")
        {
        	logger.error("returnPrintResult:get yinhai makecardresult webservice error");
        	return -1;
        }
        is.close();
        os.close();
        conn.disconnect();
		
     
		return 0;
	
	}
	
	public static int getLengthString(String str){
		  try{
		   byte[] b = str.getBytes("GBK");
		   return b.length;
		  }catch(Exception ex){
			  logger.error("get LengthString failed");
			  
		   return 0;
		  }
		 }
	
	public String AddTask(			
			String strClientName,   /*客户端标识*/
			String strQueueName,		/*队列名称*/
			String strTaskName,		/*任务名称*/
			String strTaskData,	/*任务数据*/
			String intPriority) throws Exception	/*优先级*/
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		int status = 0;
		Date today = new java.util.Date();  
        Timestamp time; 
		StringBuffer sb = new StringBuffer();
		String taskID;
		int taskinfoID = 0;

		conn=CardPrintImpl.getConn();
		stmt = conn.createStatement();
		
		sql = "select global_value from globalValue limit 1";
        rs = stmt.executeQuery(sql);
        if (rs.next()) 
        { 
        	swiftNumber = rs.getInt(1);
        	System.out.println(swiftNumber);
        	swiftNumber = swiftNumber + 1;
        	stmt = conn.createStatement();
			sql = "update globalValue set global_value = "+swiftNumber+ " where global_value = "+rs.getInt(1);
			System.out.println(sql);
            stmt.executeUpdate(sql);
        }
        else
        {
        	swiftNumber = swiftNumber + 1;
        }
        
        String locale = "";
        String lsh = "";
        int lsh_hex = 0x100001;
        
        /*自动生成流水号*/
        sql = "select locale,lsh from identity_number limit 1";
        rs = stmt.executeQuery(sql);
        if (rs.next()) 
        { 
        	locale = rs.getString(1);  //地区号
        	lsh = rs.getString(2);   //6位2进制全局号
        	System.out.println(lsh);  
        	lsh_hex = Integer.valueOf(lsh,16);  //转为整型进行计算
        	lsh_hex = lsh_hex + 1;
            System.out.format( "%x ",lsh_hex );
        	lsh = Integer.toHexString(lsh_hex); //计算完毕后转为字符串
        	System.out.println("lsh-str:"+lsh);
        	locale = locale + lsh;  //地区号和6位随机号组合
        	System.out.println(locale);
        	stmt = conn.createStatement();
			sql = "update identity_number set lsh = '" +lsh+ "' where locale = '" +rs.getString(1)+ "' ";
			System.out.println(sql);
            stmt.executeUpdate(sql);
        }
        else
        {
        	Locale = Locale + 1;
        	lsh = Integer.toHexString(Locale);
        	insertTableSQL = "INSERT INTO identity_number"
            	+ "(locale,lsh) VALUES"
            	+ "(?,?)";            
           
            prest = conn.prepareStatement(insertTableSQL);           
            prest.setString(1, "10");         
            prest.setString(2,lsh);
           
            rst = prest.execute();
        	
        }
        
		sql = "select id from task_info where (status = 0 or status = 1 or status = 100) and task_name = '" +strTaskName+ "' ";
        rs = stmt.executeQuery(sql);
 
        if (rs.next()) 
        { 
        	taskinfoID = rs.getInt(1);
        	
    		taskID = "GY"+strQueueName+swiftNumber;
    		
        	sb.append("<result>\r"); 
			sb.append("<code>0</code>\r"); 
		    sb.append("<strTaskID>");
		    sb.append(taskID);
		    sb.append("</strTaskID>\r"); 
		    sb.append("<message>打印任务已经下发，不能重复下发</message>\r");   //数据成功时保留为空
		    sb.append("</result>\r"); 
		    
		    time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            /*重复下发的情况保存到数据库的日志中*/
            prest = conn.prepareStatement(insertTableSQL);
            prest.setInt(1,taskinfoID );
            prest.setString(2, "addTask");
            prest.setInt(3, 0);
            prest.setString(4, "打印任务已经下发，不能重复下发");
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
            logger.error("打印任务重复下发，任务ID:"+taskinfoID);
		    
		    CardPrintImpl.closeConn(conn);
		    
		    System.out.println(sb.toString());
		    return sb.toString();
            
        }
		
		
		taskID = "GY"+strQueueName+swiftNumber;
		System.out.println("taskID:"+taskID);
		time = new Timestamp(today.getTime());
   
        insertTableSQL = "INSERT INTO task_info"
        	+ "(client_name,queue_name,task_name,task_data,intPriority,taskID,status,addTime,lsh) VALUES"
        	+ "(?,?,?,?,?,?,?,?,?)";
        
        try {
				prest = conn.prepareStatement(insertTableSQL);
				prest.setString(1, strClientName);
		        prest.setString(2,strQueueName);
		        prest.setString(3, strTaskName);
		        
		        prest.setString(4,strTaskData);
		        prest.setString(5,intPriority);
		        prest.setString(6,taskID);
		        prest.setInt(7, status);
		        prest.setTimestamp(8, time);
		        prest.setString(9, locale);
		        rst = prest.execute();
	        
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			StringWriter sw = new StringWriter(); 

            e.printStackTrace(new PrintWriter(sw, true)); 

            String str = sw.toString(); 

            logger.error("任务保存到数据库失败,失败信息为："+str+" 任务信息为：strClientName "+strClientName+
            			" strQueueName "+strQueueName+" strTaskName "+strTaskName+
            			" strTaskData "+strTaskData);
            			
			
			time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            /*保存任务到数据库失败，写日志*/
            prest = conn.prepareStatement(insertTableSQL);
            prest.setInt(1, taskinfoID);
            prest.setString(2, "addTask");
            prest.setInt(3, 0);
            prest.setString(4, "任务保存到数据库失败:"+strTaskName);
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
			sb.append("<result>\r"); 
			sb.append("<code>0</code>\r"); 
		    sb.append("<strTaskID>");
		    sb.append(taskID);
		    sb.append("</strTaskID>\r"); 
		    sb.append("<message>任务保存到数据库失败</message>\r");   //数据成功时保留为空
		    sb.append("</result>\r"); 
		    
		    System.out.println(sb.toString());
		    CardPrintImpl.closeConn(conn);
		    return sb.toString();
		}
		
		CardPrintImpl.closeConn(conn);
        
		
		sb.append("<result>\r"); 
		sb.append("<code>1</code>\r"); 
	    sb.append("<strTaskID>");
	    sb.append(taskID);
	    sb.append("</strTaskID>\r"); 
	    sb.append("<message></message>\r");   //数据成功时保留为空
	    sb.append("</result>\r"); 
	    System.out.println(sb.toString());
	    return sb.toString();
	}
	
	public String getTask(String encryptIndex) throws Exception
	{
		int len =0;
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		String taskData = "init";
		int taskinfoID = 0;
		String lsh ="";
		int status;
		int num = 0;
		int msgLen = 0;
		Date today = new java.util.Date();  
        Timestamp time;
        String s = "no task";
		StringBuffer sb = new StringBuffer();
		
		CardPrintImpl register = new CardPrintImpl();
		
		/*查询数据库中是否存在待打印的任务，
		 * 其中status=0表示初始状态，status=1表示已经将任务下发给客户端，status=2表示客户端返回成功的打印结果
		 * status=-1表示客户端返回失败的打印结果
		 */
		conn=CardPrintImpl.getConn();
		stmt = conn.createStatement();
		sql = "select task_data,id,lsh from task_info where (status = 0 or status = 1) and queue_name = '" +encryptIndex+ "' order by intPriority limit 1";
        rs = stmt.executeQuery(sql);
        
        if (rs.next()) 
        { 
            taskData = rs.getString(1);
            taskinfoID = rs.getInt(2); 
            lsh = rs.getString(3);
            
        }
        else
        {
        	CardPrintImpl.closeConn(conn);
        	return s;
        	//return sb.toString();
        }
        
        stmt = conn.createStatement();
		sql = "update task_info set status = 1 where status = 0 and id = "+taskinfoID;
		
        stmt.executeUpdate(sql);
        CardPrintImpl.closeConn(conn);
        
        int taskLen = register.getLengthString(taskData);
        
        sb.append("<?xml version='1.0' encoding='gbk'?>\r");
		sb.append("<taskinfo>\r"); 
	    sb.append("<strTaskID>");
	    sb.append(taskinfoID);
	    sb.append("</strTaskID>\r"); 
	    sb.append("<strlsh>");
	    sb.append(lsh);
	    sb.append("</strlsh>\r"); 
	    sb.append("<strTaskData>");
	    sb.append(taskData);
	    sb.append("</strTaskData>\r");
	    sb.append("<strTaskLen>");
	    sb.append(String.valueOf(taskLen));
	    sb.append("</strTaskLen>\r");
	    
	    sb.append("</taskinfo>\r"); 
	    
	    
	    String taskString = sb.toString();
	    
	    byte[] bytes=taskString.getBytes("gbk");
	    s = new String(bytes,"gbk");
	    
	    /*byte[] buf = new byte[taskLen]; 
		
		byte[] temp = taskString.getBytes();
	    System.arraycopy(temp, 0, buf, 0, taskLen);
	    
	    taskString = new String(taskString.getBytes("utf-8"));  
	    
	    String s1 = new String(buf, "GBK");*/
	    
	    return s;

	}
	
	public static Socket Client_socket_init()
	{
		Socket socket_client = null;
		InetAddress addr = null;
		/*设置socket的基础参数*/
		
		try {
			/*设置socket 地址 端口信息*/
			addr = InetAddress.getByName(WorkStationIP);
			socket_client = new Socket(addr, WorkStationPort);
			System.out.println("Server:socket = " + socket_client);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		
		
		return socket_client;
	}
	
	/* send message !*/
	public static void send_messageLen(Socket socket,int type,int len)
	{
		/*判断参数！！*/
		if(socket==null || len <0)
		{
			System.out.println("Server : input  para  error !");
			return ;
		}
		//byte[] receive = new byte[1056];
		int available=0;
		//Msger_Info mmInfo=null;
		Msg_Info sendmsg = new Msg_Info(type,len);
		try {
			if((available=socket.getInputStream().available())>=0)
			{
				//socket.getInputStream().read(new byte[available]);
				socket.getInputStream().skip(available);
			}
			socket.getOutputStream().write(sendmsg.getBuf());
			socket.getOutputStream().flush();
			//socket.getInputStream().read(receive);
			//mmInfo =Msger_Info .getmsger_info(receive);
			//System.out.println("received:name is :"+mmInfo.name+",msg is :"+mmInfo.msg+",len is :"+mmInfo.len);
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			logger.error("socket msg header send failed");
		}
		return ;
		//return mmInfo.msg;
		
	}
	
	
	
	
	public static void main(String[] args) throws SQLException {
		
		int len =0;
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		String taskData = "init";
		int taskinfoID = 0;
		int status;
		int num = 0;
		int msgLen = 0;
		Date today = new java.util.Date();  
        Timestamp time;
        String encryptIndex;
        int lsh = 0x00000001;
        int count = 0;
        String str = "100abcf";
		
		
        System.out.println("社保数据处理中，请不要关闭");
        
        lsh = Integer.valueOf(str,16);
        System.out.format( "%x ",lsh );
		
		// TODO Auto-generated method stub
		CardPrintImpl register = new CardPrintImpl();
		
		
		//每秒遍历任务表，取出一条未处理的优先级最高的记录
		while (true)
		{
			try 
			{	
				Thread.currentThread().sleep(50000);            
	        } 
			catch (InterruptedException e) {
	            e.printStackTrace(); 
	        }
	
        	Calendar calendar = Calendar.getInstance();
            //把当前时间减去3个月，如果为负数则会把年份往前推，比如2010年1月-3会变成2009年10月
            calendar.add(Calendar.MINUTE,-5);
        	Timestamp curTime = new Timestamp(calendar.getTimeInMillis());
        	System.out.println(curTime);
        	Timestamp addTime;
        	int tmpTaskSessionID = 0;
        	
        	conn=CardPrintImpl.getConn();
    		stmt = conn.createStatement();
        	sql = "select id,addTime from task_info where status = 1";
            rs = stmt.executeQuery(sql);
            while (rs.next())
            {
            	tmpTaskSessionID = rs.getInt(1);
            	addTime = rs.getTimestamp(2);
            	System.out.println(addTime);
            	if (curTime.after(addTime))
            	{
            		System.out.println("该条记录已经超时");
            		logger.error("this record has been timeout,taskSessionID:"+tmpTaskSessionID);
            		 //调用回写接口
            		String serviceUrl = "http://localhost:8080/CardPrintService/services/CardPrint";
            		Service serviceModel = new ObjectServiceFactory().create(CardPrint.class, null, "http://localhost:8080/CardPrintService/services/CardPrint?wsdl", null);
            		XFireProxyFactory serviceFactory = new XFireProxyFactory();
            		try
            		{
            			CardPrint service = (CardPrint)serviceFactory.create(serviceModel,serviceUrl);
            			status = service.returnPrintResult(0, "该打印记录在超时时间内未打印|", "", "", "", tmpTaskSessionID);
            		    System.out.println(status);
            		}
            		catch(Exception e)
            		{
            			logger.error("print timeout,get yinhai webservice error");
            		    e.printStackTrace();
            		}
            		stmt = conn.createStatement();
            		sql = "update task_info set status = -1 where id = "+tmpTaskSessionID;
            		
                    stmt.executeUpdate(sql);
                    
            		
            	}
            	
            }
        	CardPrintImpl.closeConn(conn);
        	continue;
           
		}
		
	
	}
	
}