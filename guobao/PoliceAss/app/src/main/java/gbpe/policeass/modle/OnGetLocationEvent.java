package gbpe.policeass.modle;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/10/21 16:54
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/10/21 16:54
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class OnGetLocationEvent {

    public static final String GPS_START = "gb.gps.start";
    public static final String GPS_STOP = "gb.gps.stop";

    public OnGetLocationEvent(String msg) {
        this.msg = msg;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public boolean gpsState() {
        if (msg.equals(GPS_START)) {
            return true;
        } else if (msg.equals(GPS_STOP)){
            return false;
        } else {
            return false;
        }
    }

    String msg;
}
