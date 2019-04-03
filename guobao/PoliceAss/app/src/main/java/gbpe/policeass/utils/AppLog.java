package gbpe.policeass.utils;

import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import gbpe.policeass.Setup.Setup;

public class AppLog {

    private static final String TAG = "AppLog";
    private static Boolean MYLOG_WRITE_TO_FILE = true;// ÈÕÖ¾Ð´ÈëÎÄ¼þ¿ª¹Ø

    private static char MYLOG_TYPE='v';// ÊäÈëÈÕÖ¾ÀàÐÍ£¬w´ú±íÖ»Êä³ö¸æ¾¯ÐÅÏ¢µÈ£¬v´ú±íÊä³öËùÓÐÐÅÏ¢
    private static String MYLOG_PATH_SDCARD_DIR= Setup.SDCARD_PATH+".dssApp/";// ÈÕÖ¾ÎÄ¼þÔÚsdcardÖÐµÄÂ·¾¶
    private static String MYLOG_PATH_RAM_DIR=Setup.RAM_PATH+"dssApp/";// ÈÕÖ¾ÎÄ¼þÔÚramÖÐµÄÂ·¾¶
    private static String MYLOG_PATH_UDISK_DIR=Setup.UDISK_FILE_PATH+"dssApp/";// ÈÕÖ¾ÎÄ¼þÔÚudiskÖÐµÄÂ·¾¶
    private static int SDCARD_LOG_FILE_SAVE_DAYS = 0;// sd¿¨ÖÐÈÕÖ¾ÎÄ¼þµÄ×î¶à±£´æÌìÊý
    private static String MYLOGFILEName = "dssAppLog.txt";// ±¾ÀàÊä³öµÄÈÕÖ¾ÎÄ¼þÃû³Æ

    private static SimpleDateFormat myLogSdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");// ÈÕÖ¾µÄÊä³ö¸ñÊ½
    private static SimpleDateFormat logfile = new SimpleDateFormat("yyyy-MM-dd");// ÈÕÖ¾ÎÄ¼þ¸ñÊ½

    public static void w(String tag, Object msg) { // ¾¯¸æÐÅÏ¢
        log(tag, msg.toString(), 'w');
    }

    public static void e(String tag, Object msg) { // ´íÎóÐÅÏ¢
        log(tag, msg.toString(), 'e');
    }

    public static void d(String tag, Object msg) {// µ÷ÊÔÐÅÏ¢
        log(tag, msg.toString(), 'd');
    }

    public static void i(String tag, Object msg) {//
        log(tag, msg.toString(), 'i');
    }

    public static void v(String tag, Object msg) {
        log(tag, msg.toString(), 'v');
    }

    public static void w(String tag, String text) {
        log(tag, text, 'w');
    }

    public static void e(String tag, String text) {
        log(tag, text, 'e');
    }

    public static void d(String tag, String text) {
        log(tag, text, 'd');
    }

    public static void i(String tag, String text) {
        log(tag, text, 'i');
    }

    public static void v(String tag, String text) {
        log(tag, text, 'v');
    }

    /**
     * 日志开关
     * @return
     */
    public static int getlogswitch(){
        //int paramValue = SPutil.paramsPref().getInt("logswitch", 0);
        //return paramValue;
        return 1;
    }

    /**
     * ¸ù¾Ýtag, msgºÍµÈ¼¶£¬Êä³öÈÕÖ¾
     *
     * @param tag
     * @param msg
     * @param level
     * @return void
     * @since v 1.0
     */
    private static void log(String tag, String msg, char level) {

        if (Setup.SYSTEM_LOG_SWITCH) {
            if ('e' == level && ('e' == MYLOG_TYPE || 'v' == MYLOG_TYPE)) { // Êä³ö´íÎóÐÅÏ¢
                Log.e(tag, msg);
            } else if ('w' == level && ('w' == MYLOG_TYPE || 'v' == MYLOG_TYPE)) {
                Log.w(tag, msg);
            } else if ('d' == level && ('d' == MYLOG_TYPE || 'v' == MYLOG_TYPE)) {
                Log.d(tag, msg);
            } else if ('i' == level && ('d' == MYLOG_TYPE || 'v' == MYLOG_TYPE)) {
                Log.i(tag, msg);
            } else {
                Log.v(tag, msg);
            }
        }

		if (('e' == level) || ('w' == level)) { 
	        //if(DeviceParamsBiz.getlogswitch() == 1)
            if (MYLOG_WRITE_TO_FILE) {
                writeLogtoFile(String.valueOf(level), tag, msg);
                // writeLogtoUdiskFile(String.valueOf(level), tag, msg);
            }
		}
		/*if(SplashScreen.inProcess){
			writeLogToMemory(String.valueOf(level), tag, msg);
		}*/
    }

