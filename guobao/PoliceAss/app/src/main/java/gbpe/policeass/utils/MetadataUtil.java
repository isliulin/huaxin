package gbpe.policeass.utils;

import android.content.ContentValues;
import android.content.Context;
import android.provider.MediaStore;

/**
 * Created by Administrator on 2016/11/26.
 */
public class MetadataUtil {
    /**
     * 修改图片的DATAADDED 属性。
     * @param ctx
     * @param filepath
     */
    public static void changeImgMetaAdded(Context ctx,String filepath )
    {
        String params[] = new String[]{filepath};
        //   ctx.getContentResolver().delete(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, MediaStore.Images.Media.DATA + " LIKE ?", params);
        ContentValues contentValues = new ContentValues(1);
        contentValues.put(MediaStore.Images.Media.DATE_ADDED,TimeUtils.milliseconds2String(System.currentTimeMillis()) );
       // contentValues.put(MediaStore.Images.Media.TITLE,"This is a Test Picture and Created By CaoJin.");
        ctx.getContentResolver().update(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                contentValues,MediaStore.Images.Media.DATA+"=?",params);
    }

    public static void changeVideoMetaAdded(Context ctx,String filepath )
    {
        String params[] = new String[]{filepath};
        //   ctx.getContentResolver().delete(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, MediaStore.Images.Media.DATA + " LIKE ?", params);
        ContentValues contentValues = new ContentValues(1);
        contentValues.put(MediaStore.Images.Media.DATE_ADDED, ""+TimeUtils.milliseconds2String(System.currentTimeMillis()));
        // contentValues.put(MediaStore.Images.Media.TITLE,"This is a Test Picture and Created By CaoJin.");
        ctx.getContentResolver().update(MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
                contentValues,MediaStore.Images.Media.DATA+"=?",params);
    }
    /**
     * 修改视频的经纬度值
     * @param ctx 上下文
     * @param filepath 视频文件路径
     * @param longitude 经度
     * @param lattude 维度
     */
    public static void changeMetaDataLocation(Context ctx,String filepath,String longitude,String lattude)
    {
        String params[] = new String[]{filepath};
        ContentValues contentValues = new ContentValues(2);
        contentValues.put(MediaStore.Video.Media.LONGITUDE,longitude);
        contentValues.put(MediaStore.Video.Media.LATITUDE,lattude);
        ctx.getContentResolver().update(MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
                contentValues, MediaStore.Video.Media.DATA+"=?",params);
    }


}
