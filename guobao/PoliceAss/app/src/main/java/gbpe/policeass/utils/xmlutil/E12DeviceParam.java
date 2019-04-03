package gbpe.policeass.utils.xmlutil;

import android.graphics.PixelFormat;
import android.media.MediaRecorder;

import java.text.SimpleDateFormat;

import gbpe.policeass.utils.SDCardUtils;

/**
 * Created by Administrator on 2017/5/25.
 */
public class E12DeviceParam {
    public static  String Resolution = "Resolution";


    public static  String PictureSize = "PictureSize";
    public static  String ContinuousShooting = "ContinuousShooting";
    public static  String SegmentedVideo = "2 分钟";
    public static  String Prerecord = "Prerecord";
    public static  String VideoDelay = "VideoDelay";
    public static  String ExposureCompensation = "ExposureCompensation";
    public static  String MotionDetection = "MotionDetection";
    public static  String InfraRed = "InfraRed";
    public static  String IndicatorLight = "IndicatorLight";
    public static  String GPS = "GPS";


    //警员编号
    public static String policeNumber = "123456";
    //设备编号
    public static String devNum = "5678";

    public static String usrName = "guobao";

    public static String productId = "456";

    public static String USBMode = "USBMode";
    public static  String FileView = "FileView";
    public static  String Volume = "Volume";
    public static  String KeyTone = "KeyTone";
    public static String AutoTurnOffScreen = "AutoTurnOffScreen";
    public static   String AutomaticShutdown = "AutomaticShutdown";
    public static  String Language = "Language";
    public static  String DefaultSetting = "DefaultSetting";

    public static String  unitNumber = "szgb123";

    public static String  unitName = "szgb";

    public static String adminPasswd = "666666";


    //音频参数设置
    public static int audioSource = MediaRecorder.AudioSource.MIC;
    public static int setAudioEncoder = MediaRecorder.AudioEncoder.AMR_NB;
    public static int sampleRate = 44100;  //音频的采样率，recorderParameters中会有默认值
    public static int MaxDuration = 80000;//录制的最长时间
    public static int getSampleRate() {return sampleRate;}
    public void setSampleRate(int sampleRate) {this.sampleRate = sampleRate;}
    public static int getMaxDuration() {return MaxDuration;}
    public void setMaxDuration(int maxDuration) {this.MaxDuration = maxDuration;}

    public static boolean apply_audio_maxduration;

    //照相设置参数
    public static int pictureFormat = PixelFormat.JPEG;

    /**
     *  1280:960 ,1600:1200 , 1920:1088  , 2048:1536 , 2560:1440 , 2560:1920 (如果不设置手机默认会选择这个分辨率) , 2880:1728
     */
    public static int picturewidht = 2560;
    public static int pictureheight = 1920;

    public static int qualickTake = 1;//l连拍功能（默认每次自拍一张）

    public static int PictureQuality = 100;		//画质： "高","中", "低"

    //连拍张数
    public static int continuousShootNum = 10;//连拍10张，20张
    //是否连拍
    public static boolean  isContinuousShooting=false ;   //连拍："关","连拍2张","连拍3张","连拍5张"，"连拍10张"，"连拍20张"

    //视频参数设置
    //public static boolean isSegmentedVideo= false;//是否分段录制

    public static int frameRate = 30;//视频帧率
    // 分辨率： 320x240 , 640x480, 1280x720 =16:9 ,1920:1080 = 16:9 ,1920:1088 =16:9
    public static int VideoWidth = 1920;
    public static int VideoHight = 1080;

    public static String getVideoQuality() {
        return VideoQuality;
    }

    public   void setVideoQuality(String videoQuality) {
        VideoQuality = videoQuality;
    }

    //画质： "中（BitRate * 1024 * 1024）","一般（BitRate * 1024 * 512）", "低（BitRate * 512 * 512）" "精质（BitRate * 1920 * 1080）"
    public static String VideoQuality = "1920*1080";
    // "关","开"
    public static boolean VideoPrerecord;
    public static int videoPrerecordTime = 15;//默认15秒保存一次 ,20 ,30 ,一分钟
    public static int outputFormat = MediaRecorder.OutputFormat.MPEG_4;// 设置录制完成后视频的封装格式THREE_GPP为3gp.MPEG_4为mp4
    public static int videoEncoder = MediaRecorder.VideoEncoder.H264; // 设置录制的视频编码h263 h264
    public static int audioEncoder = MediaRecorder.AudioEncoder.AAC;
    public static boolean isVideoDelay ;
    public static boolean VideoMotionDetection;  //移动侦测：  "关",  "开"
    public static int videoSource = MediaRecorder.VideoSource.DEFAULT;
    public static int AudioSource = MediaRecorder.AudioSource.DEFAULT;




//
//    <VideoBitrate>30</VideoBitrate>
//    <AudioOutPutFormat>wav</AudioOutPutFormat>
//    <VideoOutPutFormat>mp4</VideoOutPutFormat>
//    <PicOutPutFormat>jpg</PicOutPutFormat>
//    <PicQuality>姝ｅ父</PicQuality>
//    <AudioSampleRate>44100</AudioSampleRate>
//    <VideoMaxDuration>80000</VideoMaxDuration>
//    <AudioMaxDuration>80000</AudioMaxDuration>
//    <PreviewSize>1280x720 720p 4:3</PreviewSize>
//    <Vibrate>鍏�</Vibrate>
//    <Sos>110</Sos>


