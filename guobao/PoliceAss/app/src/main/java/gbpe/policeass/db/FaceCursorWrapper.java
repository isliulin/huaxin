package gbpe.policeass.db;

import android.database.Cursor;
import android.database.CursorWrapper;
import android.media.FaceDetector;

import java.util.UUID;

import gbpe.policeass.db.FaceDbSchema.FaceTable;
import gbpe.policeass.db.FaceDbSchema.RecordTable;


public class FaceCursorWrapper extends CursorWrapper {
    public FaceCursorWrapper(Cursor cursor) {
        super(cursor);
    }

    public FaceItem getFaceTtem() {

        String uuidString = getString(getColumnIndex(FaceTable.Cols.UUDI));
        String name = getString(getColumnIndex(FaceTable.Cols.NAME));
        String id = getString(getColumnIndex(FaceTable.Cols.FACE_ID));
        String remark = getString(getColumnIndex(FaceTable.Cols.REMARK));
        String imgPath = getString(getColumnIndex(FaceTable.Cols.IMG_PATH));
        String featurePath = getString(getColumnIndex(FaceTable.Cols.FEATURE));


        FaceItem faceItem = new FaceItem(UUID.fromString(uuidString));
        faceItem.setName(name);
        faceItem.setId(id);
        faceItem.setRemark(remark);
        faceItem.setImgPath(imgPath);
        faceItem.setFeaturePath(featurePath);

        return faceItem;
    }

    // add for get face recognition recorder
    public FaceRecord getFaceRecord() {
        String uuidStr = getString(getColumnIndex(RecordTable.Cols.UUID));
        String name = getString(getColumnIndex(RecordTable.Cols.NAME));
        String time = getString(getColumnIndex(RecordTable.Cols.TIMESTAMP));
        String path = getString(getColumnIndex(RecordTable.Cols.IMG_PATH));
        double longitude = getDouble(getColumnIndex(RecordTable.Cols.LONGITUDE));
        double latitude = getDouble(getColumnIndex(RecordTable.Cols.LATITUDE));

        FaceRecord faceRecord = new FaceRecord(UUID.fromString(uuidStr));
        faceRecord.setName(name);
        faceRecord.setTimeStamp(time);
        faceRecord.setPath(path);
        faceRecord.setLongitude(longitude);
        faceRecord.setLatitude(latitude);

        return faceRecord;
    }
}
