package gbpe.policeass.utils.gps;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;

/**
 * GPS定位服务类，封装了GPS类的细节，只向用户暴露简单的start()和stop()两个方法。需要用户实现{@link IGPSCallback}接口中的方法
 * <p>
 * 使用方法： <br>
 * GPSService gpsService = new GPSService(gpsCallback, MainActivity.this, true);
 * <br>
 * gpsService.start();
 *
 * @see IGPSCallback
 *
 * @author 优化设计，QQ:50199907 Email:yw1530@126.com
 *
 */
public class GPSService {
    /**
     * GPS函数回调接口
     */
    private IGPSCallback gpsCallback;

    private LocationManager loctionManager;

    private String provider;

    /**
     * GPSService是否运行
     */
    private boolean isRun;

    /**
     * @return GPS状态，true为正在运行，false已停止。
     */
    public boolean getIsRun() {
        return isRun;
    }

    /**
     * 超时停止
     */
    private boolean isAutoStop = false;

    /**
     * 定时器
     */
    private Timer timer;

    /**
     * 超时时间（秒）
     */
    private int outTime = 180;

    /**
     * 精度
     */
    private final float accuracy = 500.0f;

    /**
     * GPS配置参数
     */
    private Criteria criteria;

    /**
     * @return 获取criteria
     */
    public Criteria getCriteria() {
        initCriteria();
        return criteria;
    }

    /**
     * 初始化GPS参数
     */
    private void initCriteria() {
        if (criteria == null) {
            criteria = new Criteria();
            criteria.setAccuracy(Criteria.ACCURACY_FINE); // 高精度
            criteria.setAltitudeRequired(false); // 不要求海拔
            criteria.setBearingRequired(false); // 不要求方位
            criteria.setCostAllowed(true); // 允许有花费
            criteria.setPowerRequirement(Criteria.POWER_LOW);// 设置低功耗模式
        }
    }

    /**
     * 最后一次错误描述
     */
    private String lastErrorDescription = "";

    /**
     * @return 获取GPSSerivce最后一次出错的描述
     */
    public String getError() {
        return lastErrorDescription;
    }

    /**
     * 设置最后一次错误描述，该描述可以通过getError()方法获取。
     *
     * @see GPSService#getError()
     *
     * @param error
     *            错误说明
     */
    private void setError(String error) {
        if (error == null)
            return;
        this.lastErrorDescription = error;
    }

    /**
     * GPSService构造函数
     *
     * @param gpsCallback
     *            回调函数接口
     * @param context
     *            Context
     */
    public GPSService(IGPSCallback gpsCallback, Context context) {
        super();
        this.gpsCallback = gpsCallback;
        loctionManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

        initCriteria();

        /* 从可用的位置提供器中，匹配以上标准的最佳提供器 */
        provider = loctionManager.getBestProvider(criteria, true);
    }

    /**
     * GPSService构造函数
     *
     * @param gpsCallback
     *            回调函数接口
     * @param context
     *            Context
     * @param isAutoStop
     *            定位成功后是否自动停止GPS
     */
    public GPSService(IGPSCallback gpsCallback, Context context, boolean isAutoStop) {
        super();
        this.gpsCallback = gpsCallback;
        this.isAutoStop = isAutoStop;
        loctionManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

        initCriteria();

        // 从可用的位置提供器中，匹配以上标准的最佳提供器
        provider = loctionManager.getBestProvider(criteria, true);
    }

    /**
     * 返回超时时间（单位：秒）
     *
     * @return
     */
    public int getOutTime() {
        return outTime;
    }

    /**
     * 设置超时时间
     *
     * @param outTime
     *            超时时间（单位：秒，范围：10—600），只可在Start()方法调用前使用，默认180秒，如果小于10秒则超时无效，
     *            只能手动调用Stop() 方法停止GPS。
     */
    public void setOutTime(int outTime) {
        if (outTime >= 0 && outTime <= 600) {
            this.outTime = outTime;
        }
    }

    /**
     * 开始GPS定位
     *
     * @return 返回false表示GPS打开失败，可能没有硬件打开（由手机用户控制硬件开关）。
     */
    public boolean start() {
        try {
            if (!loctionManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
                // GPS没有打开
                setError("GPS没有硬件打开");
                gpsCallback.failCallBack(IGPSCallback.ERROR_CLOSE);
                return false;
            }

            if (this.outTime >= 10) {
                // 设置超时参数，启动定时器
                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        // 停止GPS
                        timer.cancel();
                        stop();
                        handler.sendEmptyMessage(0);
                    }
                }, 1000 * outTime);
            }

            // 注册监听函数
            if (locationListener != null) {
                loctionManager.requestLocationUpdates(provider, 1000 * 10, accuracy, locationListener);
            }

            isRun = true;
            return true;
        } catch (Exception e) {
            setError(e.getMessage());
            e.printStackTrace();
            return false;
        }
    }

    private Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case 0:
                gpsCallback.failCallBack(IGPSCallback.ERROR_OUTTIME);
                break;
            }
            super.handleMessage(msg);
        }
    };

    /**
     * 停止GPS定位
     * 
     * @return
     */
    public boolean stop() {
        try {
            if (locationListener != null) {
                loctionManager.removeUpdates(locationListener);
            }
            isRun = false;
            return true;
        } catch (Exception e) {
            setError(e.getMessage());
            e.printStackTrace();
            return false;
        }
    }

    /**
     * 位置监听器
     */
    private final LocationListener locationListener = new LocationListener() {
        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
        }

        @Override
        public void onProviderEnabled(String provider) {
        }

        @Override
        public void onProviderDisabled(String provider) {
        }

        // 当位置变化时触发
        @Override
        public void onLocationChanged(Location location) {
            if (location != null) {
                if (location.hasAccuracy() && location.getAccuracy() <= accuracy) {
                    // 是否自动停止
                    if (isAutoStop) {
                        stop();
                    }
                    gpsCallback.gpsCallback(location);
                }
            }
        }
    };
    public static void openGps(Context context){
        Intent settingsIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
              settingsIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
              context.startActivity(settingsIntent);
    }
}

