package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

/**
 * Created by Administrator on 2016/11/24.
 */
public class BatteryReceiver extends BroadcastReceiver {


    private BatteryElectric batteryElectric;


//监控电池电量
    public static void setPercent(int percent) {

        BatteryReceiver.percent = percent;
    }

    public static int percent = 0;
    @Override
    public void onReceive(final Context context, Intent intent) {
        int current=intent.getExtras().getInt("level");//获得当前电量
        int total=intent.getExtras().getInt("scale");//获得总电量
         // percent=current*100/total;
          setPercent(current*100/total);

        batteryElectric.getElectricLevel(percent);
        if (percent<=10)
        {
            Toast.makeText(context,"电量不足", Toast.LENGTH_SHORT).show();
        }

    }
    //获得电池电量百分比
    public static int getPercent() {
        return percent;
    }
    public interface BatteryElectric{
           public void getElectricLevel(int percent);
    }
    public void setBatteryElectric(BatteryElectric batteryElectric) {
        this.batteryElectric = batteryElectric;
    }

}
