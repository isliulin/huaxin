package gbpe.policeass.utils.gps;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;

import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.utils.AppLog;

/**
 * Created by Administrator on 2016/12/6.
 */
public class GpsUtils {
    private  static LocationManager locationManager;
    //private  static String locationProvider;
    private static Context context;
    private static LocationListener mlocationListener;
    private static boolean isGpsStart = false;
//
    public GpsUtils(Context context) {
          this.context = context;
    }

    /**
     * 显示地理位置经度和纬度信息
     * @param location
     */

    private static void showLocation(Location location){
        String locationStr = "维度：" + location.getLatitude() +"\n"
                + "经度：" + location.getLongitude();
     }

    /**
     * LocationListern监听器
     * 参数：地理位置提供器、监听位置变化的时间间隔、位置变化的距离间隔、LocationListener监听器
     */
  /*
  public static   LocationListener locationListener =  new LocationListener() {

        @Override
        public void onStatusChanged(String provider, int status, Bundle arg2) {

        }

        @Override
        public void onProviderEnabled(String provider) {

        }

        @Override
        public void onProviderDisabled(String provider) {

        }

        @Override
        public void onLocationChanged(Location location) {
            //如果位置发生变化,重新显示
            showLocation(location);
            if (currLocation !=null){
                currLocation.getCurrLocation(location);
            }
        }
    };*/

    public static Location start(Context context, LocationListener locationListener){

        if (isGpsStart) {
            return null;
        }
        isGpsStart = true;
        //double latitude = 0.0;
        //double longitude = 0.0;
        //获取地理位置管理器
        mlocationListener = locationListener;
        locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        Criteria criteria = new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE);

        String provider = locationManager.getBestProvider(criteria,true);
        if (provider == null) {
            Toast.makeText(context, R.string.no_gps_tips,Toast.LENGTH_SHORT).show();
            return null;
        }
        /*
//        //获取所有可用的位置提供器
        List<String> providers = locationManager.getProviders(true);
        if(providers.contains(LocationManager.GPS_PROVIDER)){
           //如果是GPS
           locationProvider = LocationManager.GPS_PROVIDER;
        }else if(providers.contains(LocationManager.NETWORK_PROVIDER)){
        //如果是Network
            locationProvider = LocationManager.NETWORK_PROVIDER;
        }else{
            Toast.makeText(context, R.string.no_gps_tips,Toast.LENGTH_SHORT).show();
            return null;
        }*/

        if (!locationManager.isProviderEnabled(provider)) {
            Toast.makeText(context,R.string.gps_get_failed,Toast.LENGTH_SHORT).show();
            return null;
        }

        //获取Location
        /*
        Location location = null;
        while (location == null) {
            location = locationManager.getLastKnownLocation(provider);
        }*/
        Location location = locationManager.getLastKnownLocation(provider);

        if(location!=null){
            //不为空,显示地理位置经纬度
            showLocation(location);
        }
        if (locationListener != null) {
            //监视地理位置变化
            locationManager.requestLocationUpdates(provider, 3000, 1, mlocationListener);
        }

        return location;
    }

    public static void stop() {

        if (isGpsStart) {
            isGpsStart = false;
            if (mlocationListener != null) {
                locationManager.removeUpdates(mlocationListener);
            }
        }
    }

    public   static CurrLocation currLocation;

    public CurrLocation getCurrLocation() {
        return currLocation;
    }

    public void setCurrLocation(CurrLocation currLocation) {

        this.currLocation = currLocation;
    }

    /**
     *
     */
    public interface CurrLocation{
        void  getCurrLocation(Location location);
    }



    /**
     * 判断GPS是否开启，GPS或者AGPS开启一个就认为是开启的
     * @param context
     * @return true 表示开启
     */
    public static final boolean isOPen(final Context context) {
        LocationManager locationManager
                = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        // 通过GPS卫星定位，定位级别可以精确到街（通过24颗卫星定位，在室外和空旷的地方定位准确、速度快）
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        // 通过WLAN或移动网络(3G/2G)确定的位置（也称作AGPS，辅助GPS定位。主要用于在室内或遮盖物（建筑群或茂密的深林等）密集的地方定位）
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        if (gps || network) {
            return true;
        }

        return false;
    }


    /**
     * 强制帮用户打开GPS
     * @param context
     */
    public static final void openGPS(Context context) {
        Intent GPSIntent = new Intent();
        GPSIntent.setClassName("com.android.settings",
                "com.android.settings.widget.SettingsAppWidgetProvider");
        GPSIntent.addCategory("android.intent.category.ALTERNATIVE");
        GPSIntent.setData(Uri.parse("custom:3"));
        try {
            PendingIntent.getBroadcast(context, 0, GPSIntent, 0).send();
        } catch (PendingIntent.CanceledException e) {
            e.printStackTrace();
        }
    }
}
