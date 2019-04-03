package gbpe.policeass.views;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import gbpe.policeass.utils.CountDownTimerUtil;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/9/2 15:54
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/9/2 15:54
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class CountDownTimerView extends TextView {
    public CountDownTimerView(Context context) {
        super(context);
    }

    public CountDownTimerView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }


    /**
     * 取消倒计时
     */
    public void cancel() {
        timer.cancel();
        setVisibility(GONE);
    }

    /**
     * 开始倒计时
     */
    public void start() {
        setVisibility(VISIBLE);
        timer.start();
    }


    public void setCountdownInterval(int interval){
        timer.setCountdownInterval(interval);
    }

    public void setMillisInFuture(long millis){
        timer.setMillisInFuture(millis);

    }
    CountDownTimerUtil timer = new CountDownTimerUtil(365*24*3600 * 1000, 1000) {
        @Override
        public void onTick(long millisUntilFinished) {
            setText((millisUntilFinished / 1000)+"");
        }

        @Override
        public void onFinish() {
            timer.cancel();
            setVisibility(GONE);
        }
    };

}
