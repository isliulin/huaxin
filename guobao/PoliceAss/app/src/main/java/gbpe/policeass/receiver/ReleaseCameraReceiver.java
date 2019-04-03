package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;
import org.greenrobot.eventbus.EventBus;

import gbpe.policeass.R;
import gbpe.policeass.modle.OnReleaseCamera;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/9/25 17:04
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/9/25 17:04
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class ReleaseCameraReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        //Toast.makeText( context, "收到了释放摄像头的广播", Toast.LENGTH_SHORT).show();
        Toast.makeText( context, R.string.release_camera_tips, Toast.LENGTH_SHORT).show();

       //发送消息给主界面，让他停止当前正在做的工作，然后释放摄像头
        EventBus.getDefault().post(new OnReleaseCamera("释放摄像头"));

    }


}