    //文件后缀
//    public static String imgFilePostfix   = "jpg";
//    public static String videoFilePostfix = "mp4";
//    public static String soundFilePostfix = "wav";
    public static   String audioSimpleRate = "";



    public static   String videoMaxDuration = "";
    public static   String audioMaxDuration = "";
    public static   String previewSize = "";
    public static   String vibrate = "";
    public static   String sos = "";
    public static   String videoBitrate = "";

    public static String audioOutputFormat = "";
    public static String videoOutputFormat = "";
    public static String picOutputFormat = "";

    public static String picQuality = "";



    public static String getPicQuality() {
        return picQuality;
    }

    public   void setPicQuality(String picQuality) {
        E12DeviceParam.picQuality = picQuality;
    }


    public static String getVideoOutputFormat() {
        return videoOutputFormat;
    }

    public   void setVideoOutputFormat(String videoOutputFormat) {
        E12DeviceParam.videoOutputFormat = videoOutputFormat;
    }



    public static String getPicOutputFormat() {
        return picOutputFormat;
    }

    public   void setPicOutputFormat(String picOutputFormat) {
        E12DeviceParam.picOutputFormat = picOutputFormat;
    }










    public static String getAudioMaxDuration() {
        return audioMaxDuration;
    }

    public   void setAudioMaxDuration(String audioMaxDuration) {
        E12DeviceParam.audioMaxDuration = audioMaxDuration;
    }

//    public static String getImgFilePostfix() {
//        return imgFilePostfix;
//    }
//
//    public static void setImgFilePostfix(String imgFilePostfix) {
//        E12DeviceParam.imgFilePostfix = imgFilePostfix;
//    }
//
//    public static String getVideoFilePostfix() {
//        return videoFilePostfix;
//    }
//
//    public static void setVideoFilePostfix(String videoFilePostfix) {
//        E12DeviceParam.videoFilePostfix = videoFilePostfix;
//    }
//
//    public static String getSoundFilePostfix() {
//        return soundFilePostfix;
//    }
//
//    public static void setSoundFilePostfix(String soundFilePostfix) {
//        E12DeviceParam.soundFilePostfix = soundFilePostfix;
//    }

    public static String getAudioSimpleRate() {
        return audioSimpleRate;
    }

    public   void setAudioSimpleRate(String audioSimpleRate) {
        E12DeviceParam.audioSimpleRate = audioSimpleRate;
    }

    public   void setVideoMaxDuration(String videoMaxDuration) {
        E12DeviceParam.videoMaxDuration = videoMaxDuration;
    }
    public static String getVideoMaxDuration() {
        return videoMaxDuration;
    }

    public static String getPreviewSize() {
        return previewSize;
    }

    public   void setPreviewSize(String previewSize) {
        E12DeviceParam.previewSize = previewSize;
    }

    public static String getVibrate() {
        return vibrate;
    }

    public   void setVibrate(String vibrate) {
        E12DeviceParam.vibrate = vibrate;
    }

    public static String getSos() {
        return sos;
    }

    public   void setSos(String sos) {
        E12DeviceParam.sos = sos;
    }

    public static String getVideoBitrate() {
        return videoBitrate;
    }

    public  void setVideoBitrate(String videoBitrate) {
        E12DeviceParam.videoBitrate = videoBitrate;
    }









    //预览的宽高和屏幕宽高,让手机自动选择
    public static int previewWidth = 1280;
    public static int previewHeight = 720;

    public static String battery_level = "";


    public static int surfaceWidth = 180;
    public static int surfaceHeight=240;


