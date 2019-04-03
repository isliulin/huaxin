package gbpe.policeass.receiver;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.hardware.Camera;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;

import com.arcsoft.arcfacedemo.common.Constants;
import com.arcsoft.arcfacedemo.faceserver.CompareResult;
import com.arcsoft.arcfacedemo.faceserver.FaceServer;
import com.arcsoft.arcfacedemo.model.FacePreviewInfo;
import com.arcsoft.arcfacedemo.util.ConfigUtil;
import com.arcsoft.arcfacedemo.util.face.FaceHelper;
import com.arcsoft.arcfacedemo.util.face.FaceListener;
import com.arcsoft.face.ErrorInfo;
import com.arcsoft.face.FaceEngine;
import com.arcsoft.face.FaceFeature;
import com.arcsoft.face.FaceInfo;
import com.arcsoft.face.VersionInfo;

import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import gbpe.policeass.db.FaceItem;
import gbpe.policeass.db.FaceLab;
import gbpe.policeass.utils.AppLog;
import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;

public class FaceRegAndSearch /*extends Thread*/ {

    private static final String TAG = "FaceRegAndSearch";

    //public static final int ERROR_SECCESS = 0;
    //public static final int ERROR_INIT = -1;
    //public static final int ERROR_PATH = -2;
    //public static final int ERROR_FORMAT = -3;
    //public static final int ERROR_REGISTER = -4;

    private static FaceRegAndSearch mInstance;
    //private static List<RegInfo> infoList = null;
    //private static final Object lock = new Object();
    //private static Handler msgHandler;
    //public static int errCode;
    //private static Context mContext;
    private static FaceLab mfaceLab;
    private FaceServerLisener mFaceServerLisener;
    private static FaceServer mFaceServer;
    //private static int mWorkingState;
    //private Activity mActivity;
    private static final Object searchLock = new Object();

    private static ConcurrentHashMap<Integer, Integer> requestFeatureStatusMap = new ConcurrentHashMap<>();
    private static FaceHelper faceHelper;
    //private static List<CompareResult> compareResultList;
    //private static final float SIMILAR_THRESHOLD = 0.8F;
    //private static final int MAX_DETECT_NUM = 10;
    private static int regThreadState;

    //private static FaceFeature mfrFace;
    //private static Integer mRequestId;
    //private static FaceSearchListener mFaceSearchListener;
    //private static FaceHandleThread faceHandleThread;
    //private Thread mFaceThread;
    private static FaceEngine faceEngine;
    private static int afCode = -1;
    private static final int ACTION_REQUEST_PERMISSIONS = 0x001;
    //private static boolean isThreadRun = false;
    public static final int MAX_DETECT_NUM = 10;
    public static final String IMG_SUFFIX = ".jpg";

    /**
     * 所需的所有权限信息
     */
    private static final String[] NEEDED_PERMISSIONS = new String[]{
            Manifest.permission.CAMERA,
            Manifest.permission.READ_PHONE_STATE
    };

    public static FaceRegAndSearch getInstance(Context context) {

        if (mInstance == null) {
            mInstance = new FaceRegAndSearch(context);
        }
        return mInstance;
    }

    private FaceRegAndSearch(Context context){

        mfaceLab = FaceLab.get(context);
        mFaceServer = FaceServer.getInstance();

        if (!mFaceServer.isInit()) {
            if (!mFaceServer.init(context,mfaceLab.getFaces())) {
                AppLog.e (TAG, "face server init failed");
                //errCode = ERROR_INIT;
                mFaceServer.unInit();
                if (mFaceServerLisener != null) {
                    mFaceServerLisener.onRegisterError(new Exception("Face server init error"));
                }
                return;
            }
        }

    }
/*
    private FaceRegAndSearch(Context context,FaceFeature frFace, final Integer requestId, final FaceSearchListener listener) {
        //mfrFace = frFace;
        //mRequestId = requestId;
        //mFaceSearchListener = listener;
        mfaceLab = FaceLab.get(context);
        mFaceServer = FaceServer.getInstance();

        if (!mFaceServer.isInit()) {
            if (!mFaceServer.init(context, mfaceLab.getFaces())) {
                AppLog.e(TAG, "face server init failed");
                //errCode = ERROR_INIT;
                mFaceServer.unInit();
                if (mFaceServerLisener != null) {
                    mFaceServerLisener.onRegisterError(new Exception("Face server init error"));
                }
                return;
            }
        }

        FaceHandleThread faceHandleThread = FaceHandleThread.getInstance(mfaceLab,faceHelper,mFaceServer);

        if (faceHandleThread.getThreadRun() == false) {
            faceHandleThread.addSearchInfo(frFace, requestId, listener);
            faceHandleThread.start();
        }

    }*/

