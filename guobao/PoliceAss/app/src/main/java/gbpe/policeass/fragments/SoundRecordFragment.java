package gbpe.policeass.fragments;

import android.app.Fragment;
import android.graphics.Color;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.AudioStartEvent;
import gbpe.policeass.modle.AudioStopEvent;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.TimeUtils;

/**
 * Created by Administrator on 2017/4/14.
 */
public class SoundRecordFragment extends Fragment {
    private MediaRecorder mRecorder;

    private Button mRecorderBtn;
    private Button mStopBtn;

    private TextView mStatusTextView;
    private TextView mTimeTextView;

    //private LinearLayout mVolumeLayout;
    private static final String TAG = "SoundRecordFragment";

    private MyHandler mHandler = new MyHandler();

    private static String mFilepath;
    /**
     * 开始录制时将所有的文件都保存在该容器中
     */
    private List<File> mTmpFile = new ArrayList<File>();
    private int mSeagments = 1;
    private MediaRecorderState mRecordState = MediaRecorderState.STOPPED;

    private enum MediaRecorderState {
        STOPPED, RECORDING, PAUSED
    }


    ImageView imageView;

    View view;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        view = inflater.inflate(R.layout.fragment_sound_record,null);

        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        if (getActivity().getIntent() != null && getActivity().getIntent().getExtras() != null) {
        }

//        if (Setup.isEmphasisFile){
//            mFilepath = Setup.soundPaths+"101ANDRO/";
//        }else {
//            mFilepath =  Setup.soundPaths+"100ANDRO/" ;
//        }
//
         mFilepath = FileUtils.getFileFolderPath(getActivity());
        File file = new File(mFilepath);
        if (!file.exists()){
            FileUtils.mkDir(file);
        }


        mStatusTextView = (TextView) view.findViewById(R.id.recording_tip_tv_id);
        mStatusTextView.setText(R.string.voice_record_note);
        mStatusTextView.setTextColor(Color.MAGENTA);

        mTimeTextView = (TextView) view.findViewById(R.id.recording_time_tv_id);
        mRecorderBtn = (Button) view.findViewById(R.id.recorder_btn_id);
        mRecorderBtn.setOnClickListener(new ButtonOnClickListener());

