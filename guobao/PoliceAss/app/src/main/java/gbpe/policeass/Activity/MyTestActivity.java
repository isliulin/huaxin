package gbpe.policeass.Activity;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;

import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.cpu.CpuUtil;

import gbpe.policeass.R;
import gbpe.policeass.utils.CameraHandlerThread;

public class MyTestActivity extends Activity {

    Button button ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_test);

        button  = (Button) findViewById(R.id.btn_test_record);
    }

    boolean isRecording = false;
    public void startAndStop2(View view) {
        isRecording = !isRecording;
        if (isRecording) {
            CameraHandlerThread.getCameraInstance().setPreviewCallback(CameraHandlerThread.mCameraPreviewCallback);
            CameraHandlerThread.getmCameraPreviewCallback().startRecording();
            button.setText("停止");
            showMessage();
        } else {
            CameraHandlerThread.getmCameraPreviewCallback().stopRecording();
            CameraHandlerThread.getCameraInstance().setPreviewCallback(null);
            button.setText("开始");
            hideMessage();
        }//
    }
    Button cpuInfo;
    /**
            * 显示cpu使用率
    * create at 2017/3/23 11:08
            */
    private void showMessage() {
        cpuInfo = (Button) findViewById(R.id.cpu);
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
                cpuInfo.setText("cpu:" + CpuUtil.getUsedPercentValue(MyTestActivity.this)
                        + ",time:" + (transformHMS(System.currentTimeMillis() + 1000 - time)));
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
