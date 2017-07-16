package com.service;




import java.io.*;
import java.net.*;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

import javax.servlet.http.HttpServletRequest;
import javax.sql.DataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.dbcp2.BasicDataSourceFactory;
import org.codehaus.xfire.transport.http.XFireServletController;



public class lindaImpl implements Ilinda {
	
	public static final String IP_ADDR = "localhost";//服务器地址 
	public static final int PORT = 6832;//服务器端口号  
	
	public static final int REGISTER_OK = 0;
	public static final int REGISTER_FAIL = -1;
	
	public static int ID = 0;
	
	private static final Log log = LogFactory.getLog(lindaImpl.class);
	 private static final String configFile = "dbcp.properties";
	 private static DataSource dataSource;
	 static {
	  Properties dbProperties = new Properties();
	  try {
	   dbProperties.load(lindaImpl.class.getClassLoader()
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
		 /**
		  * 关闭连接
		  * 
		  * @param conn
		  *            需要关闭的连接
		  */
		 public static void closeConn(Connection conn) {
		  try {
		   if (conn != null && !conn.isClosed()) {
		    conn.setAutoCommit(true);
		    conn.close();
		   }
		  } catch (SQLException e) {
		   log.error("关闭数据库连接失败：" + e);
		  }
		 }
	
	public String example(String message) {
		return message+"hipengwujie,this is your first";
	}
	
	public String getversion1() {
		return "version is";
	}
	
	public int cardDeviceRegister(
			String workStationIP,    /*终端IP*/
			int workStationPort,   /*终端Port*/
			String encryptIP,		/*加密机IP*/
			int encryptPort,		/*加密机Port*/
			String cardDeviceName,	/*打印机名称*/
			String encryptIndex,	/*加密因子*/
			boolean isDecrypt)		/*是否加密机制卡*/
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        String ip=null;
		
		if (true)
		{
			System.out.println(workStationIP);
			System.out.println(workStationPort);
			System.out.println(encryptIP);
			System.out.println(encryptPort);
			System.out.println(cardDeviceName);
			System.out.println(encryptIndex);
			System.out.println(isDecrypt);
			
			
			try {
				HttpServletRequest request = XFireServletController.getRequest();
			    ip = request.getRemoteAddr();
				System.out.println("访问的客户端 IP 是:"+ ip);
				
	            
	            System.out.println("Results:");
			} catch (Exception e) {

					System.out.println("无法获取HttpServletRequest");
					e.printStackTrace();

			}
			
			try 
			{
				conn=lindaImpl.getConn();
				stmt = conn.createStatement();
				sql = "select * from deviceInfo";
	            rs = stmt.executeQuery(sql);
	            System.out.println("Results:");
	            int numcols = rs.getMetaData().getColumnCount();
	            System.out.println("numcols " + numcols);
	            while (rs.next()) 
	            {
	                
	                System.out.println(" " + rs.getInt(1));
	                System.out.println(" " + rs.getString(2));
	                System.out.println(" " + rs.getInt(3));
	            }
	            ID = ID +6;
	            conn=lindaImpl.getConn();
	            sql = "insert into deviceInfo values("+ID+","+workStationIP+",workStationPort,"+encryptIP+",encryptPort,"+cardDeviceName+","+encryptIndex+",0)";
	            stmt.execute(sql);
	            System.out.println("Results:");
	            
	            ip="202.115.0.1";
	            sql = "insert into actionLog values("+ip+")";
	            stmt.execute(sql);
			} 
            
            catch (SQLException e) {
            	System.out.println("connect wrong!");
				e.printStackTrace();
			}
			
            lindaImpl.closeConn(conn);
			return REGISTER_OK;
		}
		else
		{
			return REGISTER_FAIL;
		}

	}
	
	public class msgHdr
	{
		public  int type;
		public  int len;
	}
	
	public static void main(String[] args) {
		
		int len =0;
		// TODO Auto-generated method stub
		lindaImpl register = new lindaImpl();
		
		register.cardDeviceRegister("1",2,"3",4,"5","6",true);
		
		System.out.println("客户端启动...");  
        System.out.println("当接收到服务器端字符为 \"OK\" 的时候, 客户端将终止\n"); 
        while (true) {  
        	Socket socket = null;
        	try {
        		//创建一个流套接字并将其连接到指定主机上的指定端口号
	        	socket = new Socket(IP_ADDR, PORT);  
	              
	            //读取服务器端数据  
	            DataInputStream input = new DataInputStream(socket.getInputStream());  
	            //向服务器端发送数据  
	            ObjectOutputStream  out = new ObjectOutputStream(socket.getOutputStream());  
	           
	            /*lindaImpl.msgHdr inner = register.new msgHdr();
	            inner.type = 1;
	            inner.len = 512;
	            out.writeObject(inner);*/
	            
	            Integer len1 = 512;
	            
	            DataOutputStream out1 = new DataOutputStream(socket.getOutputStream());  
	            //out1.write(len1);
	            /*
	            for (int i =0 ;i< 4;i++)
	            {
	            	String str = "abcde|bcdef|cde";
	            	len = len + str.length();
	            	
		            out.writeUTF(str); 
		            System.out.println("已发送：+len+");
	            }*/
	            
	              
	            String ret = input.readUTF();   
	            System.out.println("服务器端返回过来的是: " + ret);  
	            // 如接收到 "OK" 则断开连接  
	            if ("OK".equals(ret)) {  
	                System.out.println("客户端将关闭连接");  
	                Thread.sleep(500);  
	                break;  
	            }  
	            
	            out.close();
	            input.close();
        	} catch (Exception e) {
        		System.out.println("客户端异常:" + e.getMessage()); 
        	} finally {
        		if (socket != null) {
        			try {
						socket.close();
					} catch (IOException e) {
						socket = null; 
						System.out.println("客户端 finally 异常:" + e.getMessage()); 
					}
        		}
        	}
        }
	}
	
}