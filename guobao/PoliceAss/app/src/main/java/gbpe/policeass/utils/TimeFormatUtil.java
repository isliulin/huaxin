package gbpe.policeass.utils;

import java.util.Formatter;
import java.util.Locale;

/**
 * Created by Administrator on 2016/9/6.
 */
public class TimeFormatUtil {
    /**
     * 时间格式化
     *
     * @param timeMs
     * @return
     */
    public static  String timeFormat(int timeMs) {
        int totalSeconds = timeMs;

        int seconds = totalSeconds % 60;
        int minutes = (totalSeconds / 60) % 60;
        int hours = totalSeconds / 3600;
        StringBuilder sb = new StringBuilder();
        Formatter formatter = new Formatter(sb, Locale.getDefault());
        if (hours > 0) {
            return formatter.format("%d:%02d:%02d", hours, minutes, seconds).toString();
        } else {
            return formatter.format("%02d:%02d", minutes, seconds).toString();
        }
    }
}
