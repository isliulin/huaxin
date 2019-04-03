package com.arcsoft.arcfacedemo.faceserver;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.util.Log;

import com.arcsoft.arcfacedemo.model.FaceRegisterInfo;
import com.arcsoft.arcfacedemo.util.ImageUtil;
import com.arcsoft.face.ErrorInfo;
import com.arcsoft.face.FaceEngine;
import com.arcsoft.face.FaceFeature;
import com.arcsoft.face.FaceInfo;
import com.arcsoft.face.FaceSimilar;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import gbpe.policeass.db.FaceItem;
import gbpe.policeass.db.FaceLab;
import gbpe.policeass.utils.AppLog;

/**
 * 人脸库操作类，包含注册和搜索
 */
public class FaceServer {
    private static final String TAG = "FaceServer";
    public static final String IMG_SUFFIX = ".jpg";
    private static FaceEngine faceEngine = null;
    private static FaceServer faceServer = null;
    private static List<FaceRegisterInfo> faceRegisterInfoList;
    public static String ROOT_PATH;
    public static final String SAVE_IMG_DIR = "register" + File.separator + "imgs";
    private static final String SAVE_FEATURE_DIR = "register" + File.separator + "features";
	private List<FaceItem> regFaces;
	private Context context;
	private static boolean isInitOk;

    /**
     * 是否正在搜索人脸，保证搜索操作单线程进行
     */
    private boolean isProcessing = false;

    public static FaceServer getInstance() {
        if (faceServer == null) {
            synchronized (FaceServer.class) {
                if (faceServer == null) {
                    faceServer = new FaceServer();
                }
            }
        }
        return faceServer;
    }

    public boolean isInit() {
        return isInitOk;
    }

    /**
     * 初始化
     *
     * @param context 上下文对象
     * @return 是否初始化成功
     */
    public boolean init(Context context, List<FaceItem> faces) {
        synchronized (this) {
            if (faceEngine == null && context != null) {
                regFaces = faces;
                this.context = context;
                faceEngine = new FaceEngine();
                int engineCode = faceEngine.init(context, FaceEngine.ASF_DETECT_MODE_IMAGE, FaceEngine.ASF_OP_0_HIGHER_EXT, 16, 1, FaceEngine.ASF_FACE_RECOGNITION | FaceEngine.ASF_FACE_DETECT);
                if (engineCode == ErrorInfo.MOK) {
                    initFaceList(context);
                    isInitOk = true;
                    return true;
                } else {
                    faceEngine = null;
                    isInitOk = false;
                    Log.e(TAG, "init: failed! code = " + engineCode);
                    return false;
                }
            }
            return false;
        }
    }

    /**
     * 销毁
     */
    public void unInit() {
        synchronized (this) {
            if (faceRegisterInfoList != null) {
                faceRegisterInfoList.clear();
                faceRegisterInfoList = null;
            }
            if (faceEngine != null) {
                faceEngine.unInit();
                faceEngine = null;
            }
            isInitOk = false;
        }
    }

    /**
     * 初始化人脸特征数据以及人脸特征数据对应的注册图
     *
     * @param context 上下文对象
     */
    private void initFaceList(Context context) {
        synchronized (this) {
            if (ROOT_PATH == null) {
                ROOT_PATH = context.getFilesDir().getAbsolutePath();
            }
            File featureDir = new File(ROOT_PATH + File.separator + SAVE_FEATURE_DIR);
            if (!featureDir.exists() || !featureDir.isDirectory()) {
                return;
            }
            /*
            File[] featureFiles = featureDir.listFiles();
            if (featureFiles == null || featureFiles.length == 0) {
                return;
            }
            faceRegisterInfoList = new ArrayList<>();
            for (File featureFile : featureFiles) {
                try {
                    FileInputStream fis = new FileInputStream(featureFile);
                    byte[] feature = new byte[FaceFeature.FEATURE_SIZE];
                    fis.read(feature);
                    fis.close();
                    faceRegisterInfoList.add(new FaceRegisterInfo(feature, featureFile.getName()));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }*/
            faceRegisterInfoList = new ArrayList<>();
            for (FaceItem face : regFaces) {
                 try {
                     File featureFile = new File(face.getFeaturePath());
                     FileInputStream fis = new FileInputStream(featureFile);
                     byte[] feature = new byte[FaceFeature.FEATURE_SIZE];
                     fis.read(feature);
                     fis.close();
                     faceRegisterInfoList.add(new FaceRegisterInfo(feature,face.getName(),face.getUUID()));
                 } catch (IOException e) {
                     AppLog.e (TAG, "get reg faces err "+e.toString());
                 }
            }
        }
    }

