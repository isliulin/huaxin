package com.topvision.videodemo.rencoder;

import android.location.Location;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;


import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.util.GpsUtils;
import com.topvision.videodemo.util.VideoSet;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Vector;

import tech.shutu.jni.YuvUtils;

/**
 * Created by robi on 2016-04-01 10:50.
 */
public class VideoRunnable extends Thread {
    public static boolean DEBUG = true;

    private static final String TAG = "VideoRunnable";
    // private static final boolean VERBOSE = true; // lots of logging
    private static final boolean VERBOSE = false; // lots of logging
    // parameters for the encoder
    private static final String MIME_TYPE = "video/avc"; // H.264 Advanced Video
    private static final int FRAME_RATE = 30; // 15fps
    private static final int IFRAME_INTERVAL = 1; // 1 between
    // I-frames
    private static final int TIMEOUT_USEC = 10000;
    private static final int COMPRESS_RATIO = 256;
    //private static final int BIT_RATE = CameraWrapper.DST_IMAGE_HEIGHT * CameraWrapper.DST_IMAGE_WIDTH * 3 * 8 * FRAME_RATE / COMPRESS_RATIO; // bit rate CameraWrapper.
    //private static final int BIT_RATE = 1280 * 720 * 7; // 精细
    //private static final int BIT_RATE = 1920 *1080 * 7; // 精细
    private final Object lock = new Object();
    //
    private byte[] mFrameData;
    private Vector<byte[]> frameBytes;
    private int mWidth;
    private int mHeight;
    private MediaCodec mMediaCodec;
    private MediaCodec.BufferInfo mBufferInfo;
    private int mColorFormat;
    private long mStartTime = 0;
    private volatile boolean isExit = false;
    private WeakReference<MediaMuxerRunnable> mediaMuxerRunnable;
    private MediaFormat mediaFormat;
    private MediaCodecInfo codecInfo;
    private volatile boolean isStart = false;
    private volatile boolean isMuxerReady = false;
    public static boolean isAddOSD = true;
    public static boolean isDEBUG = false;

    public VideoRunnable(int mWidth, int mHeight, WeakReference<MediaMuxerRunnable> mediaMuxerRunnable) {
        this.mWidth = mWidth;
        this.mHeight = mHeight;
        this.mediaMuxerRunnable = mediaMuxerRunnable;
        frameBytes = new Vector<byte[]>();
		// 设置MediaFormat参数
        prepare();
    }


    /**
     * 选择合适的color
     * create at 2017/3/22 18:24
     */
    private static int selectColorFormat(MediaCodecInfo codecInfo,
                                         String mimeType) {
        MediaCodecInfo.CodecCapabilities capabilities = codecInfo
                .getCapabilitiesForType(mimeType);
        for (int i = 0; i < capabilities.colorFormats.length; i++) {
            int colorFormat = capabilities.colorFormats[i];
         //   Log.e("tvLog", "" + colorFormat);
            if (isRecognizedFormat(colorFormat)) {
                return colorFormat;
            }
        }
        if (DEBUG) Log.e(TAG,
                "couldn't find a good color format for " + codecInfo.getName()
                        + " / " + mimeType);
        return 0; // not reached
    }


    /**
     * 是否是官方提供的顏色類別
     * create at 2017/3/22 18:25
     */
    private static boolean isRecognizedFormat(int colorFormat) {
        switch (colorFormat) {
            // these are the formats we know how to handle for this test
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedSemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_TI_FormatYUV420PackedSemiPlanar:
                return true;
            default:
                return false;
        }
    }


