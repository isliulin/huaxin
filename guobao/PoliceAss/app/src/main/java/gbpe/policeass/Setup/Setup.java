package gbpe.policeass.Setup;

import android.graphics.PixelFormat;
import android.media.MediaRecorder;
import android.os.Environment;

import com.topvision.videodemo.util.TimeUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Properties;

import gbpe.policeass.utils.FileNumberFormat;
import gbpe.policeass.utils.SDCardUtils;

/*
* 将系统中所有参数都在这里定义
* */

public class Setup<T extends Object> {

	/**
	 * a.将1.picturewidht，pictureheight 2.VideoWidth，VideoHight 3.previewWidth,previewHeight
	 *              4.surfaceView width,height 这几个宽高比例必须保存一致(否则预览界面会拉伸)
	 * b.2和3，4的比例必须保持一致否则录制视频的时候画面会变形
	 *
	 */
	public T className ;


     public static final String 	internalSDCard = SDCardUtils.getSDCardPath();

	public static String TalkBackAppMainPagePath = "com.android.calendar.LaunchActivity";//设置对讲APP

	public static boolean isNormalRecording = false;

	//重点文件标识
	public static boolean isEmphasisFile = false;
	//存放普通文件的文件夹
	public static String comFileFolder = "100ANDRO/";
	//存放重点文件的文件夹
	public static String emphFileFolder="101ANDRO/";
	//存放预录视频文件夹
	public static String segmentedTemp ="SEGTEMP/";

	//内置sd卡(视频，音频，图片文件默认保存路径)
	public static String interImgPaths = SDCardUtils.getSDCardPath()+"DCIM/";
	public static String interVideoPaths = SDCardUtils.getSDCardPath()+"DCIM/";
	public static String interSoundPaths = SDCardUtils.getSDCardPath()+"DCIM/";

	//外置sd卡保存路径(视频，音频，图片文件默认保存路径)
//	public static String imagePaths = "/storage/sdcard0/DCIM/";
//	public static String videoPaths = "/storage/sdcard0/DCIM/";
//	public static String soundPaths = "/storage/sdcard0/DCIM/";

	public static String imagePaths =  SDCardUtils.getSDCardPath()+"DCIM/";
	public static String videoPaths =  SDCardUtils.getSDCardPath()+"DCIM/";
	public static String soundPaths =  SDCardUtils.getSDCardPath()+"DCIM/";

	//文件后缀
	public static String imgFilePostfix   = "jpg";
	public static String videoFilePostfix = "mp4";
	public static String soundFilePostfix = "wav";
	public static String logFilePostfix = ".log";





	//是否取消上传文件
	public static  boolean isCancleUpload = false;

	//选择文件的个数
	public static int beSelecedItems= 0;

	//服务器IP地址：
	public static String SERVER_IP = "192.168.1.176";
	//服务器端口号：
	public static int SERVER_PORT = 1314;

	//警员编号
	public static String policeNumber = "123456";
	//设备编号
	public static String devNum = "1234567";

	public static String usrName = "guobao";

	public static String  unitNumber = "szgb123";

	public static String  unitName = "szgb";

	public static String adminPasswd = "666666";

	public static  String adminName = "szgb";

	public static boolean longPressVideo = false;
	public static boolean longPressAudio = false;




	//音频参数设置
	public static int audioSource =MediaRecorder.AudioSource.MIC;
	public static int audioOutputFormat = MediaRecorder.OutputFormat.AMR_NB;
	public static int setAudioEncoder = MediaRecorder.AudioEncoder.AMR_NB;
	public static int sampleRate = 44100;  //音频的采样率，recorderParameters中会有默认值
	public static int MaxDuration = 80000;//录制的最长时间
	public static int getSampleRate() {return sampleRate;}
	public static int getMaxDuration() {return MaxDuration;}
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

	//public static String ExposureCompensation; //曝光补偿："EV 0.0","EV +0.3","EV +0.5","EV +0.7","EV +1.0","EV +1.3","EV +1.7","EV +2.0",
	//"EV -2.0","EV -1.7","EV -1.3","EV -1.0","EV -0.7","EV -0.5","EV -0.3"
	public static int ExposureCompensation;


