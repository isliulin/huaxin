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
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.preference.TwoStatePreference;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.DatePicker;
import android.widget.TimePicker;

import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.rencoder.VideoRunnable;

import org.greenrobot.eventbus.EventBus;

import java.util.Calendar;
import java.util.List;
import java.util.Locale;

import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.Automatic;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.BluetoothUtil;
import gbpe.policeass.utils.SdCardFormat;
import gbpe.policeass.utils.StringUtil;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.utils.WiFiUtil;
import gbpe.policeass.utils.gps.GpsUtils;
import gbpe.policeass.views.DateTimeDialog;
import gbpe.policeass.views.PreferenceWithTip;
import tech.shutu.jni.YuvUtils;


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
 * 描述： 设置信息相关，所有的参数设置
 * =====================================================================================
 */
public class SettingFragment extends PreferenceFragment  implements SharedPreferences.OnSharedPreferenceChangeListener{

    public static final String TAG= "SettingFragment";
    public static final String SEG_INDEX_TAG = "setIndexTag";
	// add by ycb
	private static final int PRE_RCD_TIME = 10;
    /**
     * 视频的
     */
    public static final String KEY_PREF_VIDEO_SIZE = "video_size";//视频分辨率
    public static final String KEY_PREF_VIDEO_RATE = "video_rate";//视频帧率
    public static final String KEY_PREF_VIDEO_SEGMENT= "video_segmented";//分段录制
    public static final String KEY_PREF_VIDEO_PRERECORD = "video_prerecord";//预录设置
    public static final String KEY_PREF_VIDEO_OUTPUTFORMAT = "video_outputformat";//视频输出格式
    public static final String KEY_PREF_VIDEO_MOTIONDETECTION = "video_motiondetection";//移动侦测
    public static final String KEY_PREF_VIDEO_MAXDURATION = "video_maxduration";//移动侦测
    public static final String KEY_PREF_VIDEO_DELAY = "video_delay";//延时录像
    public static final String KEY_PREF_VIDEO_QUALITY = "video_quality";//视频质量


    /**
     * 图片大小
     */
    public static final String KEY_PREF_PREV_SIZE = "preview_size";//预览
    public static final String KEY_PREF_PIC_SIZE = "picture_size";//图片大小
    public static final String KEY_PREF_FLASH_MODE = "flash_mode";//散光灯模式
    public static final String KEY_PREF_FOCUS_MODE = "focus_mode";//聚焦模式
    public static final String KEY_PREF_WHITE_BALANCE = "white_balance";//白平衡
    public static final String KEY_PREF_SCENE_MODE = "scene_mode";//情景模式

    public static final String KEY_PREF_EXPOS_COMP = "exposure_compensation";//曝光补偿
    public static final String KEY_PREF_JPEG_QUALITY = "jpeg_quality";//图片质量
    public static final String KEY_PREF_JPEG_OUTPUTFORMAT = "jpeg_outputformat";//图片质量
    public static final String KEY_PREF_JPEG_CONTINOUS = "jpeg_continous";//连拍

    /**
     * 音频设置
     */
    public static final String KEY_PREF_AUDIO_SAMPLERATE = "audio_samplerate";//音频采样率
    public static final String KEY_PREF_AUDIO_OUTPUTFORMAT = "audio_outputformat";//音频输出格式
    public static final String KEY_PREF_AUDIO_MAXDURATION= "audio_maxduration";//最长录制时间

    /**
     * 个人设置
     */

    public static final String KEY_POLICE_NUMBER = "policeNumber";//警员编号
    public static final String KEY_DEV_NUMBER = "devNumber";//设备编号
    public static final String KEY_CURRENT_DATA = "current_data";//当前日期
    public static final String KEY_CURRENT_TIME  ="current_time";//当前时间
    public static final String KEY_USER_PASSWORD = "usrPassword";//用户密码
    public static final String KEY_USER_NAME="usrName";//用户名

    public static final String KEY_ADMIN_PWD ="AdminPassword";

    public static final String KEY_ADMIN_NAME = "AdminName";



    /**
     * 系统设置
     */
    public static final String KEY_PREF_GPS_DATA = "gps_data";//Gps
    public static final String KEY_PREF_GPS_DATA2 = "gps_data2";//Gps

