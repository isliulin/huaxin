package gbpe.policeass.views;

import android.content.Context;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Chronometer;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.SettingUtils;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/5/19 09:56
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/5/19 09:56
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class IndicatorView extends  RelativeLayout{
    public IndicatorView(Context context) {
        super(context);
    }

    public IndicatorView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public IndicatorView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * 开始计时
     */
  //  private void startCounter() {

//        //计时器清零
//        chronometer.setBase(SystemClock.elapsedRealtime());
//        int hour = (int)((SystemClock.elapsedRealtime() - chronometer.getBase()))/1000/60;
//        chronometer.setFormat("0"+String.valueOf(hour)+":%s");
//        chronometer.start();
//
//        if (!Setup.isNormalRecording)
//        {
//            if (SettingUtils.getBoolenValues(this,"apply_prerecord")){
//                iv_point.setImageResource(R.drawable.bluepoint);
//            }
//        }else if (Setup.isNormalRecording)
//        {
//            iv_point.setImageResource(R.drawable.point);
//        }
//        rl_indicate.setVisibility(View.VISIBLE);
//
//    }
//
//    /**
//     * 停止计时
//     */
//    public void stopCounter() {
//        chronometer.stop();
//        rl_indicate.setVisibility(View.INVISIBLE);
//    }
//
}