    /**
     * 通过编码类型获取MediaCodecInfo对象
     * create at 2017/3/22 18:24
     */
    private static MediaCodecInfo selectCodec(String mimeType) {
        int numCodecs = MediaCodecList.getCodecCount();
        for (int i = 0; i < numCodecs; i++) {
            MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);
            if (!codecInfo.isEncoder()) {
                continue;
            }
            String[] types = codecInfo.getSupportedTypes();
            for (int j = 0; j < types.length; j++) {
                if (types[j].equalsIgnoreCase(mimeType)) {
                    return codecInfo;
                }
            }
        }
        return null;
    }


    public void exit() {
        isExit = true;
    }

    public void add(byte[] data) {
        if (frameBytes != null && isMuxerReady) {
            frameBytes.add(data);
        }
    }

    /**
     * 准备一些需要的参数
     * create at 2017/3/22 18:13
     */
    private void prepare() {
        if (DEBUG) Log.i(TAG, "VideoEncoder()");

        mFrameData = new byte[this.mWidth * this.mHeight * 3 / 2];
        mBufferInfo = new MediaCodec.BufferInfo();

        codecInfo = selectCodec(MIME_TYPE);
        if (codecInfo == null) {
            if (DEBUG) Log.e(TAG, "Unable to find an appropriate codec for " + MIME_TYPE);
            return;
        }
        if (VERBOSE)
            if (DEBUG) Log.d(TAG, "found codec: " + codecInfo.getName());

        printSupportColorFormat(codecInfo, MIME_TYPE);
        mColorFormat = selectColorFormat(codecInfo, MIME_TYPE);
        if (VERBOSE)
            if (DEBUG) Log.d(TAG, "found colorFormat: " + mColorFormat);
        //mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE,this.mHeight,//注意这里旋转后有一个大坑，就是要交换mHeight，和mWidth的位置。否则录制出来的视频时花屏的。
                //this.mWidth);
        mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE,this.mWidth,//注意这里旋转后有一个大坑，就是要交换mHeight，和mWidth的位置。否则录制出来的视频时花屏的。
                this.mHeight);        
		Log.d(TAG,"bitrate: "+VideoSet.BITE_RATE+", frameRate: "+VideoSet.frameRate+", color format: "+mColorFormat);
        //mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, VideoSet.BITE_RATE);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, VideoSet.BITE_RATE);
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE,  VideoSet.frameRate);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, mColorFormat);
        //设置关键帧的时间
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);
        if (VERBOSE)
            if (DEBUG) Log.d(TAG, "format: " + mediaFormat);
    }

    private void startMediaCodec() throws IOException {
        mMediaCodec = MediaCodec.createByCodecName(codecInfo.getName());
        mMediaCodec.configure(mediaFormat, null, null,
                MediaCodec.CONFIGURE_FLAG_ENCODE);
        mMediaCodec.start();

        isStart = true;
    }

    private void printSupportColorFormat(MediaCodecInfo codecInfo, String MIME_TYPE) {
        MediaCodecInfo.CodecCapabilities capabilities = codecInfo
                .getCapabilitiesForType(MIME_TYPE);
        for (int i = 0; i < capabilities.colorFormats.length; i++) {
            int colorFormat = capabilities.colorFormats[i];
           // Log.e("colorFormat", "" + colorFormat);
        }
    }

    private void stopMediaCodec() {
        if (mMediaCodec != null) {
            mMediaCodec.stop();
            mMediaCodec.release();
            mMediaCodec = null;
        }
        isStart = false;
        if (DEBUG) Log.e("angcyo-->", "stop video 录制...");
    }

    public synchronized void restart() {
        isStart = false;
        isMuxerReady = false;
        frameBytes.clear();
    }

    public void setMuxerReady(boolean muxerReady) {
        synchronized (lock) {
            if (DEBUG)
                Log.e("angcyo-->", Thread.currentThread().getId() + " video -- setMuxerReady..." + muxerReady);
            isMuxerReady = muxerReady;
            lock.notifyAll();
        }
    }

    private void encodeFrame(byte[] input/* , byte[] output */) {
        if (VERBOSE)
            if (DEBUG) Log.i(TAG, "encodeFrame()");
      if (isDEBUG)    VideoSet.printLog();
//==============================================加水印或者不加水印===================================================================================================

        if (isAddOSD){
            //YuvUtils.scaleAndRotateYV12ToI420(input,mFrameData,mWidth,mHeight,90,mHeight,mWidth);
            YuvUtils.scaleYV12ToI420(input,mFrameData,mWidth,mHeight,mWidth,mHeight);
			/*
            if (!(TextUtils.isEmpty(""+VideoSet.latitude)&&TextUtils.isEmpty(""+VideoSet.longitude))){
                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mHeight,mWidth,new SimpleDateFormat
                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,VideoSet.longitude+":"+VideoSet.latitude);
//                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mHeight,mWidth,new SimpleDateFormat
//                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,"22 44 17:113 54 46 ");
            }else {
                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mHeight,mWidth,new SimpleDateFormat
                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,"-:-");
            }*/
            DecimalFormat df = new DecimalFormat("#.00000");
			if (!(TextUtils.isEmpty(""+VideoSet.latitude)&&TextUtils.isEmpty(""+VideoSet.longitude))){
                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mWidth,mHeight,new SimpleDateFormat
                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,df.format(VideoSet.longitude)+":"+df.format(VideoSet.latitude));
//                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mHeight,mWidth,new SimpleDateFormat
//                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,"22 44 17:113 54 46 ");
            }else {
                com.teligen.yuvosd.YuvUtils.addOsdToI420(mFrameData,mWidth,mHeight,new SimpleDateFormat
                        ("yyyy-MM-dd HH:mm:ss").format(new Date())+" "+ VideoSet.policeNum+" "+VideoSet.devNumber,"-:-");
            }

        }else {
            //YuvUtils.scaleAndRotateYV12ToI420(input,mFrameData,mWidth,mHeight,90,mHeight,mWidth);
            YuvUtils.scaleYV12ToI420(input,mFrameData,mWidth,mHeight,mWidth,mHeight);
        }
