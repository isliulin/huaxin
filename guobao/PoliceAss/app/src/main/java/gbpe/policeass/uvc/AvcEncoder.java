//package com.camera.usbwebcam;
package gbpe.policeass.uvc;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import android.annotation.SuppressLint;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;

import android.os.Environment;
import android.util.Log;

import com.camera.usbwebcam.CameraPreview;


public class AvcEncoder
{
	private final static String TAG = "MeidaCodec";
	
	private int TIMEOUT_USEC = 12000;
	private int cameraId=0;

	private MediaCodec mediaCodec;
	int m_width;
	int m_height;
	int m_framerate;
	byte[] m_info = null;
	 
	public byte[] configbyte; 
    private MediaMuxer mMuxer;
    private int mTrackIndex;
    private boolean mMuxerStarted;
	private final boolean VERBOSE = true;

	private static String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test1.h264";
	//private static String mp4path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test1.mp4";
	private static String mp4path;
	private BufferedOutputStream outputStream;

	@SuppressLint("NewApi")
	public AvcEncoder(int width, int height, int framerate, int bitrate, String path) { 
		
		m_width  = width;
		m_height = height;
		m_framerate = framerate;
		mp4path = path;
	
	    MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", width, height);
		/*qcom*/
	   // mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);   

		/*mtk*/
		mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);   
		