    //系统信息
    //public String Language;
    public static SimpleDateFormat myLogSdf = new SimpleDateFormat("yyyyMMdd");;//日志名称格式
    public static   int SDCARD_LOG_FILE_SAVE_DAYS = 7;//sd卡上的文件最多保存的天数
    public static String LOG_PATH_SDCARD_DIR = SDCardUtils.getSDCardPath()+"/DCIM/LOGS/";
    public static boolean WifiFuntion;  	//WiFi功能开关："开","关"


    //语音播报
    //视频高低分辨率切换
    public static boolean GPSFuntion;//定位系统
    //修改密码
    //修改用户编号
    public static boolean HandLockScreen;//手动锁屏
    public static boolean LanguageZfy  ; //语言："简体中文","繁体中文","ENGLISH"});true:汉语，false:是英语

    // IndicatorLight // "开","关"

    //是否进行震动提示
    public static boolean isVibrator;

    //摄像头设置参数
    public  static int cameraPosition = 0;//0代表前置摄像头,1代表后置摄像头,默认打开前置摄像头

    //白光灯
    public static boolean whiteLight = false;//开关
    //激光灯
    public static boolean laserLamp = false;//开关

    //红外线
    public static boolean inFraRed = false;

    //sos报警
    public static boolean isWarn = false;

    
//====================================================================================================================================================================================


    public String getResolution() {
        return Resolution;
    }

    public void setResolution(String resolution) {
        Resolution = resolution;
    }



    public String getPictureSize() {
        return PictureSize;
    }

    public void setPictureSize(String pictureSize) {
        PictureSize = pictureSize;
    }

    public String getContinuousShooting() {
        return ContinuousShooting;
    }

    public void setContinuousShooting(String continuousShooting) {
        ContinuousShooting = continuousShooting;
    }

    public String getSegmentedVideo() {
        return SegmentedVideo;
    }

    public void setSegmentedVideo(String segmentedVideo) {
        SegmentedVideo = segmentedVideo;
    }

    public String getPrerecord() {
        return Prerecord;
    }

    public void setPrerecord(String prerecord) {
        Prerecord = prerecord;
    }

    public String getVideoDelay() {
        return VideoDelay;
    }

    public void setVideoDelay(String videoDelay) {
        VideoDelay = videoDelay;
    }

    public String getExposureCompensation() {
        return ExposureCompensation;
    }

    public void setExposureCompensation(String exposureCompensation) {
        ExposureCompensation = exposureCompensation;
    }

    public String getMotionDetection() {
        return MotionDetection;
    }

    public void setMotionDetection(String motionDetection) {
        MotionDetection = motionDetection;
    }

    public String getInfraRed() {
        return InfraRed;
    }

    public void setInfraRed(String infraRed) {
        InfraRed = infraRed;
    }

    public String getIndicatorLight() {
        return IndicatorLight;
    }

    public void setIndicatorLight(String indicatorLight) {
        IndicatorLight = indicatorLight;
    }

    public String getGPS() {
        return GPS;
    }

    public void setGPS(String GPS) {
        this.GPS = GPS;
    }

    public String getUSBMode() {
        return USBMode;
    }

    public void setUSBMode(String USBMode) {
        this.USBMode = USBMode;
    }

    public String getFileView() {
        return FileView;
    }

    public void setFileView(String fileView) {
        FileView = fileView;
    }

    public String getVolume() {
        return Volume;
    }

    public void setVolume(String volume) {
        Volume = volume;
    }


    public String getKeyTone() {
        return KeyTone;
    }

    public void setKeyTone(String keyTone) {
        KeyTone = keyTone;
    }

    public String getAutoTurnOffScreen() {
        return AutoTurnOffScreen;
    }

    public void setAutoTurnOffScreen(String autoTurnOffScreen) {
        AutoTurnOffScreen = autoTurnOffScreen;
    }

    public String getAutomaticShutdown() {
        return AutomaticShutdown;
    }

    public void setAutomaticShutdown(String automaticShutdown) {
        AutomaticShutdown = automaticShutdown;
    }

    public String getLanguage() {
        return Language;
    }

    public void setLanguage(String language) {
        Language = language;
    }

    public String getDefaultSetting() {
        return DefaultSetting;
    }

    public void setDefaultSetting(String defaultSetting) {
        DefaultSetting = defaultSetting;
    }

    public static String getPoliceNumber() {
        return policeNumber;
    }

    public static void setPoliceNumber(String policeNumber) {
        E12DeviceParam.policeNumber = policeNumber;
    }

    public static String getDevNum() {
        return devNum;
    }

    public static void setDevNum(String devNum) {
        E12DeviceParam.devNum = devNum;
    }

    public static String getUsrName() {
        return usrName;
    }

