package gbpe.policeass.utils;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/9/16 16:31
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/9/16 16:31
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class BluetoothUtil {
    public static boolean isOpenBluetooth(){
        BluetoothAdapter blueadapter= BluetoothAdapter.getDefaultAdapter();
        if (blueadapter==null){
            return false;
        }
     return blueadapter.isEnabled()? true:false;
    }
}
