package gbpe.policeass.Activity;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.admin.DevicePolicyManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.graphics.drawable.ClipDrawable;
import android.graphics.drawable.LayerDrawable;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationProvider;
import android.media.AudioManager;
import android.media.MediaRecorder;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.SystemClock;
import android.os.storage.StorageManager;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.PhoneStateListener;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextClock;
import android.widget.TextView;
import android.widget.Toast;
//mport com.guobao.lightboard.HardwareControler;
import com.topvision.videodemo.util.VideoSet;

import org.greenrobot.eventbus.EventBus;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Properties;

import gbpe.policeass.Photograph.PhotoManager;
import gbpe.policeass.R;
import gbpe.policeass.Setup.SettingTags;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.SysUpdate.PollService;
import gbpe.policeass.VideoRecord.VideoManager6;
import gbpe.policeass.apn.APN;
import gbpe.policeass.apn.arcVPDN;
import gbpe.policeass.fragments.SettingFragment;
import gbpe.policeass.fragments.SoundRecordFragment;
import gbpe.policeass.modle.AnyEvent;
import gbpe.policeass.modle.AudioStartEvent;
import gbpe.policeass.modle.AudioStopEvent;
import gbpe.policeass.modle.Automatic;
import gbpe.policeass.modle.OnCombineVideo;
import gbpe.policeass.modle.OnGetLocationEvent;
import gbpe.policeass.modle.OnReleaseCamera;
import gbpe.policeass.modle.OnStartCountEvent;
import gbpe.policeass.modle.OnStopCountEvent;
import gbpe.policeass.modle.PhotographEvent;
import gbpe.policeass.modle.RemoteStartSoundRecord;
import gbpe.policeass.modle.RemoteStopSoundRecord;
import gbpe.policeass.modle.VideoStartEvent;
import gbpe.policeass.modle.VideoStopEvent;
import gbpe.policeass.db.DBHelper;
import gbpe.policeass.log.LogUtils;
import gbpe.policeass.receiver.UsbActionReceiver;
import gbpe.policeass.service.MyService;
import gbpe.policeass.utils.AnimUtils;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.CameraHandlerThread;
import gbpe.policeass.utils.CameraSurfaceView;
import gbpe.policeass.utils.DevControl;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.IRCutUtils;
import gbpe.policeass.utils.MergerMp4;
import gbpe.policeass.utils.MySharePreference;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.SOSUtil;
import gbpe.policeass.utils.ScreenOffAdminReceiver;
import gbpe.policeass.utils.SelectHandleUtil;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.utils.ShellUtils;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.utils.gps.GpsUtils;
import gbpe.policeass.views.CountDownTimerView;
import gbpe.policeass.views.MemoryView;
import gbpe.policeass.views.TimeIndicator;
//import gbpe.policeass.zfysim.HomeActivity;
import gbpe.policeass.zfysim.NewHomeActivity;

import static java.lang.Thread.sleep;

// 编译问题
// 'standard output': Resource temporarily unavailable

/*
*  activity的android:screenOrientation
* unspecified，默认值，由系统决定，不同手机可能不一致
* landscape，强制横屏显示
* portrait，强制竖屏显
* behind，与前一个activity方向相同
* sensor，根据物理传感器方向转动，用户90度、180度、270度旋转手机方向，activity都更着变化
* sensorLandscape，横屏旋转，一般横屏游戏会这样设置
* sensorPortrait，竖屏旋转
* nosensor，旋转设备时候，界面不会跟着旋转。初始化界面方向由系统控制
* user，用户当前设置的方向

*/

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2016/11/10 19:37
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2016/11/10 19:37
 *
 * 描述：1.正在拍摄视频的时候，a.想要拍照(直接拍照)b.录制声音（停止并保存视频，然后录音）。
         2.正在录音，a.想要录制视频(停止录音，并保存，然后开始录制视频)，b.拍照（不可以）。
 * =====================================================================================
 */
public class MainActivity extends BaseActivity implements View.OnLongClickListener {

    private static final String TAG = "MainActivity";
    public static final String IR_CUT_MESSAGE = "gbpe.policeass.ir.cut";
    public static final String IR_CUT_STATE = "ir.cut.state";
    public static final String IR_CUT_IS_RCD = "ir.cut.is.rcd";
    public static final String IR_CUT_INIT = "IR_CUT_INIT";
    public static final String BAT_VOLTAGE_FILE = "/sys/class/power_supply/battery/batt_vol";
    public ImageView btnVideoRecorder, btnTakePhotos,
            btnSoundRecorder;
    //计时器控件
    private Chronometer chronometer;
    // 显示预览的SurfaceView
    //public CameraSurfaceView sView;
    private TextClock textClock, tcDate;
    private TextView tv_tital, tv_pid, tv_sn;
    private ImageButton tv_menue;
    private TextView tv_memory;
    //private BatteryReceiver receiver = null;
	private BroadcastReceiver pow_action_receiver = null;
    private boolean shortPress;
    public static DBHelper dbHelper;
    private ImageView iv_cd;
    private Window window;
    private TextView tv_battery_level;
    private static int mRecordTick = 0;

    private MemoryView iv_memery;
    private ImageView gps_indicator;
    private boolean gps_available = false;
    //private static Handler mIrHandler;
    private MySharePreference mySharePreference = new MySharePreference();

	// delete 20180820, 删了好像没卵用啊
    private CountDownTimerView cdtShow;
    private Button switchBtn;
    private TimeIndicator tid;
	private int mIrcutCnt = 0;
	//private boolean mIsIrcutActivie = false;
	private boolean mPhotoEnable = true;

	private Handler mCmrHandler;
	private boolean mIrCutEnable = true;
	private Handler mPreRecordHandler;

    private MediaRecorder mediaRecorder;
    private Camera camera;

    private Context context;
    private PowerManager mPowerManager;
    private PowerManager.WakeLock mWakeLock;
    private DevicePolicyManager policyManager;
    private ComponentName adminReceiver;
	private boolean isAdminDevice = false;

    public static boolean getScreenState() {
        return is_screen_on;
    }

    public static void setScreenState(boolean is_screen_on) {
        MainActivity.is_screen_on = is_screen_on;
    }

	public static void setRecordTick(int tick){
		mRecordTick = tick;
	}

    private static boolean is_screen_on = true;

    // for compass test
    private float[] accelerometerValues = new float[3];
    private float[] magneticFieldValues = new float[3];
    private SensorManager compassSensorManager;

	private String mVideoName;

