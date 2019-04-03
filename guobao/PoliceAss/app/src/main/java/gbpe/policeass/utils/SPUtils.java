package gbpe.policeass.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.PixelFormat;
import android.media.MediaRecorder;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;

import gbpe.policeass.Setup.Setup;

public class SPUtils
{
	private SPUtils()
	{
		/* cannot be instantiated */
		throw new UnsupportedOperationException("cannot be instantiated");
	}

	/**
	 * 保存在手机里面的文件名
	 */
	public static final String FILE_NAME = "share_data";

	/**
	 * 保存数据的方法，我们需要拿到保存数据的具体类型，然后根据类型调用不同的保存方法
	 * 
	 * @param context
	 * @param key
	 * @param object
	 */
	public static void put(Context context, String key, Object object)
	{

		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sp.edit();

		if (object instanceof String)
		{
			editor.putString(key, (String) object);
		} else if (object instanceof Integer)
		{
			editor.putInt(key, (Integer) object);
		} else if (object instanceof Boolean)
		{
			editor.putBoolean(key, (Boolean) object);
		} else if (object instanceof Float)
		{
			editor.putFloat(key, (Float) object);
		} else if (object instanceof Long)
		{
			editor.putLong(key, (Long) object);
		} else
		{
			editor.putString(key, object.toString());
		}

		SharedPreferencesCompat.apply(editor);
	}

	/**
	 * 得到保存数据的方法，我们根据默认值得到保存的数据的具体类型，然后调用相对于的方法获取值
	 * 
	 * @param context
	 * @param key
	 * @param defaultObject
	 * @return
	 */
	public static Object get(Context context, String key, Object defaultObject)
	{
		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);

		if (defaultObject instanceof String)
		{
			return sp.getString(key, (String) defaultObject);
		} else if (defaultObject instanceof Integer)
		{
			return sp.getInt(key, (Integer) defaultObject);
		} else if (defaultObject instanceof Boolean)
		{
			return sp.getBoolean(key, (Boolean) defaultObject);
		} else if (defaultObject instanceof Float)
		{
			return sp.getFloat(key, (Float) defaultObject);
		} else if (defaultObject instanceof Long)
		{
			return sp.getLong(key, (Long) defaultObject);
		}

		return null;
	}

	/**
	 * 移除某个key值已经对应的值
	 * 
	 * @param context
	 * @param key
	 */
	public static void remove(Context context, String key)
	{
		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sp.edit();
		editor.remove(key);
		SharedPreferencesCompat.apply(editor);
	}

	/**
	 * 清除所有数据
	 * 
	 * @param context
	 */
	public static void clear(Context context)
	{
		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sp.edit();
		editor.clear();
		SharedPreferencesCompat.apply(editor);
	}

	/**
	 * 查询某个key是否已经存在
	 * 
	 * @param context
	 * @param key
	 * @return
	 */
	public static boolean contains(Context context, String key)
	{
		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);
		return sp.contains(key);
	}

	/**
	 * 返回所有的键值对
	 * 
	 * @param context
	 * @return
	 */
	public static Map<String, ?> getAll(Context context)
	{
		SharedPreferences sp = context.getSharedPreferences(FILE_NAME,
				Context.MODE_PRIVATE);
		return sp.getAll();
	}

	/**
	 * 创建一个解决SharedPreferencesCompat.apply方法的一个兼容类
	 * 
	 * @author zhy
	 * 
	 */
	private static class SharedPreferencesCompat
	{
		private static final Method sApplyMethod = findApplyMethod();

		/**
		 * 反射查找apply的方法
		 * 
		 * @return
		 */
		@SuppressWarnings({ "unchecked", "rawtypes" })
		private static Method findApplyMethod()
		{
			try
			{
				Class clz = SharedPreferences.Editor.class;
				return clz.getMethod("apply");
			} catch (NoSuchMethodException e)
			{
			}

			return null;
		}

		/**
		 * 如果找到则使用apply执行，否则使用commit
		 * 
		 * @param editor
		 */
		public static void apply(SharedPreferences.Editor editor)
		{
			try
			{
				if (sApplyMethod != null)
				{
					sApplyMethod.invoke(editor);
					return;
				}
			} catch (IllegalArgumentException e)
			{
			} catch (IllegalAccessException e)
			{
			} catch (InvocationTargetException e)
			{
			}
			editor.commit();
		}
	}



	//开机读取APP所有配置信息