    public static void writeLogToMemory(String level, String tag, String msg) {
        // TODO Auto-generated method stub
        Date nowtime = new Date();
        String needWriteFiel = logfile.format(nowtime);
        String needWriteMessage = "["+myLogSdf.format(nowtime)+"]    " + level  + "    " + tag + "    " + msg;

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        File filePath = new File(MYLOG_PATH_RAM_DIR);
        if(!filePath.exists()){
            filePath.mkdir();
        }

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        filePath = new File(MYLOG_PATH_RAM_DIR+"log/");
        if(!filePath.exists()){
            filePath.mkdir();
        }

        File file = new File(MYLOG_PATH_RAM_DIR+"log/", needWriteFiel  + MYLOGFILEName);

        try {
            FileWriter filerWriter = new FileWriter(file, true);//ºóÃæÕâ¸ö²ÎÊý´ú±íÊÇ²»ÊÇÒª½ÓÉÏÎÄ¼þÖÐÔ­À´µÄÊý¾Ý£¬²»½øÐÐ¸²¸Ç
            BufferedWriter bufWriter = new BufferedWriter(filerWriter);
            bufWriter.write(needWriteMessage);
            bufWriter.newLine();
            bufWriter.close();
            filerWriter.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            //AppLog.e(ExceptionUtil.getInfo(e), e);
            e.printStackTrace();
        }
    }

    /**
     * ´ò¿ªÈÕÖ¾ÎÄ¼þ²¢Ð´ÈëÈÕÖ¾
     *
     * @return
     * **/
    private static void writeLogtoFile(String mylogtype, String tag, String text) {// ÐÂ½¨»ò´ò¿ªÈÕÖ¾ÎÄ¼þ

        try {
            //long sdsize = FileOper.getSDAllSize(Setup.SDCARD_PATH);
            long sdsize = SDCardUtils.getSDCardAllSize();
            if(sdsize<2048){
                return;
            }
        } catch (Exception e) {
            e.printStackTrace();
            //AppLog.e(ExceptionUtil.getInfo(e), e);
        }


        //²é¿´SD¿¨Ê£ÓàÄÚ´æÇé¿ö
        try {
            //long size = FileOper.getSDFreeSize(Setup.SDCARD_PATH);
            long size = SDCardUtils.getFreeBytes(Setup.SDCARD_PATH);
            if(size<50){
                return;
            }

        } catch (Exception e) {
            e.printStackTrace();
            //AppLog.e(ExceptionUtil.getInfo(e), e);
        }

        Date nowtime = new Date();
        String needWriteFiel = logfile.format(nowtime);
        String needWriteMessage = "["+myLogSdf.format(nowtime)+"]    " + mylogtype  + "    " + tag + "    " + text;

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        File filePath = new File(MYLOG_PATH_SDCARD_DIR);
        if(!filePath.exists()){
            filePath.mkdir();
        }

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        filePath = new File(MYLOG_PATH_SDCARD_DIR+"log/");
        if(!filePath.exists()){
            filePath.mkdir();
        }


        File file = new File(MYLOG_PATH_SDCARD_DIR+"log/", needWriteFiel  + MYLOGFILEName);

        try {
            FileWriter filerWriter = new FileWriter(file, true);//ºóÃæÕâ¸ö²ÎÊý´ú±íÊÇ²»ÊÇÒª½ÓÉÏÎÄ¼þÖÐÔ­À´µÄÊý¾Ý£¬²»½øÐÐ¸²¸Ç
            BufferedWriter bufWriter = new BufferedWriter(filerWriter);
            bufWriter.write(needWriteMessage);
            bufWriter.newLine();
            bufWriter.close();
            filerWriter.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            //AppLog.e(ExceptionUtil.getInfo(e), e);
            e.printStackTrace();
        }
    }