	//连拍张数
	public static int continuousShootNum = 10;//连拍10张，20张
	//是否连拍
	public static boolean  isContinuousShooting=false ;   //连拍："关","连拍2张","连拍3张","连拍5张"，"连拍10张"，"连拍20张"


	//视频参数设置
	//public static boolean isSegmentedVideo= false;//是否分段录制

	// tag by ycb: 视频录制都需要按时间段保存，按样机的设置方式为10,20,30分钟
	//public static int  SegmentedVideo = 0;//"5分钟"(300),"10(600)分钟","15(900)分钟","20(1200)分钟","30(1800)分钟","关"(分段录制默认值为30分钟)(按秒来计算分钟数)

	public static int frameRate = 30;//视频帧率
	// 分辨率： 320x240 , 640x480, 1280x720 =16:9 ,1920:1080 = 16:9 ,1920:1088 =16:9
	public static int VideoWidth = 640;
	public static int VideoHight = 480 ;
	//画质： "中（BitRate * 1024 * 1024）","一般（BitRate * 1024 * 512）", "低（BitRate * 512 * 512）" "精质（BitRate * 1920 * 1080）"
	public static int VideoQuality = 1920*1080;
	// "关","开"
	public static boolean VideoPrerecord;
 	public static int videoPrerecordTime = 20;//默认15秒保存一次 ,20 ,30 ,一分钟
 	//public static int videoPrerecordTime = 20;
	public static int outputFormat = MediaRecorder.OutputFormat.MPEG_4;// 设置录制完成后视频的封装格式THREE_GPP为3gp.MPEG_4为mp4
	public static int videoEncoder = MediaRecorder.VideoEncoder.H264; // 设置录制的视频编码h263 h264
	public static int audioEncoder = MediaRecorder.AudioEncoder.AAC;
	public static boolean isVideoDelay ;
	public static long VideoDelay = 0;			//录像延时:关、5秒、10秒、20分钟、30分钟
	public static boolean VideoMotionDetection;  //移动侦测：  "关",  "开"
	public static int videoSource = MediaRecorder.VideoSource.DEFAULT;
	public static int AudioSource = MediaRecorder.AudioSource.DEFAULT;
	//public static int videoMaxDuration = 80000;
	public static int videoMaxDuration = 3000;

	public static boolean apply_video_maxduration;

	public static boolean IRcutState;



	//预览的宽高和屏幕宽高,让手机自动选择
	public static int previewWidth = 1280;
	public static int previewHeight = 720;


	public static String battery_level = "";

	public static  int  titalFilesNum=0;


	//   8888888_000000_20160129205547_0001
	// add by ycb
	public static String SDCARD_PATH = "/storage/sdcard0/";
	public static String RAM_PATH    = "/storage/sdcard0/";
	public static String UDISK_FILE_PATH = "/storage/sdcard0/";
	public static Boolean SYSTEM_LOG_SWITCH = true;


	String videoFilePath =videoPaths+comFileFolder+policeNumber+"_"+devNum+"_"+
			TimeUtils.milliseconds2String2( System.currentTimeMillis())+"_"+ FileNumberFormat.formatter(titalFilesNum)+".mp4";



	/**
	 * 这里的surfaceView的尺寸，不是布局文件中设置的尺寸，
	 * 而是根据执法仪屏幕大小随便设置的宽高比为4:3或16:9
	 * 的尺寸，这样preSize会根据这个尺寸自动适配手机里面
	 * 的尺寸，并选择合适的preSize,只要preSize的尺寸和
	 * video的宽高比例一致，拍摄视频的时候界面就不会拉伸。
	 *
	 * 设置视频宽高比例的时候也要动态设置以下两个参数才可以
	 *
	 * 240:135  = 16:9
	 * 320:240 = 4:3
	 * 240:180 = 4:3
	 */

	public static int surfaceWidth = 180;
	public static int surfaceHeight=240;



