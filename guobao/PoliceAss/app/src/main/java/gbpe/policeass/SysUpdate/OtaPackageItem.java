package gbpe.policeass.SysUpdate;

public class OtaPackageItem {

    private String romid;
    private String versionCode;
    private String versionName;
    private String url;
    private String versionDesc;

    public String getRomid() {
        return romid;
    }

    public void setRomid(String romid) {
        this.romid = romid;
    }

    public String getVersionCode() {
        return versionCode;
    }

    public void setVersionCode(String versionCode) {
        this.versionCode = versionCode;
    }

    public String getVersionName() {
        return versionName;
    }

    public void setVersionName(String versionName) {
        this.versionName = versionName;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getVersionDesc() {
        return versionDesc;
    }

    public void setVersionDesc(String versionDesc) {
        this.versionDesc = versionDesc;
    }

}
