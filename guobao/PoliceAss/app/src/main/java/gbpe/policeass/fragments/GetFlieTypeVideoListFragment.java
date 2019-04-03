package gbpe.policeass.fragments;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
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
public class GetFlieTypeVideoListFragment extends BaseFragment {
    View mView;
    private List<String> videoNames;
    private ListView lv_video;
    private ImageView imageView;
    private TextView tv_type;
    public static final String TAG = "GetFlieTypeVideoListFragment";
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.fragment_get_file_type_videolist,container,false);
        lv_video = (ListView)mView.findViewById(R.id.lv_get_typefile_video);
        tv_type = (TextView)mView.findViewById(R.id.tv_type);
        tv_type.setText("视频文件");
        imageView = (ImageView) mView.findViewById(R.id.image_back);
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
        videoNames = new LinkedList<>();
        if (!videoNames.isEmpty()){
            videoNames.clear();
        }
        getFilePath(FileUtils.getInnerFileSavedPaht(getActivity()));
        if (videoNames.size()<=0) {
            Toast.makeText(getActivity(), "还没有视频文件", Toast.LENGTH_SHORT).show();
            return;
        }
    }

    //递归获得每个文件的绝对路径
    public  void getFilePath(String dirPath){
        // List<String> fileList = new LinkedList<>();
        if (TextUtils.isEmpty(dirPath)) {
            return;
        }
        File dir = new File(dirPath);
        File[] files = dir.listFiles();
        if(files == null||files.length<=0){
            return ;
        }
        for(int i = 0; i < files.length; i++) {
            if(files[i].isDirectory()) {
                getFilePath(files[i].getAbsolutePath());
            } else {
                if (files[i].getAbsolutePath().endsWith(Setup.videoFilePostfix)) {
                    videoNames.add(files[i].getAbsolutePath());
                }
            }
        }
    }




    @Override
    public void onResume() {
        super.onResume();
        CommonAdapter<String> adapter = new CommonAdapter<String>(activity,
                R.layout.item_listview_getfiletypevideo_lists,videoNames) {
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
                        Bundle bundle = new Bundle();
                        bundle.putString("filePath",videoNames.get(holder.getPosition()));
                        activity.showFragment2(GetFlieTypeVideoListFragment.this,VideoPlayFragment.class,
                                "VideoPlayFragment",bundle,true);
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