	IRCutUtils mIrCutUtils = new IRCutUtils(new IRCutUtils.IStopRecord() {
        @Override
        public void stopRcd(boolean isRcd, boolean irState) {
            IRCutActionProc(isRcd, irState);
        }
    });


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = this;
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
        //adminReceiver = new ComponentName(MainActivity.this, ScreenOffAdminReceiver.class);
        //mPowerManager = (PowerManager)getSystemService(POWER_SERVICE);
        //policyManager = (DevicePolicyManager)MainActivity.this.getSystemService(Context.DEVICE_POLICY_SERVICE);
        //checkAndTurnOnDeviceManager(null);
    }


    /**
     * 处理handler发来的消息
     *
     * @param msg
     */
    @Override
    protected void handler(Message msg) {
    }

    @Override
    protected void initView() {
    }


    /**
     * 判断按钮状态
     */
    // modified by ycb, the variable place in CameraHandlerThread
    //private boolean isRecording;//录视频
    private boolean isAudioRecording;//录音

    /**
     * 触摸按钮事件
     *
     * @param source
     */
    @Override
    public void widgetClick(View source) {

        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
        switch (source.getId()) {
            case R.id.iv_video_record: {//摄像

                if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, R.string.camera_exception, Toast.LENGTH_SHORT).show();
                    return;
                }

				if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return;
				}
				
                if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(0, 0);
                }

                //isRecording = !isRecording;
                //CameraHandlerThread.isRecordingGlobal = !CameraHandlerThread.isRecordingGlobal;
                //videoRecord();
                //if (CameraHandlerThread.isRecordingGlobal){
                if (Setup.videoRecording) {
                    //CameraHandlerThread.isRecordingGlobal = false;
                    stopVideoRecord();
                }else{
                    //CameraHandlerThread.isRecordingGlobal = true;
                    mVideoName = startVideoRecord();
                }
                break;
            }
            case R.id.iv_take_picture: {//拍照
                /*if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, "摄像头异常6", Toast.LENGTH_SHORT).show();
                    return;
                }*/
                if (isAudioRecording) {
                    Toast.makeText(MainActivity.this, R.string.no_photo_tips, Toast.LENGTH_SHORT).show();
                    return;
                }
                /*if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(1, 0);
                }*/
                takePictures();
                break;
            }
            case R.id.iv_audio_reocrd: {//录音

                if (SettingUtils.getBoolenValues(this, "key_tone")) {//是否播放按键音
                    keySound.play(0, 0);
                }
                if (!isAudioRecording)
                {
                    btnSoundRecorder.setSelected(true);
                    //if (isRecording) {//按屏幕上录像按钮开始的录像
                    //if (CameraHandlerThread.isRecordingGlobal){
                    if (Setup.videoRecording) {
                        btnVideoRecorder.setSelected(false);
                        //isRecording = !isRecording;
                        //CameraHandlerThread.isRecordingGlobal =!CameraHandlerThread.isRecordingGlobal;
                        //CameraHandlerThread.isRecordingGlobal = false;
                        stopVideoRecord();
                        //videoRecord();
                    }
                } else {
                    btnSoundRecorder.setSelected(false);
                }
                isAudioRecording = !isAudioRecording;

                if ( Setup.videoRecording ) {//按物理按键开始的录像
                    //videoRecord();//停止视频播放
                    stopVideoRecord();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            setGLRenderer3();
                        }
                    },1000);
                }else {
                    setGLRenderer3();
                }

                break;
            }
            case R.id.tv_menue: {//主页界面

                 /*if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||  Setup.videoRecording)*/
			    if (Setup.VideoPrerecord ||  Setup.videoRecording) {
                     Setup.currPage = false;
                     VideoManager6.stopAll();
                     Setup.videoRecording = false;
                     Setup.isNormalRecording = false;
					 /* 删除预录文件*/
                     //FileUtils.deleteFolder(Setup.pre_file_dir);
                     VideoManager6.clearPreFile();
                }
                if (isAudioRecording) {//如果正在录音，则停掉录音并保存
                    setGLRenderer3();
                    isAudioRecording = false;
                }

                //tid.setInVisible();
                //tid.rempveTimeRun();
                //Intent intent1 = new Intent(this, HomeActivity.class);
                Intent intent1 = new Intent(this, NewHomeActivity.class);
                startActivity(intent1);
                break;
            }
            case R.id.change:

                if (CameraHandlerThread.isCameraOpened(this, true)) {
                    CameraHandlerThread.switchCameraId();
                }

                break;
            default:
                break;
        }
    }


    /**
     * 按钮长按保存为重点文件
     *
     * @param v
     * @return
     */
    @Override
    public boolean onLongClick(View v) {

        //Setup.isEmphasisFile = true;
        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
        switch (v.getId()) {
            case R.id.iv_video_record: {//摄像

                if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, R.string.camera_exception, Toast.LENGTH_SHORT).show();
                    return true;
                }

				if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return true;
				}

                if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(0, 0);
                }

                //isRecording = !isRecording;
                //CameraHandlerThread.isRecordingGlobal = !CameraHandlerThread.isRecordingGlobal;
                Setup.longPressVideo = true;
                //if (CameraHandlerThread.isRecordingGlobal){
                if (Setup.videoRecording) {
                    //CameraHandlerThread.isRecordingGlobal = false;
                    stopVideoRecord();
                }else{
                    //CameraHandlerThread.isRecordingGlobal = true;
                    mVideoName = startVideoRecord();
                }
                //videoRecord();

                break;
            }
            case R.id.iv_take_picture: {//拍照
                /*if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, "摄像头异常8", Toast.LENGTH_SHORT).show();
                    return true;
                }
                if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(1, 0);
                }*/
                takePictures();
                break;
            }
            case R.id.iv_audio_reocrd: {//录音

                if (SettingUtils.getBoolenValues(this, "key_tone")) {//是否播放按键音
                    keySound.play(0, 0);
                }

   //========================================================================================================================
                if (!isAudioRecording) {
                    btnSoundRecorder.setSelected(true);
                    //if (isRecording) {
                    //if (CameraHandlerThread.isRecordingGlobal){
                    if (Setup.videoRecording) {
                        btnVideoRecorder.setSelected(false);
                        //isRecording = !isRecording;
                        //CameraHandlerThread.isRecordingGlobal = !CameraHandlerThread.isRecordingGlobal;
                        //videoRecord();
                        //CameraHandlerThread.isRecordingGlobal = false;
                        stopVideoRecord();
                    }
                    isAudioRecording = true;
                } else {
                    btnSoundRecorder.setSelected(false);
                    isAudioRecording = false;
                }
                if ( Setup.videoRecording ) {
                    //videoRecord();//停止视频播放
                    stopVideoRecord();
                    handler.postDelayed(new Runnable() {//延时1.5s后开始录制视频，否则surfaceview没有初始化好，会导致录制视频失败
                        @Override
                        public void run() {
                            setGLRenderer3();
                        }
                    },1000);
                }else {
                    setGLRenderer3();
                }
   //========================================================================================================================
                break;
            }
            default:
                break;
        }
        return true;
    }

    // public void IRCutActionProc(Boolean i_isRcd, int irState)
    public void IRCutActionProc(Boolean i_isRcd, final boolean irState)
    {
        AppLog.i(TAG,"IRCutActionProc");
        //HardControl.cameraTuning(Setup.currIRcutState);
        /*irState false 表示光线充足*/
        //HardControl.cameraTuning(irState?0:1);
        //HardControl.cameraTuning(irState?1:0);

        final Boolean isRcd = i_isRcd;
        new Thread()
        {
            @Override
            public void run()
            {
/*
                //boolean isRcdRsv = isRcd;
                AppLog.i(TAG,"close camera, isRcd "+isRcd);
                if (isRcd)
                {
                    AppLog.i(TAG,"stop record first");
                    //stopVideoRecord(); //停止录像
                    pauseVideoRecord();
                    mRecordTick = TimeIndicator.getTimeTick();
                }
                CameraHandlerThread.getInstance().doStopCamera();
				
                //try {
                //    sleep(100);
                //} catch (InterruptedException e) {
                //    e.printStackTrace();
                //}
                AppLog.i(TAG,"open camera");
                CameraHandlerThread.getInstance().getCameraInstance();
                CameraHandlerThread.getInstance().initCamera();

                if (isRcd)
                {
                    AppLog.i(TAG,"start record again");
					//mIrcutCnt++;
					mIsIrcutActivie = true;
                    startVideoRecord(mVideoName+".tmp"+mIrcutCnt); //重新开始录像
                    //startVideoRecord();
                    mIrcutCnt++;
                }
                //CameraHandlerThread.camera.stop();
                //CameraHandlerThread.camera.open();
                //CameraHandlerThread.camera.
                
                //CameraHandlerThread.camera.autoFocus(new Camera.AutoFocusCallback() {
                //    @Override
                //    public void onAutoFocus(boolean success, Camera camera) {
				//
                //    }
                //});
                //CameraHandlerThread.camera.cancelAutoFocus();
				//tag by ycb: 此处摄像头关闭/开启后再切换ir cut，否则ir cut切换会大概率不成功
				//此处循环等待，否则会出现IR CUT切换不成功的情况
				while (true) {
					if (IRCutUtils.IRcutAction(irState)){
						break;
					}
					try {
						sleep (200);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				AppLog.i (TAG, "IRCut switch suceess");
*/
				/*if (irState){
				    AppLog.i (TAG, "set camera Mono");
					//CameraHandlerThread.setColorEffectMono();
				} else {
				    AppLog.i (TAG, "set camera Normal");
					//CameraHandlerThread.setColorEffectNormal();
				}*/

				CameraHandlerThread.setColorEffect(irState);

				while (true) {
					if (IRCutUtils.IRcutAction(irState)){
						break;
					}
					try {
						sleep (200);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
/*
				try{
					//sleep(200);
					// 此处增加500ms 延时，不能解决一定几率IRCUT不切换问题
					sleep(200);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				IRCutUtils.IRcutAction(irState); */
            }
        }.start();

        //IRCutUtils.IRcutAction(irState);
		//AppLog.i(TAG,"IR cut exit");

		// 需要主线程延时，否则无法切换，why？
		/*
        try {
            sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }*/
		/*
		new Thread(){
			@Override
			public void run(){
				IRCutUtils.IRcutAction(irState);
			}
		}.start();*/
    }

    public static void sendIrCutSwBroadcase(Context context,boolean isRcd, int irState)
    {
    	/*if (context == null)
    	{
			AppLog.i(TAG,"Context is null, cancel the broadcast");
			return;
		}
        Intent intent = new Intent();
        intent.setAction(MainActivity.IR_CUT_MESSAGE);
        intent.putExtra(MainActivity.IR_CUT_STATE,irState);
        intent.putExtra(MainActivity.IR_CUT_IS_RCD,isRcd);
		intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND); 
        context.sendBroadcast(intent);*/
        
        //MainActivity.class.sendBroadcast(intent);
        //Message msg = new Message();
        //msg.obj = "网络数据";//可以是基本类型，可以是对象，可以是List、map等；
        //msg.what = 0;
        //msg.arg1 = isRcd? 1:0;
        //msg.arg2 = irState;
        //mIrHandler.sendMessage(msg);

    }

    /**
     * 处理按键的长按逻辑
     *
     * @param keyCode
     * @param event
     */
    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {
        //Setup.isEmphasisFile = true;
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Intent intent = new Intent(
                    Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            startActivityForResult(intent, 0);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_HOME) {
            return false;
        }  else if (keyCode == KeyEvent.KEYCODE_2)//白光灯控制
        {
            Setup.whiteLight = !Setup.whiteLight;
            if (Setup.whiteLight) {
                //HardwareControler.SetWhiteLightState(1);
                DevControl.writeGuobaoGpio(96,1);
            } else {
                //HardwareControler.SetWhiteLightState(0);
                DevControl.writeGuobaoGpio(96,1);
            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_3)
        {
        	// 录音长按切换 IR_CUT, 20180830
        	AppLog.i(TAG,"开启红外照明");
            if (Setup.inFraRed){
                return true;
            }

			if (Setup.VideoPrerecord){
				return true;
			}

            //IRCutActionProc(CameraHandlerThread.isRecordingGlobal);
            //sendIrCutSwBroadcase(this,CameraHandlerThread.isRecordingGlobal,Setup.currIRcutState);
            //IRCutActionProc(CameraHandlerThread.isRecordingGlobal,Setup.currIRcutState);
            AppLog.i(TAG, "check ir cut, 15:24");
            if (mIrCutEnable){
            	IRCutUtils.setIRstate(!IRCutUtils.getIRstate());
            	IRCutActionProc(Setup.videoRecording/*CameraHandlerThread.isRecordingGlobal*/,IRCutUtils.getIRstate());
				mIrCutEnable = false;
				new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mIrCutEnable = true;
                    }
                },2000);
            } else {
                Toast.makeText(this, R.string.no_frequent_ops, Toast.LENGTH_SHORT).show();
			}
            //IRCutUtils.IRcutAction(IRCutUtils.getIRstate());
			//AppLog.i(TAG,"IR cut exit");

            //IRCutUtils.IRcutAction();
        	/*
            if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
            {
                keySound.play(0, 0);
            }
            //========================================================================================================================
            if (!isAudioRecording) {
                btnSoundRecorder.setSelected(true);
                if (isRecording) {
                    btnVideoRecorder.setSelected(false);
                    isRecording = !isRecording;
                    videoRecord();
                }
                isAudioRecording = true;
            } else {
                btnSoundRecorder.setSelected(false);
                isAudioRecording = false;
            }
            if ( Setup.videoRecording ) {

                videoRecord();//停止视频播放
                handler.postDelayed(
                        new Runnable() {//延时1.5s后开始录制视频，否则surfaceview没有初始化好，会导致录制视频失败
                    @Override
                    public void run() {
                        setGLRenderer3();
                    }
                },1000);
            }else {
                setGLRenderer3();
            }
            //========================================================================================================================
            */

            return true;
        } else if (keyCode == KeyEvent.KEYCODE_4) {
           // Toast.makeText(MainActivity.this, "Long==>KEYCODE_4", Toast.LENGTH_SHORT).show();

            Intent intent = getPackageManager().getLaunchIntentForPackage("com.unionbroad.app");
            if (intent != null) {
                startActivity(intent);
            }

            return true;
        } else if (keyCode == KeyEvent.KEYCODE_5)//保存为重点文件KEYCODE_5
        {
        	// 长按视频按键，同样录制视频
        	// ycb 20180830 修改
            //  Toast.makeText(MainActivity.this, "Long==>KEYCODE_5", Toast.LENGTH_SHORT).show();

			

            if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
            {
                keySound.play(0, 0);
            }
            /*if (CameraHandlerThread.isRecordingGlobal){
                CameraHandlerThread.isRecordingGlobal = false;
                stopVideoRecord();
            }else{
                CameraHandlerThread.isRecordingGlobal = true;
                mVideoName = startVideoRecord();
            }*/
            if (Setup.videoRecording) {
				stopVideoRecord();
			} else {
				mVideoName = startVideoRecord();
			}
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_6)//保存为重点文件
        {
         //   Toast.makeText(MainActivity.this, "Long==>KEYCODE_6", Toast.LENGTH_SHORT).show();
            if (isAudioRecording) {
                Toast.makeText(MainActivity.this, R.string.no_photo_tips, Toast.LENGTH_SHORT).show();
                return true;
            }
            if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
            {
                keySound.play(1, 0);
            }

            takePictures();
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_7)     // 打开红外线
        {
        	/* 20180830修改，长按不响应，短按报警 */
            /** 与4G APP沟通，长按实现SOS功能 **/
            Intent intent = new Intent("gbpe.keys.sos");
            sendBroadcast(intent);
            AppLog.i(TAG,"开启 4G SOS 功能");
/*
        	AppLog.i(TAG,"开启红外照明");
            if (Setup.inFraRed){
                return true;
            }
            IRCutUtils.IRcutAction();

			new Thread()
			{
				@Override
				public void run()
				{
					AppLog.i(TAG,"close camera");
					CameraHandlerThread.getInstance().doStopCamera();
                    try {
                        sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    AppLog.i(TAG,"open camera");
					CameraHandlerThread.getInstance().getCameraInstance();
					CameraHandlerThread.getInstance().initCamera();
				}
			}.start();
            return true;
 */
            return true;
        }

        // return false;
        return true;
    }


    /**
     * 用于判断是短按还是长按。
     * 当程序中既要用到长按操
     * 作又要用到短按操作的话
     * 必须在onKeyDown（）方
     * 法中调用event.startTracking()
     * 判断是否为长按事件，否
     * 则无法实现长按
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

		AppLog.i(TAG,"get key down "+keyCode);

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_HOME) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_8) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_2) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_3) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_4) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_5) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_6) {
            shortOrLongPress(event);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_7) {
            shortOrLongPress(event);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }


    /**
     * 用于按键的短按逻辑处理
     *
     * @param keyCode
     * @param event   左边：2 5 7 右边 ：4 6 3
     */
    /* ycb 按键短按， 如果此时正在录制视频，则标记为重点视频 20180830 */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

		AppLog.i(TAG,"get key up "+keyCode);

        //普通录制之前先将重点标识置为false
       // Setup.isEmphasisFile = false;
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
        	// ycb 20180830修改，在录制视频时，标记为重点视频
        	/*
            Setup.isEmphasisFile = false;
            if (shortPress) {

            }*/

        	//AppLog.i(TAG,"screen test: "+ShellUtils.execAdbCmd("input keyevent 26"));

			if (Setup.videoRecording)
			{
                Toast.makeText(MainActivity.this,R.string.mark_as_important, Toast.LENGTH_SHORT).show();
				Setup.longPressVideo = true;
				Setup.isEmphasisFile = true;
			}
            shortPress = false;            
            return true;
        }
