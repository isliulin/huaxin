package gbpe.policeass.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;

import com.topvision.videodemo.CameraWrapper;
import com.topvision.videodemo.rencoder.VideoRunnable;

import java.util.List;
import java.util.Set;

import gbpe.policeass.R;
import gbpe.policeass.Setup.SettingTags;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.fragments.SettingSegFragment;

/**
 * Created by Administrator on 2017/3/20.
 */
public class SettingUtils {
    /**
     * 如果key对应的值是 1280x720，则返回的是字符串数组
     * 在程序中再将字符串转换成整形
     * @param key
     * @param context
     * @return
     */
    private static final String TAG = "SettingUtils";

    public static String[] getStrArryValues(Context context,String key){
        SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
        String prefVideoSize=prefs.getString(key,"");
        String[] values=prefVideoSize.split("x");
        return values;
    }


    public static String getStrValues(Context context,String key){
        SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
        String value=prefs.getString(key,"");
        return value;
    }

    public static String getStrValues(Context context,String key,String defaultValues){
        SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
        String value=prefs.getString(key,defaultValues);
        return value;
    }


    public static boolean getBoolenValues( Context context,String key){
        SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
        boolean value=prefs.getBoolean(key,false);
        return value;
    }

    /**
     * 设置key对应的值
     * @param context
     * @param key
     * @param value
     * @return
     */
   public static boolean setStrValues(Context context,String key,String value){
       SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
       if (prefs!=null){
           SharedPreferences.Editor editor =  prefs.edit();
           editor.putString(key,value);
           editor.commit();
           return true;
       }
       return false;
   }
    /**
     * 设置key对应的值
     * @param context
     * @param key
     * @param value
     * @return
     */
    public static boolean setBoolValues(Context context,String key,Boolean value){
        SharedPreferences prefs= PreferenceManager.getDefaultSharedPreferences(context);
        if (prefs!=null){
            SharedPreferences.Editor editor =  prefs.edit();
            editor.putBoolean(key,value);
            editor.commit();
            return true;
        }
        return false;
    }



    private static boolean intToBool(int time){
        if (time==0){
            return false;
        }else {
            return true;
        }
    }

