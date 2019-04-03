package gbpe.policeass.utils;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Parcelable;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;

import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class SdCardFormat {

    private static final String SD_CARD_PATH = SDCardUtils.getSDCardPath();

    //private Context mContext;

    public static void diskFormat(Context context) {
        ComponentName formatter = new ComponentName("android", "com.android.internal.os.storage.ExternalStorageFormatter");
        Intent intent = new Intent("com.android.internal.os.storage.FORMAT_ONLY"); // ExternalStorageFormatter.FORMAT_ONLY
        intent.setComponent(formatter);
        Parcelable sv = getStoragePath(context);
        intent.putExtra("storage_volume", sv);  // StorageVolume.EXTRA_STORAGE_VOLUME
        context.startService(intent);
    }

    // 获取 StorageVolume 对象
    private static Parcelable getStoragePath(Context mContext) {
        StorageManager mStorageManager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);
        Class<?> storageVolumeClazz = null;
        try {
            storageVolumeClazz = Class.forName("android.os.storage.StorageVolume");
            Method getVolumeList = mStorageManager.getClass().getMethod("getVolumeList");
            Method getPath = storageVolumeClazz.getMethod("getPath");
            Method isRemovable = storageVolumeClazz.getMethod("isRemovable");
            Object result = getVolumeList.invoke(mStorageManager);
            final int length = Array.getLength(result);
            for (int i = 0; i < length; i++) {
                Object storageVolumeElement = Array.get(result, i);
                String path = (String) getPath.invoke(storageVolumeElement);
                boolean removable = (Boolean) isRemovable.invoke(storageVolumeElement);
                if (removable && path.equals(SD_CARD_PATH)) {
                    return (Parcelable) storageVolumeElement;
                }
            }
        }  catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

}
