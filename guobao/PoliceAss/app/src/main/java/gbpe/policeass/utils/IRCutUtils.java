package gbpe.policeass.utils;

import android.util.Log;

//import com.guobao.lightboard.HardwareControler;

import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.Setup.Setup;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/8/22 17:01
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/8/22 17:01
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class IRCutUtils {


    static int lastState = -1;
    //int adc_data;
    //String adc_cc;
    private static final String ADC_FILE = "/sys/class/power_supply/battery/device/ADC_CH2";
    private static final String TAG = "IRCutUtils";
	private static int loop_cnt = 0;
	private static int SAMPLE_TIMES = 40;
    private IStopRecord mStopRecord = null;
	private static final int AMB_THR_NIG = 100;//950;
	private static final int AMB_THR_DAY = 1500;
	
	
	//private static boolean currIRcutState = true;
    private static boolean currIRcutState = false;

	public static boolean getIRstate()
    {
        return currIRcutState;
    }
    public static void setIRstate(boolean state)
    {
        currIRcutState = state;
    }

	/*
    private static Context mContext = null;

    public static Context getmContext() {
        return mContext;
    }

    public static void setmContext(Context mContext) {
        IRCutUtils.mContext = mContext;
    }
    */
	//private static Handler mStopHandle = null;
/*

    public static void setStopHandle(Handler handler)
    {
        mStopHandle = handler;
    }  */

    public interface IStopRecord{
        void stopRcd(boolean isRcd, boolean irState);
    }

    public IRCutUtils(IStopRecord func)
    {
        mStopRecord = func;
		//closeIRcut();
		//IRcutAction(currIRcutState);
    }

	//根据实际测试，当打开红外灯时，检测亮度会高1200左右
	private static int IRCUT_CMP = 300;
    //获取ADC值
    //private static int GetAdcstate() 
    private static int GetAdcstate()
    {
        int lowlight = -1;
        int adc_data = 0;
        String adc_cc;
        try {
            FileReader adc_fr = new FileReader(ADC_FILE);
            BufferedReader adc_br = new BufferedReader(adc_fr);
            adc_cc = adc_br.readLine().toString();
            adc_data = Integer.parseInt(adc_cc);

			DelayFunction(1000L);

        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        Log.i(TAG,"zhhs_Loop_GetAdcstate "+adc_data);
        if(adc_data <= AMB_THR_NIG)
        {
            lowlight = 0;
        }
        if((adc_data > AMB_THR_NIG) && (adc_data < AMB_THR_DAY))
        {
            lowlight = -1;
        }
        if(adc_data >= AMB_THR_DAY)
        {
            lowlight = 1;
        }
        return lowlight;
    }

	//private static int GetAdcValue(int times){
	private int GetAdcValue(int times){
		int lowlight = -1;
        int adc_data = 0;
		int adc_sum = 0;
        String adc_cc;
        int [] buf = new int[times];

		for (int i=0; i<times; i++)
		{		
			try{
				FileReader adc_fr = new FileReader(ADC_FILE);
	            BufferedReader adc_br = new BufferedReader(adc_fr);
	            adc_cc = adc_br.readLine().toString();
	            adc_data = Integer.parseInt(adc_cc);
                buf[i] = adc_data;
                Thread.sleep(10);
			} catch (IOException e) {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

		Arrays.sort(buf);

        /*for (int i=0;i<times;i++)
        {
            Log.i(TAG,"buf["+i+"]"+" = "+buf[i]);
        }*/

		for (int i=times/4;i<(times/4)*3;i++)
		{
			//Log.i(TAG,"buf["+i+"]"+" = "+buf[i]);
			adc_sum += buf[i];
		}
		
		Log.i(TAG,"GetAdcValue "+times+" times, "+"sum: "+adc_sum+" avr: "+(adc_sum*2)/times);

		return ((adc_sum*2)/times);
	}

	//private static int GetAmbientBrightness()
	private int GetAmbientBrightness()
	{
		int avr = GetAdcValue(SAMPLE_TIMES);

		Log.i(TAG,"GetAmbientBrightness : "+avr);
/*
		if(avr == 4095)
		{
			return -1;
		}
		*/

		//if (Setup.currIRcutState == 1)        // 光强 cut
        if ( !getIRstate())                  // 如果已经打开了红外灯，则检测的光强阈值增加
		{
			if(avr <= AMB_THR_NIG)
	        {
	            return 0;
	        }
	        if((avr > AMB_THR_NIG) && (avr < AMB_THR_DAY))
	        {
	            return -1;
	        }
	        if(avr >= AMB_THR_DAY)
	        {
	            return 1;
	        }
		} else
	    {
			if(avr <= (AMB_THR_NIG+IRCUT_CMP))
	        {
	            return 0;
	        }
	        if((avr > (AMB_THR_NIG+IRCUT_CMP)) && (avr < (AMB_THR_DAY+IRCUT_CMP)))
	        {
	            return -1;
	        }
	        if(avr >= (AMB_THR_DAY+IRCUT_CMP))
	        {
	            return 1;
	        }
		}

        return -1;
	}
	
    public static void DelayFunction(long time){
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    public static void openIRcut(){
    	// delete by ycb
        //com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
        //com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(0);//正转
        //com.guobao.lightboard.HardwareControler.SetInfraredLaserState(1);//打开红外
        DevControl.writeGuobaoGpio(120,1);
        DevControl.writeGuobaoGpio(121,0);
        DevControl.writeGuobaoGpio(94,0);
        DevControl.writeGuobaoGpio(62,1);

    }
    public static void closeIRcut(){
    	// delete by ycb
        //com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
        //com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(1);//反转
        //HardwareControler.SetInfraredLaserState(0);//关闭红外
        DevControl.writeGuobaoGpio(120,1);
        DevControl.writeGuobaoGpio(121,0);
        DevControl.writeGuobaoGpio(94,1);
        DevControl.writeGuobaoGpio(62,0);
    }
    //public static void IRCutMotor(){
    public void IRCutMotor(){
//       int state = -1;
//       System.out.println("---->>>state "+state);
//        com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
//        state =  com.guobao.lightboard.HardwareControler.GetphotosensitiveState();//光敏检测
//        System.out.println("---------->>>state1-->"+state);
//        lastState = state;
//
//        switch (state)
//        { //判断光敏的值
//
//            case 0://光敏值为0 (光 强)
//                com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(1);//点击反转
//                com.guobao.lightboard.HardwareControler.SetInfraredLaserState(0);//关闭红外
//                Setup.currIRcutState = 1;
//                System.out.println("--------->>>点击反转");
//
//                break;
//            case 1://光敏值为1 （光 弱）
//                com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(0);//电机正转
//                com.guobao.lightboard.HardwareControler.SetInfraredLaserState(1);//打开红外
//                Setup.currIRcutState = 0;
//                System.out.println("--------->>>点击正转");
//                break;
//            case  -1:
//                return;
//        }
//        DelayFunction(2000L);
//        com.guobao.lightboard.HardwareControler.ChipPowerOFF();//断 电
//        System.out.println("------>>>延时二秒停止电机");
        //Log.i(TAG,"--------->>zhhs_IRCutMotor_powerOn_start ");
        //powerOn();
        //Log.i(TAG,"--------->>zhhs_IRCutMotor_powerOn_end ");
		/*
		 *  tag by ycb, 20180910: 
		 *  此处被主线程调用，如果在主线程中直接使用delay，则预览画面会停顿
		 */
        new Thread(){
            @Override
            public void run(){
                Loop();
            }
        }.start();
        //Log.i(TAG,"--------->>zhhs_IRCutMotor_Loop_end ");
        //HardwareControler.setGPIODirection(62, 0);
        //HardwareControler.setGPIOValue(62,0);
        //Log.i(TAG,"--------->>zhhs_IRCutMotor_end ");
    }

    //public static void Loop()
    public void Loop()
    {
        while (Setup.inFraRed &&
			//此处判断是否是屏灭且非录制状态, 20180908
			(MainActivity.getScreenState() || /*CameraHandlerThread.isRecordingGlobal*/Setup.videoRecording)
			){
            int state =-1;
            //DelayFunction(1000L);//延时很重要。(不延时一秒检测一次的话，while循环可以每秒执行几百次，)
            //state = com.guobao.lightboard.HardwareControler.GetphotosensitiveState();//光敏检测
            //state = GetAdcstate();
            // modified by ycb, simple 100 times, each takes 10ms
            //Log.i(TAG,"GetAmbientBrightness, simple 100 times, each takes 10ms");
            state = GetAmbientBrightness();

			Log.i(TAG,"loop: "+(loop_cnt++)+", GetAmbientBrightness "+state);

			/*此处有逻辑问题，造成画面停顿*/
			if ((state == -1) || (lastState == state))
			{
				//lastState = state;
                DelayFunction(500L);

			} else
			{
			
				lastState = state;

	            switch (state)
	            {
	                case 1://光敏值等于0 （光强）
	                    //IStopRecord.stopRcd();
                        //currIRcutState = false;
                        setIRstate(false);
	                    if (mStopRecord != null) {
	                        //mStopRecord.stopRcd(CameraHandlerThread.isRecordingGlobal,getIRstate());
                            mStopRecord.stopRcd(Setup.videoRecording,getIRstate());
	                    }
	                    //closeIRcut();
	                    //com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
	                    //com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(1);//反转
	                    //com.guobao.lightboard.HardwareControler.SetInfraredLaserState(0);//关闭红外
	                   // com.guobao.lightboard.HardwareControler.SetInfraredLaserState(1);//关闭红外
	                    //Setup.currIRcutState = 1;

	                    Log.i(TAG,"-------->>>zhhs_Loop-->fanzhuan 反转");

	                    break;
	                case 0: //光敏值等于1（光弱）
	                	//HardControl.cameraTuning(1);
	                    //MainActivity.sendIrCutSwBroadcase(mContext, CameraHandlerThread.isRecordingGlobal,1);
                        //currIRcutState = true;
                        setIRstate(true);
	                    if (mStopRecord != null) {
	                        //mStopRecord.stopRcd(CameraHandlerThread.isRecordingGlobal,getIRstate());
                            mStopRecord.stopRcd(Setup.videoRecording,getIRstate());
	                    }
	                    //openIRcut();
	                    //com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
	                    //com.guobao.lightboard.HardwareControler.SetIrCutMotorRotation(0);//正转
	                    //com.guobao.lightboard.HardwareControler.SetInfraredLaserState(1);//打开红外
	                    //com.guobao.lightboard.HardwareControler.SetInfraredLaserState(0);//打开红外
	                    //Setup.currIRcutState = 0;
	                    Log.i(TAG,"-------->>>zhhs_Loop-->zhengzhuan正转");

	                    break;
	            }
	            //     Delay(2000l);//延时2秒

	            //DelayFunction(2000L);
	            DelayFunction(2000L);
	            Log.i(TAG,"-------->>>zhhs_Loop-->yanshi2s_stop停止");
	            //com.guobao.lightboard.HardwareControler.ChipPowerOFF();//断电
                //DevControl.writeGuobaoGpio(62,1);
                //HardwareControler.setGPIODirection(120, 0);
	            //HardwareControler.setGPIOValue(120,0);
                DevControl.writeGuobaoGpio(120,0);
                DevControl.writeGuobaoGpio(121,0);
                DevControl.writeGuobaoGpio(94,0);

	            //HardwareControler.setGPIODirection(121, 0);
	            //HardwareControler.setGPIOValue(121,0);

	            //HardwareControler.setGPIODirection(94, 0);
	            //HardwareControler.setGPIOValue(94,0);
	            Log.i(TAG,"-------->>>zhhs_Loop-->stop停止");
	            //DelayFunction(1000L);
	            DelayFunction(500L);
			}

        }
        //HardwareControler.setGPIODirection(62, 0);
        //HardwareControler.setGPIOValue(62,0);
        DevControl.writeGuobaoGpio(62,0);

        //Log.i("GetGPIOValue", "Loop: ===============>红外线自动模式结束变成手动模式");
    }

    private static class IRcutThread extends Thread{
        private boolean irCtrl;
        private static IRcutThread mInstance = null;
        private IRcutThread(boolean irCtrl){
            this.irCtrl = irCtrl;
        }
        public static IRcutThread getInstance(boolean irCtrl){
            if (mInstance == null) {
                mInstance = new IRcutThread(irCtrl);
            }else
            {
                return null;
            }
            return mInstance;
        }

        @Override
        public void run() {
            AppLog.i(TAG,"--------->>zhhs_IRcutAction ");
			/*
			com.guobao.lightboard.HardwareControler.ChipPowerOn();//上电
			try{
				sleep(500);
			} catch (InterruptedException e){
				e.printStackTrace();
			}*/			
            if (irCtrl)
            {
                AppLog.i(TAG,"开启 ir cut");
                openIRcut();
            }else
            {
                AppLog.i(TAG,"关闭 ir cut");
                closeIRcut();
            }
            //DelayFunction(2000L);
            try {
                //sleep(2000);
                //sleep(1000);
                sleep(2000);
            } catch (InterruptedException e){
                e.printStackTrace();
            }

            //com.guobao.lightboard.HardwareControler.ChipPowerOFF();//断 电
            //DevControl.writeGuobaoGpio(120,0);
            //HardwareControler.setGPIODirection(120, 0);
            //HardwareControler.setGPIOValue(120,0);

            //HardwareControler.setGPIODirection(121, 0);
            //HardwareControler.setGPIOValue(121,0);

            //HardwareControler.setGPIODirection(94, 0);
            //HardwareControler.setGPIOValue(94,0);
            DevControl.writeGuobaoGpio(120,0);
            DevControl.writeGuobaoGpio(121,0);
            DevControl.writeGuobaoGpio(94,0);
            AppLog.i(TAG,"------>>>zhhs_yanshi2s延时二秒停止电机");

			//IRcutActionEx(irCtrl);  // 放在线程里运行，IR CUT 不能切换？

            mInstance = null;
            
        }

    }

    //public static void IRcutAction(){
    public static boolean IRcutAction(boolean irCtrl){

        IRcutThread iRcutThread = IRcutThread.getInstance(irCtrl);
        //此处必须同步，否则切换画面不匹配
        if (iRcutThread!=null){
			AppLog.i(TAG,"启动切换线程");
            iRcutThread.start();
			return true;
        }else
        {
        	AppLog.e(TAG,"IR cut busy");
			return false;
        }
    }

}
