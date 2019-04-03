package gbpe.policeass.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import static gbpe.policeass.db.FaceDbSchema.RecordTable;

public class FaceRcdHelper extends SQLiteOpenHelper {

    private static final int VERISON = 1;
    private static final String DATAVASE_NAME = "faceRecord.db";

    public FaceRcdHelper(Context context) {
        super(context, DATAVASE_NAME, null, VERISON);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("create table "+ RecordTable.NAME + "(" +
                " _id integer primary key autoincrement, " +
                RecordTable.Cols.UUID + ", " +
                RecordTable.Cols.NAME + ", " +
                RecordTable.Cols.TIMESTAMP + ", "+
                RecordTable.Cols.IMG_PATH + ", " +
                RecordTable.Cols.LATITUDE + ", " +
                RecordTable.Cols.LONGITUDE +
                ")"
        );
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}