    public int getFaceNumber(Context context) {
        synchronized (this) {
            if (context == null) {
                return 0;
            }
            if (ROOT_PATH == null) {
                ROOT_PATH = context.getFilesDir().getAbsolutePath();
            }

            File featureFileDir = new File(ROOT_PATH + File.separator + SAVE_FEATURE_DIR);
            int featureCount = 0;
            if (featureFileDir.exists() && featureFileDir.isDirectory()) {
                String[] featureFiles = featureFileDir.list();
                featureCount = featureFiles == null ? 0 : featureFiles.length;
            }
            int imageCount = 0;
            File imgFileDir = new File(ROOT_PATH + File.separator + SAVE_IMG_DIR);
            if (imgFileDir.exists() && imgFileDir.isDirectory()) {
                String[] imageFiles = imgFileDir.list();
                imageCount = imageFiles == null ? 0 : imageFiles.length;
            }
            return featureCount > imageCount ? imageCount : featureCount;
        }
    }

    public int clearAllFaces(Context context) {
        synchronized (this) {
            if (context == null) {
                return 0;
            }
            if (ROOT_PATH == null) {
                ROOT_PATH = context.getFilesDir().getAbsolutePath();
            }
            if (faceRegisterInfoList != null) {
                faceRegisterInfoList.clear();
            }
            File featureFileDir = new File(ROOT_PATH + File.separator + SAVE_FEATURE_DIR);
            int deletedFeatureCount = 0;
            if (featureFileDir.exists() && featureFileDir.isDirectory()) {
                File[] featureFiles = featureFileDir.listFiles();
                if (featureFiles != null && featureFiles.length > 0) {
                    for (File featureFile : featureFiles) {
                        if (featureFile.delete()) {
                            deletedFeatureCount++;
                        }
                    }
                }
            }
            int deletedImageCount = 0;
            File imgFileDir = new File(ROOT_PATH + File.separator + SAVE_IMG_DIR);
            if (imgFileDir.exists() && imgFileDir.isDirectory()) {
                File[] imgFiles = imgFileDir.listFiles();
                if (imgFiles != null && imgFiles.length > 0) {
                    for (File imgFile : imgFiles) {
                        if (imgFile.delete()) {
                            deletedImageCount++;
                        }
                    }
                }
            }
            return deletedFeatureCount > deletedImageCount ? deletedImageCount : deletedFeatureCount;
        }
    }


    private FaceItem mLastRegister = null;

    public FaceItem getLastRegister(){
        return mLastRegister;
    }

