package gbpe.policeass.utils.gps;

import android.app.Activity;
import android.content.Context;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.util.List;

/**
 * 通过GPS/AGPS定位(AGPS采用谷歌基站免费服务获取位置信息) <br/> 
 * Title: GpsManager.java <br/>
 * <br/> 
 *  
 * @author 丸子 
 * @version 0.0.1 
 */  
public class GpsManager {  
    private static String TAG = "GPS";  
  
    private LocationManager lm; // 位置管理
    private MyLocationListener mll; // 位置监听  
    private Location currentLocation; // 当前位置
    private Criteria ctr; // 定位标准
    private String provider; // 最佳提供者  
    private boolean isCanceled; // 是否取消  
    private Context mContext;
    private Handler handMessage;
  
    private static GpsManager instance;  
  
    private GpsManager() {  
    }  
  
    public static GpsManager getInstance() {  
        if (null == instance) {  
            instance = new GpsManager();  
        }  
  
        return instance;  
    }  
  
    private void load() {  
        // 获取系统服务  
        lm = (LocationManager) mContext.getSystemService(Activity.LOCATION_SERVICE);
  
        // 创建定位标准  
        ctr = new Criteria();  
        ctr.setAccuracy(Criteria.ACCURACY_FINE); // 设置精准度，待测试差距  
        ctr.setAltitudeRequired(false); // 设置是否返回海拔  
        ctr.setBearingRequired(false); // 设置是否返回方向  
        ctr.setCostAllowed(false); // 设置是否允许付费服务  
        ctr.setPowerRequirement(Criteria.POWER_HIGH); // 设置耗电等级  
        ctr.setSpeedRequired(false); // 设置是否返回速度信息  
  
        // 根据设置的定位条件，获取最佳provider对象，获取失败默认为GPS定位  
        provider = isNull(lm.getBestProvider(ctr, true), LocationManager.NETWORK_PROVIDER);  
        // 创建位置监听器  
        mll = new MyLocationListener();  
        // 开启GPS状态监听  
        new MyGpsStatusListener().start();  
        // 创建消息  
        handMessage = new HandlerMessage();  
  
        isCanceled = true;  
    }  
  
    /** 创建定位 */  
    public void create(Context mContext) {  
        if (null != this.mContext)  
            return;  
  
        this.mContext = mContext;  
  
        load();  
    }  
  
    /** 开始定位 */  
    public void start() {  
        handMessage.sendEmptyMessage(0);  
    }  
  
    /** 停止定位 */  
    public void stop() {  
        handMessage.sendEmptyMessage(1);  
    }  
  
    /** 重启定位 */  
    public void restart() {  
        handMessage.sendEmptyMessage(2);  
    }  
  
    /** 获取地址 */  
    public String getAddress() {  
        if (null == currentLocation)  
            return null;  
  
        StringBuffer sbAdds = new StringBuffer();  
  
        try {  
            // 创建地理位置编码  
            Geocoder gCod = new Geocoder(mContext);
  
            List<Address> lAdds = gCod
                    .getFromLocation(currentLocation.getLatitude(), currentLocation.getLongitude(), 1);  
  
            if (null == lAdds || lAdds.size() == 0 || null == lAdds.get(0))  
                return null;  
  
            Address adds = lAdds.get(0);  
  
            sbAdds.append(isNull(adds.getAddressLine(1), ""));  
            sbAdds.append(isNull(adds.getAddressLine(2), ""));  
  
            if (sbAdds.length() == 0) {  
                sbAdds.append(isNull(adds.getAdminArea(), ""));  
                sbAdds.append(isNull(adds.getLocality(), ""));  
                sbAdds.append(isNull(adds.getThoroughfare(), ""));  
                sbAdds.append(isNull(adds.getFeatureName(), ""));  
            }  
        } catch (Exception ex) {  
            if (null != ex && null != ex.getMessage()) {  
                Log.i(TAG, ex.getMessage());
            }  
        }  
  
        return sbAdds.toString();  
    }  
  
    /** 获取当前位置 */  
    public Location getCurrentLocation() {  
        return currentLocation;  
    }  
  
    /** GPS状态监听 */  
    private class MyGpsStatusListener extends Thread {  
        public void run() {  
            while (true) {  
                try {  
                    Thread.sleep(500);  
  
                    String newProvider = isNull(lm.getBestProvider(ctr, true), LocationManager.NETWORK_PROVIDER);  
                    // Log.i(TAG, String.format("状态：%s", newProvider));  
  
                    if (!provider.equals(newProvider)) {  
                        provider = newProvider;  
                        restart();  
                    }  
  
                    Thread.sleep(1000);  
                } catch (Exception ex) {  
                    ex.printStackTrace();  
                }  
            }  
        }  
    }  
  
    private class HandlerMessage extends Handler {  
        public void handleMessage(Message msg) {
            super.handleMessage(msg);  
  
            switch (msg.what) {  
            case 0:  
                // 绑定位置监听器  
                if (!isCanceled)  
                    return;  
  
                lm.requestLocationUpdates(provider, 0, 0, mll);  
                isCanceled = false;  
  
                Log.i(TAG, "开始定位");  
                break;  
            case 1:  
                // 取消绑定位置监听器  
                if (isCanceled)  
                    return;  
  
                lm.removeUpdates(mll);  
                isCanceled = true;  
  
                Log.i(TAG, "停止定位");  
                break;  
            case 2:  
                // 重新绑定位置监听器  
                if (!isCanceled) {  
                    lm.removeUpdates(mll);  
                }  
                lm.requestLocationUpdates(provider, 0, 0, mll);  
                isCanceled = true;  
  
                Log.i(TAG, "重启定位");  
                break;  
            }  
        }  
    }  
  
    /** 位置监听器 */  
    private class MyLocationListener implements LocationListener {
        // 位置发生变动时触发  
        public void onLocationChanged(Location location) {  
            currentLocation = location;  
            Log.i(TAG, "获取到了位置！");  
        }  
  
        // provider终止时调用  
        public void onProviderDisabled(String provider) {  
        }  
  
        // provider启动时调用  
        public void onProviderEnabled(String provider) {  
        }  
  
        // provider状态改变时调用  
        public void onStatusChanged(String provider, int status, Bundle extras) {
        }  
    }  
  
    private String isNull(String str, String defValue) {  
        if (null == str || str.length() == 0)  
            return defValue;  
  
        return str;  
    }  
}  