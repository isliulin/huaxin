package gbpe.policeass.utils;

import android.annotation.SuppressLint;
import android.util.Log;

import java.io.File;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/8/4 10:44
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/8/4 10:44
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class VideoCombineUtil {

    public static final String TAG = "VideoCombineUtil";
    public static void mix(final  String folder ){
        //合并预录部分视频
        new Thread(new Runnable() {
             @Override
            public void run() {
                //合并预录部分视频文件
              //   CameraHandlerThread.camera.stopPreview();
                 new VideoHandleUtils(true).combine(folder);
            }
        }).start();

    }

}
