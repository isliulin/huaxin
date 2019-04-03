package gbpe.policeass.Activity;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.graphics.drawable.BitmapDrawable;
import android.hardware.Camera;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.DocumentsContract;
import android.support.v4.app.FragmentActivity;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

import gbpe.policeass.IdUtils.CameraManager;
import gbpe.policeass.R;
import gbpe.policeass.appinfo.App;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.AssestUtils;
import gbpe.policeass.views.progressBar.FaceRegInputDlg;
import gbpe.policeass.views.progressBar.IdLocalInfoDlg;
import gbpe.policeass.views.progressBar.IdRegInfoDlg;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Headers;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;
import org.w3c.dom.Text;

import com.googlecode.tesseract.android.TessBaseAPI;

public class IdRecogniseActivity extends FragmentActivity implements SurfaceHolder.Callback , Camera.PreviewCallback{

    private SurfaceView sfv;
    private CameraManager cameraManager;
    private static final String TAG = "IdRecogniseActivity";
    private boolean hasSurface;
    private OkHttpClient mOkHttpClient;
    private Context context;
    private boolean DEBUG = true;

    //训练数据路径，必须包含tesseract文件夹
    static final String TESSBASE_PATH = Environment.getExternalStorageDirectory() + "/";
    //识别语言英文
    static final String DEFAULT_LANGUAGE = "eng";
    private ImageView iv_result;
    private TextView id_info;
    private Switch sw_id_net;
    private boolean is_net = false;
    private String ROOT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator
            +"IdCard" + File.separator;
    private String PATH_ONLINE = ROOT_PATH+"online/";
    private String PATH_LOCAL = ROOT_PATH+"local/";
    private Bitmap pic_local_reg = null;
    private Bitmap pic_net_reg = null;

