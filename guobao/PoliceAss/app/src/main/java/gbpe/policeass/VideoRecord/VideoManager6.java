package gbpe.policeass.VideoRecord;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import com.orhanobut.logger.Logger;
import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.util.VideoSet;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Set;

import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.modle.OnGetLocationEvent;
import gbpe.policeass.modle.OnStartCountEvent;
import gbpe.policeass.modle.OnStopCountEvent;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.CameraHandlerThread;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.views.TimeIndicator;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/21 10:51
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/21 10:51
 * <p/>
 * 描述：视频录制类，包括预录，分段录，延时录 等逻辑的处理
 * =====================================================================================
 */



public class VideoManager6 {
    public static long  startTime =0l;
    private  static long  stopTime  =0;

    public static final String TAG = "VideoManager6";
	public static final int MSG_GET_VIDEO_DURATION = 0x111;
	public static final int MSG_NOT_ENOUGH_STORAGE = 0x112;
	public static final int MSG_VIDEO_EXCHANGE_FILE = 0x113;

	public static String mPreRcdPath1;
	public static String mPreRcdPath2;
	public static int    mPreRcdIndex = 0;

    public static int stopVideoDelayFlag = -1;//0 记录延时录像停止的次数（第一次按下停止的时候置为0）

    private VideoManager6() {
    }

	/*几种录制的逻辑糅杂在一起，看得想吐*/

