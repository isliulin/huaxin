package com.topvision.videodemo;

import android.app.Activity;
import android.content.Context;

import android.graphics.SurfaceTexture;
 import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import com.topvision.videodemo.cpu.CpuUtil;

import tech.shutu.jni.YuvUtils;

public class MainActivity extends Activity implements CameraWrapper.CamOpenOverCallback {

    CameraTexturePreview mCameraTexturePreview;
    public static final String TAG = "MainActivity";
    public static Context context;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e(TAG, "onCreate");
        //全屏
        getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);


        setContentView(R.layout.activity_blurred);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        mCameraTexturePreview = (CameraTexturePreview) findViewById(R.id.camera_textureview);
        context = this;


        findViewById(R.id.startAndStop).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startAndStop(v);
            }
        });

        findViewById(R.id.switchCamera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switchCamera(v);
            }
        });


        findViewById(R.id.btn_resolu1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                CameraWrapper.getInstance().mCamera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 1920;
                CameraWrapper.SRC_IMAGE_HEIGHT = 1080;
                CameraWrapper.DST_IMAGE_WIDTH  = 1920;
                CameraWrapper.DST_IMAGE_HEIGHT = 1080;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraWrapper.getInstance().initCamera();
            }
        });

        findViewById(R.id.btn_resolu2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraWrapper.getInstance().mCamera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 1280;
                CameraWrapper.SRC_IMAGE_HEIGHT = 720;
                CameraWrapper.DST_IMAGE_WIDTH  = 1280;
                CameraWrapper.DST_IMAGE_HEIGHT = 720;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraWrapper.getInstance().initCamera();
            }
        });

        findViewById(R.id.btn_resolu3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraWrapper.getInstance().mCamera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 640;
                CameraWrapper.SRC_IMAGE_HEIGHT = 480;
                CameraWrapper.DST_IMAGE_WIDTH  = 640;
                CameraWrapper.DST_IMAGE_HEIGHT = 480;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraWrapper.getInstance().initCamera();

            }
        });
        findViewById(R.id.btn_resolu4).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraWrapper.getInstance().mCamera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 320;
                CameraWrapper.SRC_IMAGE_HEIGHT = 240;
                CameraWrapper.DST_IMAGE_WIDTH  = 320;
                CameraWrapper.DST_IMAGE_HEIGHT = 240;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraWrapper.getInstance().initCamera();
            }
        });







        openCamera();

    }




    public void switchCamera(View view) {
        CameraWrapper.getInstance().doStopCamera();
        CameraWrapper.getInstance().switchCameraId();
        openCamera();
    }





    public void startAndStop(View view) {
        time = System.currentTimeMillis();
        String tag = (String) view.getTag();
        if (tag.equalsIgnoreCase("stop")) {
            CameraWrapper.getInstance().getmCameraPreviewCallback().stopRecording();
            CameraWrapper.getInstance().mCamera.setPreviewCallback(null);
            // CameraWrapper.getInstance().doStopCamera();
            view.setTag("start");
            ((TextView) view).setText("开始");
            hideMessage();

        } else {
            CameraWrapper.getInstance().mCamera.setPreviewCallback(CameraWrapper.getInstance().mCameraPreviewCallback);
            CameraWrapper.getInstance().getmCameraPreviewCallback().startRecording();
            //  openCamera();
            view.setTag("stop");
            ((TextView) view).setText("停止");
            showMessage();
        }
    }



    private void openCamera() {
        Thread openThread = new Thread() {
            @Override
            public void run() {
                CameraWrapper.getInstance().doOpenCamera(MainActivity.this);
            }
        };
        openThread.start();
    }

    @Override
    public void cameraHasOpened() {
        //等
        SurfaceTexture surface = null;
        while (true) {
            surface = this.mCameraTexturePreview.getSurfaceTexture();
            if (surface != null) {
                break;
            }
        }

        CameraWrapper.getInstance().doStartPreview(surface);
    }


    Button cpuInfo;



    /**
     * 显示cpu使用率
     * create at 2017/3/23 11:08
     */
    private void showMessage() {
        cpuInfo = (Button) findViewById(R.id.cpu_info);
        handlerTime.sendEmptyMessageDelayed(1, 1000);
    }


    private void hideMessage() {
        time = 0;
        handlerTime.removeMessages(1);
    }

    long time = 0;
    Handler handlerTime = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 1) {// 计时
                cpuInfo.setText("cpu:" + CpuUtil.getUsedPercentValue(MainActivity.this) + ",time:" + (transformHMS(System.currentTimeMillis() + 1000 - time)));
                handlerTime.sendEmptyMessageDelayed(1, 1000);
            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        CameraWrapper.getInstance().doStopCamera();
    }

    /**
     * 转换为时分秒格式
     * create at 2017/3/27 10:18
     */
    public static String transformHMS(long elapsed) {
        int hour, minute, second, milli;

        //milli = (int) (elapsed % 1000);
        elapsed = elapsed / 1000;

        second = (int) (elapsed % 60);
        elapsed = elapsed / 60;

        minute = (int) (elapsed % 60);
        elapsed = elapsed / 60;

        hour = (int) (elapsed % 60);

        return String.format("%02d:%02d:%02d", hour, minute, second);
    }
}
