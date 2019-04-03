package gbpe.policeass.fragments;

import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import gbpe.policeass.R;
import gbpe.policeass.VideoRecord.play.DensityUtil;
import gbpe.policeass.VideoRecord.play.FullScreenVideoView;
import gbpe.policeass.VideoRecord.play.LightnessController;
import gbpe.policeass.VideoRecord.play.VolumnController;
import gbpe.policeass.utils.AppLog;

/**
 * Created by Administrator on 2016/12/2.
 */
public class VideoPlayFragment extends BaseFragment
        implements View.OnClickListener {

    private static final String TAG = "VideoPlayFragment";
    // 自定义VideoView
    private FullScreenVideoView mVideo;
    public static final String BUNDLE_FILE_PATH = "filepath";
    public static final String BUNDLE_FILE_POSITION = "position";


    // 头部View
    private View mTopView;
	// 左边View
	private View mLeftView;
	// 右边View
	private View mRightView;
	

    // 底部View
    private View mBottomView;
    // 视频播放拖动条
    private SeekBar mSeekBar;
    private ImageView mPlay;
    private TextView mPlayTime;
    private TextView mDurationTime;

    private ImageView mPlayLast;
    private ImageView mPlayNext;

    // 音频管理器
    private AudioManager mAudioManager;

    // 屏幕宽高
    private float width;
    private float height;

    // 视频播放时间
    private int playTime;

     // 自动隐藏顶部和底部View的时间
    private static final int HIDE_TIME = 5000;

    // 声音调节Toast
    private VolumnController volumnController;

    // 原始屏幕亮度
    private int orginalLight;


    //
    private List<String> pathList;
    private int position;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_LEFT:
				playLast();
				break;

			case KeyEvent.KEYCODE_DPAD_RIGHT:
				playNext();
				break;
		}
        return false;
    }

    @Override
    protected boolean onBackPressed() {
      return false;
    }

    View view;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_videoplay, null);


        volumnController = new VolumnController(getActivity());
        mVideo = (FullScreenVideoView) view.findViewById(R.id.videoview);
        mPlayTime = (TextView) view.findViewById(R.id.play_time);
        mDurationTime = (TextView) view.findViewById(R.id.total_time);
        mPlay = (ImageView) view.findViewById(R.id.play_btn);

        // add by ycb
        mPlayLast = (ImageView)view.findViewById(R.id.play_last);
        mPlayNext = (ImageView)view.findViewById(R.id.play_next);

        mSeekBar = (SeekBar)view. findViewById(R.id.seekbar);
        mTopView = view.findViewById(R.id.top_layout);
		// add by ycb
		mLeftView = view.findViewById(R.id.left_layout);
		mRightView = view.findViewById(R.id.right_layout);
        mBottomView =view. findViewById(R.id.bottom_layout);
        view.findViewById(R.id.iv_play_video_back).setOnClickListener(this);

        return view;
    }


    @Override
    public void onPause() {
        super.onPause();
        LightnessController.setLightness(getActivity(), orginalLight);
    }

    private class PlayThread extends Thread{

        private String mVideoPath;

        public PlayThread(String path)
        {
            mVideoPath = path;
        }

        public void setVideoPath(String path)
        {
            mVideoPath = path;
        }

        @Override
        public void run()
        {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    playVideo(mVideoPath);
                }
            });
        }
    }

    private PlayThread mPlayThread =  null;

    @Override
    public void onResume() {
        super.onResume();
        mAudioManager = (AudioManager) getActivity().
                getSystemService(Context.AUDIO_SERVICE);

        width = DensityUtil.getWidthInPx(getActivity());
        height = DensityUtil.getHeightInPx(getActivity());
        threshold = DensityUtil.dip2px(getActivity(), 18);
        orginalLight = LightnessController.getLightness(activity);
        mPlay.setOnClickListener( this);
        mSeekBar.setOnSeekBarChangeListener(mSeekBarChangeListener);
        Bundle bundle = getArguments();
        //final  String videoPath = bundle.getString("filePath");
        pathList = bundle.getStringArrayList(BUNDLE_FILE_PATH);
        position = bundle.getInt(BUNDLE_FILE_POSITION,0);
        final String videoPath = pathList.get(position);

		Log.i(TAG, "onResume() position: "+position+", path: "+videoPath);

        /*
        //防止阻塞界面切换，先切换在播放
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run()
                    {
                        playVideo(videoPath);
                    }
                });
            }
        }).start();*/
        //new PlayThread(videoPath).start();
        mPlayThread = new PlayThread(videoPath);
        mPlayThread.start();

		mPlayLast.setOnClickListener(this);
		mPlayNext.setOnClickListener(this);
/*
        mPlayNext.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "play next");
                
            }
        });

        mPlayLast.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG,"play last");
            }
        });
*/


    }

    //设置滑块的滑动监听
    private SeekBar.OnSeekBarChangeListener mSeekBarChangeListener =
            new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            mHandler.postDelayed(hideRunnable, HIDE_TIME);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            mHandler.removeCallbacks(hideRunnable);
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress,
                                      boolean fromUser) {
            if (fromUser) {
                int time = progress * mVideo.getDuration() / 100;
                mVideo.seekTo(time);
            }
        }
    };

    private void backward(float delataX) {
        int current = mVideo.getCurrentPosition();
        int backwardTime = (int) (delataX / width * mVideo.getDuration());
        int currentTime = current - backwardTime;
        mVideo.seekTo(currentTime);
        mSeekBar.setProgress(currentTime * 100 / mVideo.getDuration());
        mPlayTime.setText(formatTime(currentTime));
    }

    private void forward(float delataX) {
        int current = mVideo.getCurrentPosition();
        int forwardTime = (int) (delataX / width * mVideo.getDuration());
        int currentTime = current + forwardTime;
        mVideo.seekTo(currentTime);
        mSeekBar.setProgress(currentTime * 100 / mVideo.getDuration());
        mPlayTime.setText(formatTime(currentTime));
    }

    private void volumeDown(float delatY) {
        int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        int current = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        int down = (int) (delatY / height * max * 3);
        int volume = Math.max(current - down, 0);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, volume, 0);
        int transformatVolume = volume * 100 / max;
        volumnController.show(transformatVolume);
    }

    private void volumeUp(float delatY) {
        int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        int current = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        int up = (int) ((delatY / height) * max * 3);
        int volume = Math.min(current + up, max);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, volume, 0);
        int transformatVolume = volume * 100 / max;
        volumnController.show(transformatVolume);
    }

    private void lightDown(float delatY) {
        int down = (int) (delatY / height * 255 * 3);
        int transformatLight = LightnessController.getLightness(getActivity()) - down;
        LightnessController.setLightness(getActivity(), transformatLight);
    }

    private void lightUp(float delatY) {
        int up = (int) (delatY / height * 255 * 3);
        int transformatLight = LightnessController.getLightness(getActivity()) + up;
        LightnessController.setLightness(getActivity(), transformatLight);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mHandler.removeMessages(0);
        mHandler.removeCallbacksAndMessages(null);
        //退出时，新开一个线程（防止阻碍主线程）打开surface
        new Thread(new Runnable() {
            @Override
            public void run() {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        //activity.sView.setVisibility(View.VISIBLE);
                    }
                });

            }
        }).start();

    }

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    if (mVideo.getCurrentPosition() > 0) {
                        mPlayTime.setText(formatTime(mVideo.getCurrentPosition()));
                        int progress = mVideo.getCurrentPosition() * 100 / mVideo.getDuration();
                        mSeekBar.setProgress(progress);
                        if (mVideo.getCurrentPosition() > mVideo.getDuration() - 100) {
                            mPlayTime.setText("00:00");
                            mSeekBar.setProgress(0);
                        }
                        mSeekBar.setSecondaryProgress(mVideo.getBufferPercentage());
                    } else {
                        mPlayTime.setText("00:00");
                        mSeekBar.setProgress(0);
                    }

                    break;
                case 2:
                    showOrHide();
                    break;

                default:
                    break;
            }
        }
    };

    private void playVideo(String filePath) {

        mVideo.setVideoPath(filePath);
        mVideo.requestFocus();
        mVideo.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mVideo.setVideoWidth(mp.getVideoWidth());
                mVideo.setVideoHeight(mp.getVideoHeight());

                mVideo.start();
                if (playTime != 0) {
                    mVideo.seekTo(playTime);
                }

                mHandler.removeCallbacks(hideRunnable);
                mHandler.postDelayed(hideRunnable, HIDE_TIME);
                mDurationTime.setText(formatTime(mVideo.getDuration()));
                Timer timer = new Timer();
                timer.schedule(new TimerTask() {

                    @Override
                    public void run() {
                        mHandler.sendEmptyMessage(1);
                    }
                }, 0, 1000);
            }
        });
        mVideo.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                mPlay.setImageResource(R.drawable.video_btn_down);
                mPlayTime.setText("00:00");
                mSeekBar.setProgress(0);
            }
        });
        mVideo.setOnTouchListener(mTouchListener);
    }

    private Runnable hideRunnable = new Runnable() {

        @Override
        public void run() {
            showOrHide();
        }
    };

    @SuppressLint("SimpleDateFormat")
    private String formatTime(long time) {
        DateFormat formatter = new SimpleDateFormat("mm:ss");
        return formatter.format(new Date(time));
    }

    private float mLastMotionX;
    private float mLastMotionY;
    private int startX;
    private int startY;
    private int threshold;
    private boolean isClick = true;

    private View.OnTouchListener mTouchListener = new View.OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            final float x = event.getX();
            final float y = event.getY();

            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    mLastMotionX = x;
                    mLastMotionY = y;
                    startX = (int) x;
                    startY = (int) y;
                    break;
                case MotionEvent.ACTION_MOVE:
                    float deltaX = x - mLastMotionX;
                    float deltaY = y - mLastMotionY;
                    float absDeltaX = Math.abs(deltaX);
                    float absDeltaY = Math.abs(deltaY);
                    // 声音调节标识
                    boolean isAdjustAudio = false;
                    if (absDeltaX > threshold && absDeltaY > threshold) {
                        if (absDeltaX < absDeltaY) {
                            isAdjustAudio = true;
                        } else {
                            isAdjustAudio = false;
                        }
                    } else if (absDeltaX < threshold && absDeltaY > threshold) {
                        isAdjustAudio = true;
                    } else if (absDeltaX > threshold && absDeltaY < threshold) {
                        isAdjustAudio = false;
                    } else {
                        return true;
                    }
                    if (isAdjustAudio) {
                        if (x < width / 2) {
                            if (deltaY > 0) {
                                lightDown(absDeltaY);
                            } else if (deltaY < 0) {
                                lightUp(absDeltaY);
                            }
                        } else {
                            if (deltaY > 0) {
                                volumeDown(absDeltaY);
                            } else if (deltaY < 0) {
                                volumeUp(absDeltaY);
                            }
                        }

                    } else {
                        if (deltaX > 0) {
                            forward(absDeltaX);
                        } else if (deltaX < 0) {
                            backward(absDeltaX);
                        }
                    }
                    mLastMotionX = x;
                    mLastMotionY = y;
                    break;
                case MotionEvent.ACTION_UP:
                    if (Math.abs(x - startX) > threshold
                            || Math.abs(y - startY) > threshold) {
                        isClick = false;
                    }
                    mLastMotionX = 0;
                    mLastMotionY = 0;
                    startX = (int) 0;
                    if (isClick) {
                        showOrHide();
                    }
                    isClick = true;
                    break;

                default:
                    break;
            }
            return true;
        }

    };

	private void playLast(){
		if (position > 0 )
        {
            position --;
			String path = pathList.get(position);
			AppLog.i(TAG,"position: "+position+", path: "+path);
            //mVideo.stopPlayback();
            mVideo.stopPlayback();
            mPlayThread.interrupt();
            mPlayThread = new PlayThread(path);
            mPlayThread.start();
            // mPlayThread.setVideoPath(path);
            // mPlayThread.start();
        } else {

            Toast.makeText(getActivity(),"没有更新视频",Toast.LENGTH_SHORT).show();
        }
	}

	private void playNext(){
		if (position < ( pathList.size() - 1 ))
        {
            position ++;
            //mVideo.stopPlayback();
            String path = pathList.get(position);
            Log.i(TAG,"position: "+position+", path: "+path);
            mVideo.stopPlayback();
            mPlayThread.interrupt();
            //mPlayThread.setVideoPath(path);
            //mPlayThread.start();
            mPlayThread = new PlayThread(path);
            mPlayThread.start();
            // Toast.makeText(getActivity(),"没有更早视频",Toast.LENGTH_SHORT).show();
        } else{
            Toast.makeText(getActivity(),"没有更早视频",Toast.LENGTH_SHORT).show();
        }

	}

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.play_btn: {
                if (mVideo.isPlaying())
                {
                    mVideo.pause();
                    mPlay.setImageResource(R.drawable.video_btn_down);
                } else
                {
                    mVideo.start();
                    mPlay.setImageResource(R.drawable.video_btn_on);
                }
                break;
            }
            case R.id.iv_play_video_back:{
                popSelf();
                break;
            }

			case R.id.play_last:
			    AppLog.i(TAG, "play_last");
			    playLast();
			break;

			case R.id.play_next:
			    AppLog.i(TAG, "play_next");
                playNext();
			break;
			
            default:break;
        }
    }

    private void showOrHide() {
        if (mTopView.getVisibility() == View.VISIBLE) {
            mTopView.clearAnimation();
            Animation animation = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_leave_from_top);
            animation.setAnimationListener(new AnimationImp() {
                @Override
                public void onAnimationEnd(Animation animation) {
                    super.onAnimationEnd(animation);
                    mTopView.setVisibility(View.GONE);
                }
            });
            mTopView.startAnimation(animation);

            mBottomView.clearAnimation();
            Animation animation1 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_leave_from_bottom);
            animation1.setAnimationListener(new AnimationImp() {
                @Override
                public void onAnimationEnd(Animation animation) {
                    super.onAnimationEnd(animation);
                    mBottomView.setVisibility(View.GONE);
                }
            });
            mBottomView.startAnimation(animation1);

			// add by ycb, for add play last and play next
			mLeftView.clearAnimation();
			Animation animation2 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_leave_from_left);
			animation2.setAnimationListener(new AnimationImp() {
				@Override
				public void onAnimationEnd(Animation animation){
					super.onAnimationEnd(animation);
					mLeftView.setVisibility(View.GONE);
				}
			});
			mLeftView.startAnimation(animation2);


			mRightView.clearAnimation();
			Animation animation3 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_leave_from_right);
			animation3.setAnimationListener(new AnimationImp() {
				@Override
				public void onAnimationEnd(Animation animation){
					super.onAnimationEnd(animation);
					mRightView.setVisibility(View.GONE);
				}
			});
			mRightView.startAnimation(animation3);

			
        } else {
            mTopView.setVisibility(View.VISIBLE);
            mTopView.clearAnimation();
            Animation animation = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_entry_from_top);
            mTopView.startAnimation(animation);

            mBottomView.setVisibility(View.VISIBLE);
            mBottomView.clearAnimation();
            Animation animation1 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_entry_from_bottom);
            mBottomView.startAnimation(animation1);

			// add by ycb, for add play last and play next
			mLeftView.setVisibility(View.VISIBLE);
            mLeftView.clearAnimation();
            Animation animation2 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_entry_from_left);
            mLeftView.startAnimation(animation2);

			// add by ycb, for add play last and play next
			mRightView.setVisibility(View.VISIBLE);
            mRightView.clearAnimation();
            Animation animation3 = AnimationUtils.loadAnimation(getActivity(),
                    R.anim.option_entry_from_right);
            mRightView.startAnimation(animation3);

			

			
            mHandler.removeCallbacks(hideRunnable);
            mHandler.postDelayed(hideRunnable, HIDE_TIME);
        }
    }

    private class AnimationImp implements Animation.AnimationListener {

        @Override
        public void onAnimationEnd(Animation animation) {

        }

        @Override
        public void onAnimationRepeat(Animation animation) {
        }

        @Override
        public void onAnimationStart(Animation animation) {
        }

    }


}
