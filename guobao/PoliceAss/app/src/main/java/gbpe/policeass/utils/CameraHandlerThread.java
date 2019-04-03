package gbpe.policeass.utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.media.MediaRecorder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
//import android.telecom.VideoProfile;
//import android.telecom.VideoProfile.CameraCapabilities;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.Toast;

import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.rencoder.MediaMuxerRunnable;

import org.greenrobot.eventbus.EventBus;

import java.io.IOException;
import java.util.Calendar;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.modle.VideoStartEvent;
import gbpe.policeass.modle.VideoStopEvent;

/*
*
*		
10-26 15:58:11.043 371-955/? D/CameraClient: CAMERA_CMD_SET_DISPLAY_ORIENTATION - tid(955), (degrees, mirror)=(90, 0)
**/

/**
 * Created by Administrator on 2017/3/22.
 */
public class CameraHandlerThread extends HandlerThread {
    //Handler mHandler;
    public static Camera camera;
   private static int   cameraCount;//得到摄像头的个数
    public static SurfaceHolder mholder;
    public static  int SRC_IMAGE_WIDTH = 1280;
    public static  int SRC_IMAGE_HEIGHT = 720;
    private static final String TAG = "CameraWrapper";
    private static final boolean DEBUG = true;    // TODO set false on release
     private static Camera.Parameters mCameraParamters;
    private  static boolean mIsPreviewing = false;
    private static float mPreviewRate = -1.0f;
    public static CameraPreviewCallback mCameraPreviewCallback;
    //    private byte[] mImageCallbackBuffer = new byte[CameraWrapper.IMAGE_WIDTH
   //            * CameraWrapper.IMAGE_HEIGHT * 3 / 2];
   //    private int openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    private static int openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    //private static int openCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
    public static boolean startRecordingFlag = false;  /*此标志用以判断是否正在录像，包括录制和预录*/
	//public static boolean isRecordingGlobal = false;
	public static boolean isColorEffectMono = false;

	//private static CameraCapabilities cameraCapabilities;

    public CameraHandlerThread(String name) {
        super(name);
        start();
        //mHandler = new Handler(getLooper());
    }

    synchronized void notifyCameraOpened() {
        notify();
    }

    void openCamera() {
    	Handler handler = new Handler(getLooper());
        //mHandler.post(new Runnable() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                openCameraOriginal();
                notifyCameraOpened();
            }
        });
        try {
            wait();
        } catch (InterruptedException e) {
            AppLog.w("TAG", "wait was interrupted");
        }
    }
