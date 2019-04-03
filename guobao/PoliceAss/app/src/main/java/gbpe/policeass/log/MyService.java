package gbpe.policeass.log;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.ParseException;
import java.util.Calendar;
import java.util.Date;


import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.TimeUtils;

/*
* 日志工具类：
* */
public class MyService extends Service {

    public MyBinder myBinder = new MyBinder();

    @Override
    public IBinder onBind(Intent intent) {//一绑定服务就清除过期日志
        svHandler.post(time);
        deleteSDcardExpiredLog();// 首先清除七天之前的过期日志。
        return myBinder;
    }

    public class MyBinder extends Binder {
        public MyService getServiceInstance() {
            return MyService.this;
        }
    }

    public void writeLogs(String str) {
        String currentVideoFilePath = Environment.getExternalStorageDirectory().getPath();
        if (currentVideoFilePath != null) {
            File dir = new File(currentVideoFilePath + "/MyLogs/");
            if (!dir.exists()) {
                dir.mkdir();
            }
            writeContentToSDCardFile(str,dir.getAbsolutePath(), TimeUtils.getLogDate()+".log",true,true);

        }
    }


    /* 此方法为android程序写入sd文件文件，用到了android-annotation的支持库@
  *
 * @param content   写入文件的内容
  * @param folder   保存文件的文件夹名称,如log；可为null，默认保存在sd卡根目录
  * @param fileName 文件名称，默认app_log.txt
  * @param append   是否追加写入，true为追加写入，false为重写文件
  * @param autoLine 针对追加模式，true为增加时换行，false为增加时不换行
  */
    public synchronized static void writeContentToSDCardFile( String content,final
        String folder,final String fileName,final boolean append,final  boolean autoLine){

        FileWriter fw = null;
        content = TimeUtils.getLogDate2()+" -> "+content+"(RS:14807MB TS:14820MB)";
        try {
            fw = new FileWriter(folder+"/"+fileName ,  append );
            if (autoLine){
                fw.write("\r\n" );  //写入换行
                fw.write(content );
                fw.write("\r\n" );
            }
            else {fw.write(content );}
            //关闭文件
            fw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    /**
     * 删除内存下过期的日志
     */
    private void deleteSDcardExpiredLog() {
        File file = new File(Setup.LOG_PATH_SDCARD_DIR);
        if (file.isDirectory()) {
            File[] allFiles = file.listFiles();
            if (allFiles != null){
            for (File logFile : allFiles) {
                String fileName = logFile.getName();
                Log.e("Main",fileName);
                String createDateInfo = getFileNameWithoutExtension(fileName);
                Log.e("Main",createDateInfo);
                if (canDeleteSDLog(createDateInfo)) {
                    logFile.delete();
                }

              }
            }
        }
    }

    /**
     * 判断sdcard上的日志文件是否可以删除
     * @param createDateStr
     * @return
     */
    public boolean canDeleteSDLog(String createDateStr) {
        boolean canDel = false;
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.DAY_OF_MONTH, -1 * Setup.SDCARD_LOG_FILE_SAVE_DAYS);//删除7天之前日志(两个参数的数值相加)
        Log.e("Main","calendar"+calendar.getTime().toString());
        Date expiredDate = calendar.getTime();
        try {
            Date createDate = Setup.myLogSdf.parse(createDateStr);
            Log.e("Main",createDate.toString());
            Log.e("Main","createDate"+createDate.getTime());
            canDel = createDate.before(expiredDate);
            Log.e("Main",""+canDel);
        } catch (ParseException e) {

            canDel = false;
        }
        return canDel;
    }
    /**
     * 去除文件的扩展类型（.log）
     * @param fileName
     * @return
     */
    public static String getFileNameWithoutExtension(String fileName){
        return fileName.substring(0, fileName.indexOf("."));
    }
    Handler svHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            svHandler.post(time);
        }
    };
     Runnable time = new Runnable() {
         @Override
         public void run() {
            // System.out.println("--->>>service中监控SD卡状态");
             if (!SDCardUtils.judgeSDCardFreeSizeIsPlenty(getApplicationContext()))
             {
                 //Toast.makeText(getApplicationContext(), "空间不足", Toast.LENGTH_SHORT).show();
             }
             svHandler.postDelayed(time,2000);
         }
     };
}