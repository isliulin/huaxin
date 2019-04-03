package gbpe.policeass.db;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import gbpe.policeass.db.FaceDbSchema.RecordTable;

public class FaceRcdLab {
    private static FaceRcdLab sFaceRcdLab;

    private Context context;
    private SQLiteDatabase mDatabase;

    public static FaceRcdLab getInstance(Context context) {
        if (sFaceRcdLab == null) {
            sFaceRcdLab = new FaceRcdLab(context);
        }
        return sFaceRcdLab;
    }

    private FaceRcdLab (Context context) {
        this.context = context;
        mDatabase = new FaceRcdHelper(context).getWritableDatabase();
    }

    private static ContentValues getFaceRcdValue(FaceRecord faceRecord) {
        ContentValues values = new ContentValues();
        values.put(RecordTable.Cols.UUID, faceRecord.getUuid());
        values.put(RecordTable.Cols.NAME, faceRecord.getName());
        values.put(RecordTable.Cols.IMG_PATH, faceRecord.getPath());
        values.put(RecordTable.Cols.TIMESTAMP, faceRecord.getTimeStamp());
        values.put(RecordTable.Cols.LATITUDE, faceRecord.getLatitude());
        values.put(RecordTable.Cols.LONGITUDE, faceRecord.getLongitude());
        return values;
    }

    public void addRecordItem(FaceRecord faceRecord) {
        ContentValues values = getFaceRcdValue(faceRecord);
        mDatabase.insert(RecordTable.NAME,null, values);
    }

    public void updateRecord(FaceRecord faceRecord) {
        String uuidString = faceRecord.getUuid();
        ContentValues values = getFaceRcdValue(faceRecord);
        mDatabase.update(RecordTable.NAME, values, RecordTable.Cols.UUID + " = ?",
                new String[]{uuidString});
    }

    private FaceCursorWrapper queryRecord(String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                RecordTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );
        return new FaceCursorWrapper(cursor);
    }

    public List<FaceRecord> getRecords() {
        List<FaceRecord> records = new ArrayList<>();

        FaceCursorWrapper cursor = queryRecord(null,null);

        try {
            cursor.moveToFirst();
            while (!cursor.isAfterLast()) {
                records.add(cursor.getFaceRecord());
                cursor.moveToNext();
            }
        } finally {
            cursor.close();
        }

        return records;
    }

    public FaceRecord getFaceRecord (UUID uuid) {
        FaceCursorWrapper cursor = queryRecord(
                RecordTable.Cols.UUID + " = ?",
                new String[] {uuid.toString()}
        );

        try {
            if (cursor.getCount() == 0) {
                return null;
            }
            cursor.moveToFirst();
            return cursor.getFaceRecord();
        } finally {
            cursor.close();
        }
    }
}
