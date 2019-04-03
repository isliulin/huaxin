package gbpe.policeass.usbconnection;

import android.content.Context;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

public class E12DeviceInfo {

	//定义构成规则：发送的是一个字符串;
	//1、标识符：E12;(3个字节)
	//2、命令或数据：CMD 或 DAT  ;(3个字节)
	//3、命令或数据的实体;(（1024-6）个字节以内)

	//********************************************************************************//
	//通信标识符定义：

	//PC端向E12发送的命令(行标要求函数);
	//PC端向E12发送的命令(行标要求函数);
	//定义构成规则：发送的是一个字符串;
	//1、标识符：E12;(3个字节)
	//2、命令或数据：CMD 或 DAT  ;(3个字节)
	//3、命令或数据的实体;(（1024-6）个字节以内)

	//********************************************************************************//
	//通信标识符定义：


	//定义构成规则：发送的是一个字符串;
	//1、标识符：E12;(3个字节)
	//2、命令或数据：CMD 或 DAT  ;(3个字节)
	//3、命令或数据的实体;(（1024-6）个字节以内)

	//********************************************************************************//
	//通信标识符定义：

	//PC端向E12发送的命令(行标要求函数);
	public static final String Flag_InitDevice_IDCode							= "E12CMDS001";
	public static final String Flag_ReleaseDevice_IDCode						= "E12CMDS002";
	public static final String Flag_SyncDevTime_sPwd							= "E12CMDS003";
	public static final String Flag_GetZFYInfo_ZFYINFO_sPwd					= "E12CMDS004";
	public static final String Flag_WriteZFYInfo_ZFYINFO_sPwd					= "E12CMDS005";
	public static final String Flag_ReadDeviceResolution_Width_Height_sPwd	= "E12CMDS006";
	public static final String Flag_ReadDeviceBatteryDumpEnergy_Battery_sPwd	= "E12CMDS007";
	public static final String Flag_SetMSDC_sPwd								    = "E12CMDS008";
	public static final String Flag_GetIDCode_IDCode							= "E12CMDS009";
	public static final String Flag_GetZFYPassword_sPwd_Admin					= "E12CMDS010";
	public static final String Flag_GetZFYPassword_sPwd_User					= "E12CMDS011";
	public static final String Flag_SetZFYPassword_sPwd_Admin					= "E12CMDS012";
	public static final String Flag_SetZFYPassword_sPwd_User					= "E12CMDS013";

	//E12向PC端返回的结果（行标要求函数配套）
	public static final String Flag_CONNECT_SUCCESS						     	= "E12DATR101";
	public static final String Flag_CONNECT_FAILED								= "E12DATR100";
	public static final String Flag_ReleaseDevice_SUCCESS						= "E12DATR111";
	public static final String Flag_ReleaseDevice_FAILED						= "E12DATR110";
	public static final String Flag_SET_SYSTEMTIME_SUCCESS						= "E12DATR001";
	public static final String Flag_SET_SYSTEMTIME_FAILED						= "E12DATR000";
	public static final String Flag_GET_ZFYINFO_SUCCESS					    	= "E12DATR121";
	public static final String Flag_GET_ZFYINFO_FAILED							= "E12DATR120";
	public static final String Flag_WRITE_ZFYINFO_SUCCESS						= "E12DATR021";
	public static final String Flag_WRITE_ZFYINFO_FAILED						= "E12DATR020";
	public static final String Flag_ReadDeviceResolution_SUCCESS				= "E12DATR131";
	public static final String Flag_ReadDeviceResolution_FAILED				= "E12DATR130";
	public static final String Flag_ReadDeviceBatteryDumpEnergy_SUCCESS		= "E12DATR141";
	public static final String Flag_ReadDeviceBatteryDumpEnergy_FAILED		= "E12DATR140";
	public static final String Flag_SET_MSDC_SUCCESS							= "E12DATR011";
	public static final String Flag_SET_MSDC_FAILED						    	= "E12DATR010";
	public static final String Flag_GetIDCode_SUCCESS							= "E12DATR151";
	public static final String Flag_GetIDCode_FAILED							= "E12DATR150";
	public static final String Flag_GET_ADMIN_PWD_SUCCESS						= "E12DATR031";
	public static final String Flag_GET_ADMIN_PWD_FAILED						= "E12DATR030";
	public static final String Flag_GET_USER_PWD_SUCCESS						= "E12DATR041";
	public static final String Flag_GET_USER_PWD_FAILED					    	= "E12DATR040";
	public static final String Flag_SET_ADMIN_PWD_SUCCESS						= "E12DATR161";
	public static final String Flag_SET_ADMIN_PWD_FAILED						= "E12DATR160";
	public static final String Flag_SET_USER_PWD_SUCCESS						= "E12DATR171";
	public static final String Flag_SET_USER_PWD_FAILED						    = "E12DATR170";

