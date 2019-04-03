package gbpe.policeass.Activity;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.arcsoft.arcfacedemo.faceserver.CompareResult;
import com.arcsoft.arcfacedemo.model.DrawInfo;
import com.arcsoft.arcfacedemo.model.FacePreviewInfo;
import com.arcsoft.arcfacedemo.util.ConfigUtil;
import com.arcsoft.arcfacedemo.util.DrawHelper;
import com.arcsoft.arcfacedemo.util.camera.CameraHelper;
import com.arcsoft.arcfacedemo.util.camera.CameraListener;
import com.arcsoft.arcfacedemo.util.face.FaceHelper;
import com.arcsoft.arcfacedemo.util.face.FaceListener;
import com.arcsoft.arcfacedemo.util.face.RequestFeatureStatus;
import com.arcsoft.arcfacedemo.widget.FaceRectView;
import com.arcsoft.arcfacedemo.widget.ProgressDialog;
import com.arcsoft.arcfacedemo.widget.ShowFaceInfoAdapter;
import com.arcsoft.face.AgeInfo;
import com.arcsoft.face.ErrorInfo;
import com.arcsoft.face.FaceEngine;
import com.arcsoft.face.FaceFeature;
import com.arcsoft.face.GenderInfo;
import com.arcsoft.face.LivenessInfo;
import com.topvision.videodemo.util.VideoSet;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import gbpe.policeass.Photograph.PhotoManager;
import gbpe.policeass.R;
import gbpe.policeass.SysUpdate.FileUtil;
import gbpe.policeass.db.FaceItem;
import gbpe.policeass.db.FaceLab;
import gbpe.policeass.db.FaceRcdLab;
import gbpe.policeass.db.FaceRecord;
import gbpe.policeass.receiver.FaceHandleThread;
import gbpe.policeass.receiver.FaceRegAndSearch;
import gbpe.policeass.sounds.BeatBox;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.views.progressBar.FaceRegInputDlg;
import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;

import static gbpe.policeass.receiver.FaceRegAndSearch.EngineInitLister;
import static gbpe.policeass.receiver.FaceRegAndSearch.FaceSearchListener;
import static gbpe.policeass.receiver.FaceRegAndSearch.FaceServerLisener;
//import static gbpe.policeass.receiver.FaceRegAndSearch.getInstance;

//import com.arcsoft.arcfacedemo.faceserver.FaceServer;

/* 此 Activity 为视频检测活体，人脸识别 */
public class FaceRecogniseActivity extends AppCompatActivity implements FaceSearchListener,
        FaceServerLisener{
    private static final String TAG = "RegisterAndRecognize";
    private static final String CAMERA_USED = "CameraUsed";
    private static final int MAX_DETECT_NUM = 10;
    private Toast toast = null;
    private Handler errHandler=null;
    private static final int FACE_MODE_IDEL = 0;
    private static final int FACE_MODE_RECOGNISE = 1;
    private static final int FACE_MODE_REGISTER = 2;
    //private int mFaceMode = FACE_MODE_IDEL;
    private Context context;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_HOME){
            startActivity(new Intent(FaceRecogniseActivity.this,FaceRecordActivity.class));
        }

        return super.onKeyUp(keyCode, event);
    }

    private boolean isEngineActived = false;
    //private Integer mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private int mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    private static String ROOT_DIR = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator
            + "FacesLearning" + File.separator;
    public static String BAT_REGISTER_DIR = ROOT_DIR + "samples";
    public static String REGISTER_FAILED_DIR = ROOT_DIR + "failed";
    private String HISTORY_DIR = ROOT_DIR + "recorder";
    public static String REGISTER_DIR = ROOT_DIR + "register";
    private ExecutorService executorService = Executors.newSingleThreadExecutor();
    ProgressDialog progressDialog = null;

    /**
     * 当FR成功，活体未成功时，FR等待活体的时间
     */
    private static final int WAIT_LIVENESS_INTERVAL = 50;
    private CameraHelper cameraHelper;
    private DrawHelper drawHelper;
    private Camera.Size previewSize;
    /**
     * 优先打开的摄像头
     */
    private Integer cameraID = Camera.CameraInfo.CAMERA_FACING_FRONT;
    //private FaceEngine faceEngine;
    //private FaceHelper faceHelper;
    private List<CompareResult> compareResultList;
    private ShowFaceInfoAdapter adapter;
    /**
     * 活体检测的开关
     */
    private boolean livenessDetect;

    /**
     * 注册人脸状态码，准备注册
     */
    private static final int REGISTER_STATUS_READY = 0;
    /**
     * 注册人脸状态码，注册中
     */
    private static final int REGISTER_STATUS_PROCESSING = 1;
    /**
     * 注册人脸状态码，注册结束（无论成功失败）
     */
    private static final int REGISTER_STATUS_DONE = 2;

    private int registerStatus = REGISTER_STATUS_DONE;

    private static final int RECOGNISE_STATUS_READY = 0;
    private static final int RECOGNISE_STATUS_PROCESSING = 1;
    private static final int RECOGNISE_STATUS_DONE = 0;
    private int recogniseStatus = RECOGNISE_STATUS_DONE;

    private int afCode = -1;
    private ConcurrentHashMap<Integer, Integer> requestFeatureStatusMap = new ConcurrentHashMap<>();
    private ConcurrentHashMap<Integer, Integer> livenessMap = new ConcurrentHashMap<>();
    private CompositeDisposable getFeatureDelayedDisposables = new CompositeDisposable();
    /**
     * 相机预览显示的控件，可为SurfaceView或TextureView
     */
    private View previewView;
    private TextView faceInfo;
    //private ImageButton mBtnSwCamera;
    private FaceRegAndSearch faceRegAndSearch;
    /**
     * 绘制人脸框的控件
     */
    private FaceRectView faceRectView;

    private Switch switchLivenessDetect;

    private static final int ACTION_REQUEST_PERMISSIONS = 0x001;
    private static final float SIMILAR_THRESHOLD = 0.8F;

    private static final int ERROR_MSG_INIT_ENGINE = 0x0001;
    private static final int UPDATE_FACE_INFO = 0x0002;
    private static final int DO_SOME_INIT = 0x0003;

    private FaceLab mFaceLab;

    private BeatBox mBeatBox;

    private Handler soundHandler;
    private EngineErrorHandle engineErrorHandle;

    public interface EngineErrorHandle{
        void onEngineInitError();
    }
    /**
     * 所需的所有权限信息
     */
    private static final String[] NEEDED_PERMISSIONS = new String[]{
            Manifest.permission.CAMERA,
            Manifest.permission.READ_PHONE_STATE
    };

    private boolean getLivenessCheck() {
        return PreferenceManager.getDefaultSharedPreferences(this).getBoolean(TAG,true);
    }

    private void setLivenessCheck(boolean isCheck) {
        PreferenceManager.getDefaultSharedPreferences(this)
                .edit()
                .putBoolean(TAG,isCheck)
                .commit();
    }

    private int getCameraUsed() {
        return PreferenceManager.getDefaultSharedPreferences(this).getInt(CAMERA_USED,Camera.CameraInfo.CAMERA_FACING_BACK);
    }

    private void setCameraUsed(int camUsed) {
        PreferenceManager.getDefaultSharedPreferences(this)
                .edit()
                .putInt(CAMERA_USED,camUsed)
                .commit();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Log.i ("Arcface", TAG+"onCreate");
        context = this;

        mBeatBox = new BeatBox(this);
        soundHandler = new Handler();

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_face_fragment);
        //保持亮屏
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        mCameraId = getCameraUsed();

        /* 获取人脸注册数据库 */
        mFaceLab = FaceLab.get(this);
        List<FaceItem> faces = mFaceLab.getFaces();
        for (int i=0;i<faces.size();i++) {
            AppLog.i (TAG, "face "+i+" :"+faces.get(i).getName());
        }

        /*
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            WindowManager.LayoutParams attributes = getWindow().getAttributes();
            attributes.systemUiVisibility = View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            getWindow().setAttributes(attributes);
        }*/

        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
