package gbpe.policeass.fragments;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ContentResolver;
import android.content.DialogInterface;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.RecycleHolder;
import gbpe.policeass.adapters.RecyclerAdapter;
import gbpe.policeass.utils.SDCardUtils;
import gbpe.policeass.utils.StringUtil;
import gbpe.policeass.utils.thumbnail.ImageUtils;


/**
 * Created by Administrator on 2016/10/10.
 */
public class GetVideoListsFragment2 extends BaseFragment implements View.OnClickListener  {
    private RecyclerAdapter<Bitmap> TextAdapter;
    private View view;
    private Button btnCancel, btnSend, btnSelectAll, btnClear;
    private RelativeLayout headerLayout;
    private TextView numText;
    private ArrayList<HashMap<String, Object>> items; //每一个item
    private ArrayList<Boolean> selectItems; //用于存储已选中项目的位置
    // private boolean isBack;
    private boolean isState;
    private RecyclerView rvlist;
    private ArrayList<String> beSelectedItems;
    //private View mview;
    private List<String> videoNames;
    private File videoFile;
    private File[] videos;
    private List<Bitmap> mlist;
    private String TAG = "TAG";
     private RelativeLayout rl_layout;
     private  ProgressDialog dialog1;
    private ImageView imageView;
    private Cursor cursor;



    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_get_video_list, container, false);

        return view;
    }

    public void initDatas() {
        mlist = new ArrayList<>();
        videoNames = new ArrayList<>();
        videoFile = new File(Setup.videoPaths);
        if (!videoFile.exists())
        {
            Toast.makeText(activity, "还没有视频文件", Toast.LENGTH_SHORT).show();
        }
        videos = videoFile.listFiles();
        Setup.beSelecedItems = videos.length;
        if (videos != null) {
            for (int i = 0; i < videos.length; i++) {
                if (videos[i].getName().endsWith(Setup.videoFilePostfix))
                {
                    videoNames.add(videos[i].getName());
                }

              //  mlist.add(null);
            }

        }
       initData();
    }

    public void initData() {
    MyAsyncTask asyncTask = new MyAsyncTask();
        asyncTask.execute();
    }

    //初始化
    private void initViews() {

        rl_layout = findView(R.id.rl_getvideo);
        rvlist = (RecyclerView) view.findViewById(R.id.rv_list);
        numText = (TextView) view.findViewById(R.id.number);
        btnCancel = (Button) view.findViewById(R.id.btn_cancel);
        btnSend = (Button) view.findViewById(R.id.btn_send2server);
        btnSelectAll = (Button) view.findViewById(R.id.btn_select_all);
        btnClear = (Button) view.findViewById(R.id.btn_clear);
        imageView = (ImageView)findView(R.id.image_back);
        imageView.setOnClickListener(this);
        btnCancel.setOnClickListener(this);
        btnSend.setOnClickListener(this);
        btnSelectAll.setOnClickListener(this);
        btnClear.setOnClickListener(this);

        headerLayout = (RelativeLayout) view.findViewById(R.id.header);
        beSelectedItems = new ArrayList<>();
        items = new ArrayList<>();
        selectItems = new ArrayList<>();
        for (int i = 0; i < videoNames.size(); i++) {
            HashMap<String, Object> item = new HashMap<>();
            item.put("image", videoNames.get(i));
            items.add(item);
        }


    }

    //设置当前状态 是否在多选模式（选择多张图片上传）
    private void setState(boolean b)
    {
        isState = b;
        if (b)
        {//显示标题栏
            headerLayout.setVisibility(View.VISIBLE);
            btnSend.setVisibility(View.VISIBLE);
        } else
        {//隐藏标题栏
            headerLayout.setVisibility(View.GONE);
            btnSend.setVisibility(View.GONE);
        }
    }

    //显示已选项数目（Testview中显示）
    private void setSelectNum() {//标题栏上的textview动态记录被选中的item的数目。
        int num = 0;
        for (Boolean b : selectItems) {
            if (b)
                num++;
        }
        numText.setText("已选择" + num + "项");
    }

    //全选
    private void setSelectAll(boolean b) {
        for (int i = 0; i < selectItems.size(); i++) {
            selectItems.set(i, b);
            TextAdapter.notifyDataSetChanged();
            setSelectNum();
        }
        btnSelectAll.setVisibility(b ? View.GONE : View.VISIBLE);
        btnClear.setVisibility(b ? View.VISIBLE : View.GONE);
    }

    public ArrayList<Boolean> getSelectItems() {
        return selectItems;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    protected boolean onBackPressed() {
        if (beSelectedItems != null){
            beSelectedItems.clear();
        }
        return false;
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "GetVideoListsFragment2");
        initDatas();
        initViews();
     //   init();
        rvlist.setLayoutManager(new LinearLayoutManager(mContext));
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
//       if (activity.sView != null) {
//           activity.sView.setVisibility(View.VISIBLE);
//       }
        if (beSelectedItems != null){
            beSelectedItems.clear();
        }
      //  Setup.currentProgress =0;

     }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_cancel:{// 取消上传服务器的操作
                beSelectedItems.clear();
                selectItems.clear();
                numText.setText("已选择1项");
                TextAdapter.setIsState(false);
                setState(false);
        //        Setup.currentProgress = 0;
                break;
            }
            case R.id.btn_clear:{//清除所有选中的项
                setSelectAll(false);
                if (beSelectedItems != null)
                {
                    beSelectedItems.clear();
                }
                break;
            }
            case R.id.btn_select_all:{//全部选中
                setSelectAll(true);
                break;
            }
            case R.id.btn_send2server:{//上传至服务器
                upLoad2Server();
                break;
            }
            case R.id.image_back:{
                popSelf();
                break;
            }
            default:break;
        }
    }

    //上传服务器
    private void upLoad2Server() {
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        if (!selectItems.contains(true)) {
            builder.setTitle("提示").setMessage("当前未选中项目").setPositiveButton("确认", null).create().show();
        } else {
            builder.setTitle("提示");
            builder.setMessage("确认上传所选项目？");
            builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    long temp = 0;
                    for (int i = 0; i < selectItems.size(); i++) {
                        if (selectItems.get(i)) {
                            String videoPath = Setup.videoPaths + videoNames.get(i);
                            beSelectedItems.add(videoPath);
                             temp += StringUtil.string2Double(SDCardUtils.getFileNameWithoutExtension(StringUtil.byteToMB(new File(videoPath).length())));

                        }
                    }


                    dialog1 = new ProgressDialog(activity);
                    dialog1.setTitle("上传服务器...");
                    dialog1.setMax((int) temp);
                    dialog1.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                    dialog1.show();
                }
            });
            //builder.setNegativeButton("取消", null);
            builder.setNegativeButton(getActivity().getResources().getString(R.string.cancel),null);
            builder.create().show();
        }
    }
    //字节转换成MB

    public <T extends View> T findView(int ViewId) {return (T) view.findViewById(ViewId);}

    public class MyAsyncTask extends AsyncTask<Bitmap, Bitmap, String> {
        @Override
        protected void onPreExecute() {//excute()方法执行完后被调用
            super.onPreExecute();
        }

        @Override
        protected String doInBackground(Bitmap... params) {
            try {
                Thread.sleep(150);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            for (int i = 0; i < videoNames.size(); i++) {
                String videopath = Setup.videoPaths + videoNames.get(i);

                ContentResolver resolver = mBaseActivity.getContentResolver();//   获得ContentResolver 实例必须在当前activity所占有的资源已准备好的
                // 时候否则将返回空指针（在onResume()方法中调用）
                Bitmap bitmap = ImageUtils.getVideoThumbnail(mContext, resolver, videopath);
               // mlist.set(i,bitmap);
                publishProgress(bitmap);
            }

            return "线程执行完";
        }

        @Override
        protected void onProgressUpdate(Bitmap... values) {
            super.onProgressUpdate(values);
           mlist.add(values[0]);
            TextAdapter = new RecyclerAdapter<Bitmap>(mContext, mlist, R.layout.item_recycler_video) {
                @Override
                public void convert(final RecycleHolder holder, Bitmap data, final int position) {
                    ArrayList<Boolean> list = getSelectItems();
                    final ImageView imageView = holder.findView(R.id.image);
                    imageView.setImageBitmap(mlist.get(position));
                    holder.findView(R.id.check_box).setVisibility(isState ? View.VISIBLE : View.GONE);
                    CheckBox checkBox = (CheckBox) holder.findView(R.id.check_box);
                    if (list.size() != 0) {
                        checkBox.setChecked(list.get(position));
                    }

            }
        };
        TextAdapter.notifyDataSetChanged();
        rvlist.setAdapter(TextAdapter);
        TextAdapter.setOnItemClickListener(new RecyclerAdapter.OnItemClickListener() {
            @Override
            public void OnItemClickListener(View view, int position) {
                // ToastShow("点击" + position);
                activity.mApp.isBack = false;
                if (isState) {
                    // String videoPath = sdCardPath+"/MyFinalVideo/"+videoNames.get(position);
                    // beSelectedItems.add(videoPath);
                    CheckBox checkBox = (CheckBox) view.findViewById(R.id.check_box);
                    if (checkBox.isChecked()) {
                        checkBox.setChecked(false);
                        selectItems.set(position, false);
                    } else {
                        checkBox.setChecked(true);
                        selectItems.set(position, true);
                    }
                    TextAdapter.notifyDataSetChanged();
                    setSelectNum();
                } else {
                    Bundle bundle = new Bundle();
                    bundle.putString("path", videoNames.get(position));
                  //  activity.showFragment2(GetVideoListsFragment2.this, PlayVideoFragment.class, "PlayVideoFragment", bundle, true);
//                    if (activity.surfaceView != null) {
//                        activity.surfaceView.setVisibility(View.INVISIBLE);
//                    }
                }

            }
        });

        TextAdapter.setOnItemLongClickListener(new RecyclerAdapter.OnItemLongClickListener() {
            @Override
            public void OnItemLongClickListener(View view, int position) {

                activity.mApp.isBack = false;
                Toast.makeText(mContext, "长安点击" + position, Toast.LENGTH_SHORT).show();

                TextAdapter.setIsState(true);
                if (!isState) {
                    selectItems = new ArrayList<>();
                    for (int i = 0; i < videoNames.size(); i++) {//首先将记录选中item的容器selectItems全部初始化为false，
                                                                   // 当某一个item被选中时，就将改item的状态变为true记录在给item中
                        selectItems.add(false);//让其的长度和items的长度保持一致。

                    }
                    CheckBox box = (CheckBox) view.findViewById(R.id.check_box);
                    box.setChecked(true);
                    selectItems.set(position, true);
                    setState(true);
                    TextAdapter.setIsState(true);
                    setSelectNum();
                }
            }
        });

    }

        @Override
        protected void onPostExecute(String s) {
            super.onPostExecute(s);
        }
    }



}


