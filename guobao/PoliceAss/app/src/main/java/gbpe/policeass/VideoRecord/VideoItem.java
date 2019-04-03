package gbpe.policeass.VideoRecord;

/**
 * Created by Administrator on 2017/1/11.
 */
public class VideoItem {
    int id ;
    String startTime;
    String stopTime;
    String size;
    String path;
    String latitude;
    String longtitud;

    public VideoItem(int id,String startTime,String stopTime,
                     String size,String path,String latitude,String longtitud) {

        this.id = id;
        this.startTime = startTime;
        this.stopTime = stopTime;
        this.size = size;
        this.path = path;
        this.latitude = latitude;
        this.longtitud = longtitud;
    }
    public String getSize() {
        return size;
    }

    public void setSize(String size) {
        this.size = size;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public String getStopTime() {
        return stopTime;
    }

    public void setStopTime(String stopTime) {
        this.stopTime = stopTime;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public String getLatitude() {
        return latitude;
    }

    public void setLatitude(String latitude) {
        this.latitude = latitude;
    }

    public String getLongtitud() {
        return longtitud;
    }

    public void setLongtitud(String longtitud) {
        this.longtitud = longtitud;
    }
}
