package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

//import gbpe.policeass.zfysim.HomeActivity;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/6/29 16:28
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/6/29 16:28
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class HomeKeyEventBroadCastReceiver extends BroadcastReceiver {

    public static final String LOG_TAG = "HomeKeyEventBroadCastReceiver";
    static int time=0;
    @Override
    public void onReceive(Context context, Intent intent) {

//
//        String action = intent.getAction();
//         if (action.equals(Intent.ACTION_CLOSE_SYSTEM_DIALOGS)) {
//
//             time++;
//             if (time==2){
//                 Toast.makeText( context, "home键监听广播"+time, Toast.LENGTH_SHORT).show();
//                  //context.startActivity(new Intent(context,HomeActivity.class));
//                 time =0;
//             }
//         }
//
//
//
     }
}