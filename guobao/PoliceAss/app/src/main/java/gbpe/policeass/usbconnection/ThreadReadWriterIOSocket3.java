package gbpe.policeass.usbconnection;

/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/4/21 08:50
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/4/21 08:50
 * 
 * 描述：客户端和PC端数据传输和交互的线程类
 * =====================================================================================
 */

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import android.content.Context;
import android.text.TextUtils;
import android.util.Log;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.fragments.SettingFragment;
import gbpe.policeass.utils.FileHelper;
import gbpe.policeass.utils.MyUtil;
import gbpe.policeass.utils.SettingUtils;


public class ThreadReadWriterIOSocket3 implements Runnable {
    private Socket client;
    private Context context;
    public int sCTimeOut = 3000;	//发送命令时间超时

    public ThreadReadWriterIOSocket3(Context context, Socket client) {
        this.client = client;
        this.context = context;
    }

    @Override
    public void run() {
        Log.d("chl", "a client has connected to server!");
        BufferedOutputStream out;
        BufferedInputStream in;
        try {
			/* PC端发来的数据msg */
            String currCMD = "";
            out = new BufferedOutputStream(client.getOutputStream());
            in = new BufferedInputStream(client.getInputStream());
            androidService.ioThreadFlag = true;


            while (androidService.ioThreadFlag) {
                try {
                    if (!client.isConnected()) {
                        break;
                    }
					/* 接收PC发来的数据 */
                    Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "will read......");
					/* 读操作命令 */
                    currCMD = E12DeviceInfoUtil.readCMDFromSocket(in);

                    Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "**currCMD ==== " + currCMD);

                    switch (currCMD){
                        case E12DeviceInfo.Flag_InitDevice_IDCode:{
                            E12DeviceInfo.Init_Device(null);
                            break;
                        }
                        case E12DeviceInfo.Flag_ReleaseDevice_IDCode:{
                            E12DeviceInfo.Release_Device(context,null);
                            break;
                        }
                        case E12DeviceInfo.Flag_GetIDCode_IDCode:{
                            E12DeviceInfo.GetIDCode();
                            break;
                        }
                        case E12DeviceInfo.Flag_GetZFYInfo_ZFYINFO_sPwd:{

                            E12DeviceInfo.GetZFYInfo();
                            break;
                        }
                        case E12DeviceInfo.Flag_WriteZFYInfo_ZFYINFO_sPwd:{/* 准备接收文件数据 */

                            E12DeviceInfo.WriteZFYInfo(context,null);
                            break;
                        }
                        case E12DeviceInfo.Flag_SyncDevTime_sPwd:{

                            E12DeviceInfo.SyncDevTime("");
                            break;
                        }
                        case E12DeviceInfo.Flag_SetMSDC_sPwd:{
                            E12DeviceInfo.SetMSDC(context );
                        }
                        case E12DeviceInfo.Flag_ReadDeviceResolution_Width_Height_sPwd:{
                            E12DeviceInfo.ReadDeviceResolution(context);
                            break;
                        }
                        case E12DeviceInfo.Flag_ReadDeviceBatteryDumpEnergy_Battery_sPwd:{
                            E12DeviceInfo.ReadDeviceBatteryDumpEnergy();
                            break;
                        }
                        case E12DeviceInfo.Flag_GetZFYPassword_sPwd_Admin:{
                            E12DeviceInfo.GetZFYAdminPassword();
                            break;
                        }
                        case E12DeviceInfo.Flag_GetZFYPassword_sPwd_User:{
                            E12DeviceInfo.GetZFYUserPassword();
                            break;
                        }
                        case E12DeviceInfo.Flag_SetZFYPassword_sPwd_Admin:{
                            E12DeviceInfo.SetZFYAdminPassword(context,"");
                            break;
                        }
                        case E12DeviceInfo.Flag_SetZFYPassword_sPwd_User:{

                            E12DeviceInfo.SetZFYUserPassword(context,null);
                            break;
                        }
                        case E12DeviceInfo.Flag_GetDeviceTime_sTime:{
                            E12DeviceInfo.GetDeviceTime();
                            break;
                        }
                        case E12DeviceInfo.Flag_ModifyDeviceParm_sParm:{
                            E12DeviceInfo.ModifyDeviceParm(context,null);
                            break;
                        }
                        case E12DeviceInfo.Flag_RemoteControlDevice_sCtrlCmd:{
                             break;
                        }
                        case E12DeviceInfo.Flag_RemoteControlDevice_StartVideo:{
                            E12DeviceInfo.RemoteControlStartVideo(context);
                            break;
                        }
                        case E12DeviceInfo.Flag_RemoteControlDevice_StopVideo:{
                            E12DeviceInfo.RemoteControlStopVideo(context);
                            break;
                        }
                        case E12DeviceInfo.Flag_RemoteControlDevice_StartShoot:{
                            E12DeviceInfo.RemoteControlStartShoot(context);
                            break;
                        }

                        case E12DeviceInfo.Flag_RemoteControlDevice_StartSound:{
                            E12DeviceInfo.RemoteControlStartSound(context);
                            break;
                        }
                        case E12DeviceInfo.Flag_RemoteControlDevice_StopSound:{
                            E12DeviceInfo.RemoteControlStopSound( );
                            break;
                        }
                        case E12DeviceInfo.Flag_E12Reboot:{
                            E12DeviceInfo.E12Reboot();
                            break;
                        }
                        case E12DeviceInfo.Flag_FtpServiceStart:{
                            E12DeviceInfo.FtpServiceStart( );
                            break;
                        }
                        case E12DeviceInfo.Flag_FtpExplorer:{
                            E12DeviceInfo.FtpExplorer( );
                            break;
                        }
                        case E12DeviceInfo.Flag_FtpServiceStop:{
                            E12DeviceInfo.FtpServiceStop( );
                            break;
                        }
                        default:
                            break;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            out.close();
            in.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (client != null) {
                    Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "client.close()");
                    client.close();
                }
            } catch (IOException e) {
                Log.e(androidService.TAG, Thread.currentThread().getName() + "---->" + "read write error333333");
                e.printStackTrace();
            }
        }
    }