//=====================================================================================================================================================================

        ByteBuffer[] inputBuffers = mMediaCodec.getInputBuffers();
        ByteBuffer[] outputBuffers = mMediaCodec.getOutputBuffers();
        int inputBufferIndex = mMediaCodec.dequeueInputBuffer(TIMEOUT_USEC);
        if (VERBOSE)
            if (DEBUG) Log.i(TAG, "inputBufferIndex-->" + inputBufferIndex);
        if (inputBufferIndex >= 0) {
            long endTime = System.nanoTime();
            long ptsUsec = (endTime - mStartTime) / 1000;
            if (VERBOSE)
                if (DEBUG) Log.i(TAG, "resentationTime: " + ptsUsec);
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
            inputBuffer.clear();
            inputBuffer.put(mFrameData);
            mMediaCodec.queueInputBuffer(inputBufferIndex, 0,
                    mFrameData.length, System.nanoTime() / 1000, 0);
        } else {
            // either all in use, or we timed out during initial setup
            if (VERBOSE)
                if (DEBUG) Log.d(TAG, "input buffer not available");
        }

        int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
        if (VERBOSE)
            if (DEBUG) Log.i(TAG, "outputBufferIndex-->" + outputBufferIndex);
        do {
            if (outputBufferIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                outputBuffers = mMediaCodec.getOutputBuffers();
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                MediaFormat newFormat = mMediaCodec.getOutputFormat();
                MediaMuxerRunnable mediaMuxerRunnable = this.mediaMuxerRunnable.get();
                if (mediaMuxerRunnable != null) {
                    mediaMuxerRunnable.addTrackIndex(MediaMuxerRunnable.TRACK_VIDEO, newFormat);
                }

                if (DEBUG)
                    Log.e("angcyo-->", "添加视轨 INFO_OUTPUT_FORMAT_CHANGED " + newFormat.toString());
            } else if (outputBufferIndex < 0) {
            } else {
                if (VERBOSE)
                    if (DEBUG) Log.d(TAG, "perform encoding");
                ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
                if (outputBuffer == null) {
                    throw new RuntimeException("encoderOutputBuffer " + outputBufferIndex +
                            " was null");
                }
                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // The codec config data was pulled out and fed to the muxer when we got
                    // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                    if (VERBOSE) if (DEBUG) Log.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG");
                    mBufferInfo.size = 0;
                }
                if (mBufferInfo.size != 0) {
                    MediaMuxerRunnable mediaMuxerRunnable = this.mediaMuxerRunnable.get();

                    if (mediaMuxerRunnable != null && !mediaMuxerRunnable.isVideoAdd()) {
                        MediaFormat newFormat = mMediaCodec.getOutputFormat();
                        if (DEBUG) Log.e("angcyo-->", "添加视轨  " + newFormat.toString());
                        mediaMuxerRunnable.addTrackIndex(MediaMuxerRunnable.TRACK_VIDEO, newFormat);
                    }
                    // adjust the ByteBuffer values to match BufferInfo (not needed?)
                    outputBuffer.position(mBufferInfo.offset);
                    outputBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                    if (mediaMuxerRunnable != null && mediaMuxerRunnable.isMuxerStart()) {
                        mediaMuxerRunnable.addMuxerData(new MediaMuxerRunnable.MuxerData(
                                MediaMuxerRunnable.TRACK_VIDEO, outputBuffer, mBufferInfo
                        ));
                    }
                    if (VERBOSE) {
                        if (DEBUG) Log.d(TAG, "sent " + mBufferInfo.size + " frameBytes to muxer");
                    }
                }
                mMediaCodec.releaseOutputBuffer(outputBufferIndex, false);
            }
            //outputBufferIndex = mMediaCodec.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
			outputBufferIndex = mMediaCodec.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
        } while (outputBufferIndex >= 0);
    }


    @Override
    public void run() {
        //线程中的状态标识,停止线程则将isExit设为true即可
        while (!isExit) {
            //标识是否需要开启硬编码
            if (!isStart) {
                stopMediaCodec();
                if (!isMuxerReady) {
                    synchronized (lock) {
                        try {
                            if (DEBUG) Log.e("ang-->", "video -- 等待混合器准备...");
                            lock.wait();
                        } catch (InterruptedException e) {
                        }
                    }
                }
                //判断音视频混合器是否已准备,如果已准备则开启视频硬编码
                if (isMuxerReady) {
                    try {
                        if (DEBUG) Log.e("angcyo-->", "video -- startMediaCodec...");
                        startMediaCodec();
                    } catch (IOException e) {
                        isStart = false;
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e1) {
                        }
                    }
                }

            } else if (!frameBytes.isEmpty()) {
                byte[] bytes = this.frameBytes.remove(0);
//                if(DEBUG) Log.e("ang-->", "解码视频数据:" + bytes.length);
                try {
                    encodeFrame(bytes);
                    //Log.v(TAG,"encodeFrame");
                } catch (Exception e) {
                    if (DEBUG) Log.e("angcyo-->", "解码视频(Video)数据 失败");
                    e.printStackTrace();
                }
            }
        }

//
        if (DEBUG) Log.e("angcyo-->", "Video 录制线程 退出...");
    }


    /**
     * 监听录制视频的时候的位置变化，如果变化了则，写入变化的位置。
     */



}
