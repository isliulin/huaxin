package com.topvision.videodemo;

import android.annotation.SuppressLint;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import com.topvision.videodemo.encoder.MediaEncoder;
import com.topvision.videodemo.rencoder.MediaMuxerRunnable;
import java.io.IOException;
import java.util.List;



@SuppressLint("NewApi")
public class CameraWrapper {

    public static   int DST_IMAGE_WIDTH = 1280;
    public static   int DST_IMAGE_HEIGHT = 720;
    public static   int SRC_IMAGE_WIDTH = 1280;
    public static   int SRC_IMAGE_HEIGHT = 720;

    private static   String TAG = "CameraWrapper";
    private static   boolean DEBUG = true;    // TODO set false on release
    private static CameraWrapper mCameraWrapper;
    /**
     * callback methods from encoder
     */
    private final MediaEncoder.MediaEncoderListener mMediaEncoderListener = new MediaEncoder.MediaEncoderListener() {
        @Override
        public void onPrepared(final MediaEncoder encoder) {
            if (DEBUG) Log.v(TAG, "onPrepared:encoder=" + encoder);
//            if (encoder instanceof MediaVideoEncoder)
//                mCameraView.setVideoEncoder((MediaVideoEncoder)encoder);
        }

        @Override
        public void onStopped(final MediaEncoder encoder) {
            if (DEBUG) Log.v(TAG, "onStopped:encoder=" + encoder);
//            if (encoder instanceof MediaVideoEncoder)
//                mCameraView.setVideoEncoder(null);
        }
    };
   // Camera.PreviewCallback previewCallback;

    public  Camera mCamera;
    private Camera.Parameters mCameraParamters;
    private boolean mIsPreviewing = false;
    private float mPreviewRate = -1.0f;
    public CameraPreviewCallback mCameraPreviewCallback;
    //    private byte[] mImageCallbackBuffer = new byte[CameraWrapper.IMAGE_WIDTH
//            * CameraWrapper.IMAGE_HEIGHT * 3 / 2];
    private boolean isBlur = false;
    //    private int openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    private int openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    boolean startRecordingFlag = false;

    private CameraWrapper() {
    }

    public static CameraWrapper getInstance() {
        if (mCameraWrapper == null) {
            synchronized (CameraWrapper.class) {
                if (mCameraWrapper == null) {
                    mCameraWrapper = new CameraWrapper();
                }
            }
        }
        return mCameraWrapper;
    }

//    private static String getSaveFilePath(String fileName) {
//        StringBuilder fullPath = new StringBuilder();
//        fullPath.append(FileUtils.getExternalStorageDirectory());
//        fullPath.append(FileUtils.getMainDirName());
//        fullPath.append("/video2/");
//        fullPath.append(fileName);
//        fullPath.append(".mp4");
//
//        String string = fullPath.toString();
//        File file = new File(string);
//        File parentFile = file.getParentFile();
//        if (!parentFile.exists()) {
//            parentFile.mkdirs();
//        }
//        return string;
//    }

    public void switchCameraId() {
        if (openCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            openCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        } else {
            openCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }
    }

    public void doOpenCamera(CamOpenOverCallback callback) {
        Log.i(TAG, "Camera open....");
        int numCameras = Camera.getNumberOfCameras();
        Camera.CameraInfo info = new Camera.CameraInfo();
        for (int i = 0; i < numCameras; i++) {
            Camera.getCameraInfo(i, info);
            if (info.facing == openCameraId) {
                mCamera = Camera.open(i);
                break;
            }
        }
        if (mCamera == null) {
            Log.d(TAG, "No front-facing camera found; opening default");
            mCamera = Camera.open();    // opens first back-facing camera
        }
        if (mCamera == null) {
            throw new RuntimeException("Unable to open camera");
        }
        Log.i(TAG, "Camera open over....");
        callback.cameraHasOpened();
    }

