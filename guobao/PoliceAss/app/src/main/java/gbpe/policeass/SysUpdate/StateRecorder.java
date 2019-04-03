package gbpe.policeass.SysUpdate;

import android.content.Context;
import android.preference.PreferenceManager;

public class StateRecorder {

    public static final String PREF_OTA_READY = "ota_package_ready";
    public static final String PREF_OTA_PATH = "ota_path";
    private Context context;

    public StateRecorder(Context context) {
        this.context = context;
    }

    public void setOtaReady(Boolean isRdy) {
        PreferenceManager.getDefaultSharedPreferences(context)
                .edit()
                .putBoolean(PREF_OTA_READY,isRdy)
                .commit();
    }

    public boolean getOtaReady(){
        boolean isReady = PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(PREF_OTA_READY,false);
        return isReady;
    }

    public void setOtaPath(String path) {
        PreferenceManager.getDefaultSharedPreferences(context)
                .edit()
                .putString(PREF_OTA_PATH,path)
                .commit();
    }

    public String getOtaPath (){
        String path = PreferenceManager.getDefaultSharedPreferences(context)
                .getString(PREF_OTA_PATH, "");
        return path;
    }

}
