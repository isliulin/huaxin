package gbpe.policeass.fragments;

import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.RecycleHolder;
import gbpe.policeass.adapters.RecyclerAdapter;

import gbpe.policeass.utils.FileSizeUtil;
import gbpe.policeass.views.CommolySearchView;

/**
 * Created by Administrator on 2017/1/3.
 */
public class SoundListFragment2 extends BaseFragment {
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    protected boolean onBackPressed() {

        return false;
    }

    private RecyclerView rv_video;
    private CommolySearchView<String> searchView;
    private RecyclerAdapter<String> adapter;
    private List<String> soundPaths;

    private View view;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {


        view = inflater.inflate(R.layout.fragment_sound_list2,null);

        initDatas();
        rv_video = (RecyclerView)view.findViewById(R.id.rv_video);
        searchView = (CommolySearchView) view.findViewById(R.id.csv_show);
        rv_video.setLayoutManager(new LinearLayoutManager(mContext));
        view.findViewById(R.id.image_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        adapter = new RecyclerAdapter<String>(mContext, soundPaths, R.layout.video_item) {
            @Override
            public void convert(RecycleHolder holder, String data, int position) {
                ImageView iv_thumb = holder.findView(R.id.thumb_image);
                TextView tv_title = holder.findView(R.id.video_title);
                iv_thumb.setImageBitmap(BitmapFactory.decodeResource(getResources(),
                        R.drawable.myvoice));
                String filePath = soundPaths.get(position);
                String fileName = new File(filePath).getName();
                String fileTitle = fileName.substring(0,fileName.indexOf("."));
                tv_title.setText(fileTitle);
                TextView tv_size = holder.findView(R.id.video_size);
                String fileSize = ""+FileSizeUtil.getFileOrFilesSize(data,1);
                tv_size.setText(fileSize);
            }
        };
        adapter.notifyDataSetChanged();
        rv_video.setAdapter(adapter);
//-----------------------------------------------------搜索功能设置--------------------------------------------------------------/
        searchView.setDatas(soundPaths);
        searchView.setAdapter(adapter);
        searchView.setSearchDataListener(new CommolySearchView.SearchDatas<String>()
        {
            @Override
            public List<String> filterDatas(List<String> datas, List<String> filterdatas, String inputstr)
            {

                SoundListFragment3.data.clear();
                for (int i = 0; i < datas.size(); i++)
                {
                    String fileSize = "";
                    fileSize = ""+FileSizeUtil.getFileOrFilesSize(datas.get(i),1);
                    String filePath = datas.get(i);
                    String fileName = new File(filePath).getName();
                    String fileTitle = fileName.substring(0,fileName.indexOf("."));
                    // 筛选条件
                    if ((fileTitle).contains(inputstr)||fileSize.contains(inputstr))
                    {
                        filterdatas.add(datas.get(i));
                        HashMap<String ,String> map = new HashMap<String, String>();
                        map.put("name",fileName);
                        map.put("path",filePath);
                        SoundListFragment3.data.add(map);
                     }
                }
                return filterdatas;
            }
        });



        adapter.setOnItemClickListener(new RecyclerAdapter.OnItemClickListener() {
            @Override
            public void OnItemClickListener(View view, int position) {
                //播放声音
                Bundle bundle = new Bundle();
                bundle.putInt("position",position);
                activity.showFragment2(SoundListFragment2.this,SoundListFragment3.class,
                        "SoundListFragment3",bundle,true);


            }
        });
        adapter.setOnItemLongClickListener(new RecyclerAdapter.OnItemLongClickListener() {
            @Override
            public void OnItemLongClickListener(View view, int position) {
               // Toast.makeText( mContext, "你长按了"+position, Toast.LENGTH_SHORT).show();
                showMarkImpFileDialog();
            }
        });
//---------------------------------------------------------------------------------------------------------------------------/
    }

    public void showMarkImpFileDialog(){
        View view = View.inflate(activity,R.layout.dialog_mark_file,null);
        TextView tv_dialog = (TextView) view.findViewById(R.id.tv_dialog);
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        final AlertDialog mDialog = builder.create();

        mDialog.setView(view, 0, 0, 0, 0);
        mDialog.show();
        tv_dialog.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(activity, "标记完成", Toast.LENGTH_SHORT).show();
                mDialog.dismiss();
            }
        });
    }


    /**
     * 初始化数据
     */
    public void initDatas() {
        soundPaths = new LinkedList<>();
        getFilePath(Setup.soundPaths);//SD卡DCIM下面
        getFilePath(Setup.interVideoPaths);
        if (soundPaths.size()<=0)
        {
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
}