/*
        // Activity启动后就锁定为启动时的方向
        switch (getResources().getConfiguration().orientation) {
            case Configuration.ORIENTATION_PORTRAIT:
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                break;
            case Configuration.ORIENTATION_LANDSCAPE:
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                break;
            default:
                break;
        }*/
        //本地人脸库初始化
        //FaceServer.getInstance().init(this, faces);
        //livenessDetect = getLivenessCheck();
        livenessDetect = false;     // 20190329, 暂时取消活体识别

        faceInfo = findViewById(R.id.face_info_text);
        previewView = findViewById(R.id.texture_preview);
        faceRectView = findViewById(R.id.face_rect_view);
        /*
        switchLivenessDetect = findViewById(R.id.switch_liveness_detect);
        switchLivenessDetect.setChecked(livenessDetect);
        switchLivenessDetect.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                livenessDetect = isChecked;
                setLivenessCheck(isChecked);
            }
        });*/

        //faceInfo.setTextColor(Color.rgb(0,0,0));
        RecyclerView recyclerShowFaceInfo = findViewById(R.id.recycler_view_person);
        compareResultList = new ArrayList<>();
        adapter = new ShowFaceInfoAdapter(compareResultList, this);
        recyclerShowFaceInfo.setAdapter(adapter);
        DisplayMetrics dm = getResources().getDisplayMetrics();
        int spanCount = (int) (dm.widthPixels / (getResources().getDisplayMetrics().density * 100 + 0.5f));
        recyclerShowFaceInfo.setLayoutManager(new GridLayoutManager(this, spanCount));
        recyclerShowFaceInfo.setItemAnimator(new DefaultItemAnimator());

        AppLog.i (TAG,"span count: "+spanCount);

        errHandler = new Handler(){

            @Override
            public void handleMessage(Message msg) {
                switch (msg.what){
                    case ERROR_MSG_INIT_ENGINE:
                        finish();
                        break;
                    case UPDATE_FACE_INFO:
                        //DrawInfo drawInfo = (DrawInfo)msg.obj;
                        //String info = new String("name: "+drawInfo.getName()+"\nsex: "+drawInfo.getSex()+"\nage: "+drawInfo.getAge());
                        //faceInfo.setText(info);
                        int resId = (int)msg.obj;
                        //Toast.makeText(context,resId,Toast.LENGTH_SHORT).show();
                        //AppLog.d(TAG,"INFO: "+resId);
                        break;
                }
                super.handleMessage(msg);
            }
        };

        progressDialog = new ProgressDialog(this);
        faceRegAndSearch = FaceRegAndSearch.getInstance(context);

        /*
        mBtnSwCamera = findViewById(R.id.switch_camera);
        mBtnSwCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (cameraHelper != null) {
                    cameraHelper.release();
                    cameraHelper = null;
                }
                if (mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                    mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
                } else {
                    mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
                }
                initCamera();
                AppLog.i(TAG,"switch camra to "+mCameraId);
            }
        });*/

        /*
        if (!checkPermissions(NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
        } else {
            //activeEngine(null);
            initEngine();
            initCamera();
        }*/

        initEngine();
        initCamera();

        File regDir = new File(REGISTER_DIR);
        if (!regDir.exists()) {
            regDir.mkdirs();
        }

        /*int sleepCnt = 0;
        while (isEngineActived == false) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (sleepCnt++ >50) {
                Toast.makeText(this,R.string.active_failed,Toast.LENGTH_SHORT).show();
                finish();
                return;
            }
        }*/
