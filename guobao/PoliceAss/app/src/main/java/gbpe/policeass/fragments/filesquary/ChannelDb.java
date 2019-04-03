package gbpe.policeass.fragments.filesquary;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Administrator on 2016/11/17.
 */
public class ChannelDb {
    private static List<Channel> selectedChannel=new ArrayList<Channel>();
    static{
        selectedChannel.add(new Channel("","视频",0,"",""));
        selectedChannel.add(new Channel("","图片",0,"",""));
        selectedChannel.add(new Channel("","音频",0,"",""));
        selectedChannel.add(new Channel("","日志",0,"",""));
//        selectedChannel.add(new Channel("","其它",0,"",""));
//        selectedChannel.add(new Channel("","文件",0,"",""));
//        selectedChannel.add(new Channel("","视频",0,"",""));
//        selectedChannel.add(new Channel("","图片",0,"",""));

    }
    public static List<Channel> getSelectedChannel(){
        return selectedChannel;
    }
}


