package gbpe.policeass.utils;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

//import com.guobao.lightboard.*;
//import com.guobao.lightboard.HardwareControler;

import java.util.Calendar;

import gbpe.policeass.Setup.Setup;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/6/27 10:42
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/6/27 10:42
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class SOSUtil {
    private static long lastTime;
    private static SoundPlay keySond;



     public static void sosStart(SoundPlay key){
         if (keySond ==null){
             keySond = key;
         }
         if (keySond!=null){
             handler.post(timeRun);
         }
    }

    public static void sosStop(){
        handler.removeCallbacks(timeRun);
        //HardwareControler.SetWhiteLightState(0);
        DevControl.writeGuobaoGpio(96,0);
        keySond =null;
    }

    static boolean sosState;
    static long time,currentRec;
    static Runnable timeRun = new Runnable() {
        @Override
        public void run() {
             time++;//调试的时候使用
            currentRec++;
            Log.e("TAG","time is :"+time);
            Log.e("TAG","currentRec is :"+currentRec);
            sosState = !sosState;
            if (sosState)
            {
                //HardwareControler.SetWhiteLightState(1);
                DevControl.writeGuobaoGpio(96,1);
                if (currentRec>=2){//一秒中播一次(因为该文件长度为1s)
                    currentRec=0;
                      if (keySond!=null){
                        System.out.println("------>>>开始播放");

                        keySond.play(2,0);
                        System.out.println("------>>>停止播放");
                    }
                }
            }else
            {
                //HardwareControler.SetWhiteLightState(0);
                DevControl.writeGuobaoGpio(96,0);
            }
            handler.postDelayed(timeRun, 400);//用于计时的线程(秒计时器)

        }
    };
    /**
     * 计时线程
     */
    public static Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            //将计时线程从新放到消息队列中
            handler.post(timeRun);

        }
    };
}

