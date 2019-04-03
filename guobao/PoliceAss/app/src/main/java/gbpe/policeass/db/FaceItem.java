package gbpe.policeass.db;

import java.util.UUID;

public class FaceItem {

    private String mName;
    private String mId;
    private String mRemark;
    private String mImgPath;
    private UUID mUUID;
    private String featurePath;

    public String getFeaturePath() {return featurePath;}
    public void setFeaturePath(String featurePath) {this.featurePath = featurePath;}

    public String getUUID(){
        return mUUID.toString();
    }

    public String getName() {
        return mName;
    }

    public void setName(String mName) {
        this.mName = mName;
    }

    public String getId() {
        return mId;
    }

    public void setId(String mId) {
        this.mId = mId;
    }

    public String getRemark() {
        return mRemark;
    }

    public void setRemark(String mRemark) {
        this.mRemark = mRemark;
    }

    public String getImgPath() {
        return mImgPath;
    }

    public void setImgPath(String mImgPath) {
        this.mImgPath = mImgPath;
    }

    public FaceItem (String name, String id, String remark, String path, String featurePath){
        //mUUID = UUID.randomUUID();
        this(UUID.randomUUID());
        mName = name;
        mId = id;
        mRemark = remark;
        mImgPath = path;
        this.featurePath = featurePath;
    }

    public FaceItem (UUID uuid) {
        mUUID = uuid;
    }



}
