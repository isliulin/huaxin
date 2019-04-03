package gbpe.policeass.utils;


import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.topvision.videodemo.CameraWrapper;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.VideoRecord.VideoManager6;
import gbpe.policeass.fragments.SettingFragment;
import tech.shutu.jni.YuvUtils;


public class CameraSurfaceView extends SurfaceView
            implements SurfaceHolder.Callback  {

        private SurfaceHolder holder;
        private Context context;


        public CameraSurfaceView(Context context) {
            super(context);
            this.context =context;
            init();
        }

        public CameraSurfaceView(Context context, AttributeSet attrs) {
            super(context, attrs);
            this.context =context;
            init();
        }

        @Override
        public void surfaceCreated(final SurfaceHolder holder) {

            YuvUtils.releaseMemo();
            YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                    CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);

            Log.e("SURFACE", "surface is Created.");
            if (CameraHandlerThread.camera != null) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            Thread.sleep(400);
                            Log.e("SURFACE", "===============>摄像头被初始化过了90");
							// 此处需要判断camera是否为空
							if (CameraHandlerThread.getInstance().camera != null){
                            	CameraHandlerThread.getInstance().camera.stopPreview();//这一句很重要，不然重新初始化摄像头也没什么用的
							}
                            CameraHandlerThread.setHolder(holder);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            } else {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            CameraHandlerThread.getCameraInstance();
                            CameraHandlerThread.getInstance().camera.stopPreview();//这一句很重要，不然重新初始化摄像头也没什么用的
                            CameraHandlerThread.setHolder(holder);
                            if (CameraHandlerThread.camera == null) {
                                ToastUtil.toast(context, "摄像头异常2");
                                return;
                            }

                            //-------------------------------------------------------初始化配置信息---------------------------------------------------------------------------------
                            Log.i("SURFACE", "===============>摄像头第一次被初始化 ");
                            SettingFragment.passCamera(CameraHandlerThread.camera);//传递摄像头给PreferenceFragment

                            //-------------------------------------------------------初始化配置信息-------------------------------------------------------------------------------------
                        } catch (Exception e) {
                            if (CameraHandlerThread.camera != null) {
                                CameraHandlerThread.camera.release();
                                CameraHandlerThread.camera = null;
                            } else {
                                ToastUtil.toast(context, "摄像头异常3");
                            }
                        }
                    }
                }).start();

            }
        }


        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Log.e("SURFACE","surface is changed.");
        }




        @Override
        public void surfaceDestroyed(SurfaceHolder holder){
            Log.e("SURFACE", "surface is destroyed.");
            YuvUtils.releaseMemo();
            if (CameraHandlerThread.camera != null) {
                //防止出现RuntimeException
                holder.removeCallback(this);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        CameraHandlerThread.camera.stopPreview();   //停止预览
//                       CameraHandlerThread.camera.release();       //释放Camera资源
//                        CameraHandlerThread.camera = null;
                    }
                }).start();
            }
        }

        private void init() {
            holder = this.getHolder();
            holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
            holder.addCallback(this);
            // 设置分辨率
            //holder.setFixedSize(1280,720);
            // 设置该组件让屏幕不会自动关闭
          //  holder.setKeepScreenOn(true);
        }

    public SurfaceHolder getSurfaceHold() {

        return this.holder;
    }

}
