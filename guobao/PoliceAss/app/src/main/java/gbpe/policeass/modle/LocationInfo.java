package gbpe.policeass.modle;

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2017/4/18 13:55
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2017/4/18 13:55
 *
 * 描述：拍摄视频时记录，位置移动的轨迹信息,即用户的经纬度变化值,同时包含用户名，警员编号，设备编号，
 * =====================================================================================
 */
public class LocationInfo {


    public  int id;
    public  String   longitude ;//经度
    public  String  latitude ;//纬度
    public   long    currentTime;//当前经纬度变化时的时间


    public long getCurrentTime() {
        return currentTime;
    }

    public void setCurrentTime(long currentTime) {
        this.currentTime = currentTime;
    }

    public LocationInfo(int id,String longitude,String latitude) {
        this.id         = id;
        this.longitude = longitude;
        this.latitude  = latitude;
    }
    public LocationInfo(String longitude,String latitude) {
        this.longitude = longitude;
        this.latitude  = latitude;
    }

     public  int getId() {
        return id;
    }

    public  void setId(int id) {
        this.id = id;
    }


    public  String getLongitude() {
        return longitude;
    }

    public  void setLongitude(String longitude) {
        this.longitude = longitude;
    }

    public  String getLatitude() {
        return latitude;
    }

    public  void setLatitude(String latitude) {
        this.latitude = latitude;
    }



}
