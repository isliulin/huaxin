package gbpe.policeass.SysUpdate;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.logging.LogRecord;

import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.OtaUpdateActivity;

public class HttpDownloader {

    private URL url = null;
    private final String TAG = "HttpDownloader";
    private int fileLength;
    //private Context context;
    private OtapackageHandle otapackageHandle;
    //private Handler handler;

    public interface OtapackageHandle{
        void onPackageReady(String path);
    }

    public HttpDownloader(/*Context context,*/ OtapackageHandle handle/*Handler handler*/) {
        //this.context = context;
        otapackageHandle = handle;
        //this.handler = handler;
    }

    /**
     * 读取文本文件
     * @param urlStr url路径
     * @return 文本信息
     * 根据url下载文件，前提是这个文件中的内容是文本，
     * 1.创建一个URL对象
     * 2.通过URL对象，创建一个Http连接
     * 3.得到InputStream
     * 4.从InputStream中得到数据
     */
    public String download(String urlStr) {
        StringBuffer sb = new StringBuffer();
        String line;
        BufferedReader bufferedReader = null;

        try {
            url = new URL(urlStr);
            //创建http连接
            HttpURLConnection urlConn = (HttpURLConnection) url.openConnection();
            //使用IO流读取数据
            bufferedReader = new BufferedReader(new InputStreamReader(urlConn.getInputStream()));
            while ((line = bufferedReader.readLine()) != null) {
                sb.append(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        finally {
            try {
                bufferedReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        AppLog.i("TAG","下载txt文件");
        AppLog.i("TAG",sb.toString());
        return sb.toString();
    }

    /**
     * 读取任何文件
     * 返回-1 ，代表下载失败。返回0，代表成功。返回1代表文件已经存在
     *
     * @param urlStr
     * @param path
     * @param fileName
     * @return
     */
    public int downlaodFile(String urlStr, final String path, String urlFileName) {
        InputStream input = null;

        try {
            URL url = new URL(urlStr);
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            fileLength = urlConnection.getContentLength();
            AppLog.i (TAG, "length: "+fileLength);
        } catch (MalformedURLException e) {
            AppLog.e (TAG, "get file length error, "+e);
        } catch (IOException e) {
            AppLog.e (TAG, "get file length error, "+e);
        }

        String fileName = urlFileName.replace("http://","");
        fileName = fileName.substring(fileName.lastIndexOf("/")+1);
        AppLog.i (TAG, "file name: " + fileName);

        try {
            //final String finalFileName = fileName;
            FileUtil fileUtil = new FileUtil(new FileUtil.Monitor() {
                @Override
                public void onDownloadProgress(long total) {
                    //AppLog.i (TAG, "download "+total+" bytes"+"len: "+fileLength+", "+((double)(total*100)/fileLength)+"%");
                    if ((total%(1024*1024))==0) {
                        AppLog.i (TAG, "download total "+(total/(1024*1024))+"M");
                    }
                }
            });

            if (fileUtil.isFileExist(FileUtil.getSdPath() + File.separator + path + fileName)) {
                AppLog.i (TAG,"ota file exist, take further check");
                return 1;
            } else {
                AppLog.i(TAG, "start to download: " + urlStr);
                input = getInputStearmFormUrl(urlStr);

                if (input != null) {
                    //AppLog.i (TAG, "write2SDFromInput");
                    File resultFile = fileUtil.write2SDFromInput(path,fileName,input);
                    if (resultFile == null) {
                        AppLog.e (TAG, "down load ota package failed");
                        return -1;
                    } else {
                        AppLog.i (TAG, "download finished, begin to update");
                        //StateRecorder sr = new StateRecorder(context);
                        //sr.setOtaPath(path + fileName);

                        // 设置下载标志， 每次启动查询该标志，如果为真，直接升级
                        //sr.setOtaReady(true);
                        if (otapackageHandle!=null) {
                            otapackageHandle.onPackageReady(FileUtil.getSdPath()  + File.separator + path+ fileName);
                        }
                        //otaHandle.onPackageReady(path+ finalFileName);

                        /*handler.post(new Runnable() {
                            @Override
                            public void run() {
                                Intent intent = new Intent(context,OtaUpdateActivity.class);
                                intent.putExtra(OtaUpdateActivity.PACKAGE_PATH,path+ finalFileName);
                                context.startActivity(intent);
                            }
                        });*/
                    }
                } else {
                    AppLog.i (TAG,"get input error");
                }
            }
        } catch (IOException e) {
            //e.printStackTrace();
            AppLog.e (TAG, "download file IOException " + e);
            return -1;
        } catch (Exception e) {
            AppLog.e (TAG, "download file Exception " + e);
            return -1;
        }

        finally {
            try {
                if (input != null) {
                    input.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return  0;
    }


    public InputStream getInputStearmFormUrl(String urlStr) throws IOException {
        url = new URL(urlStr);
        HttpURLConnection urlConn = (HttpURLConnection) url.openConnection();
        InputStream input = urlConn.getInputStream();
        return input;
    }
}