    public void startRegister(Context context, String url, FaceServerLisener lisener) {

        //FaceRegAndSearch faceRegAndSearch = new FaceRegAndSearch(context,url,lisener);
        /*synchronized (lock) {
            if (infoList==null) {
                infoList = new ArrayList<>();
            }
            infoList.add(new RegInfo(context,url));
        }*/


        mFaceServerLisener = lisener;

        FaceHandleThread faceHandleThread = FaceHandleThread.getInstance(mFaceServer);

        faceHandleThread.addRegResource(context, url);
        faceHandleThread.setWorkingState(FaceHandleThread.STATE_REGISTER);
        faceHandleThread.setFaceServerLisener(mFaceServerLisener);

        /*if (faceHandleThread.getThreadIdel() == false) {
            faceHandleThread.start();
        }*/


        /*
        if (faceHandleThread!=null) {
            //mWorkingState = STATE_REGISTER;
            FaceHandleThread.setWorkingState(FaceHandleThread.STATE_REGISTER);
            faceHandleThread.start();
        }else {
            //mWorkingState = STATE_REGISTER;
            FaceHandleThread.setWorkingState(FaceHandleThread.STATE_REGISTER);
        }
        */

    }

    public void startRegister(Context context, byte[] nv21, int w, int h, String name, FaceServerLisener lisener) {

        /*synchronized (lock) {
            if (infoList == null) {
                infoList = new ArrayList<>();
            }
            infoList.add(new RegInfo(context,nv21,name));
        }*/

        mFaceServerLisener = lisener;
        FaceHandleThread faceHandleThread = FaceHandleThread.getInstance(mFaceServer);
        faceHandleThread.addRegResource(context,nv21,name,w,h);
        faceHandleThread.setWorkingState(FaceHandleThread.STATE_REGISTER);
        faceHandleThread.setFaceServerLisener(mFaceServerLisener);

        /*if (faceHandleThread.getThreadRun() == false) {
            faceHandleThread.start();
        }*/



    }

    public void startSearch(FaceFeature frFace, Integer reqId, FaceSearchListener listener) {

        synchronized (searchLock) {

            FaceHandleThread faceHandleThread = FaceHandleThread.getInstance(/*faceHelper,*/ mFaceServer);
            if (!faceHandleThread.getThreadIdel()) {
                return;
            }
            faceHandleThread.setThreadRun(true);
            faceHandleThread.addSearchInfo(frFace,reqId,listener);
            faceHandleThread.setWorkingState(FaceHandleThread.STATE_SEARCH);
            //faceHandleThread.start();

        }

    }