/*
    public static void setIsColorEffectMono(boolean isMono){
        isColorEffectMono = isMono;
    }

    public static boolean getIsColorEffectMono(){
        return isColorEffectMono;
    }
    */

    private static void openCameraOriginal() {
        try {
            camera = Camera.open(openCameraId);
            if (camera !=null && cameraOpenListener !=null){
                cameraOpenListener.onCameraOpenSuccessd();
            }
            
        } catch (Exception e) {
            AppLog.d("TAG", "camera is not available");
        }
    }

    /**
     * 获得摄像头的实例
     * @return
     */
    public static Camera getCameraInstance() {
        if (camera == null) {
            CameraHandlerThread mThread = new CameraHandlerThread("camera thread");
            synchronized (mThread) {
                mThread.openCamera();
                if (camera!=null){
                    
                }
            }
        }
        return camera;
    }


    /**
     * 切换摄像头
     */
    public static void switchCameraId() {
        int id =-2;
        if (openCameraId == Camera.CameraInfo.CAMERA_FACING_BACK)
        {

             id = findFrontCamera();
            if (id !=-1){
                openCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
            }else {
                openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
            }
        } else
        {
            id = findBackCamera();
            if (id ==-1){
                return;
            }
            openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

        doStopCamera();
        getCameraInstance();
        setHolder(mholder);
    }

/*
	public static void cameraTest(){
		AppLog.i(TAG, "cameraTest");
		camera.setParameters(mCameraParamters);
	}*/

    /*** 支持的照片分辨率
     *  320x240
     *  640x480
     *  1024x768
     *  1280x720
     *  1280x768
     *  1280x960
     *  1600x1200
     *  2048x1536
     *  2560x1440
     *  2560x1920
     *  3264x2448
     *  3328x1872
     *  2880x1728
     *  3600x2160
     *  5760x3456
     *  7200x4320
     * ***/

    /**
     * 初始化摄像头参数
     */
    public static synchronized void initCamera() {
    
    	AppLog.i(TAG,"initCamera");
		
        if (camera != null) {

			//AppLog.i(TAG, "camera is not null");
		
            mCameraParamters = camera.getParameters();
            mCameraParamters.setRecordingHint(true);
/*
            List<Size> sizes = mCameraParamters.getSupportedPictureSizes();
            for (int i=0;i<sizes.size();i++) {
                AppLog.i (TAG, "size: "+sizes.get(i).width+"x"+sizes.get(i).height);
            }*/
            AppLog.d(TAG, "pictureSize: "+Setup.picturewidht+"x"+Setup.pictureheight);
            mCameraParamters.setPictureSize(Setup.picturewidht, Setup.pictureheight);

            /*
            List<int[]>range = mCameraParamters.getSupportedPreviewFpsRange();
            for (int i=0;i<range.size();i++) {
                //ppLog.i (TAG, "fps["+(i/2)+"]"+"("+rang[i]+", "+rang[i+1]+")");
                int[] r = range.get(i);
                    for (int k=0;k<r.length;k++) {
                    AppLog.i (TAG, "("+i+", "+k+") "+r[k]);
                }
            }*/

            mCameraParamters.setPreviewFormat(ImageFormat.YV12);
            //mCameraParamters.setPreviewFormat(ImageFormat.);
            mCameraParamters.setFlashMode("off");

            getPreviewSizes(mCameraParamters);

            // add by ycb, 20181215
            mCameraParamters.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO);
            mCameraParamters.setSceneMode(Camera.Parameters.SCENE_MODE_AUTO);

            //mCameraParamters.setPreviewSize(Setup.picturewidht,Setup.pictureheight);
            //mCameraParamters.setPictureFormat(PixelFormat.JPEG);
            //mCameraParamters.setJpegQuality(50);
            mCameraParamters.setPictureFormat(PixelFormat.RGB_565);
            mCameraParamters.setPreviewSize(CameraWrapper.SRC_IMAGE_WIDTH, CameraWrapper.SRC_IMAGE_HEIGHT);

			/* 重复设置了
			if (Setup.picturewidht > 3600) {
				mCameraParamters.setJpegQuality(50);
				AppLog.d (TAG, "set jpeg quality 50%");
			} else {
				mCameraParamters.setJpegQuality(100);
				AppLog.d (TAG, "set jpeg quality 100%");
			}*/

			mCameraParamters.setPreviewFpsRange(30000,60000);
            
            /*try {
                mCameraParamters.setPreviewFpsRange(60000,60000);
            } catch (Exception e){
                AppLog.e (TAG, "set frame rate error, " + e.toString());
            }*/

            //mCameraParamters.set("orientation","portrait");
            //mCameraParamters.set("rotation",180);
            /* 此处加入旋转，照片的翻转角度正确，但是录制视频角度没有改变 */
            //mCameraParamters.set("rotation", 270);
            //mCameraParamters.setRecordingHint(true);
            //mCameraParamters.setRotation(90);
		    //mCameraParamters.setColorEffect(Camera.Parameters.EFFECT_NONE);
		    if (isColorEffectMono){
            	mCameraParamters.setColorEffect(Camera.Parameters.EFFECT_MONO);
		    }

             AppLog.i(TAG, "initCamera: "+CameraWrapper.SRC_IMAGE_WIDTH+" :"+CameraWrapper.SRC_IMAGE_HEIGHT);
            //camera.setDisplayOrientation(90);
            mCameraPreviewCallback = new CameraPreviewCallback();
//            mCamera.addCallbackBuffer(mImageCallbackBuffer);
//            mCamera.setPreviewCallbackWithBuffer(mCameraPreviewCallback);
            // mCamera.setPreviewCallback(mCameraPreviewCallback);
            List<String> focusModes = mCameraParamters.getSupportedFocusModes();
            /*for (Size size : mCameraParamters.getSupportedPreviewSizes()) {
                AppLog.i(TAG, "width=" + size.width + "," + size.height);
            }*/
            if (focusModes.contains("continuous-video")) {
                mCameraParamters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }

			mCameraParamters.setJpegQuality(Setup.PictureQuality);
            //mCameraParamters.setGpsLatitude();
            //mCameraParamters.setGpsLongitude();
			
            camera.setParameters(mCameraParamters);
            try {
                camera.setPreviewDisplay(mholder);
                camera.startPreview();
                mIsPreviewing = true;
            } catch (Exception e) {
                e.printStackTrace();
            }

        }else 
        {
			AppLog.i(TAG,"camera is null");
		}
    }

	public static void setColorEffect(boolean isMono){
        AppLog.i(TAG, "current effect: "+ camera.getParameters().getColorEffect());
		//AppLog.i(TAG, "set color effect: "+(isMono?"mono":"none"));
		if (isMono){
			mCameraParamters.setColorEffect(Camera.Parameters.EFFECT_MONO);			
		}else{
			mCameraParamters.setColorEffect(Camera.Parameters.EFFECT_NONE);
		}
		camera.setParameters(mCameraParamters);
		isColorEffectMono = isMono;
	}


    /**
     * 查找前置摄像头
     *
     * @return
     */
    public  static int findFrontCamera() {
        int cameraCount = 0;
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        cameraCount = Camera.getNumberOfCameras(); // get cameras number

        for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
            Camera.getCameraInfo(camIdx, cameraInfo); // get camerainfo
            if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                // 代表摄像头的方位，目前有定义值两个分别为CAMERA_FACING_FRONT前置和CAMERA_FACING_BACK后置
                return camIdx;
            }
        }
        return -1;
    }

    /**
     * 查找后置摄像头
     *
     * @return
     */
    public static   int findBackCamera() {
        int cameraCount = 0;
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        cameraCount = Camera.getNumberOfCameras(); // get cameras number

        for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
            Camera.getCameraInfo(camIdx, cameraInfo); // get camerainfo
            if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK) {
                // 代表摄像头的方位，目前有定义值两个分别为CAMERA_FACING_FRONT前置和CAMERA_FACING_BACK后置
                return camIdx;
            }
        }
        return -1;
    }




    public static class CameraPreviewCallback implements Camera.PreviewCallback
    {
        byte[] dstYuv ;
        private CameraPreviewCallback() {
            //startRecording();
            dstYuv = new byte[CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2];
           // dstYuv = new byte[1920*1080* 3 / 2];
        }

        public void close() {
            stopRecording();
        }


        /**
         * 开始录制
         * create at 2017/3/22 17:10
         */
        public void startRecording() {
            EventBus.getDefault().post(new VideoStartEvent("开始录制视频"));
            startRecordingFlag = true;
            camera.setPreviewCallbackWithBuffer(mCameraPreviewCallback);
            camera.addCallbackBuffer(dstYuv);
            MediaMuxerRunnable.startMuxer();
			// 这里设置一个这个标志什么意思？逻辑混乱？
            //Setup.isEmphasisFile = false;
        }


        /**
         * 停止录制
         * create at 2017/3/22 17:10
         */
        public void stopRecording() {
            EventBus.getDefault().post(new VideoStopEvent("停止录制视频"));
            startRecordingFlag = false;

            // 息屏预录在此出错，camera 为null
            if (camera != null) {
                camera.setPreviewCallback(null);
            }
			// 这里这样设置，停止录制后，屏幕就不动了
            //camera.stopPreview();                     // add by ycb, 20181210
            //camera.release();
            //camera = null;
            MediaMuxerRunnable.stopMuxer();
            //AppLog.d (TAG, "stopPreview, release camera");
        }



        /**
         * 照相机预览数据回调
         * 这边的data是NV21格式
         * create at 2017/3/22 17:59
         */
        //private int firstFrameTimeStamp = 0;
		//private int currentFrameTimeStamp = 0;
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
/*
			int timeElaspe;
			currentFrameTimeStamp = (int)System.currentTimeMillis();
			timeElaspe = currentFrameTimeStamp - firstFrameTimeStamp;
			firstFrameTimeStamp = currentFrameTimeStamp;
			AppLog.i (TAG, "interval: "+timeElaspe);
			camera.addCallbackBuffer(data);*/

//            //当启动录制的视频把视频源数据加入编码中
            if (startRecordingFlag) {
                //做适当的丢帧
//                if (count % 2 == 0) {
                //CameraHandlerThread.camera.addCallbackBuffer(data);
                MediaMuxerRunnable.addVideoFrameData(data);
				// 此处，重复写一帧，1920x1080 帧率在24帧左右, 320x240 在46帧，所以此处需要重新计算
				// 会导致程序无法响应？
				//MediaMuxerRunnable.addVideoFrameData(data);
                camera.addCallbackBuffer(data);
                //  }
               // count++;

            } else {
            //    count = 0;
            }
//            camera.addCallbackBuffer(data);

        }


    }


    public static CameraPreviewCallback getmCameraPreviewCallback() {
        return mCameraPreviewCallback;
    }

    public void setmCameraPreviewCallback(CameraPreviewCallback mCameraPreviewCallback) {
        mCameraPreviewCallback = mCameraPreviewCallback;
    }



    /**
     * 释放camera 资源
     * create at 2017/3/23 9:53
     */
    public static synchronized void doStopCamera(){
        AppLog.i(TAG, "doStopCamera");
        if (camera != null) {

            if (mCameraPreviewCallback != null) {
                mCameraPreviewCallback.close();
            }
            camera.setPreviewCallback(null);
            camera.stopPreview();
            mIsPreviewing = false;
            mPreviewRate = -1f;
            camera.release();
            camera = null;

            startRecordingFlag = false;
        }
    }

	/* add by ycb, check camera delay 1 second */
	private static class CamChkThread extends Thread
    {
		private Context mContext;

		public CamChkThread(Context context)
		{
			mContext = context;
		}

		@Override
		public void run()
		{
            try {
                sleep(1000);
				if (camera == null)	{
					Toast.makeText(mContext, R.string.camera_exception, Toast.LENGTH_SHORT).show();
				}
            } catch (Exception e)  {//catch (InterruptedException e) {
                e.printStackTrace();
            }
            
		}
	}

    public static boolean isCameraOpened(Context context, boolean isDoubleCheck){
        if (camera !=null){
            return true;
        }else {
            //Toast.makeText(context, "摄像头异常1", Toast.LENGTH_SHORT).show();
            if (isDoubleCheck) {
	            Toast.makeText(context, R.string.camera_setup_tips,Toast.LENGTH_SHORT).show();
				Thread thread = new CamChkThread(context);
				thread.start();
            }
            return false;
        }
    }

