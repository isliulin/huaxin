package gbpe.policeass.utils.thumbnail;

import android.view.View;

import java.util.Calendar;

/**
 * Created by Administrator on 2016/10/15.
 */
public abstract class NoMulitClickListener implements View.OnClickListener {
    public abstract void onNoDoubleClick(View view);
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