	//E12向PC端返回的结果（非标）
	public static final String Flag_GetDeviceTime_SUCCESS						= "E12DATR051";
	public static final String Flag_GetDeviceTime_FAILED						= "E12DATR050";
	public static final String Flag_ModifyDeviceParm_SUCCESS					= "E12DATR061";
	public static final String Flag_ModifyDeviceParm_FAILED					= "E12DATR060";
	public static final String Flag_GetDeviceParm_SUCCESS						= "E12DATR081";
	public static final String Flag_GetDeviceParm_FAILED						= "E12DATR080";
	public static final String Flag_RemoteControlDevice_SUCCESS				= "E12DATR071";
	public static final String Flag_RemoteControlDevice_FAILED					= "E12DATR070";
	public static final String Flag_GetGpsPositionInfo_SUCCESS					= "E12DATR201";
	public static final String Flag_GetGpsPositionInfo_FAILED					= "E12DATR200";

	//PC端向E12发送的命令(非标);
	public static final String Flag_GetDeviceTime_sTime					    	= "E12CMDN030";
	public static final String Flag_ModifyDeviceParm_sParm						= "E12CMDN031";
	public static final String Flag_GetDeviceParm_sParm					    	= "E12CMDN033";
	public static final String Flag_RemoteControlDevice_sCtrlCmd				= "E12CMDN032";
	public static final String Flag_RemoteControlDevice_StartVideo				= "E12CMDN301";
	public static final String Flag_RemoteControlDevice_StopVideo				= "E12CMDN300";
	public static final String Flag_RemoteControlDevice_StartShoot				= "E12CMDN311";
	public static final String Flag_RemoteControlDevice_StopShoot				= "E12CMDN310";
	public static final String Flag_RemoteControlDevice_StartSound				= "E12CMDN321";
	public static final String Flag_RemoteControlDevice_StopSound				= "E12CMDN320";

	//PC端向E12发送的命令(开环控制，无需返回);
	public static final String Flag_E12Reboot									= "E12CMDD001";
	public static final String Flag_FtpServiceStart						    	= "E12CMDD002";
	public static final String Flag_FtpExplorer							    	= "E12CMDD003";
	public static final String Flag_FtpServiceStop								= "E12CMDD004";
	public static final String Flag_E12PowerOff							    	= "E12CMDD005";

	//文件传输;
	//public static final String Flag_File_To_PC								= "E12DATF001";
	//public static final String Flag_File_To_E12								= "E12DATF000";

	//中间状态:Android 处理中的反馈信号;
	public static final String Flag_Android_Deal_With_ING						= "E12DATR880";
	//完成状态:Android 处理命令完成的反馈信号;
	public static final String Flag_Android_Do_Command_FINISH					= "E12DATR881";

	//网络连接额外测试标识
	public static final String Flag_Test_Data_Transmission_Cmd					= "E12CMDF808";
	public static final String Flag_Test_Data_Transmission_OK					= "E12CMDF809";
	public static final String Flag_GetGpsPositionInfo_XYZ						= "E12CMDF011";


	//********************************************************************************//
	//E12结构体参数定义：
	String cSerial;			//执法记录仪产品序号，不可为空;
	String userNo;			//执法记录仪使用者警号，不可为空;*
	String userName;		//执法记录仪使用者姓名，管理平台使用警号关联时可为空;
	String unitNo;			//执法记录仪使用者单位编号，管理平台使用警号关联时可为空;
	String unitName;		//执法记录仪使用者单位名称，管理平台使用警号关联时可为空;
	String IDCode;			//生产代码及产品型号代码;
	int  Resolution_Width = 0;	//分辨率的宽度;
	int  Resolution_Height = 0; //分辨率的高度;
	int  Battery = 0;			//电池电量;

	int bUserType = 0;		//登录的用户类型 0：管理员，1：普通用户;
	String UserPwd;			//用户密码;
	String AdminPwd;		//管理员密码;

	int iResultFlag;		//执法仪通信错误代码标识;(专门赋值为：enum _RESULT_FLAG里面的变量)


	//********************************************************************************//
	//相关的操作及方法：


	//*************************** DLL 接口函数 *****************************************//
	//1、初始化连接;
	public static boolean Init_Device(String IDCode)
	{
		Log.e(androidService.TAG,  "Init_Device->执行到你的接口");
		boolean state = false;
		//不用处理
		state = E12DeviceInfoUtil.InitDevice(IDCode);
 		return state;
	}

