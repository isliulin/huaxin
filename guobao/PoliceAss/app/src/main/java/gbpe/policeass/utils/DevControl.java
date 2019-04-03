package gbpe.policeass.utils;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class DevControl {

    private static String TAG = "DevControl";
    private static String GB_GPIO_NODE = "/sys/class/gb_gpio/gb_gpio/Gpio";
    private static String GP_GPIO_WR_NODE = "/sys/class/gb_gpio/gb_gpio/Gpio_inout";


    public static boolean writeDevNode(String path, String value) {

        try {
            BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(path));
            bufferedWriter.write(value+"");
            bufferedWriter.close();
            AppLog.i(TAG,"write "+path+" "+value+" ok");
        } catch (IOException e) {
            AppLog.e (TAG, "write "+path+" error:"+e.getMessage());
            return false;
        }

        return true;
    }

    public static boolean writeGuobaoGpio(int pin, int value) {



        if (!writeDevNode(GB_GPIO_NODE,pin+"")) {
            return false;
        }

        if (!writeDevNode(GP_GPIO_WR_NODE,value+"")){
            return false;
        }

        return true;
    }

}
