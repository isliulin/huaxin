package gbpe.policeass.utils.gps;
import android.location.Location;

/**
 * GPS定位服务回调函数接口，需要实现{@link IGPSCallback#gpsCallback(Location location)}和
 * {@link IGPSCallback#failCallBack(String error)}方法
 *
 * @see GPSService
 *
 * @author 优化设计，QQ:50199907 Email:yw1530@126.com
 *
 */
public interface IGPSCallback {
    /**
     * GPS无信号
     */
    public final static String ERROR_NO_SIGNAL = "GPS无信号";

    /**
     * GPS超时退出
     */
    public final static String ERROR_OUTTIME = "GPS超时退出";

    /**
     * GPS硬件没有打开
     */
    public final static String ERROR_CLOSE = "GPS硬件关闭";

    /**
     * GPS执行成功，返回Location时的回调函数
     *
     * @param location
     *            位置信息
     */
    void gpsCallback(Location location);

    /**
     * GPS错误时的回调函数 包括GPS无信号、GPS超时退出、GPS硬件没有打开
     *
     * @param error
     *            错误描述，一般为{@link IGPSCallback#ERROR_NO_SIGNAL}、
     *            {@link IGPSCallback#ERROR_OUTTIME}、
     *            {@link IGPSCallback#ERROR_CLOSE}。也可以由GPSService类自定义
     */
    void failCallBack(String error);
}