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

// ????????????
// 'standard output': Resource temporarily unavailable

/*
*  activity???android:screenOrientation
* unspecified????????????????????????????????????????????????????????????
* landscape?????????????????????
* portrait??????????????????
* behind???????????????activity????????????
* sensor?????????????????????????????????????????????90??????180??????270????????????????????????activity???????????????
* sensorLandscape???????????????????????????????????????????????????
* sensorPortrait???????????????
* nosensor???????????????????????????????????????????????????????????????????????????????????????
* user??????????????????????????????

*/

/**
 * =====================================================================================
 *
 * ??????????????????????????????????????????????????????????????????(c)2017
 *
 * ?????????Administrator on 2016/11/10 19:37
 *
 * ?????????xjs250@163.com
 *
 * ???????????????2016/11/10 19:37
 *
 * ?????????1.??????????????????????????????a.????????????(????????????)b.?????????????????????????????????????????????????????????
         2.???????????????a.??????????????????(???????????????????????????????????????????????????)???b.????????????????????????
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
    //???????????????
    private Chronometer chronometer;
    // ???????????????SurfaceView
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

	// delete 20180820, ????????????????????????
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
     * ??????handler???????????????
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
     * ??????????????????
     */
    // modified by ycb, the variable place in CameraHandlerThread
    //private boolean isRecording;//?????????
    private boolean isAudioRecording;//??????

    /**
     * ??????????????????
     *
     * @param source
     */
    @Override
    public void widgetClick(View source) {

        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
        switch (source.getId()) {
            case R.id.iv_video_record: {//??????

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
            case R.id.iv_take_picture: {//??????
                /*if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, "???????????????6", Toast.LENGTH_SHORT).show();
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
            case R.id.iv_audio_reocrd: {//??????

                if (SettingUtils.getBoolenValues(this, "key_tone")) {//?????????????????????
                    keySound.play(0, 0);
                }
                if (!isAudioRecording)
                {
                    btnSoundRecorder.setSelected(true);
                    //if (isRecording) {//???????????????????????????????????????
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

                if ( Setup.videoRecording ) {//??????????????????????????????
                    //videoRecord();//??????????????????
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
            case R.id.tv_menue: {//????????????

                 /*if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||  Setup.videoRecording)*/
			    if (Setup.VideoPrerecord ||  Setup.videoRecording) {
                     Setup.currPage = false;
                     VideoManager6.stopAll();
                     Setup.videoRecording = false;
                     Setup.isNormalRecording = false;
					 /* ??????????????????*/
                     //FileUtils.deleteFolder(Setup.pre_file_dir);
                     VideoManager6.clearPreFile();
                }
                if (isAudioRecording) {//?????????????????????????????????????????????
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
     * ?????????????????????????????????
     *
     * @param v
     * @return
     */
    @Override
    public boolean onLongClick(View v) {

        //Setup.isEmphasisFile = true;
        SDCardUtils.judgeSDCardFreeSizeIsPlenty(this);
        switch (v.getId()) {
            case R.id.iv_video_record: {//??????

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
            case R.id.iv_take_picture: {//??????
                /*if (CameraHandlerThread.camera == null) {
                    Toast.makeText(MainActivity.this, "???????????????8", Toast.LENGTH_SHORT).show();
                    return true;
                }
                if (SettingUtils.getBoolenValues(this, "key_tone")) {
                    keySound.play(1, 0);
                }*/
                takePictures();
                break;
            }
            case R.id.iv_audio_reocrd: {//??????

                if (SettingUtils.getBoolenValues(this, "key_tone")) {//?????????????????????
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
                    //videoRecord();//??????????????????
                    stopVideoRecord();
                    handler.postDelayed(new Runnable() {//??????1.5s??????????????????????????????surfaceview????????????????????????????????????????????????
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
        /*irState false ??????????????????*/
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
                    //stopVideoRecord(); //????????????
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
                    startVideoRecord(mVideoName+".tmp"+mIrcutCnt); //??????????????????
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
				//tag by ycb: ?????????????????????/??????????????????ir cut?????????ir cut???????????????????????????
				//????????????????????????????????????IR CUT????????????????????????
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
					// ????????????500ms ?????????????????????????????????IRCUT???????????????
					sleep(200);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				IRCutUtils.IRcutAction(irState); */
            }
        }.start();

        //IRCutUtils.IRcutAction(irState);
		//AppLog.i(TAG,"IR cut exit");

		// ?????????????????????????????????????????????why???
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
        //msg.obj = "????????????";//???????????????????????????????????????????????????List???map??????
        //msg.what = 0;
        //msg.arg1 = isRcd? 1:0;
        //msg.arg2 = irState;
        //mIrHandler.sendMessage(msg);

    }

    /**
     * ???????????????????????????
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
        }  else if (keyCode == KeyEvent.KEYCODE_2)//???????????????
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
        	// ?????????????????? IR_CUT, 20180830
        	AppLog.i(TAG,"??????????????????");
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
            if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
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

                videoRecord();//??????????????????
                handler.postDelayed(
                        new Runnable() {//??????1.5s??????????????????????????????surfaceview????????????????????????????????????????????????
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
        } else if (keyCode == KeyEvent.KEYCODE_5)//?????????????????????KEYCODE_5
        {
        	// ???????????????????????????????????????
        	// ycb 20180830 ??????
            //  Toast.makeText(MainActivity.this, "Long==>KEYCODE_5", Toast.LENGTH_SHORT).show();

			

            if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
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
        } else if (keyCode == KeyEvent.KEYCODE_6)//?????????????????????
        {
         //   Toast.makeText(MainActivity.this, "Long==>KEYCODE_6", Toast.LENGTH_SHORT).show();
            if (isAudioRecording) {
                Toast.makeText(MainActivity.this, R.string.no_photo_tips, Toast.LENGTH_SHORT).show();
                return true;
            }
            if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
            {
                keySound.play(1, 0);
            }

            takePictures();
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_7)     // ???????????????
        {
        	/* 20180830??????????????????????????????????????? */
            /** ???4G APP?????????????????????SOS?????? **/
            Intent intent = new Intent("gbpe.keys.sos");
            sendBroadcast(intent);
            AppLog.i(TAG,"?????? 4G SOS ??????");
/*
        	AppLog.i(TAG,"??????????????????");
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
     * ????????????????????????????????????
     * ?????????????????????????????????
     * ?????????????????????????????????
     * ?????????onKeyDown?????????
     * ????????????event.startTracking()
     * ?????????????????????????????????
     * ?????????????????????
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
     * ?????????????????????????????????
     *
     * @param keyCode
     * @param event   ?????????2 5 7 ?????? ???4 6 3
     */
    /* ycb ??????????????? ????????????????????????????????????????????????????????? 20180830 */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

		AppLog.i(TAG,"get key up "+keyCode);

        //??????????????????????????????????????????false
       // Setup.isEmphasisFile = false;
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
        	// ycb 20180830???????????????????????????????????????????????????
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
//----------------------------------?????????------------------------------------------------------------------------//
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
//--------------------------------------??????------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_3) {
            if (shortPress) {
             //   Toast.makeText(MainActivity.this, "==>KEYCODE_3", Toast.LENGTH_SHORT).show();

			 	if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return true;
				}

                if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
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
                //videoRecord();//??????????????????
                stopVideoRecord();
                handler.postDelayed(new Runnable() {//??????1.5s??????????????????????????????surfaceview????????????????????????????????????????????????
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

                  //  Toast.makeText(MainActivity.this, "??????????????????", Toast.LENGTH_SHORT).show();
                    //FileUtils.deleteFolder(Setup.pre_file_dir);
                    VideoManager6.clearPreFile();
                }
                if (isAudioRecording) {//?????????????????????????????????????????????
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
//--------------------------------------??????------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_5) {
            if (shortPress) {
             //   Toast.makeText(MainActivity.this, "==>KEYCODE_5", Toast.LENGTH_SHORT).show();
             	if (!SDCardUtils.isSDCardEnable())
				{
					Toast.makeText(MainActivity.this, R.string.no_sdcard_tips, Toast.LENGTH_SHORT).show();
					return true;
				}
                if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
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
//------------------------------------ ??????------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_6) {
            if (shortPress) {
              //  Toast.makeText(MainActivity.this, "==>KEYCODE_6", Toast.LENGTH_SHORT).show();
                /*if (isAudioRecording) {
                    Toast.makeText(MainActivity.this, "????????????", Toast.LENGTH_SHORT).show();
                    return true;
                }*/
                /*if (SettingUtils.getBoolenValues(this, "key_tone"))//?????????????????????
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
//----------------------------------????????????------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_7)//??????????????????SOS???????????????/?????????????????????SOS??????????????????????????????????????????????????????????????????
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
        } else if (keyCode == KeyEvent.KEYCODE_HOME)//??????????????????SOS???????????????/?????????????????????SOS??????????????????????????????????????????????????????????????????
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
                             if (isAudioRecording) {//?????????????????????????????????????????????
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
     * ???????????????????????????????????????????????????
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

        //??????activity???view??????
        showText();
        checkSim();

		// ????????????????????????
		// ????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		if (!CameraHandlerThread.startRecordingFlag) {
	        if (Setup.VideoPrerecord){
				if (!SDCardUtils.isSDCardEnable()){
					Toast.makeText(this,R.string.no_sdcard_tips,Toast.LENGTH_SHORT).show();
				}else {
	                new Handler().postDelayed(new Runnable() {
	                    @Override
	                    public void run() {
	                    	AppLog.i (TAG, "????????????");
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

		// ??????GPS
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
		if (!CameraHandlerThread.startRecordingFlag) {                  // ??????????????????????????????????????????
            if (CameraHandlerThread.isCameraOpened(context,false)){
                CameraHandlerThread.getInstance().doStopCamera();
            }
            // ??????GPS
            EventBus.getDefault().post(new OnGetLocationEvent(OnGetLocationEvent.GPS_STOP));
		}

		PollService.setServiceAlarm(this,false);
		//AppLog.d (TAG, "onPause OK");

		
     }


    /**
     * ????????????
     */
     /*
    //public void videoRecord() {
    public void videoRecord(final String path) {
        //??????????????????????????????????????????
		AppLog.i(TAG, "videoRecord enter");
		
        if (CameraHandlerThread.camera == null) {
            Toast.makeText(MainActivity.this, "???????????????9", Toast.LENGTH_LONG).show();
            return;
        }

		AppLog.i(TAG, "Setup.videoRecording: "+Setup.videoRecording);
        if (! Setup.videoRecording) {
            //???????????????????????????????????????
            if (Setup.VideoPrerecord) {
                VideoManager6.stopRecord();
            }

            if (isAudioRecording)//????????????????????????????????????????????????
            {
                //?????????????????????
                setGLRenderer3();
                isAudioRecording = false;
                handler.postDelayed(new Runnable() {//??????1.5s??????????????????????????????surfaceview????????????????????????????????????????????????
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
         //   Toast.makeText(MainActivity.this, "??????????????????", Toast.LENGTH_SHORT).show();

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
                        AppLog.i(TAG,"??????????????????");
                    }

                    @Override
                    public void onOneFileHandleFinished(int progress) {
                        AppLog.i(TAG,"??????????????????");
                    }

                    @Override
                    public void onAllFileHandleFinished() {

                    }

                    @Override
                    public void onFileCancle() {

                    }
                });
				
			}
			
         //   Toast.makeText(MainActivity.this, "??????????????????", Toast.LENGTH_SHORT).show();

        }

        //???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
        if (!Setup.isVideoDelay){
            if (!Setup.isNormalRecording){//??????
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
			mediaRecorder = new MediaRecorder();// ??????mediarecorder??????
			//???????????????90???
			camera = Camera.open(Camera.CameraInfo.CAMERA_FACING_FRONT);
			camera.setDisplayOrientation(90);
			camera.unlock();
            mediaRecorder.setCamera(camera);
            // ????????????????????????Camera(??????)
			mediaRecorder.setVideoSource(Camera.CameraInfo.CAMERA_FACING_FRONT);
			//????????????90???
			mediaRecorder.setOrientationHint(90);
			// ??????????????????????????????????????????THREE_GPP???3gp.MPEG_4???mp4
			mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
			// ???????????????????????????h263 h264
			mediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
			//?????????????????????,????????????
			mediaRecorder.setVideoEncodingBitRate(5*1024*1024);
			// ??????????????????????????????????????????????????????????????????????????????????????????
			mediaRecorder.setVideoSize(1920, 1080);
			// ???????????????????????????????????????????????????????????????????????????????????????
			mediaRecorder.setVideoFrameRate(20);
			// ?????????????????????????????????
			mediaRecorder.setOutputFile("/sdcard/video.mp4");
			// ???????????????????????????????????????
			//mediaRecorder.setPreviewDisplay(mSurfaceView.getHolder().getSurface());
            mediaRecorder.setPreviewDisplay(cameraSurfaceView.getSurfaceHold().getSurface());
 
			mediaRecorder.setOnErrorListener(new MediaRecorder.OnErrorListener() {
 
				@Override
				public void onError(MediaRecorder mr, int what, int extra) {
					// ???????????????????????????
					mediaRecorder.stop();
					mediaRecorder.release();
					mediaRecorder = null;
					//isRecording = false;

				}
			});
 
			// ???????????????
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
		//??????????????????????????????????????????

		AppLog.i(TAG, "videoRecord enter");
		
        if (CameraHandlerThread.camera == null) {
            Toast.makeText(MainActivity.this, R.string.camera_exception, Toast.LENGTH_LONG).show();
            return null;
        }

		//tid.start(true);
        //tid.start(true);
        //tid.start(false);

		// Setup.videoRecording ??????????????????????????????
		if (! Setup.videoRecording) {
            //???????????????????????????????????????
            /*if (Setup.VideoPrerecord) {
                VideoManager6.stopRecord(false);
            }*/
            if (Setup.VideoPrerecord){
				// ??????returen
				AppLog.d(TAG, "??????????????????????????????"+VideoManager6.getRcdFilePath());
				Setup.videoRecording = true;
				VideoManager6.interruptPreRcd();
				//tid.stop();
				//tid.start(true,0);
				int stk = TimeIndicator.getTimeTick();
				if (stk>10) {
					stk = 11;
				}
				tid.start(true,stk /*,false*/);
				// ???????????????
				Setup.isNormalRecording = true;
				VideoManager6.segmentdVideo();
				return  VideoManager6.getRcdFilePath();
			}

            if (isAudioRecording)//????????????????????????????????????????????????
            {
                //?????????????????????
                setGLRenderer3();
                isAudioRecording = false;
                handler.postDelayed(new Runnable() {//??????1.5s??????????????????????????????surfaceview????????????????????????????????????????????????
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
         //   Toast.makeText(MainActivity.this, "??????????????????", Toast.LENGTH_SHORT).show();

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
                    AppLog.i(TAG,"??????????????????");
                }

                @Override
                public void onOneFileHandleFinished(int progress) {
                    AppLog.i(TAG,"??????????????????");
                }

                @Override
                public void onAllFileHandleFinished() {

                }

                @Override
                public void onFileCancle() {

                }
            });
			
		}

		// ?????????????????????????????????
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
                                // ???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
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
                        // ??????????????????????????????????????????????????????
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

				// ????????????
				
			}else{
				// ???????????????????????????????????????????????????????????????????????????????????????????????????????????????
				// ????????????????????????????????????????????????????????????
				AppLog.i(TAG,"????????????");
			}

			VideoManager6.previewRecord();
			
		}

	}
	
    /**
     * ??????
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
     * ???????????????????????????????????????
     * ??????????????????????????????????????????
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
        // ???????????????
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        window = getWindow();
        //????????????
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        // ???????????????????????????,??????surfaceview???activity????????????
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

		/*???????????????????????????*/
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
        // ?????????pl?????????????????????????????????????????????????????????
        //resetScreen();
		shutdownAdb();
		//adbSet(false);       // ????????????adb
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

				/* ?????????????????? */
                if (action.equals(Intent.ACTION_SCREEN_OFF))
                {
                    AppLog.i(TAG,"screen off");
					setScreenState(false);

					//if (false == Setup.videoRecording) // ?????????????????????????????????
					// ????????????????????????????????????????????????????????????
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
					
                } /*??????????????????*/
				else if(action.equals(Intent.ACTION_SCREEN_ON)){
                    AppLog.i(TAG,"screen on");
					setScreenState(true);
					/* 20180910 ???????????????????????? */
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
					int current=intent.getExtras().getInt("level");//??????????????????
        			int total=intent.getExtras().getInt("scale");//???????????????
					int percent = current*100/total;

					//AppLog.i(TAG, "battery percentage: "+percent);					
					tv_battery_level.setText(percent + "%");
                	Setup.battery_level = percent + "%";

					// ybc 20180906: ???????????????????????????2%??????????????????
                    if (percent < 2) {
                        if (getBatteryVol() < 3450) {
                            //if (isRecording)
                            //if (CameraHandlerThread.isRecordingGlobal)
                            //if (CameraHandlerThread.startRecordingFlag) {
                            if (Setup.videoRecording) {
                                //videoRecord(); //?????????????????????????
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
                        //CameraHandlerThread.isRecordingGlobal = false; // 20181210 ?????????????????????????????????????????????????????????????????????????????????????????????????
                    }

                }
            }
        };

        ImageView imageBatteryTest = (ImageView) findViewById(R.id.iv_main);
        LayerDrawable layerDrawableTest = (LayerDrawable) imageBatteryTest.getDrawable();
        clipDrawableTest = (ClipDrawable) layerDrawableTest
                .findDrawableByLayerId(R.id.clip_drawable);

        //???????????????
        dbHelper = new DBHelper(this);
        DBHelper.quary(dbHelper);

        showText();
        showFreeSDSize();
        checkSim();//??????SIM???????????????

		getCurrentNetDBM(this);
        setTextClock();////?????????????????????????????????
        // registerBatteryRec();//??????????????????????????????

		// add by ycb, ??????????????????
		registerScreenActionRecv();
		// for compass test
		OrientationTest();
		
        vibrator();//??????????????????
//        //??????????????????
		mPreRecordHandler = new Handler();

/*      ???????????????????????????????????????????????????????????????
        util.textToSpeech(R.string.police_number + SettingUtils.
                getStrValues(this, "policeNumber", "111111") + R.string.power_on);
*/
        //??????service,??????APP?????????????????????
        //Intent intent = new Intent();
        //intent.setClass(this, MyService.class);
        //this.startService(intent);

		// 20181009 ???????????? ir cut ??????
		IRCutUtils.IRcutAction(IRCutUtils.getIRstate());

        if(!PreferenceManager.getDefaultSharedPreferences(this).getBoolean(IR_CUT_INIT,false)) {
			// ???????????????
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
						//EventBus.getDefault().post(new OnGetLocationEvent("???????????????"));
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



        // ???????????????context??????????????????
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

		/* ??????temp????????? */
        //handleUtils = new VideoHandleUtils(true);
        //PollService.setServiceAlarm(this,true);
     }

     private void checkInfrared(final boolean isMove){
     	 /*????????????ir cut ???????????????????????????*/
         final String infrared = PreferenceManager.getDefaultSharedPreferences(this)
                 .getString(SettingFragment.INFRARED,"");
         AppLog.i(TAG,"infrared: "+infrared);

         new Thread(){
             @Override
             public void run(){
                 try{
                     sleep(2000);
          
					 /* ????????????ir cut?????????????????????????????????ir cut Event bus*/
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
     * ??????SIM???????????????
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


    //??????static??????????????????????????????,??????????????????
    public static void stopCameraPreview() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                if (CameraHandlerThread.camera != null) {
                    CameraHandlerThread.camera.stopPreview();   //????????????
                    CameraHandlerThread.camera.release();       //??????Camera??????
                    CameraHandlerThread.camera = null;
                }
            }
        }).start();
    }


    /**
     * ??????????????????????????????
     */
    private void registerBatteryRec() {
        IntentFilter filter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(pow_action_receiver, filter);//??????BroadcastReceiver

    }

    /**
     * ??????????????????????????????
     */
    private void unRegistgerBatterRec() {
		// delete by ycb for test
        //unregisterReceiver(receiver);
    }

	/*
	*  ??????SCREEN_OFF??????
	*  add by ycb
	*/
	private void registerScreenActionRecv()
    {
    	AppLog.i(TAG,"registerScreenActionRecv");
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SCREEN_ON);
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_BATTERY_CHANGED);
		// ??????????????????
		filter.addAction(Intent.ACTION_SHUTDOWN);
		filter.addAction(MainActivity.IR_CUT_MESSAGE);
		registerReceiver(pow_action_receiver,filter);
	}

	private void unRegisterScreenActionRecv()
	{
		unregisterReceiver(pow_action_receiver);
	}
	


    /**
     * ??????????????????
     */
    public void keepScreenOn() {

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    /**
     * ??????????????????
     */
    public void cancleScreenOn() {

        window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    /**
     * ????????????????????????
     */
    private void vibrator() {
        if (SettingUtils.getBoolenValues(this, "vibrate")) {
            if (vibrator != null) {
                vibrator.vibrate(300);
            }
        }
    }




    /**
     * ???????????????????????????????????????
     * ???????????????????????????????????????
     * view?????????
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
        tv_memory.setText("Free???" + SDCardUtils.getSDCardFreeSize(this));
    }


//-----------------------------??????????????????????????????---------------------------------------------------

    /**
     * ??????????????????
     */
    public void registerMessageReceiver() {
        mMessageReceiver = new MessageReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_INTENT_RECEIVER);
        registerReceiver(mMessageReceiver, filter);
    }

    public MessageReceiver mMessageReceiver;
    //??????????????????action?????????????????????
    public static String ACTION_INTENT_RECEIVER = "gbpe.policeass.receiver";
    //??????USB???action
    public static String ACTION_INTENT_USB = "gbpe.policeass.usb";

    //=================================================================================================================================================================
    public static int power;

    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ACTION_INTENT_RECEIVER)) {
            } else if (intent.getAction().equals(ACTION_INTENT_USB)) {
                //??????USB????????????
            }
        }
    }

    /**
     * ?????????????????????
     */
    private RelativeLayout ll_bottom_button;
    private boolean isShowing;
    long[] mHits = new long[3];

    public void mClick(View view) {
        System.arraycopy(mHits, 1, mHits, 0, mHits.length - 1);
        mHits[mHits.length - 1] = SystemClock.uptimeMillis();  // ??????????????????????????????????????????????????????,??????????????? :?????????,??????????????????
        if (mHits[0] >= (SystemClock.uptimeMillis() - 500)) {  // ????????????????????????
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

    // ??????????????????
    // ????????????Activity??????????????????????????????????????????????????????????????????
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
                if (Intent.ACTION_POWER_CONNECTED.equals(intent.getAction()))//????????????
                {
                    isCharging = true;

                } else if (Intent.ACTION_POWER_DISCONNECTED.equals(intent.getAction())) //????????????
                {
                    isCharging = false;
                }
             //   Toast.makeText(MainActivity.this, "?????????" + isCharging + "," + status, 1).show();
                if (isCharging) {
					onChargeProc();
                } else {
                    offChargeProc();
                }
            }
        }
    };


    /**
     * ?????????????????????????????????????????????
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
     * ???surface???????????????fragment
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
     * ???surface???????????????fragment
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

//=============================================?????????EventBus??????==============================================================================================
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
			// ????????????
			tid.start(false, mRecordTick /*, true*/);
		} else {
			tid.start(true, mRecordTick /*, true*/);
		}
		
        if (Setup.screenOn){
            keepScreenOn();//??????????????????
        }
         vibrator();

        if (Setup.isNormalRecording) {
            btnVideoRecorder.setSelected(true);
        } else {
            btnVideoRecorder.setSelected(false);
        }

		/* ???????????????????????? */
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
		 
        cancleScreenOn();//??????????????????
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
        keepScreenOn();//??????????????????

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
     * ?????????????????????,IRcut
     * @param automatic
     */
    @Override
    public void onIRcutAutomaticEvent(Automatic automatic) {
        super.onIRcutAutomaticEvent(automatic);
        AppLog.i(TAG," zhhs_onIRcutAutomaticEvent ");
        //IRCutUtils.IRCutMotor();
        /* ??????????????????IRcut, ????????????????????????IRcut??????????????? */
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
     * ?????????????????????????????????
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
        if (isAudioRecording) {//?????????????????????????????????????????????
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

        //   Toast.makeText(MainActivity.this, "???????????????", Toast.LENGTH_SHORT).show();
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
                //??????????????????????????????????????????Provider?????????????????????????????????????????????
                @Override
                public void onLocationChanged(Location location) {
                    VideoSet.longitude = location.getLongitude();
                    VideoSet.latitude = location.getLatitude();
                    setGpsIndicator(true);
                }

                // Provider????????????????????????????????????????????????????????????????????????????????????
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

                //  Provider??????enable????????????????????????????????????????????????????????????GPS???????????????????
                @Override
                public void onProviderEnabled(String provider) {
                    AppLog.i(TAG, "GPS onProviderEnabled");
                }

                // Provider???disable???????????????????????????GPS?????????
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
     * ?????????????????????
     * ????????????????????????????????????
     * @param view
     */
    /*
    public void startAndStop2(View view) {
            vibrator();

        if (!CameraHandlerThread.isCameraOpened(this)){
            Toast.makeText(MainActivity.this, "???????????????11", Toast.LENGTH_SHORT).show();
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
     * ????????????????????????
     *
     * @param context
     * @return ???????????????
     */
    public static String getOperator(Context context) {


        String ProvidersName = "";
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        String IMSI="";
        try{
            IMSI = telephonyManager.getSubscriberId();
            AppLog.i("qweqwes", "???????????????" + IMSI);
        }catch (SecurityException e){
            e.printStackTrace();
        }

        if (IMSI != null) {
            if (IMSI.startsWith("46000") || IMSI.startsWith("46002") || IMSI.startsWith("46007")) {
                ProvidersName = "????????????";
            } else if (IMSI.startsWith("46001")  || IMSI.startsWith("46006")) {
                ProvidersName = "????????????";
            } else if (IMSI.startsWith("46003")) {
                ProvidersName = "????????????";
            }
            return ProvidersName;
        } else {
            return "???????????????sim?????????";
        }
    }

    /**
     * ?????????????????????????????????????????????
     * ??????LTE?????????3G/2G????????????????????????????????????????????????
     * LTE????????????????????????????????????????????????
     * 3G/2G???????????????????????????API????????????????????????
     * asu ??? dbm ???????????????????????? dbm=-113 + 2*asu
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

				/* ??????????????????????????????dbm??????level */
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
                    AppLog.e(TAG,"????????????????????????");
                    e.printStackTrace();
                }

                /*
                if(tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_LTE){
                    //4G?????? ????????????   >-90dBm ????????????
                    int Itedbm = Integer.parseInt(params[9]);
                    setDBM(Itedbm+"");

                }else if(tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSDPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_HSUPA ||
                        tm.getNetworkType() == TelephonyManager.NETWORK_TYPE_UMTS){
                    //3G??????????????????  >-90dBm  ????????????  ps:????????????3G????????????  ???????????????dbm???????????????85dbm???
                    //?????????????????????????????????3G????????????????????????3G???????????????????????????????????????????????? ??????????????????????????????????????????????????????
                    //String yys = IntenetUtil.getYYS(getApplication());//?????????????????????
                    String yys = getOperator(context);
                    if (yys=="????????????") {
                        setDBM(0+"");//????????????3G??????????????????????????????0
                    }else if (yys=="????????????") {
                        int cdmaDbm = signalStrength.getCdmaDbm();
                        setDBM(cdmaDbm+"");
                    }else if (yys=="????????????") {
                        int evdoDbm = signalStrength.getEvdoDbm();
                        setDBM(evdoDbm+"");
                    }

                }else{
                    //2G??????????????????>-90dBm ????????????
                    int asu = signalStrength.getGsmSignalStrength();
                    int dbm = -113 + 2*asu;
                    setDBM(dbm+"");
                }*/

            }
        };
        //????????????
        tm.listen(mylistener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
    }

    private  void calculateOrientation() {
        float[] values = new float[3];
        float[] R = new float[9];
        SensorManager.getRotationMatrix(R, null, accelerometerValues, magneticFieldValues);
        SensorManager.getOrientation(R, values);

        // ???????????????????????????????????????????????????
        values[0] = (float) Math.toDegrees(values[0]);
//        AppLog.i(TAG, values[0]+"");
        //values[1] = (float) Math.toDegrees(values[1]);
        //values[2] = (float) Math.toDegrees(values[2]);
/*
        if(values[0] >= -5 && values[0] < 5){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= 5 && values[0] < 85){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= 85 && values[0] <=95){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= 95 && values[0] <175){
            AppLog.i(TAG, "??????");
        }
        else if((values[0] >= 175 && values[0] <= 180) || (values[0]) >= -180 && values[0] < -175){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= -175 && values[0] <-95){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= -95 && values[0] < -85){
            AppLog.i(TAG, "??????");
        }
        else if(values[0] >= -85 && values[0] <-5){
            AppLog.i(TAG, "??????");
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
		//????????????Sensor
		Sensor aSensor;
		Sensor mSensor;
		
		AppLog.i(TAG, "get Location");


        compassSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
		/*??????????????????*/
		aSensor = compassSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		/*??????????????????hm1?????????????????????*/
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
			                AppLog.i(TAG,"?????????????????????????????????");
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
        if (policyManager.isAdminActive(adminReceiver)) {//?????????????????????????????????
            AppLog.i(TAG,"??????????????????");

        } else {
            AppLog.i(TAG,"?????????????????????");

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
     * @param view ???????????????????????????????????????
     */
    public void checkAndTurnOnDeviceManager(View view) {
        Intent intent = new Intent(DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN);
        intent.putExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, adminReceiver);
        //intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION, "???????????????????????????????????????...");//?????????????????????
        intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION, R.string.open_device_manager_tips);
        startActivityForResult(intent, 0);

    }

    /**
     * @param view ??????
     *
     * */
    public void setScreenOn(View view) {
        mWakeLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "ycb:tag");
        mWakeLock.acquire();
        mWakeLock.release();
    }

    /**
     * @param view ??????
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
     * @param view ?????????????????????
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
        AppLog.d(TAG, aVPDN.GetApn("???????????????????????????????????????????????????????????CTNET")+"");


        if(!aVPDN.DefaultAPNName().equals("IGRPVPDN"))
        {
            if(!aVPDN.Exisit("?????????????????????????")){
                //?????????????vpdn?????????????????????????
                startActivityForResult(new Intent(android.provider.Settings.ACTION_APN_SETTINGS), 0);
            }
            apnid= aVPDN.GetApn("?????????????????????????");
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