    public static final String KEY_WIFI_DATA = "wifi_data";
    public static final String KEY_KEY_TONE = "key_tone";//按键音
    public static final String KEY_VIBRTATE = "vibrate";//震动
    public static final String KEY_SYSTEM_LANGUAGE = "LanguageZfy";//语言
    public static final String KEY_FILE_VIEW_TYPE = "fileview_type";//文件类型
    public static final String KEY_RESET = "reset";//出厂设置
    public static final String KEY_BRIGHTNESS = "brightness";//亮度
    public static final String KEY_TEXT_TO_SPEECH = "text_to_speech";//语音播报设置

    public static final String KEY_SOS = "sos";

    public static final String KEY_BULTOOTH="blutooth";


    public static final String KEY_COUSTOM = "coustom";

    public static final String INFRARED = "InfraRed";//红外线

    public static final String KEY_USBMODE = "udisk"; //U盘设置

    public static final String KEY_VOLUME = "Volume";

    public static final String KEY_VIDOE_OSD = "video_osd";
    public static final String KEY_PIC_OSD = "pic_osd";
    
    public static final String KEY_SCREEN_ON = "screenOn";


    //-------------------------------------------------------------------------------------------------------
    static Camera mCamera;
    static Camera.Parameters mParameters;

    private static int mPagerIndex = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.preferences);
        //读取Android设备默认的支持的尺寸并将这些尺寸填充进perference的entrys和entryvalues中
        if (mCamera!=null&&mParameters !=null) {

       //     loadSupportedPreviewSize();
       //     loadSupportedPictureSize();
        //    loadSupportedVideoeSize();
            // delete by ycb,
       /*
            loadSupportedFlashMode();
            loadSupportedFocusMode();
            loadSupportedWhiteBalance();
            loadSupportedSceneMode();
        */
         //   loadSupportedExposeCompensation();
        }
        initSummary(getPreferenceScreen());
        initCalendar();

        AppLog.i(TAG, "on create");
      }

    /**
     * 传一个摄像头的实例进来
     * @param camera
     */
    public static void passCamera(Camera camera) {
        mCamera = camera;
        if (mCamera !=null){
            mParameters = camera.getParameters();
        }
    }

    /**
     * 设置设备默认的参数
     * @param sharedPrefs
     */
     /*
    public static void setDefault(SharedPreferences sharedPrefs) {
        String valPreviewSize = sharedPrefs.getString(KEY_PREF_PREV_SIZE, null);
        if (valPreviewSize == null) {//如果预览尺寸等于null的时候才执行
            SharedPreferences.Editor editor = sharedPrefs.edit();
            editor.putString(KEY_PREF_PREV_SIZE, getDefaultPreviewSize());
          //  editor.putString(KEY_PREF_PIC_SIZE, getDefaultPictureSize());
            editor.putString(KEY_PREF_VIDEO_SIZE, getDefaultVideoSize());
            editor.putString(KEY_PREF_FOCUS_MODE, getDefaultFocusMode());
            editor.apply();
        }
    }*/

    /**
     *从设备中获取默认的参数
     * @return
     */
     /*
    private static String getDefaultPreviewSize() {
        Camera.Size previewSize = mParameters.getPreviewSize();
        return previewSize.width + "x" + previewSize.height;
    }

    private static String getDefaultPictureSize() {
        Camera.Size pictureSize = mParameters.getPictureSize();
        return pictureSize.width + "x" + pictureSize.height;
    }

    private static String getDefaultVideoSize() {
        Camera.Size VideoSize = mParameters.getPreferredPreviewSizeForVideo();
        return VideoSize.width + "x" + VideoSize.height;
    }

    private static String getDefaultFocusMode() {
        List<String> supportedFocusModes = mParameters.getSupportedFocusModes();
        if (supportedFocusModes.contains("continuous-picture")) {
            return "continuous-picture";
        }
        return "continuous-video";
    }*/



    /**
     * 将之前用户设置的配置参数设置到摄像头里面
     * @param sharedPref
     */
     /*
    public static void init(SharedPreferences sharedPref) {
        setPreviewSize(sharedPref.getString(KEY_PREF_PREV_SIZE, ""));
        setPictureSize(sharedPref.getString(KEY_PREF_PIC_SIZE, ""));
        setFlashMode(sharedPref.getString(KEY_PREF_FLASH_MODE, ""));
        setFocusMode(sharedPref.getString(KEY_PREF_FOCUS_MODE, ""));
        setWhiteBalance(sharedPref.getString(KEY_PREF_WHITE_BALANCE, ""));
        setSceneMode(sharedPref.getString(KEY_PREF_SCENE_MODE, ""));
        setExposComp(sharedPref.getString(KEY_PREF_EXPOS_COMP, ""));
        setJpegQuality(sharedPref.getString(KEY_PREF_JPEG_QUALITY, ""));
      //  setGpsData(sharedPref.getBoolean(KEY_PREF_GPS_DATA, false));



        setPicFormart(sharedPref.getString(KEY_PREF_JPEG_OUTPUTFORMAT,""));
        setVideoRate(sharedPref.getString(KEY_PREF_VIDEO_RATE,""));
        setVideoSegment(sharedPref.getString(KEY_PREF_VIDEO_SEGMENT,""));
        setPrerecord(sharedPref.getString(KEY_PREF_VIDEO_PRERECORD,""));
        setVideoFormart(sharedPref.getString(KEY_PREF_VIDEO_OUTPUTFORMAT,""));

		// 没有实现
        setVideoMaxDuration(sharedPref.getString(KEY_PREF_VIDEO_MAXDURATION,""));
        setAudioSamlerate(sharedPref.getString(KEY_PREF_AUDIO_SAMPLERATE,""));
        setAudioMaxDuration(sharedPref.getString(KEY_PREF_AUDIO_MAXDURATION,""));
        setAudioFormart(sharedPref.getString(KEY_PREF_AUDIO_OUTPUTFORMAT,""));
//
        setPoliceNumber(sharedPref.getString(KEY_POLICE_NUMBER,""));
        setdevNumber(sharedPref.getString(KEY_DEV_NUMBER,""));
        setUserPassword(sharedPref.getString(KEY_USER_PASSWORD,""));
        setUserName(sharedPref.getString(KEY_USER_NAME,""));
//
        setKeyTone(sharedPref.getBoolean(KEY_KEY_TONE,true));
        setVibrate(sharedPref.getBoolean(KEY_VIBRTATE,false));
//
        setSOS(sharedPref.getString(KEY_SOS,""));
        setVideoAddOsd(sharedPref.getBoolean(KEY_VIDOE_OSD,true));

		setInfraed(sharedPref.getString(INFRARED, ""));


        //setSystemLanguage(sharedPref.getString(KEY_SYSTEM_LANGUAGE,""));

//        setPicContinous(sharedPref.getString(KEY_PREF_JPEG_CONTINOUS,""));
        //----------------------------------------------------------------------------------------------
        mCamera.stopPreview();
        mCamera.setParameters(mParameters);
        mCamera.startPreview();
       // setCameraConfig(mCamera);
    }*/

    /**
     * 将预览的参数设置到preference item中去
     */
     /*
    private void loadSupportedPreviewSize() {
        cameraSizeListToListPreference(mParameters.getSupportedPreviewSizes(), KEY_PREF_PREV_SIZE);
    }*/

    /**
     *   将图片分辨率的参数设置到preference item中去
     */
     /*
    private void loadSupportedPictureSize() {
        cameraSizeListToListPreference(mParameters.getSupportedPictureSizes(), KEY_PREF_PIC_SIZE);
    }*/

    /**
     * 将视频分辨率 分辨率的参数设置到preference item中去
     */
     /*
    private void loadSupportedVideoeSize() {
        cameraSizeListToListPreference(mParameters.getSupportedVideoSizes(), KEY_PREF_VIDEO_SIZE);
    }*/

    /**
     *  将散光灯模式的参数设置到preference item中去
     */
     /*
    private void loadSupportedFlashMode() {
        stringListToListPreference(mParameters.getSupportedFlashModes(), KEY_PREF_FLASH_MODE);
    }*/


    /**
     *  将聚焦模式的参数设置到preference item中去
     */
     /*
    private void loadSupportedFocusMode() {
        stringListToListPreference(mParameters.getSupportedFocusModes(), KEY_PREF_FOCUS_MODE);
    }*/

    /**
     *   将白平衡模式的参数设置到preference item中去
     */
     /*
    private void loadSupportedWhiteBalance() {
        stringListToListPreference(mParameters.getSupportedWhiteBalance(), KEY_PREF_WHITE_BALANCE);
    }*/

    /**
     *   将加载场景模式的参数设置到preference item中去
     */
     /*
    private void loadSupportedSceneMode() {
        stringListToListPreference(mParameters.getSupportedSceneModes(), KEY_PREF_SCENE_MODE);
    }*/

    /**
     *   将加载曝光补偿模式的参数设置到preference item中去
     */
     /*
    private void loadSupportedExposeCompensation() {
        int minExposComp = mParameters.getMinExposureCompensation();
        int maxExposComp = mParameters.getMaxExposureCompensation();
        List<String> exposComp = new ArrayList<>();
        for (int value = minExposComp; value <= maxExposComp; value++) {
            exposComp.add(Integer.toString(value));
        }
        stringListToListPreference(exposComp, KEY_PREF_EXPOS_COMP);
    }*/



	/*
    private void cameraSizeListToListPreference(List<Camera.Size> list, String key) {
        List<String> stringList = new ArrayList<>();
        for (Camera.Size size : list) {
            String stringSize = size.width + "x" + size.height;
            stringList.add(stringSize);
        }
        stringListToListPreference(stringList, key);
    }*/


    /**
     * 将不同的preference item设置到对应的key所对应的entrys 和entryvalues中
     * @param list
     * @param key
     */
     /*
    private void stringListToListPreference(List<String> list, String key) {
        final CharSequence[] charSeq = list.toArray(new CharSequence[list.size()]);
        ListPreference listPref = (ListPreference) getPreferenceScreen().findPreference(key);
        listPref.setEntries(charSeq);//值是数组，空件预显示的文本
        listPref.setEntryValues(charSeq);//值是键值对，值是entries对应的值
    }*/




    /**
     * 当preference的条目数发生变化时，回调下面的方法
     * 更改对应的键值对
     * @param sharedPreferences
     * @param key
     */
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        updatePrefSummary(findPreference(key));

		AppLog.i (TAG, key+"changed");
		
        switch (key) {
            case KEY_PREF_VIDEO_SIZE://视频分辨率
                setVideoSize(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_PREV_SIZE://预览设置
                setPreviewSize(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case KEY_PREF_VIDEO_DELAY://延时录像
                setVideoDelay(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_VIDEO_QUALITY://视频质量
                setVideoQuality(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_PIC_SIZE://图片大小，即分辨率
                setPictureSize(sharedPreferences.getString(key, ""));
               // setCameraConfig(mCamera);
                break;
            case KEY_PREF_FOCUS_MODE://聚焦模式
                setFocusMode(sharedPreferences.getString(key, ""));
              //  setCameraConfig(mCamera);
                break;
            case KEY_PREF_FLASH_MODE://散光灯
                setFlashMode(sharedPreferences.getString(key, ""));
              //  setCameraConfig(mCamera);
                break;
            case KEY_PREF_WHITE_BALANCE://白平衡
                setWhiteBalance(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case KEY_PREF_SCENE_MODE://场景
                setSceneMode(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case KEY_PREF_EXPOS_COMP://曝光补偿
                setExposComp(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case KEY_PREF_JPEG_QUALITY://图片质量
                setJpegQuality(sharedPreferences.getString(key, ""));
             //   setCameraConfig(mCamera);
                break;
            case KEY_PREF_JPEG_CONTINOUS://连拍
                setPicContinous(sharedPreferences.getString(key, ""));

                break;
            case KEY_PREF_GPS_DATA://gps
//                setGpsData(sharedPreferences.getBoolean(key, false));
//                setCameraConfig(mCamera);
                break;
            case KEY_PREF_JPEG_OUTPUTFORMAT://图片输出格式
                setPicFormart(sharedPreferences.getString(key, ""));
            //    setCameraConfig(mCamera);
                break;
            case KEY_PREF_VIDEO_RATE://视频帧率
                setVideoRate(sharedPreferences.getString(key, ""));
                //setCameraConfig(mCamera);
                break;
            case KEY_PREF_VIDEO_SEGMENT://分段录制
                setVideoSegment(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_VIDEO_PRERECORD://预录设置
                //setPrerecord(sharedPreferences.getString(key, ""));
                setPrerecord(sharedPreferences.getBoolean(key, false));
                break;
            case KEY_PREF_VIDEO_OUTPUTFORMAT://视频输出格式
                setVideoFormart(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_VIDEO_MOTIONDETECTION://移动侦测
                motiondetection(sharedPreferences.getBoolean(key, false));
                break;
            case KEY_PREF_VIDEO_MAXDURATION://视频最长录制时间
                setVideoMaxDuration(sharedPreferences.getString(key, ""));
                break;

            case KEY_PREF_AUDIO_SAMPLERATE://音频采样率
                setAudioSamlerate(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_AUDIO_OUTPUTFORMAT://音频输出格式
                setAudioFormart(sharedPreferences.getString(key, ""));
                break;
            case KEY_PREF_AUDIO_MAXDURATION://最长录制时间
                setAudioMaxDuration(sharedPreferences.getString(key, ""));
                break;
            case KEY_POLICE_NUMBER://警员编号
                 setPoliceNumber(sharedPreferences.getString(key, ""));
                break;
            case KEY_DEV_NUMBER://设备编号
                setdevNumber(sharedPreferences.getString(key, ""));
                 break;
            case KEY_CURRENT_DATA://当前日期
                break;
            case KEY_CURRENT_TIME://当前时间
                break;
            case KEY_WIFI_DATA://wifi设置
              //setWifiData(sharedPreferences.getBoolean(key, false));
                break;
            case KEY_KEY_TONE://按键音设置
                setKeyTone(sharedPreferences.getBoolean(key, false));
                break;
            case KEY_VIBRTATE://震动设置
                setVibrate(sharedPreferences.getBoolean(key, false));
                break;
            case KEY_SYSTEM_LANGUAGE://设置系统语言
                setSystemLanguage(sharedPreferences.getString(key, ""));
                break;
            case KEY_FILE_VIEW_TYPE://文件浏览模式
                setFileViewPattern(sharedPreferences.getString(key, ""));
                break;
            case KEY_USER_PASSWORD://密码
                setUserPassword(sharedPreferences.getString(key, ""));
                break;
            case KEY_USER_NAME://用户名
                setUserName(sharedPreferences.getString(key, ""));
                break;
            case KEY_TEXT_TO_SPEECH:
                textToSpeech();
                break;
            case KEY_SOS:
                setSOS(sharedPreferences.getString(key, ""));
                break;
            case KEY_ADMIN_NAME:
                setAdminName(sharedPreferences.getString(key, ""));
                break;
            case KEY_ADMIN_PWD:
                setAdminPwd(sharedPreferences.getString(key, ""));
                break;
            case KEY_BULTOOTH:
                setBooth();
                break;
            case INFRARED:
                setInfraed(sharedPreferences.getString(key, ""));
                break;
            case KEY_USBMODE:
                setUSBMode(sharedPreferences.getString(key, ""));
                break;
            case KEY_VOLUME:
           //     setVolume(sharedPreferences.getString(key, ""));
                break;
            case KEY_VIDOE_OSD:
                setVideoAddOsd(sharedPreferences.getBoolean(key,false));
                break;
            case KEY_SCREEN_ON:
                setScreenOn(sharedPreferences.getBoolean(key,false));
                break;


        }
//        if (mCamera !=null){
//            //设置完参数从新初始化摄像头
//            mCamera.stopPreview();
//            mCamera.setParameters(mParameters);
//            mCamera.startPreview();
//        }
    }

	/*
    private void setWifiData(boolean aBoolean) {
        if (aBoolean){
            WiFiUtil.openWiFi(getActivity());
        } else {//关闭WiFi
            WiFiUtil.stopWiFi(getActivity());
        }

    }*/

//    private void setVolume(String value) {
//        int voice = Integer.parseInt(value);
//        AudioUtil.getInstance(App.instance).setMediaVolume(voice);
//        Setup.Volume = voice;
//    }

    private void setUSBMode(String value) {
        Setup.USBMode = value;
        AppLog.v(TAG, "===============>setUSBMode: "+value);

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
    private void setFileViewPattern(String value) {
        /*switch (value){
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
            //time = 50 * 60000;
            time = 30 * 60000;
        }else {
            //Setup.isSegmentedVideo = true;
        }
        Setup.videoMaxDuration = time;
        //AppLog.v(TAG, "====================>isSegmentedVideo: "+ Setup.isSegmentedVideo);
        //AppLog.v(TAG, "====================>setVideoSegment: "+ Setup.SegmentedVideo);
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
    private static void setPicContinous(String value){

        int num = StringUtil.getDigstFromStr(value);
        if (num ==0){
            Setup.isContinuousShooting = false;
        }else {
            Setup.isContinuousShooting = true;
            Setup.continuousShootNum = num;
        }
    }



    private   Configuration config;
    private   DisplayMetrics dm;
    private   Resources resources;
    /**
     * 设置系统语言
     */
    private void setSystemLanguage(String value){
        resources =getActivity().getResources();
        config = resources.getConfiguration();
        dm = resources.getDisplayMetrics();
        if (value.equals("Chinese")){
            config.locale = Locale.SIMPLIFIED_CHINESE;
        }else {
            config.locale = Locale.US;
        }
        resources.updateConfiguration(config, dm);
        Intent intent = new Intent(getActivity(),MainActivity.class);
        startActivity(intent);
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

        if (!WiFiUtil.wifiIsOpen(context)){
            AppLog.v(TAG, "updataWifiState:11 ============== "+WiFiUtil.wifiIsOpen(context) );
            prefer.pTipView.setText("关闭");
        }else {
            prefer.pTipView.setText("打开");
            AppLog.v(TAG, "updataWifiState:22 ============== "+WiFiUtil.wifiIsOpen(context) );
        }

    }

    private void updataGpsState(PreferenceWithTip prefer) {

        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }

        if (!GpsUtils.isOPen(context)){
            prefer.pTipView.setText("关闭");
            prefer.setSummary("关闭");
        }else {
            prefer.pTipView.setText("打开");
            prefer.setSummary("打开");

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
            prefer.pTipView.setText("关闭");
        }else {
            prefer.pTipView.setText("打开");
        }
    }
    private void updataOsDate(PreferenceWithTip prefer) {
        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }
        prefer.pTipView.setText(curr_date);
    }

    private void upDataOsTime(PreferenceWithTip prefer) {
        if (prefer ==null){
            return;
        }
        if (prefer.pTipView ==null){
            return;
        }
        prefer.pTipView.setText(curr_time);

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
        mParameters.setExposureCompensation(StringUtil.getDigstFromStr(value));
    }
    /**
     * 将图片的质量的参数设置进摄像头
     * @param value
     */
    private static void setJpegQuality(String value) {

       // mParameters.setJpegQuality(Integer.parseInt(value));
        mParameters.setJpegQuality(StringUtil.Quality(value,false));
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


    /**
     * 遍历整个PreferenceScreen
     * a.如果是PreferenceGroup则获取里面的preference
     * b.如果Preference是ListPreference则设置他的summary
     * @param pref
     */
    private static void initSummary(Preference pref) {//传进来的是PreferenceScreen
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
     * z设置ListPreference的entry
     * @param pref
     */
    private static void updatePrefSummary(Preference pref) {
        if (pref instanceof ListPreference) {
            pref.setSummary(((ListPreference) pref).getEntry());//entry就是sharedpreference里面保存的值
        }else if (pref != null )
        {
            if (!(pref instanceof TwoStatePreference)){//如果不是SwitchPreference或CheckBoxPreference
                // 则将他的summary写进去（SwitchPreference 继承TwoStatePreference）
                String summary = pref.getSharedPreferences().getString(pref.getKey(),"");
                pref.setSummary(summary);
            }
        }
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {

        String key = preference.getKey();

         switch ( key){
             case KEY_CURRENT_DATA:
                 setDate(preference);
                  break;
             case KEY_CURRENT_TIME:
                 setTime(preference);
                 break;
             case KEY_BRIGHTNESS:
                 setBritness();
                 break;
             case KEY_RESET:
                 showResetDialog(getActivity());
                 break;
             case KEY_WIFI_DATA:
                 setWifiData2(preference);
                 break;
             case KEY_BULTOOTH:
                 setBooth();
                 break;
             case KEY_PREF_GPS_DATA2:
                 setGps(preference);
                 break;
         }
         return super.onPreferenceTreeClick(preferenceScreen, preference);
    }



    String curr_date = "";
    String curr_time = "";
    private void setDate(final Preference pref){
        final DateTimeDialog dialog1 = new DateTimeDialog(getActivity(), R.layout.date);
        final DatePicker dataPicker = (DatePicker) dialog1.getPicker();
        dataPicker.init(year, month, day, new DatePicker.OnDateChangedListener() {
            @Override
            public void onDateChanged(DatePicker view, int year, int monthOfYear, int dayOfMonth) {

                SettingFragment.this.year = year;
                SettingFragment.this.month = monthOfYear;
                SettingFragment.this.day = dayOfMonth;
                dialog1.setTitle("设置日期: "+year+"/"+(month+1)+"/"+day);
            }
        });
        dialog1.setOnPositiveListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                 dialog1.dismiss();
            }
        });
        dialog1.setOnNegativeListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                TimeUtils.onDateSet(/*c,*/ year, month, day);
                curr_date = year+"/"+(month+1)+"/"+day;
                updataOsDate((PreferenceWithTip) pref);
                dialog1.dismiss();
            }
        });
        dialog1.show();
    }


    private void setTime(final Preference pref){

        final DateTimeDialog dialog = new DateTimeDialog(getActivity(), R.layout.time);
        TimePicker timePicker = (TimePicker) dialog.getPicker();
        timePicker.setOnTimeChangedListener(new TimePicker.OnTimeChangedListener() {
            @Override
            public void onTimeChanged(TimePicker view, int hourOfDay, int minute) {
                SettingFragment.this.hour = hourOfDay;
                SettingFragment.this.minute = minute;
                dialog.setTitle("设置时间: "+hourOfDay+":"+minute);
            }
        });


        dialog.setOnPositiveListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                 dialog.dismiss();
            }
        });
        dialog.setOnNegativeListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TimeUtils.onTimeSet(hour, minute);
                curr_time = hour+":"+minute;
                upDataOsTime((PreferenceWithTip) pref);
                dialog.dismiss();
            }
        });
        dialog.show();


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

    /**
     * 可以在这里更新每一个summary对应的值
     */

    static Context context;
    @Override
    public void onResume() {
        super.onResume();
        context = getActivity().getApplicationContext();
        getPreferenceScreen().getSharedPreferences()
                .registerOnSharedPreferenceChangeListener(this);



        updataGpsState((PreferenceWithTip) getPreferenceScreen().findPreference(KEY_PREF_GPS_DATA2));
        updataWifiState((PreferenceWithTip) getPreferenceScreen().findPreference(KEY_WIFI_DATA));
        updataBluetoothAdapter((PreferenceWithTip) getPreferenceScreen().findPreference(KEY_BULTOOTH));
        updataOsDate((PreferenceWithTip) getPreferenceScreen().findPreference(KEY_CURRENT_DATA));
        upDataOsTime((PreferenceWithTip) getPreferenceScreen().findPreference(KEY_CURRENT_TIME));
       PreferenceWithTip trip =  (PreferenceWithTip) getPreferenceScreen().findPreference(KEY_PREF_GPS_DATA2);


        AppLog.v(TAG, "onResume: =============>>222 "+trip.getSummary() );
    }

    @Override
    public void onPause() {
        super.onPause();
        getPreferenceScreen().getSharedPreferences()
                .unregisterOnSharedPreferenceChangeListener(this);
    }

    private void showResetDialog(Context context){
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        //builder.setTitle("确定恢复出厂设置?");
        //builder.setMessage("将会清空所有数据");
        builder.setTitle(R.string.confirm_recovery);
        builder.setMessage(R.string.clear_all_data_waring);
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
        builder.setNegativeButton(getActivity().getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
         Dialog dialog = builder.create();
        dialog.show();

    }


    // 定义5个记录当前时间的变量
    private int year;
    private int month;
    private int day;
    private int hour;
    private int minute;

    private  Calendar c;

    public void initCalendar() {
        c = Calendar.getInstance();
        year = c.get(Calendar.YEAR);
        month = c.get(Calendar.MONTH);
        day = c.get(Calendar.DAY_OF_MONTH);
        hour = c.get(Calendar.HOUR);
        minute = c.get(Calendar.MINUTE);

    }

    public static void setCameraConfig(Camera mCamera){
        if (mCamera !=null){
            //设置完参数从新初始化摄像头
            mCamera.stopPreview();
            mCamera.setParameters(mParameters);
            mCamera.startPreview();
        }
    }

    private static void setVideoAddOsd(boolean videoAddOsd) {
        VideoRunnable.isAddOSD = videoAddOsd;
     }

    public void setScreenOn(boolean screenOn) {
       Setup.screenOn = screenOn;
     }
}