//----------------------------------激光灯------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_2) {
            //Setup.isEmphasisFile = false;
            if (shortPress) {
              //  Toast.makeText(MainActivity.this, "==>KEYCODE_2", Toast.LENGTH_SHORT).show();

                Setup.laserLamp = !Setup.laserLamp;
                if (Setup.laserLamp) {
                    //HardwareControler.SetLaserState(1);
                    DevControl.writeGuobaoGpio(97,1);
                } else {
                    //HardwareControler.SetLaserState(0);
                    DevControl.writeGuobaoGpio(97,0);
                }
            }
            shortPress = false;
            return true;
        }
//--------------------------------------录音------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_3) {
            if (shortPress) {
             //   Toast.makeText(MainActivity.this, "==>KEYCODE_3", Toast.LENGTH_SHORT).show();

			 	if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return true;
				}

                if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
                {
                    keySound.play(0, 0);
                }
       //====================================================================================================================================


            if (!isAudioRecording) {
                btnSoundRecorder.setSelected(true);
                //if (isRecording) {
                //if (CameraHandlerThread.isRecordingGlobal){
                if (Setup.videoRecording) {
                    btnVideoRecorder.setSelected(false);
                    //CameraHandlerThread.isRecordingGlobal = false;
                    stopVideoRecord();
                }
                isAudioRecording = true;
            } else {
                btnSoundRecorder.setSelected(false);
                isAudioRecording = false;
            }
            if ( Setup.videoRecording ) {
                //videoRecord();//停止视频播放
                stopVideoRecord();
                handler.postDelayed(new Runnable() {//延时1.5s后开始录制视频，否则surfaceview没有初始化好，会导致录制视频失败
                    @Override
                    public void run() {

                        // VideoManager6.startRecord();
                        setGLRenderer3();
                    }
                },1500);
            }else {
                setGLRenderer3();
            }
         }
     //=====================================================================================================================================

            shortPress = false;
            return true;
        }
//--------------------------------------PTT------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_4) {
            //Setup.isEmphasisFile = false;
            if (shortPress) {

   //======================================================================================================================================
                //if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||  Setup.videoRecording) {
                if (Setup.VideoPrerecord || Setup.videoRecording) {
                    Setup.currPage = false;
                    VideoManager6.stopAll();
                    Setup.videoRecording = false;
                    Setup.isNormalRecording = false;

                  //  Toast.makeText(MainActivity.this, "删除预录文件", Toast.LENGTH_SHORT).show();
                    //FileUtils.deleteFolder(Setup.pre_file_dir);
                    VideoManager6.clearPreFile();
                }
                if (isAudioRecording) {//如果正在录音，则停掉录音并保存
                    setGLRenderer3();
                    isAudioRecording = false;
                }

                //tid.setInVisible();
                //tid.rempveTimeRun();
                //Intent intent1 = new Intent(this, HomeActivity.class);
                Intent intent1 = new Intent(this, NewHomeActivity.class);
                startActivity(intent1);


            }
            shortPress = false;
            return true;
        }
//--------------------------------------录像------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_5) {
            if (shortPress) {
             //   Toast.makeText(MainActivity.this, "==>KEYCODE_5", Toast.LENGTH_SHORT).show();
             	if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return true;
				}
                if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
                {
                    keySound.play(0, 0);
                }
                /*if (CameraHandlerThread.isRecordingGlobal){
                    CameraHandlerThread.isRecordingGlobal = false;
                    stopVideoRecord();
                }else{
                    CameraHandlerThread.isRecordingGlobal = true;
                    mVideoName = startVideoRecord();
                }*/
                if (Setup.videoRecording) {
					stopVideoRecord();
				} else {
					mVideoName = startVideoRecord();
				}
                /*
                if (CameraHandlerThread.isRecordingGlobal){
					CameraHandlerThread.isRecordingGlobal = false;
					stopRecorder();
				} else {
					startRecorder();
				}*/
            }
            shortPress = false;
            return true;
        }
//------------------------------------ 拍照------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_6) {
            if (shortPress) {
              //  Toast.makeText(MainActivity.this, "==>KEYCODE_6", Toast.LENGTH_SHORT).show();
                /*if (isAudioRecording) {
                    Toast.makeText(MainActivity.this, "不可拍照", Toast.LENGTH_SHORT).show();
                    return true;
                }*/
                /*if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
                {
                    keySound.play(1, 0);
                }*/
                if (mPhotoEnable){
					mPhotoEnable = false;
                	takePictures();
                	new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            mPhotoEnable = true;
                        }
                    },1000);

                } else {
					Toast.makeText(this,R.string.wait_tips,Toast.LENGTH_SHORT).show();
				}
            }
            shortPress = false;
            return true;
        }
//----------------------------------紧急呼叫------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_7)//单按一次呼叫SOS、长按打开/关闭红外灯（按SOS白光灯发出报警闪烁信号，扬声器发出警笛声音）
        {
            //Setup.isEmphasisFile = false;
            Setup.isEmphasisFile = true;
            if (shortPress) {
               // Toast.makeText(MainActivity.this, "==>KEYCODE_7", Toast.LENGTH_SHORT).show();

                Setup.isWarn = !Setup.isWarn;
                if (Setup.isWarn) {
                     SOSUtil.sosStart(keySound);
                } else {
                     SOSUtil.sosStop();
                }
            }
            shortPress = false;
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_HOME)//单按一次呼叫SOS、长按打开/关闭红外灯（按SOS白光灯发出报警闪烁信号，扬声器发出警笛声音）
        {
        	AppLog.i(TAG,"home key");

             if (shortPress) {
                 final Context context = this;
                 // if (Setup.videoRecording||Setup.VideoPrerecord||Setup.isSegmentedVideo||isAudioRecording){
                 if (Setup.videoRecording||Setup.VideoPrerecord||isAudioRecording){
                     AlertDialog.Builder builder = new AlertDialog.Builder(this);
                     builder.setTitle("Warning");
                     builder.setMessage(R.string.recording_interrupt_tips);
                     builder.setCancelable(true);
                     builder.setPositiveButton(R.string.confirm, new DialogInterface.OnClickListener() {
                         @Override
                         public void onClick(DialogInterface dialog, int which) {
                             // if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||  Setup.videoRecording) {
                             if (Setup.VideoPrerecord || Setup.videoRecording) {
                                 Setup.currPage = false;
                                 VideoManager6.stopAll();
                                 Setup.videoRecording = false;
                                 Setup.isNormalRecording = false;
                                 //FileUtils.deleteFolder(Setup.pre_file_dir);
                                 VideoManager6.clearPreFile();
								 //Setup.videoRecording = false;
								 //CameraHandlerThread.isRecordingGlobal = false;
                             }
                             if (isAudioRecording) {//如果正在录音，则停掉录音并保存
                                 setGLRenderer3();
                                 isAudioRecording = false;
                             }
                             //Intent intent = new Intent(context, HomeActivity.class);
                             Intent intent = new Intent(context, NewHomeActivity.class);
                             startActivity(intent);
                         }
                     });

                     builder.setNegativeButton(context.getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
                         @Override
                         public void onClick(DialogInterface dialog, int which) {

                         }
                     });
                     Dialog dialog = builder.create();
                     dialog.show();
                 } else {
                     //Intent intent = new Intent(context, HomeActivity.class);
                     Intent intent = new Intent(context, NewHomeActivity.class);
                     startActivity(intent);
                 }


                 //tid.setInVisible();
                 //tid.rempveTimeRun();
            }
            shortPress = false;
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT){

            //AppLog.i(TAG,"left key");
            VolumeDown();

        } else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT){
            //AppLog.i(TAG,"right key");
            VolumeUp();
        }


        return super.onKeyUp(keyCode, event);
    }

    private void VolumeUp(){
        AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        int vol = audioManager.getStreamVolume(AudioManager.STREAM_SYSTEM);
        /*AppLog.i (TAG, "get volume: "+ vol);
        if (vol < audioManager.getStreamMaxVolume(AudioManager.STREAM_SYSTEM)) {
            audioManager.setStreamVolume(AudioManager.STREAM_SYSTEM, vol+1, 0);
        }*/
        audioManager.adjustStreamVolume(AudioManager.STREAM_MUSIC,AudioManager.ADJUST_RAISE,AudioManager.FLAG_PLAY_SOUND|AudioManager.FLAG_SHOW_UI);

    }

    private void VolumeDown(){
        AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        /*int vol = audioManager.getStreamVolume(AudioManager.STREAM_SYSTEM);
        AppLog.i (TAG, "get volume: "+ vol);
        if (vol > 0){
            audioManager.setStreamVolume(AudioManager.STREAM_SYSTEM,vol-1,0);
        }*/
        audioManager.adjustStreamVolume(AudioManager.STREAM_MUSIC,AudioManager.ADJUST_LOWER,AudioManager.FLAG_PLAY_SOUND|AudioManager.FLAG_SHOW_UI);
    }


    /**
     * 是否为长按或短按事件的逻辑判断方法
     *
     * @param event
     */
    private void shortOrLongPress(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            event.startTracking();
            if (event.getRepeatCount() == 0) {
                shortPress = true;
            } else {
                shortPress = false;
            }
        }
    }

    private boolean getChargeState(){
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent batteryStatusIntent = registerReceiver(null, ifilter);
        int status = batteryStatusIntent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
        boolean isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                status == BatteryManager.BATTERY_STATUS_FULL;
        return  isCharging;
    }


    @Override
    protected void onResume() {
    	AppLog.d (TAG, "onResume");
        super.onResume();
        Setup.currPage = true;
        boolean isCharge = getChargeState();
        if (isCharge){
            //AppLog.i(TAG, "onResume: on charge");
            onChargeProc();
        } else {
            //AppLog.i(TAG, "onResume: off charge");
            offChargeProc();
        }
        //System.out.println("------>>>>xml" + XMLStrUtil.getXMLString2(this));
        IntentFilter intentFilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        intentFilter.addAction(Intent.ACTION_POWER_CONNECTED);
        intentFilter.addAction(Intent.ACTION_POWER_DISCONNECTED);
        registerReceiver(batteryChangedReceiver, intentFilter);
        canAnimater = true;
        handler.removeCallbacks(chargingRunnable);
        handler.post(chargingRunnable);

        cancleScreenOn();

        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
		// delete by ycb for test
		/*
        receiver.setBatteryElectric(new BatteryReceiver.BatteryElectric() {
            @Override
            public void getElectricLevel(int percent) {
                tv_battery_level.setText(percent + "%");
                Setup.battery_level = percent + "%";
            }
        });
        */

        //更新activity的view显示
        showText();
        checkSim();

		// 此处启动预录功能
		// 需要考虑的是录像熄屏的情况，预录熄屏的情况，此处仅是开机开启预录，修改预录设置后开启预录
		if (!CameraHandlerThread.startRecordingFlag) {
	        if (Setup.VideoPrerecord){
				if (!SDCardUtils.isSDCardEnable()){
					Toast.makeText(this,R.string.no_sdcard_tips,Toast.LENGTH_SHORT).show();
				}else {
	                new Handler().postDelayed(new Runnable() {
	                    @Override
	                    public void run() {
	                    	AppLog.i (TAG, "开始预录");
	                        VideoManager6.startRecord();
	                    }
	                }, 1000);
	            }
	        }
		}

		if (false == CameraHandlerThread.isCameraOpened(context,false)){
			CameraHandlerThread.getInstance().getCameraInstance();
			CameraHandlerThread.getInstance().initCamera();
		}

		// 启动GPS
		EventBus.getDefault().post(new OnGetLocationEvent(OnGetLocationEvent.GPS_START));

		PollService.setServiceAlarm(this,true);

    }

    @Override
    protected void onPause() {
    	AppLog.d (TAG, "onPause");
        super.onPause();
        canAnimater = false;
		if (batteryChangedReceiver!=null){
			unregisterReceiver(batteryChangedReceiver);
		}

		if (chargingRunnable!=null){
        	handler.removeCallbacks(chargingRunnable);
		}

		if (myListener!=null){
			compassSensorManager.unregisterListener(myListener);
		}
		//if ((!CameraHandlerThread.isRecordingGlobal)&&(!Setup.VideoPrerecord)){
		if (!CameraHandlerThread.startRecordingFlag) {                  // 如果正在录制，包括预录和录像
            if (CameraHandlerThread.isCameraOpened(context,false)){
                CameraHandlerThread.getInstance().doStopCamera();
            }
            // 关闭GPS
            EventBus.getDefault().post(new OnGetLocationEvent(OnGetLocationEvent.GPS_STOP));
		}

		PollService.setServiceAlarm(this,false);
		//AppLog.d (TAG, "onPause OK");

		
     }


    /**
     * 视频录制
     */
     /*
    //public void videoRecord() {
    public void videoRecord(final String path) {
        //开始或者停止录音都有震动提示
		AppLog.i(TAG, "videoRecord enter");
		
        if (CameraHandlerThread.camera == null) {
            Toast.makeText(MainActivity.this, "摄像头异常9", Toast.LENGTH_LONG).show();
            return;
        }

		AppLog.i(TAG, "Setup.videoRecording: "+Setup.videoRecording);
        if (! Setup.videoRecording) {
            //如果正在预录，则先停掉预录
            if (Setup.VideoPrerecord) {
                VideoManager6.stopRecord();
            }

            if (isAudioRecording)//正在录制声音的时候，想要录视频，
            {
                //停止并保存录音
                setGLRenderer3();
                isAudioRecording = false;
                handler.postDelayed(new Runnable() {//延时1.5s后开始录制视频，否则surfaceview没有初始化好，会导致录制视频失败
                    @Override
                    public void run() {
                        Setup.isNormalRecording = true;
                        Setup.videoRecording = true;
                        if (path == null) {
                            mVideoName = VideoManager6.startRecord();
                        }else {
                            mVideoName = VideoManager6.startRecord(null);
                        }
                     }
                },1500);

            }else {

				AppLog.i(TAG, "VideoManager6.startRecord()");
                Setup.isNormalRecording = true;
                Setup.videoRecording = true;
                if (path == null) {
                    mVideoName = VideoManager6.startRecord();
                } else {
                    mVideoName = VideoManager6.startRecord(path);
                }
             }
         //   Toast.makeText(MainActivity.this, "开始录制视频", Toast.LENGTH_SHORT).show();

        } else {

			AppLog.i(TAG,"stopRecord()");
			
            Setup.videoRecording = false;
            VideoManager6.stopRecord();
            Setup.isNormalRecording = false;
            Setup.longPressVideo =false;

			// add by ycb, for handle flag videos
			if (Setup.isEmphasisFile)
			{
				
				Setup.isEmphasisFile = false;
				ArrayList<String> fileList = new ArrayList<String>();
				fileList.add(mVideoName);
                SelectHandleUtil.mark(this, fileList, new SelectHandleUtil.OnFileHandleListener() {
                    @Override
                    public void onFileHandleStart() {
                        AppLog.i(TAG,"开始移动文件");
                    }

                    @Override
                    public void onOneFileHandleFinished(int progress) {
                        AppLog.i(TAG,"文件移动完成");
                    }

                    @Override
                    public void onAllFileHandleFinished() {

                    }

                    @Override
                    public void onFileCancle() {

                    }
                });
				
			}
			
         //   Toast.makeText(MainActivity.this, "停止录制视频", Toast.LENGTH_SHORT).show();

        }

        //没有设置延时录像的时候，则停止正常录像后就开始预录，如果设置了延时录像则由延时录像完成之后开始预录
        if (!Setup.isVideoDelay){
            if (!Setup.isNormalRecording){//预录
                VideoManager6.previewRecord();
            }
        }

    }

    public void videoRecord(){
        videoRecord(null);
    }*/

