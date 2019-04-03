package gbpe.policeass.SysUpdate;

import android.app.AlarmManager;
import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.os.RecoverySystem;
import android.support.annotation.Nullable;

import java.io.File;
import java.io.IOException;
import java.security.GeneralSecurityException;
import java.util.List;

import gbpe.policeass.BuildConfig;
import gbpe.policeass.utils.AppLog;

public class PollService extends IntentService implements HttpDownloader.OtapackageHandle{

    private static final String TAG = "PollService";

    // 防止多对象的引用
    private static Boolean isOnUpdate = false;

    private static final int POLL_INTERVAL = 1000 * 60; // 15 secondes

    public PollService() {
        super(TAG);
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {

        // 升级包是否已经下载完成
        StateRecorder sr = new StateRecorder(this);

        if (isOnUpdate) {
            AppLog.i (TAG, "升级进行中");
            return;
        }

        if (sr.getOtaReady()) {
            String path = sr.getOtaPath();
            AppLog.i (TAG, "开始升级: "+ path);

            File otaPackage = new File(path);
            if (otaPackage.exists()) {

                //Intent i = new Intent(this, OtaUpdateActivity.class);
                //intent.putExtra(OtaUpdateActivity.PACKAGE_PATH, path);
                //startActivity(intent);
                installOtaPackage(path);
                return;
            } else {
                AppLog.i (TAG, "升级包不存在，" + path);
                sr.setOtaReady(false);
                sr.setOtaPath(null);
            }
        } else {
            AppLog.i (TAG, "无升级包信息");
        }

        //检测网络
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        @SuppressWarnings("deprecation")
                boolean isNetworkAvailable = cm.getBackgroundDataSetting() && cm.getActiveNetworkInfo() != null;
        if (!isNetworkAvailable) {
            AppLog.i (TAG, "network is unavailable");
            return;
        }

        AppLog.i (TAG, "Received an intent: "+intent);
        //此处添加查询升级包代码
        final List<OtaPackageItem> items = new OtaPackageFetchr().fetchItems("59.110.154.81",80, "gb.sys.default ");
        if (items.size() > 0) {

            //HttpThread httpThread = new HttpThread(items.get(0).getUrl(),"", items.get(0).getUrl());
            //httpThread.start();
            //HttpThread.startInstance(items.get(0).getUrl(),"", items.get(0).getUrl(),this);
            AppLog.d (TAG, "version: "+items.get(0).getVersionName());
            if (checkVersion(items.get(0).getVersionName())){
                //Toast.makeText(this,R.string.find_new_package,Toast.LENGTH_SHORT).show();
                HttpThread.startInstance(items.get(0).getUrl(), "", items.get(0).getUrl(),this);
            }
        }
    }

    private boolean checkVersion(String ver) {

        AppLog.i (TAG, "checkVersion");
        try {
            String[]  subver = ver.split("\\.");
            int softVer = Integer.parseInt(subver[1]);
            int complileVer = Integer.parseInt(subver[2]);
            int curSoftVer = Integer.parseInt(BuildConfig.VERSION_NAME.split("\\.")[1]);
            int curComplileVer = Integer.parseInt(BuildConfig.BUILD_TIME.split("\\.")[0]);

            AppLog.i (TAG, "new ver: "+softVer+", new compile ver: "+complileVer+", cur ver: "+curSoftVer+
                    ", cur compile ver: "+curComplileVer);

            if (!subver[0].equals(BuildConfig.VERSION_NAME.split("\\.")[0])){
                AppLog.i (TAG, "硬件不匹配");
                return false;
            } else if(softVer < curSoftVer){      // soft version 可以相同
                AppLog.i (TAG, "软件版本过期");
                return false;
            } else if (complileVer <= curComplileVer) {  // complile version 必须大于 current version
                AppLog.i (TAG, "编译版本过期");
                return false;
            }else {
                return true;
            }
        }catch (Exception e) {
            return false;
        }

    }


    public static void setServiceAlarm(Context context, boolean isOn) {
        Intent i = new Intent(context, PollService.class);
        PendingIntent pi = PendingIntent.getService(context,0,i,0);

        AlarmManager alarmManager = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);

        if (isOn) {
            alarmManager.setRepeating(AlarmManager.RTC, System.currentTimeMillis(), POLL_INTERVAL, pi);
        } else {
            alarmManager.cancel(pi);
            pi.cancel();
        }
    }

    @Override
    public void onPackageReady(String path) {
        StateRecorder sr = new StateRecorder(this);
        //sr.setOtaPath(path);
        sr.setOtaReady(true);
        sr.setOtaPath(path);

        AppLog.i(TAG, "start ota update: "+path);
        //Intent intent = new Intent(this,OtaUpdateActivity.class);
        //intent.putExtra(OtaUpdateActivity.PACKAGE_PATH,path);
        //startActivity(intent);
        installOtaPackage(path);
    }

    public void installOtaPackage(String otaPath){

        // 加入标志，防止误删

        if (isOnUpdate) {
            AppLog.e (TAG, "installing is going");
            return;
        }
        isOnUpdate = true;

        //Toast.makeText(this,R.string.begin_ota_tips,Toast.LENGTH_SHORT).show();
        AppLog.i (TAG, "begin install "+otaPath);

        File file = new File(otaPath);
        if (!file.exists()) {
            AppLog.i (TAG,"firmware not found");
            return;
        }

        try {
            RecoverySystem.verifyPackage(file, new RecoverySystem.ProgressListener() {
                @Override
                public void onProgress(int progress) {
                    AppLog.i (TAG, "finish "+progress+"%");
                }
            },null);
        } catch (GeneralSecurityException e) {
            AppLog.e (TAG,"GeneralSecurityException "+e.getMessage());
        } catch (IOException e) {
            AppLog.e (TAG,"IOException "+e.getMessage());
        }

        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {

        }

        try {
            RecoverySystem.installPackage(this,file);
        } catch (Exception e){
            AppLog.i (TAG,"install failed");
        }

    }



}
