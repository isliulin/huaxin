package com.arcsoft.arcfacedemo.util.camera;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;

import java.io.IOException;
import java.util.List;

import gbpe.policeass.Photograph.PhotoManager;
import gbpe.policeass.utils.AppLog;


public class CameraHelper implements Camera.PreviewCallback,Camera.PictureCallback{
    private static final String TAG = "CameraHelper";
    private Camera mCamera;
    private int mCameraId;
    private DisplayMetrics metrics;
    private View previewView;
    private Camera.Size previewSize;
    private Camera.Size specificPreviewSize;
    private int displayOrientation = 0;
    private int rotation;
    private boolean isMirror = false;
    private PhotoCallback photoCallback;

    private Integer specificCameraId = null;
    private CameraListener cameraListener;

    public interface PhotoCallback{
        void onDataReady(byte[] data, Camera camera);
    }

    private CameraHelper(CameraHelper.Builder builder) {
        previewView = builder.previewDisplayView;
        specificCameraId = builder.specificCameraId;
        cameraListener = builder.cameraListener;
        rotation = builder.rotation;
        metrics = builder.metrics;
        specificPreviewSize = builder.previewSize;
        if (builder.previewDisplayView instanceof TextureView) {
            isMirror = builder.isMirror;
        } else if (isMirror) {
            throw new RuntimeException("mirror is effective only when the preview is on a textureView");
        }
    }

    public void init() {
        if (previewView instanceof TextureView) {
            ((TextureView) this.previewView).setSurfaceTextureListener(textureListener);
        } else if (previewView instanceof SurfaceView) {
            ((SurfaceView) previewView).getHolder().addCallback(surfaceCallback);
        }
        if (isMirror) {
            previewView.setScaleX(-1);
        }
    }

    public void start() {
        synchronized (this) {
            //???????????????2?????????1,1?????????0,??????ID 1????????????0?????????
            mCameraId = Camera.getNumberOfCameras() - 1;
            //??????????????????ID?????????????????????????????????????????????
            if (specificCameraId != null && specificCameraId <= mCameraId) {
                mCameraId = specificCameraId;
            }

            AppLog.d(TAG,"cam id: "+mCameraId+", specificCameraId: "+specificCameraId);

            //????????????
            if (mCameraId == -1) {
                if (cameraListener != null) {
                    cameraListener.onCameraError(new Exception("camera not found"));
                }
                return;
            }
            try {
                mCamera = Camera.open(mCameraId);
            } catch (Exception e) {
                AppLog.e (TAG, "open camera error: "+e.toString());
                return;
            }

            displayOrientation = getCameraOri(rotation);
            mCamera.setDisplayOrientation(displayOrientation);
            try {
                Camera.Parameters parameters = mCamera.getParameters();
                parameters.setPreviewFormat(ImageFormat.NV21);

                //??????????????????
                previewSize = parameters.getPreviewSize();
                List<Camera.Size> supportedPreviewSizes = parameters.getSupportedPreviewSizes();
                if (supportedPreviewSizes != null && supportedPreviewSizes.size() > 0) {
                    previewSize = getBestSupportedSize(supportedPreviewSizes, metrics);
                }
                parameters.setPreviewSize(previewSize.width, previewSize.height);

                //??????????????????
                List<String> supportedFocusModes = parameters.getSupportedFocusModes();
                if (supportedFocusModes != null && supportedFocusModes.size() > 0) {
                    if (supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                    } else if (supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                    } else if (supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                    }
                }
                mCamera.setParameters(parameters);
                if (previewView instanceof TextureView) {
                    mCamera.setPreviewTexture(((TextureView) previewView).getSurfaceTexture());
                } else {
                    mCamera.setPreviewDisplay(((SurfaceView) previewView).getHolder());
                }
                mCamera.setPreviewCallback(this);
                mCamera.startPreview();
                if (cameraListener != null) {
                    cameraListener.onCameraOpened(mCamera, mCameraId, displayOrientation, isMirror);
                }
            } catch (Exception e) {
                if (cameraListener != null) {
                    cameraListener.onCameraError(e);
                }
            }
        }
    }