    /**
     * 开始录制视频
     */
    //public static void startRecord()
    // modified by ycb, 20180903
    //public static String startRecord(){
    public static String startRecord(String filePath){
         boolean state = false;
        Log.e(TAG, "======================>startRecord: ");
        if (state = videoDelayStop()){
            return null;
        }

        startTime = System.currentTimeMillis();
        ++Setup.titalFilesNum;

		// tag by ycb, here indicate the file path
        //if (filePath == null) {
        //    VideoSet.videoFilePath = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);
        //}else{
        //    VideoSet.videoFilePath = filePath;
        //}
        //VideoSet.videoFilePath = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);

        VideoSet.devNumber = Setup.devNum;
        VideoSet.policeNum = Setup.policeNumber;
        VideoSet.frameRate = Setup.frameRate;
        //Log.e(TAG, "======================>startRecord: "+VideoSet.videoFilePath);

        //VideoSet.BITE_RATE = Setup.VideoQuality;
        VideoSet.BITE_RATE = CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 7;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   判断是预录还是分段录制的逻辑处理
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

        if (CameraHandlerThread.camera==null){
            return null;
        }
        Log.e(TAG, "======================>isNormalRecording: "+Setup.isNormalRecording);
        if (!Setup.isNormalRecording)  //没有按键按下
        {
            if (Setup.VideoPrerecord){
                //videoPrerecord();//预录
                // modified by ycb, 20180920 pre record use unique suffix
                VideoSet.videoFilePath = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix) + ".prv";
				
                if (mPreRcdIndex == 0){
                    if (mPreRcdPath1!=null) {
                        File f = new File(mPreRcdPath1);
                        if (f.exists()) {
                            f.delete();
                        }
                    }
					
					mPreRcdPath1 = VideoSet.videoFilePath;
					mPreRcdIndex = 1;
				} else {
                    if (mPreRcdPath2!=null) {
                        File f = new File(mPreRcdPath2);
                        if (f.exists()) {
                            f.delete();
                        }
                    }
					mPreRcdPath2 = VideoSet.videoFilePath;
					mPreRcdIndex = 0;
				}
				videoPrerecord();//预录, 实际为启动预录定时器
				AppLog.d(TAG, "开始预录"+VideoSet.videoFilePath);
            }
        } else if (Setup.isNormalRecording)
        {
        	// modified by ycb, 20180920 预录文件采用".prv"后缀
        	//VideoSet.videoFilePath = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);	
        	if (filePath == null) {
	            VideoSet.videoFilePath = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);
	        }else{
	            VideoSet.videoFilePath = filePath;
	        }
			segmentdVideo(); //分段录
            /*if (Setup.isSegmentedVideo){//有按键按下，分段录制
                segmentdVideo(); //分段录
            }*/
        }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

        //EventBus.getDefault().post(new OnGetLocationEvent("获取经纬度"));
        AppLog.i(TAG, "startRecord: ======>>>>>正式开始了" );
        //VideoSet.longitude = Setup.longitude;
        //VideoSet.latitude = Setup.latitude;
        CameraHandlerThread.getInstance().getmCameraPreviewCallback().startRecording();//开始录制视频

		return VideoSet.videoFilePath;

    }


    public static String startRecord(){
        return startRecord(null);
    }

    /**
     * 停止录制视频
     */
    public static void stopRecord(boolean isPause){

     if (Setup.isVideoDelay&& Setup.isNormalRecording)
     {//延时录像，真正开始录像之后延时录像才生效，预录的时候不生效
         if (stopVideoDelayFlag ==0){
             handler3.removeCallbacks(videoDelayRun);
             stopAll();
			 currentRec = 0;
             EventBus.getDefault().post(new OnStopCountEvent("停止倒计时"));
             Setup.videoRecording = false;
             Setup.isNormalRecording = false;
             AppLog.v(TAG, "stopRecord: ======>停止倒计时" );
             //previewRecord();
         }else {
             stopVideoDelayFlag = 0;
             handler3.postDelayed(videoDelayRun,Setup.VideoDelay);
             EventBus.getDefault().post(new OnStartCountEvent("开始倒计时"));
             AppLog.v(TAG, "stopRecord: ======>开始倒计时" );
         }
     }else
     {
         stopAll();
         currentRec = 0;
         AppLog.v(TAG, "stopRecord: ======>立刻停止" );
     }
    }

	public static String getPreRcdFilePath(){
		if (mPreRcdIndex == 0){
			// 此时录制的文件为path2，需合并path1
			return mPreRcdPath1;
		} else {
			return mPreRcdPath2;
		}
	}

	public static String getRcdFilePath(){
		if (mPreRcdIndex == 1){
			// 此时录制的文件为path2，需合并path1
			return mPreRcdPath1;
		} else {
			return mPreRcdPath2;
		}
	}

	public static void deletePreRecordFile(){
	/*
		if (mPreRcdPath1!=null) {
	        File f = new File(mPreRcdPath1);
	        if (f.exists()) {
	            f.delete();
	        }
	    }
		if (mPreRcdPath2!=null) {
			File f = new File(mPreRcdPath2);
			if (f.exists()) {
				f.delete();
			}
		}*/
        String path = FileUtils.getFileFolderPathByDate(App.instance);
        File folder = new File(path);
        if ((!folder.isDirectory())||(!folder.exists())){
            return;
        }
        File[] files = folder.listFiles();
        for (File f : files) {
            if (f.getName().endsWith(".prv")){
                f.delete();
            }
        }
	
	}

	public static void clearPreFile(){
		mPreRcdIndex = 0;
		deletePreRecordFile();
		mPreRcdPath1 = null;
		mPreRcdPath2 = null;
	}

    public static void stopAll(){

        AppLog.v(TAG, "stopAll: ==========>全部结束" );
        if (timeRun !=null){
            handler.removeCallbacks(timeRun);
        }
        try {
            CameraHandlerThread.getInstance().getmCameraPreviewCallback().stopRecording();//停止录制视频
            stopTime = System.currentTimeMillis();
        } catch (Exception e) {
            AppLog.e (TAG,"stopRecorder error: "+e.toString());
            e.printStackTrace();
        }

		AppLog.d(TAG, "start at: "+startTime+", stop at: "+stopTime);
		AppLog.d(TAG, "path: "+VideoSet.videoFilePath);

		// modified by ycb
		/*
        if (Setup.VideoPrerecord)
        {
            if (stopTime-startTime<1000){//如果停止录制的时候，文件录制时间小于1秒则直接删除。
                new File( VideoSet.videoFilePath).delete();
                startTime = stopTime=0;
            }
            //yulu();
            //设置了预录模式并且正常录制结束后有按键按下
            if (Setup.isNormalRecording){

                if (!FileUtils.getFileNum()){
                    return;
                }
//                //先保存预录下面的视频文件的绝对路径,在下一次预录之前应该先保存这些视频，否则，Setup.pre_file_dir
               VideoCombineUtil.mix(Setup.pre_file_dir+File.separator);
                //每次正式录像完成，然后从新开始预录的时候，应该改变预录文件夹的路径
                ++Setup.fconnt;
                Logger.i("Setup.fconnt"+Setup.fconnt);

				// 预录文件夹名称
                Setup. pre_file_dir = "/storage/sdcard0/VID_"+
                        gbpe.policeass.utils.TimeUtils.milliseconds2String5(System.currentTimeMillis())+"_"+Setup.fconnt;
                Logger.i("Setup.pre_file_dir"+Setup.pre_file_dir);
            }
        }*/
    }

    /**
     * 延时录像第二次停止时执行的操作
     * @return
     */
    public static boolean   videoDelayStop(){
        if (stopVideoDelayFlag ==0){
            AppLog.v(TAG, "===========>videoDelayStop: " );
            Setup.videoRecording = true;
            stopRecord(false);
            stopVideoDelayFlag = -1;
            return true;
        }
        return false;
    }


    static long currentRec;
    public static   Runnable timeRun = new Runnable() {
        @Override
        public void run() {

            currentRec++;
            handler.postDelayed(timeRun, 1000);//用于计时的线程(秒计时器)

			AppLog.v(TAG, "currentRec: "+currentRec+", videoMaxDuration: "+Setup.videoMaxDuration/1000);

            if (SDCardUtils.judgeSDCardFreeSizeIsPlenty(App.instance))
            {
                //如果是分段录制
                if (Setup.isNormalRecording)
                {
                    //if (Setup.isSegmentedVideo )
                    //{
                    //if (currentRec > Setup.SegmentedVideo/1000)
                    if (currentRec > Setup.videoMaxDuration / 1000)
                    //if (currentRec > Setup.videoMaxDuration / 10000)
                    {//计时判断,设置录制时间(定时器)
                        currentRec = 0;
						AppLog.i(TAG,"达到设定时长");
                        //mHandler.sendEmptyMessage(0x111);//定时时间到则向主线程发送消息
                        mHandler.sendEmptyMessage(MSG_VIDEO_EXCHANGE_FILE);
                    }
                    //}
                } else
                //if (!Setup.isNormalRecording)
                {
                	//如果是预录
                    if (Setup.VideoPrerecord)
                    {
                        if (currentRec>Setup.videoPrerecordTime)
                        {
                            //设置录制时间(定时器)
                            currentRec = 0;
                            //mHandler.sendEmptyMessage(0x111);//定时时间到则向主线程发送消息
                            mHandler.sendEmptyMessage(MSG_GET_VIDEO_DURATION);
                        }
                    }
                }
            }else // (不充足的意思是SD卡剩余空间不够再录制一个视频了)
            {
                if (currentRec>2)//每隔两秒检测一下sd卡上的剩余空间
                {
                    currentRec =0;
                    //mHandler.sendEmptyMessage(0x112);
                    mHandler.sendEmptyMessage(MSG_NOT_ENOUGH_STORAGE);
                }
            }
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

    static Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                //case 0x111:
                case MSG_GET_VIDEO_DURATION:
                //SD卡充足的时候
                    SDCardIsEnoughRecord(false);
                    break;

                //case 0x112:
                case MSG_NOT_ENOUGH_STORAGE:
                //sd卡将要满的时候,达到了设定值
                    SDCardIsNotEnoughRecord();
					// 显示内存不足消息，语音通知
                    break;

				case MSG_VIDEO_EXCHANGE_FILE:
                    SDCardIsEnoughRecord(true);
				    break;
            }
        }
    };