/*
        final Handler engHandler = new Handler();
        engHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if (isEngineActived){
                    initEngine();
                    initCamera();
                }else {
                    engHandler.postDelayed(this,300);
                }
            }
        },300);*/


    }

    private Observer<Integer> engineObserver = new Observer<Integer>() {
        @Override
        public void onSubscribe(Disposable d) {

        }

        @Override
        public void onNext(Integer activeCode) {
            if (activeCode == ErrorInfo.MOK) {
                showToast(getString(R.string.active_success));
                isEngineActived = true;
                //Message msg = new Message();
                //msg.what = DO_SOME_INIT;
                //errHandler.sendMessage(msg);
            } else if (activeCode == ErrorInfo.MERR_ASF_ALREADY_ACTIVATED) {
                showToast(getString(R.string.already_activated));
                isEngineActived = true;
                //Message msg = new Message();
                //msg.what = DO_SOME_INIT;
                //errHandler.sendMessage(msg);
            } else {
                showToast(getString(R.string.active_failed, activeCode));
                Message msg = new Message();
                msg.what = ERROR_MSG_INIT_ENGINE;
                errHandler.sendMessage(msg);
            }

           /* if (view != null) {
                view.setClickable(true);
            }*/
        }

        @Override
        public void onError(Throwable e) {

        }

        @Override
        public void onComplete() {
            //FaceRegAndSearch.getInstance(context).initEngine(FaceRecogniseActivity.this,engineObserver,null);
            faceRegAndSearch.initEngine(FaceRecogniseActivity.this,engineObserver,null);
        }
    };

    void initEngine() {
        //FaceRegAndSearch.getInstance(context).initEngine(this, engineObserver, new EngineInitLister() {
        faceRegAndSearch.initEngine(this, engineObserver, new EngineInitLister() {
            @Override
            public void onInitSuccess() {
                Toast.makeText(FaceRecogniseActivity.this,R.string.init_success,Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onInitFailed() {
                Toast.makeText(FaceRecogniseActivity.this,R.string.init_failed,Toast.LENGTH_SHORT).show();
                if (engineErrorHandle!=null) {
                    engineErrorHandle.onEngineInitError();
                }
            }
        });
    }

    /**
     * 激活引擎
     *
     * @param view
     */

    public void activeEngine(final View view) {

        //FaceRegAndSearch.getInstance(context).activeEngine(this,view,engineObserver);
        faceRegAndSearch.activeEngine(this,view,engineObserver);
        /*
        if (!checkPermissions(NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
            return;
        }
        if (view != null) {
            view.setClickable(false);
        }
        Observable.create(new ObservableOnSubscribe<Integer>() {
            @Override
            public void subscribe(ObservableEmitter<Integer> emitter) throws Exception {
                FaceEngine faceEngine = new FaceEngine();
                int activeCode = faceEngine.active(FaceRecogniseActivity.this, Constants.APP_ID, Constants.SDK_KEY);
                emitter.onNext(activeCode);
            }
        })
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Observer<Integer>() {
                    @Override
                    public void onSubscribe(Disposable d) {

                    }

                    @Override
                    public void onNext(Integer activeCode) {
                        if (activeCode == ErrorInfo.MOK) {
                            showToast(getString(R.string.active_success));
                            isEngineActived = true;
                            //Message msg = new Message();
                            //msg.what = DO_SOME_INIT;
                            //errHandler.sendMessage(msg);
                        } else if (activeCode == ErrorInfo.MERR_ASF_ALREADY_ACTIVATED) {
                            showToast(getString(R.string.already_activated));
                            isEngineActived = true;
                            //Message msg = new Message();
                            //msg.what = DO_SOME_INIT;
                            //errHandler.sendMessage(msg);
                        } else {
                            showToast(getString(R.string.active_failed, activeCode));
                            Message msg = new Message();
                            msg.what = ERROR_MSG_INIT_ENGINE;
                            errHandler.sendMessage(msg);
                        }

                        if (view != null) {
                            view.setClickable(true);
                        }
                    }

                    @Override
                    public void onError(Throwable e) {

                    }

                    @Override
                    public void onComplete() {
                        initEngine();
                    }
                });
                */

    }

    private void showToast(String s) {
        if (toast == null) {
            toast = Toast.makeText(this, s, Toast.LENGTH_SHORT);
            toast.show();
        } else {
            toast.setText(s);
            toast.show();
        }
    }


    /**
     * 初始化引擎
     */
    /*
    private void initEngine() {
        faceEngine = new FaceEngine();
        afCode = faceEngine.init(this, FaceEngine.ASF_DETECT_MODE_VIDEO, 1,
                16, MAX_DETECT_NUM, FaceEngine.ASF_FACE_RECOGNITION | FaceEngine.ASF_FACE_DETECT | FaceEngine.ASF_LIVENESS);
        VersionInfo versionInfo = new VersionInfo();
        faceEngine.getVersion(versionInfo);
        Log.i(TAG, "initEngine:  init: " + afCode + "  version:" + versionInfo);

        if (afCode != ErrorInfo.MOK) {
            // 初始化失败，系统ota升级之后会有这种情况，重新注册
            Toast.makeText(this, getString(R.string.init_failed, afCode), Toast.LENGTH_SHORT).show();
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    activeEngine(null);
                }
            },10000);
        }
    }*/

    @Override
    protected void onDestroy() {

        if (cameraHelper != null) {
            cameraHelper.release();
            cameraHelper = null;
        }
/*
        //faceHelper中可能会有FR耗时操作仍在执行，加锁防止crash
        if (faceHelper != null) {
            synchronized (faceHelper) {
                unInitEngine();
            }
            ConfigUtil.setTrackId(this, faceHelper.getCurrentTrackId());
            faceHelper.release();
        }else {
            unInitEngine();
        }*/

       //FaceRegAndSearch.getInstance(context).unInitFaceHelper(this);
        faceRegAndSearch.unInitFaceHelper(this);

        if (progressDialog != null && progressDialog.isShowing()) {
            progressDialog.dismiss();
        }
        if (getFeatureDelayedDisposables != null) {
            getFeatureDelayedDisposables.dispose();
            getFeatureDelayedDisposables.clear();
        }
        //FaceServer.getInstance().unInit();
        faceRegAndSearch = null;

        mBeatBox.release();
        registerStatus = -1;

        super.onDestroy();

    }

    private boolean checkPermissions(String[] neededPermissions) {
        if (neededPermissions == null || neededPermissions.length == 0) {
            return true;
        }
        boolean allGranted = true;
        for (String neededPermission : neededPermissions) {
            allGranted &= ContextCompat.checkSelfPermission(this, neededPermission) == PackageManager.PERMISSION_GRANTED;
        }
        return allGranted;
    }

    private void initCamera() {

        Log.i (TAG, "initCamera");

        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);

        final FaceListener faceListener = new FaceListener() {
            @Override
            public void onFail(Exception e) {
                Log.e(TAG, "onFail: " + e.getMessage());
            }

            //请求FR的回调
            @Override
            public void onFaceFeatureInfoGet(@Nullable final FaceFeature faceFeature, final Integer requestId) {
                // tag by ycb, 此处为识别到人脸时的调用
                Log.i(TAG, "onPreview: fr end = " + System.currentTimeMillis() + " trackId = " + requestId);
                //FR成功
                if (faceFeature != null) {
//                    Log.i(TAG, "onPreview: fr end = " + System.currentTimeMillis() + " trackId = " + requestId);

                    //不做活体检测的情况，直接搜索
                    if (!livenessDetect) {
                        AppLog.i (TAG, "No liveness detect");
                        //FaceRegAndSearch.startSearch(FaceRecogniseActivity.this,faceFeature,requestId,FaceRecogniseActivity.this);
                        faceRegAndSearch.startSearch(faceFeature,requestId,FaceRecogniseActivity.this);
                        //new RegThread(context,FaceRecordActivity.class).searchFace(faceFeature, requestId,FaceRecordActivity.class);
                        //RegThread.startInstance()
                    }
                    //活体检测通过，搜索特征
                    else if (livenessMap.get(requestId) != null && livenessMap.get(requestId) == LivenessInfo.ALIVE) {
                        AppLog.i (TAG, "Liveness pass, request id: "+requestId);
                        //searchFace(faceFeature, requestId);
                        //RegThread.startInstance(FaceRecogniseActivity.this,faceFeature,requestId,FaceRecogniseActivity.this);
                        //FaceRegAndSearch.startSearch(FaceRecogniseActivity.this,faceFeature,requestId,FaceRecogniseActivity.this);
                        faceRegAndSearch.startSearch(faceFeature,requestId,FaceRecogniseActivity.this);
                    }
                    //活体检测未出结果，延迟100ms再执行该函数
                    else if (livenessMap.get(requestId) != null && livenessMap.get(requestId) == LivenessInfo.UNKNOWN) {
                        getFeatureDelayedDisposables.add(Observable.timer(WAIT_LIVENESS_INTERVAL, TimeUnit.MILLISECONDS)
                                .subscribe(new Consumer<Long>() {
                                    @Override
                                    public void accept(Long aLong) {
                                        onFaceFeatureInfoGet(faceFeature, requestId);
                                    }
                                }));
                    }
                    //活体检测失败
                    else {
                        requestFeatureStatusMap.put(requestId, RequestFeatureStatus.NOT_ALIVE);
                    }

                }
                //FR 失败
                else {
                    requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
                }
            }

        };


        CameraListener cameraListener = new CameraListener() {
            @Override
            public void onCameraOpened(Camera camera, int cameraId, int displayOrientation, boolean isMirror) {
                previewSize = camera.getParameters().getPreviewSize();
                drawHelper = new DrawHelper(previewSize.width, previewSize.height, previewView.getWidth(), previewView.getHeight(), displayOrientation
                        , cameraId, isMirror);

                faceRegAndSearch.InitFaceHelper(previewSize,faceListener,context);
                //faceHelper = FaceRegAndSearch.getInstance(context).InitFaceHelper(previewSize,faceListener,context);


/*
                faceHelper = new FaceHelper.Builder()
                        .faceEngine(faceEngine)
                        .frThreadNum(MAX_DETECT_NUM)
                        .previewSize(previewSize)
                        .faceListener(faceListener)
                        .currentTrackId(ConfigUtil.getTrackId(FaceRecogniseActivity.this.getApplicationContext()))
                        .build();*/
            }


            @Override
            public void onPreview(final byte[] nv21, Camera camera) {
                if (faceRectView != null) {
                    faceRectView.clearFaceInfo();
                }

                //List<FacePreviewInfo> facePreviewInfoList = null;

                // 此处获取到人脸特征，并标记
                //List<FacePreviewInfo> facePreviewInfoList = faceHelper.onPreviewFrame(nv21);
                /* 每一帧都会获取图像人脸特征值，发现离开会立刻删掉，但是请求比对结果有一定延时
                * 所以当人脸从画面离开时，会采用前面某一帧的数据，会弹出识别到人脸*/
                List<FacePreviewInfo> facePreviewInfoList = faceRegAndSearch.onPreviewFrame(nv21);
                //if ((mFaceMode == FACE_MODE_REGISTER)||(mFaceMode == FACE_MODE_RECOGNISE)) {

                    // 此处获取到人脸特征，并标记
                    //facePreviewInfoList = faceHelper.onPreviewFrame(nv21);
                    if (facePreviewInfoList != null && faceRectView != null && drawHelper != null) {
                        List<DrawInfo> drawInfoList = new ArrayList<>();
                        for (int i = 0; i < facePreviewInfoList.size(); i++) {
                            //String name = faceHelper.getName(facePreviewInfoList.get(i).getTrackId());
                            String name = faceRegAndSearch.faceHelperGetName(facePreviewInfoList.get(i).getTrackId());
                            drawInfoList.add(new DrawInfo(facePreviewInfoList.get(i).getFaceInfo().getRect(),    // Rect
                                            GenderInfo.UNKNOWN,                                                  // gender
                                            AgeInfo.UNKNOWN_AGE,                                                 // age
                                            LivenessInfo.UNKNOWN,                                                // liveness
                                            name == null ? String.valueOf(facePreviewInfoList.get(i).getTrackId()) : name));
                        }
                        // 取消面部框，
                        //drawHelper.draw(faceRectView, drawInfoList);
                    }
                //}

                //if (mFaceMode == FACE_MODE_REGISTER) {
                    if (registerStatus == REGISTER_STATUS_READY && facePreviewInfoList != null && facePreviewInfoList.size() > 0) {
                        registerStatus = REGISTER_STATUS_PROCESSING;
                        Observable.create(new ObservableOnSubscribe<Boolean>() {
                            @Override
                            public void subscribe(final ObservableEmitter<Boolean> emitter) {
                                //boolean success = FaceServer.getInstance().register(FaceRecogniseActivity.this, nv21.clone(), previewSize.width, previewSize.height, "registered " + faceHelper.getCurrentTrackId());
                                //boolean success = FaceRegAndSearch.sta
                                //String regFile = REGISTER_DIR + File.separator + System.currentTimeMillis() + ".jpg";

                                //AppLog.i (TAG, "register: "+regFile);

                                //try {
                                    //PhotoManager.saveToSDCard(regFile,nv21);
                                    //FaceRegAndSearch.startRegister(FaceRecogniseActivity.this, regFile, new FaceRegAndSearch.FaceServerLisener() {
                                faceRegAndSearch.startRegister(FaceRecogniseActivity.this,nv21,previewSize.width,previewSize.height,
                                            null,new FaceServerLisener(){;
                                        @Override
                                        public void onRegisterFinished(int suc, int failed) {
                                            emitter.onNext(true);
                                        }

                                        @Override
                                        public void onRegisterError(Exception e) {
                                            emitter.onNext(false);
                                        }

                                        @Override
                                        public void onSingleOk(FaceItem reg, int cnt) {

                                        }

                                        @Override
                                        public void onShowMsg(String msg) {

                                        }
                                    });

                                //} catch (IOException e) {
                                //    e.printStackTrace();
                                //}

                            }
                        })
                                .subscribeOn(Schedulers.computation())
                                .observeOn(AndroidSchedulers.mainThread())
                                .subscribe(new Observer<Boolean>() {
                                    @Override
                                    public void onSubscribe(Disposable d) {

                                    }

                                    @Override
                                    public void onNext(Boolean success) {
                                        String result = success ? "register success!" : "register failed!";
                                        //Toast.makeText(context, result, Toast.LENGTH_SHORT).show();
                                        //registerStatus = REGISTER_STATUS_DONE;
                                        //mFaceMode = FACE_MODE_IDEL;
                                        if (success) {
                                            AppLog.i(TAG,"face register path: "+ FaceHandleThread.getLastRegFace()/*FaceServer.getInstance().getLastRegister()*/);
                                            //final FaceItem regFace = FaceRegAndSearch.getInstance(FaceRecogniseActivity.this).getLastRegister();
                                            final FaceItem regFace = FaceHandleThread.getLastRegFace();
                                            final FaceRegInputDlg  faceRegInputDlg = new FaceRegInputDlg(context,BitmapFactory.decodeFile(regFace.getImgPath()));
                                            faceRegInputDlg.setOnPositiveListener(new View.OnClickListener(){

                                                @Override
                                                public void onClick(View v) {
                                                    // 此处存储注册的名称，id， 备注， 和照片的名称建立数据库
                                                    //FaceItem regFace = FaceServer.getInstance().getLastRegister();
                                                    //mFaceLab.addFaceItem(new FaceItem(faceRegInputDlg.getName(),faceRegInputDlg.getId(),faceRegInputDlg.getRemark(),FaceServer.getInstance().getLastRegister(),null));
                                                    if (faceRegInputDlg.getName() != null) {
                                                        regFace.setName(faceRegInputDlg.getName());
                                                    }else {
                                                        regFace.setName(regFace.getUUID());
                                                    }

                                                    if (faceRegInputDlg.getId() != null) {
                                                        regFace.setId(faceRegInputDlg.getId());
                                                    }
                                                    if (faceRegInputDlg.getRemark() != null) {
                                                        regFace.setRemark(faceRegInputDlg.getRemark());
                                                    }
                                                    mFaceLab.addFaceItem(regFace);

                                                    AppLog.i(TAG, "register name: "+regFace.getName()+", id: "+regFace.getId()
                                                            +", remark: "+regFace.getRemark());

                                                    registerStatus = REGISTER_STATUS_DONE;
                                                    recogniseStatus = RECOGNISE_STATUS_DONE;

                                                    faceRegInputDlg.dismiss();
                                                }
                                            });

                                            faceRegInputDlg.setOnNegativeListener(new View.OnClickListener() {
                                                @Override
                                                public void onClick(View v) {
                                                    registerStatus = REGISTER_STATUS_DONE;
                                                    recogniseStatus = RECOGNISE_STATUS_DONE;
                                                    faceRegInputDlg.dismiss();

                                                }
                                            });
                                            faceRegInputDlg.show();
                                        }
                                    }

                                    @Override
                                    public void onError(Throwable e) {
                                        Toast.makeText(FaceRecogniseActivity.this, "register failed!", Toast.LENGTH_SHORT).show();
                                        registerStatus = REGISTER_STATUS_DONE;
                                        recogniseStatus = RECOGNISE_STATUS_DONE;
                                        //mFaceMode = FACE_MODE_IDEL;
                                    }

                                    @Override
                                    public void onComplete() {

                                    }
                                });
                    }
                //}

                /**
                * 此处为活体识别，人脸识别后显示识别结果
                 * * */
                //if (mFaceMode == FACE_MODE_RECOGNISE) {

                    if ((recogniseStatus == RECOGNISE_STATUS_READY)&&(registerStatus == REGISTER_STATUS_DONE)) {
                        //此处解决离开识别一次的问题
                        clearLeftFace(facePreviewInfoList);

                        if (facePreviewInfoList != null && facePreviewInfoList.size() > 0 && previewSize != null) {

                            //clearLeftFace(facePreviewInfoList);

                            recogniseStatus = RECOGNISE_STATUS_PROCESSING;
                            for (int i = 0; i < facePreviewInfoList.size(); i++) {
                                if (livenessDetect) {   // 活体检测
                                    livenessMap.put(facePreviewInfoList.get(i).getTrackId(), facePreviewInfoList.get(i).getLivenessInfo().getLiveness());
                                }
                                // 对于每个人脸，若状态为空或者为失败，则请求FR（可根据需要添加其他判断以限制FR次数），
                                // FR回传的人脸特征结果在{@link FaceListener#onFaceFeatureInfoGet(FaceFeature, Integer)}中回传
                                //AppLog.i (TAG, "before requestFaceFeature");
                                /*if (requestFeatureStatusMap.get(facePreviewInfoList.get(i).getTrackId()) == null
                                        || requestFeatureStatusMap.get(facePreviewInfoList.get(i).getTrackId()) == RequestFeatureStatus.FAILED) {
                                    requestFeatureStatusMap.put(facePreviewInfoList.get(i).getTrackId(), RequestFeatureStatus.SEARCHING);
                                    faceHelper.requestFaceFeature(nv21, facePreviewInfoList.get(i).getFaceInfo(), previewSize.width, previewSize.height, FaceEngine.CP_PAF_NV21, facePreviewInfoList.get(i).getTrackId());
                                    AppLog.i(TAG, "onPreview: fr start = " + System.currentTimeMillis() + " trackId = " + facePreviewInfoList.get(i).getTrackId());
                                }*/
                                /* 以上为facePreview中没有识别，或者识别错误时，才会启动requestFaceFeature，而现在的需求是按识别按钮后再识别*/
                                /* 根据 track id 获取人脸特征*/
                                requestFeatureStatusMap.put(facePreviewInfoList.get(i).getTrackId(), RequestFeatureStatus.SEARCHING);
                                //faceHelper.requestFaceFeature(nv21, facePreviewInfoList.get(i).getFaceInfo(), previewSize.width, previewSize.height, FaceEngine.CP_PAF_NV21, facePreviewInfoList.get(i).getTrackId());
                                faceRegAndSearch.requestFaceFeature(nv21,
                                                                    facePreviewInfoList.get(i).getFaceInfo(),
                                                                    previewSize.width,
                                                                    previewSize.height,
                                                                    FaceEngine.CP_PAF_NV21,
                                                                    facePreviewInfoList.get(i).getTrackId());

                                // 延时1000ms重新启动，防止反复启动
                                // 尝试放到onCompareResult里处理
                                /*
                                new Handler().postDelayed(new Runnable() {
                                    @Override
                                    public void run() {
                                        recogniseStatus = RECOGNISE_STATUS_DONE;
                                        mFaceMode = FACE_MODE_IDEL;
                                    }
                                },1000);*/

							}
                            //recogniseStatus = RECOGNISE_STATUS_DONE;
                            //mFaceMode = FACE_MODE_IDEL;
                        } else {
                            //Toast.makeText(,R.string.no_alive_tips,Toast.LENGTH_SHORT).show();
                            //Message msg = new Message();
                            //msg.what = UPDATE_FACE_INFO;
                            //msg.obj = R.string.no_alive_tips;
                            //errHandler.sendMessage(msg);
                            //recogniseStatus = RECOGNISE_STATUS_DONE;
                            //mFaceMode = FACE_MODE_IDEL;
                        }
                    }
                //}

            }

            @Override
            public void onCameraClosed() {
                Log.i(TAG, "onCameraClosed: ");
            }

            @Override
            public void onCameraError(Exception e) {
                Log.i(TAG, "onCameraError: " + e.getMessage());
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(context,R.string.open_camera_err,Toast.LENGTH_SHORT).show();
                    }
                });
            }

            @Override
            public void onCameraConfigurationChanged(int cameraID, int displayOrientation) {
                if (drawHelper != null) {
                    drawHelper.setCameraDisplayOrientation(displayOrientation);
                }
                Log.i(TAG, "onCameraConfigurationChanged: " + cameraID + "  " + displayOrientation);
            }
        };
        cameraHelper = new CameraHelper.Builder()
                .metrics(metrics)
                .rotation(getWindowManager().getDefaultDisplay().getRotation())
                //.specificCameraId(cameraID != null ? cameraID : Camera.CameraInfo.CAMERA_FACING_FRONT)
                .specificCameraId(mCameraId)
                .isMirror(false)
                .previewOn(previewView)
                .cameraListener(cameraListener)
                .build();
        cameraHelper.init();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == ACTION_REQUEST_PERMISSIONS) {
            boolean isAllGranted = true;
            for (int grantResult : grantResults) {
                isAllGranted &= (grantResult == PackageManager.PERMISSION_GRANTED);
            }
            if (isAllGranted) {
                activeEngine(null);
                //initEngine();
                //initCamera();
                if (cameraHelper != null) {
                    cameraHelper.start();
                }
            } else {
                Toast.makeText(this, R.string.permission_denied, Toast.LENGTH_SHORT).show();
            }
        }
    }

    /**
     * 删除已经离开的人脸
     *
     * @param facePreviewInfoList 人脸和trackId列表
     */
    private void clearLeftFace(List<FacePreviewInfo> facePreviewInfoList) {
        Set<Integer> keySet = requestFeatureStatusMap.keySet();
        if (compareResultList != null) {
            for (int i = compareResultList.size() - 1; i >= 0; i--) {
                if (!keySet.contains(compareResultList.get(i).getTrackId())) {
                    compareResultList.remove(i);
                    adapter.notifyItemRemoved(i);
                }
            }
        }
        if (facePreviewInfoList == null || facePreviewInfoList.size() == 0) {
            requestFeatureStatusMap.clear();
            livenessMap.clear();
            return;
        }

        for (Integer integer : keySet) {
            boolean contained = false;
            for (FacePreviewInfo facePreviewInfo : facePreviewInfoList) {
                if (facePreviewInfo.getTrackId() == integer) {
                    contained = true;
                    break;
                }
            }
            if (!contained) {
                requestFeatureStatusMap.remove(integer);
                livenessMap.remove(integer);
            }
        }

    }

    /**
     * 将准备注册的状态置为{@link #REGISTER_STATUS_READY}
     *
     * @param view 注册按钮
     */
    public void register(View view) {
    /*
        if ((registerStatus == REGISTER_STATUS_DONE) && (recogniseStatus == RECOGNISE_STATUS_DONE)){
            registerStatus = REGISTER_STATUS_READY;
            mFaceMode = FACE_MODE_REGISTER;
            Toast.makeText(this,R.string.start_register_tips,Toast.LENGTH_SHORT).show();
        } else {
			Toast.makeText(this,R.string.service_busy_tips,Toast.LENGTH_SHORT).show();
		}*/
		if (registerStatus == REGISTER_STATUS_DONE) {
            registerStatus = REGISTER_STATUS_READY;
        }
    }

