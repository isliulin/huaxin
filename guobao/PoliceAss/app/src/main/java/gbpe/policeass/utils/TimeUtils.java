package gbpe.policeass.utils;

import android.os.SystemClock;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

/**
 * Created by Administrator on 2016/11/10.
 */
public class TimeUtils {
    public static final SimpleDateFormat DEFAULT_SDF = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
    public static final SimpleDateFormat DEFAULT_SDF2 = new SimpleDateFormat("yyyy/MM/dd-HH:mm:ss", Locale.getDefault());
    public static final SimpleDateFormat DEFAULT_SDF3 = new SimpleDateFormat("yyyyMMddHHmmss", Locale.getDefault());
    public static final SimpleDateFormat DEFAULT_SDF4 = new SimpleDateFormat("HH-mm-ss");
    public static final SimpleDateFormat DEFAULT_SDF5 = new SimpleDateFormat("yyyyMMdd", Locale.getDefault());

    public static String getDate() {
        Calendar ca = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmss");
        String date = sdf.format(ca.getTimeInMillis());
        return date;
    }
    public static String getLogDate(){
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.DAY_OF_MONTH, 0);
        SimpleDateFormat myLogSdf = new SimpleDateFormat("yyyyMMdd");
        String date = myLogSdf.format(calendar.getTime());
        return date;
    }
    public static String getLogDate2(){
        Calendar ca = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        String date = sdf.format(ca.getTimeInMillis());
        return date;
    }

    /**
     * 将时间戳转为时间字符串

     * <p>格式为yyyy/MM/dd-HH:mm:ss</p>
     *
     * @param milliseconds 毫秒时间戳
     * @return 时间字符串
     */
    public static String milliseconds2String(long milliseconds) {
        return milliseconds2String(milliseconds, DEFAULT_SDF2);
    }

    /**
     * 将时间戳转为时间字符串
     * <p>格式为yyyyMMddHHmmss</p>
     *
     * @param milliseconds 毫秒时间戳
     * @return 时间字符串
     */
    public static String milliseconds2String2(long milliseconds) {
        return milliseconds2String(milliseconds, DEFAULT_SDF3);
    }
    /**
     * 将时间戳转为时间字符串
     * <p>格式为yyyy-MM-dd HH:mm:ss</p>
     *
     * @param milliseconds 毫秒时间戳
     * @return 时间字符串
     */
    public static String milliseconds2String3(long milliseconds) {
        return milliseconds2String(milliseconds, DEFAULT_SDF);
    }

    public static String milliseconds2String5(long milliseconds) {
        return milliseconds2String(milliseconds, DEFAULT_SDF5);
    }

    /**
     * 将时间戳转为时间字符串
     * <p>格式为用户自定义</p>
     *
     * @param milliseconds 毫秒时间戳
     * @param format       时间格式
     * @return 时间字符串
     */
    public static String milliseconds2String(long milliseconds, SimpleDateFormat format) {
        return format.format(new Date(milliseconds));
    }



    public static String milliseconds2String4(long milliseconds) {
        return milliseconds2String(milliseconds, DEFAULT_SDF4);
    }
    /*
    * 毫秒转化
    */
    public static String formatTime(long ms) {

        int ss = 1000;
        int mi = ss * 60;
        int hh = mi * 60;
        int dd = hh * 24;

        long day = ms / dd;
        long hour = (ms - day * dd) / hh;
        long minute = (ms - day * dd - hour * hh) / mi;
        long second = (ms - day * dd - hour * hh - minute * mi) / ss;
        long milliSecond = ms - day * dd - hour * hh - minute * mi - second * ss;

        String strDay = day < 10 ? "0" + day : "" + day; //天
        String strHour = hour < 10 ? "0" + hour : "" + hour;//小时
        String strMinute = minute < 10 ? "0" + minute : "" + minute;//分钟
        String strSecond = second < 10 ? "0" + second : "" + second;//秒
        String strMilliSecond = milliSecond < 10 ? "0" + milliSecond : "" + milliSecond;//毫秒
        strMilliSecond = milliSecond < 100 ? "0" + strMilliSecond : "" + strMilliSecond;

        return strMinute + " 分钟 " + strSecond + " 秒";
    }

    /**
     * 将毫秒转换成时分秒00:00:10
     * @param ms
     * @return
     */
    private String msFormat(long ms){

        SimpleDateFormat formatter = new SimpleDateFormat("HH:mm:ss");
        formatter.setTimeZone(TimeZone.getTimeZone("GMT+00:00"));
        String hms = formatter.format(ms);
        return hms;
    }



    public static void onDateSet(/*Calendar c,*/ int year, int month, int day) {

        Calendar c = Calendar.getInstance();
        c.set(Calendar.YEAR, year);
        c.set(Calendar.MONTH, month);
        c.set(Calendar.DAY_OF_MONTH, day);
        long when = c.getTimeInMillis();

        try {
            if (when / 1000 < Integer.MAX_VALUE) {
                SystemClock.setCurrentTimeMillis(when);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }



    public static void onTimeSet(/*Calendar calendar,*/ int hourOfDay, int minute) {
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR_OF_DAY, hourOfDay);
        calendar.set(Calendar.MINUTE, minute);
        long when = calendar.getTimeInMillis();
        if (when / 1000 < Integer.MAX_VALUE) {
            //最关键的一句话
            SystemClock.setCurrentTimeMillis(when);
        }
    }



    /*
    * 将时间转换为时间戳
    */
    public static String dateToStamp(String s) throws ParseException {
        String res;
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = simpleDateFormat.parse(s);
        long ts = date.getTime();
        res = String.valueOf(ts);
        return res;
    }


    /**
     * 设置当前系统时间
     * @param year
     * @param month
     * @param day
     * @param hourOfDay
     * @param minute
     */
    public static void onTimeSet2(int year,int month,int day, int hourOfDay, int minute,int millisecond) {

        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.YEAR, year);
        calendar.set(Calendar.MONTH, (month-1));
        calendar.set(Calendar.DAY_OF_MONTH, day);
        calendar.set(Calendar.HOUR_OF_DAY, hourOfDay);
        calendar.set(Calendar.MINUTE, minute);
        calendar.set(Calendar.SECOND,millisecond);

        long when = calendar.getTimeInMillis();
        if (when / 1000 < Integer.MAX_VALUE) {
            //最关键的一句话
            SystemClock.setCurrentTimeMillis(when);
        }
    }

    public static String getSystemDate() {
        String date = null;
        Calendar calendar = Calendar.getInstance();
        int year = calendar.get(Calendar.YEAR);
        int month = calendar.get(Calendar.MONTH)+1;
        int day = calendar.get(calendar.DAY_OF_MONTH);

        date = String.format(year+"/"+month+"/"+day);
        //AppLog.i (TAG,"date: "+date);
        return date;
    }

    public static String getSystemTime() {
        String time= null;
        Calendar calendar = Calendar.getInstance();
        int hour = calendar.get(Calendar.HOUR_OF_DAY);
        int minute = calendar.get(Calendar.MINUTE);
        int second = calendar.get(Calendar.SECOND);

        //time = String.format(hour+":"+minute+":"+second);
        time = String.format("%02d:%02d:%02d",hour,minute,second);

        return time;
    }
}
