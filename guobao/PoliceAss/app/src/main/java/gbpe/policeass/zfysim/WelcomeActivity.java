package gbpe.policeass.zfysim;

import android.os.Bundle;
import android.os.Message;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.log.LogUtils;
import gbpe.policeass.utils.TimeUtils;


public class WelcomeActivity extends BaseActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final View view = View.inflate(this, R.layout.activity_welcome, null);
        setContentView(view); //渐变展示启动屏
        AlphaAnimation aa = new AlphaAnimation(0.3f,1.0f);

        aa.setDuration(1000);
        view.startAnimation(aa);
        aa.setAnimationListener(new Animation.AnimationListener()
        {
            @Override
            public void onAnimationEnd(Animation arg0)
            {
                go(MainActivity.class);
                LogUtils.writeTxtToFile(getString(R.string.power_on),Setup.LOG_PATH_SDCARD_DIR,
                        TimeUtils.getLogDate()+".log");
                finish();
            }
            @Override
            public void onAnimationRepeat(Animation animation) {}
            @Override
            public void onAnimationStart(Animation animation) {} });
    }
    @Override
    protected void handler(Message msg) {}
    @Override
    protected void initView() {}

    @Override
    public void loadViewLayout() {

    }

    @Override
    public void findViewById() {

    }

    @Override
    public void setListener() {

    }

    @Override
    public void processLogic() {

    }
}

