package gbpe.policeass.fragments;

 import android.net.Uri;
 import android.os.Bundle;
 import android.support.annotation.Nullable;
 import android.view.KeyEvent;
 import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;
import com.bumptech.glide.Glide;
 import java.io.File;
import java.util.ArrayList;
import java.util.List;

 import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
 import gbpe.policeass.utils.FileUtils;
 import gbpe.policeass.utils.thumbnail.ImageUtils;


/**
 * Created by Administrator on 2016/11/2.
 */
public class QureyFragment extends BaseFragment implements
        AdapterView.OnItemSelectedListener,View.OnClickListener{
    private View view ;
    private Spinner sp_iemts,sp_rank;
    private ImageView iv_preview;
    private List<String> fileList = new ArrayList<>();//默认让其装的是为视频的路径
    private static int mposition = 0;
    private static String currentFolder;
    private static String currentFilePath;


    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_qurey,container,false);
        sp_iemts = findView(R.id.sp_items);sp_rank  = findView(R.id.sp_rank);
        iv_preview  = findView(R.id.iv_preview);
        findView(R.id.image_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });
        initData();
        return view;
    }


    @Override
    public void onResume()
    {
        super.onResume();
        sp_rank.setOnItemSelectedListener(this);
        sp_iemts.setOnItemSelectedListener(this);
        findView(R.id.tv_front).setOnClickListener(this);
        findView(R.id.tv_next).setOnClickListener(this);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    protected boolean onBackPressed() {
        return false;
    }

    public <T extends View> T findView(int ViewId) {
        return (T) view.findViewById(ViewId);
    }


    //设置下拉菜单的点击事件
    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
    {

        //获得spinear上item的文字
      String ss =   parent.getItemAtPosition(position).toString();

        if (ss.equals(getString(R.string.get_video_lists)))//视屏
        {
            if (fileList.size()<=0){
                return;
            }

            Toast.makeText(activity, "视频", Toast.LENGTH_SHORT).show();
            currentFolder = Setup.videoPaths;

            fileList.clear();
            getFilePath(currentFolder);
            //显示第一个视频缩略图，并记录该缩略图在list中的位置的位
           // iv_preview.setImageBitmap(ImageUtils.decodeThumbBitmapForFile(fileList.get(0),200,200));
            /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
            Glide
                    .with( getActivity())
                    .load( Uri.fromFile( new File( fileList.get(0) ) ) )
                    .placeholder(R.drawable.ic_stub)
                    .error(R.drawable.videoerror)
                    .into( iv_preview );
        }
        else if (ss.equals(getString(R.string.get_sound_lists)))//声音
        {
            if (fileList.size()<=0) {
                return;
            }

            Toast.makeText(activity, "声音", Toast.LENGTH_SHORT).show();
            currentFolder = Setup.soundPaths;
            fileList.clear();
            getFilePath(currentFolder);
            iv_preview.setImageResource(R.drawable.myvoice);
        }
        else if (ss.equals(getString(R.string.get_photo_lists)))//图片
        {
            if (fileList.size()<=0){
                return;
            }
            currentFolder = Setup.imagePaths;
            fileList.clear();
            getFilePath(currentFolder);
          //  iv_preview.setImageBitmap(ImageUtils.getImageThumbnail(fileList.get(0),200,200));
            /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
            Glide
                    .with( getActivity())
                    .load( Uri.fromFile( new File( fileList.get(0) ) ) )
                    .placeholder(R.drawable.ic_stub)
                    .error(R.drawable.videoerror)
                    .into( iv_preview );
        }
        else if (ss.equals(getString(R.string.importance)))//标记重要文件
        {
            if (fileList.size()<=0){
                return;
            }

          final   String filePath =  fileList.get(mposition);
            //只有为非重点文件时才可以标记为重点文件
            if (!FileUtils.emphasisFile(filePath))
            {
                //将普通文件标记为重点文件，并其从普通文件夹拷贝到重点文件夹
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        FileUtils.changeFileType(filePath,getActivity());

                    }
                }).start();
            }


        }
        else if (ss.equals(getString(R.string.common)))//一般
        {
            if(fileList.size()<=0) {
                return;
            }
            //只有是重点文件时才可以标记为普通文件
            if (FileUtils.emphasisFile(fileList.get(mposition)))
            {
                //标记为重点文件后，将其从普通文件夹拷贝到重点文件夹
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        FileUtils.changeFileType(fileList.get(mposition),getActivity());
                    }
                }).start();
            }
            Toast.makeText(activity, "一般"+fileList.size(), Toast.LENGTH_SHORT).show();

        }

    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {}


    //递归获得每个文件的绝对路径
    private void getFilePath(String dirPath)
    {
        File dir = new File(dirPath);
        File[] files = dir.listFiles();
        if(files == null)
        {
            return;
        }
        for(int i = 0; i < files.length; i++)
        {
            if(files[i].isDirectory())
            {
                getFilePath(files[i].getAbsolutePath());
            }
            else
            {
                fileList.add(files[i].getAbsolutePath());
            }
        }
    }



    @Override
    public void onClick(View v)
    {
        if (fileList.size()<=0)
        {
            return;
        }
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        switch (v.getId())
        {
            case R.id.tv_front: {
                --mposition;
                currentFilePath = fileList.get(judge());
                break;
            }
            case R.id.tv_next: {
                ++mposition;
                currentFilePath = fileList.get(judge());
                break;
            }
            default:break;
         }
        //显示选择的图片
        if (currentFolder.equals(Setup.imagePaths))
        {
            iv_preview.setImageBitmap(ImageUtils.
                    getImageThumbnail(currentFilePath,200,200));
        }
        else if (currentFolder.equals(Setup.soundPaths))
        {
            iv_preview.setImageResource(R.drawable.myvoice);
        }
        else if (currentFolder.equals(Setup.videoPaths))
        {
            iv_preview.setImageBitmap(ImageUtils.
                    decodeThumbBitmapForFile(currentFilePath,200,200));
        }

    }

    /**
     * 判断当前文件在fliestList中的位置
     */
    private  int   judge()
    {
         if ((mposition >0) && (mposition >= fileList.size()))
         {
             mposition = fileList.size() -1;
         }
         if (mposition <= 0)
         {
             mposition = 0;
         }
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return mposition;
    }
    private void  initData()
    {
        getFilePath(Setup.videoPaths);
    }
}
