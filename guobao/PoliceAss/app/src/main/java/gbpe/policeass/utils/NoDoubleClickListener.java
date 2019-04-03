package gbpe.policeass.utils;

/**
 * Created by Administrator on 2016/10/11.
 */
import android.view.View;

import java.util.Calendar;

import gbpe.policeass.fragments.BaseFragment;

/**
 * Created by Administrator on 2016/10/11.
 */
public abstract class NoDoubleClickListener extends BaseFragment implements View.OnClickListener {
    public abstract void onNoDoubleClick(View view);
    //public abstract boolean onKeyUp(int keyCode, KeyEvent event);
    public static final int MIN_CLICK_DELAY_TIME = 1000;
    private long lastClickTime = 0;

    @Override
    public void onClick(View v) {
        long currentTime = Calendar.getInstance().getTimeInMillis();
        if (currentTime - lastClickTime > MIN_CLICK_DELAY_TIME) {
            lastClickTime = currentTime;
            onNoDoubleClick(v);
        }
    }
}
