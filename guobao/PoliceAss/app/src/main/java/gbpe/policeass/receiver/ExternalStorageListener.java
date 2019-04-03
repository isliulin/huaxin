package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/6/28 11:20
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/6/28 11:20
 * 
 * 描述：
 * =====================================================================================
 */
public class ExternalStorageListener extends BroadcastReceiver {


    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();

        if (action.equals(Intent.ACTION_MEDIA_EJECT)) {
            //TODO:
            System.out.println("-------------------> mount ACTION_MEDIA_EJECT");
            Log.d("TestFile", "SD card is not avaiable/writeable right now.");
            Toast.makeText(context, "ACTION_MEDIA_EJECT", Toast.LENGTH_SHORT).show();

        } else if (action.equals(Intent.ACTION_MEDIA_MOUNTED)) {
            //TODO:
            System.out.println("-------------------> mount ACTION_MEDIA_MOUNTED");
            Log.d("TestFile", "SD card is  still avaiable/writeable   now.");
            Toast.makeText(context, "ACTION_MEDIA_MOUNTED", Toast.LENGTH_SHORT).show();
        }
    }
}