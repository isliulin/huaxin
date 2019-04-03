package gbpe.policeass.utils;

import com.coremedia.iso.boxes.Container;
import com.googlecode.mp4parser.authoring.Movie;
import com.googlecode.mp4parser.authoring.Track;
import com.googlecode.mp4parser.authoring.builder.DefaultMp4Builder;
import com.googlecode.mp4parser.authoring.container.mp4.MovieCreator;
import com.googlecode.mp4parser.authoring.tracks.AppendTrack;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;

public class MergerMp4 {
    public List<String> mMP4List = null;
    private static final String TAG = "MergerMp4";
    private String mTargetFile;

    public MergerMp4(List<String> list, String target){
        mMP4List = list;
        mTargetFile = target;
    }

    public MergerMp4(String target){
        mMP4List = null;
        mTargetFile = target;
    }

    public void addMp4(String file){
        if(mMP4List==null){
            mMP4List = new ArrayList<>();
        }
        mMP4List.add(file);
    }

    public  void mergeMP4(){
        if (mMP4List == null){
            AppLog.i(TAG,"No file to parse");
            return;
        }

        if(mTargetFile == null){
            mTargetFile = FileUtils.getFilePath2(App.instance, Setup.videoFilePostfix);
        }

        List<com.googlecode.mp4parser.authoring.Movie> moviesList = new LinkedList<>();
        //添加需要合并的文件

        try{
            for (String file : mMP4List)
            {
                moviesList.add(MovieCreator.build(file));
            }
        }
        catch (IOException e)
        {
            AppLog.e(TAG,"add mp4 error");
            e.printStackTrace();
            return;
        }

        List<Track> videoTracks = new LinkedList<Track>();
        List<Track> audioTracks = new LinkedList<Track>();
        for (com.googlecode.mp4parser.authoring.Movie m : moviesList)
        {
            for (Track t : m.getTracks())
            {
                if (t.getHandler().equals("soun"))
                {
                    audioTracks.add(t);
                }
                if (t.getHandler().equals("vide"))
                {
                    videoTracks.add(t);
                }
            }
        }

        Movie result = new Movie();

        try
        {
            if (audioTracks.size() > 0)
            {
                result.addTrack(new AppendTrack(audioTracks.toArray(new Track[audioTracks.size()])));
            }
            if (videoTracks.size() > 0)
            {
                result.addTrack(new AppendTrack(videoTracks.toArray(new Track[videoTracks.size()])));
            }
        }
        catch (IOException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return;
        }

        Container out = new DefaultMp4Builder().build(result);

        try
        {
//输出合并后的文件
            FileChannel fc = new RandomAccessFile(mTargetFile+".tmp", "rw").getChannel();
            out.writeContainer(fc);
            fc.close();
        }
        catch (Exception e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return;
        }

        moviesList.clear();
        //mMP4List.clear();

        new File(mTargetFile+".tmp").renameTo(new File(mTargetFile));

    }
}
