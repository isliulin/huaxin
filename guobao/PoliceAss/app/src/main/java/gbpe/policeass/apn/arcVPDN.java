package gbpe.policeass.apn;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.telephony.TelephonyManager;

public class arcVPDN {
    private static final Uri APN_TABLE_URI = Uri.parse("content://telephony/carriers");
    private static final Uri PREFERRED_APN_URI = Uri.parse("content://telephony/carriers/preferapn");

    private Context gContext= null;
    private  int gDefaultApn= -1;

    public arcVPDN(Context _context){
        gContext= _context;
        gDefaultApn= Default( );
    }

    /**
     * 根据手机网络名称判断是否存在
     * @param _name
     * @return
     */
    public boolean Exisit(String _name)
    {
        Cursor mCursor = gContext.getContentResolver().query(APN_TABLE_URI, null, null, null, null);
        while(mCursor != null && mCursor.moveToNext()){
            String name = mCursor.getString(mCursor.getColumnIndex("name"));
            if(_name.equals(name)) return true;
        }

        return false;
    }

    /**
     * 添加vpdn
     * @param _name
     * @param _apn
     * @param _user
     * @param _pwd
     */
    public void AddApn(String _name, String _apn, String _user, String _pwd)
    {
        ContentResolver resolver = gContext.getContentResolver( );
        ContentValues values = new ContentValues( );
        values.put("name", _name);
        values.put("apn", _apn);

        TelephonyManager tm = (TelephonyManager)gContext.getSystemService(gContext.TELEPHONY_SERVICE);
        String numeric = tm.getSimOperator();
        String mcc = numeric.substring(0, 3);
        String mnc = numeric.substring(3, numeric.length());
        String SimOperator = tm.getSimOperator();

        values.put("user", _user);
        values.put("password", _pwd);
        values.put("mcc", mcc);
        values.put("mnc", mnc);
        values.put("numeric", SimOperator);

        Uri newRow = resolver.insert(APN_TABLE_URI, values);
    }

    /**
     * 根据网络名称返回该网络编号
     * @param _name
     * @return
     */
    public int GetApn(String _name)
    {
        Cursor mCursor = gContext.getContentResolver().query(APN_TABLE_URI, null, null, null, null);
        while(mCursor != null && mCursor.moveToNext())
        {
            int apnId = mCursor.getShort(mCursor.getColumnIndex("_id"));
            String name = mCursor.getString(mCursor.getColumnIndex("name"));
            if(_name.equals(name)) return apnId;
        }

        return -1;
    }

    /**
     * 获取默认网络的编号
     * @return
     */
    public int Default(){
        ApnNode apnNode = new ApnNode();
        Cursor mCursor = gContext.getContentResolver().query(PREFERRED_APN_URI, null, null, null, null);
        if(mCursor == null)return -1;

        int apnid= -1;
        while (mCursor != null && mCursor.moveToNext())
        {
            String id = mCursor.getString(mCursor.getColumnIndex("_id"));
            apnid= Integer.valueOf(id);
        }

        return apnid;
    }

    /**
     * 获取默认网络名称
     * @return
     */
    public String DefaultAPNName(){
        ApnNode apnNode = new ApnNode();
        Cursor mCursor = gContext.getContentResolver().query(PREFERRED_APN_URI, null, null, null, null);
        if(mCursor == null)return "";

        while (mCursor != null && mCursor.moveToNext())
        {
            String name = mCursor.getString(mCursor.getColumnIndex("name"));
            return name;
        }

        return "";
    }

    /**
     * 根据编号设置网络
     * @param _apnid
     */
    public void SetDefault(int _apnid){
        if(_apnid==gDefaultApn)return;

        ContentResolver resolver = gContext.getContentResolver();
        ContentValues values = new ContentValues();

        values.put("apn_id", _apnid);
        resolver.update(PREFERRED_APN_URI, values, null, null);
    }

    public void Release(){
        if(gDefaultApn!=-1)
        {
            ContentResolver resolver = gContext.getContentResolver();
            ContentValues values = new ContentValues();
            values.put("apn_id", gDefaultApn);
            resolver.update(PREFERRED_APN_URI, values, null, null);

        }
    }



}
