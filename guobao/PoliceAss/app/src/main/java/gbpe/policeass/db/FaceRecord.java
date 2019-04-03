package gbpe.policeass.db;

import java.util.UUID;

public class FaceRecord {

    private UUID uuid;

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setTimeStamp(String timeStamp) {
        this.timeStamp = timeStamp;
    }

    private double longitude, latitude;
    private String name;
    private String timeStamp;

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    private String path;

    public String getUuid() {
        return uuid.toString();
    }

    public double getLongitude() {
        return longitude;
    }

    public double getLatitude() {
        return latitude;
    }

    public String getName() {
        return name;
    }

    public String getTimeStamp() {
        return timeStamp;
    }

    public FaceRecord (String name, String path, double latitude, double longitude, String timeStamp) {
        this(UUID.randomUUID());
        this.name = name;
        this.path = path;
        this.latitude = latitude;
        this.longitude = longitude;
        this.timeStamp = timeStamp;
    }

    public FaceRecord (UUID uuid) {
        this.uuid = uuid;
    }


}
