package gbpe.policeass.fragments;

import android.app.Dialog;
import android.content.Context;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.IOException;

import gbpe.policeass.R;
import gbpe.policeass.utils.AppLog;

public class PlaySoundDialog extends Dialog {
    private Context context;
    private View dialogView;
    private MediaPlayer mediaPlayer;
    private TextView time_now;
    private TextView time_total;
    private SeekBar play_progress;
    private TextView sound_name;
    //private ImageView play_switch;
    private PlayComplishListener playComplishListener;
    private static final String TAG = "PlaySoundDialog";
    private Handler handler;
    //private int play_inc;
    private int position;
    private int items;
    //private String filePath;

    /*
    public PlaySoundDialog(Context context, String fp){
        super(context);
        this.context = context;
        filePath = fp;
    }*/

    public interface PlayComplishListener{
        void onStateChangeHandle(int inc);
        //void onKeyLeft(int inc);
        //void onkeyRight(int inc);
    }

    public PlaySoundDialog(Context context,int theme, /*PlayComplishListener listener,*/ int cnt, int start){
        super(context);
        this.context = context;
        //filePath = fp;
        //playComplishListener = listener;
        LayoutInflater inflater = LayoutInflater.from(context);
        dialogView = inflater.inflate(R.layout.sound_play_dialog,null);
        position = start;
        items = cnt;
    }

    public void setListener(PlayComplishListener listener){
        playComplishListener = listener;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.setContentView(dialogView);
        mediaPlayer = new MediaPlayer();
        /* 控件 */
        time_now = dialogView.findViewById(R.id.time_now);
        time_total = dialogView.findViewById(R.id.time_total);
        play_progress = dialogView.findViewById(R.id.play_progress);
        sound_name = dialogView.findViewById(R.id.sound_name);
        //play_switch = dialogView.findViewById(R.id.play_switch);
        handler = new Handler();
        //play_inc = 0;
    }

    @Override
    public <T extends View> T findViewById(int id) {
        return super.findViewById(id);
    }

    public View getDialogView(){
        return dialogView;
    }

    public void startPlay(String soundfile){
        this.setTitle(soundfile);
        /* 进度条监听器 */
        play_progress.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                /* 此处设置拖动滑块之后，定位播放 */
                mediaPlayer.seekTo(play_progress.getProgress());
            }
        });

        mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                if (position < items-1) {
                    position ++;
                    playComplishListener.onStateChangeHandle(position);
                }
            }
        });

        play(soundfile);
        //play_progress.setMax(mediaPlayer.getDuration());
        //time_total.setText(formatTime(mediaPlayer.getDuration()));

        handler.post(new Runnable() {
            @Override
            public void run() {
                if (mediaPlayer==null){
                    return;
                }
                time_now.setText(formatTime(mediaPlayer.getCurrentPosition()));
                play_progress.setProgress(mediaPlayer.getCurrentPosition());
                handler.postDelayed(this,1000);
            }
        });

    }

    @Override
    public void dismiss() {
        super.dismiss();
        if (mediaPlayer!=null){
            mediaPlayer.stop();
            //mediaPlayer.release();
            mediaPlayer = null;
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, @NonNull KeyEvent event) {
        switch (keyCode){
            case KeyEvent.KEYCODE_DPAD_LEFT:
                //AppLog.i(TAG,"key left");
                if (position > 0) {
                    position --;
                    playComplishListener.onStateChangeHandle(position);
                }
                break;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                //AppLog.i(TAG,"key right");
                if (position < items-1) {
                    position ++;
                    playComplishListener.onStateChangeHandle(position);
                }
                break;
            case KeyEvent.KEYCODE_HOME:
                if (mediaPlayer!=null){
                    if (mediaPlayer.isPlaying()){
                        mediaPlayer.pause();
                    } else {
                        mediaPlayer.start();
                    }
                }
                break;

        }
        return super.onKeyUp(keyCode, event);
    }

    public void play(String soundPath){
        try {

            mediaPlayer.reset();
            mediaPlayer.setDataSource(soundPath);
            mediaPlayer.prepare();
            mediaPlayer.start();
            play_progress.setMax(mediaPlayer.getDuration());
            time_total.setText(formatTime(mediaPlayer.getDuration()));
            AppLog.i(TAG, "d当前播放: "+soundPath);
        } catch (IOException e) {
            AppLog.e(TAG,"播放错误");
            e.printStackTrace();
        }
    }

    public String formatTime(long time)
    {
        time = time/ 1000;
        String strHour = "" + (time/3600);
        String strMinute = "" + time%3600/60;
        String strSecond = "" + time%3600%60;

        strHour = strHour.length() < 2? "0" + strHour: strHour;
        strMinute = strMinute.length() < 2? "0" + strMinute: strMinute;
        strSecond = strSecond.length() < 2? "0" + strSecond: strSecond;

        String strRsult = "";

        if (!strHour.equals("00"))
        {
            strRsult += strHour + ":";
        }

        //if (!strMinute.equals("00"))
        {
            strRsult += strMinute + ":";
        }

        strRsult += strSecond;

        return strRsult;
    }
}
