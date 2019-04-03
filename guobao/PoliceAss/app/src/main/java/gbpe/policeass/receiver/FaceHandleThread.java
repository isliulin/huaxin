package gbpe.policeass.receiver;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.arcsoft.arcfacedemo.faceserver.CompareResult;
import com.arcsoft.arcfacedemo.faceserver.FaceServer;
import com.arcsoft.arcfacedemo.util.ImageUtil;
import com.arcsoft.arcfacedemo.util.face.FaceHelper;
import com.arcsoft.arcfacedemo.util.face.RequestFeatureStatus;
import com.arcsoft.face.FaceFeature;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import gbpe.policeass.Activity.FaceRecogniseActivity;
import gbpe.policeass.R;
import gbpe.policeass.db.FaceItem;
import gbpe.policeass.db.FaceLab;
import gbpe.policeass.utils.AppLog;
import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

import static gbpe.policeass.receiver.FaceRegAndSearch.*;

public class FaceHandleThread extends Thread {

    private static final String TAG = "FaceHandleThread";

    private static FaceHandleThread mFaceHandleThread;
    private static boolean isThreadRun = false;
    private int mWorkingState;

    public static final int ERROR_SECCESS = 0;
    public static final int ERROR_INIT = -1;
    public static final int ERROR_PATH = -2;
    public static final int ERROR_FORMAT = -3;
    public static final int ERROR_REGISTER = -4;

    public static final int STATE_REGISTER = 1;
    public static final int STATE_SEARCH = 2;
    public static final int STATE_IDEL = 0;
    public static final int STATE_BUSY = -1;
    private FaceFeature mfrFace;
    private static Integer mRequestId;
    private static FaceSearchListener mFaceSearchListener;

    public static int errCode;
    private static FaceServerLisener mFaceServerLisener;
    //private FaceLab mfaceLab;
    //private FaceHelper faceHelper;
    private static List<RegInfo> infoList = null;
    private static final Object lock = new Object();
    //private ConcurrentHashMap<Integer, Integer> requestFeatureStatusMap = new ConcurrentHashMap<>();
    private static final float SIMILAR_THRESHOLD = 0.8F;
    //private static List<CompareResult> compareResultList;
    //public static final int MAX_DETECT_NUM = 10;
    private FaceServer mFaceServer;
    private static FaceItem mLastRegFace;

    public void setWorkingState(int state) {
        mWorkingState = state;
    }

//    public boolean getThreadRun() {
//        return isThreadRun;
//    }

    public void setThreadRun(boolean val) {
        isThreadRun = val;
    }

    public boolean getThreadIdel() {
        return mWorkingState == STATE_IDEL;
    }

    public void setFaceServerLisener(FaceServerLisener faceServerLisener){
        mFaceServerLisener = faceServerLisener;
    }

    private Bitmap bitmapFromFiles(File src) {
        Bitmap bitmap = BitmapFactory.decodeFile(src.getAbsolutePath());
        if (bitmap == null) {
            File failedFile = new File(FaceRecogniseActivity.REGISTER_FAILED_DIR + File.separator + src.getName());
            if (!failedFile.getParentFile().exists()) {
                failedFile.getParentFile().mkdirs();
            }
            src.renameTo(failedFile);
            return null;
        }
        bitmap = ImageUtil.alignBitmapForNv21(bitmap);

        if (bitmap == null) {
            File failedFile = new File(FaceRecogniseActivity.REGISTER_FAILED_DIR + File.separator + src.getName());
            if (!failedFile.getParentFile().exists()) {
                failedFile.getParentFile().mkdirs();
            }
            src.renameTo(failedFile);
            return null;
        }

        return bitmap;
    }



    public static FaceHandleThread getInstance(FaceServer faceServer){

        if (mFaceHandleThread == null){
            mFaceHandleThread = new FaceHandleThread(faceServer);
            //return mFaceHandleThread;
            mFaceHandleThread.start();
        }
        return mFaceHandleThread;

    }
    private FaceHandleThread(FaceServer faceServer){
        //mfaceLab = faceLab;
        //this.faceHelper = faceHelper;
        mFaceServer = faceServer;
    }