/*
    public void doStartPreview(SurfaceHolder holder, float previewRate) {
        AppLog.i(TAG, "doStartPreview...");
        if (mIsPreviewing) {
            this.camera.stopPreview();
            return;
        }

        try {
            this.camera.setPreviewDisplay(holder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        initCamera();
    }*/



    //=================================================================================================================
    public CameraOpenListener getCameraOpenListener() {
        return cameraOpenListener;
    }

    public void setCameraOpenListener(CameraOpenListener cameraOpenListener) {
        this.cameraOpenListener = cameraOpenListener;
    }

    private static CameraOpenListener cameraOpenListener;
    public interface CameraOpenListener{
        public void onCameraOpenSuccessd();
        public void onCamearOpenFailed();
    }

   //=================================================================================================================

    public static SurfaceHolder getHolder() {
        return mholder;
    }

    public  static void setHolder(SurfaceHolder holder) {
        mholder = holder;
        AppLog.i(TAG, "setHolder" );
        initCamera();
    }



    private static CameraHandlerThread cameraHandlerThread;
    public static CameraHandlerThread getInstance() {
        if (cameraHandlerThread == null) {
            synchronized (CameraWrapper.class) {
                if (cameraHandlerThread == null) {
                    cameraHandlerThread = new CameraHandlerThread("CameraHandlerThread");
                }
            }
        }
        return cameraHandlerThread;
    }


    /**
     * 视频录制监听接口
      * @return
     */

    public OnVideoRecordListener getRecordListener() {
        return recordListener;
    }

    public void setRecordListener(OnVideoRecordListener recordListener) {
        this.recordListener = recordListener;
    }

    static OnVideoRecordListener recordListener;

    /**
     * 视频录制监听接口
     */
    public interface OnVideoRecordListener{
        public void onVideoStartRecord();
        public void onVideoStopRecord();
    }


    public  static void getPreviewSizes(Camera.Parameters params){
        List<Size> ShowSizes = params.getSupportedPreviewSizes();
        int showLength = ShowSizes.size();

        for (int i = 0; i < showLength; i++)
        {
            AppLog.i(TAG,"getPreviewSizes: "+ShowSizes.get(i).width+":"+ShowSizes.get(i).height);
        }
    }

}




