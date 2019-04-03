package gbpe.policeass.zfysim;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
 import java.util.List;
import android.app.Activity;
import android.graphics.Color;
import android.media.MediaRecorder;
import android.media.MediaRecorder.OnErrorListener;
import android.os.Bundle;
 import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
 import android.widget.ImageView;
 import android.widget.TextView;
import android.widget.Toast;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.TimeUtils;

//392f41  363b4f
public class RecordActivity extends Activity {

    private static final String tag = "RecordActivity";
    private MediaRecorder mRecorder;

    private Button mRecorderBtn;
    private Button mStopBtn;

    private TextView mStatusTextView;
    private TextView mTimeTextView;
    private ImageView iv_back;

    //private LinearLayout mVolumeLayout;

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

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        WindowManager.LayoutParams params = getWindow().getAttributes();
//        params.systemUiVisibility =View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
//        getWindow().setAttributes(params);

        this.setContentView(R.layout.activity_sound_record);

        if (this.getIntent() != null && this.getIntent().getExtras() != null) {
        }
        if (Setup.isEmphasisFile){
            mFilepath = Setup.soundPaths+"101ANDRO/";
        }else {
            mFilepath =  Setup.soundPaths+"100ANDRO/" ;
        }
        File file = new File(mFilepath);
        if (!file.exists()){
            FileUtils.mkDir(file);
        }


        mStatusTextView = (TextView) this.findViewById(R.id.recording_tip_tv_id);
        mStatusTextView.setText("点击录音按钮开始录音.");
        mStatusTextView.setTextColor(Color.MAGENTA);

        mTimeTextView = (TextView) this.findViewById(R.id.recording_time_tv_id);
        mRecorderBtn = (Button) this.findViewById(R.id.recorder_btn_id);
        mRecorderBtn.setOnClickListener(new ButtonOnClickListener());

        mStopBtn = (Button) this.findViewById(R.id.stop_btn_id);
        mStopBtn.setOnClickListener(new ButtonOnClickListener());
        mStopBtn.setEnabled(false);
        iv_back = (ImageView)findViewById(R.id.image_back);
        iv_back.setOnClickListener(new ButtonOnClickListener());
        //  mVolumeLayout = (LinearLayout)this.findViewById(R.id.volume_show_id);
        int[] location = new int[2];
        //  mVolumeLayout.getLocationOnScreen(location);
        // volumeView = new VolumeView(this,location[1]+100);
        //   mVolumeLayout.removeAllViews();
        // mVolumeLayout.addView(volumeView);
        imageView = (ImageView) findViewById(R.id.iv_voice);
        Thread t = new Thread(new HandlerInvocation());
        t.start();
    }

    @Override
    protected void onStop() {
        if (mRecorder != null && mRecordState != MediaRecorderState.STOPPED) {
            stopRecording();
        }
        super.onStop();
    }

    private void startRecording() {
        mRecordState = MediaRecorderState.RECORDING;

      //  File file = new File(mFilepath + (new Date().getTime()) + "_" + mSeagments + ".wav");
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
        mRecorder.setOnErrorListener(new OnErrorListener() {
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
       // File finalFile = new File(mFilepath + (new Date().getTime()) + ".wav");
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
    }


    class ButtonOnClickListener implements OnClickListener {
        @Override
        public void onClick(View paramView) {
            switch (paramView.getId()) {
                case R.id.recorder_btn_id:
                    mStopBtn.setEnabled(true);
                    if (mRecordState == MediaRecorderState.STOPPED || mRecordState == MediaRecorderState.PAUSED) {
                        startRecording();
                        // mRecorderBtn.setImageResource(R.drawable.stop);
                        mStatusTextView.setText("正在进行录音...");
                        mStatusTextView.setTextColor(Color.GREEN);
                        mRecorderBtn.setText("Pause");
                    } else if (mRecordState == MediaRecorderState.RECORDING) {
                        pauseRecording();
                        // mRecorderBtn.setImageResource(R.drawable.start);
                        mStatusTextView.setText("录音已暂停");
                        mStatusTextView.setTextColor(Color.GRAY);

                        mRecorderBtn.setText("Start");
                    }
                    break;
                case R.id.stop_btn_id:
                    stopRecording();
                    mStopBtn.setEnabled(false);
                    // mRecorderBtn.setImageResource(R.drawable.start);
                    mStatusTextView.setText("录音已停止");
                    mStatusTextView.setTextColor(Color.RED);
                    Toast.makeText(RecordActivity.this, "保存成功", Toast.LENGTH_SHORT).show();
                    mRecorderBtn.setText("Start");
                    break;
                case R.id.image_back:
                    finish();
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
                    RecordActivity.this.mHandler.sendMessage(msg);
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
                    RecordActivity.this.mHandler.sendMessage(msg);
                } else {
                    Message msg = new Message();
                    msg.what = db;
                    RecordActivity.this.mHandler.sendMessage(msg);
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
    protected void onResume() {
        super.onResume();
        mStopBtn.setEnabled(true);
        if (mRecordState == MediaRecorderState.STOPPED || mRecordState == MediaRecorderState.PAUSED) {
            startRecording();
            // mRecorderBtn.setImageResource(R.drawable.stop);
            mStatusTextView.setText("正在进行录音...");
            mStatusTextView.setTextColor(Color.GREEN);
            mRecorderBtn.setText("Pause");
        } else if (mRecordState == MediaRecorderState.RECORDING) {
            pauseRecording();
            // mRecorderBtn.setImageResource(R.drawable.start);
            mStatusTextView.setText("录音已暂停");
            mStatusTextView.setTextColor(Color.GRAY);

            mRecorderBtn.setText("Start");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopRecording();
        mStopBtn.setEnabled(false);
        // mRecorderBtn.setImageResource(R.drawable.start);
        mStatusTextView.setText("录音已停止");
        mStatusTextView.setTextColor(Color.RED);
        Toast.makeText(RecordActivity.this, "保存成功", Toast.LENGTH_SHORT).show();
        mRecorderBtn.setText("Start");
    }

    /**
     * 获取文件路径 修改布局
     * @return
     */
    private String getSoundFilePath(){
        String str = "";
        if (Setup.isEmphasisFile){
            str =  mFilepath+TimeUtils.
                    milliseconds2String2(System.currentTimeMillis())+"IMP"+Setup.soundFilePostfix;
        }
        else
        {
            str = mFilepath+TimeUtils.
                    milliseconds2String2(System.currentTimeMillis())+Setup.soundFilePostfix;
        }
        return str;
    }

}

