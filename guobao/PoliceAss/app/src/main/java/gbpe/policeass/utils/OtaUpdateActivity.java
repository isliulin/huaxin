package gbpe.policeass.utils;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.RecoverySystem;
import android.support.annotation.Nullable;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.security.GeneralSecurityException;
import java.util.ArrayList;
import java.util.EventListener;
import java.util.List;
import java.util.Scanner;

import gbpe.policeass.R;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.views.MyProgressView;
//import gbpe.policeass.zfysim.HomeActivity;


public class OtaUpdateActivity extends Activity{

    private static final String TAG = "OtaUpdate";
    //private static final String OTA_FILE = "full_newmobi6753_66t_v_l1-ota.zip";
    public static final String OTA_FILE = "update.zip";
    //private static final String INTER_SD = "/sdcard/";
    private static final String INTER_SD = "/mnt/media_rw/";
    private static final String APK_NAME = "LawRecorder.zip";
	private static final String EXT_SD = "/sdcard/";
    //private static final String APK_NAME = "LawRecorder.apk";
    //private static final String INTER_SD = "/mnt/media_rw/sdcard0/";
    //private Context mContext;
    private Handler mResposeHandler;
    public static final int MSG_SHWO_TOAST = 1001;
    public static final int MSG_PRO_DIA = 1002;
    public static final int MSG_CLR_DIA = 1003;
    public static final int MSG_KILL_PROC = 1004;
    public static final int MSG_SHOW_PRG = 1005;
    public static final int MSG_CLOSE_PRG = 1006;
    private MyProgressView mOtaProgress;
	private ProgressDialog mProgressDialog;
	public static final String PACKAGE_PATH = "package_path";
	private String mOtaPackagePath;

    private void showOtaMessage(String str){
        Toast.makeText(this,str,Toast.LENGTH_SHORT).show();
    }

	private void showProgressDialog(String text){
		mProgressDialog = ProgressDialog.show(this,null,text);
		mProgressDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                mProgressDialog.dismiss();
                return false;
            }
        });
	}

	private void closeProgressDialog(){
        if (mProgressDialog!=null){
            mProgressDialog.dismiss();
        }
    }

    private class OtaHandler extends Handler{
        private Context mContext;
        public OtaHandler(Context context){
            mContext = context;
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what){
                case MSG_SHWO_TOAST:
                    showOtaMessage((String) msg.obj);
                    break;
                case MSG_PRO_DIA:
                    mOtaProgress.setProgress(msg.arg1);
                    break;
                case MSG_CLR_DIA:
                    mOtaProgress.setVisibility(msg.arg1);
                    break;
                case MSG_KILL_PROC:
                    //Toast.makeText(mContext,msg.obj,Toast.LENGTH_SHORT).show();
                    Toast.makeText(mContext,(String)msg.obj,Toast.LENGTH_SHORT).show();
                    finish();
                    break;
                case MSG_SHOW_PRG:
                    showProgressDialog((String)msg.obj);
                    break;
                case MSG_CLOSE_PRG:
                    closeProgressDialog();
                    break;
            }
        }
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ota_update);
        mOtaProgress = (MyProgressView) findViewById(R.id.ota_progress);
        mResposeHandler = new OtaHandler(this);

        mOtaPackagePath = getIntent().getStringExtra(PACKAGE_PATH);

        File ota = new File(mOtaPackagePath);
        if (!ota.exists()) {
            mOtaPackagePath = EXT_SD + OTA_FILE;
        }

        AppLog.i (TAG, "ota package file: "+mOtaPackagePath);

