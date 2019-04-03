package gbpe.policeass.fragments;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.CommonAdapter;
import gbpe.policeass.adapters.ViewHolder;
import gbpe.policeass.utils.FileUtils;


/**
 * Created by Administrator on 2016/10/8.
 */
public class GetFileTypeAudioListsFragment extends BaseFragment {
    View mView;
    private List<String> soundPaths;
    private ListView lv_video;
    private ImageView imageView;
    public static final String TAG = "GetFlieTypeVideoListFragment";
    private TextView tv_type;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.fragment_get_file_type_videolist,container,false);
        lv_video = (ListView)mView.findViewById(R.id.lv_get_typefile_video);
        imageView = (ImageView) mView.findViewById(R.id.image_back);
        tv_type = (TextView)mView.findViewById(R.id.tv_type);
        tv_type.setText("录音文件");
        imageView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });

        initDatas();
        return mView;
    }


    /**
     * 初始化数据
     */
    public void initDatas() {
        soundPaths = new LinkedList<>();
        if (!soundPaths.isEmpty()){
            soundPaths.clear();
        }
        getFilePath(FileUtils.getInnerFileSavedPaht(getActivity()));
        if (soundPaths.size()<=0) {
            Toast.makeText(getActivity(), "还没有音频文件", Toast.LENGTH_SHORT).show();
            return;
        }
    }


    //递归获得每个文件的绝对路径
    public  void getFilePath(String dirPath){
        File dir = new File(dirPath);
        File[] files = dir.listFiles();
        if(files == null){
            return ;
        }
        for(int i = 0; i < files.length; i++){
            if(files[i].isDirectory())
            {
                getFilePath(files[i].getAbsolutePath());
            } else
            {
                if (files[i].getAbsolutePath().
                        endsWith(Setup.soundFilePostfix)) {
                    soundPaths.add(files[i].getAbsolutePath());
                }
            }
        }

        if (soundPaths.size()<=0){
            return;
        }
        if (SoundListFragment3.data !=null) {
            if (SoundListFragment3.data.size() >= 0) {
                SoundListFragment3.data.clear();
            }
            for (int i=0;i<soundPaths.size();i++){
                HashMap<String ,String> map = new HashMap<String, String>();
                String filePath = soundPaths.get(i);
                String fileName = new File(filePath).getName();
                map.put("name",fileName);
                map.put("path",filePath);
                SoundListFragment3.data.add(map);
            }
        }
    }




    @Override
    public void onResume() {
        super.onResume();
        CommonAdapter<String> adapter = new CommonAdapter<String>(activity,
                R.layout.item_listview_getfiletypevideo_lists,soundPaths) {
            @Override
            public void conver(final ViewHolder holder, String s) {
                TextView textView = holder.getView(R.id.tv_video_lists);
                String fileName = new File(s).getName();
                StringBuffer sb = new StringBuffer();
                String [] message = fileName.split("_");
                sb.append(message[2]).append("_").append(message[3]);//  123456_456_20100101083001_0001

                textView.setText(sb.toString());
                textView.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        //播放声音
                        Bundle bundle = new Bundle();
                        bundle.putInt("position",holder.getPosition());
                        activity.showFragment2(GetFileTypeAudioListsFragment.this,SoundListFragment3.class,
                                "SoundListFragment3",bundle,true);
                    }
                });
            }
        };
        lv_video.setAdapter(adapter);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    protected boolean onBackPressed() {
        return false;}
}