/*
     public void stopVideoRecord(){
         mediaRecorder.stop();
         mediaRecorder.release();
         mediaRecorder = null;
         if (camera != null) {
             camera.release();
         }
     }
*/

/*
	public String startVideoRecord(){
		try {
			mediaRecorder = new MediaRecorder();// 创建mediarecorder对象
			//摄像头旋转90度
			camera = Camera.open(Camera.CameraInfo.CAMERA_FACING_FRONT);
			camera.setDisplayOrientation(90);
			camera.unlock();
            mediaRecorder.setCamera(camera);
            // 设置录制视频源为Camera(相机)
			mediaRecorder.setVideoSource(Camera.CameraInfo.CAMERA_FACING_FRONT);
			//录像旋转90度
			mediaRecorder.setOrientationHint(90);
			// 设置录制完成后视频的封装格式THREE_GPP为3gp.MPEG_4为mp4
			mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
			// 设置录制的视频编码h263 h264
			mediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
			//设置高质量录制,改变码率
			mediaRecorder.setVideoEncodingBitRate(5*1024*1024);
			// 设置视频录制的分辨率。必须放在设置编码和格式的后面，否则报错
			mediaRecorder.setVideoSize(1920, 1080);
			// 设置录制的视频帧率。必须放在设置编码和格式的后面，否则报错
			mediaRecorder.setVideoFrameRate(20);
			// 设置视频文件输出的路径
			mediaRecorder.setOutputFile("/sdcard/video.mp4");
			// 设置捕获视频图像的预览界面
			//mediaRecorder.setPreviewDisplay(mSurfaceView.getHolder().getSurface());
            mediaRecorder.setPreviewDisplay(cameraSurfaceView.getSurfaceHold().getSurface());
 
			mediaRecorder.setOnErrorListener(new MediaRecorder.OnErrorListener() {
 
				@Override
				public void onError(MediaRecorder mr, int what, int extra) {
					// 发生错误，停止录制
					mediaRecorder.stop();
					mediaRecorder.release();
					mediaRecorder = null;
					//isRecording = false;

				}
			});
 
			// 准备、开始
			mediaRecorder.prepare();
			mediaRecorder.start();
 
			//startButton.setEnabled(false);
			//stopButton.setEnabled(true);
			//isRecording = true;
		} catch (Exception e) {
			//e.printStackTrace();
            AppLog.e (TAG,e.toString());
		}

		return null;
 
	}
*/


	public String startVideoRecord(){
		return startVideoRecord(null);
	}
	
	public String startVideoRecord(final String path)    {
    //public String startVideoRecord(){
		//开始或者停止录音都有震动提示

		AppLog.i(TAG, "videoRecord enter");
		
        if (CameraHandlerThread.camera == null) {
            Toast.makeText(MainActivity.this, R.string.camera_exception, Toast.LENGTH_LONG).show();
            return null;
        }

		//tid.start(true);
        //tid.start(true);
        //tid.start(false);

		// Setup.videoRecording 做为录制标志是否合适
		if (! Setup.videoRecording) {
            //如果正在预录，则先停掉预录
            /*if (Setup.VideoPrerecord) {
                VideoManager6.stopRecord(false);
            }*/
            if (Setup.VideoPrerecord){
				// 直接returen
				AppLog.d(TAG, "正在预录，开始录制，"+VideoManager6.getRcdFilePath());
				Setup.videoRecording = true;
				VideoManager6.interruptPreRcd();
				//tid.stop();
				//tid.start(true,0);
				int stk = TimeIndicator.getTimeTick();
				if (stk>10) {
					stk = 11;
				}
				tid.start(true,stk /*,false*/);
				// 启动定时器
				Setup.isNormalRecording = true;
				VideoManager6.segmentdVideo();
				return  VideoManager6.getRcdFilePath();
			}

            if (isAudioRecording)//正在录制声音的时候，想要录视频，
            {
                //停止并保存录音
                setGLRenderer3();
                isAudioRecording = false;
                handler.postDelayed(new Runnable() {//延时1.5s后开始录制视频，否则surfaceview没有初始化好，会导致录制视频失败
                    @Override
                    public void run() {
                        Setup.isNormalRecording = true;
                        Setup.videoRecording = true;
                        if (path == null) {
                            mVideoName = VideoManager6.startRecord();
                        }else {
                            mVideoName = VideoManager6.startRecord(null);
                        }
                        //mVideoName = VideoManager6.startRecord();
                     }
                },1500);

            }else {

				AppLog.i(TAG, "VideoManager6.startRecord()");
                Setup.isNormalRecording = true;
                Setup.videoRecording = true;
                if (path == null) {
                    return VideoManager6.startRecord();
                } else {
                    //mVideoName = VideoManager6.startRecord(path);
                    return VideoManager6.startRecord(path);
                    
                }
                //return VideoManager6.startRecord();
             }
         //   Toast.makeText(MainActivity.this, "开始录制视频", Toast.LENGTH_SHORT).show();

        } 

		return null;
	}

	public void pauseVideoRecord(){
		AppLog.i(TAG,"stopRecord()");
		Setup.videoRecording = false;
        VideoManager6.stopRecord(true);
        Setup.isNormalRecording = false;
	}

	public void stopVideoRecord(){
		AppLog.i(TAG,"stopRecord()");
			
        Setup.videoRecording = false;
        VideoManager6.stopRecord(false);
        Setup.isNormalRecording = false;
		mRecordTick = 0;
        //Setup.longPressVideo =false;
        //tid.stop();

		// add by ycb, for handle flag videos
		if (Setup.isEmphasisFile){
			
			Setup.isEmphasisFile = false;
			ArrayList<String> fileList = new ArrayList<String>();
			fileList.add(mVideoName);
            SelectHandleUtil.mark(this, fileList, new SelectHandleUtil.OnFileHandleListener() {
                @Override
                public void onFileHandleStart() {
                    AppLog.i(TAG,"开始移动文件");
                }

                @Override
                public void onOneFileHandleFinished(int progress) {
                    AppLog.i(TAG,"文件移动完成");
                }

                @Override
                public void onAllFileHandleFinished() {

                }

                @Override
                public void onFileCancle() {

                }
            });
			
		}

		// 如果有预录，则合并预录
		if (Setup.VideoPrerecord){
			final String preVideo = VideoManager6.getPreRcdFilePath();
			if (preVideo != null){
				new Thread(){
						@Override
						public void run(){
//						ArrayList<String> videoList = new ArrayList<>();
                        //MergerMp4 mergerMp4 = new MergerMp4(mVideoName);
                        // rename the files
                            File f1 = new File(preVideo);
                            //f1.renameTo(new File(preVideo.replace(".prv",".seg")));
                            File f2 = new File(mVideoName);
                            if (f1.exists() && f2.exists()) {
                                MergerMp4 mergerMp4 = new MergerMp4(mVideoName.replace(".prv", ""));

                                String tmpFile1 = preVideo.replace(".prv",".seg");
                                if (!f1.renameTo(new File(tmpFile1))){
                                    AppLog.i(TAG, "rename failed");
                                    return;
                                }

                                String tmpFile2 = mVideoName.replace(".prv", ".seg");
                                if (!f2.renameTo(new File(tmpFile2))) {
                                    AppLog.i(TAG, "rename failed");
                                    return;
                                }
                                mergerMp4.addMp4(tmpFile1);
                                mergerMp4.addMp4(tmpFile2);
                                //videoList.add(preVideo);
                                //videoList.add(mVideoName);
                                // 此处把两个预录文件的路径设置为空，以防再次预录的时候，删除了预录文件，导致合并出错
                                VideoManager6.clearPreFile();
                                AppLog.d(TAG, "pre rcd combin: " + tmpFile1 + "+" + tmpFile2);
                                //VideoHandleUtils vhu = new VideoHandleUtils(true);
                                //vhu.combine(videoList);
                                mergerMp4.mergeMP4();
                                new File(tmpFile1).delete();
                                new File(tmpFile2).delete();
                                AppLog.d(TAG,"combin ok");
                                //new File(preVideo).delete();
                            }
                        // 合并完毕之后再删除文件，否则会有问题
                        //VideoManager6.clearPreFile();
						//VideoManager6.previewRecord()
                        /*    mPreRecordHandler.post(new Runnable() {
                                @Override
                                public void run() {
                                    VideoManager6.previewRecord();
                                }
                            });
                            */

					}
				}.start();

				// 开启预录
				
			}else{
				// 上一段视频已合并，预录时间没有超过一段的时间，没有新文件产生，就不需要合并
				// 第一次开机，还没有预录文件，也不需要合并
				AppLog.i(TAG,"无需合并");
			}

			VideoManager6.previewRecord();
			
		}

	}
	
    /**
     * 拍照
     */
    private boolean isPhotoManagerBusy = false;
    private Handler photoHnadler = new Handler();
    private void takePictures() {

        if (CameraHandlerThread.camera == null) {
            Toast.makeText(MainActivity.this, R.string.camera_exception, Toast.LENGTH_LONG).show();
            return;
        }
        if (isAudioRecording) {
			Toast.makeText(MainActivity.this, R.string.on_audio_recording, Toast.LENGTH_SHORT).show();
            return;
        }

		if (!SDCardUtils.isSDCardEnable()) {
			Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
			return;
		}

		if (isPhotoManagerBusy) {
            Toast.makeText(MainActivity.this,R.string.photo_manager_busy, Toast.LENGTH_SHORT).show();
            return;
        }

		if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(1, 0);
        }

        ++Setup.titalFilesNum;
        PhotoManager.getInstance().
                startPhotograph(cameraSurfaceView.getSurfaceHold(), this, new PhotoManager.SaveCallBack() {
                    @Override
                    public void onDataSaved(final Context context) {
                        isPhotoManagerBusy = false;
                        photoHnadler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(context,R.string.photo_ok_tips,Toast.LENGTH_SHORT).show();
                            }
                        });
                        //Toast.makeText(context,R.string.photo_ok_tips,Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onNextShoot(final Context context, final int num) {
                        photoHnadler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(context,String.format("%d",num),Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                    @Override
                    public void onFinished(final Context context) {
                        isPhotoManagerBusy = false;
                        photoHnadler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(context,R.string.photo_ok_tips,Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                });
        LogUtils.writeTxtToFile(getString(R.string.take_picture), Setup.LOG_PATH_SDCARD_DIR, TimeUtils.getLogDate() + ".log");

		Toast.makeText(MainActivity.this, R.string.take_picture, Toast.LENGTH_SHORT).show();

    }

    /**
     * 不同的系统会选择对应的不同
     * 的设置，因此需要写两种格式的
     */
    public void setTextClock() {
        textClock.setFormat24Hour("hh:mm:ss,EEEE");
        tcDate.setFormat24Hour("yyyy/MM/dd");
        textClock.setFormat12Hour("hh:mm:ss,EEEE");
        tcDate.setFormat12Hour("yyyy/MM/dd");
    }


    //=============================================================================================================
    @Override
    public void loadViewLayout() {
        // 隐藏标题栏
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        window = getWindow();
        //设置全屏
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        // 选择支持半透明模式,在有surfaceview的activity中使用。
        window.setFormat(PixelFormat.TRANSLUCENT);
        setContentView(R.layout.activity_main);
    }


    @Override
    public void findViewById() {
        switchBtn = (Button) findViewById(R.id.change);
         btnVideoRecorder = (ImageView) findViewById(R.id.iv_video_record);
        btnTakePhotos = (ImageView) findViewById(R.id.iv_take_picture);
        btnSoundRecorder = (ImageView) findViewById(R.id.iv_audio_reocrd);
        tv_menue = (ImageButton) findViewById(R.id.tv_menue);
        ll_bottom_button = (RelativeLayout) findViewById(R.id.ll_bottom_button);
        iv_memery = (MemoryView) findViewById(R.id.iv_memory);
        gps_indicator = findViewById(R.id.gps_available);

		/*显示录制时间的控件*/
        chronometer = (Chronometer) findViewById(R.id.timer_main);
        textClock = (TextClock) findViewById(R.id.tc_time);
        tcDate = (TextClock) findViewById(R.id.tc_date);
        tv_tital = (TextView) findViewById(R.id.tv_tital);
        tv_pid = (TextView) findViewById(R.id.tv_pid);
        tv_sn = (TextView) findViewById(R.id.tv_sn);
        iv_cd = (ImageView) findViewById(R.id.iv_cd);
        tv_memory = (TextView) findViewById(R.id.tv_memory);
        tv_battery_level = (TextView) findViewById(R.id.tv_battery_level);

		// delete 20180820
        cdtShow = (CountDownTimerView)findViewById(R.id.cdt_show);

        //button = (Button) findViewById(R.id.vvvv);
           findViewById(R.id.btn_jump2).setOnClickListener(new View.OnClickListener() {
                       @Override
                       public void onClick(View v) {
                       //    startActivity(new Intent(MainActivity.this, CompassActivity.class));
                           if (Setup.VideoPrerecord)
                           VideoManager6.startRecord();

                        }
                   });

        tid = (TimeIndicator)findViewById(R.id.tid);
    }

    @Override
    public void setListener() {

        btnVideoRecorder.setOnClickListener(this);
        btnVideoRecorder.setOnLongClickListener(this);
        btnTakePhotos.setOnClickListener(this);
        btnTakePhotos.setOnLongClickListener(this);
        btnSoundRecorder.setOnClickListener(this);
        btnSoundRecorder.setOnLongClickListener(this);
        tv_menue.setOnClickListener(this);
        ll_bottom_button.setOnClickListener(this);
        switchBtn.setOnClickListener(this);

/*


        findViewById(com.topvision.videodemo.R.id.btn_resolu1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                CameraHandlerThread.getInstance().camera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 1920;
                CameraWrapper.SRC_IMAGE_HEIGHT = 1080;
                CameraWrapper.DST_IMAGE_WIDTH  = 1920;
                CameraWrapper.DST_IMAGE_HEIGHT = 1080;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraHandlerThread.getInstance().initCamera();
            }
        });

        findViewById(com.topvision.videodemo.R.id.btn_resolu2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraHandlerThread.getInstance().camera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 1280;
                CameraWrapper.SRC_IMAGE_HEIGHT = 720;
                CameraWrapper.DST_IMAGE_WIDTH  = 1280;
                CameraWrapper.DST_IMAGE_HEIGHT = 720;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraHandlerThread.getInstance().initCamera();
            }
        });

        findViewById(com.topvision.videodemo.R.id.btn_resolu3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraHandlerThread.getInstance().camera.stopPreview();
                YuvUtils.releaseMemo();
                CameraWrapper.SRC_IMAGE_WIDTH = 640;
                CameraWrapper.SRC_IMAGE_HEIGHT = 480;
                CameraWrapper.DST_IMAGE_WIDTH  = 640;
                CameraWrapper.DST_IMAGE_HEIGHT = 480;
                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
                CameraHandlerThread.getInstance().initCamera();

            }
        });
        findViewById(com.topvision.videodemo.R.id.btn_resolu4).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                CameraHandlerThread.getInstance().camera.stopPreview();
//                YuvUtils.releaseMemo();
//                CameraWrapper.SRC_IMAGE_WIDTH = 320;
//                CameraWrapper.SRC_IMAGE_HEIGHT = 240;
//                CameraWrapper.DST_IMAGE_WIDTH  = 320;
//                CameraWrapper.DST_IMAGE_HEIGHT = 240;
//                YuvUtils.allocateMemo(CameraWrapper.SRC_IMAGE_WIDTH * CameraWrapper.SRC_IMAGE_HEIGHT * 3 / 2, 0,
//                        CameraWrapper.DST_IMAGE_WIDTH * CameraWrapper.DST_IMAGE_HEIGHT * 3 / 2);
//                CameraHandlerThread.getInstance().initCamera();

                new Thread(new Runnable() {
                    @Override
                    public void run() {

                       handleUtils. combine("/storage/sdcard0/DCIM/SEGTEMP/");//8888888_000000_20160101000139_0031
                    }
                }).start();

             }
        });
*/

    }

    private int getBatteryVol() {

        try {
            FileReader batRd = new FileReader(BAT_VOLTAGE_FILE);
            BufferedReader bat_br = new BufferedReader(batRd);
            String strVol = bat_br.readLine();
            return Integer.parseInt(strVol);

        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }

//VideoHandleUtils handleUtils = null;
    @Override
    public void initConfig() {
        //setGLRenderer();
        // add by ycb, 20181130 to avoid screen problem
        // 修改了pl之后，屏幕初始化参数正常，无需灭屏操作
        //resetScreen();
		shutdownAdb();
		//adbSet(false);       // 首先关闭adb
		setIME();
		//adbSet(getAdbConfig());
		
        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
		// delete by ycb for test
        //receiver = new BatteryReceiver();
        pow_action_receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
            	String action = intent.getAction();
                //AppLog.i(TAG,"receive screen action broadcast, "+action);

				/* 屏幕熄灭广播 */
                if (action.equals(Intent.ACTION_SCREEN_OFF))
                {
                    AppLog.i(TAG,"screen off");
					setScreenState(false);

					//if (false == Setup.videoRecording) // 不在录像，则关闭摄像头
					// 需要判断是否在预录，否则熄屏后预录会出错
					// if ((!CameraHandlerThread.isRecordingGlobal)&&(!Setup.VideoPrerecord))
					if (!CameraHandlerThread.startRecordingFlag) {
						
						AppLog.i(TAG,"stop camera");
						if (CameraHandlerThread.isCameraOpened(context,false))
						{
							CameraHandlerThread.getInstance().doStopCamera();
						/*
							new Handler().postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    CameraHandlerThread.getInstance().doStopCamera();
                                }
                            },1000);
                         */

							/*
							handler.postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    AppLog.i(TAG, "cameraTuning");
                                    HardControl.cameraTuning(Setup.currIRcutState);
                                }
                            },1000);*/
						}
					}
					
                } /*屏幕点亮广播*/
				else if(action.equals(Intent.ACTION_SCREEN_ON)){
                    AppLog.i(TAG,"screen on");
					setScreenState(true);
					/* 20180910 加入红外切换动作 */
					checkInfrared(false);
					// ycb, 20190107
					//HardwareControler.SetGreenLightState(1);
                    DevControl.writeGuobaoGpio(20,1);

					if (false == CameraHandlerThread.isCameraOpened(context,true))
					{
						AppLog.i(TAG,"initCamera");
						//CameraHandlerThread.getInstance().initCamera();
						/*if (IRCutUtils.getIRstate()){
							CameraHandlerThread.setIsColorEffectMono(true);
						}*/
						CameraHandlerThread.getInstance().getCameraInstance();
						CameraHandlerThread.getInstance().initCamera();

/*
						new Thread(){
						    @Override
                            public void run(){
						        if (IRCutUtils.getIRstate()){
                                    try {
                                        CameraHandlerThread.getInstance().getCameraInstance();
                                        CameraHandlerThread.getInstance().initCamera();
                                        sleep(1000);
                                        CameraHandlerThread.getInstance().doStopCamera();
                                        sleep(1000);

                                    } catch (InterruptedException e) {
                                        e.printStackTrace();
                                    }
                                }
                                CameraHandlerThread.getInstance().getCameraInstance();
                                CameraHandlerThread.getInstance().initCamera();
                            }
                        }.start();*/
						
						//finish();
                		//startActivity(new Intent(context,MainActivity.class));
					}
                } else if(action.equals(Intent.ACTION_BATTERY_CHANGED))
				{
					//AppLog.i(TAG, "Battery info");
					int current=intent.getExtras().getInt("level");//获得当前电量
        			int total=intent.getExtras().getInt("scale");//获得总电量
					int percent = current*100/total;

					//AppLog.i(TAG, "battery percentage: "+percent);					
					tv_battery_level.setText(percent + "%");
                	Setup.battery_level = percent + "%";

					// ybc 20180906: 检测电量，如果低于2%，则提示关机
                    if (percent < 2) {
                        if (getBatteryVol() < 3450) {
                            //if (isRecording)
                            //if (CameraHandlerThread.isRecordingGlobal)
                            //if (CameraHandlerThread.startRecordingFlag) {
                            if (Setup.videoRecording) {
                                //videoRecord(); //åœæ­¢å½•åƒ
                                stopVideoRecord();
                                //CameraHandlerThread.isRecordingGlobal = false; //
                                AppLog.w(TAG,"battery low, stop recording now!");
                                //Toast.makeText(MainActivity.this, R.string.no_battery_tips, Toast.LENGTH_SHORT).show();
                                new Handler().postDelayed(new Runnable() {
                                    @Override
                                    public void run() {
                                        AppLog.w(TAG, "power off, good night");
                                        shutDown();
                                    }
                                }, 5000);
                            }
                        }
                    }

                    //AppLog.d(TAG, "bat voltage: "+getBatteryVol());
					
				}else if(action.equals(MainActivity.IR_CUT_MESSAGE))
				{
					//AppLog.i(TAG,"receive ir cut message");
					//boolean isRcd = intent.getBooleanExtra(MainActivity.IR_CUT_IS_RCD,false);
					//int irState = intent.getIntExtra(MainActivity.IR_CUT_STATE,0);
					//IRCutActionProc(isRcd,irState);
				}else if (action.equals(Intent.ACTION_SHUTDOWN)) {

				    AppLog.w (TAG, "receive Power off broadcast");
					AppLog.w (TAG,"bat: "+getBatteryVol());
                    //if (CameraHandlerThread.startRecordingFlag) {
                    if (Setup.videoRecording) {
						AppLog.w (TAG,"stop recording");
                        stopVideoRecord();
                        //CameraHandlerThread.isRecordingGlobal = false; // 20181210 æ­¤æ ‡å¿—éœ€è®¾ç½®ï¼Œå¦åˆ™ï¼ŒæŒ‰ä¸‹æ— å“åº?
                    }

                }
            }
        };

        ImageView imageBatteryTest = (ImageView) findViewById(R.id.iv_main);
        LayerDrawable layerDrawableTest = (LayerDrawable) imageBatteryTest.getDrawable();
        clipDrawableTest = (ClipDrawable) layerDrawableTest
                .findDrawableByLayerId(R.id.clip_drawable);

        //创建数据库
        dbHelper = new DBHelper(this);
        DBHelper.quary(dbHelper);

        showText();
        showFreeSDSize();
        checkSim();//判断SIM卡是否可用

		getCurrentNetDBM(this);
        setTextClock();////显示当前系统日期和时间
        // registerBatteryRec();//注册监听电池电量广播

		// add by ycb, 注册屏幕监控
		registerScreenActionRecv();
		// for compass test
		OrientationTest();
		
        vibrator();//开机是否震动
