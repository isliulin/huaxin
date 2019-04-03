//package com.camera.usbwebcam;
package com.camera.usbwebcam;

import java.io.IOException;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Environment;

import java.io.File;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;


import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.Queue;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Environment;
import android.util.Log;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import junit.framework.Assert;

import gbpe.policeass.utils.AppLog;


public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback, Runnable {

	private static final boolean DEBUG = true;
	private static final String TAG="WebCam";
	protected Context context;
	private SurfaceHolder holder;
    Thread mainLoop = null;
	private Bitmap bmp=null;
	private Bitmap bmp1=null;
	private boolean cameraExists=false;
	private boolean shouldStop=false;
    private boolean stopvdorec = false;

	// This definition also exists in ImageProc.h.
	// Webcam must support the resolution 640x480 with YUYV format. 
	public int IMG_WIDTH = 1280;
	public int IMG_HEIGHT = 720;
	public int framerate = 30;
	public int bitrate = IMG_WIDTH*IMG_HEIGHT*30;
	
	// /dev/videox (x=cameraId+cameraBase) is used.
	// In some omap devices, system uses /dev/video[0-3],
	// so users must use /dev/video[4-].
	// In such a case, try cameraId=0 and cameraBase=4
	private int cameraId=0;
	private int cameraId1=1;
	private int cameraBase=1;

	// The following variables are used to draw camera images.
    private int winWidth=0;
    private int winHeight=0;
    private Rect rect;
    private Rect rect2;
    private int dw, dh;
    private float rate;
  
    // JNI functions
   // public native int prepareCamera(int videoid);
    public native int prepareCameraWithBase(int camid, int videobase);
    public native void processCamera(int camid);
    public native void stopCamera(int camid);
    public native void pixeltobmp(Bitmap bitmap,int camid);
    public static native byte[] getyuv(int camid);
    public native void setimgsize(int camid, int width, int height);
	
		
    static {
    	try {
			System.loadLibrary("nmUVC");
		} catch (Exception e){
            AppLog.e (TAG,"nmUVC load error, "+e);
            e.printStackTrace();
		}

    }
    
	public CameraPreview(Context context) {
		super(context);
		this.context = context;
		if(DEBUG) Log.d(TAG,"CameraPreview constructed");
		setFocusable(true);
		
		holder = getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_NORMAL);	
	}

	public CameraPreview(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		if(DEBUG) Log.d(TAG,"CameraPreview constructed");
		setFocusable(true);
		
		holder = getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_NORMAL);	
	}


	public void StartCam2Thread(){
		Thread Cam2Thread = new Thread(new Runnable() {

			@Override
			public void run() {
			
				if(bmp1==null){
					bmp1 = Bitmap.createBitmap(IMG_WIDTH, IMG_HEIGHT, Bitmap.Config.ARGB_8888);
				
				}
				
				setimgsize(cameraId1, IMG_WIDTH, IMG_HEIGHT);
				int ret = prepareCameraWithBase(cameraId1, cameraBase);

				while (true) {
					
					rect2 = new Rect(winWidth/2,0,winWidth,winHeight);
					processCamera(cameraId1);
					pixeltobmp(bmp1,cameraId1);
					
					Canvas canvas = getHolder().lockCanvas(rect2);
					//if(DEBUG) Log.d(TAG, "rect2.left= "+rect2.left+"rect2.top= "+rect2.top+
					//		"rect2.right= "+rect2.right+"rect2.bottom= "+rect2.bottom);
					
					if (canvas != null)
					{
						// draw camera bmp on canvas
						canvas.drawBitmap(bmp1,null,rect2,null);
						
						getHolder().unlockCanvasAndPost(canvas);
					}
					
				}
			}
		});
		Cam2Thread.start();
		
	}

	
    @Override
    public void run() {
    
       while (true && cameraExists) {
        	//obtaining display area to draw a large image
        	if(winWidth==0){
        		winWidth=this.getWidth();
        		winHeight=this.getHeight();
        		}
				//if(DEBUG) Log.d(TAG,"winWidth="+winWidth+" ,winHeight"+winHeight);
        	//600 480
        	

				/*
        		if(winWidth*3/4<=winHeight){
        			dw = 0;
        			dh = (winHeight-winWidth*3/4)/2;
        			rate = ((float)winWidth)/IMG_WIDTH;
        			rect = new Rect(dw,dh,dw+winWidth-1,dh+winWidth*3/4-1);
        		}else{
        	//		
        			dw = (winWidth-winHeight*4/3)/2;
        			dh = 0;
        			rate = ((float)winHeight)/IMG_HEIGHT;
        			rect = new Rect(dw,dh,dw+winHeight*4/3 -1,dh+winHeight-1);
        		}
        	}
        	*/

			
			//rect = new Rect(0,0,winWidth/2,winHeight);
        	// obtaining a camera image (pixel data are stored in an array in JNI). And ture yuv420 to rgb pixels
        	// modified by ycb, 20180820
        	rect = new Rect(0,0,winWidth,winHeight);
        	processCamera(cameraId);
        	// camera image to bmp
        	//BitmapFactory.decodeByteArray(data, offset, length)
        	pixeltobmp(bmp,cameraId);
        	// get Canvas
            Canvas canvas = getHolder().lockCanvas(rect);
			//if(DEBUG) Log.d(TAG, "rect.left= "+rect.left+"rect.top= "+rect.top+
			//	"rect.right= "+rect.right+"rect.bottom= "+rect.bottom);
			
            if (canvas != null)
            {
            	// draw camera bmp on canvas
            	canvas.drawBitmap(bmp,null,rect,null);
				
            	getHolder().unlockCanvasAndPost(canvas);
            }
            if(shouldStop){
            	shouldStop = false;  
            	break;
            }	        
        }
    }

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		if(DEBUG) Log.d(TAG, "surfaceCreated");

		if(bmp==null){
			bmp = Bitmap.createBitmap(IMG_WIDTH, IMG_HEIGHT, Bitmap.Config.ARGB_8888);
		}

		// /dev/videox (x=cameraId + cameraBase) is used
		setimgsize(cameraId, IMG_WIDTH, IMG_HEIGHT);
		int ret = prepareCameraWithBase(cameraId, cameraBase);
		if(ret!=-1) 
			cameraExists = true;

		try {
			Runtime.getRuntime().exec("chmod 777 /dev/video/"+(cameraBase+cameraId));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
        mainLoop = new Thread(this);
        mainLoop.start();
		
		//StartCam2Thread();
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		if(DEBUG) Log.d(TAG, "surfaceChanged");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		if(DEBUG) Log.d(TAG, "surfaceDestroyed");
		if(cameraExists){
			shouldStop = true;
			while(shouldStop){
				try{ 
					Thread.sleep(100); // wait for thread stopping
				}catch(Exception e){}
			}
		}
		stopCamera(cameraId);
		//stopCamera(cameraId1);
	}   

	public void saveBitmapToSD() {  
       File path = Environment.getExternalStorageDirectory();  
       File file = new File(path.getAbsolutePath() + "/uvcvideo/", 
	   	System.currentTimeMillis() + ".jpg");
       try {  
           FileOutputStream  out = new FileOutputStream(file);  
           bmp.compress(Bitmap.CompressFormat.JPEG, 100, out);
		   out.flush();
		   out.close();
	   } catch (Exception e) {
           e.printStackTrace();  
       }
   }  

    public byte[] swapYV12toI420(byte[] yv12bytes, int width, int height) {
        byte[] i420bytes = new byte[yv12bytes.length];
        for (int i = 0; i < width*height; i++)
            i420bytes[i] = yv12bytes[i];
        for (int i = width*height; i < width*height + (width/2*height/2); i++)
            i420bytes[i] = yv12bytes[i + (width/2*height/2)];
        for (int i = width*height + (width/2*height/2); i < width*height + 2*(width/2*height/2); i++)
            i420bytes[i] = yv12bytes[i - (width/2*height/2)];
        return i420bytes;
    }

    void swapYV12toNV21(byte[] yv12bytes, byte[] nv12bytes, int width,int height) {

        int nLenY = width * height;

        int nLenU = nLenY / 4;

        System.arraycopy(yv12bytes, 0, nv12bytes, 0, width * height);

        for (int i = 0; i < nLenU; i++) {

            nv12bytes[nLenY + 2 * i + 1] = yv12bytes[nLenY + i];

            nv12bytes[nLenY + 2 * i] = yv12bytes[nLenY + nLenU + i];

        }

    }

}

