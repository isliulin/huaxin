package gbpe.policeass.fragments;

import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import com.bumptech.glide.Glide;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.RecycleHolder;
import gbpe.policeass.adapters.RecyclerAdapter;
import gbpe.policeass.utils.FileSizeUtil;
import gbpe.policeass.utils.FileUtils;
import gbpe.policeass.views.CommolySearchView;

/**
 * Created by Administrator on 2017/1/3.
 */
public class PhotoListFragment2 extends BaseFragment {
    private Bundle bundle;
    private RecyclerView rv_video;
    private CommolySearchView<String> searchView;
    private RecyclerAdapter<String> adapter;
    private ArrayList<String> imgPaths;

    @Override
    protected boolean onBackPressed() {

        return false;
    }
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_videolist2,container,false);
        rv_video = (RecyclerView)view.findViewById(R.id.rv_video);
        searchView = (CommolySearchView) view.findViewById(R.id.csv_show);
        rv_video.setLayoutManager(new LinearLayoutManager(mContext));
        view.findViewById(R.id.image_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });
        bundle = new Bundle();
        initDatas();
        return view;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        adapter = new RecyclerAdapter<String>(mContext,imgPaths,R.layout.video_item) {
            @Override
            public void convert(RecycleHolder holder, String data, int position) {
                ImageView iv_thumb = holder.findView(R.id.thumb_image);
                TextView tv_title = holder.findView(R.id.video_title);

                /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
                Glide
                        .with( getActivity())
                        .load( Uri.fromFile( new File( data ) ) )
                        .placeholder(R.drawable.ic_stub)
                        .error(R.drawable.videoerror)
                        .into( iv_thumb );
                String fileName = new File(data).getName();
                String fileTitle = fileName.substring(0,fileName.indexOf("."));

                tv_title.setText(fileTitle);
                TextView tv_size = holder.findView(R.id.video_size);
                String fileSize = ""+ FileSizeUtil.getFileOrFilesSize(data,1);
                tv_size.setText(fileSize);
            }
        };
        adapter.notifyDataSetChanged();
        rv_video.setAdapter(adapter);
//-----------------------------------------------------搜索功能设置--------------------------------------------------------------/
        searchView.setDatas(imgPaths);
        searchView.setAdapter(adapter);
        searchView.setSearchDataListener(new CommolySearchView.SearchDatas<String>()
        {
            @Override
            public List<String> filterDatas(List<String> datas, List<String> filterdatas, String inputstr)
            {
                imgPaths.clear();
                for (int i = 0; i < datas.size(); i++)
                {
                    String fileSize = ""+FileSizeUtil.getFileOrFilesSize(datas.get(i),1);
                    String filePath = datas.get(i);
                    String fileName = new File(filePath).getName();
                    String fileTitle = fileName.substring(0,fileName.indexOf("."));
                    // 筛选条件
                    if (fileTitle.contains(inputstr)||fileSize.contains(inputstr))
                    {
                        filterdatas.add(datas.get(i));
                    }
                }
                return filterdatas;
            }
        });
        adapter.setOnItemClickListener(new RecyclerAdapter.OnItemClickListener() {
            @Override
            public void OnItemClickListener(View view, int position) {
                //  Toast.makeText( mContext, "你点击了"+position, Toast.LENGTH_SHORT).show();

                if (imgPaths.size()>0&&imgPaths!=null)
                {
                    //ImgDetialActivity2.startMe(getActivity(),position,imgPaths);
                    Bundle bundle = new Bundle();
                    bundle.putInt("position",position);
                    bundle.putStringArrayList("imgPaths",imgPaths);
                    activity.showFragment2(PhotoListFragment2.this,ImgDetialFragment.class,
                            "ImgDetialFragment",bundle,true);
                }

            }
        });
        adapter.setOnItemLongClickListener(new RecyclerAdapter.OnItemLongClickListener() {
            @Override
            public void OnItemLongClickListener(View view, int position) {
                Toast.makeText( mContext, "你长按了"+position, Toast.LENGTH_SHORT).show();
            }
        });
    }



//---------------------------------------------------------------------------------------------------------------------------/


    /**
     * 初始化数据
     */
    public void initDatas() {
        imgPaths = new ArrayList<>();

         getFilePath(FileUtils.getInnerFileSavedPaht(getActivity()));
        if (imgPaths.size()<=0) {
            Toast.makeText(getActivity(), "还没有视频文件", Toast.LENGTH_SHORT).show();
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
            if(files[i].isDirectory()){
                getFilePath(files[i].getAbsolutePath());
            }
            else {
                if (files[i].getAbsolutePath().
                        endsWith(Setup.imgFilePostfix))
                {
                    imgPaths.add(files[i].getAbsolutePath());
                }
            }
        }
    }

}
