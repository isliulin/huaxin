package gbpe.policeass.zfysim;

import android.content.Context;
import android.os.Environment;
import android.support.annotation.StringRes;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.AppLog;

public class HomeItemLab {

    private List<HomeItem> homeItems;
    private static final String TAG = "HomeItemLab";
    private static HomeItemLab mHomeItemLab;
    private Context mContext;
    private static boolean EXT_FUNCTION = false;

    private HomeItem.ItemClicked inputPasswork(@StringRes int tag) {
        return new HomeItem.ItemClicked() {
            @Override
            public void onItemClicked(String tag) {
                  AppLog.i(TAG, tag + "clicked");
            }
        };
    }

    private boolean getExtFuntionEnable() {
        String ext_en = Setup.getProperties("EXT_FUNCTION");
        //return ext_en.equals("enable");
        return (ext_en==null)?false:ext_en.equals("enable");
    }

    private void buildHomeItems(){

        homeItems = new ArrayList<>();

        EXT_FUNCTION = getExtFuntionEnable();

        /*
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.video_browsing), R.drawable.spll,inputPasswork(mContext.getResources().getString(R.string.video_browsing))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.photo_browsing), R.drawable.tp,inputPasswork(mContext.getResources().getString(R.string.photo_browsing))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.sound_browsing), R.drawable.syll,inputPasswork(mContext.getResources().getString(R.string.sound_browsing))));

        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.compass), R.drawable.pass,inputPasswork(mContext.getResources().getString(R.string.compass))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.forth_gen_trans), R.drawable.sc,inputPasswork(mContext.getResources().getString(R.string.forth_gen_trans))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.usb_video_camera), R.drawable.wjsxt,inputPasswork(mContext.getResources().getString(R.string.usb_video_camera))));

        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.system_setting), R.drawable.setting,inputPasswork(mContext.getResources().getString(R.string.system_setting))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.journal), R.drawable.zfy,inputPasswork(mContext.getResources().getString(R.string.journal))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.phone), R.drawable.phone,inputPasswork(mContext.getResources().getString(R.string.phone))));

        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.ftp_setting), R.drawable.wsftp,inputPasswork(mContext.getResources().getString(R.string.ftp_setting))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.message), R.drawable.message,inputPasswork(mContext.getResources().getString(R.string.message))));
        homeItems.add(new HomeItem(mContext.getResources().getString(R.string.wifi_setting), R.drawable.sifi,inputPasswork(mContext.getResources().getString(R.string.wifi_setting))));
        */
        homeItems.add(new HomeItem(R.string.video_browsing, R.drawable.spll,inputPasswork(R.string.video_browsing)));
        homeItems.add(new HomeItem(R.string.photo_browsing, R.drawable.tp,inputPasswork(R.string.photo_browsing)));
        homeItems.add(new HomeItem(R.string.sound_browsing, R.drawable.syll,inputPasswork(R.string.sound_browsing)));

        homeItems.add(new HomeItem(R.string.compass, R.drawable.pass,inputPasswork(R.string.compass)));
        homeItems.add(new HomeItem(R.string.forth_gen_trans, R.drawable.voice_intercom,inputPasswork(R.string.forth_gen_trans)));
        homeItems.add(new HomeItem(R.string.usb_video_camera, R.drawable.wjsxt,inputPasswork(R.string.usb_video_camera)));

        homeItems.add(new HomeItem(R.string.system_setting, R.drawable.setting,inputPasswork(R.string.system_setting)));
        homeItems.add(new HomeItem(R.string.journal, R.drawable.zfy,inputPasswork(R.string.journal)));
        homeItems.add(new HomeItem(R.string.phone, R.drawable.phone,inputPasswork(R.string.phone)));

        homeItems.add(new HomeItem(R.string.ftp_setting, R.drawable.wsftp,inputPasswork(R.string.ftp_setting)));
        homeItems.add(new HomeItem(R.string.message, R.drawable.message,inputPasswork(R.string.message)));
        homeItems.add(new HomeItem(R.string.wifi_setting, R.drawable.sifi,inputPasswork(R.string.wifi_setting)));

        //if (EXT_FUNCTION) {
        if (true) {
            homeItems.add(new HomeItem(R.string.car_recognise, R.drawable.car_plate, inputPasswork(R.string.car_recognise)));
            homeItems.add(new HomeItem(R.string.face_recognise, R.drawable.face_recognise, inputPasswork(R.string.face_recognise)));
            homeItems.add(new HomeItem(R.string.id_recognise, R.drawable.id_recognise, inputPasswork(R.string.id_recognise)));
        }

    }

    private HomeItemLab(Context context){

        mContext = context;
        buildHomeItems();

    }

    public static HomeItemLab getInstance (Context context){
        if (mHomeItemLab == null) {
            mHomeItemLab = new HomeItemLab(context);
        }
        return mHomeItemLab;
    }

    public List<HomeItem> getHomeItems(){
        return homeItems;
    }

