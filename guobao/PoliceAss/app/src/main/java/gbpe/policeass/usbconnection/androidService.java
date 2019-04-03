package gbpe.policeass.usbconnection;


import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import gbpe.policeass.utils.AppLog;

public class androidService extends Service
{

	public static final String TAG = "guobaoErr";
	public static Boolean mainThreadFlag = true;
	public static Boolean ioThreadFlag = true;
	ServerSocket serverSocket = null;
	//final int SERVER_PORT = 10086;
	int SERVER_PORT = 10086;

	private sysBroadcastReceiver sysBR;

	@Override
	public IBinder onBind(Intent intent)
	{
		return null;
	}

	@Override
	public void onCreate()
	{
		super.onCreate();
		AppLog.d(TAG, "androidService--->onCreate()");
		/* 创建内部类sysBroadcastReceiver 并注册registerReceiver */
		sysRegisterReceiver();
		/*
		 * new Thread() { public void run() { doListen(); }; }.start();
		 */
	}

	private void doListen()
	{
		serverSocket = null;
		try
		{
			//AppLog.d(TAG, "doListen()");
			serverSocket = new ServerSocket(SERVER_PORT);
			AppLog.d(TAG, "doListen() 2");
			while (mainThreadFlag)
			{
				//AppLog.d(TAG, "doListen() 3");
				Socket socket = serverSocket.accept();
				AppLog.d(TAG, "doListen() 4");
				new Thread(new ThreadReadWriterIOSocket(this, socket)).start();
			}
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}

	/* 创建内部类sysBroadcastReceiver 并注册registerReceiver */
	private void sysRegisterReceiver()
	{
		AppLog.v(TAG, Thread.currentThread().getName() + "---->" + "sysRegisterReceiver");
		sysBR = new sysBroadcastReceiver();
		/* 注册BroadcastReceiver */
		IntentFilter filter = new IntentFilter();
		/* 新的应用程序被安装到了设备上的广播 */
		filter.addAction("android.intent.action.PACKAGE_ADDED");
		filter.addDataScheme("package");
		filter.addAction("android.intent.action.PACKAGE_REMOVED");
		filter.addDataScheme("package");
		filter.addAction("guobao.intent.action.ModifyServicePort");
		registerReceiver(sysBR, filter);
	}

	/* 内部类：BroadcastReceiver 用于接收系统事件 */
	private class sysBroadcastReceiver extends BroadcastReceiver
	{
		@Override
		public void onReceive(Context context, Intent intent)
		{
			String action = intent.getAction();
			if (action.equalsIgnoreCase("android.intent.action.PACKAGE_ADDED"))
			{
				// ReadInstalledAPP();
			}
			else if (action.equalsIgnoreCase("android.intent.action.PACKAGE_REMOVED"))
			{
				// ReadInstalledAPP();
			}
			else if (action.equalsIgnoreCase("guobao.intent.action.ModifyServicePort"))
			{
				// 修改服务器端口
				ioThreadFlag = false;

				String sPort = intent.getStringExtra("Port");  //发送广播形式：putExtra("Port", sPort);
				try {
					SERVER_PORT = Integer.parseInt(sPort);
					Toast.makeText(getApplicationContext(), "修改端口成功", Toast.LENGTH_SHORT).show();
				} catch (NumberFormatException e) {
					e.printStackTrace();
				}
				ioThreadFlag = true;
				doListen();

			}

			AppLog.v(TAG, Thread.currentThread().getName() + " ---> " + "sysBroadcastReceiver onReceive");
		}
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
	{
		AppLog.d(TAG, "androidService----->onStartCommand()");
		mainThreadFlag = true;
		new Thread()
		{
			public void run()
			{
				doListen();
			};
		}.start();
		return START_NOT_STICKY;
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		// 关闭线程
		mainThreadFlag = false;
		ioThreadFlag = false;
		// 关闭服务器
		unregisterReceiver(sysBR);
		try
		{
			AppLog.v(TAG, Thread.currentThread().getName() + "---->" + "serverSocket.close()");
			if (serverSocket != null) serverSocket.close();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		AppLog.v(TAG, Thread.currentThread().getName() + "---->" + "**************** onDestroy****************");
	}

}
