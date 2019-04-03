package gbpe.policeass.utils;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import com.topvision.videodemo.CameraWrapper;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;

/**
 * Created by Administrator on 2016/10/23.
 */
public class StringUtil {

    private static final String TAG = "StringUtil";
    private static Context context = App.instance;

    public static String cutStirng(String fileName){
        return fileName.substring(0, fileName.indexOf(" "));//返回的数据格式是711 MB或711 GB，所以应该截取空格前面的数据。
    }
    public static double string2Double(String str){

        return  Double.parseDouble(str);
    }
    public static int strint2Int(String str ){
        return Integer.parseInt(str);
    }

    /*
    *
    * 字节转化成MB
    * */
    public static String byteToMB(long size){
        long kb = 1024;
        long mb = kb*1024;
        long gb = mb*1024;
        if (size >= gb){
            return String.format("%.1f MB",(float)size/mb);
        }else if (size >= mb){
            float f = (float) size/mb;
            return String.format(f > 100 ?"%.0f MB":"%.1f MB",f);
        }else if (size > kb){
            float f = (float) size / mb;
            return String.format(f>100?"%.0f MB":"%.1f KB",f);
        }else {
            return String.format("%d B",size);
        }
    }


    /*
    *
    * 字节转化成MB
    * */
    public static String byteToBKGM(long size){
        long kb = 1024;
        long mb = kb*1024;
        long gb = mb*1024;
        if (size >= gb){
            return String.format("%.1f GB",(float)size/gb);
        }else if (size >= mb){
            float f = (float) size/mb;
            return String.format(f > 100 ?"%.0f MB":"%.1f MB",f);
        }else if (size > kb){
            float f = (float) size / kb;
            return String.format(f>100?"%.0f KB":"%.1f KB",f);
        }else {
            return String.format("%d B",size);
        }
    }






//-------------------------------------------------以下为设置界面参数处理工具方法-----------------------------------------------------------------






    /**
     * 将正则表达式修改为“^-?[0-9]+”即可，修改为“-?[0-9]+.?[0-9]+”即可匹配所有数字。
     *
     * @param str
     * @return
     */
    public static boolean isNumeric(String str) {
        Pattern pattern = Pattern.compile("[0-9]*");
        Matcher isNum = pattern.matcher(str);
        if (!isNum.matches()) {
            return false;
        }
        return true;
    }

    //public static int videoRate = 0;
    //public static int picFine;

    /***
     * 画质：操作类
     * @param str：参数字符串
     * @param flag ture :视频画质  false : 音频画质
     * @return
     */
    public static int Quality(String str, boolean flag) {
    	int quality = 100;
        if (flag)
        {
            if (str.equals(context.getString(R.string.common))) {
                quality = CameraWrapper.DST_IMAGE_HEIGHT * CameraWrapper.DST_IMAGE_WIDTH * 3 * 8 * 25 / 256;//2160000
            } else if (str.equals(context.getString(R.string.fine))) {
                quality = 3236400;
            } else if (str.equals(context.getString(R.string.excellent))) {
                quality = 1280 * 720 * 7;
            }
        } else
        {
            if (str.equals(context.getString(R.string.common))) {
                quality = 60;
            } else if (str.equals(context.getString(R.string.fine))) {
                quality = 80;
            } else if (str.equals(context.getString(R.string.excellent))) {
                quality = 100;
            }
        }
		return quality;

    }



    /**
     * 获取带数字的字符串中的数字。（用于预录，录像）
     * @param src 5秒，10秒，20分，30分，关
     * @return 返回0的时候，是不连拍
     */
    public static int getDigstFromStr(String src)
    {
        // String a="love23next234csdn3423javaeye";
        String regEx="[^0-9]";
        Pattern p = Pattern.compile(regEx);
        Matcher m = p.matcher(src);
        String digst = m.replaceAll("").trim();
        if (TextUtils.isEmpty(digst)){
            return  0;
        } else if (isNumeric(digst)){
           return Integer.parseInt(digst);
        }
        return 0;
    }

