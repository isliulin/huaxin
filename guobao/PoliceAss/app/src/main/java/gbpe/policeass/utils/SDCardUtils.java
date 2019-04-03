package gbpe.policeass.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Method;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.os.storage.StorageManager;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.util.Log;
import android.widget.Toast;

/**
 * SD卡相关的辅助类
 * 
 * @author zhy
 * 
 */
public class SDCardUtils {
	private SDCardUtils() {
		/* cannot be instantiated */
		throw new UnsupportedOperationException("cannot be instantiated");
	}

	/**
	 * 判断SDCard是否可用
	 *
	 * @return
	 */
	public static boolean isSDCardEnable() {
		return Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED);

	}

	/**
	 * 获取SD卡路径
	 *
	 * @return
	 */
	public static String getSDCardPath() {
		return Environment.getExternalStorageDirectory().getAbsolutePath()
				+ File.separator;
	}

	/**
	 * 获取SD卡的剩余容量 单位字节 ：byte
	 *
	 * @return
	 */
	public static long getSDCardAllSize() {
		if (isSDCardEnable()) {
			StatFs stat = new StatFs(getSDCardPath());
			// 获取空闲的数据块的数量
			long availableBlocks = (long) stat.getAvailableBlocks() - 4;
			// 获取单个数据块的大小（byte）
			long freeBlocks = stat.getAvailableBlocks();
			return freeBlocks * availableBlocks;
		}
		return 0;
	}

	/**
	 * 获取指定路径所在空间的剩余可用容量字节数，单位：byte
	 *
	 * @param filePath
	 * @return 容量字节 SDCard可用空间，内部存储可用空间
	 */
	public static long getFreeBytes(String filePath) {
		// 如果是sd卡的下的路径，则获取sd卡可用容量
		if (filePath.startsWith(getSDCardPath())) {
			filePath = getSDCardPath();
		} else {// 如果是内部存储的路径，则获取内存存储的可用容量
			filePath = Environment.getDataDirectory().getAbsolutePath();
		}
		StatFs stat = new StatFs(filePath);
		long availableBlocks = (long) stat.getAvailableBlocks() - 4;
		return stat.getBlockSize() * availableBlocks;
	}


	/**
	 * 获取系统存储路径
	 *
	 * @return
	 */
	public static String getRootDirectoryPath() {
		return Environment.getRootDirectory().getAbsolutePath();
	}


	/*
	利用formatSize函数把字节转换为用户等看懂的大小数值单位
	* */
	public static String formatSize(Context context, long size) {
		return Formatter.formatFileSize(context, size);
	}


	/**
	 * 去除文件的扩展类型（ GB/MB）因为sd卡的空间就是GB或MB
	 *
	 * @param fileName
	 * @return
	 */


	public static String getFileNameWithoutExtension(String fileName) {
		return fileName.substring(0, fileName.indexOf(" "));//返回的数据格式是711 MB或711 GB，所以应该截取空格前面的数据。
	}


	/**
	 * 以长整形返回sd卡上的剩余空间大小
	 */
	public static Float getSdCardFreeSize(Context context) {
		Float sdCardFreeSize = Float.parseFloat(getFileNameWithoutExtension(formatSize(context, getSDCardAllSize())));

		return sdCardFreeSize;
	}