	//2、获取生产厂代码及产品型号代码;
	public static String GetIDCode()
	{
		Log.e(androidService.TAG,  "GetIDCode->执行到你的接口");
		String IDCode = "";
		/*************** 开始你的方法  *************/
		//提示：下面调用你的函数体，得到生产厂代码及产品型号代码，并赋值给IDCode;

		IDCode = E12DeviceInfoUtil.GetIDCode();

		/*************** 方法结束  *************/
		return IDCode;
	}

	//3、获取记录仪信息;
	public static byte[] GetZFYInfo()
	{
		Log.e(androidService.TAG,  "GetZFYInfo->执行到你的接口");
		/*************** 开始你的方法  *************/

		byte [] sZFYInfo = E12DeviceInfoUtil.GetZFYInfo();

		/*************** 方法结束  *************/
		//sZFYInfo：89个字节传在一起的字符串
		//顺序：产品序号（7）+ 警号（6）+ 用户姓名（32）+ 单位编号（12）+ 单位名称（32）；
		return sZFYInfo;
	}

	//4、写入记录仪信息;
	public static boolean WriteZFYInfo(Context context,byte [] sZFYInfo)
	{
		Log.e(androidService.TAG,  "WriteZFYInfo->执行到你的接口");
		boolean state  = false;
		//操作成功返回 true; 否则返回 state;
		/*************** 开始你的方法  *************/


		state = E12DeviceInfoUtil.WriteZFYInfo(context,sZFYInfo);

		/*************** 方法结束  *************/
		//提示：对参数sZFYInfo进行截取，格式如上函数
		return state;
	}

	//5、同步执法记录仪时间;
	public  static boolean SyncDevTime(String sTime)
	{
		Log.e(androidService.TAG,  "SyncDevTime->执行到你的接口");
		boolean state = false;
		//操作成功返回 true; 否则返回 state;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.SyncDevTime(sTime);

		/*************** 方法结束  *************/
		//sTime: 参数时间格式为 2017-05-15 8:00:00
		return state;
	}

