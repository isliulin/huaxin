package gbpe.policeass.fragments;


import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.storage.StorageManager;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.preference.TwoStatePreference;
import android.provider.Settings;
import android.support.annotation.Nullable;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.TimePicker;
import android.widget.Toast;

import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.MainActivity;
import com.topvision.videodemo.rencoder.VideoRunnable;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.lang.reflect.Method;
import java.util.Calendar;
import java.util.List;
import java.util.Locale;

import gbpe.policeass.BuildConfig;
import gbpe.policeass.R;
import gbpe.policeass.Setup.SettingTags;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.Automatic;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.BluetoothUtil;
import gbpe.policeass.utils.OtaUpdateActivity;
import gbpe.policeass.utils.SdCardFormat;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.utils.StringUtil;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.utils.WiFiUtil;
import gbpe.policeass.utils.gps.GpsUtils;
import gbpe.policeass.views.CustomDialog;
import gbpe.policeass.views.DateTimeDialog;
import gbpe.policeass.views.PreferenceWithTip;
import gbpe.policeass.zfysim.HomeItemLab;
import tech.shutu.jni.YuvUtils;

//import gbpe.policeass.utils.OtaUpdate;

public class SettingSegFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener{

    private static final String TAG = "SettingSegFragment";
    private static final String SEG_ID_TAG = "SegmentIdTag";
    //private TextView mTextView;
    private int mSegId;
    static Camera mCamera;
    static Camera.Parameters mParameters;
	// add by ycb
	private static final int PRE_RCD_TIME = 10;
    static Context settingContext;
    //String mCurrDate = "";
    //String mCurrTime = "";

    private int mYear;
    private int mMonth;
    private int mDay;
    private int mHour;
    private int mMinute;
    private Calendar mCalendar;

    /* 附加argument bundle给fragment，需调用Fragment.setArguments(Bundle)方法。
    * 该任务必须在fragment创建后，添加给activity前完成*/
    public static SettingSegFragment newInstance(int idx){
        Bundle args = new Bundle();
        args.putInt(SEG_ID_TAG,idx);
        SettingSegFragment  fragment = new SettingSegFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //resetPref(SettingTags.KEY_VERSION,BuildConfig.VERSION_NAME +"."+BuildConfig.BUILD_TIME);
        //mInfo = getActivity().getIntent().getStringExtra(TAG);
        mSegId = getArguments().getInt(SEG_ID_TAG);
        AppLog.i(TAG, "create seg "+mSegId);

        try {
            switch (mSegId) {
                case 0:
                    addPreferencesFromResource(R.xml.preference_video);
                    break;
                case 1:
                    addPreferencesFromResource(R.xml.preference_photo);
                    break;
                case 2:
                    addPreferencesFromResource(R.xml.preference_audio);
                    break;
                case 3:
                    addPreferencesFromResource(R.xml.preference_system);
                    break;
                case 4:
                    addPreferencesFromResource(R.xml.preference_user);
                    break;
                default:
                    addPreferencesFromResource(R.xml.preferences);
                    break;
            }
        }catch (Exception e) {
            // when use ota update, the default sharepreference may be cause error
            AppLog.e (TAG, "init error");
            resetAllSetting();
            getActivity().finish();
            return;

        }

		initSummary(getPreferenceScreen());

    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        AppLog.i (TAG, "key: "+key);
        updatePrefSummary(findPreference(key));
		setPreference(sharedPreferences,key);
		
    }

