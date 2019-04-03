package gbpe.policeass.usbconnection;

import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;
import org.greenrobot.eventbus.EventBus;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.AnyEvent;
import gbpe.policeass.modle.PhotographEvent;
import gbpe.policeass.modle.RemoteStartSoundRecord;
import gbpe.policeass.modle.RemoteStopSoundRecord;
import gbpe.policeass.utils.CameraHandlerThread;
import gbpe.policeass.utils.MyUtil;
import gbpe.policeass.utils.NetUtils;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.utils.StringUtil;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.utils.gps.GpsUtils;
import gbpe.policeass.utils.xmlutil.E12DeviceParam;
import gbpe.policeass.utils.xmlutil.XMLStrUtil;

/**
 * ===========================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/5/16 14:00
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/5/16 14:00
 * 
 * 描述：
 * =====================================================================================
 */
public class E12DeviceInfoUtil {
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

    /* 读取命令 */
    public static String readCMDFromSocket(InputStream in) {
        int MAX_BUFFER_BYTES = 2048;
        String msg = "";
        byte[] tempbuffer = new byte[MAX_BUFFER_BYTES];
        try {
            int numReadedBytes = in.read(tempbuffer, 0, tempbuffer.length);
            msg = new String(tempbuffer, 0, numReadedBytes, "utf-8");
            tempbuffer = null;
        } catch (Exception e) {
            Log.v(androidService.TAG, Thread.currentThread().getName() + "---->" + "readFromSocket error");
            e.printStackTrace();
        }
        // Log.v(Service139.TAG, "msg=" + msg);
        return msg;
    }

//=========================================================================================================================================
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
    public String readCmdFromSocket(Socket client,InputStream in, int timeout)
    {
        int MAX_BUFFER_BYTES = 1024;
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
        }

