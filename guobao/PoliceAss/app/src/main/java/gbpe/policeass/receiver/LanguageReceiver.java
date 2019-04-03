package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;

import java.util.Locale;

import gbpe.policeass.utils.AppLog;

public class LanguageReceiver extends BroadcastReceiver {

    private static final String TAG = "LanguageReceiver";
    @Override
    public void onReceive(Context context, Intent intent) {
        AppLog.i (TAG, "language changed");
        Locale locale = context.getResources().getConfiguration().locale;
        if (locale.equals(Locale.CHINESE)||locale.equals(Locale.SIMPLIFIED_CHINESE)) {
            AppLog.i (TAG, "set ime");
            try {
                Settings.Secure.putString(context.getContentResolver(), Settings.Secure.DEFAULT_INPUT_METHOD,"com.iflytek.inputmethod/.FlyIME");
            } catch (Exception e) {
                AppLog.e (TAG, "setIME error:"+e.getMessage());
            }
        }
    }
}