    private boolean batchRegister(RegInfo regInfo, FaceServer faceServer) {

        AppLog.i (TAG, "batchRegister");
        byte[] nv21;
        int totalCount = 0;
        int successCount;
        int failedCount;

        File[] jpgFiles = null;

        if (regInfo.isNv21() == false) {
            String path = regInfo.getUrl();
            if (path == null) {
                AppLog.i(TAG, "no sample file(s) avialable");
                errCode = ERROR_PATH;
                return false;
            }
            File dir = new File(path);

            if (!dir.exists()) {
                AppLog.e(TAG, "file(s) not exist: " + dir.getAbsolutePath());
                errCode = ERROR_PATH;
                return false;     // 20181217
            }

            if (dir.isDirectory()) {

                jpgFiles = dir.listFiles(new FilenameFilter() {
                    @Override
                    public boolean accept(File dir, String name) {
                        return (name.endsWith(FaceRegAndSearch.IMG_SUFFIX));
                    }
                });
            } else {

                AppLog.i(TAG, "get file " + dir.getName());
                if (dir.getName().endsWith(FaceRegAndSearch.IMG_SUFFIX)) {
                    jpgFiles = new File[]{new File(path)};
                } else {
                    AppLog.i(TAG, "invalid name " + dir.getName());
                    errCode = ERROR_FORMAT;
                    if (mFaceServerLisener != null) {
                        mFaceServerLisener.onRegisterError(new FileNotFoundException(dir.getName() + " NotFound"));
                    }
                    return false;
                }

            }

            totalCount = jpgFiles.length;
            successCount = 0;
            failedCount = 0;
        }else {

            totalCount = 1;
            successCount = 0;
            failedCount = 0;
            //nv21 = regInfo.getNv21();
        }

        int width,height;
        String regName, regFileName;

        for (int i = 0; i < totalCount; i++) {

            if (regInfo.isNv21()) {
                regName = regInfo.getName();
                //regFileName = regName + ".jpg";
                nv21 = regInfo.getNv21();
                width = regInfo.getWidth();
                height = regInfo.getHeight();
            } else {
                regFileName = jpgFiles[i].getName();
                regName = regFileName.substring(0,regFileName.lastIndexOf("."));
                Bitmap bitmap;
                bitmap = bitmapFromFiles(jpgFiles[i]);
                if (bitmap == null) {
                    continue;
                }
                width = bitmap.getWidth();
                height = bitmap.getHeight();
                nv21 = ImageUtil.bitmapToNv21(bitmap, width, height);
            }
            //final int finalI = i;
            AppLog.i(TAG, "注册第" + i + "项");

            //AppLog.i (TAG, "bitmapToNv21 ok");
            boolean success;
            try {
                //success = FaceServer.getInstance().register(context, nv21, bitmap.getWidth(), bitmap.getHeight(),
                //success = faceServer.register(context, nv21, width, height,
                //jpgFile.getName().substring(0, jpgFile.getName().lastIndexOf(".")));
                success = faceServer.register(regInfo.getContext(),nv21,width,height,
                        regName);
            } catch (Exception e) {
                AppLog.e(TAG, "注册人脸失败, " + e.toString());
                //msg.obj = R.string.register_failed;
                //msgHandler.sendMessage(msg);
                //faceServer.unInit();
                errCode = ERROR_REGISTER;
                if (mFaceServerLisener != null) {
                    mFaceServerLisener.onRegisterError(e);
                }
                return false;
            }

            AppLog.i(TAG, "注册 " + regName + (success ? "成功" : "失败"));
            if (!success) {
                //File failedFile = new File(FaceRecogniseActivity.REGISTER_FAILED_DIR + File.separator + regFileName);
                //if (!failedFile.getParentFile().exists()) {
                //    failedFile.getParentFile().mkdirs();
                // }
                //jpgFile.renameTo(failedFile);
                //msg.obj = R.string.register_failed;
                //msgHandler.sendMessage(msg);
                failedCount++;
                errCode = ERROR_REGISTER;
                mFaceServerLisener.onRegisterError(new Exception(regName));
                //return false;

            } else {
                //FaceItem regFace = FaceServer.getInstance().getLastRegister();
                //FaceItem regFace = faceServer.getLastRegister();
                mLastRegFace = faceServer.getLastRegister();
                //mfaceLab.addFaceItem(mLastRegFace);
                successCount++;
                errCode = ERROR_SECCESS;
                //msg.obj = R.string.register_success;
                //msgHandler.sendMessage(msg);
                if (mFaceServerLisener != null) {
                    // 当批量注册时，此时注册单张照片成功，会添加进数据库
                    // 点击预览注册时，在输入名称等信息后添加进入数据库
                    mFaceServerLisener.onSingleOk(mLastRegFace,successCount);
                }
                AppLog.i(TAG, "注册完毕");
                //return true;
            }
            //final int finalSuccessCount = successCount;
        }

        if (mFaceServerLisener!=null) {
            mFaceServerLisener.onRegisterFinished(successCount, failedCount);
        }
        return true;
        //faceServer.unInit();
        //AppLog.i (TAG, "注册完毕");

        //Toast.makeText(context,R.string.face_bat_register_finished,Toast.LENGTH_SHORT).show();
        //AppLog.d (TAG, "register ok, "+successCount+" ok");
    }

