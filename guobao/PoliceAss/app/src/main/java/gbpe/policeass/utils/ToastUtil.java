package gbpe.policeass.utils;

import android.app.Activity;
import android.content.Context;
import android.widget.Toast;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/12 16:13
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/12 16:13
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class ToastUtil {

    public static void toast(final Context context,final String msg){
        ((Activity)context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