//    /**
//     * 功能：从socket流中读取完整文件数据
//     *
//     * InputStream in：socket输入流
//     *
//     * byte[] filelength: 流的前4个字节存储要转送的文件的字节数
//     *
//     * byte[] fileformat：流的前5-8字节存储要转送的文件的格式（如.apk）
//     */
//    public static byte[] receiveFileFromSocket(InputStream in, OutputStream out, byte[] filelength, byte[] fileformat) {
//        byte[] filebytes = null;// 文件数据
//        try {
//            in.read(filelength);// 读文件长度
//            int filelen = MyUtil.bytesToInt(filelength);// 文件长度从4字节byte[]转成Int
//            String strtmp = "read file length ok:" + filelen;
//            out.write(strtmp.getBytes("utf-8"));
//            out.flush();
//
//            filebytes = new byte[filelen];
//            int pos = 0;
//            int rcvLen = 0;
//            while ((rcvLen = in.read(filebytes, pos, filelen - pos)) > 0) {
//                pos += rcvLen;
//            }
//            Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "read file OK:file size="
//                    + filebytes.length);
//            out.write("read file ok".getBytes("utf-8"));
//            out.flush();
//        } catch (Exception e) {
//            Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "receiveFileFromSocket error");
//            e.printStackTrace();
//        }
//        return filebytes;
//    }
//
//    /* 读取命令 */
//    public String readCMDFromSocket(InputStream in) {
//        int MAX_BUFFER_BYTES = 2048;
//        String msg = "";
//        byte[] tempbuffer = new byte[MAX_BUFFER_BYTES];
//        try {
//            int numReadedBytes = in.read(tempbuffer, 0, tempbuffer.length);
//            msg = new String(tempbuffer, 0, numReadedBytes, "utf-8");
//            tempbuffer = null;
//        } catch (Exception e) {
//            Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "readFromSocket error");
//            e.printStackTrace();
//        }
//        // Log.v(Service139.TAG, "msg=" + msg);
//        return msg;
//    }
//
//
//
//
//    /**
//     * 初始化设备
//     * @param out
//     * @return
//     */
//    public String InitDevice(BufferedOutputStream out){
//        try {
//            out.write("yes".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//    /**
//     * 获取生产厂代码及产品型号代码
//     * @param out
//     * @return
//     */
//    public String GetIDCode(BufferedOutputStream out){
////        String devNumber = SettingUtils.getStrValues(context,"devNumber","456");
////        if (!TextUtils.isEmpty(devNumber)){
////            try {
////                out.write(devNumber.getBytes());
////                out.flush();
////            } catch (IOException e) {
////                e.printStackTrace();
////            }
////        }
//        try {
//            out.write("GetIDCode sucess!".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *获取执法仪相关信息
//     * @param out
//     * @return
//     */
//    public String GetZFYInfo(BufferedOutputStream out){
//        String devNumber = SettingUtils.getStrValues(context,"devNumber","456");
//        String policeNumber = SettingUtils.getStrValues(context,"policeNumber","123456");
//        if (!TextUtils.isEmpty(devNumber)&&!TextUtils.isEmpty(policeNumber)){
//            try {
//                out.write((devNumber+":"+policeNumber+"").getBytes());
//                out.flush();
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
//        }
//        return "";
//    }
//
//    /**
//     *写入记录仪信息
//     * @param in
//     * @param out
//     * @return
//     */
//    public String WriteZFYInfo(BufferedInputStream in,BufferedOutputStream out){
//
//        try {
//            out.write("service receive OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//
//						/* 接收文件数据，4字节文件长度，4字节文件格式，其后是文件数据 */
//        byte[] filelength = new byte[4];
//        byte[] fileformat = new byte[4];
//        byte[] filebytes = null;
//
//						/* 从socket流中读取完整文件数据 */
//        filebytes = receiveFileFromSocket(in, out, filelength, fileformat);
//
//        // Log.v(Service139.TAG, "receive data =" + new
//        // String(filebytes));
//        try {
//            /* 生成文件 */
//            File file = FileHelper.newFile("ucliulanqi.apk");
//            if (file.exists()){
//                FileHelper.writeFile(file, filebytes, 0, filebytes.length);
//            }else {
//                Log.e(androidService.TAG, "文件不存在 " );
//            }
//
//        } catch (IOException e)
//        {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//
//    /**
//     *同步执法记录仪时间
//     * @param out
//     * @return
//     */
//    public String SyncDevTime(BufferedOutputStream out){
//        try {
//            out.write("OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *设置为移动磁盘模式
//     * @param out
//     * @return
//     */
//    public String SetMSDC(BufferedOutputStream out){
//        try {
//            out.write("OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *读取当前录像分辨率
//     * @param out
//     * @return
//     */
//    public String ReadDeviceResolutio(BufferedOutputStream out){
//        String video_size = SettingUtils.getStrValues(context, SettingFragment.KEY_PREF_VIDEO_SIZE, "");
//        if (!TextUtils.isEmpty(video_size)){
//            try {
//                out.write((video_size+"").getBytes());
//                out.flush();
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
//        }
//        return "";
//    }
//
//    /**
//     *读取电量
//     * @param out
//     */
//    public String ReadDeviceBatteryDumpEnergy(BufferedOutputStream out){
//        String powerLevel = Setup.battery_level;
//        if (!TextUtils.isEmpty(powerLevel)){
//            try {
//                out.write((powerLevel+"").getBytes());
//                out.flush();
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
//        }
//        return "";
//    }
//
//    /**
//     *获取管理员密码
//     */
//    public String GetAdminPasswd(BufferedOutputStream out){
//        try {
//            out.write("GetAdminPasswd OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *获取用户密码
//     */
//    public String GetUserPasswd(BufferedOutputStream out){
//        try {
//            out.write("GetUserPasswd OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *设置管理员密码
//     */
//    public String SetAdminPasswd(BufferedOutputStream out){
//        try {
//            out.write("SetAdminPasswd OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    /**
//     *设置用户员密码
//     */
//    public String SetUserPasswd(BufferedOutputStream out){
//        try {
//            out.write("SetUserPasswd OK".getBytes());
//            out.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        return "";
//    }

}