    public static void setUsrName(String usrName) {
        E12DeviceParam.usrName = usrName;
    }

    public static String getProductId() {
        return productId;
    }

    public static void setProductId(String productId) {
        E12DeviceParam.productId = productId;
    }

    public static String getUnitNumber() {
        return unitNumber;
    }

    public static void setUnitNumber(String unitNumber) {
        E12DeviceParam.unitNumber = unitNumber;
    }

    public static String getUnitName() {
        return unitName;
    }

    public static void setUnitName(String unitName) {
        E12DeviceParam.unitName = unitName;
    }

    public static String getAdminPasswd() {
        return adminPasswd;
    }

    public static void setAdminPasswd(String adminPasswd) {
        E12DeviceParam.adminPasswd = adminPasswd;
    }

    public static int getAudioSource() {
        return audioSource;
    }

    public static void setAudioSource(int audioSource) {
        E12DeviceParam.audioSource = audioSource;
    }




    public static int getPreviewWidth() {
        return previewWidth;
    }

    public static void setPreviewWidth(int previewWidth) {
        E12DeviceParam.previewWidth = previewWidth;
    }

    public static int getPreviewHeight() {
        return previewHeight;
    }

    public static void setPreviewHeight(int previewHeight) {
        E12DeviceParam.previewHeight = previewHeight;
    }

    public static String getBattery_level() {
        return battery_level;
    }

    public static void setBattery_level(String battery_level) {
        E12DeviceParam.battery_level = battery_level;
    }

    public static int getSurfaceWidth() {
        return surfaceWidth;
    }

    public static void setSurfaceWidth(int surfaceWidth) {
        E12DeviceParam.surfaceWidth = surfaceWidth;
    }

    public static int getSurfaceHeight() {
        return surfaceHeight;
    }

    public static void setSurfaceHeight(int surfaceHeight) {
        E12DeviceParam.surfaceHeight = surfaceHeight;
    }

    public static SimpleDateFormat getMyLogSdf() {
        return myLogSdf;
    }

    public static void setMyLogSdf(SimpleDateFormat myLogSdf) {
        E12DeviceParam.myLogSdf = myLogSdf;
    }

    public static int getSdcardLogFileSaveDays() {
        return SDCARD_LOG_FILE_SAVE_DAYS;
    }

    public static void setSdcardLogFileSaveDays(int sdcardLogFileSaveDays) {
        SDCARD_LOG_FILE_SAVE_DAYS = sdcardLogFileSaveDays;
    }

    public static String getLogPathSdcardDir() {
        return LOG_PATH_SDCARD_DIR;
    }

    public static void setLogPathSdcardDir(String logPathSdcardDir) {
        LOG_PATH_SDCARD_DIR = logPathSdcardDir;
    }

    public static boolean isWifiFuntion() {
        return WifiFuntion;
    }

    public static void setWifiFuntion(boolean wifiFuntion) {
        WifiFuntion = wifiFuntion;
    }

    public static boolean isGPSFuntion() {
        return GPSFuntion;
    }

    public static void setGPSFuntion(boolean GPSFuntion) {
        E12DeviceParam.GPSFuntion = GPSFuntion;
    }

    public static boolean isHandLockScreen() {
        return HandLockScreen;
    }

    public static void setHandLockScreen(boolean handLockScreen) {
        HandLockScreen = handLockScreen;
    }

    public static boolean isLanguageZfy() {
        return LanguageZfy;
    }

    public static void setLanguageZfy(boolean languageZfy) {
        LanguageZfy = languageZfy;
    }

    public static boolean isVibrator() {
        return isVibrator;
    }

    public static void setIsVibrator(boolean isVibrator) {
        E12DeviceParam.isVibrator = isVibrator;
    }

    public static int getCameraPosition() {
        return cameraPosition;
    }

    public static void setCameraPosition(int cameraPosition) {
        E12DeviceParam.cameraPosition = cameraPosition;
    }

    public static boolean isWhiteLight() {
        return whiteLight;
    }

    public static void setWhiteLight(boolean whiteLight) {
        E12DeviceParam.whiteLight = whiteLight;
    }

    public static boolean isLaserLamp() {
        return laserLamp;
    }

    public static void setLaserLamp(boolean laserLamp) {
        E12DeviceParam.laserLamp = laserLamp;
    }

    public static boolean isInFraRed() {
        return inFraRed;
    }

    public static void setInFraRed(boolean inFraRed) {
        E12DeviceParam.inFraRed = inFraRed;
    }

    public static boolean isWarn() {
        return isWarn;
    }

    public static void setIsWarn(boolean isWarn) {
        E12DeviceParam.isWarn = isWarn;
    }

