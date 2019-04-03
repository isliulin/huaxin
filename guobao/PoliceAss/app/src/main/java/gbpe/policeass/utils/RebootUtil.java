package gbpe.policeass.utils;

import android.content.Context;
import android.content.Intent;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/5/22 11:37
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/5/22 11:37
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class RebootUtil {

    public static void reoot(Context context){
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_REBOOT);
        intent.putExtra("nowait", 1);
        intent.putExtra("interval", 1);
        intent.putExtra("window", 0);
        context.sendBroadcast(intent);
    }
}
