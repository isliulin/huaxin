//package com.camera.usbwebcam;
package gbpe.policeass.uvc;

import java.io.File;
import java.io.FileOutputStream;
import java.sql.Date;
import java.text.SimpleDateFormat;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.PixelFormat;


import android.os.Bundle;
import android.os.Environment;
import android.os.Message;

import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
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
import android.util.Log;

import com.camera.usbwebcam.CameraPreview;
//import com.guobao.lightboard.HardwareControler;

import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.DevControl;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.views.CountDownTimerView;
import gbpe.policeass.views.MemoryView;
import gbpe.policeass.views.TimeIndicator;

//public class UvcMainActivity extends Activity {
public class UvcMainActivity extends BaseActivity
        implements View.OnLongClickListener{
//public class UvcMainActivity extends MainActivity {
	CameraPreview cp;
	/* 隐藏的控件 */
	public ImageView btnVideoRecorder, btnTakePhotos,
				btnSoundRecorder;
	//计时器控件
    private Chronometer chronometer;
	private TextClock textClock, tcDate;
	private TextView tv_tital, tv_pid, tv_sn;
	private ImageButton tv_menue;
    private TextView tv_memory;

	private BroadcastReceiver pow_action_receiver = null;
	private boolean shortPress;

	private MemoryView iv_memery;

	// delete 20180820, 删了好像没卵用啊
    private CountDownTimerView cdtShow;
    private Button switchBtn;
    TimeIndicator tid;
	
	private ImageView iv_cd;
    private Window window;
    private TextView tv_battery_level;
	private RelativeLayout ll_bottom_button;

	//private Button mVideoStartBtn;
    private SurfaceHolder mSurfaceHolder;
    private File mRecVedioPath;
    private File mRecAudioFile;
   // private TextView timer;
    private int hour = 0;
    private int minute = 0;
    private int second = 0;
    private boolean bool;
    private int parentId;
    protected boolean isPreview;
    private boolean isRecording = true; // true表示没有录像，点击开始；false表示正在录像，点击暂停
	//private AvcEncoder avcCodec;
    //private Window window;
	private static final String TAG = "UvcMainActivity";
	private AvcEncoder avcCodec;

	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// setContentView(R.layout.uvc_main_activity);
		//全屏
        /*
        getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
		*/
		/*
		try {
			Runtime.getRuntime().exec("su");
			
		} catch (IOException e) {
			e.printStackTrace();
		}*/
		//cp = (CameraPreview) findViewById(R.id.cp);

		//mVideoStartBtn = (Button) findViewById(R.id.button_video);

		/*
        mVideoStartBtn.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View v) {

			if (isRecording) {

				second = 0;
				minute = 0;
				hour = 0;
				bool = true;
				
				showMsg(getString(R.string.uvc_record_start));
				isRecording = !isRecording;
				// 此处为保存设置
				//cp.StartVdoRecord();
				//avcCodec = new AvcEncoder(cp.IMG_WIDTH,cp.IMG_HEIGHT,cp.framerate,cp.bitrate);
				//avcCodec.StartEncoderThread();
				
			} else {

				isRecording = !isRecording;
				showMsg(getString(R.string.uvc_record_finish));
				//avcCodec.StopThread();
			}
            }
        });*/

        /*
        Button btnImgStart = (Button) findViewById(R.id.button1);
        btnImgStart.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                cp.saveBitmapToSD();
				showMsg("拍照!!!");
        }
       }
		);*/
	}

    @Override
    protected void handler(Message msg) {

    }

    @Override
    protected void initView() {

    }

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
        //setContentView(R.layout.activity_main);
        setContentView(R.layout.uvc_main_activity);

    }

	@Override
    public void findViewById()
    {
		//cp = (CameraPreview) findViewById(R.id.cp);
		setGLRenderer();

		switchBtn = (Button) findViewById(R.id.uvc_charge);
		btnVideoRecorder = (ImageView) findViewById(R.id.uvc_iv_video_record);
		btnTakePhotos = (ImageView) findViewById(R.id.uvc_iv_take_picture);
		btnSoundRecorder = (ImageView) findViewById(R.id.uvc_iv_audio_reocrd);
		tv_menue = (ImageButton) findViewById(R.id.uvc_tv_menue);
		ll_bottom_button = (RelativeLayout) findViewById(R.id.uvc_bottom_button);
		iv_memery = (MemoryView) findViewById(R.id.uvc_iv_memory);
		chronometer = (Chronometer) findViewById(R.id.uvc_timer_main);
		textClock = (TextClock) findViewById(R.id.uvc_tc_time);
		tcDate = (TextClock) findViewById(R.id.uvc_tc_date);
		tv_tital = (TextView) findViewById(R.id.uvc_tv_tital);
		tv_pid = (TextView) findViewById(R.id.uvc_tv_pid);
		tv_sn = (TextView) findViewById(R.id.uvc_tv_sn);
		iv_cd = (ImageView) findViewById(R.id.uvc_iv_cd);
		tv_memory = (TextView) findViewById(R.id.uvc_tv_memory);
		tv_battery_level = (TextView) findViewById(R.id.uvc_tv_battery_level);

/*
		button = (Button) findViewById(R.id.vvvv);
           findViewById(R.id.btn_jump2).setOnClickListener(new View.OnClickListener() {
                       @Override
                       public void onClick(View v) {
                       //    startActivity(new Intent(MainActivity.this, CompassActivity.class));
                           if (Setup.VideoPrerecord)
                           VideoManager6.startRecord();

                        }
                   });*/

		tid = (TimeIndicator)findViewById(R.id.uvc_tid);
	}

	@Override
	public void setListener()
    {
		SurfaceHolder holder = cp.getHolder();

        holder.addCallback(new Callback() {

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            
            	Log.i(TAG,"surfaceDestroyed");
				
                cp = null;
                mSurfaceHolder = null;
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {

				Log.i(TAG,"surfaceCreated");
				
                mSurfaceHolder = holder;
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                    int width, int height) {
                    
                Log.i(TAG,"surfaceCreated");
				
                mSurfaceHolder = holder;
            }
        });
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	}

	public void setGLRenderer() {
        final ViewGroup container = ((ViewGroup) findViewById(R.id.uvc_fl_sv_container));
        //if (cp != null || record != null) 
        if (cp != null)
		{
            container.removeView(container.getChildAt(0));
            cp = null;
        }
        //cameraSurfaceView = new CameraSurfaceView(this);
        cp = new CameraPreview(this);
        //cameraSurfaceView.setOnClickListener(new View.OnClickListener() {
        cp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //mClick(v);
                Log.i(TAG, "surface on clicked!");
            }
        });
        //container.addView(cameraSurfaceView);
        container.addView(cp);
    }

	

	@Override
	public void initConfig()
	{
		 // 设置缓存路径
        mRecVedioPath = new File(Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/uvcvideo/");
        if (!mRecVedioPath.exists()) {
            mRecVedioPath.mkdirs();
        }

		Log.i(TAG,"cash path: "+mRecVedioPath);

		showText();
        showFreeSDSize();
        checkSim();//判断SIM卡是否可用
        setTextClock();////显示当前系统日期和时间

		vibrator();//开机是否震动

		//语音播报功能
        /*util.textToSpeech("警员编号" + SettingUtils.
                getStrValues(this, "policeNumber", "111111") + "开机");*/

		pow_action_receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
            	String action = intent.getAction();
                Log.i(TAG,"receive screen action broadcast, "+action);
                if (action.equals(Intent.ACTION_SCREEN_OFF))
                {
                    Log.i(TAG,"screen off");
					
                } else if(action.equals(Intent.ACTION_SCREEN_ON))
                {
                    Log.i(TAG,"screen on");
					
                } else if(action.equals(Intent.ACTION_BATTERY_CHANGED))
				{
					Log.i(TAG, "Battery info");
					int current=intent.getExtras().getInt("level");//获得当前电量
        			int total=intent.getExtras().getInt("scale");//获得总电量
					int percent = current*100/total;

					Log.i(TAG, "battery percentage: "+percent);
					
					tv_battery_level.setText(percent + "%");
                	Setup.battery_level = percent + "%";
					
				}
            }
        };

		// add by ycb, 注册屏幕监控
		registerScreenActionRecv();

		
	}

	/*
	*  注册SCREEN_OFF广播
	*  add by ycb
	*/
	private void registerScreenActionRecv()
    {
    	Log.i(TAG,"registerScreenActionRecv");
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SCREEN_ON);
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_BATTERY_CHANGED);
		registerReceiver(pow_action_receiver,filter);
	}

	private void unRegisterScreenActionRecv()
	{
		unregisterReceiver(pow_action_receiver);
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


    public void onPictureTaken(byte[] data) {
		   Bitmap bitmap = BitmapFactory.decodeByteArray(data, data.length, 0);
		   Matrix matrix = new Matrix();
		   // 设置缩放
		   matrix.postScale(5f, 4f);
		   bitmap = Bitmap.createBitmap(bitmap, 0, 0,
				   bitmap.getWidth(), bitmap.getHeight(),
				   matrix, true);
	   
		   String path = Environment
				   .getExternalStorageDirectory()
				   .getAbsolutePath()
				   + "/hfdatabase/img/"
				   + String.valueOf(parentId) + "/";
		   long time=System.currentTimeMillis();//long now = android.os.SystemClock.uptimeMillis();  
		   String fileName = new SimpleDateFormat(
				   "yyyyMMddHHmmss").format(new Date(time))
				   + ".jpg";
		   File out = new File(path);
		   if (!out.exists()) {
			   out.mkdirs();
		   }
		   out = new File(path, fileName);
		   try {
			   FileOutputStream outStream = new FileOutputStream(
					   out);
			   bitmap.compress(CompressFormat.JPEG, 100,
					   outStream);
			   outStream.close();
		   } catch (Exception e) {
			   e.printStackTrace();
		   }
		   showMsg("拍照成功");
	   }

	
	   /*
     * 消息提示
     */
    private Toast toast;

    public void showMsg(String arg) {
        if (toast == null) {
            toast = Toast.makeText(this, arg, Toast.LENGTH_SHORT);
        } else {
            toast.cancel();
            toast.setText(arg);
        }
        toast.show();
    }

	public void startAndStop2(View view)
    {
		Log.i(TAG, "startAndStop2");
	}

	public boolean checkUsbCamera()
	{
		// 此处检查usb摄像头是否连接
		return true;
	}

	public void videoRecord()
	{
		if (!checkUsbCamera())
		{
			Toast.makeText(UvcMainActivity.this,"usb摄像头异常", Toast.LENGTH_SHORT).show();
			return;
		}

		if (SettingUtils.getBoolenValues(this, "key_tone")) {
			keySound.play(0, 0);
		}
		
        if (isRecording) {

            second = 0;
            minute = 0;
            hour = 0;
            bool = true;
            //showMsg(getString(R.string.record_start));
  			Log.i(TAG, "开始录像");
			Toast.makeText(UvcMainActivity.this, "开始录像", Toast.LENGTH_SHORT).show();
            isRecording = !isRecording;
			tid.start(false,0 /*, true*/);
            //cp.StartVdoRecord();
            //AppLog.i(TAG, FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix));
			//File f = new File(FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix));
			String path = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);
			path = path.substring(0,path.lastIndexOf("."))+"u."+Setup.videoFilePostfix;
			AppLog.i(TAG, "record path "+path);

            avcCodec = new AvcEncoder(cp.IMG_WIDTH,cp.IMG_HEIGHT,cp.framerate,cp.bitrate,path);
            avcCodec.StartEncoderThread();
			//tid.start(true);

        } else {

            isRecording = !isRecording;
			tid.stop();
            //showMsg(getString(R.string.record_finish));
            Toast.makeText(UvcMainActivity.this, R.string.stop_recorder, Toast.LENGTH_SHORT).show();
            Log.i(TAG, "停止录像");
            avcCodec.StopThread();
			//tid.stop();
        }
    }

	/**
	 * 判断按钮状态
	 */
	//private boolean isRecording;//录视频
	//private boolean isAudioRecording;//录音

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
				Log.i(TAG, "开始uvc录像，请确认已安装SD卡");

				videoRecord();
				
