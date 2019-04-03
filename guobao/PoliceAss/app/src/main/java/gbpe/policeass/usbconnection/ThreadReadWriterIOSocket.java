package gbpe.policeass.usbconnection;

import android.content.Context;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import gbpe.policeass.utils.FileHelper;
import gbpe.policeass.utils.MyUtil;


public class ThreadReadWriterIOSocket implements Runnable
{
	private Socket client;
	private Context context;

	E12DeviceInfo m_cE12Dev = new E12DeviceInfo();

	public int sCTimeOut = 3000;	//发送命令时间超时
	public int rCTimeOut = 5000;	//读取命令时间超时

	BufferedOutputStream out;
	BufferedInputStream in;
	String m_CurrCmd = "";	//PC端发来的数据

	byte[] tempbuffer = new byte[2048];

	public ThreadReadWriterIOSocket(Context context, Socket client)
	{
		this.client = client;
		this.context = context;
	}

	@Override
	public void run()
	{
		Log.d(androidService.TAG, "a client has connected to server!");

		try
		{
			out = new BufferedOutputStream(client.getOutputStream());
			in = new BufferedInputStream(client.getInputStream());
			androidService.ioThreadFlag = true;
			while (androidService.ioThreadFlag)
			{
				try
				{
					if (!client.isConnected())
					{
						Log.e(androidService.TAG,  "client is not Connected ???? ");
						break;
					}

					//接收PC发来的数据
					//Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "will read......");
					//读操作命令
					//m_CurrCmd = "";
					m_CurrCmd = readCmdFromSocket(in,5000);
					Log.i(androidService.TAG, Thread.currentThread().getName() + "---->" + "**currCMD == " + m_CurrCmd);

					//数据诊断
					String strTmp = m_CurrCmd.substring(0,3);
					if (!strTmp.equals("E12"))
					{
						Log.e(androidService.TAG,  "Data is not equals E12 , error !");
						break;
					}

					strTmp = m_CurrCmd.substring(0,7);
					if (strTmp.equals("E12CMDF"))
					{
						E12_CMDF_Execute();				//使用最频繁的命令处理
					}
					else if (strTmp.equals("E12CMDS"))
					{
						E12_CMDS_Execute();				//行业标准命令处理
					}
					else if (strTmp.equals("E12CMDN"))
					{
						E12_CMDN_Execute();				//自定义命令处理
					}
					else if (strTmp.equals("E12CMDD"))
					{
						E12_CMDD_Execute();				//自定义命令处理(开环，不用返回)
					}
					else
					{
						//错误的命令数据
						Log.e(androidService.TAG,  "Error commad !");
						break;
					}


				} catch (Exception e)
				{
					e.printStackTrace();
					Log.e(androidService.TAG,  "try Error.");
				}
			}
			out.close();
			in.close();
		} catch (Exception e)
		{
			e.printStackTrace();
		} finally
		{
			try
			{
				if (client != null)
				{
					Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "client.close()");
					client.close();
				}
			} catch (IOException e)
			{
				Log.e(androidService.TAG, Thread.currentThread().getName() + "---->" + "read write error !!!");
				e.printStackTrace();
			}
		}
	}

	//处理使用最频繁的命令
	public void E12_CMDF_Execute()
	{
		try {
			if (m_CurrCmd.equals(E12DeviceInfo.Flag_Test_Data_Transmission_Cmd))		//网络测试，等效于ping;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Test_Data_Transmission_OK, sCTimeOut);
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetGpsPositionInfo_XYZ))	//得到GPS位置信息;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String strtmp = "";
				strtmp = m_cE12Dev.GetGpsPositionInfo(context);
				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_GetGpsPositionInfo_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals("4"))
			{
				Log.e(androidService.TAG, Thread.currentThread().getName() + "---->" + "**currCMD ==== " + m_CurrCmd);
				/* 准备接收文件数据 */
				try
				{
					out.write("service receive OK".getBytes());
					out.flush();
				} catch (IOException e)
				{
					e.printStackTrace();
				}

				/* 接收文件数据，4字节文件长度，4字节文件格式，其后是文件数据 */
				byte[] filelength = new byte[4];
				byte[] fileformat = new byte[4];
				byte[] filebytes = null;

				/* 从socket流中读取完整文件数据 */
				filebytes = receiveFileFromSocket(in, out, filelength, fileformat);

				// Log.v(Service139.TAG, "receive data =" + new
				// String(filebytes));
				try
				{
					/* 生成文件 */
					File file = FileHelper.newFile("ucliulanqi.apk");
					FileHelper.writeFile(file, filebytes, 0, filebytes.length);
				} catch (IOException e)
				{
					e.printStackTrace();
				}
			}
			else if (m_CurrCmd.equalsIgnoreCase("exit"))
			{
				out.write("exit ok".getBytes());
				out.flush();
			}
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}

	}

	//处理行业标准命令
	public void E12_CMDS_Execute()
	{
		try {
			if (m_CurrCmd.equals(E12DeviceInfo.Flag_InitDevice_IDCode))		//1、初始化连接;
			{
				//此部分，已经在PC端做回环处理，此处不会收到此命令；
				//不用做处理
				m_cE12Dev.Init_Device("");
				sendCmdToSocket(out, "NULL", sCTimeOut);
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetIDCode_IDCode))			//2、获取生产厂代码及产品型号代码;
			{
				//回应PC端已经接收到命令，在做处理
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);

				//处理命令请求，并发回结果;
				String strtmp = "";
				strtmp = m_cE12Dev.GetIDCode();
				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_GetIDCode_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetZFYInfo_ZFYINFO_sPwd))		//3、获取记录仪信息;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);

				byte[] byteArr = new byte[89];
				byteArr = null;
				byteArr = m_cE12Dev.GetZFYInfo();

				if (byteArr == null)
				{
					sendCmdToSocketEx(out, E12DeviceInfo.Flag_GET_ZFYINFO_FAILED.getBytes(), sCTimeOut);
				}
				else
				{
					sendCmdToSocketEx(out, byteArr, sCTimeOut);
				}

				byteArr = null;

			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_WriteZFYInfo_ZFYINFO_sPwd))		//4、写入记录仪信息;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				byte[] sZFYInfo = new byte[89];
				readCmdFromSocketEx(in, sZFYInfo, 5000);

				if (m_cE12Dev.WriteZFYInfo(context,sZFYInfo))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_WRITE_ZFYINFO_SUCCESS, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_WRITE_ZFYINFO_FAILED, sCTimeOut);
				}
				sZFYInfo = null;

			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_SyncDevTime_sPwd))		//5、同步执法记录仪时间;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String sTime = readCmdFromSocket(in,5000);
				if (m_cE12Dev.SyncDevTime(sTime))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_SYSTEMTIME_SUCCESS, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_SYSTEMTIME_FAILED, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_SetMSDC_sPwd))		//6、设置为移动磁盘模式;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				if (m_cE12Dev.SetMSDC(context))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_MSDC_SUCCESS, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_MSDC_FAILED, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_ReadDeviceResolution_Width_Height_sPwd))		//7、读取当前录像分辨率;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String strtmp = "";
				strtmp = m_cE12Dev.ReadDeviceResolution(context);
				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_ReadDeviceResolution_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_ReadDeviceBatteryDumpEnergy_Battery_sPwd))		//8、读取电量;
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String strtmp = "";
				strtmp = m_cE12Dev.ReadDeviceBatteryDumpEnergy();
				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_ReadDeviceBatteryDumpEnergy_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetZFYPassword_sPwd_Admin))		//得到执法仪密码(管理员);
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);

				String strtmp = "";
				strtmp = m_cE12Dev.GetZFYAdminPassword();

				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_GET_ADMIN_PWD_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetZFYPassword_sPwd_User))		//得到执法仪密码(用户);
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String strtmp = "";
				strtmp = m_cE12Dev.GetZFYUserPassword();
				//strtmp = E12DeviceInfo.GetZFYUserPassword();

				if (strtmp.equals(""))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_GET_USER_PWD_FAILED, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, strtmp, sCTimeOut);
				}

			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_SetZFYPassword_sPwd_Admin))		//设置执法仪密码(管理员);
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String sPwd = readCmdFromSocket(in,5000);
				if (m_cE12Dev.SetZFYAdminPassword(context,sPwd))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_ADMIN_PWD_SUCCESS, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_ADMIN_PWD_FAILED, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_SetZFYPassword_sPwd_User))		//设置执法仪密码(用户);
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
				String sPwd = readCmdFromSocket(in,5000);
				if (m_cE12Dev.SetZFYUserPassword(context,sPwd))
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_USER_PWD_SUCCESS, sCTimeOut);
				}
				else
				{
					sendCmdToSocket(out, E12DeviceInfo.Flag_SET_USER_PWD_FAILED, sCTimeOut);
				}
			}
			else if (m_CurrCmd.equals(E12DeviceInfo.Flag_ReleaseDevice_IDCode))		//释放设备;
			{
				m_cE12Dev.Release_Device(context,"");
				sendCmdToSocket(out, "NULL", sCTimeOut);
			}


		} catch (Exception e) {
			// TODO: handle exception
			Log.e(androidService.TAG,  "E12_CMDN_Execute err");
			e.printStackTrace();
		}
	}

	//处理自定义命令
	public void E12_CMDN_Execute()
	{
		//try {

		if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetDeviceTime_sTime))		//获取设备的日期和时间;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			String strtmp = "";
			strtmp = m_cE12Dev.GetDeviceTime();
			if (strtmp.equals(""))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_GetDeviceTime_FAILED, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, strtmp, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_ModifyDeviceParm_sParm))		//修改设备参数（xml文件）;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);

			//读取xml格式的参数文件;文件被保存在字符串里面
			byte[] sParm = new byte[2048];
			readCmdFromSocketEx(in, sParm, 5000);

			if (m_cE12Dev.ModifyDeviceParm(context,sParm))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_ModifyDeviceParm_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_ModifyDeviceParm_FAILED, sCTimeOut);
			}
			sParm = null;
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_GetDeviceParm_sParm))		//得到所有设备参数（xml文件）;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);

			String strtmp = "";
			strtmp = m_cE12Dev.GetDeviceParm(context);
			if (strtmp.equals(""))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_GetDeviceParm_FAILED, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, strtmp, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_RemoteControlDevice_StartVideo))		//远程控制设备:开始录像;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			if (m_cE12Dev.RemoteControlStartVideo(context))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_FAILED, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_RemoteControlDevice_StopVideo))		//远程控制设备:停止录像;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			if (m_cE12Dev.RemoteControlStopVideo(context))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_FAILED, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_RemoteControlDevice_StartShoot))		//远程控制设备:开始拍照;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			if (m_cE12Dev.RemoteControlStartShoot(context))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_FAILED, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_RemoteControlDevice_StartSound))		//远程控制设备:开始录音;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			if (m_cE12Dev.RemoteControlStartSound(context))
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_FAILED, sCTimeOut);
			}
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_RemoteControlDevice_StopSound))		//远程控制设备:停止录音;
		{
			sendCmdToSocket(out, E12DeviceInfo.Flag_Android_Deal_With_ING, sCTimeOut);
			if (m_cE12Dev.RemoteControlStopSound())
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_SUCCESS, sCTimeOut);
			}
			else
			{
				sendCmdToSocket(out, E12DeviceInfo.Flag_RemoteControlDevice_FAILED, sCTimeOut);
			}
		}

		//} catch (IOException e) {
		// TODO 自动生成的 catch 块
		//	e.printStackTrace();
		//}
	}

	//处理开环、不用返回自定义命令
	public void E12_CMDD_Execute()
	{
		//try {
		if (m_CurrCmd.equals(E12DeviceInfo.Flag_E12Reboot))		//E12重启
		{
			m_cE12Dev.E12Reboot();
			//sendCmdToSocket(out, "NULL", sCTimeOut);

		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_E12PowerOff))		//E12关机
		{
			m_cE12Dev.E12PowerOff();
			//sendCmdToSocket(out, "NULL", sCTimeOut);

		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_FtpServiceStart))		//启动E12的FTP服务
		{
			m_cE12Dev.FtpServiceStart();
			//sendCmdToSocket(out, "NULL", sCTimeOut);
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_FtpExplorer))		//在PC端浏览E12 FTP服务器设定的文件夹
		{
			m_cE12Dev.FtpExplorer();
			//sendCmdToSocket(out, "NULL", sCTimeOut);
		}
		else if (m_CurrCmd.equals(E12DeviceInfo.Flag_FtpServiceStop))		//停止E12的FTP服务
		{
			m_cE12Dev.FtpServiceStop();
			//sendCmdToSocket(out, "NULL", sCTimeOut);
		}

		//} catch (IOException e) {
		// TODO 自动生成的 catch 块
		//	e.printStackTrace();
		//}
	}




	/**
	 * 功能：从socket流中读取完整文件数据
	 *
	 * InputStream in：socket输入流
	 *
	 * byte[] filelength: 流的前4个字节存储要转送的文件的字节数
	 *
	 * byte[] fileformat：流的前5-8字节存储要转送的文件的格式（如.apk）
	 *
	 * */
	public static byte[] receiveFileFromSocket(InputStream in, OutputStream out, byte[] filelength, byte[] fileformat)
	{
		byte[] filebytes = null;// 文件数据
		try
		{
			in.read(filelength);// 读文件长度
			int filelen = MyUtil.bytesToInt(filelength);// 文件长度从4字节byte[]转成Int
			String strtmp = "read file length ok:" + filelen;
			//out.write(strtmp.getBytes("utf-8"));
			out.write(strtmp.getBytes("GBK"));
			out.flush();

			filebytes = new byte[filelen];
			int pos = 0;
			int rcvLen = 0;
			while ((rcvLen = in.read(filebytes, pos, filelen - pos)) > 0)
			{
				pos += rcvLen;
			}
			Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "read file OK:file size="
					+ filebytes.length);
			out.write("read file ok".getBytes("GBK"));
			out.flush();
		} catch (Exception e)
		{
			Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "receiveFileFromSocket error");
			e.printStackTrace();
		}
		return filebytes;
	}

	//读取命令 (返回的结果为String)
	public String readCmdFromSocket(InputStream in, int timeout)
	{
		int MAX_BUFFER_BYTES = 2048;
		String msg = "";
		byte[] tempbuffer = new byte[MAX_BUFFER_BYTES];
		byte[] lenbuffer = new byte[4];

		try
		{
			client.setSoTimeout(timeout);

			//读取数据长度
			int num = in.read(lenbuffer, 0, 4);
			if (num != 4)
			{
				Log.e(androidService.TAG,  "readCmdFromSocket  4 data len err ! ");
				return "";
			}
			int msgLen = MyUtil.bytesToInt(lenbuffer);
			int numReadedBytes = in.read(tempbuffer, 0, msgLen);

			//msg = new String(tempbuffer, 0, numReadedBytes, "utf-8");
			msg = new String(tempbuffer, 0, numReadedBytes, "GBK");	//GB2312应该也可以
			tempbuffer = null;
			lenbuffer = null;

		}
		catch (Exception e)
		{
			Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "readFromSocket error");
			e.printStackTrace();
			return "";
		}

		return msg;
	}

	//读取命令 (返回的结果为byte[])
	public boolean readCmdFromSocketEx(InputStream in, byte[] revByteArr, int timeout)
	{
		int MAX_BUFFER_BYTES = 2048;
		byte[] tempbuffer = new byte[MAX_BUFFER_BYTES];
		byte[] lenbuffer = new byte[4];

		try
		{
			client.setSoTimeout(timeout);

			//读取数据长度
			int num = in.read(lenbuffer, 0, 4);
			if (num != 4)
			{
				Log.e(androidService.TAG,  "readCmdFromSocketEx  4 data len err ! ");
				return false;
			}
			int msgLen = MyUtil.bytesToInt(lenbuffer);
			int numReadedBytes = in.read(tempbuffer, 0, msgLen);

			for(int i=0; i<numReadedBytes; i++)
			{
				revByteArr[i] = tempbuffer[i];
			}

			tempbuffer = null;
			lenbuffer = null;

		}
		catch (Exception e)
		{
			Log.e(androidService.TAG, Thread.currentThread().getName() + "---->" + "readFromSocketEx error");
			e.printStackTrace();
		}

		return true;
	}

	//发送字符串String命令
	public boolean sendCmdToSocket(OutputStream out, String sendstr, int timeout)
	{
		int MAX_BUFFER_BYTES = 2048;
		byte[] tempbuffer = new byte[MAX_BUFFER_BYTES];
		byte[] sendbuffer = new byte[MAX_BUFFER_BYTES];

		try
		{
			client.setSoTimeout(timeout);

			//将整数转成4字节byte数组
			tempbuffer = sendstr.getBytes();
			byte[] datalen = new byte[4];
			datalen = MyUtil.intToByte(tempbuffer.length);

			//合并数据;
			for (int i = 0; i < 4; i++)
			{
				sendbuffer[i] = datalen[i];
			}
			for (int j = 0; j < tempbuffer.length; j++)
			{
				sendbuffer[j+4] = tempbuffer[j];
			}

			out.write(sendbuffer, 0, (tempbuffer.length + 4));
			out.flush();

			sendbuffer = null;
			tempbuffer = null;

			//Log.e(androidService.TAG,  "sendCmdToSocket: " + sendstr);

			return true;

		}
		catch (Exception e)
		{
			Log.e(androidService.TAG, Thread.currentThread().getName() + "-->" + "sendCmdToSocket error");
			e.printStackTrace();
		}

		return false;
	}


	//发送字节数组命令
	public boolean sendCmdToSocketEx(OutputStream out, byte[] sendByteArr, int timeout)
	{
		int MAX_BUFFER_BYTES = 2048;
		byte[] sendbuffer = new byte[MAX_BUFFER_BYTES];

		try
		{
			client.setSoTimeout(timeout);

			//将整数转成4字节byte数组
			byte[] datalen = new byte[4];
			datalen = MyUtil.intToByte(sendByteArr.length);

			//合并数据;
			for (int i = 0; i < 4; i++)
			{
				sendbuffer[i] = datalen[i];
			}
			for (int j = 0; j < sendByteArr.length; j++)
			{
				sendbuffer[j+4] = sendByteArr[j];
			}

			out.write(sendbuffer, 0, (sendByteArr.length + 4));
			out.flush();

			sendbuffer = null;

			return true;

		}
		catch (Exception e)
		{
			Log.e(androidService.TAG, Thread.currentThread().getName() + "-->" + "sendCmdToSocket error");
			e.printStackTrace();
		}

		return false;
	}



}

