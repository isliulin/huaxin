package gbpe.policeass.Activity;

//import android.app.FragmentManager;
import android.app.Fragment;
import android.app.FragmentManager;
import android.os.Message;
//import android.support.v4.app.Fragment;
//import android.support.v4.app.FragmentManager;
import android.support.v13.app.FragmentStatePagerAdapter;
//import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager;
        import android.os.Bundle;
import android.view.KeyEvent;
        import android.view.Window;

//import com.guobao.lightboard.HardwareControler;

import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
        import gbpe.policeass.fragments.SettingSegFragment;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.DevControl;
import gbpe.policeass.utils.IRCutUtils;
import gbpe.policeass.utils.SOSUtil;
import gbpe.policeass.views.SimpleViewpagerIndicator;

public class SettingActivity extends BaseActivity implements ViewPager.OnPageChangeListener{

	private static final String TAG = "SettingActivity";
	private ViewPager mViewPager;
	private SimpleViewpagerIndicator mIndicator;
    //private String[] pageTitles = {"视频参数", "图像参数", "录音参数", "系统设置", "用户设置"};
    private String[] pageTitles;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.activity_setting);

        pageTitles = new String[5];
        pageTitles[0] = getResources().getString(R.string.video_para);
        pageTitles[1] = getResources().getString(R.string.picture_para);
        pageTitles[2] = getResources().getString(R.string.audio_para);
        pageTitles[3] = getResources().getString(R.string.system_para);
        pageTitles[4] = getResources().getString(R.string.user_para);

        mIndicator = findViewById(R.id.indicator);
        mViewPager = findViewById(R.id.setting_viewpager);
        FragmentManager fm = getFragmentManager();
        mViewPager.setAdapter(new FragmentStatePagerAdapter(fm) {
            @Override
            public Fragment getItem(int position) {
                //return null;
                //SettingFragment sf =  SettingFragment.getInstance(position);
                AppLog.i(TAG, "get fragment "+position);
                //return SettingSegFragment.newInstance("viewPager "+position);
                //return new SettingFragment();
                return SettingSegFragment.newInstance(position);
            }

            @Override
            public int getCount() {
                //return 0;
                AppLog.i(TAG, "get count");
                return 5;
            }

            @Override
            public CharSequence getPageTitle(int position) {
                //return super.getPageTitle(position);
                return pageTitles[position%5];
            }
        });


        mIndicator.setViewPager(mViewPager);
        mIndicator.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

            }

            @Override
            public void onPageSelected(int position) {

            }

            @Override
            public void onPageScrollStateChanged(int state) {

            }
        });
        //setContentView(R.layout.activity_setting);

        /*findViewById(R.id.add_reback_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });*/
        //getFragmentManager().beginTransaction().replace(R.id.camera_preview,
                //new SettingFragment()).commit();
        //getFragmentManager().beginTransaction().replace(R.id.camera_preview,new SettingFragment()).commit();
        //getFragmentManager().beginTransaction().replace(R.id.camera_preview,
                                                //SettingSegFragment.newInstance("viewPager start")).commit();

        //mViewPager = new ViewPager(this);
        //mViewPager.setId(R.id.view_pager);
        //setContentView(mViewPager);
        //mViewPager = findViewById(R.id.setting_viewpager);

        //FragmentManager fm = getSupportFragmentManager();


    }

    @Override
    protected void handler(Message msg) {
    }

    @Override
    protected void initView() {
    }

    /**
     * 处理按键的长按逻辑
     * @param keyCode
     * @param event
     */
    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {


        if (keyCode == KeyEvent.KEYCODE_2)//保存为重点文件
        {
            Setup.whiteLight = !Setup.whiteLight;
            if (Setup.whiteLight){
                //HardwareControler.SetWhiteLightState(1);
                DevControl.writeGuobaoGpio(96,1);
            }else {
                //HardwareControler.SetWhiteLightState(0);
                DevControl.writeGuobaoGpio(96,0);
            }
            return true;
        }else if (keyCode == KeyEvent.KEYCODE_7)//打开红外线
        {
            if (Setup.inFraRed){
                return true;
            }
            //IRCutUtils.IRcutAction();
            IRCutUtils.setIRstate(!IRCutUtils.getIRstate());
            IRCutUtils.IRcutAction(IRCutUtils.getIRstate());
            return true;
        }
        return false;
    }


    /**
     * 用于判断是短按还是长按。
     * 当程序中既要用到长按操
     * 作又要用到短按操作的话
     * 必须在onKeyDown（）方
     * 法中调用event.startTracking()
     * 判断是否为长按事件，否
     * 则无法实现长按
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_2)
        {
            shortOrLongPress(event);
            return true;
        }else if (keyCode == KeyEvent.KEYCODE_7)
        {
            shortOrLongPress(event);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }



    /**
     * 用于按键的短按逻辑处理
     * @param keyCode
     * @param event
     */
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_2)
        {
            if (shortPress)
            {
                Setup.laserLamp = !Setup.laserLamp;
                if (Setup.laserLamp){
                    //HardwareControler.SetLaserState(1);
                    DevControl.writeGuobaoGpio(97,1);
                }else {
                    //HardwareControler.SetLaserState(0);
                    DevControl.writeGuobaoGpio(97,0);
                }
            }
            shortPress = false;
            return true;
        }
//----------------------------------紧急呼叫------------------------------------------------------------------------//
        else if (keyCode == KeyEvent.KEYCODE_7)//单按一次呼叫SOS、长按打开/关闭红外灯（按SOS白光灯发出报警闪烁信号，扬声器发出警笛声音）
        {
            if (shortPress)
            {
                if (shortPress)
                {
                    Setup.isWarn = !Setup.isWarn;
                    if (Setup.isWarn) {
                        //  SOSUtil.startSOS(keySound);
                        SOSUtil.sosStart(keySound);
                    }else {
                        //  SOSUtil.stopSOS(keySound);
                        SOSUtil.sosStop();
                    }
                }

            }
            shortPress = false;

            return true;
        }
        return super.onKeyUp(keyCode, event);
    }


    boolean shortPress;
    /**
     * 是否为长按或短按事件的逻辑判断方法
     * @param event
     */
    private void shortOrLongPress(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            event.startTracking();
            if (event.getRepeatCount() == 0)
            {
                shortPress = true;
            }else
            {
                shortPress = false;
            }
        }
    }


    @Override
    public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {
        AppLog.i(TAG, "onPageScrolled");
    }

    @Override
    public void onPageSelected(int position) {
        AppLog.i(TAG, "onPageSelected");
    }

    @Override
    public void onPageScrollStateChanged(int state) {
        AppLog.i(TAG, "onPageScrollStateChanged");
    }
}
