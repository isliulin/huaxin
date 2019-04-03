package gbpe.policeass.utils;

public class HardControl {

    private static final String TAG = "HardControl";

    public static native int virtPadOpen();
    public static native void virtPadClose();
    public static native int virtPadCtrl(int which, int status);
    public static native int cameraTuning(int scene);

    static {

        try{
            System.loadLibrary("hardcontrol");
        } catch (Exception e)
        {
            e.printStackTrace();
            AppLog.i(TAG, "load libhardcontrol.so failed");
        }

    }
}