//        //语音播报功能
		mPreRecordHandler = new Handler();

/*      一个纵欲过度的老男人声音，每次开机呻吟一次
        util.textToSpeech(R.string.police_number + SettingUtils.
                getStrValues(this, "policeNumber", "111111") + R.string.power_on);
*/
        //开启service,防止APP被系统进程杀掉
        //Intent intent = new Intent();
        //intent.setClass(this, MyService.class);
        //this.startService(intent);

		// 20181009 添加开机 ir cut 切换
		IRCutUtils.IRcutAction(IRCutUtils.getIRstate());

        if(!PreferenceManager.getDefaultSharedPreferences(this).getBoolean(IR_CUT_INIT,false)) {
			// 初始化一次
            checkInfrared(true);
            PreferenceManager.getDefaultSharedPreferences(this)
                    .edit()
                    .putBoolean(IR_CUT_INIT,true)
                    .commit();
        }

        mCmrHandler = new Handler(){

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case 1001:                        
                        //resetScreen();
						setGLRenderer();
						//EventBus.getDefault().post(new OnGetLocationEvent("获取经纬度"));
                        //EventBus.getDefault().post(new OnGetLocationEvent(OnGetLocationEvent.GPS_START));
						//resetScreen();
						/*new Handler().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                AppLog.i (TAG,"screen on");
                                try {
                                    setScreenOn(null);
                                }catch (Exception e) {
                                    AppLog.i (TAG, "setScreenOn error: "+e.toString());
                                }

                            }
                        },1000);*/
                        break;
                }
            }
        };

        new Thread(){
            @Override
            public void run() {
                try{
                    sleep(1000);
                    Message msg = new Message();
                    msg.what = 1001;
                    mCmrHandler.sendMessage(msg);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

		// otg power up test, ycb20181110
		//HardwareControler.setGPIODirection(53, 1);
        //HardwareControler.setGPIOValue(53,1);



        // 给该类添加context，以发送广播
        //IRCutUtils.setmContext(this);
		
        /*mIrHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what)
                {
                    case 0:
                        AppLog.i(TAG, "ir cut message");
                        break;
                }
            }
        };*/

		/* 合并temp文件夹 */
        //handleUtils = new VideoHandleUtils(true);
        //PollService.setServiceAlarm(this,true);
     }

     private void checkInfrared(final boolean isMove){
     	 /*此处获得ir cut 方式，自动或者手动*/
         final String infrared = PreferenceManager.getDefaultSharedPreferences(this)
                 .getString(SettingFragment.INFRARED,"");
         AppLog.i(TAG,"infrared: "+infrared);

         new Thread(){
             @Override
             public void run(){
                 try{
                     sleep(2000);
          
					 /* 此处设置ir cut自动或手动方式，并触发ir cut Event bus*/
					 /*if (isAdminDevice){
						setScreenOff(null);
					 }*/
	                 SettingFragment.setInfraredPub(infrared);
					 if (isMove) {
	                     IRCutUtils.IRcutAction(IRCutUtils.getIRstate());
	                 }
					 /*
					 if (isAdminDevice){
					     sleep(1000);
						setScreenOn(null);
					 }*/

				 } catch (InterruptedException e){
                     e.printStackTrace();
                 }
				 
				 
             }

         }.start();
     }
//============================================================================================================

    /**
     * 判断SIM卡是否可用
     */
    private void checkSim() {
        iv_memery.setImgSrc();
    }

    //@Override
    //protected void onStop() {
    //    HardwareControler.setGPIODirection(62, 0);
     //   HardwareControler.setGPIOValue(62,0);
        //Setup.inFraRed = false;
       // super.onStop();
   // }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // myService.writeLogs(getString(R.string.power_off));
        //util.textToSpeech(R.string.police_number + SettingUtils.getStrValues(this, "policeNumber", "111111") + R.string.power_off);
        vibrator();
        LogUtils.writeTxtToFile(getString(R.string.power_off), Setup.LOG_PATH_SDCARD_DIR,
                TimeUtils.getLogDate() + ".log");
        unRegistgerBatterRec();
		unRegisterScreenActionRecv();

       //util.releaseTextToSpeech();

        if (CameraHandlerThread.camera != null) {
            stopCameraPreview();
        }

         //=====================================================================================
        if (Setup.videoRecording){
            VideoManager6.stopRecord(false);
        }

        //com.guobao.lightboard.HardwareControler.SetInfraredLaserState(0);
        //HardwareControler.setGPIODirection(62, 0);
        //HardwareControler.setGPIOValue(62,0);

        DevControl.writeGuobaoGpio(62,0);
        Setup.inFraRed = false;
    }


    //加上static，变成静态匿名内部类,防止内存泄露
    public static void stopCameraPreview() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                if (CameraHandlerThread.camera != null) {
                    CameraHandlerThread.camera.stopPreview();   //停止预览
                    CameraHandlerThread.camera.release();       //释放Camera资源
                    CameraHandlerThread.camera = null;
                }
            }
        }).start();
    }


    /**
     * 注册电池电量监控广播
     */
    private void registerBatteryRec() {
        IntentFilter filter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(pow_action_receiver, filter);//注册BroadcastReceiver

    }

    /**
     * 取消注册电量监控广播
     */
    private void unRegistgerBatterRec() {
		// delete by ycb for test
        //unregisterReceiver(receiver);
    }

	/*
	*  注册SCREEN_OFF广播
	*  add by ycb
	*/
	private void registerScreenActionRecv()
    {
    	AppLog.i(TAG,"registerScreenActionRecv");
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SCREEN_ON);
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_BATTERY_CHANGED);
		// 接收关机广播
		filter.addAction(Intent.ACTION_SHUTDOWN);
		filter.addAction(MainActivity.IR_CUT_MESSAGE);
		registerReceiver(pow_action_receiver,filter);
	}

	private void unRegisterScreenActionRecv()
	{
		unregisterReceiver(pow_action_receiver);
	}
	


    /**
     * 设置屏幕常亮
     */
    public void keepScreenOn() {

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    /**
     * 取消屏幕常亮
     */
    public void cancleScreenOn() {

        window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    /**
     * 系统是否震动提示
     */
    private void vibrator() {
        if (SettingUtils.getBoolenValues(this, "vibrate")) {
            if (vibrator != null) {
                vibrator.vibrate(300);
            }
        }
    }




    /**
     * 显示需要显示在界面上的信息
     * 该方法用于设置完参数后更新
     * view的显示
     */
    @SuppressLint("SetTextI18n")
    public void showText() {
        tv_tital.setText(Setup.VideoWidth + "x" + Setup.VideoHight);
        tv_pid.setText("SN:" + SettingUtils.getStrValues(this, "devNumber", "1234567"));
        tv_sn.setText("PID:" + SettingUtils.getStrValues(this, "policeNumber", "123456"));
    }

    @SuppressLint("SetTextI18n")
    public void showFreeSDSize() {

        //   Toast.makeText(MainActivity.this, ""+SDCardUtils.getSDCardAllSize(this), Toast.LENGTH_SHORT).show();
        //    Toast.makeText(MainActivity.this, ""+SDCardUtils.getSDCardFreeSize(this), Toast.LENGTH_SHORT).show();
        tv_memory.setText("Free：" + SDCardUtils.getSDCardFreeSize(this));
    }


//-----------------------------广播方式监听其它设置---------------------------------------------------

    /**
     * 动态注册广播
     */
    public void registerMessageReceiver() {
        mMessageReceiver = new MessageReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_INTENT_RECEIVER);
        registerReceiver(mMessageReceiver, filter);
    }

    public MessageReceiver mMessageReceiver;
    //可以定义多个action，监听不同需求
    public static String ACTION_INTENT_RECEIVER = "gbpe.policeass.receiver";
    //监听USB的action
    public static String ACTION_INTENT_USB = "gbpe.policeass.usb";

    //=================================================================================================================================================================
    public static int power;

    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ACTION_INTENT_RECEIVER)) {
            } else if (intent.getAction().equals(ACTION_INTENT_USB)) {
                //更改USB插入状态
            }
        }
    }

    /**
     * 隐藏的底部按钮
     */
    private RelativeLayout ll_bottom_button;
    private boolean isShowing;
    long[] mHits = new long[3];

    public void mClick(View view) {
        System.arraycopy(mHits, 1, mHits, 0, mHits.length - 1);
        mHits[mHits.length - 1] = SystemClock.uptimeMillis();  // 将离开机的时间设置给数组的第二个元素,离开机时间 :毫秒值,手机休眠不算
        if (mHits[0] >= (SystemClock.uptimeMillis() - 500)) {  // 判断是否多击操作
            if (!isShowing) {
                //ll_bottom_button.setVisibility(View.VISIBLE);
                AnimUtils.PopInAinm(MainActivity.this, ll_bottom_button);

            } else {
                // ll_bottom_button.setVisibility(View.GONE);
                AnimUtils.PopOutAinm(MainActivity.this, ll_bottom_button);
            }
            isShowing = !isShowing;
        }
    }