    /**
     * 注册人脸
     *
     * @param context 上下文对象
     * @param nv21    NV21数据
     * @param width   NV21宽度
     * @param height  NV21高度
     * @param name    保存的名字，可为空
     * @return 是否注册成功
     */
    public boolean register(Context context, byte[] nv21, int width, int height, String name) {
        synchronized (this) {
            if (faceEngine == null || context == null || nv21 == null || width % 4 != 0 || nv21.length != width * height * 3 / 2) {
                AppLog.i (TAG, "参数错误");
                return false;
            }

            if (ROOT_PATH == null) {
                ROOT_PATH = context.getFilesDir().getAbsolutePath();
            }
            boolean dirExists = true;
            //特征存储的文件夹
            File featureDir = new File(ROOT_PATH + File.separator + SAVE_FEATURE_DIR);
            if (!featureDir.exists()) {
                dirExists = featureDir.mkdirs();
            }
            if (!dirExists) {
                AppLog.e (TAG, "特征文件夹创建失败");
                return false;
            }
            //图片存储的文件夹
            File imgDir = new File(ROOT_PATH + File.separator + SAVE_IMG_DIR);
            if (!imgDir.exists()) {
                dirExists = imgDir.mkdirs();
            }
            if (!dirExists) {
                AppLog.e (TAG, "图像存储文件夹创建失败");
                return false;
            }
            //1.人脸检测
            List<FaceInfo> faceInfoList = new ArrayList<>();
            int code = faceEngine.detectFaces(nv21, width, height, FaceEngine.CP_PAF_NV21, faceInfoList);
            if (code == ErrorInfo.MOK && faceInfoList.size() > 0) {

                AppLog.i (TAG, "检测到人脸，开始注册");

                FaceFeature faceFeature = new FaceFeature();

                //2.特征提取
                code = faceEngine.extractFaceFeature(nv21, width, height, FaceEngine.CP_PAF_NV21, faceInfoList.get(0), faceFeature);
                // 此处采用uuid做为注册的唯一序列号
                String userName = UUID.randomUUID().toString();
                //String userName = (name == null? UUID.randomUUID().toString() : name);
                //String userName = name == null ? String.valueOf(System.currentTimeMillis()) : name;
                try {
                    //3.保存注册结果（注册图、特征数据）
                    if (code == ErrorInfo.MOK) {
                        YuvImage yuvImage = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
                        //为了美观，扩大rect截取注册图
                        Rect cropRect = getBestRect(width, height, faceInfoList.get(0).getRect());
                        if (cropRect == null) {
                            return false;
                        }
                        File file = new File(imgDir + File.separator + userName + IMG_SUFFIX);
                        FileOutputStream fosImage = new FileOutputStream(file);
                        yuvImage.compressToJpeg(cropRect, 100, fosImage);
                        fosImage.close();
                        Bitmap bitmap = BitmapFactory.decodeFile(file.getAbsolutePath());
                        //mLastRegister = file.getAbsolutePath();

                        //判断人脸旋转角度，若不为0度则旋转注册图
                        boolean needAdjust = false;
                        if (bitmap != null) {
                            switch (faceInfoList.get(0).getOrient()) {
                                case FaceEngine.ASF_OC_0:
                                    break;
                                case FaceEngine.ASF_OC_90:
                                    bitmap = ImageUtil.getRotateBitmap(bitmap, 90);
                                    needAdjust = true;
                                    break;
                                case FaceEngine.ASF_OC_180:
                                    bitmap = ImageUtil.getRotateBitmap(bitmap, 180);
                                    needAdjust = true;
                                    break;
                                case FaceEngine.ASF_OC_270:
                                    bitmap = ImageUtil.getRotateBitmap(bitmap, 270);
                                    needAdjust = true;
                                    break;
                                default:
                                    break;
                            }
                        }
                        if (needAdjust) {
                            fosImage = new FileOutputStream(file);
                            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, fosImage);
                            fosImage.close();
                        }
						

						/*此处存储特征文件*/
                        FileOutputStream fosFeature = new FileOutputStream(featureDir + File.separator + userName);
                        fosFeature.write(faceFeature.getFeatureData());
                        fosFeature.close();

                        //内存中的数据同步
                        if (faceRegisterInfoList == null) {
                            faceRegisterInfoList = new ArrayList<>();
                        }
                        // 增加faceRegisterInfo共两处
                        faceRegisterInfoList.add(new FaceRegisterInfo(faceFeature.getFeatureData(), userName, userName));
                        //mLastRegister = new FaceItem(userName,"", "",file.getAbsolutePath(),featureDir + File.separator + userName);
                        mLastRegister = new FaceItem(UUID.fromString(userName));
                        mLastRegister.setImgPath(file.getAbsolutePath());
                        if (name!=null) {
                            mLastRegister.setName(name);
                        }
                        mLastRegister.setFeaturePath(featureDir + File.separator + userName);
                        return true;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {

                if (code != ErrorInfo.MOK) {
                    AppLog.e (TAG, "检测失败: "+code);
                } else if (faceInfoList.size() <= 0) {
                    AppLog.e (TAG, "没有检测到人脸");
                }

            }
            return false;
        }

    }

    /**
     * 在特征库中搜索
     *
     * @param faceFeature 传入特征数据
     * @return 比对结果
     */
    public CompareResult getTopOfFaceLib(FaceFeature faceFeature) {
        if (faceEngine == null || isProcessing || faceFeature == null || faceRegisterInfoList == null || faceRegisterInfoList.size() == 0) {
            return null;
        }
        FaceFeature tempFaceFeature = new FaceFeature();
        FaceSimilar faceSimilar = new FaceSimilar();
        float maxSimilar = 0;
        int maxSimilarIndex = -1;
        isProcessing = true;
        /** faceRegisterInfoList 为存储注册face的列表 **/
        /** for 循环为逐一比对给定人脸特征和注册特征库的样本，获取特征值最匹配的人脸 **/
        for (int i = 0; i < faceRegisterInfoList.size(); i++) {
            tempFaceFeature.setFeatureData(faceRegisterInfoList.get(i).getFeatureData());

            /** FaceEngine.compareFaceFeature 为比较两个人脸特征的方法 **/
            faceEngine.compareFaceFeature(faceFeature, tempFaceFeature, faceSimilar);
            if (faceSimilar.getScore() > maxSimilar) {
                maxSimilar = faceSimilar.getScore();
                maxSimilarIndex = i;
            }
        }
        isProcessing = false;
        if (maxSimilarIndex != -1) {
            /** 所有比对得分大于零，则返回一个得分最高的result **/
            //return new CompareResult(faceRegisterInfoList.get(maxSimilarIndex).getName(), maxSimilar);
            //return new CompareResult(FaceLab.get(context).getFace(faceRegisterInfoList.get(maxSimilarIndex).getUuid()),maxSimilar);
            return new CompareResult(FaceLab.get(context).getFaceItem(UUID.fromString(faceRegisterInfoList.get(maxSimilarIndex).getUuid())),
                    maxSimilar);
        }
        return null;
    }

    /**
     * 将图像中需要截取的Rect向外扩张一倍，若扩张一倍会溢出，则扩张到边界，若Rect已溢出，则收缩到边界
     *
     * @param width   图像宽度
     * @param height  图像高度
     * @param srcRect 原Rect
     * @return 调整后的Rect
     */
    private static Rect getBestRect(int width, int height, Rect srcRect) {

        if (srcRect == null) {
            return null;
        }
        Rect rect = new Rect(srcRect);
        //1.原rect边界已溢出宽高的情况
        int maxOverFlow = 0;
        int tempOverFlow = 0;
        if (rect.left < 0) {
            maxOverFlow = -rect.left;
        }
        if (rect.top < 0) {
            tempOverFlow = -rect.top;
            if (tempOverFlow > maxOverFlow) {
                maxOverFlow = tempOverFlow;
            }
        }
        if (rect.right > width) {
            tempOverFlow = rect.right - width;
            if (tempOverFlow > maxOverFlow) {
                maxOverFlow = tempOverFlow;
            }
        }
        if (rect.bottom > height) {
            tempOverFlow = rect.bottom - height;
            if (tempOverFlow > maxOverFlow) {
                maxOverFlow = tempOverFlow;
            }
        }
        if (maxOverFlow != 0) {
            rect.left += maxOverFlow;
            rect.top += maxOverFlow;
            rect.right -= maxOverFlow;
            rect.bottom -= maxOverFlow;
            return rect;
        }
        //1.原rect边界未溢出宽高的情况
        int padding = rect.height() / 2;
        //若以此padding扩张rect会溢出，取最大padding为四个边距的最小值
        if (!(rect.left - padding > 0 && rect.right + padding < width && rect.top - padding > 0 && rect.bottom + padding < height)) {
            padding = Math.min(Math.min(Math.min(rect.left, width - rect.right), height - rect.bottom), rect.top);
        }

        rect.left -= padding;
        rect.top -= padding;
        rect.right += padding;
        rect.bottom += padding;
        return rect;
    }
}
