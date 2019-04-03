package gbpe.policeass.log;

import android.util.Log;

import java.io.File;
import java.io.RandomAccessFile;

import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.TimeUtils;

/**
 * Created by Administrator on 2016/12/6.
 */
public class LogUtils {

     //public static String filePath = "/sdcard/DCIM/LOGS/";
     // String fileName = "test.log";

    /**
     * 将字符串写入到文本文件中
     * @param strcontent 写入的内容
     * @param filePath 文件保存的路径
     * @param fileName 文件名字(以日期命名)
     */
    public static void writeTxtToFile(String strcontent, String filePath, String fileName) {

      //  2016-12-27 14:36:56 -> 开机(RS:14807MB TS:14820MB)

        String content = TimeUtils.getLogDate2()+" -> "+strcontent+"(RS:14807MB TS:14820MB)";

        //生成文件夹之后，再生成文件，不然会出错
        makeFilePath(filePath, fileName);

        String strFilePath = filePath + fileName;
        // 每次写入时，都换行写
        String strContent = content + "\r\n";
        try {
            File file = new File(strFilePath);
            if (!file.exists()) {
                Log.d("TestFile", "Create the file:" + strFilePath);
                file.getParentFile().mkdirs();
                file.createNewFile();
            }
            RandomAccessFile raf = new RandomAccessFile(file, "rwd");
            raf.seek(file.length());
            raf.write(strContent.getBytes());
            raf.close();
        } catch (Exception e) {
            Log.e("TestFile", "Error on write File:" + e);
        }
    }

    /**
     * 生成文件
     * @param filePath 文件路径
     * @param fileName 文件名
     * @return
     */
    public static File makeFilePath(String filePath, String fileName) {
        File file = null;
        makeRootDirectory(filePath);
        try {
            file = new File(filePath + fileName);
            if (!file.exists()) {
                file.createNewFile();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return file;
    }

    /**
     * 生成文件夹
     * @param filePath 文件夹路径
     */
    public static void makeRootDirectory(String filePath) {
        File file = null;
        try {
            file = new File(filePath);
            if (!file.exists()) {
                //file.mkdir();
                FileUtils.mkDir(file);
            }
        } catch (Exception e) {
            Log.i("error:", e + "");
        }
    }
}
