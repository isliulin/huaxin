package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.storage.StorageManager;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.text.TextUtils;
import android.widget.Toast;

import java.io.File;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.SettingTags;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.FileUtils;

public class UsbActionReceiver extends BroadcastReceiver {

    private static final String TAG = "UsbActionReceiver";
    private static final String ACTION_USB_STATE = "android.hardware.usb.action.USB_STATE";
    private static final String ACTION_USB_DEV_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    private static final String ACTION_USB_DEV_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";
    private static final String ACTION_MEDIA_MOUNTED = "android.intent.action.MEDIA_MOUNTED";
    private static final String ACTION_MEDIA_EJECT = "android.intent.action.MEDIA_EJECT";
    public static boolean isUsbConnected = false;
    public Handler usbHandler = null;
    public Runnable usbRunable = null;

    public void adbSet(Context context, boolean on) {
        try {
            if (on) {
                Settings.Global.putInt(context.getContentResolver(),Settings.Global.ADB_ENABLED,1);
            } else {
                Settings.Global.putInt(context.getContentResolver(),Settings.Global.ADB_ENABLED,0);
            }
        } catch (Exception e) {
            AppLog.e (TAG, "adb set error: "+e.getMessage());
        }
    }

    public boolean getAdbConfig() {
        //return false;
        String adb_en = Setup.getProperties("ADB");
        return (adb_en==null)?false:adb_en.equals("enable");
    }

    public void closeUsbInterface(Context context) {

        try{
            StorageManager storageManager = (StorageManager)context.getSystemService(Context.STORAGE_SERVICE);
            //Settings.Global.putInt(getContentResolver(),Settings.Global.ADB_ENABLED,0);
            Method method = storageManager.getClass().getMethod("isUsbMassStorageEnabled");
            boolean state = (boolean)method.invoke(storageManager);
            if (state) {   // 已连接
                method = storageManager.getClass().getMethod("disableUsbMassStorage");
                method.invoke(storageManager);
            }

        } catch (Exception e) {
            AppLog.i (TAG, "closeUsbInterface error: "+ e.toString());
        }

        try {
            SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
            SharedPreferences.Editor editor = sp.edit();
            editor.putString(SettingTags.KEY_USBMODE,context.getString(R.string.usb_forbiden));
            editor.commit();

        } catch (Exception e) {
            AppLog.e (TAG, "refrash error");

        }

    }


    private void getFilePath(String dirPath, List<String> videoFiles) {
        //List<String> videoFiles = new ArrayList<>();
        if (TextUtils.isEmpty(dirPath)) {
            return;
        }
        File dir  = new File(dirPath);
        File[] files = dir.listFiles();
        if ((files==null)||(files.length<=0)) {
            return;
        }
        for (int i=0;i<files.length;i++) {
            if (files[i].isDirectory()) {
                getFilePath(files[i].getAbsolutePath(),videoFiles);
            } else {
                if (files[i].getAbsolutePath().endsWith(Setup.videoFilePostfix)){
                    if (files[i].length() <= 0) {
                        videoFiles.add(files[i].getAbsolutePath());
                    }
                }
            }
        }
        return;
    }

    public void deleteBlankFile(Context context) {
        List<String> blankFiles = new ArrayList<>();
        getFilePath(FileUtils.getInnerFileSavedPaht(context),blankFiles);
        for (int i=0;i<blankFiles.size();i++) {
            File tmp = new File(blankFiles.get(i));
            if (tmp.exists()) {
                AppLog.i(TAG,"delete blank file: "+tmp.getAbsolutePath());
                tmp.delete();
            }
        }
    }

    private void onUsbState(Intent intent, final Context context) {

        AppLog.i (TAG, "Usb state, "+intent.getExtras().getBoolean("connected"));

        if (intent.getExtras().getBoolean("connected")) {
            isUsbConnected = true;
            // 刪除视频文件夹下的0文件
            deleteBlankFile(context);
        } else {
            isUsbConnected = false;
        }

        if (usbRunable==null) {
            usbRunable = new Runnable() {
                @Override
                public void run() {
                    if (isUsbConnected) {
                        if (getAdbConfig()) {
                            adbSet(context,true);
                        }
                    }else {
                        closeUsbInterface(context);
                    }

                    if (usbHandler!=null) {
                        usbHandler.removeCallbacks(usbRunable);
                        usbHandler = null;
                    }
                }
            };
        }

        if (usbHandler==null) {
            usbHandler = new Handler();
            usbHandler.postDelayed(usbRunable,5000);
        }
    }

    public void onUsbDeviceAttached(Context context) {
        Toast.makeText(context,"USB "+ context.getString(R.string.device) + context.getString(R.string.attached),Toast.LENGTH_SHORT).show();
    }

    public void onUsbDeviceDettached(Context context) {
        Toast.makeText(context,"USB "+ context.getString(R.string.device) + context.getString(R.string.dettached),Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();

        if (action.equals(ACTION_USB_STATE)) {
            onUsbState(intent,context);
        } else if (action.equals(ACTION_USB_DEV_ATTACHED)) {
            onUsbDeviceAttached(context);
        } else if (action.equals(ACTION_USB_DEV_DETACHED)) {
            onUsbDeviceDettached(context);
        } else if(action.equals(ACTION_MEDIA_MOUNTED)) {
            AppLog.i (TAG, "media mounted");
        } else if (action.equals(ACTION_MEDIA_EJECT)) {
            AppLog.i (TAG, "media eject");
        }
    }
}