/*
    public void recognise(View view){
        if ((registerStatus == REGISTER_STATUS_DONE) && (recogniseStatus == RECOGNISE_STATUS_DONE)) {
			//AppLog.i (TAG,"begin recognise");
            recogniseStatus = RECOGNISE_STATUS_READY;
            mFaceMode = FACE_MODE_RECOGNISE;
            Toast.makeText(this,R.string.start_recognise_tips,Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(this,R.string.service_busy_tips,Toast.LENGTH_SHORT).show();
        }

    }*/

    /** 切换摄像头 **/
    public void switchFaceCamera(View v) {
        if (cameraHelper != null) {
            cameraHelper.release();
            cameraHelper = null;
        }
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        } else {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        }
        initCamera();
        cameraHelper.start();
        setCameraUsed(mCameraId);
        //onDestroy();
        //onCreate(null);
        AppLog.i(TAG,"switch camra to "+mCameraId);
    }

    public void batchRegister(View view) {
        File dir  = new File (BAT_REGISTER_DIR);
        if (!dir.exists()|| !dir.isDirectory()) {
            Toast.makeText(this,R.string.batch_no_exit_tips,Toast.LENGTH_SHORT).show();
            return;     // 20181217
        }

        registerStatus = REGISTER_STATUS_PROCESSING;;

        File[] files = dir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String name) {
                return (name.endsWith(FaceRegAndSearch.IMG_SUFFIX));
            }
        });

        if (files == null) {
            registerStatus = REGISTER_STATUS_DONE;
            return;
        }

        progressDialog.setMaxProgress(files.length);
        progressDialog.show();

        faceRegAndSearch.startRegister(this, dir.getAbsolutePath(), new FaceServerLisener() {
            @Override
            public void onRegisterFinished(final int suc, final int failed) {
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.dismiss();
                        String info = context.getString(R.string.success)+" "+suc+", "+context.getString(R.string.failed)+" "+failed;
                        Toast.makeText(context,info,Toast.LENGTH_SHORT).show();
                    }
                });
                registerStatus = REGISTER_STATUS_DONE;
                AppLog.i(TAG, "onRegisterFinished");
            }

            @Override
            public void onRegisterError(Exception e) {
                AppLog.e(TAG, "register error: "+ e);
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.dismiss();
                        String info = context.getString(R.string.register)+" "+context.getString(R.string.failed);
                        Toast.makeText(context,info,Toast.LENGTH_SHORT).show();
                    }
                });
                registerStatus = REGISTER_STATUS_DONE;
            }

            @Override
            public void onSingleOk(FaceItem reg, final int cnt) {
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.refreshProgress(cnt);
                    }
                });
            }

            @Override
            public void onShowMsg(String msg) {

            }
        });

    }

    /*
    public void batchRegister(View v) {
        File dir  = new File (BAT_REGISTER_DIR);
        if (!dir.exists()|| !dir.isDirectory()) {
            Toast.makeText(this,R.string.batch_no_exit_tips,Toast.LENGTH_SHORT).show();
            return;     // 20181217
        }

        final File[] jpgFiles = dir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String name) {
                return name.endsWith(FaceServer.IMG_SUFFIX);
            }
        });

        //int totalCount = jpgFiles.length;
        progressDialog.setMaxProgress(jpgFiles.length);
        progressDialog.show();
        //Intent intent = new Intent(FaceSampleReceiver.FACE_DET_MSG);
        //intent.putExtra(FaceSampleReceiver.PATH, BAT_REGISTER_DIR);
        //sendBroadcast(intent);
        FaceSampleReceiver.nativeRegister(BAT_REGISTER_DIR,this, new FaceServerLisener(){
            @Override
            public void onRegisterFinished(final int suc, final int failed) {
                //progressDialog.dismiss();
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.dismiss();
                        String info = context.getString(R.string.success)+" "+suc+", "+context.getString(R.string.failed)+" "+failed;
                        Toast.makeText(context,info,Toast.LENGTH_SHORT).show();
                    }
                });
                AppLog.i(TAG, "onRegisterFinished");
            }

            @Override
            public void onRegisterError(Exception e) {
                //progressDialog.dismiss();
                //Toast.makeText(context,R.string.register_failed,Toast.LENGTH_SHORT).show();
                AppLog.e(TAG, "register error: "+ e);
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.dismiss();
                    }
                });
            }

            @Override
            public void onSingleOk(final int cnt) {
                //progressDialog.refreshProgress(cnt);
                errHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.refreshProgress(cnt);
                    }
                });
            }

            @Override
            public void onShowMsg(String msg) {

            }
        });

        executorService.execute(new Runnable() {
        //new Thread(new Runnable() {
            @Override
            public void run() {
                //AppLog.i (TAG, "开始批量注册");
                final int totalCount = jpgFiles.length;
                int successCount = 0;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.setMaxProgress(totalCount);
                        progressDialog.show();
                    }
                });

                for (int i=0;i<totalCount;i++) {
                    final int finalI = i;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (progressDialog!=null) {
                                progressDialog.refreshProgress(finalI);
                            }
                        }
                    });
                    final File jpgFile = jpgFiles[i];
                    Bitmap bitmap = BitmapFactory.decodeFile(jpgFile.getAbsolutePath());
                    //AppLog.i (TAG, "No."+i+": "+jpgFile.getName()+" 开始注册");
                    if (bitmap == null) {
                        File failedFile = new File(REGISTER_FAILED_DIR + File.separator + jpgFile.getName());
                        if (!failedFile.getParentFile().exists()) {
                            failedFile.getParentFile().mkdirs();
                        }
                        jpgFile.renameTo(failedFile);
                        continue;
                    }
                    //AppLog.i (TAG, "No."+i+": "+" 注册alignBitmapForNv21");
                    bitmap = ImageUtil.alignBitmapForNv21(bitmap);
                    if (bitmap == null) {
                        File failedFile = new File(REGISTER_FAILED_DIR + File.separator + jpgFile.getName());
                        if (!failedFile.getParentFile().exists()) {
                            failedFile.getParentFile().mkdirs();
                        }
                        jpgFile.renameTo(failedFile);
                        continue;
                    }
                    //AppLog.i (TAG, "No."+i+": "+" 注册 bitmap width: "+ bitmap.getWidth()+", Height: "+bitmap.getHeight());
                    byte[] nv21 = ImageUtil.bitmapToNv21(bitmap, bitmap.getWidth(), bitmap.getHeight());

                    //AppLog.i (TAG, "bitmapToNv21 ok");
                    boolean success;
                    try {
                        success = FaceServer.getInstance().register(FaceRecogniseActivity.this, nv21, bitmap.getWidth(), bitmap.getHeight(),
                                jpgFile.getName().substring(0, jpgFile.getName().lastIndexOf(".")));
                    }catch (Exception e) {
                        AppLog.e (TAG, "注册人脸失败, "+e.toString());
                        return;
                    }

                    AppLog.i (TAG, "注册 "+jpgFile.getName().substring(0,jpgFile.getName().lastIndexOf("."))+(success?"成功":"失败"));
                    if (!success) {
                        File failedFile = new File(REGISTER_FAILED_DIR + File.separator + jpgFile.getName());
                        if (!failedFile.getParentFile().exists()) {
                            failedFile.getParentFile().mkdirs();
                        }
                        jpgFile.renameTo(failedFile);
                    } else {
                        FaceItem regFace = FaceServer.getInstance().getLastRegister();
                        mFaceLab.addFaceItem(regFace);
                        successCount++;
                    }

                    //final int finalSuccessCount = successCount;
                }
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        progressDialog.dismiss();
                    }
                });
                //AppLog.d (TAG, "register ok, "+successCount+" ok");
            }
        });

    }*/

