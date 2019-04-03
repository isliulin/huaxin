package gbpe.policeass.utils.gps;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.Intent;
import android.location.Address;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.util.List;
import java.util.Locale;

import gbpe.policeass.utils.Commons;

/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/4/19 15:37
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/4/19 15:37
 * 
 * 描述：
 * =====================================================================================
 */
public class LocationUtil implements LocationListener {

    private   final String TAG = "LocationService";
    private   final String SERVICE_NAME = "LocationService";

    private   final long MIN_TIME = 1000l;
    private   final float MIN_DISTANCE = 10f;

    private    LocationManager locationManager;
    private  Context context;

    public LocationUtil() {

    }

    public  void start(Context context){
        this.context = context;
        if (!Gps.GPSIsOPen(context)){
            Gps.openGps(context);
        }
        locationManager = (LocationManager) context.getSystemService(context.LOCATION_SERVICE);
        if (locationManager.getProvider(LocationManager.NETWORK_PROVIDER) != null || locationManager.getProvider(LocationManager.GPS_PROVIDER) != null) {
            Log.i(TAG, "正在定位");
			/*
			 * 进行定位
			 * provider:用于定位的locationProvider字符串
			 * minTime:时间更新间隔，单位：ms
			 * minDistance:位置刷新距离，单位：m
			 * listener:用于定位更新的监听者locationListener
			 */
            locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, MIN_TIME, MIN_DISTANCE, this);
        } else {
            Log.i(TAG, "无法定位");
            //无法定位：1、提示用户打开定位服务；2、跳转到设置界面
            Toast.makeText(context, "无法定位，请打开定位服务", Toast.LENGTH_SHORT).show();
            Intent i = new Intent();
            i.setAction(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            context.startActivity(i);
        }

    }


    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    //当坐标改变时触发此函数，如果Provider传进相同的坐标，它就不会被触发
    @Override
    public void onLocationChanged(Location location) {

        //得到纬度
        double latitude = location.getLatitude();
        //	double latitude = 22.733726;
        Log.i(TAG, "纬度 =" + latitude);
        //得到经度
        double longitude = location.getLongitude();
        //	double longitude = 113.910660;
        Log.i(TAG, "经度 =" + longitude);
        Log.i(TAG, "所有 =" + location.getProvider()+","+location.getAccuracy()+","+location.getAltitude()+","
                +location.getBearing()+","+location.getSpeed()+""+location.getElapsedRealtimeNanos());

        List<Address> locationList = getLocationList(latitude, longitude);
        if (!locationList.isEmpty()) {
            Address address = locationList.get(0);
            Log.i(TAG, "address =" + address);
            String countryName = address.getCountryName();
            Log.i(TAG, "countryName = " + countryName);
            String countryCode = address.getCountryCode();
            Log.i(TAG, "countryCode = " + countryCode);
            String locality = address.getLocality();
            Log.i(TAG, "locality = " + locality);
            for (int i = 0; address.getAddressLine(i) != null; i++) {
                String addressLine = address.getAddressLine(i);
                Log.i(TAG, "addressLine = " + addressLine);
            }
        }
        // 通知Activity
        Intent intent = new Intent();
        intent.setAction(Commons.LOCATION_ACTION);
        intent.putExtra(Commons.LOCATION, location.toString());
        context.sendBroadcast(intent);
        Log.i(TAG, "sendBroadcast");
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) {
        // Provider的转态在可用、暂时不可用和无服务三个状态直接切换时触发此函数
        if (status == LocationProvider.OUT_OF_SERVICE || status == LocationProvider.TEMPORARILY_UNAVAILABLE){
        }
    }

    @Override
    public void onProviderEnabled(String provider) {
        // Provider被enable时触发此函数，比如GPS被打开
    }

    @Override
    public void onProviderDisabled(String provider) {
        //Provider被disable时触发此函数，比如GPS被关闭
    }
    private List<Address> getLocationList(double latitude, double longitude) {
        Log.i(TAG, "getLocationList");
        Geocoder gc = new Geocoder(context, Locale.getDefault());
        List<Address> locationList = null;
        try {
            locationList = gc.getFromLocation(latitude, longitude, 1);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return locationList;
    }



}
