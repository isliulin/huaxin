package gbpe.policeass.db;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.media.FaceDetector;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import gbpe.policeass.db.FaceDbSchema.FaceTable;

public class FaceLab {
    private static FaceLab sFaceLab;

    //private List<FaceItem> mFaces;
    private Context mContext;
    private SQLiteDatabase mDatabase;

    public static FaceLab get(Context context) {
        if (sFaceLab == null) {
            sFaceLab = new FaceLab(context);
        }

        return sFaceLab;
    }

    private FaceLab (Context context) {
        mContext = context.getApplicationContext();
        mDatabase = new FaceBaseHelper(mContext)
                .getWritableDatabase();
    }

    private static ContentValues getContentValues(FaceItem faceItem) {
        ContentValues values = new ContentValues();
        values.put(FaceTable.Cols.UUDI, faceItem.getUUID());
        values.put(FaceTable.Cols.NAME, faceItem.getName());
        values.put(FaceTable.Cols.FACE_ID, faceItem.getId());
        values.put(FaceTable.Cols.REMARK, faceItem.getRemark());
        values.put(FaceTable.Cols.IMG_PATH, faceItem.getImgPath());
        values.put(FaceTable.Cols.FEATURE,faceItem.getFeaturePath());

        return values;
    }

    public void addFaceItem(FaceItem faceItem) {
        ContentValues values = getContentValues(faceItem);
        mDatabase.insert(FaceTable.NAME,null,values);

    }

    public void updateFace(FaceItem faceItem) {
        String uuidString = faceItem.getUUID();
        ContentValues values = getContentValues(faceItem);
        mDatabase.update(FaceTable.NAME, values,
                FaceTable.Cols.UUDI + " = ?",
                new String[] {uuidString});
    }

    //private Cursor queryFaces(String whereClause, String[] whereArgs) {
    private FaceCursorWrapper queryFaces(String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                FaceTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,  //groupBy
                null,    //having
                null   //orderBy
        );
        return new FaceCursorWrapper(cursor);
    }

    public List<FaceItem> getFaces() {
        List<FaceItem> faceItems = new ArrayList<>();

        FaceCursorWrapper cursor = queryFaces(null, null);

        try {
            cursor.moveToFirst();
            while (!cursor.isAfterLast()) {
                faceItems.add(cursor.getFaceTtem());
                cursor.moveToNext();
            }
        } finally {
            cursor.close();
        }

        return faceItems;
    }

    public FaceItem getFaceItem(UUID uuid) {
        FaceCursorWrapper cursor = queryFaces(
                FaceTable.Cols.UUDI + " = ?",
                new String[] {uuid.toString()}
        );

        try {
            if (cursor.getCount() == 0) {
                return null;
            }
            cursor.moveToFirst();
            return cursor.getFaceTtem();
        } finally {
            cursor.close();
        }

    }

}