//    public void onItemLeft() {
//        if (compareResultList.size() >= FaceRegAndSearch.MAX_DETECT_NUM) {
//            compareResultList.remove(0);
//            adapter.notifyItemRemoved(0);
//        }
//    }
//
//    public void onItemInsert(CompareResult compareResult) {
//        compareResultList.add(compareResult);
//        adapter.notifyItemInserted(compareResultList.size() - 1);
//    }

    public void sendInfoReady(final String name) {

        cameraHelper.takePicture(new CameraHelper.PhotoCallback() {
            @Override
            public void onDataReady(byte[] data, Camera camera) {

                if (camera == null) {
                    AppLog.i(TAG, "can not get camera, may be exit");
                    return;
                }

                String fileName = FileUtil.getSdPath() + File.separator + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"+name+".jpg";
                try {
                    AppLog.i (TAG, "save "+ fileName + " to sdcard");
                    PhotoManager.saveToSDCard(fileName,data);
                } catch (IOException e) {
                    Toast.makeText(FaceRecogniseActivity.this,R.string.picture_save_error,Toast.LENGTH_SHORT).show();
                    AppLog.e (TAG, "save data to sdcard error: "+ e.getMessage());
                    return;
                }


                DecimalFormat df = new DecimalFormat("#.00000");
                //String location = df.format(VideoSet.longitude) + ":" + df.format(VideoSet.latitude);
                String timeStamp = TimeUtils.getSystemDate()+" "+TimeUtils.getSystemTime();
                Intent intent = new Intent("gbpe.face.detect.result");
                intent.putExtra("time",timeStamp);
                //intent.putExtra("location", location);
                intent.putExtra("latitude",VideoSet.latitude);
                intent.putExtra("longtitude",VideoSet.longitude);
                intent.putExtra("picture",fileName);
                sendBroadcast(intent);
                AppLog.i(TAG, "上传广播发送完成");

                FaceRcdLab.getInstance(FaceRecogniseActivity.this).addRecordItem(new FaceRecord(
                        name,fileName,VideoSet.latitude,VideoSet.longitude,timeStamp));
                AppLog.i(TAG, "添加记录信息："+timeStamp+": "+name+", "+fileName+", "+
                        VideoSet.latitude+" "+VideoSet.longitude);

                soundHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        mBeatBox.play(mBeatBox.getSound("Ding"));
                    }
                });

                //initCamera();
                //cameraHelper.start();
                cameraHelper.startPreview();

            }
        });
    }

