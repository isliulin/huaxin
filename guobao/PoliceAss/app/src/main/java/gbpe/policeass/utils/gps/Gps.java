package gbpe.policeass.utils.gps;
import android.content.Context;
import android.location.LocationManager;
import android.provider.Settings;

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2017/4/19 14:53
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2017/4/19 14:53
 *
 * 描述：打开或关闭GPS，但此方法需要系统权限，即需要将自己的应用变成系统应用
 * =====================================================================================
 */
public class Gps {

    /**
     * 打开GPS
     * @param context
     */
    public static void openGps(Context context){
        if (!GPSIsOPen(context)){
            Settings.Secure.setLocationProviderEnabled(context.getContentResolver(), LocationManager.GPS_PROVIDER, true);
        }
    }

    /**
     * 关闭GPS
     * @param context
     */
    public static void closeGps(Context context){
        if (GPSIsOPen(context)){
            Settings.Secure.setLocationProviderEnabled(context.getContentResolver(), LocationManager.GPS_PROVIDER, false);
        }
    }


    /**
     * 判断GPS是否开启，GPS或者AGPS开启一个就认为是开启的
     * @param context
     * @return true 表示开启
     */
    public static final boolean GPSIsOPen(final Context context) {
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
}