	private void resetPref(String key, String prf) {
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getActivity());
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(key,prf);
		editor.commit();
        //setPreferenceScreen(getPreferenceScreen());
	}

	private String checkPrefValid(String str, String key) {
		switch (key) {
			case SettingTags.KEY_PREF_VIDEO_SEGMENT:
				if ((!str.equals(getActivity().getString(R.string.ten_minutes)))&&(!str.equals((getActivity().getString(R.string.twenty_minutes))))&&(!str.equals(getActivity().getString(R.string.thirty_minutes)))) {
				    str = getActivity().getString(R.string.thirty_minutes);
					setVideoSegment(str);
                    resetPref(key,str);
				    return str;
                }
				break;

			case SettingTags.KEY_PREF_VIDEO_DELAY:
                if ((!str.equals(getActivity().getString(R.string.ten_seconds)))&&(!str.equals((getActivity().getString(R.string.twenty_seconds))))&&(!str.equals(getActivity().getString(R.string.thrity_second)))
                        &&(!str.equals(getActivity().getString(R.string.close)))) {
                    str = getActivity().getString(R.string.close);
					setVideoDelay(str);
                    resetPref(key,str);
                    return str;
                }
                break;
			case SettingTags.KEY_PREF_VIDEO_QUALITY:
                if ((!str.equals(getActivity().getString(R.string.normal)))&&(!str.equals((getActivity().getString(R.string.fine))))&&(!str.equals(getActivity().getString(R.string.excellent)))) {
                    str = getActivity().getString(R.string.normal);
					setVideoQuality(str);
                    resetPref(key,str);
                    return str;
                }
                break;
			case SettingTags.KEY_PREF_JPEG_QUALITY:
                if ((!str.equals(getActivity().getString(R.string.normal)))&&(!str.equals((getActivity().getString(R.string.fine))))&&(!str.equals(getActivity().getString(R.string.excellent)))) {
                    str = getActivity().getString(R.string.normal);
					setJpegQuality(str);
                    resetPref(key,str);
                    return str;
                }
                break;
			case SettingTags.KEY_PREF_JPEG_CONTINOUS:
                if ((!str.equals(getActivity().getString(R.string.three_times)))&&(!str.equals((getActivity().getString(R.string.five_times))))&&(!str.equals(getActivity().getString(R.string.ten_times)))
                        &&(!str.equals(getActivity().getString(R.string.close)))) {
                    str = getActivity().getString(R.string.close);
					setPicContinous(str);
                    resetPref(key,str);
                    return str;
                }
                break;
			case SettingTags.KEY_USBMODE:
                if ((!str.equals(getActivity().getString(R.string.usb_forbiden)))&&(!str.equals((getActivity().getString(R.string.usb_mode_cmd))))&&(!str.equals(getActivity().getString(R.string.usb_mode_udisk)))) {
                    str = getActivity().getString(R.string.usb_forbiden);
					setUSBMode(str);
                    resetPref(key,str);
                    return str;
                }
                break;
			case SettingTags.INFRARED:
                if ((!str.equals(getActivity().getString(R.string.auto)))&&(!str.equals((getActivity().getString(R.string.manual))))) {
                    str = getActivity().getString(R.string.manual);
					setInfraed(str);
                    resetPref(key,str);
                    return str;
                }
                break;
            //case SettingTags.KEY_VERSION:
            //    resetPref(key,BuildConfig.VERSION_NAME +"."+BuildConfig.BUILD_TIME);
            //break;

            /*case SettingTags.KEY_PREF_VIDEO_SIZE:
                if ((!str.equals(getActivity().getString(R.string.video_240p)))&&(!str.equals((getActivity().getString(R.string.video_480p))))&&(!str.equals(getActivity().getString(R.string.video_720p)))
                    &&(!str.equals(getActivity().getString(R.string.video_1080p)))) {
                    str = getActivity().getString(R.string.video_1080p);
                    setVideoSize(str);
                    return str;
                }
                break;*/
		}
		return str;
	}

	private void refrashDate() {
        String date = null;
        Calendar calendar = Calendar.getInstance();
        mYear = calendar.get(Calendar.YEAR);
        mMonth = calendar.get(Calendar.MONTH)+1;
        mDay = calendar.get(calendar.DAY_OF_MONTH);
    }

    private void refrashTime() {
        String date = null;
        Calendar calendar = Calendar.getInstance();
        mHour = calendar.get(Calendar.HOUR_OF_DAY);
        mMinute = calendar.get(Calendar.MINUTE);
    }
	

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        if (mSegId == 4) {
            final PreferenceWithTip dateTips = (PreferenceWithTip) getPreferenceScreen().findPreference(SettingTags.KEY_CURRENT_DATA);
            dateTips.setListener(new PreferenceWithTip.ViewBindListener() {
                @Override
                public void onViewBinded() {
                    if (dateTips.pTipView!=null) {
                        dateTips.pTipView.setText(TimeUtils.getSystemDate());
                    }
                }
            });

            final PreferenceWithTip timeTips = (PreferenceWithTip) getPreferenceScreen().findPreference(SettingTags.KEY_CURRENT_TIME);
            timeTips.setListener(new PreferenceWithTip.ViewBindListener() {
                @Override
                public void onViewBinded() {
                    if (timeTips.pTipView!=null) {
                        timeTips.pTipView.setText(TimeUtils.getSystemTime());
                        final Handler handler = new Handler();
                        handler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                timeTips.pTipView.setText(TimeUtils.getSystemTime());
                                handler.postDelayed(this,1000);
                            }
                        },1000);
                    }
                }
            });

            final PreferenceWithTip sysVer = (PreferenceWithTip)getPreferenceScreen().findPreference(SettingTags.KEY_VERSION);
            sysVer.setListener(new PreferenceWithTip.ViewBindListener() {
                @Override
                public void onViewBinded() {
                    if(sysVer.pTipView!=null) {
                        sysVer.pTipView.setText(BuildConfig.VERSION_NAME +"."+BuildConfig.BUILD_TIME);
                    }
                }
            });
        }
    }

    /**
     * z设置ListPreference的entry
     * @param pref
     */
    private void updatePrefSummary(Preference pref) {

		AppLog.i (TAG, "updatePrefSummary");

		/*
		if (pref.getKey().equals(SettingTags.KEY_CURRENT_DATA)) {
		    String date = getSystemDate();
		    pref.setSummary(date);
            //((PreferenceWithTip)pref).pTipView.setText(date);
            //pref.setDefaultValue(getSystemDate());
            //updataOsDate((PreferenceWithTip)pref);
			//resetPref(pref.getKey(),getSystemDate());
		    AppLog.i (TAG,"set date");
		    return;
        } else if (pref.getKey().equals(SettingTags.KEY_CURRENT_TIME)){
		    String tm = getSystemTime();
            pref.setSummary(tm);
            //((PreferenceWithTip)pref).pTipView.setText(tm);
			//resetPref(pref.getKey(),getSystemTime());

            AppLog.i(TAG, "set time");
            return;
        }*/
	
        if (pref instanceof ListPreference) {
            //CharSequence summary = ((ListPreference) pref).getEntry();
            String strPref = pref.getSharedPreferences().getString(pref.getKey(),"");
            //pref.setSummary(summary);           //entry就是sharedpreference里面保存的值
            strPref = checkPrefValid(strPref, pref.getKey());
            /*if (!ckStr.equals(strPref)) {
                strPref = ckStr;
                pref.setKey(strPref);
            }*/
            pref.setSummary(strPref);           //entry就是sharedpreference里面保存的值
            AppLog.i (TAG, "key: "+pref.getKey()+", val: "+strPref);
            //AppLog.i(TAG,"getOrder: "+pref.getOrder());
            //AppLog.i(TAG, "pref.getSharedPreferences().getString: "+string);
        }else if (pref != null )
        {
            if (!(pref instanceof TwoStatePreference)){//如果不是SwitchPreference或CheckBoxPreference
                // 则将他的summary写进去（SwitchPreference 继承TwoStatePreference）
                String summary = pref.getSharedPreferences().getString(pref.getKey(),"");
				summary = checkPrefValid(summary, pref.getKey());
				/*if (!ckStr.equals(summary)) {
				    summary = ckStr;
				    pref.setKey(summary);
                }*/
                pref.setSummary(summary);
                AppLog.i (TAG, "key: "+pref.getKey()+", sum: "+summary);
                //AppLog.i(TAG,"getOrder: "+pref.getOrder());
                //pref.setOrder();
            }
        }

    }

	/**
     * 遍历整个PreferenceScreen
     * a.如果是PreferenceGroup则获取里面的preference
     * b.如果Preference是ListPreference则设置他的summary
     * @param pref
     */
    private void initSummary(Preference pref) {//传进来的是PreferenceScreen
        if (pref instanceof PreferenceGroup) {
            PreferenceGroup prefGroup = (PreferenceGroup) pref;
            for (int i = 0; i < prefGroup.getPreferenceCount(); i++) {
                initSummary(prefGroup.getPreference(i));
            }
        } else {
            AppLog.i(TAG,"updatePrefSummary: "+pref.toString());
            updatePrefSummary(pref);
        }
    }

    /**
     *     by ycb
     *     此处加载有预加载机制，启动SettingActivity后，会首先加载两个页面，切换到最后一个页面，实际已经在前一个页面加载好了
     *     但此时获取 PreferenceWithTip 的 pTextView 却无法获取到
     * **/

    @Override
    public void onResume() {
        super.onResume();
        getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
        /*if (mSegId == 4) {
            PreferenceWithTip tips = (PreferenceWithTip) getPreferenceScreen().findPreference(SettingTags.KEY_CURRENT_DATA);
            tips.pTitleView.setText(getSystemDate());
            tips = (PreferenceWithTip) getPreferenceScreen().findPreference(SettingTags.KEY_CURRENT_TIME);
            tips.pTitleView.setText(getSystemTime());
        }*/
    }

    @Override
    public void onPause() {
        super.onPause();
        getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
    }

	private void setPreference(SharedPreferences sharedPreferences, String key) {
		switch (key) {
            case SettingTags.KEY_PREF_VIDEO_SIZE://视频分辨率
                setVideoSize(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_PREV_SIZE://预览设置
                setPreviewSize(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_VIDEO_DELAY://延时录像
                setVideoDelay(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_VIDEO_QUALITY://视频质量
                setVideoQuality(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_PIC_SIZE://图片大小，即分辨率
                setPictureSize(sharedPreferences.getString(key, ""));
               // setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_FOCUS_MODE://聚焦模式
                setFocusMode(sharedPreferences.getString(key, ""));
              //  setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_FLASH_MODE://散光灯
                setFlashMode(sharedPreferences.getString(key, ""));
              //  setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_WHITE_BALANCE://白平衡
                setWhiteBalance(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_SCENE_MODE://场景
                setSceneMode(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_EXPOS_COMP://曝光补偿
                setExposComp(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_JPEG_QUALITY://图片质量
                setJpegQuality(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_JPEG_CONTINOUS://连拍
                setPicContinous(sharedPreferences.getString(key, ""));

                break;
            case SettingTags.KEY_PREF_GPS_DATA://gps
//                setGpsData(sharedPreferences.getBoolean(key, false));
//                setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_JPEG_OUTPUTFORMAT://图片输出格式
                setPicFormart(sharedPreferences.getString(key, ""));
            //    setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_VIDEO_RATE://视频帧率
                setVideoRate(sharedPreferences.getString(key, ""));
              //  setCameraConfig(mCamera);
                break;
            case SettingTags.KEY_PREF_VIDEO_SEGMENT://分段录制
                setVideoSegment(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_VIDEO_PRERECORD://预录设置
                //setPrerecord(sharedPreferences.getString(key, ""));
                setPrerecord(sharedPreferences.getBoolean(key, false));
                break;
            case SettingTags.KEY_PREF_VIDEO_OUTPUTFORMAT://视频输出格式
                setVideoFormart(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_VIDEO_MOTIONDETECTION://移动侦测
                motiondetection(sharedPreferences.getBoolean(key, false));
                break;
            case SettingTags.KEY_PREF_VIDEO_MAXDURATION://视频最长录制时间
                setVideoMaxDuration(sharedPreferences.getString(key, ""));
                break;

            case SettingTags.KEY_PREF_AUDIO_SAMPLERATE://音频采样率
                setAudioSamlerate(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_AUDIO_OUTPUTFORMAT://音频输出格式
                setAudioFormart(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_PREF_AUDIO_MAXDURATION://最长录制时间
                setAudioMaxDuration(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_POLICE_NUMBER://警员编号
                 setPoliceNumber(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_DEV_NUMBER://设备编号
                setdevNumber(sharedPreferences.getString(key, ""));
                 break;
            case SettingTags.KEY_CURRENT_DATA://当前日期
                break;
            case SettingTags.KEY_CURRENT_TIME://当前时间
                break;
            case SettingTags.KEY_WIFI_DATA://wifi设置
              //setWifiData(sharedPreferences.getBoolean(key, false));
                break;
            case SettingTags.KEY_KEY_TONE://按键音设置
                setKeyTone(sharedPreferences.getBoolean(key, false));
                break;
            case SettingTags.KEY_VIBRTATE://震动设置
                setVibrate(sharedPreferences.getBoolean(key, false));
                break;
            case SettingTags.KEY_SYSTEM_LANGUAGE://设置系统语言
                setSystemLanguage(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_FILE_VIEW_TYPE://文件浏览模式
                setFileViewPattern(getActivity(),sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_USER_PASSWORD://密码
                setUserPassword(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_USER_NAME://用户名
                setUserName(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_TEXT_TO_SPEECH:
                textToSpeech();
                break;
            case SettingTags.KEY_SOS:
                setSOS(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_ADMIN_NAME:
                setAdminName(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_ADMIN_PWD:
                setAdminPwd(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_BULTOOTH:
                setBooth();
                break;
            case SettingTags.INFRARED:
                setInfraed(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_USBMODE:
                setUSBMode(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_VOLUME:
           //     setVolume(sharedPreferences.getString(key, ""));
                break;
            case SettingTags.KEY_VIDOE_OSD:
                setVideoAddOsd(sharedPreferences.getBoolean(key,false));
                break;
            case SettingTags.KEY_SCREEN_ON:
                setScreenOn(sharedPreferences.getBoolean(key,false));
                break;
        }
	}

    private void setUSBMode(String value) {
        Setup.USBMode = value;
        //AppLog.v(TAG, "===============>setUSBMode: "+value);
        try {
            StorageManager storageManager = (StorageManager)getActivity().getSystemService(Context.STORAGE_SERVICE);
            if (storageManager != null) {
                /*if (value.equals(getString(R.string.usb_mode_cmd))){
                    //AppLog.i (TAG, "USB设置模式");
                    AppLog.i(TAG,getString(R.string.usb_mode_cmd));
                    Settings.Global.putInt(getActivity().getContentResolver(),Settings.Global.ADB_ENABLED,1);
                    Method method = storageManager.getClass().getMethod("disableUsbMassStorage");
                    //Method method = storageManager.getClass().getMethod("aadbfagfa");
                    method.invoke(storageManager);
                } else */
                if (value.equals(getString(R.string.usb_mode_udisk))) {
                    //storageManager.enableUsbMassStorage();
                    AppLog.i(TAG,getString(R.string.usb_mode_udisk));
                    Settings.Global.putInt(getActivity().getContentResolver(),Settings.Global.ADB_ENABLED,0);
					Method method = storageManager.getClass().getMethod("isUsbMassStorageEnabled");   // 判断是否已打开
					boolean state = (boolean)method.invoke(storageManager);
					if (!state){
                    	method = storageManager.getClass().getMethod("enableUsbMassStorage");
                    	method.invoke(storageManager);
					}
                } else /*else if (value.equals(getString(R.string.usb_forbiden)))*/{
                    AppLog.i(TAG,getString(R.string.usb_forbiden));
                    //Settings.Global.putInt(getActivity().getContentResolver(),Settings.Global.ADB_ENABLED,0);
                    Method method = storageManager.getClass().getMethod("disableUsbMassStorage");
                    method.invoke(storageManager);
                }
            } else {
                AppLog.e (TAG, "Failed to get StorageManager");
            }
        } catch (Exception e) {
            AppLog.e(TAG, "set usb mode error, "+e.getMessage());
        }

    }
    //设置红外线的自动和手动
    private static void setInfraed(String value) {
        Setup.inFraRed = StringUtil.InfraRed(value);
        AppLog.v(TAG, "===============>setInfraed: "+value);
        AppLog.v(TAG, "===============> Setup.inFraRed: "+ Setup.inFraRed);

        if (Setup.inFraRed){
            EventBus.getDefault().post(new Automatic("自动"));

        }
    }

    public static void setInfraredPub(String value)
    {
        setInfraed(value);
    }

    private void setVideoQuality(String value) {

        Setup.VideoQuality  = StringUtil.Quality(value,true);
        AppLog.v(TAG, "setVideoQuality: =====>"+ Setup.VideoQuality);
    }

    private void setVideoDelay(String value) {

        int time = StringUtil.getSecFromStr(value);

        if (time==0){
            Setup.isVideoDelay = false;
        }else {
            Setup.isVideoDelay = true;
        }
        Setup.VideoDelay = time;

        AppLog.v(TAG, "setVideoDelay: "+Setup.isVideoDelay );
        AppLog.v(TAG, "setVideoDelay: "+ StringUtil.getSecFromStr(value) );
    }

    private void setVideoSize(String value) {
        if (value!=null){
            YuvUtils.releaseMemo();
            Setup.VideoWidth = Integer.parseInt(value.split(" ")[0].split("x")[0]);
            Setup.VideoHight = Integer.parseInt(value.split(" ")[0].split("x")[1]);
            CameraWrapper.SRC_IMAGE_WIDTH  = Setup.VideoWidth;
            CameraWrapper.SRC_IMAGE_HEIGHT = Setup.VideoHight;
            CameraWrapper.DST_IMAGE_WIDTH  = Setup.VideoWidth;
            CameraWrapper.DST_IMAGE_HEIGHT = Setup.VideoHight;
        }

    }

    private void setBooth() {
        startActivity(new
                Intent(Settings.ACTION_BLUETOOTH_SETTINGS));//直接进入手机中的蓝牙设置界面
    }




    /**
     * s设置管理员名字
     */
    private void setAdminName(String value) {

        Setup.adminName = value;
    }

    /**
     * 设置管理员密码
     */
    private void setAdminPwd(String value) {

        Setup.adminPasswd = value;

    }

    /**
     * 设置用户名
     */
    private static void setUserName(String value) {

        Setup.usrName = value;
    }

    /**
     * 设置密码
     */
    public static   void  setUserPassword(String value){

        Setup.usrPassword = value;
    }
    /**
     * 设置警员编号
     */
    private static void setPoliceNumber(String value){

        Setup.policeNumber = value;
    }
    /**
     * 设置设备编号
     */
    private static void setdevNumber(String value){

        Setup.devNum = value;
    }


    /**
     *
     * 设置sos
     * @param value
     */
    private static void setSOS(String value) {

        Setup.sos = Integer.parseInt(value);
    }


    /**
     * 设置文件浏览模式
     */
    private void setFileViewPattern(Context context, String value) {
        if (value.equals(R.string.file_view_mode)) {
            Setup.FileView = context.getString(R.string.file_view_mode);
        } else if (value.equals(R.string.picture_view_mode)) {
            Setup.FileView = context.getString(R.string.picture_view_mode);
        }
        /*
        switch (value){
            case StringUtil.FileView://文件类型
                Setup.FileView = StringUtil.FileView;
                break;
            case StringUtil.PicView://图片类型
                Setup.FileView = StringUtil.PicView;

                break;
        }*/
    }
    /**
     * 将预览的参数设置进摄像头
     * @param value
     */
    private static void setPreviewSize(String value) {


        if (value!=null){
            Setup.previewWidth = Integer.parseInt( value.split(" ")[0].split("x")[0]);
            Setup.previewHeight = Integer.parseInt(value.split(" ")[0].split("x")[1]);
        }
    }


    /**
     * 设置语音播报功能
     */
    private   void textToSpeech(){
        Intent intent = new Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS);
        startActivity(intent);

    }
    /**
     * 设置图片格式
     */
    private static void setPicFormart(String value){

        Setup.imgFilePostfix = "."+value;
    }


    /**
     * 设置视频帧率格式
     */
    private static void setVideoRate(String value){

        Setup.frameRate = Integer.parseInt(value);
        AppLog.v(TAG, "setVideoRate: ===================>"+Setup.frameRate );
    }
    /**
     * 设置视频分段录制时间
     */
    private static void setVideoSegment(String value){

        int time = StringUtil.getSecFromStr(value);
        if (time==0){
            //Setup.isSegmentedVideo = false;
            //time = 50;
            time = 30 * 60000;
        }/*else {
            Setup.isSegmentedVideo = true;
        }*/
        Setup.videoMaxDuration = time;
        //AppLog.v(TAG, "====================>isSegmentedVideo: "+ Setup.isSegmentedVideo);
        AppLog.v(TAG, "====================>setVideoSegment: "+ Setup.videoMaxDuration);
    }
    /**
     * 设置视频预录
     */
    private static void setPrerecord(String value){

        Setup.VideoPrerecord = StringUtil.switchState(value);
        AppLog.v("SettingFragment", "======================>setPrerecord: "+ Setup.VideoPrerecord );
        if (Setup.VideoPrerecord){
            Setup.videoPrerecordTime = PRE_RCD_TIME;
        }else {
            Setup.videoPrerecordTime = 0;
        }
        AppLog.v("SettingFragment", "======================>setPrerecord: "+ Setup.videoPrerecordTime );

    }

	private static void setPrerecord(boolean value) {
		Setup.VideoPrerecord = value;
		if (Setup.VideoPrerecord){
            Setup.videoPrerecordTime = PRE_RCD_TIME;
        }else {
            Setup.videoPrerecordTime = 0;
        }
        AppLog.v("SettingFragment", "======================>setPrerecord: "+ Setup.videoPrerecordTime );
		
	}
    /**
     * 设置视频输出格式
     */
    private static void setVideoFormart(String value){

        Setup.videoFilePostfix  = "."+value;
    }
    /**
     * 设置移动侦测
     */
    private static void motiondetection(boolean value){
        Setup.VideoMotionDetection= value;
    }
    /**
     * 设置视频最长录制时间
     */
    private static void setVideoMaxDuration(String value){


    }
    /**
     * 设置音频采样率
     */
    private static void setAudioSamlerate(String value){
        //  Setup.sampleRate = Integer.parseInt(value);
    }
    /**
     * 设置音频最长录制时间
     */
    private static void setAudioMaxDuration(String value){
        // Setup.MaxDuration =Integer.parseInt(value);
    }
    /**
     * 设置视频输出格式
     */
    private static void setAudioFormart(String value){

        Setup.soundFilePostfix = "."+value;
    }



    /**
     * 设置按键音
     */
    private static void setKeyTone(boolean value){

        Setup.KeyTone = value;
    }
    /**
     * 设置震动
     */
    private static void setVibrate(boolean value){

        Setup.isVibrator = value;
    }

    /**
     * 连拍设置
     * @param value
     */
    public static void setPicContinous(String value){

        int num = StringUtil.getDigstFromStr(value);
        if (num ==0){
            Setup.isContinuousShooting = false;
        }else {
            Setup.isContinuousShooting = true;
            Setup.continuousShootNum = num;
        }
    }



    private Configuration config;
    private DisplayMetrics dm;
    private Resources resources;

	 public void setlanguage()
    {
        //获取系统当前的语言
        String able= getResources().getConfiguration().locale.getLanguage();
        resources =getResources();//获得res资源对象  
        config = resources.getConfiguration();//获得设置对象  
        dm = resources.getDisplayMetrics();
        //根据系统语言进行设置
        if (able.equals("zh")) {
            config.locale = Locale.SIMPLIFIED_CHINESE;
            resources.updateConfiguration(config, dm);
            
        }
        else if(able.equals("en"))
        {
            config.locale = Locale.US;
            resources.updateConfiguration(config, dm);
        }
    }
    /**
     * 设置系统语言
     */
    private void setSystemLanguage(String value){
/*
        resources =getActivity().getResources();
        config = resources.getConfiguration();
        dm = resources.getDisplayMetrics();
        if (value.equals("Chinese")){
            config.locale = Locale.SIMPLIFIED_CHINESE;
        }else {
            config.locale = Locale.US;
        }
        resources.updateConfiguration(config, dm);*/
        try {
            Class<?> activityManagerNative = Class.forName("android.app.ActivityManagerNative");
            AppLog.i(TAG, "amnType: "+activityManagerNative.toString());

            Object am = activityManagerNative.getMethod("getDefault").invoke(activityManagerNative);
            AppLog.i(TAG, "amType: "+am.getClass().toString());

            Object config=am.getClass().getMethod("getConfiguration").invoke(am);
            AppLog.i(TAG,"configType: "+config.getClass().toString());

            AppLog.i(TAG,"set language: "+value);
            /*if (value.equals("Chinese"))*/
			if (value.equals("中文(简体)")){
                config.getClass().getDeclaredField("locale").set(config, Locale.CHINA);
            } else if (value.equals("한글")){
			    config.getClass().getDeclaredField("locale").set(config, Locale.KOREA);
            } else {
                config.getClass().getDeclaredField("locale").set(config,Locale.ENGLISH);
            }
            config.getClass().getDeclaredField("userSetLocale").setBoolean(config,true);
            am.getClass().getMethod("updateConfiguration",android.content.res.Configuration.class).invoke(am,config);

        } catch (Exception e) {
            e.printStackTrace();
            AppLog.e(TAG,"set language error: "+e.getMessage());
        }

		//setlanguage();
        PreferenceManager.setDefaultValues(getActivity(), R.xml.preferences, false);
        HomeItemLab.getInstance(getActivity()).refrash();

		// killProcess Activity.finish() 无法解决语言切换后选项没有切换的问题
        //android.os.Process.killProcess(android.os.Process.myPid());
        //getActivity().finish();

		/*SettingUtils.reset(getActivity());

		AppLog.d(TAG, "reset items");
		//initSummary(getPreferenceScreen());
		updatePrefSummary(findPreference("video_segmented"));
        updatePrefSummary(findPreference("video_delay"));
        updatePrefSummary(findPreference("video_quality"));
        updatePrefSummary(findPreference("jpeg_continous"));
        updatePrefSummary(findPreference("jpeg_quality"));
        updatePrefSummary(findPreference("fileview_type"));
        updatePrefSummary(findPreference("udisk"));
        updatePrefSummary(findPreference("InfraRed"));

        AppLog.d(TAG, "reset items ok");*/

        //Intent intent = new Intent(getActivity(),MainActivity.class);
        //startActivity(intent);
    }
    /**
     * 设置WiFi
     */
    private   void setWifiData2(Preference preference){


        //  WiFiUtil.openWiFi(context);
        //判断WiFi状态是否可用，如果可用的话就让该preference的summary设置为打开或关闭

        Intent intent = new Intent(Settings.ACTION_WIFI_SETTINGS);
        startActivity(intent);

    }

    private void updataWifiState(PreferenceWithTip prefer)
    {
        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }

        if (!WiFiUtil.wifiIsOpen(settingContext)){
            AppLog.v(TAG, "updataWifiState:11 ============== "+WiFiUtil.wifiIsOpen(settingContext) );
            prefer.pTipView.setText(R.string.close);
        }else {
            prefer.pTipView.setText(R.string.open);
            AppLog.v(TAG, "updataWifiState:22 ============== "+WiFiUtil.wifiIsOpen(settingContext) );
        }

    }

    private void updataGpsState(PreferenceWithTip prefer) {

        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }

        if (!GpsUtils.isOPen(settingContext)){
            prefer.pTipView.setText(R.string.close);
            prefer.setSummary(R.string.close);
        }else {
            prefer.pTipView.setText(R.string.open);
            prefer.setSummary(R.string.open);

        }

    }
    private void updataBluetoothAdapter(PreferenceWithTip prefer) {

        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }

        if (!BluetoothUtil.isOpenBluetooth()){
            prefer.pTipView.setText(R.string.close);
        }else {
            prefer.pTipView.setText(R.string.open);
        }
    }
    private void updataOsDate(PreferenceWithTip prefer,String str_date) {
        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }
        prefer.pTipView.setText(str_date);
    }

    private void upDataOsTime(PreferenceWithTip prefer,String str_time) {
        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }
        prefer.pTipView.setText(str_time);

    }
    /**
     * 将图片尺寸参数设置进摄像头
     * @param value
     */
    private static void setPictureSize(String value) {
//        String[] split = value.split("x");//以x为分割符号进行分割字符串
//        int width = Integer.parseInt(split[0]);
//        int height = Integer.parseInt(split[1]);
//        mParameters.setPictureSize(width,height);

        List<String> resolution =  StringUtil.getBracketsValue(value);
        Setup.picturewidht = Integer.parseInt(resolution.get(0).split("x")[0]);
        Setup.pictureheight =Integer.parseInt(resolution.get(0).split("x")[1]);
        AppLog.v(TAG, "setPictureSize:=========> "+Setup.picturewidht+"  "+Setup.pictureheight );
    }
    /**
     * 将聚焦的参数设置进摄像头
     * @param value
     */
    private static void setFocusMode(String value) {

        mParameters.setFocusMode(value);
    }
    /**
     * 将散光灯的参数设置进摄像头
     * @param value
     */
    private static void setFlashMode(String value) {

        mParameters.setFlashMode(value);
    }
    /**
     * 将白平衡的参数设置进摄像头
     * @param value
     */
    private static void setWhiteBalance(String value) {

        mParameters.setWhiteBalance(value);
    }
    /**
     * 将场景的参数设置进摄像头
     * @param value
     */
    private static void setSceneMode(String value) {

        mParameters.setSceneMode(value);
    }
    /**
     * 将曝光的参数设置进摄像头
     * @param value
     */
    private static void setExposComp(String value) {


        // mParameters.setExposureCompensation(Integer.parseInt(value));
        //mParameters.setExposureCompensation(StringUtil.getDigstFromStr(value));
		Setup.ExposureCompensation = StringUtil.getDigstFromStr(value);
    }
    /**
     * 将图片的质量的参数设置进摄像头
     * @param value
     */
    private static void setJpegQuality(String value) {

        // mParameters.setJpegQuality(Integer.parseInt(value));
        //mParameters.setJpegQuality(StringUtil.Quality(value,false));
		Setup.PictureQuality = StringUtil.Quality(value,false);
    }

    /**
     * 将Gps的参数设置进摄像头
     * @param value
     */
    private  static void setGpsData(Boolean value) {

        if (value.equals(false)) {
            mParameters.removeGpsData();
        }
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
		 String key = preference.getKey();

         switch ( key){
             case SettingTags.KEY_CURRENT_DATA:
                 setDate(preference);
                  break;
             case SettingTags.KEY_CURRENT_TIME:
                 setTime(preference);
                 break;
             case SettingTags.KEY_BRIGHTNESS:
                 setBritness();
                 break;
             case SettingTags.KEY_RESET:
                 showResetDialog(getActivity());
                 break;
             case SettingTags.KEY_FORMAT_SD:
			 	 showFormatSdDialog(getActivity());
                 break;
             case SettingTags.KEY_UPDATE_SYSTEM:
                 showUpdateSystemDialog(getActivity());
                 break;
             case SettingTags.KEY_WIFI_DATA:
                 setWifiData2(preference);
                 break;
             case SettingTags.KEY_BULTOOTH:
                 setBooth();
                 break;
             case SettingTags.KEY_PREF_GPS_DATA2:
                 setGps(preference);
                 break;
             case SettingTags.KEY_APN_SETTING:
                 setAPN(preference);
                 break;
         }	
         return super.onPreferenceTreeClick(preferenceScreen, preference);
         //return true;
    }

    private void setDate(final Preference pref){
        final DateTimeDialog dialog1 = new DateTimeDialog(getActivity(), R.layout.date);
        final DatePicker dataPicker = (DatePicker) dialog1.getPicker();
        refrashDate();    // 获取当前日期
        dataPicker.init(mYear, mMonth-1, mDay, new DatePicker.OnDateChangedListener() {
            @Override
            public void onDateChanged(DatePicker view, int year, int monthOfYear, int dayOfMonth) {

                mYear = year;
                mMonth = monthOfYear+1;
                mDay = dayOfMonth;
                dialog1.setTitle(getString(R.string.set_date_tips)+" "+year+"/"+mMonth+"/"+mDay);
            }
        });
        dialog1.setOnPositiveListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TimeUtils.onDateSet( mYear, mMonth, mDay);
                //mCurrDate = mYear+"/"+mMonth+"/"+mDay;
                updataOsDate((PreferenceWithTip) pref, String.format(mYear+"/"+mMonth+"/"+mDay));
                dialog1.dismiss();
            }
        });
        dialog1.setOnNegativeListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                dialog1.dismiss();
            }
        });
        dialog1.show();
        dialog1.setTitle(getString(R.string.set_date_tips)+" "+TimeUtils.getSystemDate());
    }


    private void setTime(final Preference pref){
        refrashTime();
        final DateTimeDialog dialog = new DateTimeDialog(getActivity(), R.layout.time);
        TimePicker timePicker = (TimePicker) dialog.getPicker();
        timePicker.setOnTimeChangedListener(new TimePicker.OnTimeChangedListener() {
            @Override
            public void onTimeChanged(TimePicker view, int hourOfDay, int minute) {
                SettingSegFragment.this.mHour = hourOfDay;
                SettingSegFragment.this.mMinute = minute;
                dialog.setTitle(getString(R.string.set_time_tips)+" "+hourOfDay+":"+minute);
            }
        });


        dialog.setOnPositiveListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TimeUtils.onTimeSet(mHour, mMinute);
                //mCurrTime = mHour+":"+mMinute;
                upDataOsTime((PreferenceWithTip) pref,String.format(mHour+":"+mMinute));
                dialog.dismiss();
            }
        });
        dialog.setOnNegativeListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.dismiss();
            }
        });
        dialog.show();
        dialog.setTitle(getString(R.string.set_time_tips)+" "+mHour+":"+mMinute);

    }
    private void setBritness(){
        Intent intent =  new Intent(Settings.ACTION_DISPLAY_SETTINGS);
        startActivity(intent);

    }

    private void setGps(Preference pref){
        Intent i = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
        getActivity().startActivity(i);
        //  updataGpsState((PreferenceWithTip) pref);
    }

    private void setAPN(Preference pref) {
        Intent intent =  new Intent(Settings.ACTION_SETTINGS);
        startActivity(intent);
        //startActivity(new Intent(android.provider.Settings.ACTION_APN_SETTINGS));
    }

/*
    public static void setCameraConfig(Camera mCamera){
        if (mCamera !=null){
            //设置完参数从新初始化摄像头
            mCamera.stopPreview();
            mCamera.setParameters(mParameters);
            mCamera.startPreview();
        }
    }*/

    private static void setVideoAddOsd(boolean videoAddOsd) {
        VideoRunnable.isAddOSD = videoAddOsd;
    }

    public void setScreenOn(boolean screenOn) {
        Setup.screenOn = screenOn;
    }

	private void showResetDialog(Context context){
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(R.string.restore_factory_tips);
        builder.setMessage(R.string.restore_factory_note);
        builder.setCancelable(true);
        builder.setPositiveButton(R.string.confirm, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              //  SettingUtils.reset(getActivity());
                new Thread(){
                    @Override
                    public void run(){
                        SdCardFormat.diskFormat(getActivity());
                    }
                }.start();

                dialog.dismiss();
            }
        });
        //builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        Dialog dialog = builder.create();
        dialog.show();

    }

    // 设置ota progressDialog
    /*
    private ProgressDialog progressDialog;
    private void setProgressBar(String title){
        try{
            progressDialog = ProgressDialog.show(getActivity(),null, title);
            progressDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
                @Override
                public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                    if (keyCode == KeyEvent.KEYCODE_BACK){
                        clrProgressDialog();
                    }
                    return false;
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void clrProgressDialog(){
        if (progressDialog!=null){
            progressDialog.dismiss();
        }
    }*/

	private boolean checkInput(String password){
        if(password==null||password.length()<6){
            Toast.makeText(getActivity(), R.string.key_format_error_tips, Toast.LENGTH_SHORT).show();
            return false;
        }
        if (!password.equals(SettingUtils.getStrValues(getActivity(),"usrPassword","111111"))){
            Toast.makeText(getActivity(), R.string.key_error_tips, Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

    private void showFormatSdDialog(Context context){
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(R.string.format_sdcard_tips);
        builder.setMessage(R.string.format_sdcard_notes);
        builder.setCancelable(true);

        builder.setPositiveButton(R.string.confirm, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                //  SettingUtils.reset(getActivity());
                final CustomDialog dlg = new CustomDialog(getActivity());
                final EditText content = (EditText)dlg.getEditText();
                dlg.setOnPositiveListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v){
                        dlg.dismiss();
                    }
                });
                dlg.setOnNegativeListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v){
                        String str = content.getText().toString().trim();
                        if (checkInput(str)){

                            new Thread(){
                                @Override
                                public void run(){
                                    try {
                                        SdCardFormat.diskFormat(getActivity());
                                        // 格式化SD卡后，禁止USB，此处为方便出厂设置，ycb
                                        Settings.Global.putInt(getActivity().getContentResolver(),Settings.Global.ADB_ENABLED,0);
                                        StorageManager storageManager = (StorageManager)getActivity().getSystemService(Context.STORAGE_SERVICE);
                                        if (storageManager!=null){
                                            Method method = storageManager.getClass().getMethod("disableUsbMassStorage");
                                            method.invoke(storageManager);
                                        }
                                        // 设置usb选项
                                        PreferenceManager.getDefaultSharedPreferences(getActivity())
                                                .edit()
                                                .putString("udisk",getActivity().getString(R.string.usb_forbiden))
                                                .commit();

                                    } catch (Exception e){
                                        Toast.makeText(getActivity(),R.string.format_sdcar_error_tips,Toast.LENGTH_SHORT).show();
                                    }

                                }
                            }.start();
                        } else {
                            Toast.makeText(getActivity(),R.string.key_error_tips,Toast.LENGTH_SHORT).show();
                        }
                        dlg.dismiss();
                    }
                });

                dialog.dismiss();
                dlg.show();
            }
        });
        //builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
        builder.setNegativeButton(getActivity().getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        Dialog dialog = builder.create();
        dialog.show();
    }

    private void showUpdateSystemDialog(Context context){
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(R.string.update_system_tips);
        builder.setMessage(R.string.update_system_message);
        builder.setCancelable(true);
/*
        final Handler otaHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case OtaUpdate.MSG_SHWO_TOAST:
                        String str = (String) msg.obj;
                        Toast.makeText(getActivity(),str, Toast.LENGTH_SHORT).show();
                        clrProgressDialog();
                        break;
                    case OtaUpdate.MSG_PRO_DIA:
                        str = (String) msg.obj;
                        setProgressBar(str);
                        break;
                    case OtaUpdate.MSG_CLR_DIA:
                        clrProgressDialog();
                        break;
                }
            }
        };*/

        builder.setPositiveButton(getActivity().getResources().getString(R.string.confirm), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                AppLog.i(TAG, "开始升级系统");
                new Thread(){
                    @Override
                    public void run(){
                        //new OtaUpdate(getActivity(),otaHandler).beginOtaProcess();
                        //new OtaUpdate(getActivity()).beginOtaProcess();
                        Intent intent = new Intent(getActivity(), OtaUpdateActivity.class);
                        intent.putExtra(OtaUpdateActivity.PACKAGE_PATH, Environment.getExternalStorageDirectory().toString()+
                                File.separator+OtaUpdateActivity.OTA_FILE);
                        startActivity(intent);

                    }
                }.start();
            }
        });

        //builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
		builder.setNegativeButton(getActivity().getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        Dialog dialog = builder.create();
        dialog.show();
    }

    private void resetAllSetting() {
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getActivity());
        SharedPreferences.Editor editor = sp.edit();
        editor.clear();
        editor.commit();
    }
	
}
