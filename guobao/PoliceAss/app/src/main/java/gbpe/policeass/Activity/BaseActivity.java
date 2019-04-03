package gbpe.policeass.Activity;


import android.app.Activity;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Vibrator;
import android.os.storage.StorageManager;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.util.DisplayMetrics;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;
import com.nostra13.universalimageloader.core.ImageLoader;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.lang.ref.WeakReference;
import java.lang.reflect.Method;
import java.util.Locale;

import gbpe.policeass.R;
import gbpe.policeass.Setup.SettingTags;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.modle.AnyEvent;
import gbpe.policeass.modle.AudioStartEvent;
import gbpe.policeass.modle.AudioStopEvent;
import gbpe.policeass.modle.Automatic;
import gbpe.policeass.modle.OnCombineVideo;
import gbpe.policeass.modle.OnEventMessage;
import gbpe.policeass.modle.OnGetLocationEvent;
import gbpe.policeass.modle.OnReleaseCamera;
import gbpe.policeass.modle.OnStartCountEvent;
import gbpe.policeass.modle.OnStopCountEvent;
import gbpe.policeass.modle.PhotographEvent;
import gbpe.policeass.modle.RemoteStartSoundRecord;
import gbpe.policeass.modle.RemoteStopSoundRecord;
import gbpe.policeass.modle.VideoStartEvent;
import gbpe.policeass.modle.VideoStopEvent;
import gbpe.policeass.log.MyService;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.ClickUtil;
import gbpe.policeass.utils.Commons;
import gbpe.policeass.utils.SoundPlay;
import gbpe.policeass.utils.TextToSpeechUtil;


