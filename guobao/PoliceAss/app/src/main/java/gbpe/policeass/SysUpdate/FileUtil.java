package gbpe.policeass.SysUpdate;

import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import gbpe.policeass.utils.AppLog;

/**
 * Created by Spring on 2015/11/7.
 */
public class FileUtil {

    private static final String TAG = "FileUtilDownload";
    private Monitor monitor;

    public interface Monitor{
        void onDownloadProgress(long total);
    }

    public FileUtil(Monitor monitor){
        this.monitor = monitor;

    }

    public static String getSdPath() {
        if (Environment.getExternalStorageState().equalsIgnoreCase(Environment.MEDIA_MOUNTED)) {
            return Environment.getExternalStorageDirectory().toString();
        } else {
            return null;
        }
    }
    /**
     * 在SD卡上创建文件
     * @param fileName
     * @return
     * @throws java.io.IOException
     */
    public File createSDFile(String fileName) throws IOException {
        AppLog.i (TAG, "createSDFile, "+getSdPath()+File.separator+fileName);
        File file = new File(getSdPath() +File.separator+ fileName);
        file.createNewFile();
        return file;
    }

    /**
     * 在SD卡上创建目录
     * @param dirName 目录名字
     * @return 文件目录
     */
    public File createDir(String dirName){
        File dir = new File(getSdPath() + dirName);
        dir.mkdir();
        return dir;
    }

    /**
     * 判断文件是否存在
     * @param fileName
     * @return
     */
    public boolean isFileExist(String fileName){
        File file = new File(getSdPath() + fileName);
        return file.exists();
    }

    public File write2SDFromInput(String path,String fileName,InputStream input){
        File file = null;
        OutputStream output = null;

        AppLog.i (TAG, "write2SDFromInput "+path+", "+fileName);

         try {
            createDir(path);
            file =createSDFile(path + fileName);
            output = new FileOutputStream(file);

            // 往SD卡写入是4K的buffer, 从net input读取是1K的buffer
            byte [] buffer = new byte[4 * 1024];
            int len;
            long total = 0;
            while((len = input.read(buffer)) != -1){
                output.write(buffer,0, len);
                output.flush();
                total += len;
                if (monitor != null) {
                    monitor.onDownloadProgress(total);
                }
            }
        } catch (IOException e) {
            //e.printStackTrace();
             AppLog.e (TAG, "get package error: " + e);
        }
        finally {
            try {
                output.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return file;
    }
}