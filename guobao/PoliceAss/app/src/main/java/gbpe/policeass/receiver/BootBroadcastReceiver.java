package gbpe.policeass.receiver;

/**
 * Created by Administrator on 2017/3/4.
 */
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import gbpe.policeass.Activity.MainActivity;

/**
 * @author 编写人： xi
 * @date 创建时间： 2016/8/20
 * @Description 功能描述： 该类用来实现该app开机自动运行
 * http://blog.csdn.net/mmmccc000/article/details/52626075
 */
public class BootBroadcastReceiver extends BroadcastReceiver {


    public static final String TAG = "boot";
    /**
     * 可以实现开机自动打开软件并运行。
     */
    @Override
    public void onReceive(Context context, Intent intent) {

          Log.d("XRGPS", "BootReceiver.onReceive: " + intent.getAction());
         Log.d("XRGPS","------>>>>自启动程序即将执行");
        //MainActivity就是开机显示的界面
        Intent mBootIntent = new Intent(context, MainActivity.class);
        //下面这句话必须加上才能开机自动运行app的界面
        mBootIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(mBootIntent);

    }
}