    //private FrameLayout frameLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            Window window = getWindow();
            window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        }
        setContentView(R.layout.activity_id_recognizer);

        try {
            initView();
        } catch (IOException e) {
            e.printStackTrace();
        }

        initClient();
        context = this;

        File dir = new File(ROOT_PATH);
        if (!dir.exists()) {
            dir.mkdirs();
        }
        dir = new File(ROOT_PATH+"local");
        if (!dir.exists()) {
            dir.mkdirs();
        }
        dir = new File(ROOT_PATH+"online");
        if (!dir.exists()) {
            dir.mkdirs();
        }

        new AssestUtils(this).init();

    }

    @Override
    public View onCreateView(View parent, String name, Context context, AttributeSet attrs) {
        WindowManager winManager=(WindowManager)getSystemService(Context.WINDOW_SERVICE);
        AppLog.i (TAG, "window size "+winManager.getDefaultDisplay().getWidth()+"x"+winManager.getDefaultDisplay().getHeight());
        AppLog.i (TAG, "rectangle: "+dp2px(300,this)+"x"+dp2px(185,this));
        return super.onCreateView(parent, name, context, attrs);
    }


    private void initClient() {
        mOkHttpClient = new OkHttpClient.Builder()
                .connectTimeout(1000, TimeUnit.MINUTES)
                .readTimeout(1000, TimeUnit.MINUTES)
                .writeTimeout(1000, TimeUnit.MINUTES)
                .build();
    }

    private static int dp2px(int value,Context context)
    {
        return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, value,context.getResources().getDisplayMetrics() );
    }


    private void initView() throws IOException {
        sfv = findViewById(R.id.sfv);
        SurfaceHolder surfaceHolder = sfv.getHolder();
        iv_result = findViewById(R.id.iv_result);
        id_info = findViewById(R.id.id_info);
        sw_id_net = findViewById(R.id.sw_id_net);
        sw_id_net.setChecked(is_net);
        sw_id_net.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                is_net = isChecked;
            }
        });
        //frameLayout = findViewById(R.id.rect_angle);

        /*final ViewTreeObserver vto = frameLayout.getViewTreeObserver();
        vto.addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener() {
            @Override
            public boolean onPreDraw() {
                vto.removeOnPreDrawListener(this);
                //int h = frameLayout.getMeasuredHeight();
                //int w = frameLayout.getMeasuredWidth();
                AppLog.i (TAG, "rectangle: "+frameLayout.getMeasuredWidth()+"x"+frameLayout.getMeasuredHeight());
                return true;
            }
        });*/
        if (hasSurface) {
            // activity在paused时但不会stopped,因此surface仍旧存在；
            // surfaceCreated()不会调用，因此在这里初始化camera
            initCamera(surfaceHolder);
        } else {
            // 重置callback，等待surfaceCreated()来初始化camera
            surfaceHolder.addCallback(this);
        }
    }

    private void initCamera(SurfaceHolder holder) throws IOException {
        cameraManager = new CameraManager();
        if (holder == null) {
            throw new IllegalStateException("No SurfaceHolder provided");
        }
        if (cameraManager.isOpen()) {
            return;
        }
        try {
            // 打开Camera硬件设备
            cameraManager.openDriver(holder, this);
            // 创建一个handler来打开预览，并抛出一个运行时异常
            cameraManager.startPreview(this);
        } catch (Exception ioe) {
            Log.d("zk", ioe.toString());

        }
    }

    private void uninitCamera() {
        if (cameraManager!=null) {
            cameraManager.closeDriver();
        }
    }

    public String getHtmlMsg(String s, String startContent, String endContent) {
        String[] id = s.split(startContent);
        String[] idTrue = id[1].split(endContent);
        return idTrue[0];
    }

    private void uploadAndRecognize(String url) {
        if (!TextUtils.isEmpty(url)) {
            File file = new File(url);
            //构造上传请求，类似web表单
            RequestBody requestBody = new MultipartBody.Builder().setType(MultipartBody.FORM)
                    .addPart(Headers.of("Content-Disposition", "form-data; name=\"callbackurl\""), RequestBody.create(null, "/idcard/"))
                    .addPart(Headers.of("Content-Disposition", "form-data; name=\"action\""), RequestBody.create(null, "idcard"))
                    .addPart(Headers.of("Content-Disposition", "form-data; name=\"img\"; filename=\"idcardFront_user.jpg\""), RequestBody.create(MediaType.parse("image/jpeg"), file))
                    .build();
            //进行包装，使其支持进度回调
            final Request request = new Request.Builder()
                    .header("Host", "ocr.ccyunmai.com:8080")
                    .header("Origin", "http://ocr.ccyunmai.com:8080")
                    .header("Referer", "http://ocr.ccyunmai.com:8080/idcard/")
                    .header("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2398.0 Safari/537.36")
                    .url("http://ocr.ccyunmai.com:8080/UploadImage.action")
                    .post(requestBody)
                    .build();
            //开始请求
            mOkHttpClient.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    hideProgressDlg();
                    Toast.makeText(context,"网络申请失败",Toast.LENGTH_SHORT).show();
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    hideProgressDlg();
                    final String result = response.body().string();
                    Document parse = Jsoup.parse(result);
                    final Elements select = parse.select("div#ocrresult");
                    AppLog.e(TAG, "onResponse: " + select.text());
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            //tv_result.setText(getHtmlMsg(select.text(), "公民身份号码:", "签发机关"));
                            //id_info.setText(getHtmlMsg(select.text(), "公民身份号码:", "签发机关"));
                            //id_info.setVisibility(View.VISIBLE);
                            // 姓名: 张红涛 性别: 男 民族: 汉 出生: 1982年11月01日 住址: 河南省巩义市小关镇龙门村外大堰坪58号 公民身份号码: 410181198211011014 签发机关: 有效期: 姓名: 张红涛 性别: 男 民族: 汉 出生: 1982年11月01日 住址: 河南省巩义市小关镇龙门村外大堰坪58号 公民身份号码: 410181198211011014 签发机关: 有效期:
                            /*final IdRegInfoDlg idRegInfoDlg = new IdRegInfoDlg(context);
                            idRegInfoDlg.setIdInfo("识别结果",getHtmlMsg(select.text(),"姓名:","性别"),
                                    getHtmlMsg(select.text(), "公民身份号码:", "签发机关"),getHtmlMsg(select.text(), "住址:", "公民身份号码"));
                            idRegInfoDlg.show();
                            new Handler().postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    //id_info.setVisibility(View.INVISIBLE);
                                    //faceRegInputDlg.dismiss();
                                    //idRegInfoDlg.show();
                                }
                            },10000);*/
                            final FaceRegInputDlg faceRegInputDlg = new FaceRegInputDlg(context,pic_net_reg);
                            faceRegInputDlg.setFaceInfo("识别结果",getHtmlMsg(select.text(),"姓名:","性别"),
                                    getHtmlMsg(select.text(), "公民身份号码:", "签发机关"),getHtmlMsg(select.text(), "住址:", "公民身份号码"));
                            faceRegInputDlg.setEditDisable();
                            faceRegInputDlg.show();
                        }
                    });

                }
            });
        }
    }

    private ProgressDialog progressDialog;
    private void showProgressDlg(String title) {
        progressDialog = new ProgressDialog(this);
        progressDialog.setMessage(title);
        progressDialog.show();
    }

    private void hideProgressDlg() {
        progressDialog.dismiss();
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

        if ((keyCode == KeyEvent.KEYCODE_5) || (keyCode == KeyEvent.KEYCODE_6)){
            cameraManager.takePicture(null, null, new Camera.PictureCallback() {
                @Override
                public void onPictureTaken(byte[] data, Camera camera) {
                    camera.startPreview();
                    if (context!=null) {
                        Toast.makeText(context,"picture ok",Toast.LENGTH_SHORT).show();
                        if (is_net) {
                            camera.addCallbackBuffer(data);
                            // 截取照片，上传，识别
                            Bitmap bitmap = BitmapFactory.decodeByteArray(data,0,data.length);
                            double left = bitmap.getWidth()*0.25;
                            double top = bitmap.getHeight()*0.21;
                            double w = bitmap.getWidth()*0.5;
                            double h = bitmap.getHeight()*0.58;
                            pic_net_reg = Bitmap.createBitmap(bitmap,(int)left,(int)top,(int)w,(int)h,null,false);
                            /*File dir = new File(ROOT_PATH);
                            if (!dir.exists()) {
                                dir.mkdirs();
                            }*/
                            String path = PATH_ONLINE+System.currentTimeMillis()+".jpg";
                            AppLog.d(TAG,"照片位置："+path);
                            File f = new File(path);
                            try {
                                FileOutputStream out = new FileOutputStream(f);
                                pic_net_reg.compress(Bitmap.CompressFormat.JPEG,100,out);
                                out.flush();
                                out.close();
                                AppLog.d(TAG, "照片已保存");
                            } catch (Exception e) {
                                AppLog.e(TAG, "照片保存失败, "+e.getMessage());
                                return;
                            }
                            uploadAndRecognize(path);
                            showProgressDlg("网络连接中");

                        } else {
                            // 本地识别
                            showProgressDlg("识别中");
                            LocalIdRecognize(data,camera);
                            hideProgressDlg();

                        }
                    }
                }
            });
        } else  if (keyCode==KeyEvent.KEYCODE_HOME) {
            is_net = !is_net;
            sw_id_net.setChecked(is_net);
        }

        return super.onKeyUp(keyCode, event);
    }

    private void LocalIdRecognize(byte[] data, Camera camera) {
        camera.addCallbackBuffer(data);
        //ByteArrayOutputStream baos;
        //byte[] rawImage;
       // Bitmap bitmap;

        //
        /*Camera.Size previewSize = camera.getParameters().getPreviewSize();//获取尺寸,格式转换的时候要用到
        BitmapFactory.Options newOpts = new BitmapFactory.Options();
        newOpts.inJustDecodeBounds = true;
        YuvImage yuvimage = new YuvImage(
                data,
                ImageFormat.NV21,
                previewSize.width,
                previewSize.height,
                null);
        baos = new ByteArrayOutputStream();
        yuvimage.compressToJpeg(new Rect(0, 0, previewSize.width, previewSize.height), 100, baos);// 80--JPG图片的质量[0-100],100最高
        rawImage = baos.toByteArray();
        //将rawImage转换成bitmap
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.RGB_565;
        bitmap = BitmapFactory.decodeByteArray(rawImage, 0, rawImage.length, options);*/

        Bitmap bitmapSrc = BitmapFactory.decodeByteArray(data,0,data.length);
        double left = bitmapSrc.getWidth()*0.25;
        double top = bitmapSrc.getHeight()*0.21;
        double w = bitmapSrc.getWidth()*0.5;
        double h = bitmapSrc.getHeight()*0.58;

        Bitmap destBitmap = Bitmap.createBitmap(bitmapSrc,(int)left,(int)top,(int)w,(int)h,null,false);

        if (DEBUG) {
            String path = PATH_LOCAL+System.currentTimeMillis()+".jpg";
            AppLog.d(TAG,"照片位置："+path);
            File f = new File(path);
            try {
                FileOutputStream out = new FileOutputStream(f);
                destBitmap.compress(Bitmap.CompressFormat.JPEG,100,out);
                out.flush();
                out.close();
            } catch (Exception e) {
                AppLog.e(TAG,e.getMessage());
            }
        }

        if (destBitmap == null) {
            Log.d("zka", "bitmap is nlll");
            return;
        } else {
            // iv_result.setImageBitmap(bit_hm);
            if(destBitmap != null){
                String localre = localre(destBitmap);
                if (localre.length() == 18) {
                    Log.e(TAG, "onPreviewFrame: "+localre );
                    //Toast.makeText(getApplicationContext(),localre,Toast.LENGTH_SHORT).show();
                    //IdLocalInfoDlg idLocalInfoDlg = new IdLocalInfoDlg(this);
                    //idLocalInfoDlg.setInfo(pic_local_reg,"号码",localre);
                    //idLocalInfoDlg.show();
                    AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setTitle("身份证");
                    builder.setMessage(localre);
                    //builder.setIcon(new BitmapDrawable(pic_local_reg));
                    Dialog dialog = builder.create();
                    dialog.show();
                } else {
                    Toast.makeText(this,"识别失败",Toast.LENGTH_SHORT).show();
                }
            }
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        //LocalIdRecognize(data,camera);
    }

    private String localre(Bitmap bmp) {
        String content = "";
        //bm = bm.copy(Bitmap.Config.ARGB_8888, true);
        //iv_result.setImageBitmap(bm);

        int x, y, w, h;
        x = (int) (bmp.getWidth() * 0.340);
        y = (int) (bmp.getHeight() * 0.800);
        w = (int) (bmp.getWidth() * 0.6 + 0.5f);
        h = (int) (bmp.getHeight() * 0.12 + 0.5f);
        pic_local_reg = Bitmap.createBitmap(bmp,x,y,w,h);
        TessBaseAPI baseApi = new TessBaseAPI();
        baseApi.init(TESSBASE_PATH, DEFAULT_LANGUAGE);
        //设置识别模式
        baseApi.setPageSegMode(TessBaseAPI.PageSegMode.PSM_SINGLE_LINE);
        //设置要识别的图片
        baseApi.setImage(pic_local_reg);
        baseApi.setVariable("tessedit_char_whitelist", "0123456789Xx");
        Log.e(TAG, "localre: "+ baseApi.getUTF8Text());
        content = baseApi.getUTF8Text();
        baseApi.clear();
        baseApi.end();
        return content;
    }

    public int dip2px(int dp) {
        float density = this.getResources().getDisplayMetrics().density;
        return (int) (dp * density + 0.5);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        try {
            initCamera(holder);
        } catch (IOException e) {
            AppLog.e(TAG, e.getMessage());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        hasSurface = false;
        //防止会出现Camera is being used after Camera.release() was called异常
        holder.removeCallback(this);
        uninitCamera();
    }
}