    private void regDownPhotoes(FaceServer server, FaceServerLisener lisener) {

        boolean success;
        AppLog.i (TAG, "start register");
        while (infoList.size() > 0) {

            success = batchRegister(infoList.get(0),server);

            if (success) {
                //Message msg = new Message();
                //msg.obj = mContext.getString(R.string.register)+ " ";
                //String info = new File(infoList.get(0).getUrl()).getName() + " ";
                //info += mContext.getString(R.string.register_success);
                //msg.obj = info;
                //msgHandler.sendMessage(msg);
            } else {
                //Message msg = new Message();
                //String info = new File(infoList.get(0).getUrl()).getName()+" ";
                //info += mContext.getString(R.string.register_failed);
                //msg.obj = info;
                //msgHandler.sendMessage(msg);
            }

            synchronized (lock) {
                infoList.remove(0);
            }
        }

    }

    public void searchFace(final FaceFeature frFace, final Integer requestId, final FaceSearchListener listener) {

        AppLog.i (TAG, "search face");

        Observable
                .create(new ObservableOnSubscribe<CompareResult>() {
                    @Override
                    public void subscribe(ObservableEmitter<CompareResult> emitter) {
//                        Log.i(TAG, "subscribe: fr search start = " + System.currentTimeMillis() + " trackId = " + requestId);
                        /** 此函数和注册的人脸特征库逐一比对，有相似度大于0的，则返回相似的最大的结果，否则返回null**/
                        CompareResult compareResult = FaceServer.getInstance().getTopOfFaceLib(frFace);
//                        Log.i(TAG, "subscribe: fr search end = " + System.currentTimeMillis() + " trackId = " + requestId);
                        if (compareResult == null) {
                            emitter.onError(null);
                        } else {
                            emitter.onNext(compareResult);
                        }
                    }
                })
                .subscribeOn(Schedulers.computation())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Observer<CompareResult>() {
                    @Override
                    public void onSubscribe(Disposable d) {

                    }

                    @Override
                    public void onNext(CompareResult compareResult) {
                        //faceHelper.addName(requestId,name);
                        listener.onCompareResult( compareResult,requestId);
//                        if (faceHelper != null) {
//                              faceHelper.addName(requestId, tmpName);
//                        }
//                        /*String name=null;
//                        if (compareResult.getFace()!=null){
//                            name = compareResult.getFace().getName();
//                        }
//                        //if (compareResult == null || compareResult.getFace().getName() == null) {
//                        if (compareResult == null || name == null) {
//                            requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
//                            faceHelper.addName(requestId, "VISITOR " + requestId);
//                            return;
//                        }*/
//
////                        Log.i(TAG, "onNext: fr search get result  = " + System.currentTimeMillis() + " trackId = " + requestId + "  similar = " + compareResult.getSimilar());
//                        /**  得分超过0.8 才认为是可信的 **/
//                        //if (compareResult.getSimilar() > SIMILAR_THRESHOLD) {
//                        /*    boolean isAdded;
//                            if (listener.isCompareListNull()) {
//                                requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
//                                faceHelper.addName(requestId, "VISITOR " + requestId);
//                                return;
//                            }*/
//
//                            /**查找比较列表，看是否已经记录**/
//                            /*
//                            for (CompareResult compareResult1 : compareResultList) {
//                                if (compareResult1.getTrackId() == requestId) {
//                                    isAdded = true;
//                                    break;
//                                }
//                            }*/
//                            isAdded = listener.checkIsAdd(requestId);
//
//                            if (!isAdded) {
//                                //listener.itemAdded();
//
//                                //对于多人脸搜索，假如最大显示数量为 MAX_DETECT_NUM 且有新的人脸进入，则以队列的形式移除
//                                //if (compareResultList.size() >= FaceRegAndSearch.MAX_DETECT_NUM) {
//                                    //compareResultList.remove(0);
//                                    // 此处移除识别出的人脸列表
//                                    //adapter.notifyItemRemoved(0);
//                                    //listener.onItemLeft();
//                                //}
//                                listener.onItemLeft();
//                                //添加显示人员时，保存其trackId
//                                compareResult.setTrackId(requestId);
//
//                                //compareResultList.add(compareResult);
//                                //adapter.notifyItemInserted(compareResultList.size() - 1);
//                                listener.onItemInsert(compareResult);
//                                listener.onInfoReady(name);
//
//                                // 拍摄图片，收集信息，上传到服务器，20190219
//                                /*
//                                final String finalName = name;
//                                cameraHelper.takePicture(new CameraHelper.PhotoCallback() {
//                                    @Override
//                                    public void onDataReady(byte[] data, Camera camera) {
//                                        String fileName = FileUtil.getSdPath() + File.separator + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"+finalName+".jpg";
//                                        try {
//                                            AppLog.i (TAG, "save "+ fileName + " to sdcard");
//                                            PhotoManager.saveToSDCard(fileName,data);
//                                        } catch (IOException e) {
//                                            Toast.makeText(FaceRecogniseActivity.this,R.string.picture_save_error,Toast.LENGTH_SHORT).show();
//                                            AppLog.e (TAG, "save data to sdcard error: "+ e.getMessage());
//                                            return;
//                                        }
//
//
//                                        DecimalFormat df = new DecimalFormat("#.00000");
//                                        //String location = df.format(VideoSet.longitude) + ":" + df.format(VideoSet.latitude);
//                                        String timeStamp = TimeUtils.getSystemDate()+" "+TimeUtils.getSystemTime();
//                                        Intent intent = new Intent("gbpe.face.detect.result");
//                                        intent.putExtra("time",timeStamp);
//                                        //intent.putExtra("location", location);
//                                        intent.putExtra("latitude",VideoSet.latitude);
//                                        intent.putExtra("longtitude",VideoSet.longitude);
//                                        intent.putExtra("picture",fileName);
//                                        sendBroadcast(intent);
//                                        AppLog.i(TAG, "上传广播发送完成");
//
//                                        FaceRcdLab.getInstance(FaceRecogniseActivity.this).addRecordItem(new FaceRecord(
//                                                finalName,fileName,VideoSet.latitude,VideoSet.longitude,timeStamp));
//                                        AppLog.i(TAG, "添加记录信息："+timeStamp+": "+finalName+", "+fileName+", "+
//                                                VideoSet.latitude+" "+VideoSet.longitude);
//
//                                        soundHandler.post(new Runnable() {
//                                            @Override
//                                            public void run() {
//                                                mBeatBox.play(mBeatBox.getSound("Ding"));
//                                            }
//                                        });
//
//                                        //initCamera();
//                                        //cameraHelper.start();
//                                        cameraHelper.startPreview();
//
//                                    }
//                                });*/
//                            }
//
//                            /* 此处将识别到的人脸添加到显示列表 */
//                            //adapter.notifyItemInserted(compareResultList.size() - 1);
//                            /* 弹出识别到的人脸 */
//                            //String imgPath = FaceServer.ROOT_PATH + File.separator + FaceServer.SAVE_IMG_DIR + File.separator + compareResultList.get(compareResultList.size() - 1).getUserName() + FaceServer.IMG_SUFFIX;
//                            //String imgPath = compareResult.getFace().getImgPath();
//                            /*
//                            FaceItem face = compareResult.getFace();
//                            final FaceRegInputDlg faceRegInputDlg = new FaceRegInputDlg(context,BitmapFactory.decodeFile(face.getImgPath()));
//                            faceRegInputDlg.setFaceInfo(getString(R.string.face_recognise_tips),face.getName(),face.getId(),face.getRemark());
//                            faceRegInputDlg.setEditDisable();
//                            faceRegInputDlg.setOnPositiveListener(new View.OnClickListener() {
//                                @Override
//                                public void onClick(View v) {
//                                    faceRegInputDlg.dismiss();
//									//recogniseStatus = RECOGNISE_STATUS_DONE;
//                        			//mFaceMode = FACE_MODE_IDEL;
//                                }
//                            });
//                            //mFaceMode = FACE_MODE_IDEL;
//                            faceRegInputDlg.show();
//                            */
//
//                            /*设置为识别成功*/
//                            requestFeatureStatusMap.put(requestId, RequestFeatureStatus.SUCCEED);
//                            faceHelper.addName(requestId, compareResult.getFace().getName());
//
//                            AppLog.i(TAG, "recognise face: "+compareResult.getFace().getName());
//
//                        } else {
//
//                            requestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
//                            faceHelper.addName(requestId, "VISITOR " + requestId);
//                            AppLog.i(TAG, "fail to recognise face: ");
//                        }
                    }

                    @Override
                    public void onError(Throwable e) {
                        //equestFeatureStatusMap.put(requestId, RequestFeatureStatus.FAILED);
                        listener.onCompareError(requestId);
                    }

                    @Override
                    public void onComplete() {

                    }
                });
    }

    public void addRegResource(Context context, String url){

        synchronized (lock) {
            if (infoList == null) {
                infoList = new ArrayList<>();
            }
            infoList.add(new RegInfo(context,url));
        }

    }

    public void addRegResource(Context context, byte[] nv21, String name, int width, int height) {

        synchronized (lock) {
            if (infoList == null) {
                infoList = new ArrayList<>();
            }
            infoList.add(new RegInfo(context,nv21,name,width, height));
        }

    }

    public void addSearchInfo(FaceFeature faceFeature, Integer requestId, FaceSearchListener listener) {
        mfrFace = faceFeature;
        mRequestId = requestId;
        mFaceSearchListener = listener;
    }

    @Override
    public void run() {

        isThreadRun = true;
        while (isThreadRun) {
            if (mWorkingState == STATE_REGISTER) {
                try {
                    regDownPhotoes(mFaceServer, mFaceServerLisener);
                } catch (Exception e) {
                    AppLog.e(TAG, "REGISTER ERROR, " + e.getMessage());
                    mFaceServerLisener.onRegisterError(e);
                } finally {
                    //mInstance = null;
                    mWorkingState = STATE_IDEL;
                    //mWorkingState = STATE_BUSY;
                    //isThreadRun = false;
                }
            } else if (mWorkingState == STATE_SEARCH) {
                try {
                    searchFace(mfrFace, mRequestId, mFaceSearchListener);
                } catch (Exception e) {
                    AppLog.e(TAG, "SEARCH ERROR, " + e.getMessage());
                } finally {
                    mWorkingState = STATE_IDEL;
                }
            } else{
                try {
                    sleep(300);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                //isThreadRun = false;
            }
        }
    }

    public static FaceItem getLastRegFace(){
        return mLastRegFace;
    }

}