/*
				if (CameraHandlerThread.camera == null) {
					Toast.makeText(MainActivity.this, "摄像头异常5", Toast.LENGTH_SHORT).show();
					return;
				}
				if (SettingUtils.getBoolenValues(this, "key_tone")) {
					keySound.play(0, 0);
				}

				isRecording = !isRecording;
			    videoRecord();
*/
				break;
			}
			case R.id.iv_take_picture: {//拍照
				Log.i(TAG, "开始UVC拍照，请确认已安装SD卡");
				
				takePictures();
/*
				if (CameraHandlerThread.camera == null) {
					Toast.makeText(MainActivity.this, "摄像头异常6", Toast.LENGTH_SHORT).show();
					return;
				}
				if (isAudioRecording) {
					Toast.makeText(MainActivity.this, "不可拍照", Toast.LENGTH_SHORT).show();
					return;
				}
				if (SettingUtils.getBoolenValues(this, "key_tone")) {
					keySound.play(1, 0);
				}
				takePictures();
*/
				break;
			}
			case R.id.iv_audio_reocrd: {//录音

				Log.i(TAG,"开始录音");
/*
				if (SettingUtils.getBoolenValues(this, "key_tone")) {//是否播放按键音
					keySound.play(0, 0);
				}
				if (!isAudioRecording)
				{
					btnSoundRecorder.setSelected(true);
					if (isRecording) {//按屏幕上录像按钮开始的录像
						btnVideoRecorder.setSelected(false);
						isRecording = !isRecording;
						videoRecord();
					}
				} else {
					btnSoundRecorder.setSelected(false);
				}
				isAudioRecording = !isAudioRecording;

				if ( Setup.videoRecording ) {//按物理按键开始的录像
					videoRecord();//停止视频播放
					handler.postDelayed(new Runnable() {
						@Override
						public void run() {
							setGLRenderer3();
						}
					},1000);
				}else {
					setGLRenderer3();
				}
*/

				break;
			}
			case R.id.tv_menue: {//主页界面

				Log.i(TAG, "开启控制页面");
/*
				 if (Setup.VideoPrerecord || Setup.isSegmentedVideo ||	Setup.videoRecording) {
					 Setup.currPage = false;
					 VideoManager6.stopAll();
					 Setup.videoRecording = false;
					 Setup.isNormalRecording = false;
					 FileUtils.deleteFolder(Setup.pre_file_dir);
				}
				if (isAudioRecording) {//如果正在录音，则停掉录音并保存
					setGLRenderer3();
					isAudioRecording = false;
				}

				tid.setInVisible();
				tid.rempveTimeRun();
				Intent intent1 = new Intent(this, HomeActivity.class);
				startActivity(intent1);
*/
				break;
			}
			case R.id.change:

				Log.i(TAG, "切换摄像头");
