package gbpe.policeass.utils.screen;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.net.Uri;
import android.provider.Settings;
import android.view.WindowManager;
import android.widget.SeekBar;

/**
 * Created by Administrator on 2016/11/25.
 */
/*
 * Android调节屏幕亮度工具类
 * 注意：需要添加setting权限
 * <uses-permission android:name="android.permission.WRITE_SETTINGS">
 */
public class BrightnessUtils {

    // 判断是否开启了自动亮度调节

    public static boolean IsAutoBrightness(Context context)
    {

        boolean IsAutoBrightness = false;

        try {

            IsAutoBrightness = Settings.System.getInt(
                    context.getContentResolver(),

                    Settings.System.SCREEN_BRIGHTNESS_MODE) == Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
        }

        catch (Settings.SettingNotFoundException e)

        {
            e.printStackTrace();
        }

        return IsAutoBrightness;
    }


    // 获取当前屏幕的亮度

    public static int getScreenBrightness(Context context) {

        int nowBrightnessValue = 0;

        ContentResolver resolver = context.getContentResolver();

        try {

            nowBrightnessValue = android.provider.Settings.System.getInt(
                    resolver, Settings.System.SCREEN_BRIGHTNESS);

        }

        catch (Exception e) {

            e.printStackTrace();

        }

        return nowBrightnessValue;
    }

    // 设置亮度
    // 程序退出之后亮度失效

    public static void setCurWindowBrightness(Context context, int brightness) {

        // 如果开启自动亮度，则关闭。否则，设置了亮度值也是无效的
        if (IsAutoBrightness(context)) {
            stopAutoBrightness(context);
        }

        // context转换为Activity
        Activity activity = (Activity) context;
        WindowManager.LayoutParams lp = activity.getWindow().getAttributes();

        // 异常处理
        if (brightness < 1) {
            brightness = 1;
        }

        // 异常处理
        if (brightness > 255) {
            brightness = 255;
        }

        lp.screenBrightness = Float.valueOf(brightness) * (1f / 255f);

        activity.getWindow().setAttributes(lp);

    }



    // 设置系统亮度
    // 程序退出之后亮度依旧有效
    public static void setSystemBrightness(Context context, int brightness) {
        // 异常处理
        if (brightness < 1) {
            brightness = 1;
        }

        // 异常处理
        if (brightness > 255) {
            brightness = 255;
        }
        saveBrightness(context, brightness);
    }

    // 停止自动亮度调节

    public static void stopAutoBrightness(Context context) {

        Settings.System.putInt(context.getContentResolver(),

                Settings.System.SCREEN_BRIGHTNESS_MODE,

                Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
    }



    // 开启亮度自动调节

    public static void startAutoBrightness(Context context) {

        Settings.System.putInt(context.getContentResolver(),

                Settings.System.SCREEN_BRIGHTNESS_MODE,

                Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC);

    }

    // 保存亮度设置状态

    public static void saveBrightness(Context context, int brightness) {

        Uri uri = android.provider.Settings.System
                .getUriFor(Settings.System.SCREEN_BRIGHTNESS);

        android.provider.Settings.System.putInt(context.getContentResolver(),
                Settings.System.SCREEN_BRIGHTNESS, brightness);

        context.getContentResolver().notifyChange(uri, null);
    }

//---------------------------------------------------------------------------------------------------------------//
//
//    initBrightness();
//    seekbar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
//
//        @Override
//        public void onStopTrackingTouch(SeekBar seekBar) {
//            brightness = seekBar.getProgress();
//            seekBar.setProgress(brightness);
//
//        }
//
//        @Override
//        public void onStartTrackingTouch(SeekBar seekBar) {
//        }
//
//        @Override
//        public void onProgressChanged(SeekBar seekBar, int progress,
//        boolean fromUser) {
//            int curProgress = seekBar.getProgress();// 得到当前进度值
//            // 当进度小于70时，设置成70，防止太黑看不见的情况。
////                if (curProgress < 70) {
////                    curProgress = 70;
////                }
//            // 根据当前进度改变屏幕亮度
//            Settings.System.putInt(getContentResolver(),
//                    Settings.System.SCREEN_BRIGHTNESS, curProgress);
//            curProgress = Settings.System.getInt(getContentResolver(),
//                    Settings.System.SCREEN_BRIGHTNESS, -1);
//            BrightnessUtils.setCurWindowBrightness(TestScreenBrightnessActivity.this,
//                    curProgress);
//            // BrightnessUtil.saveBrightness(TestScreenBrightnessActivity.this,
//            // curProgress);
//        }
//    });
//
//    /**
//     * 初始化屏幕亮度值
//     */
//    private void initBrightness() {
//        // 如果开启了自动亮度调节，则关闭之
//        if (BrightnessUtils.IsAutoBrightness(this)) {
//            BrightnessUtils.stopAutoBrightness(this);
//        }
//        // 读取文件中设置的亮度值
//        //   String light = SetAndGetDataUtil.GetData(this, "test", "light");
////        if (!"".equals(light)) {
////            brightness = Integer.valueOf(light);
////        } else {
////            brightness = BrightnessUtils.getScreenBrightness(this);
////        }
//        BrightnessUtils.setCurWindowBrightness(this, brightness);
//        seekbar.setProgress(brightness);
//        seekbar.setMax(255);
//    }

}