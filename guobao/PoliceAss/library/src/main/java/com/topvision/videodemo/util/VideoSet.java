package com.topvision.videodemo.util;

import android.util.Log;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/22 14:19
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/22 14:19
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class VideoSet {

    public  static String videoFilePath ="";
    public static int frameRate =0;
    public static  String policeNum;
    public static String devNumber;
    public static int BITE_RATE = 0;

    public static long start = 0;
    public static long stop = 0;
    public static int times = 0;
    public static boolean TEST = true;

    public static double longitude;
    public static double latitude;

    public static void printLog(){
        VideoSet.stop = System.currentTimeMillis();
        if (VideoSet.start ==0){
            VideoSet. start = System.currentTimeMillis();
            Log.e("TEST", "Test===============>: start --> "+VideoSet.start );
        }
        ++VideoSet.times;
        if (VideoSet.stop -VideoSet.start<=1000){
            if (TEST) Log.e("TEST", "打印测试数据帧数..." + VideoSet.times);
        }else {
            VideoSet.times =0;
            VideoSet.start=0;
            Log.e("TEST", "Test===============>: stop  "+VideoSet.stop );
        }

    }
}