	//6、设置为移动磁盘模式;
	public static boolean SetMSDC(Context context)
	{
		Log.e(androidService.TAG,  "SetMSDC->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//你程序可以调用此状态，一旦执行了此函数，安卓将进入FTP服务器模式，-
		//你预览界面作停止或退出程序等等，节省更多系统资源提供给FTP

		state = E12DeviceInfoUtil.SetMSDC(context);
		/*************** 方法结束  *************/
		return state;
	}

	//7、读取当前录像分辨率;
	public static String ReadDeviceResolution(Context context)
	{
 		String sResolution = "";
		/************** 开始你的方法  *************/
		Log.e(androidService.TAG,  "ReadDeviceResolution->执行到你的接口");
		sResolution = E12DeviceInfoUtil.ReadDeviceResolutio(context);

		/*************** 方法结束  *************/
		//sResolution格式为：宽*高
		return sResolution;
	}

	//8、读取电量;
	public static String ReadDeviceBatteryDumpEnergy()
	{
		Log.e(androidService.TAG,  "ReadDeviceBatteryDumpEnergy->执行到你的接口");
		String sBattery = "";
		/************** 开始你的方法  *************/
		sBattery = E12DeviceInfoUtil.ReadDeviceBatteryDumpEnergy();

		/*************** 方法结束  *************/
		//sBattery为一个整数转为字符串
		return sBattery;
	}

	//9得到执法仪管理员密码;
	public static  String GetZFYAdminPassword()
	{
		Log.e(androidService.TAG,  "GetZFYAdminPassword->执行到你的接口");
		String sPwd = "";
		/************** 开始你的方法  *************/

		sPwd = E12DeviceInfoUtil.GetAdminPasswd();

		/*************** 方法结束  *************/
		return sPwd;
	}

	//10得到执法仪用户密码;
	public static String GetZFYUserPassword()
	{
		Log.e(androidService.TAG,  "GetZFYUserPassword->执行到你的接口");
		String sPwd = "";
		/************** 开始你的方法  *************/
		sPwd = E12DeviceInfoUtil.GetUserPasswd();

		/*************** 方法结束  *************/
		return sPwd;
	}

	//11.设置执法仪管理员密码;
	public static boolean SetZFYAdminPassword(Context context,String sPwd)
	{
		Log.e(androidService.TAG,  "SetZFYAdminPassword->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.SetAdminPasswd(context,sPwd);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;

	}

	//12.设置执法仪用户密码;
	public static boolean SetZFYUserPassword(Context context,String sPwd)
	{
		Log.e(androidService.TAG,  "SetZFYUserPassword->执行到你的接口");
		boolean state = false;

		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.SetUserPasswd(context,sPwd);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;

	}


	//13.释放设备;
	public static  boolean Release_Device(Context context,String IDCode)
	{
		Log.e(androidService.TAG,  "Release_Device->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理
		state = E12DeviceInfoUtil.Release_Device(context,IDCode);

		/*************** 方法结束  *************/
		return state;
	}

	//14.E12重启
	public  static boolean E12Reboot()
	{
		Log.e(androidService.TAG,  "E12Reboot->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理
		state = E12DeviceInfoUtil.E12Reboot();

		/*************** 方法结束  *************/
		return state;
	}

	//15 E12关机
	public boolean E12PowerOff()
	{
		Log.e(androidService.TAG,  "E12PowerOff->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理

 		/*************** 方法结束  *************/
		return true;
	}

	//16.启动E12的FTP服务
	public static boolean FtpServiceStart()
	{
		Log.e(androidService.TAG,  "FtpServiceStart->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理
		state = E12DeviceInfoUtil.FtpServiceStart();

		/*************** 方法结束  *************/
		return state;
	}

	//17.在PC端浏览E12 FTP服务器设定的文件夹
	public static boolean FtpExplorer()
	{
		Log.e(androidService.TAG,  "FtpExplorer->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理
		state = E12DeviceInfoUtil.FtpExplorer();

		/*************** 方法结束  *************/
		return state;
	}

	//18.停止E12的FTP服务
	public static boolean FtpServiceStop()
	{
		Log.e(androidService.TAG,  "FtpServiceStop->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		//不做处理，或调用此状态为其它做处理
		state = E12DeviceInfoUtil.FtpServiceStop();

		/*************** 方法结束  *************/
		return state;
	}

	//19.获取设备的日期和时间;
	public static String GetDeviceTime()
	{
		Log.e(androidService.TAG,  "GetDeviceTime->执行到你的接口");
		String sDateTime = "";
		/************** 开始你的方法  *************/

		sDateTime = E12DeviceInfoUtil.GetDeviceTime();
 		/*************** 方法结束  *************/
		return sDateTime;
	}

	//20.修改设备参数;
	public static boolean ModifyDeviceParm(Context context,byte[] sParm)
	{
		Log.e(androidService.TAG,  "ModifyDeviceParm->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.ModifyDeviceParm(context,sParm);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}

	//21.修改设备参数;
	public String GetDeviceParm(Context context)
	{
		Log.e(androidService.TAG,  "GetDeviceParm->执行到你的接口");
		String sParm = "";
		/************** 开始你的方法  *************/
		sParm = E12DeviceInfoUtil.GetDeviceParm(context);
		/*************** 方法结束  *************/
		//sParm：为xml文件的String形式
		return sParm;
	}
	//22.远程控制设备：开始录像;
	public static boolean RemoteControlStartVideo(Context context)
	{
		Log.e(androidService.TAG,  "RemoteControlStartVideo->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.RemoteControlStartVideo(context);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}

	//23.远程控制设备：停止录像;
	public static boolean RemoteControlStopVideo(Context context)
	{
		Log.e(androidService.TAG,  "RemoteControlStopVideo->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/

		state = E12DeviceInfoUtil.RemoteControlStopVideo(context);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}

	//24.远程控制设备：开始拍照;
	public static boolean RemoteControlStartShoot(Context context)
	{
		Log.e(androidService.TAG,  "RemoteControlStartShoot->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.RemoteControlStartShoot(context);

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}

	//25.远程控制设备：开始录音;
	public static boolean RemoteControlStartSound(Context context)
	{
		Log.e(androidService.TAG,  "RemoteControlStartSound->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/

		state = E12DeviceInfoUtil.RemoteControlStartSound(context);
		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}

	//26.远程控制设备：停止录音;
	public static boolean RemoteControlStopSound()
	{
		Log.e(androidService.TAG,  "RemoteControlStopSound->执行到你的接口");
		boolean state = false;
		/************** 开始你的方法  *************/
		state = E12DeviceInfoUtil.RemoteControlStopSound();

		/*************** 方法结束  *************/
		//操作成功返回 true; 否则返回 state;
		return state;
	}



	//27. 获取当前E12 GPS位置信息;
	public String GetGpsPositionInfo(Context context)
	{
		Log.e(androidService.TAG,  "GetGpsPositionInfo->执行到你的接口");
		//sPositionInfo格式为：x*y  这个再简单一点，只要经度和纬度，其它不要
		String sPositionInfo = "";
		/************** 开始你的方法  *************/

		sPositionInfo = E12DeviceInfoUtil.GetGpsPositionInfo(context);
		/*************** 方法结束  *************/
		//sPositionInfo格式为：x*y*z
		return sPositionInfo;
	}
}