/*
        mResposeHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case MSG_SHWO_TOAST:
                        showOtaMessage((String) msg.obj);
                        break;
                    case MSG_PRO_DIA:
                        mOtaProgress.setProgress(msg.arg1);
                        break;
                    case MSG_CLR_DIA:
                        mOtaProgress.setVisibility(View.INVISIBLE);
                        break;
                    case MSG_KILL_PROC:
                        Toast.makeText(,msg.obj,Toast.LENGTH_SHORT).show();
                        finish();
                        break;
                }
            }
        };*/

        new Thread(){
            @Override
            public void run(){
                beginOtaProcess(mResposeHandler);
                //testSdPath();
                /*APK 更新*/
				/*
                showOldProgressDlg(mResposeHandler,"开始更新");
                installApk(INTER_SD + APK_NAME);
                closeOldProgressDlg(mResposeHandler);
                */
                //ShellUtils.execRootCmd("ls /system/priv-app");
            }

        }.start();
    }

    //public OtaUpdate(Context context, Handler handler) {
    //public OtaUpdate(Context context) {
        //mContext = context;
        //mResposeHandler = handler;
    //}

    private void sendToastMsg(Handler handler, String toast){
        Message msg = new Message();
        msg.what = MSG_SHWO_TOAST;
        msg.obj = toast;
        handler.sendMessage(msg);
    }

    private void sendProgress(Handler handler, int pro){
        Message msg = new Message();
        msg.what = MSG_PRO_DIA;
        msg.arg1 = pro;
        handler.sendMessage(msg);
    }

    private void sendKillMsg(Handler handler, String string) {
        Message msg = new Message();
        msg.what = MSG_KILL_PROC;
        msg.obj = string;
        handler.sendMessage(msg);
    }

    private void clrProgressDlg(Handler handler, int visible){
        Message msg = new Message();
        msg.what = MSG_CLR_DIA;
        msg.arg1 = visible;
        handler.sendMessage(msg);

    }

    private void showOldProgressDlg(Handler handler,String info){
        Message msg = new Message();
        msg.what = MSG_SHOW_PRG;
        msg.obj = info;
        handler.sendMessage(msg);
    }

    private void closeOldProgressDlg(Handler handler) {
        Message msg = new Message();
        msg.what = MSG_CLOSE_PRG;
        handler.sendMessage(msg);
    }