//    /** 0-100 */
    private int currentPower = 0;
    public static boolean isCharging = false;
    private boolean canAnimater;
    private ClipDrawable clipDrawableTest;
    private Handler handler = new Handler();

	private void onChargeProc(){
		// add by ycb, 20181115, turn on charge light
		//HardwareControler.setGPIOValue(98,1);
        DevControl.writeGuobaoGpio(98,1);
	
        handler.removeCallbacks(chargingRunnable);
        handler.post(chargingRunnable);

        iv_cd.setVisibility(View.VISIBLE);

      //  Toast.makeText(MainActivity.this, "power = " + power, Toast.LENGTH_SHORT).show();
        if (power == 100) {
            power = 0;
            Setup.IndicatorLight = false;
            //HardwareControler.SetChargingIndicatorState(0);
        } else {
            Setup.IndicatorLight = true;
            //HardwareControler.SetChargingIndicatorState(1);
        }
	}

	private void offChargeProc(){
		// add by ycb, 20181115, turn off charge light
		//HardwareControler.setGPIOValue(98,0);
        DevControl.writeGuobaoGpio(98,0);
	
        iv_cd.setVisibility(View.INVISIBLE);
        Setup.IndicatorLight = false;
        //HardwareControler.SetChargingIndicatorState(0);
	}

    // 电量变化广播
    // 假设进入Activity之前就已经在充电，进入之后无法收到充电的广播
    private BroadcastReceiver batteryChangedReceiver = new BroadcastReceiver() {

        public void onReceive(Context context, Intent intent) {
            if (Intent.ACTION_BATTERY_CHANGED.equals(intent.getAction())) {
                int level = intent.getIntExtra("level", 0);
                int scale = intent.getIntExtra("scale", 100);
                power = level * 100 / scale;
                currentPower = power;
                if (clipDrawableTest != null) {
                    clipDrawableTest.setLevel(calculateLevel2(power));
                }

                if (power == 100) {
                    //HardwareControler.SetChargingIndicatorState(0);
                } else {
                    // HardwareControler.SetChargingIndicatorState(1);
                }
            } else {
                int status = intent.getIntExtra("status", 0);
                if (Intent.ACTION_POWER_CONNECTED.equals(intent.getAction()))//连接电源
                {
                    isCharging = true;

                } else if (Intent.ACTION_POWER_DISCONNECTED.equals(intent.getAction())) //拔掉电源
                {
                    isCharging = false;
                }
             //   Toast.makeText(MainActivity.this, "充电：" + isCharging + "," + status, 1).show();
                if (isCharging) {
					onChargeProc();
                } else {
                    offChargeProc();
                }
            }
        }
    };


    /**
     * 根据自己的电池图标做响应的调整
     *
     * @param progress 0-100
     * @return 0-10000
     */
    private int calculateLevel2(int progress) {

        int bottomOffest = 1;
        int powerLength = 18;// 40 px in hdpi
        int totalLength = 21;// 49 px in hdpi
        int level = (bottomOffest + powerLength * progress / 100) * 10000 / totalLength;
        return level;
    }

    private  Runnable chargingRunnable = new Runnable() {

        @Override
        public void run() {

            AppLog.d("test", "isCharging:" + isCharging + ",canAnimater:" + canAnimater);
            if (!isCharging || !canAnimater) {
                return;
            }
            ValueAnimator animator = ValueAnimator.ofInt(100, currentPower, 100);
            animator.setDuration(1000);
            animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    int animatedValue = (Integer) animation.getAnimatedValue();
                    clipDrawableTest.setLevel(calculateLevel2(animatedValue));
                }
            });
            animator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    if (isCharging && canAnimater) {
                        handler.postDelayed(chargingRunnable, 400);
                    } else {
                        clipDrawableTest.setLevel(calculateLevel2(currentPower));
                    }
                }
            });
            animator.start();
        }
    };





    /**
     * 将surface替换成录音fragment
     */
    CameraSurfaceView cameraSurfaceView;

    public void setGLRenderer() {
        final ViewGroup container = ((ViewGroup) findViewById(R.id.fl_sv_container));
        if (cameraSurfaceView != null || record != null) {
            container.removeView(container.getChildAt(0));
            cameraSurfaceView = null;
        }
        cameraSurfaceView = new CameraSurfaceView(this);
        cameraSurfaceView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mClick(v);
            }
        });
        container.addView(cameraSurfaceView);
    }


    /**
     * 将surface替换成录音fragment
     */
    SoundRecordFragment record = null;

    public void setGLRenderer3() {

        final ViewGroup container = ((ViewGroup) findViewById(R.id.fl_sv_container));
        if (cameraSurfaceView != null && record == null) {
            container.removeView(cameraSurfaceView);
            cameraSurfaceView = null;
            record = new SoundRecordFragment();
            getFragmentManager()
                    .beginTransaction().addToBackStack("FragmentRecord")
                    .replace(R.id.fl_sv_container, record)
                    .commit();
			record.start(true);
        } else {
			record.start(false);
            getFragmentManager().popBackStack();
            record = null;
            setGLRenderer();
        }
    }

