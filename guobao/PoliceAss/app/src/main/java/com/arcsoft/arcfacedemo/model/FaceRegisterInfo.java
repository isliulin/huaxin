package com.arcsoft.arcfacedemo.model;

import java.util.UUID;

public class FaceRegisterInfo {
    private byte[] featureData;
    private String name;   /* 名称即UUID*/
    private String uuid;

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public FaceRegisterInfo(byte[] faceFeature, String name, String uuid) {
        this.featureData = faceFeature;
        this.name = name;
        this.uuid = uuid;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public byte[] getFeatureData() {
        return featureData;
    }

    public void setFeatureData(byte[] featureData) {
        this.featureData = featureData;
    }


}
