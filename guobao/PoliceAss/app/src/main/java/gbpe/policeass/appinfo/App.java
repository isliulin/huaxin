package gbpe.policeass.appinfo;

import android.annotation.TargetApi;
import android.app.Application;
import android.content.Context;
import android.content.res.Resources;
import android.os.Build;
import android.preference.PreferenceManager;

import com.nostra13.universalimageloader.cache.disc.naming.Md5FileNameGenerator;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.QueueProcessingType;
import com.squareup.leakcanary.LeakCanary;
import com.squareup.leakcanary.RefWatcher;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.CrashHandler;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.SettingUtils;

/**
 * Created by Administrator on 2016/9/12.
 */
public class App extends Application {
    public Resources resources;
    public long lockSceenTime;
    public  boolean isBack;
    public static Context instance ;
    private static final String TAG = "GUOBAO_APP";

    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    @SuppressWarnings("unused")
    @Override
    public void onCreate() {
        super.onCreate();
        resources = getResources();
        //初始化系统参数配置
        CrashHandler crashHandler = CrashHandler.getInstance();
        crashHandler.init(getApplicationContext());

        instance = getApplicationContext();

		// modified by ycb, 20190105
        PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preferences, false);
        //PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preference_video, false);
        //PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preference_photo, false);
        //PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preference_audio, false);
        //PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preference_system, false);
        //PreferenceManager.setDefaultValues(getApplicationContext(), R.xml.preference_user, false);

            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        SettingUtils.initConfigure(getApplicationContext());
                        SDCardUtils.writePwdToSD(getApplicationContext(), "adminpwd", Setup.adminPasswd);
                        SDCardUtils.writePwdToSD(getApplicationContext(), "userpwd", Setup.usrPassword);
                    } catch (Exception e) {
                        AppLog.e (TAG, "initConfig error: "+e.toString());
                    }
                }
            }).start();


      //  SPUtils.initConfigure(this);
        initImageLoader(getApplicationContext());

     //   mRefWatcher = LeakCanary.install(this);
     }


    public static void initImageLoader(Context context) {

        ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(context)
                .threadPriority(Thread.NORM_PRIORITY - 2)
                .denyCacheImageMultipleSizesInMemory()
                .discCacheFileNameGenerator(new Md5FileNameGenerator())
                .tasksProcessingOrder(QueueProcessingType.LIFO)
                .writeDebugLogs() // Remove for release app
                .build();
        ImageLoader.getInstance().init(config);



    }
//
//    public static RefWatcher getRefWatcher(Context context){
//        App baseApplication = (App) context.getApplicationContext();
//        return baseApplication.mRefWatcher;
//    }
//
//    private RefWatcher mRefWatcher;





    static {
        System.loadLibrary("yuv_utils");
        System.loadLibrary("yuv");
    }


}