	    mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, width*height*5);
	    mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 30);
	    mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
		
	    try {
			mediaCodec = MediaCodec.createEncoderByType("video/avc");

		    mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
		    mediaCodec.start();
		    createfile();

			Log.i(TAG, "path: "+mp4path);

			// Create a MediaMuxer.  We can't add the video track and start() the muxer here,
	        // because our MediaFormat doesn't have the Magic Goodies.  These can only be
	        // obtained from the encoder after it has started processing data.
	        //
	        // We're not actually interested in multiplexing audio.  We just want to convert
	        // the raw H.264 elementary stream we get from MediaCodec into a .mp4 file.
	        mMuxer = new MediaMuxer(mp4path,
	                MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        mTrackIndex = -1;
        mMuxerStarted = false;
	}
	
	//FileOutputStream outStream;
	private void createfile(){
		File file = new File(path);
		if(file.exists()){
			file.delete();
		}
	    try {
	        outputStream = new BufferedOutputStream(new FileOutputStream(file));
	    } catch (Exception e){ 
	        e.printStackTrace();
	    }
	}

	@SuppressLint("NewApi")
	private void StopEncoder() {
	    try {
	        mediaCodec.stop();
	        mediaCodec.release();
	    } catch (Exception e){ 
	        e.printStackTrace();
	    }
	}
	
	ByteBuffer[] inputBuffers;
	ByteBuffer[] outputBuffers;

	public boolean isRuning = false;
	
	public void StopThread(){
		isRuning = false;
        try {
        	StopEncoder();
			outputStream.flush();
	        outputStream.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        if (mMuxer != null) {
            // TODO: stop() throws an exception if you haven't fed it any data.  Keep track
            //       of frames submitted, and don't call stop() if we haven't written anything.
            mMuxer.stop();
            mMuxer.release();
            mMuxer = null;
        }
	}
	
	int count = 0;

/*
	public void StartEncoderThread(){
		Thread EncoderThread = new Thread(new Runnable() {

			@SuppressLint("NewApi")
			@Override
			public void run() {
				isRuning = true;
				byte[] input = null;
				long pts =  0;
				long generateIndex = 0;

				while (isRuning) {

					input = CameraPreview.getyuv();
					
					if (input != null) {
						try {
							long startMs = System.currentTimeMillis();
							ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
							ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
							int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
							if (inputBufferIndex >= 0) {
								pts = computePresentationTime(generateIndex);
								ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
								inputBuffer.clear();
								inputBuffer.put(input);
								mediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, pts, 0);
								generateIndex += 1;
							}
							
							MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
							int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
							while (outputBufferIndex >= 0) {
								//Log.i("AvcEncoder", "Get H264 Buffer Success! flag = "+bufferInfo.flags+",pts = "+bufferInfo.presentationTimeUs+"");
								ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
								byte[] outData = new byte[bufferInfo.size];
								outputBuffer.get(outData);
								if(bufferInfo.flags == 2){
									configbyte = new byte[bufferInfo.size];
									configbyte = outData;
								}else if(bufferInfo.flags == 1){
									byte[] keyframe = new byte[bufferInfo.size + configbyte.length];
									System.arraycopy(configbyte, 0, keyframe, 0, configbyte.length);
									System.arraycopy(outData, 0, keyframe, configbyte.length, outData.length);
									
									outputStream.write(keyframe, 0, keyframe.length);
								}else{
									outputStream.write(outData, 0, outData.length);
								}

								mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
								outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
							}

						} catch (Throwable t) {
							t.printStackTrace();
						}
					} else {
						try {
							Thread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
				}
			}
		});
		EncoderThread.start();
		
	}
	*/

	public void StartEncoderThread(){
		Thread EncoderThread = new Thread(new Runnable() {

			@SuppressLint("NewApi")
			@Override
			public void run() {
				isRuning = true;
				byte[] input = null;
				long pts =	0;
				long generateIndex = 0;

				while (isRuning) {

					/* 获得一帧原始数据 */
					input = CameraPreview.getyuv(cameraId);
					
					if (input != null) {
						try {
							long startMs = System.currentTimeMillis();
							ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
							ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
							/* 返回一个output buffer */
							int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
							if (inputBufferIndex >= 0) {
								/* 计算每帧间隔时间*/
								pts = computePresentationTime(generateIndex);
								ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
								inputBuffer.clear();
								inputBuffer.put(input);
								mediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, pts, 0);
								generateIndex += 1;
							}
							
							MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
							/*
							int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
							while (outputBufferIndex >= 0) {
								//Log.i("AvcEncoder", "Get H264 Buffer Success! flag = "+bufferInfo.flags+",pts = "+bufferInfo.presentationTimeUs+"");
								ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];

								byte[] outData = new byte[bufferInfo.size];
								outputBuffer.get(outData);
								if(bufferInfo.flags == 2){
									configbyte = new byte[bufferInfo.size];
									configbyte = outData;
								}else if(bufferInfo.flags == 1){
									byte[] keyframe = new byte[bufferInfo.size + configbyte.length];
									System.arraycopy(configbyte, 0, keyframe, 0, configbyte.length);
									System.arraycopy(outData, 0, keyframe, configbyte.length, outData.length);
									
									outputStream.write(keyframe, 0, keyframe.length);
								}else{
									outputStream.write(outData, 0, outData.length);
								}

								mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
								outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
							}*/

							//while (true) {
								/* 此处应该为同步读取 */
								int encoderStatus = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
								if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
									// no output available yet
									if (!isRuning) {
										break;		// out of while
									} else {
										if (VERBOSE) Log.d(TAG, "no output available, spinning to await EOS");
									}
								} else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
									// not expected for an encoder
									outputBuffers = mediaCodec.getOutputBuffers();
								} else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
									// should happen before receiving buffers, and should only happen once
									if (mMuxerStarted) {
										throw new RuntimeException("format changed twice");
									}
									MediaFormat newFormat = mediaCodec.getOutputFormat();
									Log.d(TAG, "encoder output format changed: " + newFormat);
							
									// now that we have the Magic Goodies, start the muxer
									mTrackIndex = mMuxer.addTrack(newFormat);
									mMuxer.start();
									mMuxerStarted = true;
								} else if (encoderStatus < 0) {
									Log.w(TAG, "unexpected result from encoder.dequeueOutputBuffer: " +
											encoderStatus);
									// let's ignore it
								} else {
									ByteBuffer encodedData = outputBuffers[encoderStatus];
									if (encodedData == null) {
										throw new RuntimeException("encoderOutputBuffer " + encoderStatus +
												" was null");
									}
							
									if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
										// The codec config data was pulled out and fed to the muxer when we got
										// the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
										if (VERBOSE) Log.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG");
										bufferInfo.size = 0;
									}
							
									if (bufferInfo.size != 0) {
										if (!mMuxerStarted) {
											throw new RuntimeException("muxer hasn't started");
										}
							
										// adjust the ByteBuffer values to match BufferInfo (not needed?)
										encodedData.position(bufferInfo.offset);
										encodedData.limit(bufferInfo.offset + bufferInfo.size);
							
										mMuxer.writeSampleData(mTrackIndex, encodedData, bufferInfo);
										if (VERBOSE) {
											Log.d(TAG, "sent " + bufferInfo.size + " bytes to muxer, ts=" +
													bufferInfo.presentationTimeUs);
										}
									}
							
									mediaCodec.releaseOutputBuffer(encoderStatus, false);
							
									if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
										if (!isRuning) {
											Log.w(TAG, "reached end of stream unexpectedly");
										} else {
											if (VERBOSE) Log.d(TAG, "end of stream reached");
										}
										break;		// out of while
									}
								}
							//}

						} catch (Throwable t) {
							t.printStackTrace();
						}
					} else {
						try {
							Thread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
				}
			}
		});
		EncoderThread.start();
		
	}

	private void NV21ToNV12(byte[] nv21,byte[] nv12,int width,int height){
		if(nv21 == null || nv12 == null)return;
		int framesize = width*height;
		int i = 0,j = 0;
		System.arraycopy(nv21, 0, nv12, 0, framesize);
		for(i = 0; i < framesize; i++){
			nv12[i] = nv21[i];
		}
		for (j = 0; j < framesize/2; j+=2)
		{
		  nv12[framesize + j-1] = nv21[j+framesize];
		}
		for (j = 0; j < framesize/2; j+=2)
		{
		  nv12[framesize + j] = nv21[j+framesize-1];
		}
	}
	
    /**
     * Generates the presentation time for frame N, in microseconds.
     */
    private long computePresentationTime(long frameIndex) {
        return 132 + frameIndex * 1000000 / m_framerate;
    }
}
