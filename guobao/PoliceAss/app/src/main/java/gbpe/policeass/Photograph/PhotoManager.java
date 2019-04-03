package gbpe.policeass.Photograph;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.hardware.Camera;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.Photograph.watermark.ImageUtil;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.CameraHandlerThread;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.utils.TimeUtils;


/**
 * Created by Administrator on 2016/11/10.
 */
public class PhotoManager implements Camera.PictureCallback {


    private   static MainActivity context ;
    private   SurfaceHolder holder ;
	private static final int MSG_PIC_CONTINUOUS = 0x111;
	private static final String TAG = "PhotoManager";

    //不用Setup.isEmphasisFile判断是否为重点文件的原因是，
    // 在MainActivity里面onkeydown,长按时会执行多次最终
    // Setup.isEmphasisFile可能不准确。
    private static  boolean isEmphasis;//重点文件判断
    private Basket basket = new Basket();
    private boolean isSaving ;
    private static String mCacheDir;
    private SaveCallBack saveCallBack =null;
    private int photos = 0;

    public interface SaveCallBack{
        void onDataSaved(final Context context);
        void onNextShoot(final Context context, final int num);
        void onFinished(final Context context);
    }

	

    /**
     * 拍照
     * @param context
     * @param holder
     */

    public   void startPhotograph(SurfaceHolder holder, MainActivity context, SaveCallBack scb) {

        if (isSaving){
            Toast.makeText(context, "正在保存请稍后", Toast.LENGTH_SHORT).show();
            return;
        }

        saveCallBack = scb;

        Log.e("shoot",""+Setup.isContinuousShooting);

        isEmphasis = Setup.isEmphasisFile;

        if (this.context == null){
            this.context = context;
            mCacheDir = context.getCacheDir().getAbsolutePath();
        }
       if (this.holder == null){
           this.holder = holder;
       }

            if (CameraHandlerThread.camera !=null) {
                CameraHandlerThread.camera.takePicture(null,null,PhotoManager.getInstance());
           }

    }

    /**
     * 拍照完成时的回调
     * @param data
     * @param camera
     */
    @Override
    public void onPictureTaken(final byte[] data, final Camera camera)
    {
/*
        if (Setup.isContinuousShooting)
        {
            System.out.println("---------->开始连拍");
            try
            {
                //mHand.sendEmptyMessageDelayed(0x111, 5);
                mHand.sendEmptyMessageDelayed(MSG_PIC_CONTINUOUS, 5);
                basket.putData(data);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }else
        {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    saveData(data);
                    if (saveCallBack!=null) {
                        saveCallBack.onDataSaved(context);
                    }

                }
            }).start();
        }*/

        new Thread(new Runnable() {
            @Override
            public void run() {
                saveData(data);

                if (Setup.isContinuousShooting) {

                    if (++photos >= Setup.continuousShootNum){
                        if (saveCallBack!=null) {
                            saveCallBack.onFinished(context);
                        }
                        photos = 0;
                    }              else {
                        if (saveCallBack!=null) {
                            saveCallBack.onNextShoot(context,photos);
                        }
                        CameraHandlerThread.camera.takePicture(null,null,PhotoManager.getInstance());
                    }
                } else {
                    if (saveCallBack!=null) {
                        saveCallBack.onDataSaved(context);
                    }
                }
            }
        }).start();

        CameraHandlerThread.camera.startPreview();
        if (onCaptureListener !=null){
            onCaptureListener.onFinishCapture();
        }
    }

    public static String saveToSDCard(String path) throws IOException{
        InputStream inputStream = context.getContentResolver().openInputStream(Uri.fromFile(new File(path)));
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.RGB_565;
        options.inPurgeable = true;
        options.inInputShareable = true;
        Bitmap bitmap = BitmapFactory.decodeStream(inputStream,null,options);

        return saveToSDCard(null, UIUtil.bitmapToByte(bitmap,100));

    }

    /**
     * 将图片保存至sd卡
     * @param data
     * @return
     * @throws IOException
     */
    public static  String saveToSDCard(String path, byte[] data) throws IOException {

		if (path == null) {
        	//String filePath2 =FileUtils.getFilePath2(context,Setup.imgFilePostfix);
            path = FileUtils.getFilePath2(context,Setup.imgFilePostfix);
		}
//        if (isEmphasis)
//        {
//            filename =  TimeUtils.getDate()+"IMP."+ Setup.imgFilePostfix;
//        }
//        else
//        {
//            filename = TimeUtils.getDate()+"."+ Setup.imgFilePostfix;
//         }

//        String folder ="";
//        if (isEmphasis){
//
//                folder = Setup.interImgPaths+Setup.emphFileFolder;
//        }else
//        {
//                folder = Setup.interImgPaths+Setup.comFileFolder;
//        }
//        File fileFolder = new File(folder);
//        if (!fileFolder.exists()) { // 如果目录不存在，则创建一个名为"oracleen"的目录
//            //fileFolder.mkdir();
//            FileUtils.mkDir(fileFolder);
//        }
        final File jpgFile = new File(path);
        System.out.println("-------->>>"+jpgFile.getAbsolutePath());

        FileOutputStream outputStream = new FileOutputStream(jpgFile); // 文件输出流
        outputStream.write(data); // 写入sd卡中
        outputStream.close(); // 关闭输出流
        //拍照完成，将重点标识置为false
        // 20181222 此处按要求对照片进行拉伸
        /*new Thread(new Runnable() {
            @Override
            public void run() {
                //RgbInterpolation.photoEnlargeImp(jpgFile.getPath(), jpgFile.getPath()+".lag");
                //RgbInterpolation.enlargeBitmap(jpgFile.getPath(),jpgFile.getPath()+".yuv");
            }
        }).start();*/
        return jpgFile.getPath();
    }