/*
    public boolean checkIsAdd(int reqId) {
        boolean isAdded = false;
        for (CompareResult compareResult1 : compareResultList) {
            if (compareResult1.getTrackId() == reqId) {
                isAdded = true;
                break;
            }
        }
        return isAdded;
    }

    public boolean isCompareListNull() {
        return compareResultList==null;
    }
*/

    @Override
    public void onCompareResult(CompareResult compareResult,Integer requestId) {

        try {

            String name = null;
            //String faceHelperRecorder = null;
            if (compareResult.getFace() != null) {
                name = compareResult.getFace().getName();
            }
            //if (compareResult == null || compareResult.getFace().getName() == null) {
            if (compareResult == null || name == null) {
                requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
                //fh.addName(requestId, "VISITOR " + requestId);
                //faceRegAndSearch.faceHelperAddName(requestId,"VISITOR " + requestId);
                //return faceHelperRecorder;
                return;
            }

//                        Log.i(TAG, "onNext: fr search get result  = " + System.currentTimeMillis() + " trackId = " + requestId + "  similar = " + compareResult.getSimilar());
            /**  得分超过0.8 才认为是可信的 **/
            if (compareResult.getSimilar() > SIMILAR_THRESHOLD) {

                boolean isAdded = false;
                if (compareResultList == null) {
                    requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
                    //faceRegAndSearch.faceHelperAddName(requestId,"VISITOR " + requestId);
                    //faceHelperRecorder = "VISITOR " + requestId;
                    //return faceHelperRecorder;
                    return;
                }

                /**查找比较列表，看是否已经记录**/
                for (CompareResult compareResult1 : compareResultList) {
                    if (compareResult1.getTrackId() == requestId) {
                        isAdded = true;
                        break;
                    }
                }

                if (!isAdded) {

                    //对于多人脸搜索，假如最大显示数量为 MAX_DETECT_NUM 且有新的人脸进入，则以队列的形式移除
                    if (compareResultList.size() >= FaceRegAndSearch.MAX_DETECT_NUM) {
                        compareResultList.remove(0);
                        // 此处移除识别出的人脸列表
                        adapter.notifyItemRemoved(0);
                    }
                    //添加显示人员时，保存其trackId
                    compareResult.setTrackId(requestId);
                    compareResultList.add(compareResult);
                    adapter.notifyItemInserted(compareResultList.size() - 1);
                    //listener.onInfoReady(name);
                    synchronized (this) {
                        sendInfoReady(name);
                    }

                    /*设置为识别成功*/
                    requestFeatureStatusMap.put(requestId, RequestFeatureStatus.SUCCEED);
                    faceRegAndSearch.faceHelperAddName(requestId, compareResult.getFace().getName());
                    //faceHelperRecorder = compareResult.getFace().getName();

                    AppLog.i(TAG, "recognise face: " + compareResult.getFace().getName());

                } else {

                    requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
                    //fh.addName(requestId, "VISITOR " + requestId);
                    // faceHelperRecorder = "VISITOR " + requestId;
                    //faceRegAndSearch.faceHelperAddName(requestId,"VISITOR " + requestId);
                    AppLog.i(TAG, "fail to recognise face: ");

                }
            }
        } catch (Exception e){
            AppLog.e (TAG, "onCompareResult error: "+e.getMessage());
        } finally {
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    recogniseStatus = RECOGNISE_STATUS_DONE;
                    //mFaceMode = FACE_MODE_IDEL;
                }
            },500);

        }
        //faceRegAndSearch.faceHelperAddName(requestId,faceHelperRecorder);
        //faceHelper.addName(requestId,faceHelperRecorder);
        //return faceHelperRecorder;
    }

    @Override
    public void onCompareError(Integer requestId) {
        requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
        recogniseStatus = RECOGNISE_STATUS_DONE;
    }

    @Override
    public void onRegisterFinished(int suc, int failed) {

    }

    @Override
    public void onRegisterError(Exception e) {

    }

    @Override
    public void onSingleOk(FaceItem reg, int cnt) {

    }

    @Override
    public void onShowMsg(String msg) {

    }
}