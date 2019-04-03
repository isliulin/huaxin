package com.arcsoft.arcfacedemo.faceserver;

import gbpe.policeass.db.FaceItem;

/** 比较结果类，包含名称，相似度(浮点类型)，track id**/
public class CompareResult {
    //private String userName;
    private float similar;
    private int trackId;
    private FaceItem faceItem;
    //private String id;
    //private String remark;
    //private String imgPath;

    //public CompareResult(String userName, float similar) {
    public CompareResult(FaceItem face, float similar) {
        faceItem = face;
        this.similar = similar;
    }

    public FaceItem getFace(){return faceItem;}

    //public void setId(String id) {this.id = id;}
    //public String getId() {return id;}

    //public void setRemark(String remark) {this.remark = remark;}
    //public String getRemark() {return remark;}

    //public void setImgPath(String path) {this.imgPath = path;}
    //public String getImgPath() {return imgPath;}

    //public String getUserName() {  return userName; }

    //public void setUserName(String userName) {  this.userName = userName; }

    public float getSimilar() {
        return similar;
    }

    public void setSimilar(float similar) {
        this.similar = similar;
    }

    public int getTrackId() {
        return trackId;
    }

    public void setTrackId(int trackId) {
        this.trackId = trackId;
    }
}