    //开机读取APP所有配置信息
    public static void initConfigure(Context context){

        Log.e("guobao","=====================>initConfigure=================================================================================================================" );



        //获取视频参数的配置
        // videoMaxDuration 不能为0, 否则录像闪退
        Setup.videoMaxDuration = StringUtil.getSecFromStr(getStrValues(context,"video_segmented"));
        Log.e("guobao","=====================>SegmentedVideo  "+getStrValues(context,"video_segmented"));
        if (Setup.videoMaxDuration < 600000) {  // 不小于10分钟
            //Setup.videoMaxDuration = 3000000;
            Setup.videoMaxDuration = 30 * 60000;
        }
        //Setup.isSegmentedVideo = intToBool(Setup.SegmentedVideo);
        //Log.e("guobao","=====================>isSegmentedVideo  "+ Setup.isSegmentedVideo );
//
//        Setup.videoPrerecordTime = StringUtil.getSecFromStr(getStrValues(context,"video_prerecord"));
//        Log.e("guobao","=====================>video_prerecord  "+getStrValues(context,"video_segmented"));
//        Setup.VideoPrerecord = intToBool(Setup.videoPrerecordTime);
//        Log.e("guobao","=====================>VideoPrerecord  "+ Setup.isVideoDelay );

        Setup.VideoPrerecord = getBoolenValues(context,"video_prerecord");
        Log.e("guobao","=====================>video_prerecord  "+Setup.VideoPrerecord);
        if (Setup.VideoPrerecord)
            Setup.videoPrerecordTime = 10;
        else
            Setup.videoPrerecordTime = 0;
        Log.e("guobao","=====================>VideoPrerecord  "+ Setup.videoPrerecordTime );


        Setup.VideoDelay = StringUtil.getSecFromStr(getStrValues(context,"video_delay"));
        Log.e("guobao","=====================>VideoDelay  "+getStrValues(context,"video_delay"));
        Setup.isVideoDelay = intToBool((int) Setup.VideoDelay);
        Log.e("guobao","=====================>isVideoDelay  "+ Setup.isVideoDelay );


        Setup.frameRate = Integer.parseInt(getStrValues(context,"video_rate"));
        Log.e("guobao","=====================>frameRate  "+getStrValues(context,"video_rate"));

        Setup.videoFilePostfix = getStrValues(context,"video_outputformat");
        Log.e("guobao","=====================>videoFilePostfix  "+getStrValues(context,"video_outputformat"));

        Setup.imgFilePostfix = getStrValues(context,"jpeg_outputformat");
        Log.e("guobao","=====================>imgFilePostfix  "+getStrValues(context,"jpeg_outputformat"));

        //获取音频参数的配置
        Setup.sampleRate = Integer.parseInt(getStrValues(context,"audio_samplerate","44100"));
        Log.e("guobao","=====================>sampleRate  "+getStrValues(context,"audio_samplerate"));

        Setup.MaxDuration = Integer.parseInt(getStrValues(context,"audio_maxduration","0"));
        Log.e("guobao","=====================>MaxDuration  "+getStrValues(context,"audio_maxduration"));

        Setup.soundFilePostfix = getStrValues(context,"audio_outputformat");
        Log.e("guobao","=====================>soundFilePostfix  "+getStrValues(context,"audio_outputformat"));

        Setup.KeyTone = getBoolenValues(context,"key_tone");
        Log.e("guobao","=====================>KeyTone  "+getBoolenValues(context,"key_tone"));

        Setup.policeNumber = getStrValues(context,"policeNumber");
        Log.e("guobao","=====================>policeNumber  "+getStrValues(context,"policeNumber"));

        Setup.devNum = getStrValues(context,"devNumber");
        Log.e("guobao","=====================>devNumber  "+getStrValues(context,"devNumber"));

        Setup.usrPassword = getStrValues(context,"usrPassword");
        Log.e("guobao","=====================>usrPassword  "+getStrValues(context,"usrPassword"));

        Setup.isVibrator = getBoolenValues(context,"vibrate");//震动
        Log.e("guobao","=====================>isVibrator  "+getBoolenValues(context,"vibrate"));

        Setup.usrName = getStrValues(context,"usrName");
        Log.e("guobao","=====================>usrName  "+getStrValues(context,"usrName"));

        Setup.adminName = getStrValues(context,"AdminName");
        Log.e("guobao","=====================>adminName  "+getStrValues(context,"AdminName"));

        Setup.adminPasswd = getStrValues(context,"AdminPassword");
        Log.e("guobao","=====================>adminPasswd  "+getStrValues(context,"AdminPassword"));

        List<String> resolution =  StringUtil.getBracketsValue(getStrValues(context,"picture_size","5M (2560x1920)"));
        Log.e("guobao","=====================>setting  "+resolution.get(0));
        Setup.picturewidht = Integer.parseInt(resolution.get(0).split("x")[0]);
        Setup.pictureheight =Integer.parseInt(resolution.get(0).split("x")[1]);
        System.out.println("---------->>>>picsize"+ Setup.picturewidht+": "+ Setup.pictureheight);

        Setup.FileView = getStrValues(context,"fileview_type");
        Log.e("guobao","=====================>FileView  "+getStrValues(context,"fileview_type"));

        //Setup.videoMaxDuration = Integer.parseInt(getStrValues(context,"video_maxduration"));
        //Log.e("guobao","=====================>videoMaxDuration  "+getStrValues(context,"video_maxduration"));

        //Setup.VideoWidth = Integer.parseInt(getStrValues(context,"video_size","640x480").split(" ")[0].split("x")[0]);
        //Setup.VideoHight = Integer.parseInt(getStrValues(context,"video_size","640x480").split(" ")[0].split("x")[1]);
        //modified by ycb, 修改默认分辨率
        Setup.VideoWidth = Integer.parseInt(getStrValues(context,"video_size","1920x1080").split(" ")[0].split("x")[0]);
        Setup.VideoHight = Integer.parseInt(getStrValues(context,"video_size","1920x1080").split(" ")[0].split("x")[1]);
        Log.e("guobao","=====================> VideoWidth :"+ Setup.VideoWidth +"VideoHight: "+ Setup.VideoHight);

        CameraWrapper.SRC_IMAGE_WIDTH  = Setup.VideoWidth;
        CameraWrapper.SRC_IMAGE_HEIGHT = Setup.VideoHight;
        CameraWrapper.DST_IMAGE_WIDTH  = Setup.VideoWidth;
        CameraWrapper.DST_IMAGE_HEIGHT = Setup.VideoHight;


        Setup.PictureQuality = StringUtil.Quality(getStrValues(context,"jpeg_quality"),false);
        Log.e("guobao","=====================>PictureQuality  "+getStrValues(context,"jpeg_quality"));

        /*Setup.ExposureCompensation = Integer.parseInt(getStrValues(context,"exposure_compensation","0"));
        Log.e("guobao","=====================>ExposureCompensation  "+getStrValues(context,"exposure_compensation"));*/

        Setup.VideoMotionDetection = getBoolenValues(context,"apply_motiondetection");
        Log.e("guobao","=====================>VideoMotionDetection  "+getBoolenValues(context,"apply_motiondetection"));

        Setup.previewWidth = Integer.parseInt(getStrValues(context,"video_size","640x480").split(" ")[0].split("x")[0]);
        Setup.previewHeight = Integer.parseInt(getStrValues(context,"video_size","640x480").split(" ")[0].split("x")[1]);
        Log.e("guobao","=====================>previewWidth  :"+ Setup.previewWidth+" previewHeight:"+ Setup.previewHeight);

        Setup.HandLockScreen = getBoolenValues(context,"onekey_lockscreen");
        Log.e("guobao","=====================>HandLockScreen  "+getBoolenValues(context,"onekey_lockscreen"));

        //Setup.videoMaxDuration = Integer.parseInt(getStrValues(context,"video_maxduration"));
        //Log.e("guobao","=====================>MaxDuration  "+getStrValues(context,"video_maxduration"));

         Setup.VideoQuality = StringUtil.Quality(getStrValues(context,"video_quality"),true);
        Log.e("guobao","=====================>VideoQuality  "+getStrValues(context,"video_quality"));
        Log.e("guobao","=====================>VideoQuality  "+Setup.VideoQuality );

        Setup.sos =  Integer.parseInt( getStrValues(context,"sos","0"));

        Log.e("guobao","=====================>sos  "+getStrValues(context,"sos"));

        Setup.isVibrator = getBoolenValues(context,"vibrate");
        Log.e("guobao","=====================>isVibrator  "+getBoolenValues(context,"vibrate"));


        Setup.USBMode = getStrValues(context,"udisk");
        Log.e("guobao","=====================>USBMode  "+getStrValues(context,"udisk"));

        Setup.inFraRed = StringUtil.InfraRed(getStrValues(context,"InfraRed"));

        Log.e("guobao","=====================>inFraRed  "+ Setup.inFraRed );


        VideoRunnable.isAddOSD = SettingUtils.getBoolenValues(context,"video_osd");
        Log.e("guobao","=====================>isAddOSD  "+ VideoRunnable.isAddOSD );

//        Setup.Volume = Integer.parseInt(getStrValues(context,"Volume")) ;
//        Log.e("guobao","=====================>Volume  "+getStrValues(context,"Volume"));
//        AudioUtil.getInstance(context).setMediaVolume(Setup.Volume)
        Setup.screenOn = SettingUtils.getBoolenValues(context,"screenOn");
        Log.e("guobao","=====================>screenOn  "+  Setup.screenOn);

		String photos = SettingUtils.getStrValues(context,SettingTags.KEY_PREF_JPEG_CONTINOUS,context.getString(R.string.close));
		SettingSegFragment.setPicContinous(photos);
		
    }