/*
				if (CameraHandlerThread.isCameraOpened(this)) {
					CameraHandlerThread.switchCameraId();
				}
*/
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

				//Log.i(TAG, "长按，开始uvc录像");
				Toast.makeText(UvcMainActivity.this,"开始Uvc录像",Toast.LENGTH_SHORT).show();
				Log.i(TAG, "onLongClick, 开始Uvc录像");
				videoRecord();
/*
				if (CameraHandlerThread.camera == null) {
					Toast.makeText(MainActivity.this, "摄像头异常7", Toast.LENGTH_SHORT).show();
					return true;
				}

				if (SettingUtils.getBoolenValues(this, "key_tone")) {
					keySound.play(0, 0);
				}

				isRecording = !isRecording;
				Setup.longPressVideo = true;
				videoRecord();
*/
				break;
			}
			case R.id.iv_take_picture: {//拍照

				//Log.i(TAG, "长按，开始拍照");
				//Toast.makeText(UvcMainActivity.this,"图像生成中",Toast.LENGTH_SHORT).show();
				takePictures();
/*
				if (CameraHandlerThread.camera == null) {
					Toast.makeText(MainActivity.this, "摄像头异常8", Toast.LENGTH_SHORT).show();
					return true;
				}
				if (SettingUtils.getBoolenValues(this, "key_tone")) {
					keySound.play(1, 0);
				}
				takePictures();
*/
				break;
			}
			case R.id.iv_audio_reocrd: {//录音

				//Log.i(TAG, "长按，开始录音");
				//Toast.makeText(UvcMainActivity.this,"开始录音",Toast.LENGTH_SHORT).show();
/*
				if (SettingUtils.getBoolenValues(this, "key_tone")) {//是否播放按键音
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
   */
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
		//HardControl.cameraTuning(irState?0:1);
		//IRCutUtils.IRcutAction(irState);

		// 需要主线程延时，否则无法切换，why？
		/*try {
			sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}*/
	}

	@Override
    public boolean onKeyUp(int keyCode, KeyEvent event){
		AppLog.i(TAG,"get key up "+keyCode);

		if (keyCode == KeyEvent.KEYCODE_BACK){
			AppLog.i(TAG,"back");
			finish();
		} else if (keyCode == KeyEvent.KEYCODE_5){

			AppLog.i(TAG, "onKeyLongPress, 开始录像");
			if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
            {
                keySound.play(0, 0);
            }
            //Setup.longPressVideo = true;
            videoRecord();

			return true;

		} else if (keyCode == KeyEvent.KEYCODE_6){

			AppLog.i(TAG, "长按拍照");
			Toast.makeText(UvcMainActivity.this, "拍照", Toast.LENGTH_SHORT).show();
			takePictures();
			return true;

		}
		return super.onKeyUp(keyCode,event);
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
			//Intent intent = new Intent(
			//		Settings.ACTION_LOCATION_SOURCE_SETTINGS);
			//startActivityForResult(intent, 0);
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
				DevControl.writeGuobaoGpio(96,0);
			}
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_3)//录音，长按保存为重点文件
		{
		
			// 录音长按切换 IR_CUT, 20180830
        	//AppLog.i(TAG,"开启红外照明");
            //if (Setup.inFraRed){
            //    return true;
            //}

            //IRCutActionProc(CameraHandlerThread.isRecordingGlobal);
            //sendIrCutSwBroadcase(this,CameraHandlerThread.isRecordingGlobal,Setup.currIRcutState);
            //IRCutActionProc(CameraHandlerThread.isRecordingGlobal,Setup.currIRcutState);
            //IRCutUtils.setIRstate(!IRCutUtils.getIRstate());
            //IRCutActionProc(CameraHandlerThread.isRecordingGlobal,IRCutUtils.getIRstate());

			return true;
		} else if (keyCode == KeyEvent.KEYCODE_4) {
		   // Toast.makeText(MainActivity.this, "Long==>KEYCODE_4", Toast.LENGTH_SHORT).show();

			//Intent intent = getPackageManager().getLaunchIntentForPackage("com.unionbroad.app");
			//if (intent != null) {
			//	startActivity(intent);
			//}

			return true;
		} else if (keyCode == KeyEvent.KEYCODE_5)//保存为重点文件KEYCODE_5
		{
			//Log.i(TAG, "onKeyLongPress, 开始录像");
			//if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
            //{
            //    keySound.play(0, 0);
            //}
            //Setup.longPressVideo = true;
            //videoRecord();
            return true;
		} else if (keyCode == KeyEvent.KEYCODE_6)//保存为重点文件
		{
		 //   Toast.makeText(MainActivity.this, "Long==>KEYCODE_6", Toast.LENGTH_SHORT).show();
		 	/*
			if (isAudioRecording) {
				Toast.makeText(MainActivity.this, "不可拍照", Toast.LENGTH_SHORT).show();
				return true;
			}
			if (SettingUtils.getBoolenValues(this, "key_tone"))//是否播放按键音
			{
				keySound.play(1, 0);
			}

			takePictures();*/
			//Log.i(TAG, "长按拍照");
			//Toast.makeText(UvcMainActivity.this, "拍照", Toast.LENGTH_SHORT).show();
			//takePictures();
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_7)//打开红外线
		{
			// UVC摄像头暂不支持夜间模式
			/*
			if (Setup.inFraRed){
				return true;
			}
			IRCutUtils.IRcutAction();
			return true;*/
		}
		return false;
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

		Log.i(TAG,"get key down "+keyCode);

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            //shortOrLongPress(event);
			finish();
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

	@Override
    protected void onResume() 
    {
		super.onResume();
	}

	@Override
    protected void onPause() 
	{
		super.onPause();
	}

	 /**
     * 拍照
     */
    private void takePictures() 
    {
    	if (!checkUsbCamera())
		{
			Toast.makeText(UvcMainActivity.this,"usb摄像头异常", Toast.LENGTH_SHORT).show();
		}

		if (SettingUtils.getBoolenValues(this, "key_tone")) {
			keySound.play(1, 0);
		}
		cp.saveBitmapToSD();
	}

	
	  /*
     * 生成video文件名字
     */
	  /*
    protected void videoRename() {
        String path = Environment.getExternalStorageDirectory()
                .getAbsolutePath()
                + "/uvcvideo/"
                + String.valueOf(parentId) + "/";
        long time=System.currentTimeMillis();
        String fileName = new SimpleDateFormat("yyyyMMddHHmmss")
                .format(new Date(time)) + ".mp4";
        File out = new File(path);
        if (!out.exists()) {
            out.mkdirs();
        }
        out = new File(path, fileName);
        if (mRecAudioFile.exists())
            mRecAudioFile.renameTo(out);
    }*/

    /*
     * 定时器设置，实现计时
     */
    /*
    private Handler handler = new Handler();
    private Runnable task = new Runnable() {
        public void run() {
            if (bool) {
                handler.postDelayed(this, 1000);
                second++;
                if (second >= 60) {
                    minute++;
                    second = second % 60;
                }
                if (minute >= 60) {
                    hour++;
                    minute = minute % 60;
                }
//                timer.setText(format(hour) + ":" + format(minute) + ":"
 //                       + format(second));
            }
        }
    };
    */

    /*
     * 格式化时间
     */
    /*
    public String format(int i) {
        String s = i + "";
        if (s.length() == 1) {
            s = "0" + s;
        }
        return s;
    }
    */

	/**
     * 判断SIM卡是否可用
     */
    private void checkSim() {
        iv_memery.setImgSrc();
    }

    /*
     * 覆写返回键监听
     */
    @Override
    public void onBackPressed() {
    	AppLog.i(TAG,"onBackPressed");
        finish();
    }

	@Override
    protected void onDestroy() {
    	AppLog.i(TAG,"onDestroy");
    	super.onDestroy();
		unRegisterScreenActionRecv();

		//HardwareControler.setGPIODirection(62, 0);
        //HardwareControler.setGPIOValue(62,0);
	}

}

