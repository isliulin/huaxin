package gbpe.policeass.utils;

import android.app.Service;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;

/**
 * Created by Administrator on 2016/9/13.
 */
public class WiFiUtil {

    private WiFiUtil(){
        /* cannot be instantiated */
        throw new UnsupportedOperationException("cannot be instantiated");
    }
    public static void openWiFi(Context context){
        WifiManager wifiManager =(WifiManager) context.getSystemService(Service.WIFI_SERVICE);
        if (!wifiIsOpen(context)) {
            wifiManager.setWifiEnabled(true);
        }
    }
    public static  void stopWiFi(Context context){
        WifiManager wifiManager =(WifiManager) context.getSystemService(Service.WIFI_SERVICE);
        if (wifiIsOpen(context)) {
            wifiManager.setWifiEnabled(false);
        }
    }
   public static boolean wifiIsOpen(Context context) {
       WifiManager wifiManager =(WifiManager) context.getSystemService(Service.WIFI_SERVICE);
       return wifiManager.isWifiEnabled();
  }


}