    /**
     * 从参数字符串找那个提取时间数字，比如分钟或小时转换成秒
     * @param time
     * @return: 如果是关，则返回0， 否则返回大于0的数。
     *   5秒，10秒，20分，30分，关
     */
    public  static int getSecFromStr(String time)
    {

        int seconds =1;
        String uint = getNoDigestStr(time).trim();
        Log.i("TAG", "getSecFromStr: == > "+uint);
        if (uint.equals(context.getString(R.string.second))) {
            seconds = getDigstFromStr(time)*1000;
        } else if (uint.equals(context.getString(R.string.minute))) {
            seconds *=getDigstFromStr(time)*60000;
        } else if (uint.equals(context.getString(R.string.hour))) {
            seconds *=getDigstFromStr(time)*60000*60;
        } else if (uint.equals(context.getString(R.string.close))) {
            seconds =0;
        } else {
            seconds = 0;
        }

        return seconds;
    }



    /**
     * 去掉字符串中的数字
     * @param str String testStr = "大幅度要栽夺工地奇巧地厅革dfdsfdsafd32123313";
     */

    public static String getNoDigestStr(String str)
    {
        return str.replaceAll("[0-9]","");
    }



    /**
     * 获取字符串中,括号( )里的内容
     * 3M (2560x1440) 16:9
     * @return
     */
    public static List<String> getBracketsValue(String Bvalue) {
        List<String> list = new ArrayList<String>();
        String[] sp = Bvalue.split(";");
        String temp = "";
        for (int i = 0; i < sp.length; i++) {
            temp = sp[i].substring(sp[i].indexOf("(") + 1, sp[i].indexOf(")"));
            list.add(temp);
        }
        return list;
    }


    /**
     * 使用正则表达式提取中括号[ ]中的内容
     * @param msg
     * @return
     */
    public static List<String> extractMessageByRegular(String msg){

        List<String> list=new ArrayList<String>();
        Pattern p = Pattern.compile("(\\[[^\\]]*\\])");
        Matcher m = p.matcher(msg);
        while(m.find()){
            list.add(m.group().substring(1, m.group().length()-1));
        }
        return list;
    }


    /**
     * 判断  开和关两种状态
     * @param state
     * @return
     */
    public static boolean switchState(String state)
    {
        boolean flag = false;

        //AppLog.i(TAG, R.string.open+", "+R.string.close);

        if (state.equals(context.getString(R.string.open))) {
            flag = true;
        } else if (state.equals(context.getString(R.string.close))) {
            flag = false;
        }

        return flag;
    }

    /**
     * 判断  开和关两种状态
     * @param state
     * @return
     */
    public static boolean fileView(String state)
    {
        boolean flag = false;

        if (state.equals(context.getString(R.string.file_view_mode))) {
            flag = true;
        } else if (state.equals(context.getString(R.string.picture_view_mode))) {
            flag = false;
        }

        return flag;
    }


    /**
     * 红外线：手动，自动，关
     * @param state
     */
    public static boolean InfraRed(String state)
    {
        if (state.equals(context.getString(R.string.manual))) {
            Setup.inFraRed = false;
        } else if (state.equals(context.getString(R.string.auto))) {
            Setup.inFraRed = true;
        }
        return   Setup.inFraRed;

    }

    /**
     * U盘设置模式：USB设置模式，U盘模式
     * @param state
     */
    public  static boolean uDisk(String state)
    {
        if (state.equals(context.getString(R.string.usb_mode_cmd))) {
            return true;
        } else {
            return false;
        }
    }


    /**
     * 将boolean值转换成对应的开和关
     * @param state
     * @return
     */
    public  static int boolToChinese(boolean state)
    {
        //return state==true? "开":"关";
        return state==true?R.string.open:R.string.close;
    }


}