    public static String saveToCache(byte[] data) {

        if (mCacheDir == null) {
            mCacheDir = context.getCacheDir().getAbsolutePath();
        }
        String cache = mCacheDir+File.separator+"cache.jpg";
        AppLog.d (TAG, "photo cache: "+cache);

        File jpgCache = new File(cache);

        try {
            FileOutputStream outputStream = new FileOutputStream(jpgCache);
            outputStream.write(data);
            outputStream.close();
        } catch (IOException e) {
            AppLog.e (TAG, "sava photo cache error: "+e.toString());
        }

        return cache;
    }


    //实现对执法仪功能进行分类设计，实现
    private  PhotoManager() {}//单例模式：使其全局唯一（即使该类的所有属性全局唯一）
    private static volatile PhotoManager instance = null;

    public static PhotoManager getInstance() {
        if (instance == null) {
            synchronized (Setup.class) {
                if (instance == null) {
                    instance = new PhotoManager();
                }
            }
        }
        return instance;
    }

    //简单低效率的连拍
    int picNum;
     Handler mHand = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            //if (msg.what == 0x111){
            if (msg.what == MSG_PIC_CONTINUOUS){
                picNum++;
                if (picNum>=Setup.continuousShootNum){
                    picNum =0;
                    //开启线程保存图
                    SaveDataRunnable save = new SaveDataRunnable(basket);
                    save.run();
                    return;
                }else {
                    CameraHandlerThread.camera.takePicture(null,null,PhotoManager.getInstance());
                }
            }
        }
    };


//=============================================================================================

    /**
     * 定义存放数据的队列
     */
    public class Basket {
        BlockingQueue<byte[]> basket = new LinkedBlockingQueue<>();

        // 存数据到队列
        public void putData(byte[] data) throws InterruptedException {

             basket.put(data);
            Log.e("Basket","---->>>存数据");
        }

        // 从对列中取出数据
        public byte[] takeData() throws InterruptedException {
            Log.e("Basket","---->>>取数据");
            return basket.take();
        }

        public int getDataSize(){
            return basket.size();
        }
    }

     class SaveDataRunnable implements Runnable{

        Basket basket;
         public SaveDataRunnable(Basket basket   ) {
            this.basket = basket;
         }

        @Override
        public void run() {
            try {
                isSaving = true;
                while (basket.getDataSize()>0)
                {
                   byte [] data =  basket.takeData();
                   saveData(data);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }finally {
                isSaving = false;
            }
        }
    }


    private void saveData(byte [] data){
        try {

			AppLog.i(TAG,"data len: "+data.length);

            if ( data.length > 9*1024*1024 ) {
                saveToSDCard(null, data);
            }else {

                //saveToSDCard(data);
                String path = saveToCache(data);
                InputStream inputStream = context.getContentResolver().openInputStream(Uri.fromFile(new File(path)));
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inPreferredConfig = Bitmap.Config.RGB_565;
                options.inPurgeable = true;
                options.inInputShareable = true;
                options.inMutable = true;
                Bitmap bitmap = BitmapFactory.decodeStream(inputStream,null,options);
                //inputStream.close();
                //BitmapFactory.Options opt = new BitmapFactory.Options();
                //Bitmap bitmap = BitmapFactory.decodeFile(path,opt);

                //Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
                //Matrix matrix = new Matrix();
                //bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);

                //-----------------------------------------添加水印-----------------------------------------------------------------

                //在图片右下角添加水印
                //Bitmap textBitmap = ImageUtil.drawTextToRightBottom(context, path,
                bitmap = ImageUtil.drawTextToRightBottom(context, bitmap,
                        TimeUtils.milliseconds2String(System.currentTimeMillis()) + " " + Setup.policeNumber,
                        40, Color.WHITE, 30, 30);

                //左上角添加水印格式为 (_设备编号_警员编号_日期时间_序号)
                bitmap = ImageUtil.drawTextToLeftTop(context, bitmap, "_" + Setup.devNum + "_" + Setup.policeNumber + "_" +
                                TimeUtils.milliseconds2String2(System.currentTimeMillis()) + "010_0", 40,
                        Color.WHITE, 10, 10);
                //-------------------------------------------------------------------------------------------------------------------
                //bitmap.recycle();
                //inputStream.close();
                if (Setup.picturewidht > 3600) {
                    saveToSDCard(null, UIUtil.bitmapToByte(bitmap, 90));
                } else {
                    saveToSDCard(null, UIUtil.bitmapToByte(bitmap, 100));
                }

                bitmap.recycle();
                inputStream.close();
            }


        } catch (Exception e) {
            e.printStackTrace();
            AppLog.e (TAG, "save data error, "+e.toString());
        }
    }

    //=========================================================================================




    public interface  OnCaptureListener{
        public void onFinishCapture();
     }

    public static OnCaptureListener getOnCaptureListener() {
        return onCaptureListener;
    }

    public static void setOnCaptureListener(OnCaptureListener onCaptureListener) {

        PhotoManager.onCaptureListener = onCaptureListener;
    }

    public static OnCaptureListener onCaptureListener;

}