    private int getCameraOri(int rotation) {
        int degrees = rotation * 90;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
            default:
                break;
        }
        int result;
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(mCameraId, info);
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            //result = (info.orientation + degrees) % 360;
            //result = (360 - result) % 360;
            result = 0;
        } else {
            //result = (info.orientation - degrees + 360) % 360;
            result = 0;
        }
        return result;
    }

    public void startPreview() {
        mCamera.startPreview();
    }

    public void stop() {
        synchronized (this) {
            if (mCamera == null) {
                return;
            }
            try {
                mCamera.setPreviewCallback(null);
                mCamera.setPreviewDisplay(null);
                mCamera.stopPreview();
                mCamera.release();
                mCamera = null;
                if (cameraListener != null) {
                    cameraListener.onCameraClosed();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public boolean isStopped() {
        synchronized (this) {
            return mCamera == null;
        }
    }

    public void release() {
        stop();
        previewView = null;
        specificCameraId = null;
        cameraListener = null;
        metrics = null;
        specificPreviewSize = null;
        previewSize = null;
    }

    private Camera.Size getBestSupportedSize(List<Camera.Size> sizes, DisplayMetrics metrics) {
        Camera.Size bestSize = sizes.get(0);
        float screenRatio = (float) metrics.widthPixels / (float) metrics.heightPixels;
        if (screenRatio > 1) {
            screenRatio = 1 / screenRatio;
        }

        for (Camera.Size s : sizes) {
            if (specificPreviewSize != null && specificPreviewSize.width == s.width && specificPreviewSize.height == s.height) {
                return specificPreviewSize;
            }
            if (Math.abs((s.height / (float) s.width) - screenRatio) < Math.abs(bestSize.height / (float) bestSize.width - screenRatio)) {
                bestSize = s;
            }
        }
        return bestSize;
    }

    @Override
    public void onPreviewFrame(byte[] nv21, Camera camera) {
        if (cameraListener != null) {
            cameraListener.onPreview(nv21, camera);
        }
    }

    private TextureView.SurfaceTextureListener textureListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
            start();
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height) {
            Log.i(TAG, "onSurfaceTextureSizeChanged: " + width + "  " + height);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
            stop();
            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

        }
    };
    private SurfaceHolder.Callback surfaceCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            start();
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            stop();
        }
    };

    public void changeDisplayOrientation(int rotation) {
        if (mCamera != null) {
            this.rotation = rotation;
            displayOrientation = getCameraOri(rotation);
            mCamera.setDisplayOrientation(displayOrientation);
            if (cameraListener != null) {
                cameraListener.onCameraConfigurationChanged(mCameraId, displayOrientation);
            }
        }
    }

    public void takePicture(PhotoCallback callback){
        photoCallback = callback;
        mCamera.takePicture(null,null,this);
    }

    @Override
    public void onPictureTaken(byte[] data, Camera camera) {
        if (photoCallback!=null) {
            photoCallback.onDataReady(data, camera);
        }
    }

    public static final class Builder {

        /**
         * ???????????????view??????????????????surfaceView???textureView
         */
        private View previewDisplayView;

        /**
         * ??????????????????????????????textureView
         */
        private boolean isMirror;
        /**
         * ???????????????ID
         */
        private Integer specificCameraId;
        /**
         * ????????????
         */
        private CameraListener cameraListener;
        /**
         * ??????????????????????????????????????????????????????
         */
        private DisplayMetrics metrics;
        /**
         * ??????getWindowManager().getDefaultDisplay().getRotation()????????????
         */
        private int rotation;
        /**
         * ??????????????????????????????????????????????????????????????????????????????
         */
        private Camera.Size previewSize;

        public Builder() {
            //specificCameraId = cam;
        }


        public Builder previewOn(View val) {
            if (val instanceof SurfaceView || val instanceof TextureView) {
                previewDisplayView = val;
                return this;
            } else {
                throw new RuntimeException("you must preview on a textureView or a surfaceView");
            }
        }


        public Builder isMirror(boolean val) {
            isMirror = val;
            return this;
        }

        public Builder previewSize(Camera.Size val) {
            previewSize = val;
            return this;
        }

        public Builder metrics(DisplayMetrics val) {
            metrics = val;
            return this;
        }

        public Builder rotation(int val) {
            rotation = val;
            return this;
        }

        public Builder specificCameraId(Integer val) {
            specificCameraId = val;
            return this;
        }

        public Builder cameraListener(CameraListener val) {
            cameraListener = val;
            return this;
        }

        public CameraHelper build() {
            return new CameraHelper(this);
        }
    }

}
