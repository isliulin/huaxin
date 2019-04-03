package gbpe.policeass.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import gbpe.policeass.db.FaceDbSchema.FaceTable;

public class FaceBaseHelper extends SQLiteOpenHelper {

    private static final int VERSION = 1;
    private static final String DATAVASE_NAME = "faceBase.db";

    public FaceBaseHelper(Context context) {
        super(context,DATAVASE_NAME,null, VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("create table "+ FaceTable.NAME + "(" +
                " _id integer primary key autoincrement, " +
                FaceTable.Cols.UUDI + ", " +
                FaceTable.Cols.NAME + ", " +
                FaceTable.Cols.FACE_ID + ", " +
                FaceTable.Cols.REMARK + ", " +
                FaceTable.Cols.IMG_PATH +", "+
                FaceTable.Cols.FEATURE +
                ")"
        );
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}