    /**
     * 初始化配置信息（恢复出厂设置）
     * @param context
     */
    public static void reset(Context context) {

        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = sp.edit();
        
        editor.putString("video_segmented",context.getString(R.string.thirty_minutes));
        editor.putString("video_delay",context.getString(R.string.close));
        editor.putString("video_quality",context.getString(R.string.normal));
        editor.putString("jpeg_continous",context.getString(R.string.close));
        
        editor.putString("jpeg_quality",context.getString(R.string.normal));

        editor.putString("udisk",context.getString(R.string.usb_forbiden));
        editor.putString("InfraRed",context.getString(R.string.manual));

		AppLog.d(TAG, "video_segmented: "+context.getString(R.string.thirty_minutes));
		AppLog.d(TAG, "video_delay: "+context.getString(R.string.close));
		AppLog.d(TAG, "video_quality: "+context.getString(R.string.normal));
		AppLog.d(TAG, "jpeg_continous: "+context.getString(R.string.close));
		AppLog.d(TAG, "jpeg_quality: "+context.getString(R.string.normal));
		AppLog.d(TAG, "udisk: "+context.getString(R.string.usb_forbiden));
		AppLog.d(TAG, "InfraRed: "+context.getString(R.string.manual));

		//editor.putString("video_size","1920x1080 1080p 16:9");
        //editor.putString("video_rate","30");

		//editor.putString("audio_samplerate","44100");

        //editor.putString("audio_outputformat","wav");
        //editor.putString("audio_maxduration","1800000");
        //editor.putString("fileview_type",context.getString(R.string.picture_mode));

		//editor.putString("video_outputformat","mp4");
        //editor.putString("video_motiondetection",context.getString(R.string.close));
        //editor.putString("video_maxduration","1800000");

        //editor.putString("preview_size","5M (2560x1920)");
        //editor.putString("picture_size","5M (2560x1920)");
        //editor.putString("picture_outformat","jpg");
        //editor.putString("flash_mode","auto");
        //editor.putString("focus_mode","continous-video");
        //editor.putString("white_balance","auto");
        //editor.putString("scene_mode","auto");


        //editor.putString("exposure_compensation","0 EV");

        //editor.putString("LanguageZfy","Chinese");
        //editor.putString("policeNumber","111111");
        //editor.putString("devNumber","456");
        //editor.putString("usrPassword","111111");
        //editor.putString("usrName","szgb");

        //editor.putString("AdminName","szgb");
        //editor.putString("AdminPassword","666666");


       // editor.putBoolean("gps_data",false);
       // editor.putBoolean("wifi_data",false);
       // editor.putBoolean("key_tone",false);
       // editor.putBoolean("vibrate",false);
       // editor.putBoolean("onekey_lockscreen",false);
//        editor.putBoolean("",false);
//        editor.putBoolean("",false);
//        editor.putBoolean("",false);
//        editor.putBoolean("",false);
        try {
            editor.commit();
        } catch (Exception e) {
            AppLog.e(TAG, "reset error: "+e.toString());
        }


     }
}
