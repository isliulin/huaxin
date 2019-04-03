package gbpe.policeass.views;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

//import com.guobao.lightboard.HardwareControler;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.DevControl;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/8/3 19:05
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/8/3 19:05
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class TimeIndicator extends LinearLayout {

    public static final String TAG = "TimeIndicatorView";
    //private static ImageView iv_point;
    //private static TextView tv_counter;
    //private static LinearLayout ll_indicator;
    protected static ImageView iv_point;
    protected static TextView tv_counter;
    protected static LinearLayout ll_indicator;
    public static final int Updata_timing = 0x123;
    private static Thread mThread;
    private static boolean isThreadKill = false;
    //private TimeTickThread timeTickThread;
    private boolean isPointVisible = true;
    private Handler mDuraHandler = null;
    private Runnable mTickRunnable = null;
	private long mStartSystemTime = 0;
    //private int timerTicks = 0;

    public static int getTimeTick() {
        return mTimeTick;
    }

    public static void setTimeTick(int mTimeTick) {
        TimeIndicator.mTimeTick = mTimeTick;
    }

    private static int mTimeTick = 0;
	private static int mStartTick = 0;

    protected static boolean timeOut = false;//定时时间到

    //protected static boolean isVideoRecording;

    public TimeIndicator(Context context) {
        this(context,null);
    }

    public TimeIndicator(Context context, AttributeSet attrs) {
       this(context,attrs,0);
    }

    public TimeIndicator(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        LayoutInflater.from(context).inflate(R.layout.indicator,this,true);
        iv_point = (ImageView)findViewById(R.id.iv_counter);
        tv_counter = (TextView) findViewById(R.id.tc_time);
        ll_indicator = (LinearLayout)findViewById(R.id.rl_indicate);
    }

    private void setVisible(){
        ll_indicator.setVisibility(VISIBLE);
    }

    public void setInVisible(){
        ll_indicator.setVisibility(INVISIBLE);
    }


    //private static boolean isStarting;//是否开始预录了

    private void setPointImgRescourse(boolean type){
        if (!type)//预录
        {
            //time = -1;
            //tv_counter.setText("00:00:00");
            iv_point.setImageResource(R.drawable.bluepoint);
            //isStarting = true;
        }else//开始正式录像
        {
            //isStarting = false;
            //isVideoRecording = true;
            iv_point.setImageResource(R.drawable.point);
        }
    }

    private void setPointTwinkle(){
        isPointVisible = !isPointVisible;
        iv_point.setVisibility(isPointVisible?VISIBLE:INVISIBLE);
    }

    /*
    private static class TimeTickThread<Token> extends Thread{

        private static TimeTickThread mInstance = null;
        private static  boolean isRuning = false;
        private Handler mResponseHandler;
        //private int mTimeTick = 0;
        private Listener<Token> mListener;
		private static boolean isTypeRcd = false;

        public interface Listener<Token>{
            void refreshTick(Token token, int ticks, boolean type);
        }

        public void setListener(Listener<Token> listener){
            mListener = listener;
        }

        public static TimeTickThread getInstance(Handler handler, int sTick, boolean type){
            if (mInstance==null){
                mInstance = new TimeTickThread(handler,sTick);
                //return mInstance;
                isTypeRcd = type;
            }
            return mInstance;
        }

        public static TimeTickThread getInstance(){
            return mInstance;
        }

        public void stopTick(){
            isRuning = false;
            mInstance = null;
            interrupt();
            // 此处需要延时一会，等待线程退出
            try {
                sleep(10);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        public static boolean isOnRun(){
            if(mInstance==null){
                return false;
            } else {
                return true;
            }
        }

        private TimeTickThread(Handler handler, int tickStart){
            mResponseHandler = handler;
            mTimeTick = tickStart;
        }


        @Override
        public void start(){
            isRuning = true;
            super.start();
        }

        @Override
        public void run(){
            final Token token = null;
            while (isRuning){
                // do the tick
                //try {
                //    sleep(1000);
               // } catch (InterruptedException e) {
                //    e.printStackTrace();
               // }
                //mTimeTick++;
                mResponseHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        mListener.refreshTick(token,mTimeTick++,isTypeRcd);
                    }
                });
				try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    AppLog.e(TAG, "Thread interrupted");
                }
            }
            //mTimeTick = 0;
            mInstance = null;
        }
    }*/


    /**
     * 本程序中可以根据是否真正开始录像来改变指示器状态
     * @param type->isVideoRecording
     */
    public void start(final boolean type, final int sTick/*, boolean isNew*/){
/*
        if (!type){

            if (isStarting){
                return;
            }
        }*/

        //handler.removeCallbacks(timeRun);
        //setVisible();
        //setPointImgRescourse(type);
        //if (!type){   // 如果是预录，则不显示录制时间
        //    tv_counter.setVisibility(INVISIBLE);
       // }
        AppLog.i(TAG,"start, type:"+type);

		//if (isNew){
		//	mStartSystemTime = System.currentTimeMillis();
		//}

        //tv_counter.setVisibility(type?VISIBLE:INVISIBLE);
        /*
        if (TimeTickThread.isOnRun() == true){
            TimeTickThread.getInstance().stopTick();
        }

        if (TimeTickThread.isOnRun() == false){
            timeTickThread = TimeTickThread.getInstance(new Handler(),sTick,type);
            timeTickThread.setListener(new TimeTickThread.Listener<Integer>() {
                @Override
                public void refreshTick(Integer integer, int ticks, boolean type) {
                    AppLog.i(TAG,"refreshTick type:"+type+", ticks:"+ticks+", duration: "+Setup.videoMaxDuration);
                	if (type){
                	    // 显示录制的tick，在此判断是否到达最大的Duration
                    	updataText(ticks);
                    	if ((ticks>0)&&((ticks%Setup.videoMaxDuration)==0)) {
							// 判断是否达到最大时长
                        }
                	} else {
                        //setPointTwinkle();
                        if (ticks > Setup.videoPrerecordTime){
							setPointTwinkle();
							updataText(Setup.videoPrerecordTime);
						}else {
							updataText(ticks);
						}
                        
                    }
                }
            });
            timeTickThread.start();
        }*/

        if (mDuraHandler!=null) {
            // 此处如果非空，表示已经有定时器显示，如果不删除，则会交替显示
            mDuraHandler.removeCallbacks(mTickRunnable);
            mTickRunnable = null;
            mDuraHandler = null;
        }

        mTimeTick = sTick;

        mStartTick = mTimeTick;
        mStartSystemTime = System.currentTimeMillis();
        //updataText(mTimeTick++);
        // ycb, 20190103
        updataText(mTimeTick);
        mDuraHandler = new Handler();
        mTickRunnable = new Runnable() {
            @Override
            public void run() {
                if (type) {
                    //updataText(mTimeTick++);
                    long interval = (System.currentTimeMillis() - mStartSystemTime)/1000;
					mTimeTick = mStartTick + (int)interval;
                    updataText(mTimeTick);
                } else {
                    if (mTimeTick >= Setup.videoPrerecordTime) {
                        setPointTwinkle();
                        updataText(Setup.videoPrerecordTime);
                    } else {
                        long interval = (System.currentTimeMillis() - mStartSystemTime)/1000;
                        mTimeTick = mStartTick + (int)interval;
                        updataText(mTimeTick);
                    }
                }
				//HardwareControler.SetRedLightState(1);
				//HardwareControler.SetGreenLightState(0);
                DevControl.writeGuobaoGpio(21,1);
                DevControl.writeGuobaoGpio(20,0);
                mDuraHandler.postDelayed(this,1000);
            }
        };
        mDuraHandler.postDelayed(mTickRunnable,1000);
		setVisible();
        setPointImgRescourse(type);
		// 不管什么模式，指示圆点始终先点亮
		iv_point.setVisibility(VISIBLE);
		//tv_counter.setVisibility(type?VISIBLE:INVISIBLE);
		tv_counter.setVisibility(VISIBLE);
        /*
        mThread = new Thread(){
            @Override
            public void run(){
                while(isThreadKill){
                    try{
                        sleep(1000);
                    } catch (InterruptedException e){
                        e.printStackTrace();
                    }

                }
            }
        };*/
        //handler.post(timeRun);
        /*
        handler.post(new Runnable() {
            @Override
            public void run() {
                Message msg = new Message();
                msg.what = Updata_timing;
                msg.arg1 = time;
            }
        });*/
        //new HandlerThread(){}.start();
    }



    /**
     * type->isVideoRecording
     */
    public void stop(){
        /*
        if (isStarting){
            return;
        }*/
        /*
        if (timeTickThread != null){
        	timeTickThread.stopTick();
        }*/
        //currentRec =0;
        //handler.removeCallbacks(timeRun);
        if (mDuraHandler!=null) {
            mDuraHandler.removeCallbacks(mTickRunnable);
            mTickRunnable = null;
            mDuraHandler = null;
        }
		//mTimeTick = 0;
        setInVisible();
        /*
        if (!isVideoRecording){
            time = timeToSec(tv_counter.getText().toString());
        }else {
            time = -1;
            tv_counter.setText("00:00:00");
        }
        isVideoRecording = false;*/
    }


    /**
     * 定义一个定时线程
     * @param time
     * @return
     */
//========================================================================================================================================
    /*private static long currentRec;
    private static int  time = -1;
    public static Runnable timeRun = new Runnable() {
        @Override
        public void run() {

            time++;
          // Log.e("TAG","=================>time is :"+time);
           Message msg = new Message();
            msg.what = Updata_timing;
            msg.arg1 = time;
            mHandler.sendMessage(msg);
            handler.postDelayed(timeRun, 1000);//用于计时的线程(秒计时器)
            if (!isVideoRecording) {
                currentRec++;
                if (currentRec > 40) {
                    currentRec = 0;
                    timeOut = true;
                    handler.removeCallbacks(timeRun);
                }
            }
        }
    };*/
    /**
     * 计时线程
     */
    /*
    public static Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
             //将计时线程从新放到消息队列中
            handler.post(timeRun);
        }

    };*/
    /*
    public static Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == Updata_timing){
                updataText(time);
            }
        }
    };*/


    /**
     *  秒钟转换成这种格式的时间00:00:10
     * @param time
     * @return
     */
    public static String secToTime(int time) {

        String timeStr = null;
        int hour = time / 3600;
        int minute = time / 60 % 60;
        int second = time % 60;
        timeStr = unitFormat(hour) + ":" + unitFormat(minute) + ":" + unitFormat(second);
        return timeStr;
    }
    public static String unitFormat(int i) {
        String retStr = null;
        if (i >= 0 && i < 10)
            retStr = "0" + Integer.toString(i);
        else
            retStr = "" + i;
        return retStr;
    }

    /**
     * 00:00:10 这种格式的时间转化成秒钟
     * @param time
     * @return
     */
    public static int timeToSec(String time){
        String[] timeArray = time.split(":");
        int hour  = Integer.parseInt(timeArray[0])*3600;
        int min = Integer.parseInt(timeArray[1])*60;
        int sec = Integer.parseInt(timeArray[2]);
        return hour+min+sec;
    }


    private static void updataText(final int time){
//        ( (Activity)mContext).runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//                tv_counter.setText(secToTime(time));
//            }
//        });
        tv_counter.setText(secToTime(time));
    }


    /**
     * 仅仅是停止预录的时候调用
     */
    public void rempveTimeRun(){/*
        if (timeRun!=null){
            handler.removeCallbacks(timeRun);
            time = -1;
        }
        mHandler.removeMessages(Updata_timing);
        tv_counter.setText("00:00:00");
        isStarting = false;*/
    }
}
