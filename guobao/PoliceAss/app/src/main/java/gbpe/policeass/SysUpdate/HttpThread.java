package gbpe.policeass.SysUpdate;

import android.content.Context;
import android.os.Handler;


public class HttpThread extends Thread {

    private String url;
    private String path;
    private String name;
    //private Context context;
    //private Handler responseHandler;
    private HttpDownloader.OtapackageHandle otaHandler;

    private static HttpThread instance = null;

    public static void startInstance(String url, String path, String name, /*Context context,*/ HttpDownloader.OtapackageHandle handle
                                     /*Handler handler*/){

        if (instance == null) {
            instance = new HttpThread(url,path,name,/*context,*/handle);
            instance.start();
        }
    }


    private HttpThread(String url, String path, String name, /*Context context,*/ HttpDownloader.OtapackageHandle handle/*Handler handler*/) {
        this.url = url;
        this.path = path;
        this.name = name;
        //this.responseHandler = handler;
        this.otaHandler = handle;
        //this.context = context;
    }

    @Override
    public void run() {
        HttpDownloader httpDownloader = new HttpDownloader(/*context,*/otaHandler);
        httpDownloader.downlaodFile(url,path, name);
        instance = null;
    }
}
