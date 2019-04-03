package gbpe.policeass.modle;

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2017/4/17 19:37
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2017/4/17 19:37
 *
 * 描述：usb和PC通信常量的定义
 * =====================================================================================
 */
public class UsbConsts {
//
//    publicpublic  static final String Flag_InitDevice_IDCode					 	    =     "E12C01";
//    publicpublic  static final String Flag_ReleaseDevice_IDCode						=     "E12C02";//断开设备连接
//
//    publicpublic  static final String Flag_SyncDevTime_sPwd							=     "E12C03";
//    publicpublic  static final String Flag_GetZFYInfo_ZFYINFO_sPwd					=     "E12C04";
//    publicpublic  static final String Flag_WriteZFYInfo_ZFYINFO_sPwd					=     "E12C05";
//    publicpublic  static final String Flag_ReadDeviceResolution_Width_Height_sPwd	=     "E12C06";
//    publicpublic  static final String Flag_ReadDeviceBatteryDumpEnergy_Battery_sPwd	=     "E12C07";
//    publicpublic  static final String Flag_SetMSDC_sPwd							    	=     "E12C08";
//    publicpublic  static final String Flag_GetIDCode_IDCode							=     "E12C09";
//
//    publicpublic  static final String Flag_GetZFYPassword_sPwd_Admin					=     "E12C10";
//    publicpublic  static final String Flag_GetZFYPassword_sPwd_User					=     "E12C11";
//    publicpublic  static final String Flag_SetZFYPassword_sPwd_Admin					=     "E12C12";
//    publicpublic  static final String Flag_SetZFYPassword_sPwd_User					=     "E12C13";

//    publicpublic  static final String Flag_SET_SYSTEMTIME_SUCCESS						=     "E12R01";
//    publicpublic  static final String Flag_SET_SYSTEMTIME_FAILED						=     "E12R02";
//    publicpublic  static final String Flag_SET_MSDC_SUCCESS							=     "E12R03";
//    publicpublic  static final String Flag_SET_MSDC_FAILED							    =     "E12R04";
//    publicpublic  static final String Flag_WRITE_ZFYINFO_SUCCESS						=     "E12R05";
//    publicpublic  static final String Flag_WRITE_ZFYINFO_FAILED						=     "E12R06";
//    publicpublic  static final String Flag_SET_ADMIN_PWD_SUCCESS						=     "E12R07";
//    publicpublic  static final String Flag_SET_ADMIN_PWD_FAILED						=     "E12R08";
//    publicpublic  static final String Flag_SET_USER_PWD_SUCCESS						=     "E12R09";
//    publicpublic  static final String Flag_SET_USER_PWD_FAILED					    	=     "E12R10";
//
//    publicpublic  static final String Flag_Client_Send_OK								=     "E12SOK";
//    publicpublic  static final String Flag_Client_Receive_OK							=     "E12ROK";





 //=============================================================================================================

    //定义构成规则：发送的是一个字符串;
    //1、标识符：E12;(3个字节)
    //2、命令或数据：CMD 或 DAT  ;(3个字节)
    //3、命令或数据的实体;(（1024-6）个字节以内)

    //********************************************************************************//
    //通信标识符定义：

    //PC端向E12发送的命令(行标要求函数);
   public  static final String Flag_InitDevice_IDCode							= "E12CMDS001";
   public  static final String Flag_ReleaseDevice_IDCode						= "E12CMDS002";
   public  static final String Flag_SyncDevTime_sPwd							= "E12CMDS003";
   public  static final String Flag_GetZFYInfo_ZFYINFO_sPwd					= "E12CMDS004";
   public  static final String Flag_WriteZFYInfo_ZFYINFO_sPwd					= "E12CMDS005";
   public  static final String Flag_ReadDeviceResolution_Width_Height_sPwd	= "E12CMDS006";
   public  static final String Flag_ReadDeviceBatteryDumpEnergy_Battery_sPwd	= "E12CMDS007";
   public  static final String Flag_SetMSDC_sPwd							     	= "E12CMDS008";
   public  static final String Flag_GetIDCode_IDCode							= "E12CMDS009";
   public  static final String Flag_GetZFYPassword_sPwd_Admin					= "E12CMDS010";
   public  static final String Flag_GetZFYPassword_sPwd_User					= "E12CMDS011";
   public  static final String Flag_SetZFYPassword_sPwd_Admin					= "E12CMDS012";
   public  static final String Flag_SetZFYPassword_sPwd_User					= "E12CMDS013";