        return msg;
    }

    //读取命令 (返回的结果为byte[])
    public boolean readCmdFromSocketEx(Socket client,InputStream in, byte[] revByteArr, int timeout)
    {
        int MAX_BUFFER_BYTES = 1024;
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
    public boolean sendCmdToSocket(Socket client,OutputStream out, String sendstr, int timeout)
    {
        int MAX_BUFFER_BYTES = 1024;
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
    public boolean sendCmdToSocketEx(Socket client,OutputStream out, byte[] sendByteArr, int timeout)
    {
        int MAX_BUFFER_BYTES = 1024;
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


//====================================================================================以下是工具方法========================================================================================= =

    /**
     * 初始化设备
     * @return
     */
    public static boolean InitDevice(String IDCode){  //---暂未处理
        //不用处理
        Log.e(androidService.TAG,  "InitDevice->执行到我的方法");
        return false;
    }
    /**
     * 获取生产厂代码及产品型号代码
     * @return
     */
    public static String GetIDCode(){               //---暂未处理
        Log.e(androidService.TAG,  "ReadDeviceResolutio->执行到我的方法");
        String IDCode = "ABCDE";

        return IDCode;
    }

    /**
     *获取执法仪相关信息
     * @return
     */
    public  static byte [] GetZFYInfo(){
        Log.e(androidService.TAG,  "GetZFYInfo->执行到我的方法");
         byte[] sZFYInfo = new byte[89];

        //返回码，遇到中文时，采用“GBK”编码形式;
        /*************** 开始你的方法  *************/
        byte[] cSerial  = new byte[7];
        byte[] userNo   = new byte[6];
        byte[] userName = new byte[32];
        byte[] unitNo   = new byte[12];
        byte[] unitName = new byte[32];

        String s1 = Setup.devNum;
        String s2 = Setup.policeNumber;
        String s3 = Setup.usrName;
        String s4 = Setup.unitNumber;
        String s5 = Setup.unitName;

        try {
            cSerial  = s1.getBytes("GBK");
            userNo 	 = s2.getBytes("GBK");
            userName = s3.getBytes("GBK");
            unitNo   = s4.getBytes("GBK");
            unitName = s5.getBytes("GBK");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        for (int i = 0; i < cSerial.length; i++)
        {
            sZFYInfo[i] = cSerial[i];
        }
        for (int i = 0; i < userNo.length; i++)
        {
            sZFYInfo[i+7] = userNo[i];
        }
        for (int i = 0; i < userName.length; i++)
        {
            sZFYInfo[i+13] = userName[i];
        }
        for (int i = 0; i < unitNo.length; i++)
        {
            sZFYInfo[i+45] = unitNo[i];
        }
        for (int i = 0; i < unitName.length; i++)
        {
            sZFYInfo[i+57] = unitName[i];
        }
        /*************** 方法结束  *************/
        //sZFYInfo：89个字节串在一起的字符串
        //顺序：产品序号（7）+ 警号（6）+ 用户姓名（32）+ 单位编号（12）+ 单位名称（32）；
        Log.e(androidService.TAG,  "GetZFYInfo-> cSerial:"+Setup.devNum+"userNo:"+Setup.policeNumber
                +"userName:"+Setup.usrName+"unitNo"+Setup.unitNumber+"unitName"+Setup.unitName);
        return sZFYInfo;
    }

//    /**
//     *写入记录仪信息
//     * @return
//     */
//    public static  boolean WriteZFYInfo(Context context,byte [] sZFYInfo) {
//        Log.e(androidService.TAG, "WriteZFYInfo->执行到我的方法");
//        //处理命令请求，并发回结果;
//        if (sZFYInfo.length <= 0)
//        {
//            return false;
//        } else
//        {
//          try
//          {
//            byte[] temp1 = new byte[7];
//            byte[] temp2 = new byte[6];
//            byte[] temp3 = new byte[32];
//            byte[] temp4 = new byte[12];
//            byte[] temp5 = new byte[32];
//            for (int i = 0; i < 7; i++) {
//                temp1[i] = sZFYInfo[i];
//            }
//
//                Setup.devNum = new String(temp1, 0, temp1.length, "utf-8");
//                SettingUtils.setStrValues(context, "devNumber", Setup.devNum);
//                for (int i = 0; i < 6; i++) {
//                    temp2[i] = sZFYInfo[i + 7];
//                }
//                Setup.policeNumber = new String(temp2, 0, temp2.length, "utf-8");
//                SettingUtils.setStrValues(context, "policeNumber", Setup.policeNumber);
//                for (int i = 0; i < 32; i++) {
//                    temp3[i] = sZFYInfo[i + 14];
//                }
//                Setup.usrName = new String(temp3, 0, temp3.length, "utf-8");
//                SettingUtils.setStrValues(context, "usrName", Setup.usrName);
//
//                for (int i = 0; i < 12; i++) {
//                    temp4[i] = sZFYInfo[i + 46];
//                }
//                Setup.unitNumber = new String(temp4, 0, temp4.length, "utf-8");
//                SettingUtils.setStrValues(context, "unitNumber", Setup.usrName);
//                for (int i = 0; i < 32; i++) {
//                    temp5[i] = sZFYInfo[i + 57];
//                }
//                Setup.unitName = new String(temp5, 0, temp5.length, "utf-8");
//                SettingUtils.setStrValues(context, "unitName", Setup.usrName);
//            } catch (UnsupportedEncodingException e) {
//                e.printStackTrace();
//            }
//
//            if (TextUtils.isEmpty(Setup.devNum) || TextUtils.isEmpty(Setup.policeNumber) || TextUtils.isEmpty(Setup.usrName) ||
//                    TextUtils.isEmpty(Setup.unitNumber) || TextUtils.isEmpty(Setup.unitName)) {
//                return false;
//            }
//            Log.e(androidService.TAG, "WriteZFYInfo-> cSerial:" + Setup.devNum + "userNo:" + Setup.policeNumber
//                    + "userName:" + Setup.usrName + "unitNo" + Setup.unitNumber + "unitName" + Setup.unitName);
//            EventBus.getDefault().post(new AnyEvent("update"));
//            return true;
//        }
//    }


    /**
     *写入记录仪信息
     * @return
     */
    public static  boolean WriteZFYInfo(Context context,byte [] sZFYInfo) {
        Log.e(androidService.TAG, "WriteZFYInfo come in !!!");

        Log.e(androidService.TAG,"=============================>WriteZFYInfo"+sZFYInfo.length);


        if (sZFYInfo.length <= 0)
        {
            Log.e(androidService.TAG, "数据格式错误");
            return false;
        }
        else
        {
            try
            {

                Log.e(androidService.TAG, "数据正常");
                byte[] devNum = new byte[8];
                byte[] policeNumber = new byte[7];
                byte[] usrName = new byte[33];
                byte[] unitNum = new byte[13];
                byte[] unitName = new byte[33];

                for (int i = 0; i < 7; i++)
                {
                    devNum[i] = sZFYInfo[i];
                }
                Log.e(androidService.TAG, "=======>开始解析字符串2");
                String sdevNum = new String(devNum, 0, devNum.length, "GBK");
                int pos1 = sdevNum.indexOf("\0");
                Setup.devNum = sdevNum.substring(0,pos1);

             //   Setup.devNum = new String(devNum,0,devNum.length-1,"GBK");

                Log.e(androidService.TAG, "=======>解析字符串完成2");
                SettingUtils.setStrValues(context, "devNumber", Setup.devNum);
                Log.e(androidService.TAG, "WriteZFYInfo sdevNum ->" + Setup.devNum);

                for (int i = 0; i < 6; i++)
                {
                    policeNumber[i] = sZFYInfo[i + 7];
                }
                String spoliceNumber = new String(policeNumber, 0, policeNumber.length, "GBK");
                int pos2 = spoliceNumber.indexOf("\0");
                Setup.policeNumber = spoliceNumber.substring(0,pos2);
                SettingUtils.setStrValues(context, "policeNumber", Setup.policeNumber);
                Log.e(androidService.TAG, "WriteZFYInfo spoliceNumber ->" + Setup.policeNumber);

                for (int i = 0; i < 32; i++)
                {
                    usrName[i] = sZFYInfo[i + 13];
                }
                String susrName = new String(usrName, 0, usrName.length, "GBK");
                int pos = susrName.indexOf("\0");
                Setup.usrName = "";
                Setup.usrName = susrName.substring(0,pos);
                SettingUtils.setStrValues(context, "usrName", Setup.usrName);
                Log.e(androidService.TAG, "WriteZFYInfo susrName ->" +  Setup.usrName);

                for (int i = 0; i < 12; i++)
                {
                    unitNum[i] = sZFYInfo[i + 45];
                }
                String unitNumber = new String(unitNum, 0, unitNum.length, "GBK");
                int pos3 = unitNumber.indexOf("\0");
                Setup.unitNumber = unitNumber.substring(0,pos3);
                SettingUtils.setStrValues(context, "unitNumber", Setup.unitNumber);
                Log.e(androidService.TAG, "WriteZFYInfo unitNumber ->" +  Setup.unitNumber);

                for (int i = 0; i < 32; i++)
                {
                    unitName[i] = sZFYInfo[i + 57];
                }
                String sunitName = new String(unitName, 0, unitName.length, "GBK");
                int pos4 = sunitName.indexOf("\0");
                Setup.unitName = sunitName.substring(0,pos4);
                SettingUtils.setStrValues(context, "unitName", Setup.unitName);
                Log.e(androidService.TAG, "WriteZFYInfo unitName ->" +  Setup.unitName);
                EventBus.getDefault().post(new AnyEvent("update"));

            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
                Log.e(androidService.TAG, "WriteZFYInfo try Err");
                return false;
            }

            return true;
        }
    }


    /**
     *同步执法记录仪时间
     * @return
     */
    public static  boolean SyncDevTime(String time){
        Log.e(androidService.TAG,  "SyncDevTime->执行到我的方法");
            if (TextUtils.isEmpty(time)){
                return false;
            }else {
                String [] times = time.split(" ");
                String [] sDate = times[0].split("-");
                String [] sTime = times[1].split(":");
                TimeUtils.onTimeSet2(Integer.parseInt(sDate[0]),Integer.parseInt(sDate[1]),Integer.parseInt(sDate[2]),
                        Integer.parseInt(sTime[0]),Integer.parseInt(sTime[1]),Integer.parseInt(sTime[2]));
                Log.e(androidService.TAG,  "SyncDevTime->"+time);
                return true;
            }

    }

    /**
     *设置为移动磁盘模式
     * @return
     */
    public  static boolean SetMSDC(Context context){ //
        Log.e(androidService.TAG,  "SetMSDC->执行到我的方法");
        //你程序可以调用此状态，一旦执行了此函数，安卓将进入FTP服务器模式，-
        //你预览界面作停止或退出程序等等，节省更多系统资源提供给FTP

        //2017/6/12
        //这部分，你程序要做相应的修改，发送广播：guobao.intent.action.E12_SET_MSDC，可参考下面举例
		//举例：
		Intent intent = new Intent();
        intent.setAction("guobao.intent.action.E12_SET_MSDC");
        intent.putExtra("guobao", "E12_SET_MSDC");
        context.sendBroadcast(intent);
        /**
         * 变为U盘之后，本程序已经处于暂停状态了，所以，对本程序的任何操作都是不起作用
         */
        Log.e(androidService.TAG,  "SetMSDC");
        return true;
    }




    /**
     *读取当前录像分辨率
     * @return
     */
    public static  String ReadDeviceResolutio(Context context){
        Log.e(androidService.TAG,  "ReadDeviceResolutio->执行到我的方法");
       // String video_size = SettingUtils.getStrValues(context, SettingFragment.KEY_PREF_VIDEO_SIZE, "");
         String video_size = Setup.VideoWidth+"*"+Setup.VideoHight;
        if (!TextUtils.isEmpty(video_size)){
           // video_size =  video_size.split("x")[0]+"*"+video_size.split("x")[1];
            Log.e(androidService.TAG,  "ReadDeviceResolutio->"+video_size);
            return video_size;
           }
       return null;
    }

    /**
     *读取电量
     */
    public  static String ReadDeviceBatteryDumpEnergy(){
        Log.e(androidService.TAG,  "ReadDeviceBatteryDumpEnergy->执行到我的方法");
         String powerLevel = Setup.battery_level;
        if (!TextUtils.isEmpty(powerLevel)) {
            Log.e(androidService.TAG,  "ReadDeviceBatteryDumpEnergy->"+powerLevel);
            return powerLevel;
        }
         return null;
    }

    /**
     *获取管理员密码
     */
    public static  String GetAdminPasswd(){
        Log.e(androidService.TAG,  "GetAdminPasswd->执行到我的方法");
            if (!TextUtils.isEmpty(Setup.adminPasswd)) {
                Log.e(androidService.TAG,  "GetAdminPasswd->"+Setup.adminPasswd);
                return Setup.adminPasswd;
            }

         return null;
    }

    /**
     *获取用户密码
     */
    public  static String GetUserPasswd(){
        Log.e(androidService.TAG,  "GetUserPasswd->执行到我的方法");
            if (!TextUtils.isEmpty(Setup.usrPassword)) {
                Log.e(androidService.TAG,  "GetUserPasswd->"+Setup.usrPassword);
                return Setup.usrPassword;
            }
          return null;
    }

    /**
     *设置管理员密码
     */
    public  static boolean SetAdminPasswd(Context context,String sPwd){
        Log.e(androidService.TAG,  "SetAdminPasswd->执行到我的方法");
            if (!TextUtils.isEmpty(sPwd)){
                Setup.adminPasswd = sPwd;
                SettingUtils.setStrValues(context,"AdminPassword",Setup.adminPasswd);
                Log.e(androidService.TAG,  "SetAdminPasswd->"+sPwd);
                return true;
            }
         return false;
    }

    /**
     *设置用户员密码
     */
    public static  boolean SetUserPasswd(Context context,String sPwd){
        Log.e(androidService.TAG,  "SetUserPasswd->执行到我的方法");
            if (!TextUtils.isEmpty(sPwd)){
               Setup.usrPassword = sPwd;
                SettingUtils.setStrValues(context,"usrPassword", Setup.usrPassword);
                Log.e(androidService.TAG,  "SetUserPasswd->"+sPwd);
                return true;
            }
         return false;
    }



    //释放设备;
    public static  boolean Release_Device(Context context,String IDCode)      //-暂未处理
    {
        Log.e(androidService.TAG,  "Release_Device->执行到我的方法");
         /************** 开始你的方法  *************/
        //不做处理，或调用此状态为其它做处理
            /**
             * 检测USB是否连接
             */
            if (!SDCardUtils.usbIsConn(context)){
                Log.e(androidService.TAG,  "Release_Device");
                return true;
            }
        /*************** 方法结束  *************/
        return false;
    }

    //E12重启
    public  static boolean E12Reboot()            //--暂未处理
    {
        Log.e(androidService.TAG,  "E12Reboot->执行到我的方法");
         /************** 开始你的方法  *************///(比如文件保存，或其它问题)
        //不做处理，或调用此状态为其它做处理

       // RebootUtil.reoot(context);

        Log.e(androidService.TAG,  "E12Reboot");
        /*************** 方法结束  *************/
        return false;
    }

    //启动E12的FTP服务
    public static boolean FtpServiceStart() //--暂未处理
    {
        Log.e(androidService.TAG,  "FtpServiceStart->执行到我的方法");
        /************** 开始你的方法  *************/
        //不做处理，或调用此状态为其它做处理

        Log.e(androidService.TAG,  "FtpServiceStart");

        /*************** 方法结束  *************/
        return false;
    }

    //在PC端浏览E12 FTP服务器设定的文件夹
    public static boolean FtpExplorer() //--暂未处理
    {
        Log.e(androidService.TAG,  "FtpExplorer->执行到我的方法");
        /************** 开始你的方法  *************/
        //不做处理，或调用此状态为其它做处理
        Log.e(androidService.TAG,  "FtpExplorer");

        /*************** 方法结束  *************/
        return false;
    }

    //停止E12的FTP服务
    public static boolean FtpServiceStop() //--暂未处理
    {
        ;  Log.e(androidService.TAG,  "FtpServiceStop->执行到我的方法");
        /************** 开始你的方法  *************/
        //不做处理，或调用此状态为其它做处理

        Log.e(androidService.TAG,  "FtpServiceStop");
        /*************** 方法结束  *************/
        return false;
    }

    //获取设备的日期和时间;
    public static String GetDeviceTime()
    {
        Log.e(androidService.TAG,  "GetDeviceTime->执行到我的方法");
        String sDateTime = "";
        /************** 开始你的方法  *************/
         sDateTime = TimeUtils.milliseconds2String3(System.currentTimeMillis());
        Log.e(androidService.TAG,  "GetDeviceTime->"+sDateTime);
        /*************** 方法结束  *************/
        return sDateTime;
    }



     public static boolean isVideoRecording;
    //远程控制设备：开始录像;
    public static boolean RemoteControlStartVideo(Context context)
    {
       Log.e(androidService.TAG,  "RemoteControlStartVideo->执行到我的方法");

        if (CameraHandlerThread.camera ==null){
            return false;
        }
        if (!isVideoRecording)
        {
            isVideoRecording = true;
            VideoRecord(context);
            return true;
        }
        /*************** 方法结束  *************/
        //操作成功返回 true; 否则返回 state;
        Log.e(androidService.TAG,  "RemoteControlStartVideo");
        return false;
    }


    //远程控制设备：停止录像;
    public static boolean RemoteControlStopVideo(Context context)
    {
        Log.e(androidService.TAG,  "RemoteControlStopVideo->执行到我的方法");
         /************** 开始你的方法  *************/
        if (isVideoRecording)
        {
            isVideoRecording = false;
            VideoRecord(context);
            return true;
        }

        /*************** 方法结束  *************/
        //操作成功返回 true; 否则返回 state;
        Log.e(androidService.TAG,  "RemoteControlStopVideo");
        return false;
    }



    //远程控制设备：开始拍照;
    public static boolean RemoteControlStartShoot(final Context context)
    {
        Log.e(androidService.TAG,  "RemoteControlStartShoot->执行到我的方法");
        /************** 开始你的方法  *************/

        if (CameraHandlerThread.camera==null){
            return false;
        }
        EventBus.getDefault().post(new PhotographEvent("RemoteControlStartShoot"));

        /*************** 方法结束  *************/
        //操作成功返回 true; 否则返回 state;
        Log.e(androidService.TAG,  "RemoteControlStartShoot");
        return true;
    }

    //远程控制设备：开始录音;
    public static boolean RemoteControlStartSound(Context context)
    {
        Log.e(androidService.TAG,  "RemoteControlStartSound->执行到我的方法");
        /************** 开始你的方法  *************/
        EventBus.getDefault().post(new RemoteStartSoundRecord("RemoteStartSoundRecord"));

        /*************** 方法结束  *************/
        //操作成功返回 true; 否则返回 state;
        Log.e(androidService.TAG,  "RemoteControlStartSound");
        return true;
    }

    //远程控制设备：停止录音;
    public static boolean RemoteControlStopSound()
    {
        Log.e(androidService.TAG,  "RemoteControlStopSound->执行到我的方法");
        /************** 开始你的方法  *************/
        EventBus.getDefault().post(new RemoteStopSoundRecord("RemoteStartSoundRecord"));
        /*************** 方法结束  *************/
        //操作成功返回 true; 否则返回 state;
        Log.e(androidService.TAG,  "RemoteControlStopSound");
        return true;
    }


    //修改设备参数;
    public static boolean ModifyDeviceParm(Context context,byte[]  sParm) {
        Log.e(androidService.TAG, "ModifyDeviceParm->执行到我的方法");
        /************** 开始你的方法  *************/
        //读取xml格式的参数文件;文件被保存在字符串里面
        if (sParm.length > 0)
        {
            /*****************************修改设备参数的方法******************************/
                     /************** 开始你的方法  *************/
                try
                {
                    String msg = new String(sParm, 0, sParm.length, "utf-8");
                    List<E12DeviceParam> params = XMLStrUtil.getParaValue(msg);
                    E12DeviceParam param = params.get(0);
                    setDeviceParams(context, param);
                    return true;
                } catch (Exception e)
                {
                    e.printStackTrace();
                    Log.e(androidService.TAG, "ModifyDeviceParm to String Exception !!");
                }
        }
           Log.e(androidService.TAG, "ModifyDeviceParm->" + sParm);
        return false;
    }
    //获得设备参数;
    public static String GetDeviceParm(Context context)
    {
        Log.e(androidService.TAG,  "GetDeviceParm->执行到我的方法");
        String sParm = "";//sParm：为xml文件的String形式
         /************** 开始你的方法  **********/
        sParm = XMLStrUtil.getXMLString2(context);
        Log.e(androidService.TAG,  "GetDeviceParm->"+sParm);
        /*************** 方法结束  *************/
        return sParm;
    }

    /**
     * 设置执法仪参数
     * @param param
     */
    private static void setDeviceParams(Context context,E12DeviceParam param) {

        Log.e("guobao","=====================>读取服务端发来的数据==============================================================================================================>");

        SettingUtils.setStrValues(context,"video_segmented", String.valueOf(param.getSegmentedVideo()));
        Log.e("guobao","=====================>video_segmented==>  "+String.valueOf(param.getSegmentedVideo()));
        SettingUtils.setStrValues(context,"video_rate", String.valueOf(param.getVideoBitrate()));
        Log.e("guobao","=====================>video_rate==>   "+String.valueOf(param.getVideoBitrate()));

        SettingUtils.setStrValues(context,"video_outputformat", String.valueOf(param.getVideoOutputFormat()));
        Log.e("guobao","=====================>video_outputformat==>   "+String.valueOf(param.getVideoOutputFormat()));

        SettingUtils.setStrValues(context,"video_prerecord", String.valueOf(param.getPrerecord()));
        Log.e("guobao","=====================>video_prerecord==>   "+String.valueOf(param.getPrerecord()));

        SettingUtils.setStrValues(context,"picture_outformat", String.valueOf(param.getPicOutputFormat()));
        Log.e("guobao","=====================>picture_outformat==>   "+String.valueOf(param.getPicOutputFormat()));

        SettingUtils.setStrValues(context,"audio_samplerate", String.valueOf(param.getAudioSimpleRate()));
        Log.e("guobao","=====================>audio_samplerate==>   "+String.valueOf(param.getAudioSimpleRate()));

        SettingUtils.setStrValues(context,"audio_maxduration", String.valueOf(param.getAudioMaxDuration()));
        Log.e("guobao","=====================>audio_maxduration==>   "+String.valueOf(param.getAudioMaxDuration()));

        SettingUtils.setStrValues(context,"video_maxduration", String.valueOf(param.getVideoMaxDuration()));
        Log.e("guobao","=====================>video_maxduration==>   "+String.valueOf(param.getAudioMaxDuration()));


        SettingUtils.setStrValues(context,"audio_outputformat", String.valueOf(param.getAudioOutputFormat()));
        Log.e("guobao","=====================>audio_outputformat==>   "+String.valueOf(param.getAudioOutputFormat()));

        SettingUtils.setBoolValues(context,"key_tone", StringUtil.switchState(String.valueOf(param.getKeyTone())));
        Log.e("guobao","=====================>key_tone==>   "+String.valueOf(param.getKeyTone()));
        Log.e("guobao","=====================>key_tone==>   "+ StringUtil.switchState(String.valueOf(param.getKeyTone())));

        SettingUtils.setStrValues(context,"policeNumber", String.valueOf(param.getPoliceNumber()));
        Log.e("guobao","=====================>policeNumber==>   "+String.valueOf(param.getPoliceNumber()));

        SettingUtils.setStrValues(context,"devNum", String.valueOf(param.getDevNum()));
        Log.e("guobao","=====================>devNum==>   "+String.valueOf(param.getDevNum()));

      //  SettingUtils.setStrValues(context,"usrName", String.valueOf(param.));
        SettingUtils.setStrValues(context,"picture_size", String.valueOf(param.getPictureSize()));
        Log.e("guobao","=====================>picture_size==>   "+String.valueOf(param.getPictureSize()));


        SettingUtils.setStrValues(context,"fileview_type", String.valueOf(param.getFileView()));
        Log.e("guobao","=====================>fileview_type==>   "+String.valueOf(param.getFileView()));

        SettingUtils.setStrValues(context,"video_maxduration", String.valueOf(param.getMaxDuration()));
        Log.e("guobao","=====================>video_maxduration==>   "+String.valueOf(param.getMaxDuration()));

        SettingUtils.setStrValues(context,"video_size", String.valueOf(param.getResolution()));
        Log.e("guobao","=====================>video_size==>   "+String.valueOf(param.getResolution()));

        SettingUtils.setStrValues(context,"LanguageZfy", String.valueOf(param.getLanguage()));
        Log.e("guobao","=====================>LanguageZfy==>   "+String.valueOf(param.getLanguage()));

        SettingUtils.setStrValues(context,"jpeg_quality", String.valueOf(param.getPicQuality()));
        Log.e("guobao","=====================>jpeg_quality==>   "+String.valueOf(param.getPicQuality()));

        SettingUtils.setStrValues(context,"exposure_compensation", String.valueOf(param.getContinuousShooting()));
        Log.e("guobao","=====================>exposure_compensation==>   "+String.valueOf(param.getContinuousShooting()));

        SettingUtils.setStrValues(context,"preview_size", String.valueOf(param.getPreviewSize()));
        Log.e("guobao","=====================>preview_size==>   "+String.valueOf(param.getPreviewSize()));

        // SettingUtils.setStrValues(context,"onekey_lockscreen", String.valueOf(param.ge));

        SettingUtils.setStrValues(context,"video_quality", String.valueOf(param.getVideoQuality()));
        Log.e("guobao","=====================>video_quality==>   "+String.valueOf(param.getVideoQuality()));

        SettingUtils.setStrValues(context,"video_delay", String.valueOf(param.getVideoDelay()));

        Log.e("guobao","=====================>video_delay==>   "+String.valueOf(param.getVideoDelay()));


        SettingUtils.setBoolValues(context,"vibrate", StringUtil.switchState(String.valueOf(param.getVibrate())));
        Log.e("guobao","=====================>video_rate==>   "+String.valueOf(param.getVideoBitrate()));

        SettingUtils.setStrValues(context,"jpeg_continous",String.valueOf(param.getContinuousShooting()));
        Log.e("guobao","=====================>jpeg_continous==>   "+String.valueOf(param.getVideoBitrate()));

        SettingUtils.setBoolValues(context,"video_motiondetection",StringUtil.switchState(String.valueOf(param.getMotionDetection())));
        Log.e("guobao","=====================>video_motiondetection==>   "+StringUtil.switchState(String.valueOf(param.getMotionDetection())));

        SettingUtils.setStrValues(context,"udisk",String.valueOf(param.getUSBMode()));
        Log.e("guobao","=====================>udisk==>   "+String.valueOf(param.getUSBMode()));

        SettingUtils.setStrValues(context,"jpeg_outputformat",String.valueOf(param.getPicOutputFormat()));
        Log.e("guobao","=====================>jpeg_outputformat==>   "+String.valueOf(param.getPicOutputFormat()));

        SettingUtils.setStrValues(context,"sos",String.valueOf(param.getSos()));
        Log.e("guobao","=====================>sos==>   "+String.valueOf(param.getSos()));

        SettingUtils.setStrValues(context,"InfraRed",String.valueOf(param.getInfraRed()));
        Log.e("guobao","=====================>InfraRed==>   "+String.valueOf(param.getInfraRed()));

      //  SettingUtils.setStrValues(context,"Volume",String.valueOf(param.getVolume()));



        SettingUtils.initConfigure(context);
        /**
         * 向主界面发送请求
         */
        EventBus.getDefault().post(new AnyEvent("update"));
    }


    //获取当前E12 GPS位置信息;
    public static String GetGpsPositionInfo(Context context)
    {
        Log.e(androidService.TAG,  "GetGpsPositionInfo->执行到我的方法");
        //sPositionInfo格式为：x*y  这个再简单一点，只要经度和纬度，其它不要
        String sPositionInfo = "";
        /************** 开始你的方法  *************/
        if (!NetUtils.isConnected(context)) {
            Toast.makeText(context, "请先连接网络", Toast.LENGTH_SHORT).show();
            return sPositionInfo;
        }
        GpsUtils gpsUtils = new GpsUtils(context);
        Location location = gpsUtils.start(context, null);
        double latitude  = 0.0f;
        double longitude = 0.0f;
        if (location!=null)
        {
             latitude = location.getLatitude();
             longitude = location.getLongitude();
             sPositionInfo = latitude+"*"+longitude;
        }
       // sPositionInfo = "720*1280*360";

        Log.e(androidService.TAG,  "GetGpsPositionInfo->"+sPositionInfo);
        /*************** 方法结束  *************/
        //sPositionInfo格式为：x*y*z
        return sPositionInfo;
    }




      /**
     * 录制视频方法。
     * 摄像头异常，不能拍摄视频
     */
    public static void VideoRecord(Context context) {
        if (!CameraHandlerThread.isCameraOpened(context,false)) {
            Toast.makeText(context, "摄像头异常4", Toast.LENGTH_SHORT).show();
            return;
        }

        if (isVideoRecording) {
            CameraHandlerThread.getInstance().camera.setPreviewCallback(CameraHandlerThread.mCameraPreviewCallback);
            CameraHandlerThread.getInstance().getmCameraPreviewCallback().startRecording();
            //button.setText("停止");
        } else {
            CameraHandlerThread.getInstance().getmCameraPreviewCallback().stopRecording();
            CameraHandlerThread.getInstance().getCameraInstance().setPreviewCallback(null);
            // button.setText("开始");
        }
    }

}