//public static void initConfigure(Context context){

//	//获取视频参数的配置
//	Setup.isSegmentedVideo = (boolean)get(context,"isSegmentedVideo",false);//是否分段录制
//	Setup.videoSource = (int)get(context,"videoSource",MediaRecorder.VideoSource.DEFAULT) ;
//	Setup.videoPaths =(String)get(context,"videoPath",Setup.videoPaths);
//	Setup.frameRate = (int)get(context,"frameRate",30);
//	Setup.VideoWidth = (int)get(context,"VideoWidth",1280);
//	Setup.VideoHight = (int)get(context,"VideoHight",720);
//	Setup.VideoQuality = (int) get(context,"VideoQuality",1024*512);
//	Setup.VideoDelay =  (int)get(context,"VideoDelay",0);
//	Setup.VideoMotionDetection = (boolean)get(context,"VideoMotionDetection",false);
//	Setup.outputFormat = (int)get(context,"outputFormat", MediaRecorder.OutputFormat.MPEG_4) ;
//	Setup.videoEncoder = (int)get(context,"videoEncoder",MediaRecorder.VideoEncoder.H264) ;
//	Setup.audioEncoder = (int)get(context,"audioEncoder",MediaRecorder.AudioEncoder.AAC);
//	Setup.SegmentedVideo = (int) get(context,"SegmentedVideo",1800); //分段录制。
//	Setup.VideoPrerecord = (boolean) SPUtils.get(context,"VideoPrerecord",false);
//
//	Setup.surfaceWidth = (int) get(context,"surfaceWidth",180);
//	Setup.surfaceHeight = (int)get(context,"surfaceHeight",240);
//
//
//	//获取图片参数的配置
//	Setup.picturewidht = (int)get(context,"picturewidht",2560);
//	Setup.pictureheight= (int)get(context,"pictureheight",1920);
//	Setup.PictureQuality= (int) get(context,"PictureQuality",100);
//
//	Setup.continuousShootNum = (int)get(context,"continuousShootNum",10);
//	Setup.isContinuousShooting = (boolean)get(context,"isContinuousShooting",false);
//
//	Setup.ExposureCompensation =(String) get(context,"ExposureCompensation","EV 0.0");
//
//	Setup.qualickTake = (int)get(context,"qualickTake",1);
//	Setup.pictureFormat = (int)get(context,"pictureFormat", PixelFormat.JPEG);
//
//	//获取音频参数的配置
//	Setup.sampleRate = (int) get(context,"sampleRate",44100);
//	Setup.MaxDuration = (int)get(context,"MaxDuration",80000);
//	Setup.audioSource = (int)get(context,"audioSource",MediaRecorder.AudioSource.MIC);
//	Setup.setAudioEncoder = (int)get(context,"setAudioEncoder",MediaRecorder.AudioEncoder.AMR_NB);
//	Setup.audioOutputFormat = (int)get(context,"audioOutputFormat",MediaRecorder.OutputFormat.AMR_NB);
//
//	//系统其他设置
////	Setup.AutomaticShutdown = (long) get(context,"AutomaticShutdown",30000);
//	Setup.USBMode = (String)get(context,"USBMode","USB设置模式");
//	Setup.WifiFuntion = (boolean)get(context,"WifiFuntion",false);
//	Setup.AutoTurnOffScreen = (int) get(context,"AutoTurnOffScreen",30000);
//	Setup.Volume = (int) get(context,"Volume",0);
//	Setup.KeyTone =(boolean) get(context,"KeyTone",false);
//	//Setup.FileView = (boolean)get(context,"FileView",false);
//	Setup.GPSFuntion = (boolean)get(context,"GPSFuntion",false);
//	Setup.HandLockScreen = (boolean)get(context,"HandLockScreen",false);
//	Setup.LanguageZfy = (boolean) get(context,"LanguageZfy",true);
//	//文件后缀配置信息
//	Setup.imgFilePostfix = (String)get(context,"imgFilePostfix",".jpg");
//	Setup.videoFilePostfix = (String)get(context,"videoFilePostfix",".mp4");
//	Setup.soundFilePostfix = (String)get(context,"soundFilePostfix",".wav");
//
//	Setup.policeNumber = (String)get(context,"policeNumber","123456");
//	Setup.devNum = (String)get(context,"devNum","456");
//	Setup.usrPassword = (String)get(context,"usrPassword","111111");
//	Setup.isVibrator = (boolean)get(context,"isVibrator",false);
//
// }
//
//	/**
//	 * 初始化配置信息（恢复出厂设置）
//	 * @param context
//     */
//	public static void restFacSetting(Context context)
//	{
// 		SPUtils.put(context,"VideoPrerecord",false);
//		put(context,"isSegmentedVideo",false);//是否分段录制
//		put(context,"videoSource",MediaRecorder.VideoSource.DEFAULT) ;
//		put(context,"videoPath",Setup.videoPaths);
//		put(context,"frameRate",30);
//		put(context,"VideoWidth",1280);
//		put(context,"VideoHight",720);
//		put(context,"VideoQuality",1024*512);
//		put(context,"VideoDelay",0);
//		put(context,"VideoMotionDetection",false);
//		put(context,"outputFormat", MediaRecorder.OutputFormat.MPEG_4) ;
//		put(context,"videoEncoder",MediaRecorder.VideoEncoder.H264) ;
//		put(context,"audioEncoder",MediaRecorder.AudioEncoder.AAC);
//		put(context,"SegmentedVideo",300000); //分段录制。
//
//		//获取图片参数的配置
//		put(context,"picturewidht",1024);
//		put(context,"pictureheight",768);
//		put(context,"PictureQuality",100);
//		put(context,"ContinuousShooting","只拍一张");
//		put(context,"ExposureCompensation","EV 0.0");
//		put(context,"qualickTake",1);
//		put(context,"pictureFormat", PixelFormat.JPEG);
//
//		//获取音频参数的配置
//		put(context,"sampleRate",44100);
//		put(context,"MaxDuration",80000);
//		put(context,"audioSource",MediaRecorder.AudioSource.MIC);
//		put(context,"setAudioEncoder",MediaRecorder.AudioEncoder.AMR_NB);
//		put(context,"audioOutputFormat",MediaRecorder.OutputFormat.AMR_NB);
//
//		//系统其他设置
////	Setup.AutomaticShutdown = (long) get(context,"AutomaticShutdown",30000);
//		put(context,"USBMode","USB设置模式");
//		put(context,"WifiFuntion",false);
//		put(context,"AutoTurnOffScreen",30000);
//		put(context,"Volume",0);
//		put(context,"KeyTone",false);
//		put(context,"FileView",false);
//		put(context,"GPSFuntion",false);
//		put(context,"HandLockScreen",false);
//		put(context,"LanguageZfy",true);
//		//文件后缀配置信息
//		put(context,"imgFilePostfix",".jpg");
//		put(context,"videoFilePostfix",".mp4");
//		put(context,"soundFilePostfix",".wav");
//
//		put(context,"policeNumber","123456");
//		put(context,"devNum","456");
//		put(context,"usrPassword","111111");
//		put(context,"isVibrator",false);
//
//		initConfigure(context);
//	}
}