    //E12向PC端返回的结果（行标要求函数配套）
   public  static final String Flag_CONNECT_SUCCESS						    	= "E12DATR101";
   public  static final String Flag_CONNECT_FAILED								= "E12DATR100";
   public  static final String Flag_ReleaseDevice_SUCCESS						= "E12DATR111";
   public  static final String Flag_ReleaseDevice_FAILED						= "E12DATR110";
   public  static final String Flag_SET_SYSTEMTIME_SUCCESS						= "E12DATR001";
   public  static final String Flag_SET_SYSTEMTIME_FAILED						= "E12DATR000";
   public  static final String Flag_GET_ZFYINFO_SUCCESS					    	= "E12DATR121";
   public  static final String Flag_GET_ZFYINFO_FAILED							= "E12DATR120";
   public  static final String Flag_WRITE_ZFYINFO_SUCCESS						= "E12DATR021";
   public  static final String Flag_WRITE_ZFYINFO_FAILED						= "E12DATR020";
   public  static final String Flag_ReadDeviceResolution_SUCCESS				= "E12DATR131";
   public  static final String Flag_ReadDeviceResolution_FAILED				= "E12DATR130";
   public  static final String Flag_ReadDeviceBatteryDumpEnergy_SUCCESS		= "E12DATR141";
   public  static final String Flag_ReadDeviceBatteryDumpEnergy_FAILED		= "E12DATR140";
   public  static final String Flag_SET_MSDC_SUCCESS							= "E12DATR011";
   public  static final String Flag_SET_MSDC_FAILED						    	= "E12DATR010";
   public  static final String Flag_GetIDCode_SUCCESS							= "E12DATR151";
   public  static final String Flag_GetIDCode_FAILED							= "E12DATR150";
   public  static final String Flag_GET_ADMIN_PWD_SUCCESS						= "E12DATR031";
   public  static final String Flag_GET_ADMIN_PWD_FAILED						= "E12DATR030";
   public  static final String Flag_GET_USER_PWD_SUCCESS						= "E12DATR041";
   public  static final String Flag_GET_USER_PWD_FAILED					    	= "E12DATR040";
   public  static final String Flag_SET_ADMIN_PWD_SUCCESS						= "E12DATR161";
   public  static final String Flag_SET_ADMIN_PWD_FAILED						= "E12DATR160";
   public  static final String Flag_SET_USER_PWD_SUCCESS						= "E12DATR171";
   public  static final String Flag_SET_USER_PWD_FAILED					    	= "E12DATR170";

    //E12向PC端返回的结果（非标）
   public  static final String Flag_GetDeviceTime_SUCCESS						= "E12DATR051";
   public  static final String Flag_GetDeviceTime_FAILED						= "E12DATR050";
   public  static final String Flag_ModifyDeviceParm_SUCCESS					= "E12DATR061";
   public  static final String Flag_ModifyDeviceParm_FAILED					= "E12DATR060";
   public  static final String Flag_RemoteControlDevice_SUCCESS				= "E12DATR071";
   public  static final String Flag_RemoteControlDevice_FAILED					= "E12DATR070";

    //PC端向E12发送的命令(非标);
   public  static final String Flag_GetDeviceTime_sTime					    	= "E12CMDN030";
   public  static final String Flag_ModifyDeviceParm_sParm						= "E12CMDN031";
   public  static final String Flag_RemoteControlDevice_sCtrlCmd				= "E12CMDN032";
   public  static final String Flag_RemoteControlDevice_StartVideo			    = "E12CMDN301";
   public  static final String Flag_RemoteControlDevice_StopVideo				= "E12CMDN300";
   public  static final String Flag_RemoteControlDevice_StartShoot			    = "E12CMDN311";
   public  static final String Flag_RemoteControlDevice_StopShoot				= "E12CMDN310";
   public  static final String Flag_RemoteControlDevice_StartSound			    = "E12CMDN321";
   public  static final String Flag_RemoteControlDevice_StopSound				= "E12CMDN320";

    //PC端向E12发送的命令(开环控制，无需返回);
   public  static final String Flag_E12Reboot									= "E12CMDD001";
   public  static final String Flag_FtpServiceStart					        	= "E12CMDD002";
   public  static final String Flag_FtpExplorer							    	= "E12CMDD003";
   public  static final String Flag_FtpServiceStop								= "E12CMDD004";

    //文件传输;
    //static final String Flag_File_To_PC								= "E12DATF001";
    //static final String Flag_File_To_E12								= "E12DATF000";

    //中间状态:Android 处理中的反馈信号;
   public  static final String Flag_Android_Deal_With_ING						= "E12DATR880";
    //完成状态:Android 处理命令完成的反馈信号;
   public  static final String Flag_Android_Do_Command_FINISH					= "E12DATR881";

}




