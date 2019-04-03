package gbpe.policeass.fragments;


import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.LinearLayout;
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
import gbpe.policeass.utils.thumbnail.DensityUtil;
import gbpe.policeass.utils.thumbnail.ImageLoader;


/**
 * Created by Administrator on 2016/9/6.
 */
public class GetPicturesListsFragment1 extends BaseFragment implements View.OnClickListener {

    private RecyclerAdapter<String> TextAdapter;
    private ImageLoader imageLoader;
    private List<String> datas;
    private File videoFile;
    private File [] pictures;
    private RelativeLayout rl_Detial;
    private View view;
    private Button btnCancel, btnSend, btnSelectAll, btnClear;
    private RelativeLayout headerLayout;
    private TextView numText;
    private ArrayList<HashMap<String, Object>> items; //每一个item
    private ArrayList<Boolean> selectItems; //用于存储已选中项目的位置
    // private boolean isBack;
    private boolean isState;
    private RecyclerView rv_list;
    private ArrayList<String> beSelectedItems;
    private ImageView ivDetial;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_getpictureslists_image,container,false);
        initDatas();
        initViews();
        return  view;
    }



    private void initDatas() {
        datas = new ArrayList<String>();
        videoFile = new File(Setup.imagePaths);
        if ( !videoFile.exists())
        {
            Toast.makeText(activity, "还没有图片", Toast.LENGTH_SHORT).show();
        }
        pictures = videoFile.listFiles();
        if (pictures != null) {
            for (int i = 0; i < pictures.length; i++) {
                if (pictures[i].getName().endsWith(Setup.imgFilePostfix))
                {
                    datas.add(Setup.imagePaths + pictures[i].getName());
                }
            }
        }
    }
    private void initViews(){
         rl_Detial = (RelativeLayout)view.findViewById(R.id.rl_detial);
         rv_list = findView(R.id.rv_list);
        numText = (TextView)view. findViewById(R.id.number);
        btnCancel = (Button) view.findViewById(R.id.btn_cancel);
        btnSend = (Button)view.findViewById(R.id.btn_send2server);
        btnSelectAll = (Button)view. findViewById(R.id.btn_select_all);
        btnClear = (Button)view. findViewById(R.id.btn_clear);
        btnCancel.setOnClickListener(this);
        btnSend.setOnClickListener(this);
        btnSelectAll.setOnClickListener(this);
        btnClear.setOnClickListener(this);
        ivDetial = (ImageView)findView(R.id.iv_detial);
        headerLayout = (RelativeLayout) view.findViewById(R.id.header);
        beSelectedItems = new ArrayList<>();
        items = new ArrayList<>();

        selectItems = new ArrayList<>();
        for (int i = 0; i < datas.size(); i++) {
            HashMap<String, Object> item = new HashMap<>();
            item.put("image", datas.get(i));
            items.add(item);
        }

        rl_Detial.setOnClickListener(this);

    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        activity.mApp.isBack = true;
    }

    @Override
    public void onResume() {
        super.onResume();
        imageLoader = ImageLoader.getInstance(mContext);

        rv_list.setLayoutManager(new GridLayoutManager(mContext,3));

        /**
         * item changes and structural changes. Item changes are when a single item has its data updated
         * but no positionalchanges have occurred. Structural changes are when items are inserted, removed
         * or movedwithin the data set.
         * //notifyDataSetChanged会创新填充适配器内容
         */

        TextAdapter = new RecyclerAdapter<String>(mContext, datas, R.layout.recycler_item) {
            @Override
            public void convert(final RecycleHolder holder, String data, int position) {

                ArrayList<Boolean> list =  getSelectItems();
                //  holder.setText(R.id.tv, data);
                holder.setImageResource(R.id.image, R.drawable.background);//默认背景图片

                final String path = datas.get(position);
                holder.findView(R.id.image).setTag(path);
                imageLoader.loadImage((ImageView) holder.findView(R.id.image),
                        path,
                        DensityUtil.dip2px(80),
                        DensityUtil.dip2px(80),
                        new ImageLoader.onBitmapLoadedListener() {
                            @Override
                            public void displayImage(ImageView view, Bitmap bitmap) {
                                String imagePath= view.getTag().toString();
                                if(imagePath.equals(path)){
                                    view.setImageBitmap(bitmap);
                                }
                            }
                        });

                //根据isState判断所有的item上的CheckBox是否被选中(只要又一次长按，就会让所有的item上的CheckBox处于可见转态)
                holder.findView(R.id.check_box) .setVisibility(isState? View.VISIBLE : View.GONE);
                CheckBox checkBox = (CheckBox)holder.findView(R.id.check_box);
                LinearLayout ll = (LinearLayout)holder.findView(R.id.ll);
                if (list.size() != 0) {
                    checkBox.setChecked(list.get(position));

                }
            }
        };

        TextAdapter.notifyDataSetChanged();
        rv_list.setAdapter(TextAdapter);

        TextAdapter.setOnItemClickListener(new RecyclerAdapter.OnItemClickListener() {
            @Override
            public void OnItemClickListener(View view, int position) {
                // ToastShow("点击" + position);
                activity.mApp.isBack = false;
                if (isState){//如果为true则让所有的item上的CheckBox显示出来
                    // beSelectedItems.add(datas.get(position));
                    CheckBox checkBox = (CheckBox) view.findViewById(R.id.check_box);
                    if (checkBox.isChecked()) {//如果已经是选中状态
                        checkBox.setChecked(false);
                        selectItems.set(position, false);
                    } else { //没有被选中状态
                        checkBox.setChecked(true);
                        selectItems.set(position, true);
                    }
                    //任何一个条目改变时都需要刷新适配器，这样才会显示
                    TextAdapter.notifyDataSetChanged();
                    setSelectNum();
                }

            }
        });


        /**
         * 长按item isState置为true(isState控制点击item和是否显示所有item上的CheckBox的作用)，
         * selectItems全部初始化为false,让当前的item对应的checkbox处于选中状态
         * ，让titlebar显示出来，修改显示被选中的条目的个数（刷新适配器）--->isState
         * 为true的时候点击item让对应的item上的CheckBox改变转态，如果，当前的CheckBox
         * 的状态为true是则让CheckBox处于被取消选中，否则选中，刷新适配器
         *
         * 长按选中的逻辑是，先长按，选中当前条目，然后让titlebar显示出来，
         * 然后让isState 置为true,isState为true的时候，点击item才会生效，即点击选中
         */
        TextAdapter.setOnItemLongClickListener(new RecyclerAdapter.OnItemLongClickListener() {
            @Override
            public void OnItemLongClickListener(View view, int position) {

                activity.mApp.isBack = false;
                Toast.makeText(mContext, "长安点击"+position, Toast.LENGTH_SHORT).show();
                //  beSelectedItems.add(datas.get(position));
                TextAdapter.setIsState(true );
                if (!isState) {
                    selectItems = new ArrayList<>();
                    for (int i = 0; i < items.size(); i++) {//首先将记录选中item的容器selectItems全部初始化为false，
                        // 当某一个item倍选中时，就将改item的状态变为true记录在给item中
                        selectItems.add(false);            //让其的长度和items的长度保持一致。
                    }
                    CheckBox box = (CheckBox) view.findViewById(R.id.check_box);
                    box.setChecked(true);
                    selectItems.set(position, true);
                    setState(true);//显示titlebar，改变isState的值
                    TextAdapter.setIsState(true);
                    setSelectNum();
                }
            }
        });
    }

    public ArrayList<Boolean> getSelectItems() {return selectItems;}


    //设置当前状态 是否在多选模式（选择多张图片上传）
    private void setState(boolean b) {
        isState = b;
        if (b) {//显示标题栏
            headerLayout.setVisibility(View.VISIBLE);
            btnSend.setVisibility(View.VISIBLE);

        } else {//隐藏标题栏
            headerLayout.setVisibility(View.GONE);
            btnSend.setVisibility(View.GONE);

        }
    }

    //显示已选项数目（Testview中显示）
    private void setSelectNum() {//标题栏上的textview动态记录被选中的item的数目。
        int num = 0;
        for (Boolean b : selectItems) {
            if (b)
                num ++;
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
        //全选的时候全选按钮消失，清除按钮出现，反之则亦然
        btnSelectAll.setVisibility(b? View.GONE : View.VISIBLE);
        btnClear.setVisibility(b ? View.VISIBLE : View.GONE);
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
                    for (int i=0;i<selectItems.size();i++){
                        if (selectItems.get(i)){
                            String videoPath = datas.get(i);
                            beSelectedItems.add(videoPath);
                        }
                    }
                     //new TransferClient1(beSelectedItems).service();
//                    for (int i=0;i<datas.size();i++){//判断selectItems里面的哪个值是true，如果为true则从items中取出被选中的照片的路径上传至服务器。
//                        if (selectItems.get(i)){
//
//                            Toast.makeText(activity, "正在上传..."+"第"+i+"项", Toast.LENGTH_SHORT).show();
//                        }
//                    }
                }
            });
            //builder.setNegativeButton("取消", null);
            builder.setNegativeButton(getActivity().getResources().getString(R.string.cancel),null);
            builder.create().show();
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    //捕获返回键事件，（比如，按返回键时缩小图片）
    @Override
    protected boolean onBackPressed() {//在这里写一些处理按返回键需要处理的逻辑。（比如：缩小图片等）
        if (!activity.mApp.isBack){
            activity.mApp.isBack = !activity.mApp.isBack;

            selectItems.clear();
            numText.setText("已选择1项");
            TextAdapter.setIsState(false);
            setState(false);

            rl_Detial.setVisibility(View.INVISIBLE);
            rv_list.setVisibility(View.VISIBLE);

            rv_list.setVisibility(View.VISIBLE);
            return true;//自己处理按返回键时的操作。
        }

        else {return false;}
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {
            case R.id.btn_cancel: // 不选中

                beSelectedItems.clear();
                selectItems.clear();
                numText.setText("已选择1项");
                TextAdapter.setIsState(false);
                setState(false);
                break;
            case R.id.btn_clear://清除所有选中的项
                setSelectAll(false);
                beSelectedItems.clear();
                break;
            case R.id.btn_select_all://全部选中
                setSelectAll(true);
                break;
            case R.id.btn_send2server:
                upLoad2Server();//上传至服务器
                break;
        }
    }

    public <T extends View> T findView(int ViewId) {
        return (T) view.findViewById(ViewId);
    }

}