    public static String getAudioOutputFormat() {
        return audioOutputFormat;
    }

    public   void setAudioOutputFormat(String audioOutputFormat) {
        E12DeviceParam.audioOutputFormat = audioOutputFormat;
    }

    public static int getSetAudioEncoder() {
        return setAudioEncoder;
    }

    public static void setSetAudioEncoder(int setAudioEncoder) {
        E12DeviceParam.setAudioEncoder = setAudioEncoder;
    }

    public static boolean isApply_audio_maxduration() {
        return apply_audio_maxduration;
    }

    public static void setApply_audio_maxduration(boolean apply_audio_maxduration) {
        E12DeviceParam.apply_audio_maxduration = apply_audio_maxduration;
    }

    public static int getPictureFormat() {
        return pictureFormat;
    }

    public static void setPictureFormat(int pictureFormat) {
        E12DeviceParam.pictureFormat = pictureFormat;
    }

    public static int getPicturewidht() {
        return picturewidht;
    }

    public static void setPicturewidht(int picturewidht) {
        E12DeviceParam.picturewidht = picturewidht;
    }

    public static int getPictureheight() {
        return pictureheight;
    }

    public static void setPictureheight(int pictureheight) {
        E12DeviceParam.pictureheight = pictureheight;
    }

    public static int getQualickTake() {
        return qualickTake;
    }

    public static void setQualickTake(int qualickTake) {
        E12DeviceParam.qualickTake = qualickTake;
    }

    public static int getPictureQuality() {
        return PictureQuality;
    }

    public static void setPictureQuality(int pictureQuality) {
        PictureQuality = pictureQuality;
    }

    public static int getContinuousShootNum() {
        return continuousShootNum;
    }

    public static void setContinuousShootNum(int continuousShootNum) {
        E12DeviceParam.continuousShootNum = continuousShootNum;
    }

    public static boolean isContinuousShooting() {
        return isContinuousShooting;
    }

    public static void setIsContinuousShooting(boolean isContinuousShooting) {
        E12DeviceParam.isContinuousShooting = isContinuousShooting;
    }

    public static boolean isSegmentedVideo() {
        //return isSegmentedVideo;
        return true;
    }

    public static void setIsSegmentedVideo(boolean isSegmentedVideo) {
        //E12DeviceParam.isSegmentedVideo = isSegmentedVideo;
        //E12DeviceParam.isSegmentedVideo = true;
    }

    public static int getFrameRate() {
        return frameRate;
    }

    public static void setFrameRate(int frameRate) {
        E12DeviceParam.frameRate = frameRate;
    }

    public static int getVideoWidth() {
        return VideoWidth;
    }

    public static void setVideoWidth(int videoWidth) {
        VideoWidth = videoWidth;
    }

    public static int getVideoHight() {
        return VideoHight;
    }

    public static void setVideoHight(int videoHight) {
        VideoHight = videoHight;
    }



    public static boolean isVideoPrerecord() {
        return VideoPrerecord;
    }

    public static void setVideoPrerecord(boolean videoPrerecord) {
        VideoPrerecord = videoPrerecord;
    }

    public static int getVideoPrerecordTime() {
        return videoPrerecordTime;
    }

    public static void setVideoPrerecordTime(int videoPrerecordTime) {
        E12DeviceParam.videoPrerecordTime = videoPrerecordTime;
    }

    public static int getOutputFormat() {
        return outputFormat;
    }

    public static void setOutputFormat(int outputFormat) {
        E12DeviceParam.outputFormat = outputFormat;
    }

    public static int getVideoEncoder() {
        return videoEncoder;
    }

    public static void setVideoEncoder(int videoEncoder) {
        E12DeviceParam.videoEncoder = videoEncoder;
    }

    public static int getAudioEncoder() {
        return audioEncoder;
    }

    public static void setAudioEncoder(int audioEncoder) {
        E12DeviceParam.audioEncoder = audioEncoder;
    }

    public static boolean isVideoDelay() {
        return isVideoDelay;
    }

    public static void setIsVideoDelay(boolean isVideoDelay) {
        E12DeviceParam.isVideoDelay = isVideoDelay;
    }

    public static boolean isVideoMotionDetection() {
        return VideoMotionDetection;
    }

    public static void setVideoMotionDetection(boolean videoMotionDetection) {
        VideoMotionDetection = videoMotionDetection;
    }

    public static int getVideoSource() {
        return videoSource;
    }

    public static void setVideoSource(int videoSource) {
        E12DeviceParam.videoSource = videoSource;
    }

}