//----------------------------------------------------------------------------------------//
// 得到文件系统的信息：存储块大小，总的存储块数量，可用存储块数量
// 获取sd卡空间
// 存储设备会被分为若干个区块
// 每个区块的大小 * 区块总数 = 存储设备的总大小
// 每个区块的大小 * 可用区块的数量 = 存储设备可用大小
//--------------------------------------------------------------------------------------//

	/**
	 * 获取sd卡总的空间
	 *
	 * @param context
	 * @return 由于API18（Android4.3）以后getBlockSize过时
	 * 并且改为了getBlockSizeLong因此这里需要根据
	 * 版本号来使用那一套API
	 */

	public static String getSDCardAllSize(Context context) {
		if (!isSDCardEnable()) {
			return "";
		}
		File path = Environment.getExternalStorageDirectory();
		long blockSize;
		long totalBlocks;
		long availableBlocks;
		StatFs stat = new StatFs(path.getPath());
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
			blockSize = stat.getBlockSizeLong();
			totalBlocks = stat.getBlockCountLong();
			availableBlocks = stat.getAvailableBlocksLong();
		} else {
			blockSize = stat.getBlockSize();
			totalBlocks = stat.getBlockCount();
			availableBlocks = stat.getAvailableBlocks();
		}
		return formatSize(context, blockSize * totalBlocks);
	}

	/**
	 * 获取sd卡剩余空间
	 *
	 * @param context
	 * @return
	 */
	public static String getSDCardFreeSize(Context context) {
		if (!isSDCardEnable()) {
			return "";
		}
		File path = Environment.getExternalStorageDirectory();
		long blockSize;
		long totalBlocks;
		long availableBlocks;
		StatFs stat = new StatFs(path.getPath());
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
			blockSize = stat.getBlockSizeLong();
			totalBlocks = stat.getBlockCountLong();
			availableBlocks = stat.getAvailableBlocksLong();
		} else {
			blockSize = stat.getBlockSize();
			totalBlocks = stat.getBlockCount();
			availableBlocks = stat.getAvailableBlocks();
		}
		return formatSize(context, blockSize * availableBlocks);
	}


	/**
	 * 判断SDCard空间是否充足
	 *
	 * @param context
	 */
	public static boolean judgeSDCardFreeSizeIsPlenty(Context context) {
		if (!isSDCardEnable()) {
			return false;
		}
		//String totalSize = getSDCardAllSize(context);
		String freeSize;
		try {
			// 卸载U盘时会导致崩溃，ycb20190111
			freeSize = getSDCardFreeSize(context);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		if (freeSize.endsWith(" GB")) {
			//System.out.println("--->>>内存充足");
			return true;
		} else if (freeSize.endsWith(" MB")) {
			String ss = getFileNameWithoutExtension(freeSize);
			float free = Float.parseFloat(ss);
			if (free <= 100.0f)//小于100MB
			{
				Toast.makeText(context, "内存不足", Toast.LENGTH_LONG).show();
				return false;
			}
		}
		return true;
	}

	/**
	 * 以float（浮点型）返回SDCard空间剩余容量（去掉后缀GB，MB）
	 *
	 * @param context
	 * @return
	 */
	public static float getFloatSDCardFreeSize(Context context) {
		String freeSize = getSDCardFreeSize(context);
		String ss = getFileNameWithoutExtension(freeSize);
		float free = Float.parseFloat(ss);
		return free;
	}

	/**
	 * 获取内置sd卡路径(反射方式实现)
	 *
	 * @param context
	 * @return
	 */
	public static String getInternalSDCardPath(Context context) {
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumePathsMethod = StorageManager.class.getMethod("getVolumePaths", (Class<?>[]) null);
			String[] paths = (String[]) getVolumePathsMethod.invoke(sm, new Object[]{});
			// first element in paths[] is primary storage path
			return paths[0];
		} catch (Exception e) {
			//Toast.makeText(context, "内置sd卡不可用", Toast.LENGTH_SHORT).show();
			return "";
		}

	}


	/**
	 * 获取外置SD卡路径(反射方式实现)
	 *
	 * @param context
	 * @return
	 */
	public static String getExternalSDCardPath(Context context) {
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumePathsMethod = StorageManager.class.getMethod("getVolumePaths", (Class<?>[]) null);
			String[] paths = (String[]) getVolumePathsMethod.invoke(sm, new Object[]{});
			// second element in paths[] is secondary storage path
			return paths[1];
		} catch (Exception e) {
			//Toast.makeText(context, "外置sd卡不可用", Toast.LENGTH_SHORT).show();
			return "";
		}
	}


	/**
	 * 判断sd卡是否挂载，可用(传入内置，或外置sd卡路径即可)
	 *
	 * @param context
	 * @param path
	 * @return
	 */
	public static boolean getStorageState(Context context, String path) {
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumeStateMethod = StorageManager.class.getMethod("getVolumeState", new Class[]{String.class});
			String state = (String) getVolumeStateMethod.invoke(sm, path);
			if (state.equals("mounted")) {
				return true;
			} else {
				return false;
			}
		} catch (Exception e) {
			//Toast.makeText(context, "SD卡不可用", Toast.LENGTH_SHORT).show();
			return false;
		}
	}

	/**
	 * 判断usb是否连接
	 *
	 * @param context
	 * @return
	 */
	public static boolean usbIsConn(Context context) {
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			//Method getVolumeStateMethod = StorageManager.class.getMethod("isUsbMassStorageConnected", null);
			// modified by ycb
			Method getVolumeStateMethod = StorageManager.class.getMethod("isUsbMassStorageConnected", new Class[0]);
			boolean state = (boolean) getVolumeStateMethod.invoke(sm, new Object[]{});

			return state;
		} catch (Exception e) {
			//e.printStackTrace();
			return false;
		}
		//return false;
	}

	public static boolean isExternalSDCardEnable(Context context) {

		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumeStateMethod = StorageManager.class.getMethod("getVolumeState", new Class[]{String.class});
			String path = getExternalSDCardPath(context);
			if (TextUtils.isEmpty(path)) {
				return false;
			}
			String state = (String) getVolumeStateMethod.invoke(sm, path);
			if (state.equals("mounted")) {
				return true;
			} else {
			//	Toast.makeText(context, "外置SD卡不可用", Toast.LENGTH_SHORT).show();
				return false;
			}
		} catch (Exception e) {
			//Toast.makeText(context, "外置SD卡不可用", Toast.LENGTH_SHORT).show();
			return false;
		}

	}


	/**
	 * 判断外置sd卡是否可以读、写
	 *
	 * @param context
	 * @return
	 */
	public static boolean isExternalSDCardCanWriteOrRead(Context context) {
		String path = getExternalSDCardPath(context);
		if (TextUtils.isEmpty(path)) {
			return false;
		}
		File file = new File(path, "test.txt");
		if (!file.exists()) {
			try {
				boolean state = file.createNewFile();
				if (state) {
					file.delete();
				}
				if (!state) {
					Toast.makeText(context, "系统限制外置sd卡不可读写", Toast.LENGTH_SHORT).show();
				}
				return state;
			} catch (Exception e) {
				//e.printStackTrace();
				return false;
			}
		}
		return false;
	}


	/**
	 * 获取外置sd卡剩余容量
	 *
	 * @param context
	 * @return
	 */
	public static long getStorageBytesUntilLow(Context context) {
		long size = 0l;
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumeStateMethod = StorageManager.class.getMethod("getStorageBytesUntilLow", File.class);
			String path = getExternalSDCardPath(context);
			if (TextUtils.isEmpty(path)) {
				return 0;
			}
			//size = (long) getVolumeStateMethod.invoke(sm, new File[]{new File(path)});
			size = (long) getVolumeStateMethod.invoke(sm, (Object[])new File[]{new File(path)});
		} catch (Exception e) {
			//e.printStackTrace();
			//	Toast.makeText(context, "获取外置sd卡剩余容量失败", Toast.LENGTH_SHORT).show();

			return 0;
		}
		return size;
	}


	/**
	 * 查询Seetings数据库中sys_storage_threshod_percentage的值，默认是10，
	 * 即当DATA_PATH目录下剩余空间少于其总空间的10%时，认为空间不足（ACTION_DEVICE_STORAGE_LOW）。
	 *
	 * @param context
	 * @return
	 */
	public static long getStorageLowBytes(Context context) {
		long size = 0l;
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumeStateMethod = StorageManager.class.getMethod("getStorageLowBytes", File.class);

			String path = getExternalSDCardPath(context);
			if (TextUtils.isEmpty(path)) {
				return 0;
			}
			size = (long) getVolumeStateMethod.invoke(sm, (Object[]) new File[]{new File(path)});
		} catch (Exception e) {
			//e.printStackTrace();
			//Toast.makeText(context, "", Toast.LENGTH_SHORT).show();
			return 0;
		}
		return size;
	}


	/*
    查询Settings数据库中的sys_storage_full_threshold_bytes的值，默认是1MB，即当DATA_PATH
    目录下剩余空间小于等于1M时，任务空间已满，剩余的部分是保留给系统使用的。
    */
	public static long getStorageFullBytes(Context context) {
		long size = 0l;
		try {
			StorageManager sm = (StorageManager) context.getSystemService(context.STORAGE_SERVICE);
			Method getVolumeStateMethod = StorageManager.class.getMethod("getStorageFullBytes", File.class);

			String path = getExternalSDCardPath(context);
			if (TextUtils.isEmpty(path)) {
				return 0;
			}
			size = (long) getVolumeStateMethod.invoke(sm, (Object[]) new File[]{new File(path)});
		} catch (Exception e) {
			e.printStackTrace();
		}
		return size;
	}

	//01-01 00:06:37.458 3123-3123/gbpe.javacvnewtest I/System.out: ----->getStorageBytesUntilLow  20.11 GB
	//01-01 00:06:37.461 3123-3123/gbpe.javacvnewtest I/System.out: ----->getStorageLowBytes       500 MB
	//01-01 00:06:37.462 3123-3123/gbpe.javacvnewtest I/System.out: ----->getStorageFullBytes      1.00 MB

	//123456_456_20100101083001_0001IMP.mp4
	//123456_456_  2010 0101 0   8 30 01_0001 .mp4

	/**
	 * 判断外置sd卡空间是否充足（设定当空间小于10M时，认为该sd卡空间不足）
	 *
	 * @param context
	 * @return
	 */
	public static boolean isExternalSdCardSizePlenty(Context context) {

		String sdCardFreeSize = formatSize(context, getStorageBytesUntilLow(context));
		String[] values = sdCardFreeSize.split(" ");
		float freeSize = Float.parseFloat(values[0]);
		String unit = values[1];
		if (unit.equals("GB")) {
			return true;
		} else if (unit.equals("MB")) {
			if (freeSize < 10.00) {
				return false;
			} else {
				return true;
			}
		} else if (unit.equals("KB")) {
			return false;
		} else if (unit.equals("B")) {
			return false;
		}
		return true;
	}


	private void sdToast(final Context context, final String msg) {
		((Activity) context).runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(context, "" + msg, Toast.LENGTH_SHORT).show();
			}
		});
	}


	/**
	 * 向文件中写入密码
	 *
	 * @param context  /data/data/包名/files/Admin.txt
	 * @param filename （Admin.txt 和 User.txt）
	 * @param msg
	 */
	public static void writePwdToSD(Context context, String filename, String msg) {
		String sdStatus = Environment.getExternalStorageState();
		if (!sdStatus.equals(Environment.MEDIA_MOUNTED)) {
			Log.d("TestFile", "SD card is not avaiable/writeable right now.");
			return;
		}
		try {
			//	String pathName="/data/data/"+context.getPackageName()+"/files/";
			String pathName = "/data/data/";
			String fileName = filename;
			File path = new File(pathName);
			File file = new File(pathName + fileName);
			if (!path.exists()) {
				Log.d("TestFile", "Create the path:" + pathName);
				path.mkdir();
			}
			if (!file.exists()) {
				Log.d("TestFile", "Create the file:" + fileName);
				file.createNewFile();
			}
			FileOutputStream stream = new FileOutputStream(file);
			// String s = "this is a test string writing to file.";
			byte[] buf = msg.getBytes();
			stream.write(buf);
			stream.close();

		} catch (Exception e) {
			Log.e("TestFile", "Error on writeFilToSD.");
			e.printStackTrace();
		}
	}


	public static void writePwdToSD2(Context context, String filename, String msg) {

		FileOutputStream outStream = null;
		try {
//			//outStream = this.openFileOutput("caojin.txt", Context.MODE_PRIVATE);
//			File file = context.getFilesDir();
//			String path = file.getAbsolutePath()+"/userpwd";
//			File folder = new File(path);
//			if (!folder.exists()){
//				folder.mkdir();
//			}
			outStream = context.openFileOutput(filename, Context.MODE_WORLD_READABLE);
			outStream.write(msg.getBytes("utf-8"));
			if (outStream!=null){
				outStream.close();
			}
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public void writeFileData(String fileName,String message){
		try{
			//FileOutputStream fout = openFileOutput(fileName, MODE_PRIVATE);
			FileOutputStream fout = new FileOutputStream(fileName);
			byte [] bytes = message.getBytes();
			fout.write(bytes);
			fout.close();
 		}
		catch(Exception e){
			e.printStackTrace();
 		}
	}


}
