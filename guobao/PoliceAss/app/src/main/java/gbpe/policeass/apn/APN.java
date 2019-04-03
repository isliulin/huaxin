package gbpe.policeass.apn;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.net.Uri;
import android.telephony.TelephonyManager;

import gbpe.policeass.utils.AppLog;

public class APN {
    //取得全部的APN列表：
    public static final Uri APN_URI = Uri.parse("content://telephony/carriers");
    //    取得当前设置的APN：
    public static final Uri preferapn = Uri.parse("content://telephony/carriers/preferapn");
    //取得current=1的APN
    public static final Uri CURRENT_APN_URI = Uri.parse("content://telephony/carriers/current");

    private static final String TAG = "APNUtils";

    private Context context;
    public static boolean hasAPN;

    public APN(Context context){
        this.context = context;
    }

    // 新增一个cmnet接入点
    //    public void APN() {
    //
    //        checkAPN();
    //    }

    public int addAPN() {
        int id = -1;
        String NUMERIC = getSIMInfo();
        if (NUMERIC == null) {
            return -1;
        }
        AppLog.i(TAG,"vpn: "+NUMERIC);

        ContentResolver resolver = context.getContentResolver();
        ContentValues values = new ContentValues();
        values.put("name", "专用APN");                                  //apn中文描述
        values.put("apn", "xxx.ln");                                     //apn名称
        values.put("type", "default");                            //apn类型
        values.put("numeric", NUMERIC);
        values.put("mcc", NUMERIC.substring(0, 3));
        values.put("mnc", NUMERIC.substring(3, NUMERIC.length()));
        values.put("proxy", "");                                        //代理
        values.put("port", "1116");                                         //端口
        values.put("mmsproxy", "");                                     //彩信代理
        values.put("mmsport", "");                                      //彩信端口
        values.put("user", "");                                         //用户名
        values.put("server","1.1.1.1");                                       //服务器
        values.put("password", "");                                     //密码
        values.put("mmsc", "");                                          //MMSC
        Cursor c = null;
        Uri newRow = resolver.insert(APN_URI, values);
        if (newRow != null) {
            c = resolver.query(newRow, null, null, null, null);
            int idIndex = c.getColumnIndex("_id");
            c.moveToFirst();
            id = c.getShort(idIndex);
        }
        if (c != null)
            c.close();
        return id;
    }

    protected String getSIMInfo() {
        TelephonyManager iPhoneManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        return iPhoneManager.getSimOperator();
    }

    // 设置接入点
    public void SetAPN(int id) {
        ContentResolver resolver = context.getContentResolver();
        ContentValues values = new ContentValues();
        values.put("apn_id", id);
        resolver.update(APN_URI, values, null, null);
//        resolver.update(CURRENT_APN_URI, values, null, null);
    }

    public void checkAPN() {
        // 检查当前连接的APN
        Cursor cr = context.getContentResolver().query(APN_URI, null, null, null, null);
        while (cr != null && cr.moveToNext()) {
            if (cr.getString(cr.getColumnIndex("apn")).equals("dlums.ln")) {
                AppLog.d(TAG, "apn: " + cr.getString(cr.getColumnIndex("apn")));

                int apnId = cr.getShort(cr.getColumnIndex("_id"));
                APN.hasAPN = true;
                AppLog.d(TAG, "id: " +apnId);
                setDefaultApn(apnId);
                break;
            }
        }
    }


    /**
     * 根据apnId将设置的APN选中
     *
     * @param apnId
     * @return
     */
    public boolean setDefaultApn(int apnId) {
        boolean res = false;
        AppLog.d(TAG,"setDefaultApn:" + apnId);
        ContentResolver resolver = context.getContentResolver();
        ContentValues values = new ContentValues();
        values.put("apn_id", apnId);

        try {
            resolver.update(preferapn, values, null, null);
            Cursor c = resolver.query(preferapn, new String[]{"name",
                    "apn"}, "_id=" + apnId, null, null);
            if (c != null) {
                res = true;
                c.close();
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return res;
    }

}
