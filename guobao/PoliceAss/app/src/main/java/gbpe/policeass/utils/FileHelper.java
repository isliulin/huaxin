package gbpe.policeass.utils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.List;

import android.os.Environment;

public class FileHelper
{
	private static String FILEPATH = "";

	public static File newFile(String filename)
	{

		if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
		{
			File sdCardDir = Environment.getExternalStorageDirectory();
			// SDCard目录：/mnt/sdcard
			FILEPATH = sdCardDir.getAbsolutePath();
			System.out.println("here is  sdcardir......lixiaodoaaaa.    " + FILEPATH);
		}

		File file = null;

		try
		{
			file = new File(FILEPATH, filename);
			file.delete();
			file.createNewFile();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		return file;
	}

	public static void writeFile(File file, byte[] data, int offset, int count) throws IOException
	{
		FileOutputStream fos = new FileOutputStream(file, true);
		fos.write(data, offset, count);
		fos.flush();
		fos.close();
	}

	public static byte[] readFile(String filename) throws IOException
	{
		File file = new File(FILEPATH, filename);
		// file.createNewFile();
		FileInputStream fis = new FileInputStream(file);
		BufferedInputStream bis = new BufferedInputStream(fis);
		int leng = bis.available();
		byte[] b = new byte[leng];
		bis.read(b, 0, leng);
		bis.close();
		return b;
	}
	/**
	 * 封装好的文件拷贝方法
	 */
	public static void copy(String from, String to,OnFileCopyListener listener) {
		if (listener !=null){
			listener.onFileCopyStart();
		}
		try {
			InputStream in = new FileInputStream(from);
			OutputStream out = new FileOutputStream(to);

			byte[] buff = new byte[1024];
			int len = 0;
			while ((len = in.read(buff)) != -1) {
				out.write(buff, 0, len);
			}
			in.close();
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		if (listener !=null){
			listener.onFileCopyFinished();
		}
	}

	//递归获得每个文件的绝对路径
	public static List getFilePath(String dirPath){
		List<String> fileList = new LinkedList<>();
		File dir = new File(dirPath);
		File[] files = dir.listFiles();
		if(files == null){
			return null;
		}
		for(int i = 0; i < files.length; i++){
			if(files[i].isDirectory()){
				getFilePath(files[i].getAbsolutePath());
			}
			else {
				fileList.add(files[i].getAbsolutePath());
			}
		}
		return fileList;
	}

	public interface OnFileCopyListener{
		public void onFileCopyStart();
		public void onFileCopyFinished();
	}
}