    /**
     * ´ò¿ªÈÕÖ¾ÎÄ¼þ²¢Ð´ÈëÈÕÖ¾
     *
     * @return
     * **/
    private static void writeLogtoUdiskFile(String mylogtype, String tag, String text) {// ÐÂ½¨»ò´ò¿ªÈÕÖ¾ÎÄ¼þ

        try {
            //long sdsize = FileOper.getSDAllSize(Setup.UDISK_FILE_PATH);
            long sdsize = SDCardUtils.getSDCardAllSize();
            if(sdsize<2048){
                return;
            }
        } catch (Exception e) {
            e.printStackTrace();
            //AppLog.e(ExceptionUtil.getInfo(e), e);
        }


        //²é¿´SD¿¨Ê£ÓàÄÚ´æÇé¿ö
        try {
            //long size = FileOper.getSDFreeSize(Setup.UDISK_FILE_PATH);
            long size = SDCardUtils.getFreeBytes(Setup.UDISK_FILE_PATH);
            if(size<50){
                return;
            }

        } catch (Exception e) {
            e.printStackTrace();
            //AppLog.e(ExceptionUtil.getInfo(e), e);
        }

        Date nowtime = new Date();
        String needWriteFiel = logfile.format(nowtime);
        String needWriteMessage = "["+myLogSdf.format(nowtime)+"]    " + mylogtype  + "    " + tag + "    " + text;

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        File filePath = new File(MYLOG_PATH_UDISK_DIR);
        if(!filePath.exists()){
            filePath.mkdir();
        }

        //¼ì²éÎÄ¼þÂ·¾¶ÊÇ·ñ´æÔÚ
        filePath = new File(MYLOG_PATH_UDISK_DIR+"log/");
        if(!filePath.exists()){
            filePath.mkdir();
        }


        File file = new File(MYLOG_PATH_UDISK_DIR+"log/", needWriteFiel  + MYLOGFILEName);

        try {
            FileWriter filerWriter = new FileWriter(file, true);//ºóÃæÕâ¸ö²ÎÊý´ú±íÊÇ²»ÊÇÒª½ÓÉÏÎÄ¼þÖÐÔ­À´µÄÊý¾Ý£¬²»½øÐÐ¸²¸Ç
            BufferedWriter bufWriter = new BufferedWriter(filerWriter);
            bufWriter.write(needWriteMessage);
            bufWriter.newLine();
            bufWriter.close();
            filerWriter.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            // AppLog.e(ExceptionUtil.getInfo(e), e);
            e.printStackTrace();
        }
    }

    /**
     * É¾³ýÖÆ¶¨µÄÈÕÖ¾ÎÄ¼þ
     * */
    public static void delFile() {// É¾³ýÈÕÖ¾ÎÄ¼þ
        String needDelFiel = logfile.format(getDateBefore());
        File file = new File(MYLOG_PATH_SDCARD_DIR, needDelFiel + MYLOGFILEName);
        if (file.exists()) {
            file.delete();
        }
    }

    /**
     * µÃµ½ÏÖÔÚÊ±¼äÇ°µÄ¼¸ÌìÈÕÆÚ£¬ÓÃÀ´µÃµ½ÐèÒªÉ¾³ýµÄÈÕÖ¾ÎÄ¼þÃû
     * */
    private static Date getDateBefore() {
        Date nowtime = new Date();
        Calendar now = Calendar.getInstance();
        now.setTime(nowtime);
        now.set(Calendar.DATE, now.get(Calendar.DATE) - SDCARD_LOG_FILE_SAVE_DAYS);
        return now.getTime();
    }

    /** ¿É¿ØÖÆµÄÈÕÖ¾ */
    private static final boolean IS_SYSOSHOW= true;
    /** ¿É¿ØÖÆµÄÈÕÖ¾ */
    public static void systemzOutp(String text) {
        if (IS_SYSOSHOW) {
            System.out.println(text);
        }
    }
    public static void updateMsg(String string) {
        // TODO Auto-generated method stub
        AppLog.systemzOutp(string);
    }
}

