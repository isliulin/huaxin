package gbpe.policeass.SoundRecord;

import android.content.Context;
import android.media.MediaRecorder;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.io.IOException;

import gbpe.policeass.Photograph.PhotoManager;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.AudioStartEvent;
import gbpe.policeass.modle.AudioStopEvent;
import gbpe.policeass.modle.VideoStopEvent;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.TimeUtils;

/**
 * Created by Administrator on 2016/11/10.
 */
public class SoundRecordManager {

    public static MediaRecorder mRecorder;
    private SoundRecordManager()
    {
        throw new UnsupportedOperationException("cannot be instantiated");
    }

    /**
     * 开始录制声音
     */
    public static void startSoundRecord(Context context)
    {

        try {
            if (mRecorder == null) {
                mRecorder = new MediaRecorder();
            }



            mRecorder.setAudioSource(Setup.audioSource);//设置音频采集方式
            mRecorder.setOutputFormat(Setup.audioOutputFormat);//设置音频输出格式
            mRecorder.setAudioEncoder(Setup.setAudioEncoder);//设置音频编码方式
            mRecorder.setMaxDuration(Setup.getMaxDuration());//最长录制时间
            mRecorder.setAudioEncodingBitRate(Setup.getSampleRate());//音频采样率

            String path = SDCardUtils.getSDCardPath();
            String folder ="";
            if (Setup.isEmphasisFile){
                folder = Setup.soundPaths+Setup.emphFileFolder;
            }else
            {
                folder = Setup.soundPaths+Setup.comFileFolder;
            }

            if (path != null) {
                File dir = new File(folder);
                if (!dir.exists())
                {
                    //dir.mkdir();
                    FileUtils.mkDir(dir);
                }
                if (Setup.isEmphasisFile)
                {
                    path = dir + "/" + TimeUtils.getDate()+"IMP"+ Setup.soundFilePostfix;
                }
                else
                {
                    path = dir + "/" + TimeUtils.getDate() + Setup.soundFilePostfix;
                }
            }
            Toast.makeText(context,path, Toast.LENGTH_SHORT).show();
            mRecorder.setOutputFile(path);//设置录音文件输出路径
            mRecorder.prepare();
            mRecorder.start();
            Log.d("TAG","开始录音.");
         //   myService.writeLogs("开始录音");

//---------------------------更新麦克风的状态--------------------------------------------------------------------
            startTime = System.currentTimeMillis();
            updateMicStatus();
//---------------------------------------------------------------------------------------------------------------
            if (orl !=null){
                orl.onStart();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        EventBus.getDefault().post(new AudioStartEvent("开始录音"));
     }

    /**
     * 停止录制声音
     */
    public static void stopSoundRecord() {

        Log.d("TAG","停止录音.");
        if (mRecorder!=null){
            mRecorder.release();
            mRecorder = null;
        }
      //  myService.writeLogs("停止录音");
         if (orl !=null){
            orl.onStop();
        }
        EventBus.getDefault().post(new AudioStopEvent("停止录音"));
    }






//--------------------------------------------------------------------------------------------------

    private static final Handler mHandler = new Handler();
    private static Runnable mUpdateMicStatusTimer = new Runnable() {
        public void run() {
            updateMicStatus();
        }
    };



    private static long startTime;
    private long endTime;
    private static OnAudioStatusUpdateListener audioStatusUpdateListener;
    private static int BASE = 1;
    private static int SPACE = 100;// 间隔取样时间

    public static void setOnAudioStatusUpdateListener(OnAudioStatusUpdateListener mAudioStatusUpdateListener) {
           audioStatusUpdateListener = mAudioStatusUpdateListener;
    }

    /**
     * 更新麦克状态
     */
    private static void updateMicStatus() {

        if (mRecorder != null) {
            double ratio = (double)mRecorder.getMaxAmplitude() / BASE;
            double db = 0;// 分贝
            if (ratio > 1) {
                db = 20 * Math.log10(ratio);
                if(null != audioStatusUpdateListener) {
                    audioStatusUpdateListener.onUpdate(db,System.currentTimeMillis()-startTime);
                }
            }
            mHandler.postDelayed(mUpdateMicStatusTimer, SPACE);
        }
    }
    //麦克风录制状态监听回调接口
    public interface OnAudioStatusUpdateListener {
        /**
         * 录音中...
         * @param db 当前声音分贝
         * @param time 录音时长
         */
        public void onUpdate(double db,long time);

        /**
         * 停止录音
         * @param filePath 保存路径
         */
        public void onStop(String filePath);
    }





//    //录音回调
//    mAudioRecoderUtils.setOnAudioStatusUpdateListener(new AudioRecoderUtils.OnAudioStatusUpdateListener() {  //录音回调(用于更新麦克风的状态)
//
//        //录音中....db为声音分贝，time为录音时长
//        @Override
//        public void onUpdate(double db, long time) {
//            mImageView.getDrawable().setLevel((int) (3000 + 6000 * db / 100));
//            mTextView.setText(TimeUtils.long2String(time));
//        }

    public interface OnSoundRecordListener{
        public void onStart();
        public void onStop();
    }

    public static OnSoundRecordListener getOrl() {
        return orl;
    }

    public static void setOrl(OnSoundRecordListener orl) {
        SoundRecordManager.orl = orl;
    }

    public static OnSoundRecordListener orl;


}