//
    /**
     * SD卡充足的情况录制
     */
    private  static void SDCardIsEnoughRecord(boolean isRcd){
        //if (Setup.isSegmentedVideo&&Setup.isNormalRecording){
        if (Setup.isNormalRecording) {
            stopAll();
        }else {
            stopRecord(false);
        }

		// delete by ycb
        //DelayTimeFunction(500l);
        if (SDCardUtils.judgeSDCardFreeSizeIsPlenty(App.instance))
        {
        	AppLog.v(TAG,"重新开始录制，"+Setup.videoPrerecordTime);
        	if (isRcd) {
        	    // 接着之前的时间显示
        	    MainActivity.setRecordTick(TimeIndicator.getTimeTick());
            } else {
                MainActivity.setRecordTick(Setup.videoPrerecordTime);
            }
            startRecord();
        }else
        {//此时内存已经达到设定值，在(mHandlerz中)开启定时器监视内存变化
            Toast.makeText(App.instance ,"SDCard 不足", Toast.LENGTH_SHORT).show();
        }
    }
    /**
     * SD卡不足的情况下录制
     */
    private static   void SDCardIsNotEnoughRecord(){
        //SD卡上的空间小于10MB的话就自动结束录像(剩余空间<=10 MB)
        if (SDCardUtils.getFloatSDCardFreeSize(App.instance)<10.0f)
        {//如果sd卡剩余内存小于0.1g则结束录制
            stopRecord(false);
            //停止计时
            handler.removeCallbacks(timeRun);
        }
    }



    static long lastTime;
    /**
     * 非线程延时函数
     * @param time 延时时间
     */
    public static void DelayTimeFunction(Long  time ) {
        long MIN_DELAY_TIME = time;//最低延时时间
        //获取当前系统时间
        lastTime = Calendar.getInstance().getTimeInMillis();
        while (true)
        {
            //每一次循环都获取当前系统时间
            long currentTime = Calendar.getInstance().getTimeInMillis();
            if (currentTime - lastTime >= MIN_DELAY_TIME)
            {
                lastTime = currentTime;
                break;
            }
        }
    }

	public static void interruptPreRcd(){
		if (timeRun!=null){
			handler.removeCallbacks(timeRun);
		}
	}


    /**
     * 判断是否为分段录制
     *
     */
    public  static void segmentdVideo(){
        //判断是否为分段录制
        //if (Setup.isSegmentedVideo) {
        Log.e(TAG,"==================>正在分段录制...");
        handler.post(timeRun);
        //
    }

    /**
     * 判断是否为预录
     */
    public  static void videoPrerecord(){
    	handler.post(timeRun);
/*
        if (!Setup.isNormalRecording)
        {
            if (Setup.VideoPrerecord)
            {
                handler.post(timeRun);
                Log.e("TAG", "=====================>正在预录...");
            }
        }
*/
    }
    /**
     * 判断是否预录，如果是，就进行预录
     */
    public static void previewRecord() {
        //启动界面判断是否进行预录
        if (Setup.VideoPrerecord) {
          //  DelayTimeFunction(1000l);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    Setup.isNormalRecording = false;
                    VideoManager6.startRecord();
                }
            },1500);

        }
    }


    /**
     * 启动定时器，定时时间到文件保存完后再调用此方法 ,预录四十秒，十秒钟保存一个文件
     */
    private static void yulu(){
        // 123456_456_20100101093309_0002.mp4           123456_456_20100101093552_0003IMP.mp4
       // File file = new File(App.instance.getFilesDir().getAbsolutePath());
        Logger.e("yulu =="+Setup.pre_file_dir);
        File file = new File(Setup.pre_file_dir);
        //文件是按照日期来排序的
        File [] fileNum = file.listFiles();
        if (fileNum==null){
            return;
        }
        if (fileNum.length<=0){
            return;
        }
        //每次判断如果文件超过十个（4个）,删除第一个
        if (fileNum.length>4)
        {
            Arrays.sort(fileNum);
            fileNum[0].delete();
        }
    }


    //===================================延时录像逻辑处理===============================================================================

    static Handler handler3 = new Handler();
    public static   Runnable videoDelayRun = new Runnable() {
        @Override
        public void run() {
            Log.e(TAG, "run: ===========>>>定时时间到" );
            stopAll();
            stopVideoDelayFlag = -1;
            EventBus.getDefault().post(new OnStopCountEvent("停止倒计时"));
            Setup.videoRecording = false;//让按键状态回到开始录制的状态
            Setup.isNormalRecording = false;
            previewRecord();
         }
    };


}