    public void doStartPreview(SurfaceHolder holder, float previewRate) {
        Log.i(TAG, "doStartPreview...");
        if (mIsPreviewing) {
            this.mCamera.stopPreview();
            return;
        }

        try {
            this.mCamera.setPreviewDisplay(holder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        initCamera();
    }

    public void doStartPreview(SurfaceTexture surface) {
        Log.i(TAG, "doStartPreview()");
        if (mIsPreviewing) {
            this.mCamera.stopPreview();
            return;
        }

        try {
            this.mCamera.setPreviewTexture(surface);
        } catch (IOException e) {
            e.printStackTrace();
        }
        initCamera();
    }


    /**
     * ??????camera ??????
     * create at 2017/3/23 9:53
     */
    public void doStopCamera() {
        Log.i(TAG, "doStopCamera");
        if (this.mCamera != null) {
            if (mCameraPreviewCallback != null) {
                mCameraPreviewCallback.close();
            }
            this.mCamera.setPreviewCallback(null);
            this.mCamera.stopPreview();
            this.mIsPreviewing = false;
            this.mPreviewRate = -1f;
            this.mCamera.release();
            this.mCamera = null;

            startRecordingFlag = false;
        }
    }

    public void initCamera() {
        if (this.mCamera != null) {
            this.mCameraParamters = this.mCamera.getParameters();
            this.mCameraParamters.setPreviewFormat(ImageFormat.YV12);
            this.mCameraParamters.setFlashMode("off");
            this.mCameraParamters.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO);
            this.mCameraParamters.setSceneMode(Camera.Parameters.SCENE_MODE_AUTO);
            this.mCameraParamters.setPreviewSize(SRC_IMAGE_WIDTH, SRC_IMAGE_HEIGHT);
			
            //this.mCamera.setDisplayOrientation(90);
            mCameraPreviewCallback = new CameraPreviewCallback();
//            mCamera.addCallbackBuffer(mImageCallbackBuffer);
//            mCamera.setPreviewCallbackWithBuffer(mCameraPreviewCallback);
           // mCamera.setPreviewCallback(mCameraPreviewCallback);
            List<String> focusModes = this.mCameraParamters.getSupportedFocusModes();
            for (Camera.Size size : this.mCameraParamters.getSupportedPreviewSizes()) {
                Log.e("tvLog", "width=" + size.width + "," + size.height);
            }
            if (focusModes.contains("continuous-video")) {
                this.mCameraParamters
                        .setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
            this.mCamera.setParameters(this.mCameraParamters);
            this.mCamera.startPreview();

            this.mIsPreviewing = true;
        }
    }

//
//    public void setPreviewCallback(Camera.PreviewCallback callback) {
//        previewCallback = callback;
//    }

    public interface CamOpenOverCallback {
        public void cameraHasOpened();
    }

    public class CameraPreviewCallback implements Camera.PreviewCallback {

        private CameraPreviewCallback() {
            //startRecording();
        }

        public void close() {
            stopRecording();
        }


        /**
         * ????????????
         * create at 2017/3/22 17:10
         */
        public void startRecording() {
            startRecordingFlag = true;
            MediaMuxerRunnable.startMuxer();

        }


        /**
         * ????????????
         * create at 2017/3/22 17:10
         */
        public void stopRecording() {
            startRecordingFlag = false;
            MediaMuxerRunnable.stopMuxer();
        }

        byte[] dstYuv = new byte[CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2];
        int count = 0;

        /**
         * ???????????????????????????
         * ?????????data???NV21??????
         * create at 2017/3/22 17:59
         */
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {

            //?????????????????????????????????????????????????????????
            if (startRecordingFlag) {
                //??????????????????
//                if (count % 2 == 0) {
                MediaMuxerRunnable.addVideoFrameData(data);
                //  }
                count++;

            } else {
                count = 0;
            }
//            camera.addCallbackBuffer(data);
        }
    }

    public CameraPreviewCallback getmCameraPreviewCallback() {
        return mCameraPreviewCallback;
    }

    public void setmCameraPreviewCallback(CameraPreviewCallback mCameraPreviewCallback) {
        this.mCameraPreviewCallback = mCameraPreviewCallback;
    }

}