	//系统信息
	//LoginPWD
	//DefaultSetting
	//public String Language;
	public static SimpleDateFormat myLogSdf = new SimpleDateFormat("yyyyMMdd");;//日志名称格式
	public static   int SDCARD_LOG_FILE_SAVE_DAYS = 7;//sd卡上的文件最多保存的天数
	public static String LOG_PATH_SDCARD_DIR = SDCardUtils.getSDCardPath()+"/DCIM/LOGS/";
	public static long  AutomaticShutdown = 30000l;   //自动关机： "关","1分钟","2分钟","3分钟","5分钟"
	//public static String USBMode="USB设置模式";  		//USB："USB设置模式","USB密码模式"
	public static String USBMode = "";
	public static boolean WifiFuntion;  	//WiFi功能开关："开","关"
	public static int  AutoTurnOffScreen = 30000;  //自动关屏： "30秒","1分钟", "3分钟", "从不"
	public static int  Volume = 8;//16,"15","14","13", "12","11","10","09","08","07","06","05", "04","03","02","01","关"

	//密拍模式
	public static boolean KeyTone;	//按键音："开","关"
	public static String InfraRed; //红外： "关","自动","手动"
	//语音播报
	public static String  FileView; //文件查看模式："缩略图","列表"
	//视频高低分辨率切换
	public static boolean GPSFuntion;//定位系统
	//修改密码
	//修改用户编号
	public static boolean HandLockScreen;//手动锁屏
	public static boolean LanguageZfy  ; //语言："简体中文","繁体中文","ENGLISH"});true:汉语，false:是英语

	public static boolean IndicatorLight ;// "开","关"

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


	//屏幕亮度调节
	public static int screenLight = 0;
	//在导出存储卡数据时需要输入密码。在回看视频，照片，录音时需要输入密码。
	public static String usrPassword = "111111";//初始状态为6个1

	public static boolean isLanuched;

	public static int sos = 110 ;

	public static int fileCount =0;
	public static boolean isCancle =false;

	public static int fconnt=0;
	public static boolean videoRecording;

	public static boolean currPage = true;
	public static boolean screenOn = true;

	public static int currIRcutState =1;//记录IRcut/红外灯当前的状态，是处于打开状态还是处于关闭状态

	//public static boolean currIRcutState = true;
	public static boolean isFirstOpenIRcut = true;


	//public static double longitude;
	//public static double latitude;

	public static String pre_file_dir = "/storage/sdcard0/VID_"+
			gbpe.policeass.utils.TimeUtils.milliseconds2String5(System.currentTimeMillis())+"_"+fconnt;

	private Setup() {}//单例模式：使其全局唯一（即使该类的所有属性全局唯一）
	private static volatile Setup instance = null;
	public static Setup getInstance() {
		if (instance == null) {
			synchronized (Setup.class) {
				if (instance == null) {
					instance = new Setup();
				}
			}
		}
		return instance;
	}

	public static boolean precord_less_than_ten_sec = false;//当预录的文件小于十秒的时候

	public static String getProperties(String key) {
		String val = "";
		try {
			Properties properties = new Properties();
			InputStream in = new FileInputStream(Environment.getExternalStorageDirectory()+ File.separator+"gbpe.prop");
			properties.load(in);
			in.close();
			val = properties.getProperty(key);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return val;
	}


	

//	/**
//	 * 数据库
//	 */
//	public static String DB_NAME = "metadata.db";
//	/**
//	 * 保存视频文件元数据的表
//	 */
//	public static String VIDEO_TABLE = "videoinfo";
//	private final static String CREATE_VIDEO_TABLE = "create table phone(_id integer primary key autoincrement, startTime text, stopTime text," +
//			" size text, path text, latitude text, longitude text)";
//	/**
//	 * 保存图片文件信息的表
//	 */
//	//public static String PIC_TABLE = "picinfo";
//	private final static String CREATE_PIC_TABLE = "create table pic(_id integer primary key autoincrement, startTime text, stopTime text," +
//			" size text, path text, latitude text, longitude text)";
//	/**
//	 * 保存音频文件的表
//	 */
////	public static String AUDIO_TABLE ="audioinfo";
//
//	private final static String CREATE_AUDIO_TABLE = "create table audio(_id integer primary key autoincrement, startTime text, stopTime text," +
//			" size text, path text, latitude text, longitude text)";

	/**
	 * 开发板支持的视频分辨率
	 *
	 * 320:240 352:288  480:320  480:368  640:480    720:480 800:480  800:600   864:480 960:540  1280:720  1920:1080  1920:1088 1680:1248
	 */

}