/*
    public void testSdPath(){
        File f = new File(INTER_SD + "ycb.txt");
        try {
            FileOutputStream fos = new FileOutputStream(f);
            fos.write(new String("adbdefg").getBytes());
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            AppLog.e(TAG,e.toString());
        } catch (IOException e) {
            e.printStackTrace();
            AppLog.e(TAG,e.toString());
        }
    }*/

    public void beginOtaProcess(final Handler handler){
        Message msg = new Message();
        //AppLog.i(TAG, "sd path: "+INTER_SD + OTA_FILE);
        //AppLog.i(TAG,"sd path: "+EXT_SD + OTA_FILE);
        // 检测SD卡中是否有ota文件
        //File file = new File(SDCardUtils.getSDCardPath()+OTA_FILE);
        //File file =  new File(Environment.getRootDirectory()+OTA_FILE);
        // modified by ycb, 20190302, 实现在线下载
        File file = new File(mOtaPackagePath);
        //File file = new File(EXT_SD + OTA_FILE);
        if (!file.exists()) {
            AppLog.i (TAG,"firmware not found");
            //Toast.makeText(this,"找不到固件",Toast.LENGTH_SHORT).show();
            //Toast.makeText(mContext,"找不到升级文件",Toast.LENGTH_SHORT).show();
            //Message msg = new Message();
            //msg.what = MSG_SHWO_TOAST;
            //msg.obj = new String("找不到升级文件");
            //mResposeHandler.sendMessage(msg);
            sendKillMsg(handler,"找不到升级文件");
            return;
        }

        sendToastMsg(handler,"开始校验升级包");
        //setProgressBar("开始校验升级包");
        //msg.what = MSG_PRO_DIA;
        //msg.obj = new String("开始校验升级包");
        //mResposeHandler.sendMessage(msg);
        //String destPath = Environment.getDataDirectory() + File.separator +OTA_FILE;
        //String destPath = mContext.getFilesDir()  + File.separator + OTA_FILE;

        //AppLog.i (TAG, "copy file to "+destPath);
        //FileUtils.copyFile(SDCardUtils.getSDCardPath()+OTA_FILE,destPath);

        //AppLog.i (TAG, "copy ok, begin to verify firmware");

        try {
            RecoverySystem.verifyPackage(file, new RecoverySystem.ProgressListener() {
                @Override
                public void onProgress(int progress) {
                    //AppLog.i (TAG, "finish "+progress+"%");
                    sendProgress(handler,progress);
                }
            },null);
        } catch (GeneralSecurityException e) {
            AppLog.e (TAG,"GeneralSecurityException "+e.getMessage());
            e.printStackTrace();
        } catch (IOException e) {
            AppLog.e (TAG,"IOException "+e.getMessage());
            e.printStackTrace();
        }

        //msg.what = MSG_CLR_DIA;
        //mResposeHandler.sendMessage(msg);
        sendToastMsg(handler,"校验完成，开始安装");

        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        //setProgressBar("开始安装升级包");
        //msg = new Message();
        //msg.what = MSG_PRO_DIA;
        //msg.obj = new String("开始安装升级包");
        //mResposeHandler.sendMessage(msg);

        try {
            RecoverySystem.installPackage(this,file);
        } catch (Exception e){
            AppLog.i (TAG,"install failed");
            //Toast.makeText(mContext,"升级失败",Toast.LENGTH_SHORT).show();
            //msg = new Message();
            //msg.what = MSG_SHWO_TOAST;
            //msg.obj = new String("升级失败");
            //mResposeHandler.sendMessage(msg);
            sendToastMsg(handler,"升级失败");
            e.printStackTrace();
            finish();
        }

    }

    //获取android所有的挂载点
    private static List<String> getDevMountList() {
        List<String> mVold = new ArrayList<String>(10);
        mVold.add("/mnt/sdcard");

        try {
            File voldFile = new File("/system/etc/vold.fstab");
            if(voldFile.exists()){
                Scanner scanner = new Scanner(voldFile);
                while (scanner.hasNext()) {
                    String line = scanner.nextLine();
                    if (line.startsWith("dev_mount")) {
                        String[] lineElements = line.split(" ");
                        String element = lineElements[2];
                        AppLog.d(TAG, "Vold: element = "+element);

                        if (element.contains(":"))
                            element = element.substring(0, element.indexOf(":"));
                        if (!element.equals("/mnt/sdcard"))
                            mVold.add(element);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return mVold;
    }

    public static String getExternalSdCardPath() {
        String  path = null;
        String state = Environment.getExternalStorageState();
        //Environment.getExternalStorageDirectory()获取的是内部SDcard
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            path = Environment.getExternalStorageDirectory().getPath();
        } else {  //Environment.getExternalStorageDirectory()获取的是外部SDcard，但没插外部sdcard
            List<String> devMountList = getDevMountList();
            for (String devMount : devMountList) {
                File file = new File(devMount);
                if (file.isDirectory() && file.canWrite()) {
                    path = file.getAbsolutePath();
                    break;
                }
            }
        }
        AppLog.d(TAG, "getExternalSdCardPath: path = "+path);
        return path;
    }

    private void installApk(String apkPath){

        //String packageName = apkPath.substring(0,apkPath.indexOf(".zip"));

        AppLog.i(TAG, "开始尝试安装应用");

        clrProgressDlg(mResposeHandler, View.INVISIBLE);

        if (new File(apkPath).exists() == false){
            AppLog.e(TAG, "找不到固件");
            sendKillMsg(mResposeHandler,"找不到固件");
            return;
        }

        String fileDir = getFilesDir().getAbsolutePath();

        try {
            FileUtils.copyFile(apkPath,fileDir+"/"+APK_NAME);
        } catch (Exception e) {
            e.printStackTrace();
            sendKillMsg(mResposeHandler,"复制文件出错");
        }

        try {
            //ZipUtils.decompress(apkPath,packageName);
            ZipUtils.decompress(fileDir+"/"+APK_NAME,fileDir);
        } catch (Exception e) {
            e.printStackTrace();
            AppLog.e(TAG, "解压错误");
            sendKillMsg(mResposeHandler,"解压错误");
        }

        ShellUtils shellUtils = new ShellUtils();
        AppLog.i(TAG, "开始安装");
        //shellUtils.run("pm install -r "+ apkPath,-1);    // 无超时关闭
        // 重新挂载system为可读
        //shellUtils.run("mount -o remount,rw /system",-1);
        //AppLog.i(TAG,"result: "+shellUtils.getResult());
        ShellUtils.execRootCmd("mount -o remount,rw /system");

        // 删除文件夹
        //shellUtils.run("rm -r /system/priv-app/LawRecorder/",-1);
        //AppLog.i(TAG,"result: "+shellUtils.getResult());
        ShellUtils.execRootCmd("rm -r /system/priv-app/LawRecorder/");

        // 复制文件夹
        //shellUtils.run("mv -r "+packageName+" /system/priv-app/",-1);
        //AppLog.i(TAG,"result: "+shellUtils.getResult());
        ShellUtils.execRootCmd("cp -r "+fileDir+"/"+"LawRecorder"+" /system/priv-app/");

        // 删除中间文件
        ShellUtils.execRootCmd("rm -r "+fileDir+"/"+"LawRecorder");

        // 删除zip
        ShellUtils.execRootCmd("rm -r "+fileDir+"/"+APK_NAME);

        // 设置权限
        ShellUtils.execRootCmd("chmod -R 777 /system/priv-app/LawRecorder");

        // 重启
        ShellUtils.execRootCmd("reboot");

        sendKillMsg(mResposeHandler, "更新完毕");
    }
/*
	private void installApk() {
        File apkfile = new File(mSavePath,Constant.appFileName);
        if (!apkfile.exists()) {
            return;
        }
        // 通过Intent安装APK文件
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        i.setDataAndType(Uri.parse("file://" + apkfile.toString()),
            "application/vnd.android.package-archive");
        mContext.startActivity(i);
        android.os.Process.killProcess(android.os.Process.myPid());
    }*/

}