//=============================================以下为EventBus测试==============================================================================================
    @Override
    public void onEventMainThread(AnyEvent event) {
        super.onEventMainThread(event);
        showText();
    }

    @Override
    public void onVideoStartMainThread(VideoStartEvent event) {
        super.onVideoStartMainThread(event);
        //tid.start(Setup.videoRecording);
        AppLog.i (TAG, "onVideoStartMainThread: "+mRecordTick);
        if ((Setup.VideoPrerecord)&&(!Setup.isNormalRecording)){
			// 预录模式
			tid.start(false, mRecordTick /*, true*/);
		} else {
			tid.start(true, mRecordTick /*, true*/);
		}
		
        if (Setup.screenOn){
            keepScreenOn();//保持屏幕常亮
        }
         vibrator();

        if (Setup.isNormalRecording) {
            btnVideoRecorder.setSelected(true);
        } else {
            btnVideoRecorder.setSelected(false);
        }

		/* 录像时，开启红灯 */
        //HardwareControler.SetRedLightState(1);
        DevControl.writeGuobaoGpio(21,1);
        LogUtils.writeTxtToFile(getString(R.string.start_recorder),
                Setup.LOG_PATH_SDCARD_DIR, TimeUtils.getLogDate() + ".log");
        //button.setText(R.string.stop);
        //HardwareControler.SetGreenLightState(0);
        DevControl.writeGuobaoGpio(20,0);

    }

    @Override
    public void onVideoStopMainThread(VideoStopEvent event) {
        super.onVideoStopMainThread(event);

		if (tid != null){
        	tid.stop();
		}
		 
        cancleScreenOn();//取消屏幕常亮
        vibrator();
        btnVideoRecorder.setSelected(false);
        showFreeSDSize();
        //HardwareControler.SetRedLightState(0);
        DevControl.writeGuobaoGpio(21,0);
        LogUtils.writeTxtToFile(getString(R.string.stop_recorder),
                Setup.LOG_PATH_SDCARD_DIR, TimeUtils.getLogDate() + ".log");
        //button.setText(R.string.start);
        DevControl.writeGuobaoGpio(20,1);
        //HardwareControler.SetGreenLightState(1);
      //  Setup.isEmphasisFile = false;

    }

    @Override
    public void onAudioStartMainThread(AudioStartEvent event) {
        super.onAudioStartMainThread(event);
        vibrator();
        keepScreenOn();//保持屏幕常亮

        btnSoundRecorder.setSelected(true);
        DevControl.writeGuobaoGpio(20,0);
        //HardwareControler.SetGreenLightState(0);
        DevControl.writeGuobaoGpio(19,1);
        //HardwareControler.SetYellowLightState(1);
        LogUtils.writeTxtToFile(getString(R.string.start_audio),
                Setup.LOG_PATH_SDCARD_DIR, TimeUtils.getLogDate() + ".log");
      //  Setup.isEmphasisFile = false;
    }

    @Override
    public void onAudioStopMainThread(AudioStopEvent event) {
        super.onAudioStopMainThread(event);
        vibrator();
        cancleScreenOn();
        btnSoundRecorder.setSelected(false);
        //HardwareControler.SetGreenLightState(1);
        //HardwareControler.SetYellowLightState(0);
        DevControl.writeGuobaoGpio(20,1);
        DevControl.writeGuobaoGpio(19,0);
        LogUtils.writeTxtToFile(getString(R.string.stop_audio),
                Setup.LOG_PATH_SDCARD_DIR, TimeUtils.getLogDate() + ".log");
        if (!Setup.longPressVideo){
            Setup.isEmphasisFile = false;
            Setup.longPressVideo = false;
        }
        showFreeSDSize();
    }

    @Override
    public void onRemouteAudioStartMainThread(RemoteStartSoundRecord event) {
        super.onRemouteAudioStartMainThread(event);
        setGLRenderer3();

    }

    @Override
    public void onRemouteAudioStopMainThread(RemoteStopSoundRecord event) {
        super.onRemouteAudioStopMainThread(event);
        setGLRenderer3();
    }

    @Override
    public void onPhotographThread(PhotographEvent event) {
        super.onPhotographThread(event);
        takePictures();
        Setup.isEmphasisFile = false;
    }

    /**
     * 自动处理红外线,IRcut
     * @param automatic
     */
    @Override
    public void onIRcutAutomaticEvent(Automatic automatic) {
        super.onIRcutAutomaticEvent(automatic);
        AppLog.i(TAG," zhhs_onIRcutAutomaticEvent ");
        //IRCutUtils.IRCutMotor();
        /* 此处关闭自动IRcut, 不能解决一定记录IRcut不切换问题 */
        mIrCutUtils.IRCutMotor();
    }

    @Override
    public void onStartCoundDown(OnStartCountEvent event) {
        super.onStartCoundDown(event);

		// delete 20180820
        cdtShow.setCountdownInterval(1000);
        cdtShow.setMillisInFuture(Setup.VideoDelay);
        cdtShow.start();
    }

    @Override
    public void onStopCoundDown(OnStopCountEvent event) {
        super.onStopCoundDown(event);

		// delete 20180820
        cdtShow.cancel();
    }

    /**
     *
     * 当有其他界面呼叫的时候
     * @param event
     */
    @Override
    public void onReleaseCamera(OnReleaseCamera event) {

        super.onReleaseCamera(event);

        //if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||  Setup.videoRecording) {
        if (Setup.VideoPrerecord || Setup.videoRecording) {
            Setup.currPage = false;
            VideoManager6.stopAll();
            Setup.videoRecording = false;
            Setup.isNormalRecording = false;
            //FileUtils.deleteFolder(Setup.pre_file_dir);
            VideoManager6.clearPreFile();
        }
        if (isAudioRecording) {//如果正在录音，则停掉录音并保存
            setGLRenderer3();
            isAudioRecording = false;
        }

        //tid.setInVisible();
        //tid.rempveTimeRun();
        stopCameraPreview();

    }

    @Override
    public void onCombineVideo(OnCombineVideo event) {
        super.onCombineVideo(event);

        //   Toast.makeText(MainActivity.this, "合并视频是", Toast.LENGTH_SHORT).show();
    }

    private void setGpsIndicator(boolean on) {
        gps_available = on;
        if (on) {
            gps_indicator.setVisibility(View.VISIBLE);
        } else {
            gps_indicator.setVisibility(View.INVISIBLE);
        }
    }

    @Override
    public void onGetLocation(OnGetLocationEvent event) {
        super.onGetLocation(event);
		AppLog.i (TAG, "onGetLocation");

		if (event.gpsState()) {   // gps on

            Location location = GpsUtils.start(MainActivity.this, new LocationListener() {
                //当坐标改变时触发此函数，如果Provider传进相同的坐标，它就不会被触发
                @Override
                public void onLocationChanged(Location location) {
                    VideoSet.longitude = location.getLongitude();
                    VideoSet.latitude = location.getLatitude();
                    setGpsIndicator(true);
                }

                // Provider的在可用、暂时不可用和无服务三个状态直接切换时触发此函数
                @Override
                public void onStatusChanged(String provider, int status, Bundle extras) {
                    AppLog.i(TAG, "Gps status changed");
                    switch (status) {
                        case LocationProvider.AVAILABLE:
                            setGpsIndicator(true);
                            break;

                        case LocationProvider.OUT_OF_SERVICE:
                        case LocationProvider.TEMPORARILY_UNAVAILABLE:
                            setGpsIndicator(false);
                            break;
                    }
                }

                //  Providerè¢«enableæ—¶è§¦å‘æ­¤å‡½æ•°ï¼Œæ¯”å¦‚GPSè¢«æ‰“å¼?
                @Override
                public void onProviderEnabled(String provider) {
                    AppLog.i(TAG, "GPS onProviderEnabled");
                }

                // Provider被disable时触发此函数，比如GPS被关闭
                @Override
                public void onProviderDisabled(String provider) {
                    AppLog.i(TAG, "GPS onProviderDisabled");
                    setGpsIndicator(false);
                }
            });
            if (location != null) {
                setGpsIndicator(true);
                VideoSet.longitude = location.getLongitude();
                VideoSet.latitude = location.getLatitude();
                AppLog.i(TAG, "loc: " + VideoSet.longitude + "," + VideoSet.latitude);
            } else {
                setGpsIndicator(false);
            }
        } else {
		    GpsUtils.stop();
        }

    }


    //====================================================================================================================================================

    //boolean Record = false;
    //Button button;

    /**
     * 录制视频方法。
     * 摄像头异常，不能拍摄视频
     * @param view
     */
    /*
    public void startAndStop2(View view) {
            vibrator();

        if (!CameraHandlerThread.isCameraOpened(this)){
            Toast.makeText(MainActivity.this, "摄像头异常11", Toast.LENGTH_SHORT).show();
            return;
        }
        Record = !Record;
        if (Record) {
            VideoManager6.startRecord();
        } else {
            VideoManager6.stopRecord();
        }
    }*/

    /**
     * 获取当前的运营商
     *
     * @param context
     * @return 运营商名字
     */
    public static String getOperator(Context context) {


        String ProvidersName = "";
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        String IMSI="";
        try{
            IMSI = telephonyManager.getSubscriberId();
            AppLog.i("qweqwes", "运营商代码" + IMSI);
        }catch (SecurityException e){
            e.printStackTrace();
        }

        if (IMSI != null) {
            if (IMSI.startsWith("46000") || IMSI.startsWith("46002") || IMSI.startsWith("46007")) {
                ProvidersName = "中国移动";
            } else if (IMSI.startsWith("46001")  || IMSI.startsWith("46006")) {
                ProvidersName = "中国联通";
            } else if (IMSI.startsWith("46003")) {
                ProvidersName = "中国电信";
            }
            return ProvidersName;
        } else {
            return "没有获取到sim卡信息";
        }
    }

    /**
     * 得到当前的手机蜂窝网络信号强度
     * 获取LTE网络和3G/2G网络的信号强度的方式有一点不同，
     * LTE网络强度是通过解析字符串获取的，
     * 3G/2G网络信号强度是通过API接口函数完成的。
     * asu 与 dbm 之间的换算关系是 dbm=-113 + 2*asu
     */
    public void getCurrentNetDBM(final Context context) {

        final TelephonyManager tm = (TelephonyManager) context
                .getSystemService(Context.TELEPHONY_SERVICE);
        PhoneStateListener mylistener = new PhoneStateListener(){
            //            @Override
            public void onSignalStrengthsChanged(SignalStrength signalStrength) {
                super.onSignalStrengthsChanged(signalStrength);
                String signalInfo = signalStrength.toString();
                String[] params = signalInfo.split(" ");

				/* 此处通过发射获取信号dbm值和level */
                try {
                    Method method1 = null;
                    method1 = signalStrength.getClass().getMethod("getDbm");
                    int dmb = (int)method1.invoke(signalStrength);

                    Method method2 = signalStrength.getClass().getMethod("getLteLevel");
                    int level = (int)method2.invoke(signalStrength);

                    //AppLog.i(TAG,"signal strength is "+dmb+", level is "+level);

					iv_memery.setSignalStrength(level);
                }catch (Exception e)
                {
                    AppLog.e(TAG,"获取信号强度失败");
                    e.printStackTrace();
                }

                /*
                if(tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_LTE){
                    //4G网络 最佳范围   >-90dBm 越大越好
                    int Itedbm = Integer.parseInt(params[9]);
                    setDBM(Itedbm+"");

                }else if(tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSDPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSUPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_UMTS){
                    //3G网络最佳范围  >-90dBm  越大越好  ps:中国移动3G获取不到  返回的无效dbm值是正数（85dbm）
                    //在这个范围的已经确定是3G，但不同运营商的3G有不同的获取方法，故在此需做判断 判断运营商与网络类型的工具类在最下方
                    //String yys = IntenetUtil.getYYS(getApplication());//获取当前运营商
                    String yys = getOperator(context);
                    if (yys=="中国移动") {
                        setDBM(0+"");//中国移动3G不可获取，故在此返回0
                    }else if (yys=="中国联通") {
                        int cdmaDbm = signalStrength.getCdmaDbm();
                        setDBM(cdmaDbm+"");
                    }else if (yys=="中国电信") {
                        int evdoDbm = signalStrength.getEvdoDbm();
                        setDBM(evdoDbm+"");
                    }

                }else{
                    //2G网络最佳范围>-90dBm 越大越好
                    int asu = signalStrength.getGsmSignalStrength();
                    int dbm = -113 + 2*asu;
                    setDBM(dbm+"");
                }*/

            }
        };
        //开始监听
        tm.listen(mylistener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
    }

    private  void calculateOrientation() {
        float[] values = new float[3];
        float[] R = new float[9];
        SensorManager.getRotationMatrix(R, null, accelerometerValues, magneticFieldValues);
        SensorManager.getOrientation(R, values);

        // 要经过一次数据格式的转换，转换为度
        values[0] = (float) Math.toDegrees(values[0]);
//        AppLog.i(TAG, values[0]+"");
        //values[1] = (float) Math.toDegrees(values[1]);
        //values[2] = (float) Math.toDegrees(values[2]);
/*
        if(values[0] >= -5 && values[0] < 5){
            AppLog.i(TAG, "正北");
        }
        else if(values[0] >= 5 && values[0] < 85){
            AppLog.i(TAG, "东北");
        }
        else if(values[0] >= 85 && values[0] <=95){
            AppLog.i(TAG, "正东");
        }
        else if(values[0] >= 95 && values[0] <175){
            AppLog.i(TAG, "东南");
        }
        else if((values[0] >= 175 && values[0] <= 180) || (values[0]) >= -180 && values[0] < -175){
            AppLog.i(TAG, "正南");
        }
        else if(values[0] >= -175 && values[0] <-95){
            AppLog.i(TAG, "西南");
        }
        else if(values[0] >= -95 && values[0] < -85){
            AppLog.i(TAG, "正西");
        }
        else if(values[0] >= -85 && values[0] <-5){
            AppLog.i(TAG, "西北");
        }*/
    }


    final SensorEventListener myListener = new SensorEventListener()
    {
        public void onSensorChanged(SensorEvent sensorEvent) {

            if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
                magneticFieldValues = sensorEvent.values;
            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
                accelerometerValues = sensorEvent.values;
            calculateOrientation();
        }
        public void onAccuracyChanged(Sensor sensor, int accuracy)
        {

        }
    };


    // add by ycb for Orientation
	private void OrientationTest()
    {
    	
		//SensorManager compassSensorManager;
		//需要两个Sensor
		Sensor aSensor;
		Sensor mSensor;
		
		AppLog.i(TAG, "get Location");


        compassSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
		/*加速度传感器*/
		aSensor = compassSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		/*磁场传感器，hm1暂时没有检测到*/
		mSensor = compassSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

        compassSensorManager.registerListener(myListener, aSensor, SensorManager.SENSOR_DELAY_NORMAL);
        compassSensorManager.registerListener(myListener, mSensor,SensorManager.SENSOR_DELAY_NORMAL);

        calculateOrientation();
	}

	private void resetScreen(){
	    //Toast.makeText(this,"reset screen",Toast.LENGTH_SHORT).show();
		AppLog.i(TAG,"resetScreen");
		new Thread() {
			@Override
			public void run() {
			    int cnt = 0;
			    try {
			        while (isAdminDevice == false){
			            sleep(1000);
			            if (cnt ++ > 10){
			                AppLog.i(TAG,"获取设备管理员权限失败");
			                return;
                        }
                    }
                    sleep(3000);
                    AppLog.i(TAG,"screen off");
                    setScreenOff(null);
                    sleep(500);
                    AppLog.i(TAG, "screen on");
                    setScreenOn(null);
                    //sleep(1000);

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                /*try {
                    sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                AppLog.i(TAG,"screen off");
				//ShellUtils.execRootCmd("input keyevent 26");
                //String strRst = ShellUtils.execAdbCmd("ls");
                //AppLog.i(TAG,"result: "+strRst);
                //keepScreenOn(context,true);
                setScreenOff(null);
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                AppLog.i(TAG, "screen on");
                setScreenOn(null);
                //ShellUtils.execRootCmd("input keyevent 26");
               // ShellUtils.execAdbCmd("input keyevent 26");
                //keepScreenOn(context,false);*/
			}
	
		}.start();

	}

    private void isOpen() {
        if (policyManager.isAdminActive(adminReceiver)) {//判断超级管理员是否激活
            AppLog.i(TAG,"设备已被激活");

        } else {
            AppLog.i(TAG,"设备没有被激活");

        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        //isOpen();
        if (policyManager.isAdminActive(adminReceiver)){
            //resetScreen();
            isAdminDevice = true;
            //setScreenOff(null);
		}else {
			//Toast.makeText(this,R.string.admin_active_error,Toast.LENGTH_SHORT).show();
			isAdminDevice = false;
		}
    }
/*
    private static PowerManager.WakeLock wakeLock;

    public static void keepScreenOn(Context context, boolean on) {
        if (on) {
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            wakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE,TAG+":wakelock");
            wakeLock.acquire();
        } else {
            if (wakeLock != null) {
                wakeLock.release();
                wakeLock = null;
            }
        }
    }*/

    /**
     * @param view 检测并去激活设备管理器权限
     */
    public void checkAndTurnOnDeviceManager(View view) {
        Intent intent = new Intent(DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN);
        intent.putExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, adminReceiver);
        //intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION, "开启后就可以使用锁屏功能了...");//显示位置见图二
        intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION, R.string.open_device_manager_tips);
        startActivityForResult(intent, 0);

    }

    /**
     * @param view 亮屏
     *
     * */
    public void setScreenOn(View view) {
        mWakeLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "ycb:tag");
        mWakeLock.acquire();
        mWakeLock.release();
    }

    /**
     * @param view 熄屏
     */
    public void setScreenOff(View view) {
        boolean admin = policyManager.isAdminActive(adminReceiver);
        if (admin) {
            policyManager.lockNow();
        } else {
            //Toast.makeText(this,R.string.no_admin_privilege,Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * @param view 熄屏并延时亮屏
     */
    public void setScreenOffAndDelayOn(View view) {
        boolean admin = policyManager.isAdminActive(adminReceiver);
        if (admin) {
            policyManager.lockNow();
            //handler.sendEmptyMessageDelayed(1, 3000);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    setScreenOn(null);
                }
            },3000);
        } else {
            Toast.makeText(this,R.string.no_admin_privilege,Toast.LENGTH_SHORT).show();
        }
    }

	private void shutDown(){
            Intent intent = new Intent("android.intent.action.ACTION_REQUEST_SHUTDOWN");
            intent.putExtra("android.intent.extra.KEY_CONFIRM", false);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
     }

     private void shutdownAdb() {
        try {
            Settings.Global.putInt(getContentResolver(),Settings.Global.ADB_ENABLED,0);
        } catch (Exception e) {
            AppLog.e (TAG, "adb set error: "+e.toString());
        }
     }