    public void refrash() {
        homeItems = null;
        buildHomeItems();
    }

}

/**
public class ErrorInfo {
    public static final int MOK = 0;
    public static final int MERR_UNKNOWN = 1;
    public static final int MERR_INVALID_PARAM = 2;
    public static final int MERR_UNSUPPORTED = 3;
    public static final int MERR_NO_MEMORY = 4;
    public static final int MERR_BAD_STATE = 5;
    public static final int MERR_USER_CANCEL = 6;
    public static final int MERR_EXPIRED = 7;
    public static final int MERR_USER_PAUSE = 8;
    public static final int MERR_BUFFER_OVERFLOW = 9;
    public static final int MERR_BUFFER_UNDERFLOW = 10;
    public static final int MERR_NO_DISKSPACE = 11;
    public static final int MERR_COMPONENT_NOT_EXIST = 12;
    public static final int MERR_GLOBAL_DATA_NOT_EXIST = 13;
    public static final int MERR_FSDK_INVALID_APP_ID = 28673;
    public static final int MERR_FSDK_INVALID_SDK_ID = 28674;
    public static final int MERR_FSDK_INVALID_ID_PAIR = 28675;
    public static final int MERR_FSDK_MISMATCH_ID_AND_SDK = 28676;
    public static final int MERR_FSDK_SYSTEM_VERSION_UNSUPPORTED = 28677;
    public static final int MERR_FSDK_LICENCE_EXPIRED = 28678;
    public static final int MERR_FSDK_FR_INVALID_MEMORY_INFO = 73729;
    public static final int MERR_FSDK_FR_INVALID_IMAGE_INFO = 73730;
    public static final int MERR_FSDK_FR_INVALID_FACE_INFO = 73731;
    public static final int MERR_FSDK_FR_NO_GPU_AVAILABLE = 73732;
    public static final int MERR_FSDK_FR_MISMATCHED_FEATURE_LEVEL = 73733;
    public static final int MERR_FSDK_FACEFEATURE_UNKNOWN = 81921;
    public static final int MERR_FSDK_FACEFEATURE_MEMORY = 81922;
    public static final int MERR_FSDK_FACEFEATURE_INVALID_FORMAT = 81923;
    public static final int MERR_FSDK_FACEFEATURE_INVALID_PARAM = 81924;
    public static final int MERR_FSDK_FACEFEATURE_LOW_CONFIDENCE_LEVEL = 81925;
    public static final int MERR_ASF_EX_FEATURE_UNSUPPORTED_ON_INIT = 86017;
    public static final int MERR_ASF_EX_FEATURE_UNINITED = 86018;
    public static final int MERR_ASF_EX_FEATURE_UNPROCESSED = 86019;
    public static final int MERR_ASF_EX_FEATURE_UNSUPPORTED_ON_PROCESS = 86020;
    public static final int MERR_ASF_EX_INVALID_IMAGE_INFO = 86021;
    public static final int MERR_ASF_EX_INVALID_FACE_INFO = 86022;
    public static final int MERR_ASF_ACTIVATION_FAIL = 90113;
    public static final int MERR_ASF_ALREADY_ACTIVATED = 90114;
    public static final int MERR_ASF_NOT_ACTIVATED = 90115;
    public static final int MERR_ASF_SCALE_NOT_SUPPORT = 90116;
    public static final int MERR_ASF_VERION_MISMATCH = 90117;
    public static final int MERR_ASF_DEVICE_MISMATCH = 90118;
    public static final int MERR_ASF_UNIQUE_IDENTIFIER_MISMATCH = 90119;
    public static final int MERR_ASF_PARAM_NULL = 90120;
    public static final int MERR_ASF_LIVENESS_EXPIRED = 90121;
    public static final int MERR_ASF_VERSION_NOT_SUPPORT = 90122;
    public static final int MERR_ASF_SIGN_ERROR = 90123;
    public static final int MERR_ASF_DATABASE_ERROR = 90124;
    public static final int MERR_ASF_UNIQUE_CHECKOUT_FAIL = 90125;
    public static final int MERR_ASF_COLOR_SPACE_NOT_SUPPORT = 90126;
    public static final int MERR_ASF_IMAGE_WIDTH_HEIGHT_NOT_SUPPORT = 90127;
    public static final int MERR_ASF_READ_PHONE_STATE_DENIED = 90128;
    public static final int MERR_ASF_ACTIVATION_DATA_DESTROYED = 90129;
    public static final int MERR_ASF_SERVER_UNKNOWN_ERROR = 90130;
    public static final int MERR_ASF_NETWORK_COULDNT_RESOLVE_HOST = 94209;
    public static final int MERR_ASF_NETWORK_COULDNT_CONNECT_SERVER = 94210;
    public static final int MERR_ASF_NETWORK_CONNECT_TIMEOUT = 94211;
    public static final int MERR_ASF_NETWORK_UNKNOWN_ERROR = 94212;
    int code = -1;

    ErrorInfo() {
    }

    int getCode() {
        return this.code;
    }
}

**/