    /**
     * 初始化引擎
     */
    public void initEngine(final Activity activity, final Observer<Integer> observer,EngineInitLister lister) {
        faceEngine = new FaceEngine();
        afCode = faceEngine.init(activity, FaceEngine.ASF_DETECT_MODE_VIDEO, /*ConfigUtil.getFtOrient(this)*/1,
                16, MAX_DETECT_NUM, FaceEngine.ASF_FACE_RECOGNITION | FaceEngine.ASF_FACE_DETECT | FaceEngine.ASF_LIVENESS);
        VersionInfo versionInfo = new VersionInfo();
        faceEngine.getVersion(versionInfo);
        Log.i(TAG, "initEngine:  init: " + afCode + "  version:" + versionInfo);

        if (afCode != ErrorInfo.MOK) {
            // 初始化失败，系统ota升级之后会有这种情况，重新注册
            //Toast.makeText(this, getString(R.string.init_failed, afCode), Toast.LENGTH_SHORT).show();
            if (lister!=null) {
                lister.onInitFailed();
            }
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    activeEngine(activity,null,observer);
                }
            },10000);
        } else {
            if (lister!=null) {
                lister.onInitSuccess();
            }
        }
    }

    private boolean checkPermissions(Context context, String[] neededPermissions) {
        if (neededPermissions == null || neededPermissions.length == 0) {
            return true;
        }
        boolean allGranted = true;
        for (String neededPermission : neededPermissions) {
            allGranted &= ContextCompat.checkSelfPermission(context, neededPermission) == PackageManager.PERMISSION_GRANTED;
        }
        return allGranted;
    }

    /**
     * 激活引擎
     *
     * @param view
     */

    public void activeEngine(final Activity activity, final View view, Observer<Integer> observer) {

        if (activity==null) {
            return;
        }

        if (!checkPermissions(activity,NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(activity, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
            return;
        }
        if (view != null) {
            view.setClickable(false);
        }
        Observable.create(new ObservableOnSubscribe<Integer>() {
            @Override
            public void subscribe(ObservableEmitter<Integer> emitter) {
                FaceEngine faceEngine = new FaceEngine();
                int activeCode = faceEngine.active(activity, Constants.APP_ID, Constants.SDK_KEY);
                emitter.onNext(activeCode);
            }
        })
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(observer);

    }

    public interface EngineInitLister{
        void onInitSuccess();
        void onInitFailed();
    }

    public interface FaceServerLisener {
        void onRegisterFinished(int suc, int failed);
        void onRegisterError(Exception e);
        void onSingleOk(FaceItem reg, int cnt);
        void onShowMsg(String msg);
    }

    public interface FaceSearchListener {
        //void onItemLeft();
        //void onItemInsert(CompareResult result);
        //void onInfoReady(String name);
        //boolean checkIsAdd(int reqId);
        //boolean isCompareListNull();
        void onCompareResult(CompareResult compareResult, Integer requestId);
        void onCompareError(Integer requestId);

    }


    // });
    //}


    private void unInitEngine() {

        if (afCode == ErrorInfo.MOK) {
            afCode = faceEngine.unInit();
            Log.i(TAG, "unInitEngine: " + afCode);
        }

    }

    public void unInitFaceHelper(Context context) {
        //faceHelper中可能会有FR耗时操作仍在执行，加锁防止crash
        if (faceHelper != null) {
            synchronized (faceHelper) {
                //unInitEngine();
            }
            ConfigUtil.setTrackId(context, faceHelper.getCurrentTrackId());
            faceHelper.release();
            faceHelper = null;
        }else {
            //unInitEngine();
        }

        // 关闭 FaceRecogniseActivity时，不能反初始化该服务，否则下发注册错误
        //FaceServer.getInstance().unInit();
    }

    public void InitFaceHelper(Camera.Size size, FaceListener faceListener, Context context) {

        if (faceHelper == null) {
            faceHelper =  new FaceHelper.Builder()
                    .faceEngine(faceEngine)
                    .frThreadNum(MAX_DETECT_NUM)
                    .previewSize(size)
                    .faceListener(faceListener)
                    .currentTrackId(ConfigUtil.getTrackId(context.getApplicationContext()))
                    .build();
        }
    }


    public List<FacePreviewInfo> onPreviewFrame(byte[] nv21) {
        return faceHelper.onPreviewFrame(nv21);
    }

    public String faceHelperGetName(int trackId) {
        return faceHelper.getName(trackId);
    }

    public void faceHelperAddName(int trackId,String name) {
        if (faceHelper != null) {
            faceHelper.addName(trackId,name);
        }
    }

    public void requestFaceFeature(byte[] nv21, FaceInfo faceInfo, int width, int height, int format, Integer trackId) {
        faceHelper.requestFaceFeature(nv21,faceInfo,width,height,format,trackId);
    }

    /*public FaceItem getLastRegister() {
        return FaceServer.getInstance().getLastRegister();
    }*/

}