/*
    @Override
	public void adbSet(boolean on) {
		try {
            if (on) {
                Settings.Global.putInt(getContentResolver(),Settings.Global.ADB_ENABLED,1);
                //Toast.makeText(this,"adb on", Toast.LENGTH_SHORT).show();
            } else {
                Settings.Global.putInt(getContentResolver(),Settings.Global.ADB_ENABLED,0);
                //Toast.makeText(this,"adb off", Toast.LENGTH_SHORT).show();
            }
		} catch (Exception e) {
		    AppLog.e (TAG, "adb set error: "+e.toString());
        }
	}

	@Override
     public boolean getAdbConfig() {
        String adb_en = Setup.getProperties("ADB");
        return (adb_en==null)?false:adb_en.equals("enable");
     }
*/
    private void setApn() {
/*
        int apnid = -1;

        APN apn = new APN(this);
        apn.checkAPN();
        if (!APN.hasAPN) {
            apn.SetAPN(apn.addAPN());
        }

        arcVPDN aVPDN = new arcVPDN(this);

        AppLog.d(TAG, aVPDN.DefaultAPNName());
        AppLog.d(TAG, aVPDN.GetApn("ä¸­å›½ç”µä¿¡äº’è”ç½‘è®¾ç½®CTNET")+"");


        if(!aVPDN.DefaultAPNName().equals("IGRPVPDN"))
        {
            if(!aVPDN.Exisit("ç½‘ç»œåç§°")){
                //è°ƒç”¨vpdnè®¾ç½®ç•Œé¢
                startActivityForResult(new Intent(android.provider.Settings.ACTION_APN_SETTINGS), 0);
            }
            apnid= aVPDN.GetApn("ç½‘ç»œåç§°");
            aVPDN.SetDefault(apnid);

        }*/
    }
    private void setIME(){

        Locale curLocale = getResources().getConfiguration().locale;

        if (curLocale.equals(Locale.CHINESE)||curLocale.equals(Locale.SIMPLIFIED_CHINESE)) {
            AppLog.i (TAG, "CHINESE set input");
            try {
                Settings.Secure.putString(getContentResolver(), Settings.Secure.DEFAULT_INPUT_METHOD,"com.iflytek.inputmethod/.FlyIME");
            } catch (Exception e) {
                AppLog.e(TAG,"setIME error:"+e.getMessage());
            }
        }

    }
}