public abstract class BaseActivity extends FragmentActivity
        implements View.OnClickListener{
    public ImageLoader imageLoader = ImageLoader.getInstance();
    public Vibrator vibrator;//震动

    public App mApp;
    private MyService.MyBinder myBinder;
    public FragmentTransaction transaction;
    public MyService myService;
    public SoundPlay keySound = new SoundPlay();
    private BroadcastReceiver usb_receiver = null;

    private Configuration config;
    private DisplayMetrics dm;
    private Resources resources;
    //public TextToSpeechUtil util;
//    protected Handler handler =new Handler(){
//        @Override
//        public void handleMessage(Message msg) {
//            super.handleMessage(msg);
//            switch (msg.what){
//                case Commons.constant:
//                    handler(msg);
//                    break;
//            }
//        }
//    };

    protected  MyHandler handler = new MyHandler(this){
        @Override
        public void handleMessage(Message msg) {
        final Activity activity = mActivityReference.get();
                if (activity != null) {
                    switch (msg.what){
                        case Commons.constant:
                            handler(msg);
                            break;
                    }
                }
            }

    };
    static class MyHandler extends Handler {
        WeakReference<Activity> mActivityReference;
        MyHandler(Activity activity) {
            mActivityReference= new WeakReference<Activity>(activity);
        }
    }



//    MyHandler myHandler = new MyHandler(this);
//    private static class MyHandler extends Handler {
//
//        private WeakReference<Activity> activityWeakReference;
//
//        public MyHandler(Activity activity) {
//            activityWeakReference = new WeakReference<>(activity);
//        }
//
//        @Override
//        public void handleMessage(Message msg) {
//            Activity activity = activityWeakReference.get();
//            if (activity != null) {
//                if (msg.what ==  Commons.constant) {
//                    // 做相应逻辑
//                    handler(msg);
//                }
//            }
//        }
//    }
/*

    private static final String ACTION_USB_STATE = "android.hardware.usb.action.USB_STATE";
    private static final String ACTION_USB_DEV_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    private static final String ACTION_USB_DEV_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";
    private static final String ACTION_MEDIA_MOUNTED = "android.intent.action.MEDIA_MOUNTED";
    private static final String ACTION_MEDIA_EJECT = "android.intent.action.MEDIA_EJECT";

	public boolean isUsbForbiden() {
		
        if (getUsbSetPref().equals(getString(R.string.usb_forbiden))) {
            return true;
        } else {
			return false;
		}	

	}

	public boolean isUsbDiskMode() {
		if (getUsbSetPref().equals(getString(R.string.usb_mode_udisk))) {
		    return true;
        } else {
		    return false;
        }
	}

	public String getUsbSetPref() {
		return PreferenceManager.getDefaultSharedPreferences(this)
                .getString(SettingTags.KEY_USBMODE,getString(R.string.usb_forbiden));
	}


	public void setUsbDiskMode() {
	    try {
            StorageManager storageManager = (StorageManager) getSystemService(Context.STORAGE_SERVICE);
            if (storageManager != null) {
                Settings.Global.putInt(getContentResolver(), Settings.Global.ADB_ENABLED, 0);
				Method method = storageManager.getClass().getMethod("isUsbMassStorageEnabled");
				boolean state = (boolean)method.invoke(storageManager);
				if (!state) {
	                method = storageManager.getClass().getMethod("enableUsbMassStorage");
	                method.invoke(storageManager);
                    //Toast.makeText(this,"set udisk ok",Toast.LENGTH_SHORT).show();
				}
            }
        } catch (Exception e) {
	        AppLog.e (getLocalClassName(),""+e.toString());

        }
    }

    public void closeUsbInterface() {

        try{
			StorageManager storageManager = (StorageManager)getSystemService(Context.STORAGE_SERVICE);
            //Settings.Global.putInt(getContentResolver(),Settings.Global.ADB_ENABLED,0);
            Method method = storageManager.getClass().getMethod("isUsbMassStorageEnabled");
            boolean state = (boolean)method.invoke(storageManager);
            if (state) {   // 已连接
                method = storageManager.getClass().getMethod("disableUsbMassStorage");
                method.invoke(storageManager);
            }

        } catch (Exception e) {
            AppLog.i (getLocalClassName(), "closeUsbInterface error: "+ e.toString());
        }

        try {
            SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
            SharedPreferences.Editor editor = sp.edit();
            editor.putString(SettingTags.KEY_USBMODE,getString(R.string.usb_forbiden));
            editor.commit();

        } catch (Exception e) {
            AppLog.e (getLocalClassName(), "refrash error");

        }

    }

	public void adbSet(boolean on) {

	}

	public boolean getAdbConfig() {
        return false;
	}

	public static boolean isUsbConnected = false;
	public Handler usbHandler = null;
	public Runnable usbRunable = new Runnable() {
        @Override
        public void run() {
            if (isUsbConnected) { // 如果为连接
                if (getAdbConfig()) {
                    // 优先adb连接
                    adbSet(true);
                }
            } else {
                closeUsbInterface();
            }
            if (usbHandler!=null) {
                usbHandler.removeCallbacks(usbRunable);
                usbHandler = null;
            }
        }
    };

    public void onUsbState(Intent intent, Context context) {
        if (intent.getExtras().getBoolean("connected")){
            //Toast.makeText(context,"USB "+ getString(R.string.inserted),Toast.LENGTH_SHORT).show();
            //AppLog.i ("BaseActivity","usb connected");
			isUsbConnected = true;

        } else {
            //Toast.makeText(context,"USB "+ getString(R.string.ejected),Toast.LENGTH_SHORT).show();
            // 检测到USB断开，则关闭USB功能
            //closeUsbInterface();
			isUsbConnected = false;
        }

        if (usbHandler==null) {
            usbHandler = new Handler();
            usbHandler.postDelayed(usbRunable,5000);
        }

    }

    public void onUsbDeviceAttached(Context context) {
        //Toast.makeText(context,"USB "+ getString(R.string.device) + getString(R.string.attached),Toast.LENGTH_SHORT).show();
    }

    public void onUsbDeviceDettached(Context context) {
        //Toast.makeText(context,"USB "+ getString(R.string.device) + getString(R.string.dettached),Toast.LENGTH_SHORT).show();
    }

    private void registerUsbActionReceiver() {
        usb_receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                try {
                    String action = intent.getAction();
                    if (action.equals(ACTION_USB_STATE)) {
                        onUsbState(intent,context);
                    } else if (action.equals(ACTION_USB_DEV_ATTACHED)) {
                        onUsbDeviceAttached(context);
                    } else if (action.equals(ACTION_USB_DEV_DETACHED)) {
                        onUsbDeviceDettached(context);
                    }
                } catch (Exception e) {
                    AppLog.e(getLocalClassName(),"usb broadcast handle error");
                }

            }
        };
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_STATE);
        filter.addAction(ACTION_USB_DEV_ATTACHED);
        filter.addAction(ACTION_USB_DEV_DETACHED);
        //filter.addAction(ACTION_MEDIA_MOUNTED);
        //filter.addAction(ACTION_MEDIA_EJECT);
        registerReceiver(usb_receiver,filter);
    }

    private void unregisterUsbReceiver() {
        if (usb_receiver != null) {
            unregisterReceiver(usb_receiver);
            usb_receiver = null;
        }
    }*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initView();
        //util = new TextToSpeechUtil(this.getApplicationContext());
        initViews();
        Intent service =new Intent(this, MyService.class);
        if (!isFristBindService()){
            bindService(service,conn, Service.BIND_AUTO_CREATE);
        }else {
            bindService(service,conn,0);
        }

        //registerUsbActionReceiver();

        //初始化震动
        vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        //初始化语音播报功能

        EventBus.getDefault().register(this);
     }



    /**
     * 使用onEventMainThread来接收事件，那么不论分发事件在哪个线程运行，接收事件永远在UI线程执行
     * 可以用来更新UI
     * @param event
     */
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEventMainThread(AnyEvent event) {

    }

    /**
     * 使用onEventBackgroundThread来接收事件，如果分发事件在子线程运行，那么接收事件直接在同样线程
     * 运行，如果分发事件在UI线程，那么会启动一个子线程运行接收事件
     * @param event
     */
    @Subscribe(threadMode = ThreadMode.BACKGROUND)
    public void onEventBackgroundThread(AnyEvent event) {

    }


    /**
     * 使用onEventBackgroundThread来接收事件，如果分发事件在子线程运行，那么接收事件直接在同样线程
     * 运行，如果分发事件在UI线程，那么会启动一个子线程运行接收事件
     * @param event
     */
    @Subscribe(threadMode = ThreadMode.BACKGROUND)
    public void onEventBackgroundThread2(OnEventMessage event) {

    }


    /**
     * 使用onEventAsync接收事件，无论分发事件在（UI或者子线程）哪个线程执行，接收都会在另外一个子线程执行
     * @param event
     */
    @Subscribe(threadMode = ThreadMode.ASYNC)
    public void onEventAsync(AnyEvent event) {
    }
    @Subscribe(threadMode = ThreadMode.POSTING)
    public void onEventPost(AnyEvent event){
    }

     @Subscribe(threadMode = ThreadMode.MAIN)
    public void onVideoStartMainThread(VideoStartEvent event) {
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onVideoStopMainThread(VideoStopEvent event) {
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onAudioStartMainThread(AudioStartEvent event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onAudioStopMainThread(AudioStopEvent event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onRemouteAudioStartMainThread(RemoteStartSoundRecord event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onRemouteAudioStopMainThread(RemoteStopSoundRecord event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onPhotographThread(PhotographEvent event) {
    }


    @Subscribe(threadMode = ThreadMode.BACKGROUND)
    public void onIRcutAutomaticEvent(Automatic automatic){
        System.out.println("--------->>zhhs_ThreadMode.BACKGROUND_onIRcutAutomaticEvent ");
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onStartCoundDown(OnStartCountEvent event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onStopCoundDown(OnStopCountEvent event) {
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onReleaseCamera(OnReleaseCamera event) {
    }
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onCombineVideo(OnCombineVideo event) {
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onGetLocation(OnGetLocationEvent event) {
    }
    /**
     * @param beShowedFragment
     * @param fragmentTag ： 该fragment的tag
     * @param bundle :
     * @param addBackStack :
     */

    protected ServiceConnection conn =new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {

            myBinder= (MyService.MyBinder) service;
            myService = myBinder.getServiceInstance();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

         }
    };

    protected abstract void  handler(Message msg);

   // protected abstract void serviceConnned(MyService.MyBinder myBinder);

    protected abstract void initView();

    protected boolean isFristBindService(){
        return false;
    }

    public void go(Class cls) {    //跳转到指定的Activity
        Intent intent = new Intent(this, cls);
        startActivity(intent);

       // overridePendingTransition(android.R.anim.slide_in_left,android.R.anim.slide_out_right);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mApp = (App)getApplication();

         resources =getResources();
        config = resources.getConfiguration();
        dm = resources.getDisplayMetrics();
       // setSysLanguage();
      //  lockScreenTimer();
       initKeySoud();
	   //registerUsbActionReceiver();

    }

    @Override
    protected void onPause() {
        super.onPause();
        //unregisterUsbReceiver();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(conn);
        //unregisterReceiver();
       // myHandler.removeCallbacksAndMessages(null);
        handler.removeCallbacksAndMessages(null);
     }

    public <T extends View> T findView(int ViewId) {

        return (T) findViewById(ViewId);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.item_clear_memory_cache:
                imageLoader.clearMemoryCache();
                return true;
            case R.id.item_clear_disc_cache:
                imageLoader.clearDiscCache();
                return true;
            default:
                return false;
        }
    }

    /**
     * 初始化系统音频文件（按键音，拍照，录像，录音提示声音）
     */
    private void initKeySoud() {
        keySound.initSounds(BaseActivity.this);
        keySound.loadSfx(this,R.raw.keypressreturn,0);
        keySound.loadSfx(this,R.raw.camera_click,1);
        keySound.loadSfx(this,R.raw.siren2,2);
    }




    @Override
    public void onClick(View v) {
        if(!ClickUtil.isFastClick(300)){
            widgetClick(v);
        }else{
            Toast.makeText(BaseActivity.this, "主人不要点击太快！", Toast.LENGTH_SHORT).show();
         }
    }

    public void widgetClick(View v) {

    }

    public void initViews(){
        loadViewLayout();
        findViewById();
        processLogic();
        setListener();
        initConfig();
    }

    public void findViewById(){}//找id
    public void loadViewLayout(){}//加载布局
    public void setListener(){}//设置监听
    public void processLogic(){}//处理后台数据
    public void initConfig(){}

}