        mStopBtn = (Button) view.findViewById(R.id.stop_btn_id);
        mStopBtn.setOnClickListener(new ButtonOnClickListener());
        mStopBtn.setEnabled(false);
        imageView = (ImageView)view. findViewById(R.id.iv_voice);
        Thread t = new Thread(new HandlerInvocation());
        t.start();
    }
    @Override
    public void onStop() {

		AppLog.i (TAG, "onStop");
    	
        /*if (mRecorder != null && mRecordState != MediaRecorderState.STOPPED) {
            stopRecording();
        }*/
        
        super.onStop();
    }

	public void start(boolean on){
		if (on) {
			startRecording();
			//mStatusTextView.setText(R.string.voice_recording_tips);
            //mStatusTextView.setTextColor(Color.GREEN);
		} else {
			stopRecording();
		}
	}

    private void startRecording() {
        ++Setup.titalFilesNum;
        mRecordState = MediaRecorderState.RECORDING;

        File file = new File(getSoundFilePath());

        mTmpFile.add(file);
        mSeagments++;
        if (file.exists()) {
            if (file.delete())
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
        } else {
            try {
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        mRecorder = new MediaRecorder();
        mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mRecorder.setOutputFormat(MediaRecorder.OutputFormat.AMR_NB);
        mRecorder.setOutputFile(file.getAbsolutePath());
        mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        mRecorder.setOnErrorListener(new MediaRecorder.OnErrorListener() {
            @Override
            public void onError(MediaRecorder mr, int what, int extra) {
                mRecorder.reset();
            }
        });
        try {
            mRecorder.prepare();
            mRecorder.start();
            updateMicStatus();
            Thread t = new Thread(new DbThread());
            t.start();
        } catch (Exception e) {
            e.printStackTrace();
            mRecorder.release();
        } finally {

        }

        EventBus.getDefault().post(new AudioStartEvent("audiostart"));
    }

    /**
     * 暂停录制
     */
    private void pauseRecording() {
        mRecordState = MediaRecorderState.PAUSED;
        if (mRecorder != null) {
            mRecorder.stop();
            mRecorder.release();
            mRecorder = null;
        }
    }

    /**
     * 停止录制
     */
    private void stopRecording() {

		AppLog.i (TAG,"stopRecording");	
        mRecordState = MediaRecorderState.STOPPED;
        timeCount = 0;
        if (mRecorder != null) {
            mRecorder.stop();
            mRecorder.release();
            mRecorder = null;
        }

        if (mTmpFile.size()<=0){
            return;
        }
        File finalFile = new File( getSoundFilePath());
        if (!finalFile.exists()) {
            try {
                finalFile.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        FileOutputStream fileOutputStream = null;
        try {
            fileOutputStream = new FileOutputStream(finalFile);
        } catch (IOException e) {
            e.printStackTrace();
        }
        for (int i = 0; i < mTmpFile.size(); i++) {
            File tmpFile = mTmpFile.get(i);
            FileInputStream fis = null;
            try {
                fis = new FileInputStream(tmpFile);
                byte[] tmpBytes = new byte[fis.available()];
                int lenght = tmpBytes.length;
                if (i == 0) {
                    while (fis.read(tmpBytes) != -1) {
                        fileOutputStream.write(tmpBytes, 0, lenght);
                    }
                } else {
                    while (fis.read(tmpBytes) != -1) {
                        fileOutputStream.write(tmpBytes, 6, lenght - 6);
                    }
                }
                fileOutputStream.flush();
                fis.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                fis = null;
            }
        }
        try {
            if (fileOutputStream != null)
                fileOutputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            fileOutputStream = null;
        }
        for (File f : mTmpFile)
            f.delete();
        mTmpFile.clear();
        mSeagments = 1;

        EventBus.getDefault().post(new AudioStopEvent("audiostop"));
    }


    class ButtonOnClickListener implements View.OnClickListener {
        @Override
        public void onClick(View paramView) {
            switch (paramView.getId()) {
                case R.id.recorder_btn_id:
                    mStopBtn.setEnabled(true);
                    if (mRecordState == MediaRecorderState.STOPPED || mRecordState == MediaRecorderState.PAUSED) {
                        startRecording();
                        // mRecorderBtn.setImageResource(R.drawable.stop);
                        mStatusTextView.setText(R.string.voice_recording_tips);
                        mStatusTextView.setTextColor(Color.GREEN);
                        mRecorderBtn.setText(R.string.pause);
                    } else if (mRecordState == MediaRecorderState.RECORDING) {
                        pauseRecording();
                        // mRecorderBtn.setImageResource(R.drawable.start);
                        mStatusTextView.setText(R.string.voice_pause_tips);
                        mStatusTextView.setTextColor(Color.GRAY);

                        mRecorderBtn.setText(R.string.start);
                    }
                    break;
                case R.id.stop_btn_id:
                    stopRecording();
                    mStopBtn.setEnabled(false);
                    // mRecorderBtn.setImageResource(R.drawable.start);
                    mStatusTextView.setText(R.string.voice_recording_stop_tips);
                    mStatusTextView.setTextColor(Color.RED);
                    Toast.makeText(getActivity(), R.string.save_ok_tips, Toast.LENGTH_SHORT).show();
                    mRecorderBtn.setText(R.string.start);
                    break;


            }

        }
    }

    private int timeCount = 0;


    class MyHandler extends Handler {
        public MyHandler() {
        }

        public MyHandler(Looper L) {
            super(L);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == -1) {
                int minute = timeCount / 60;
                int second = timeCount % 60;
                String min = minute >= 10 ? minute + "" : "0" + minute;
                String sec = second >= 10 ? second + "" : "0" + second;
                mTimeTextView.setText(min + ":" + sec);
            }


        }
    }

    class HandlerInvocation implements Runnable {
        @Override
        public void run() {
            while (true) {
                if (mRecordState == MediaRecorderState.RECORDING) {
                    Message msg = new Message();
                    msg.what = -1;
                    SoundRecordFragment.this.mHandler.sendMessage(msg);
                    timeCount++;
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }


    //--------------------------------------------------------------------------------------------------
    //                获得录音分贝线程控制线程
    //--------------------------------------------------------------------------------------------------

    class DbThread implements Runnable {
        @Override
        public void run() {
            while (true) {
                int db = 0;
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (mRecorder != null && mRecordState == MediaRecorderState.RECORDING) {
                    try {
                        double f = 10 * Math.log10(mRecorder.getMaxAmplitude());
                        if (f < 0) {
                            db = 0;
                        } else {
                            db = (int) (f * 2);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                        continue;
                    }
                    Message msg = new Message();
                    msg.what = db;
                    SoundRecordFragment.this.mHandler.sendMessage(msg);
                } else {
                    Message msg = new Message();
                    msg.what = db;
                    SoundRecordFragment.this.mHandler.sendMessage(msg);
                    break;
                }
            }
        }
    }


    /**
     * 更新麦克状态
     */
    private void updateMicStatus() {

        if (mRecorder != null) {
            double ratio = (double) mRecorder.getMaxAmplitude() / 1;
            double db = 0;// 分贝
            if (ratio > 1) {
                db = 20 * Math.log10(ratio);
                imageView.getDrawable().setLevel((int) (3000 + 6000 * db / 100));
            }
            handler.postDelayed(mUpdateMicStatusTimer, 100);
        }
    }


    private final Handler handler = new Handler();
    private Runnable mUpdateMicStatusTimer = new Runnable() {
        public void run() {
            updateMicStatus();
        }
    };

    @Override
    public void onResume() {
        super.onResume();
        if (mRecordState == MediaRecorderState.RECORDING) {
            mStatusTextView.setText(R.string.voice_recording_tips);
            mStatusTextView.setTextColor(Color.GREEN);
        }
		/*
		mStopBtn.setEnabled(true);
        if (mRecordState == MediaRecorderState.STOPPED || mRecordState == MediaRecorderState.PAUSED) {
            startRecording();
            // mRecorderBtn.setImageResource(R.drawable.stop);
            mStatusTextView.setText(R.string.voice_recording_tips);
            mStatusTextView.setTextColor(Color.GREEN);
            mRecorderBtn.setText("Pause");
        } else if (mRecordState == MediaRecorderState.RECORDING) {
            pauseRecording();
            // mRecorderBtn.setImageResource(R.drawable.start);
            //mStatusTextView.setText("录音已暂停");
            mStatusTextView.setText(R.string.voice_recording_stop_tips);
            mStatusTextView.setTextColor(Color.GRAY);

            mRecorderBtn.setText("Start");
        }*/
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopRecording();
        mStopBtn.setEnabled(false);
        // mRecorderBtn.setImageResource(R.drawable.start);
        mStatusTextView.setText(R.string.voice_recording_stop_tips);
        mStatusTextView.setTextColor(Color.RED);
        //Toast.makeText(getActivity(), "保存成功", Toast.LENGTH_SHORT).show();
        Toast.makeText(getActivity(), R.string.save_ok_tips, Toast.LENGTH_SHORT).show();
        mRecorderBtn.setText(R.string.start);
     }

    /**
     * 获取文件路径 修改布局
     * @return
     */
    private String getSoundFilePath(){
        String str = "";
//        if (Setup.isEmphasisFile){
//            str =  mFilepath+ TimeUtils.
//                    milliseconds2String2(System.currentTimeMillis())+"IMP."+Setup.soundFilePostfix;
//        }
//        else
//        {
//            str = mFilepath+TimeUtils.
//                    milliseconds2String2(System.currentTimeMillis())+"."+Setup.soundFilePostfix;
//        }
        str = FileUtils.getFilePath2(getActivity(),Setup.soundFilePostfix);
        return str;
    }





}

