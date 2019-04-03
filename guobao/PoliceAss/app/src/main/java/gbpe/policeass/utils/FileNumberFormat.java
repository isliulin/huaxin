package gbpe.policeass.utils;

/**
 * Created by Administrator on 2017/7/22.
 */
public class FileNumberFormat {
    public static String formatter(int s){
        //int s=0;
        s=s==1000?1:s;
        // String reslut=s>=10?(s>=100?s+"":"00"+s):"000"+s; // 计算 转型
        String reslut=s>=10?(s>=100?(s>1000?s+"":"0"+s):"00"+s):"000"+s; // 计算 转型
        System.out.println(reslut); //
        return reslut;
    }
}
