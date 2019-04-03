package gbpe.policeass.db;

public class FaceDbSchema {

    public static final class FaceTable {
        public static final String NAME = "faces";

        public static final class Cols {
            public static final String UUDI = "uuid";
            public static final String IMG_PATH = "img_path";
            public static final String NAME = "name";
            public static final String FACE_ID = "id";
            public static final String REMARK = "remark";
            public static final String FEATURE = "feature";
        }

    }

    public static final class RecordTable {
        public static String NAME = "records";

        public static final class Cols {
            public static final String UUID = "uuid";
            public static final String IMG_PATH = "img_path";
            public static final String NAME = "name";
            public static final String LONGITUDE = "longitude";
            public static final String LATITUDE = "latitude";
            public static final String TIMESTAMP = "timestamp";
        }
    }

}
