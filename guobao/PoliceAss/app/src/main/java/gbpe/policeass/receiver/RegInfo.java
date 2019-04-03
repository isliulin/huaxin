package gbpe.policeass.receiver;

import android.content.Context;

public class RegInfo {
    private Context context;
    private String url;
    private byte[] nv21;
    private boolean isNv21;
    private String name;
    private int width;
    private int height;


    public RegInfo(Context context, String url) {
        this.context = context;
        this.url = url;
        isNv21 = false;
    }

    public RegInfo(Context context, byte[] nv21,String name,int width, int height) {
        this.context = context;
        this.nv21 = nv21;
        this.name = name;
        isNv21 = true;
        this.width = width;
        this.height = height;
    }

    public Context getContext() {
        return context;
    }

    public String getUrl() {
        return url;
    }

    public byte[] getNv21() {
        return nv21;
    }

    public boolean isNv21() {
        return isNv21;
    }

    public String getName() {
        return name;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

}
