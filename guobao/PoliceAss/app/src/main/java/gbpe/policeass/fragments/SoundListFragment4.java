package gbpe.policeass.fragments;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
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
import com.bumptech.glide.Glide;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.RecycleHolder;
import gbpe.policeass.adapters.RecyclerAdapter;
import gbpe.policeass.uploadfile.TransferClient1;
import gbpe.policeass.utils.AnimUtils;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.FileSizeUtil;
import gbpe.policeass.utils.SelectHandleUtil;
import gbpe.policeass.views.CommolySearchView;
import gbpe.policeass.views.HandleDialog;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/1 09:19
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/1 09:19
 * <p/>
 * 描述：
 * =====================================================================================
 */

public class SoundListFragment4 extends BaseFragment implements View.OnClickListener{
    private RecyclerView rv_video;
    private CommolySearchView<String> searchView;
    private RecyclerAdapter<String> adapter;
    private ArrayList<String> soundPaths ;
    private LinearLayout ll_botton_menue;
    private View view;
    private Button btnCancel, btnSelectAll, btnClear,btn_search_cancle;
    private RelativeLayout headerLayout;
    private TextView numText;
    private ArrayList<Boolean> selectItems; //用于存储已选中项目的位置
    private boolean isState;
    private ArrayList<String> beSelectedItems;
    private boolean isSearch;
    //private PlaySoundDialog playSoundDialog;
    private MediaPlayer player;
    private static final String TAG = "SoundListFragment4";

    private int id;
    private ImageView iv_search_icon;
    TextView tv_sc,tv_mark;
    @Override
    protected boolean onBackPressed() {
        return false;
    }
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_videolist2,null);

        player = new MediaPlayer();

        initView();
        initDatas();
        return view;
    }

    private void initView(){
        rv_video = (RecyclerView)view.findViewById(R.id.rv_video);
        searchView = (CommolySearchView) view.findViewById(R.id.csv_show);
        rv_video.setLayoutManager(new LinearLayoutManager(mContext));
        ll_botton_menue = (LinearLayout) view.findViewById(R.id.ll_botton_menue);
        headerLayout = (RelativeLayout)view.findViewById(R.id.header);

        btnCancel = (Button)view.findViewById(R.id.btn_cancel);
        btnCancel.setOnClickListener(this);
        btnClear = (Button)view.findViewById(R.id.btn_clear);
        btnClear.setOnClickListener(this);
        btnSelectAll =(Button)view.findViewById(R.id.btn_select_all);
        btnSelectAll.setOnClickListener(this);
        btn_search_cancle = (Button)view.findViewById(R.id.btn_search_cancle);
        btn_search_cancle.setOnClickListener(this);

        iv_search_icon = (ImageView) view.findViewById(R.id.iv_search_icon);
        iv_search_icon.setOnClickListener(this);

        numText = (TextView)view.findViewById(R.id.number);

        tv_sc = (TextView)view.findViewById(R.id.tv_sc);
        tv_sc.setOnClickListener(this);
        tv_mark = (TextView)view.findViewById(R.id.tv_mark);
        tv_mark.setOnClickListener(this);
        view.findViewById(R.id.image_back).setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                popSelf();
            }
        });
        beSelectedItems = new ArrayList<>();
        selectItems = new ArrayList<>();
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        //return false;
        switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_LEFT:
				AppLog.i(TAG, "key left");
				break;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
			    AppLog.i(TAG, "key right");
				break;
		}
        return true;
    }

	private String getSoundName(String absPath){
		File f = new File(absPath);
		String name = f.getName();
		return name.substring(0,name.indexOf('.'));
	}

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        adapter = new RecyclerAdapter<String>(mContext, soundPaths, R.layout.item_list_image) {
            @Override
            public void convert(RecycleHolder holder, String data, int position) {

                ArrayList<Boolean> list =  getSelectItems();
                ImageView iv_thumb = holder.findView(R.id.video_thumb);
                TextView tv_title = holder.findView(R.id.tv_title);
                String fileSize = FileSizeUtil.getFileOrFilesSize(data,1);
                TextView tv_size = holder.findView(R.id.tv_size);

                String fileName ="";
                File file = new File(data);
                fileName = file.getName();
                String fileTitle = fileName.substring(0,fileName.indexOf("."));
                tv_title.setText(fileTitle); //获取文件名);

                /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
                Glide
                        .with( getActivity())
                        .load(R.drawable.myvoice )
                        .placeholder(R.drawable.ic_stub)
                        .error(R.drawable.videoerror)
                        .into( iv_thumb );

                tv_size.setText(fileSize);

                //根据isState判断所有的item上的CheckBox是否被选中(只要又一次长按，就会让所有的item上的CheckBox处于可见转态)
                holder.findView(R.id.check_box) .setVisibility(isState? View.VISIBLE : View.GONE);
                CheckBox checkBox = holder.findView(R.id.check_box);
                LinearLayout ll = holder.findView(R.id.ll_item_bg);
                if (list.size() != 0) {
                    checkBox.setChecked(list.get(position));
                    ll.setSelected(list.get(position));
                    ll.setPressed(list.get(position));
                }else {
                    ll.setSelected(false);
                    ll.setPressed(false);
                }
            }
        };
        adapter.notifyDataSetChanged();
        rv_video.setAdapter(adapter);
        //-------------------------------------------搜索功能设置------------------------------------------------------------------------
        searchView.setDatas(soundPaths);
        searchView.setAdapter(adapter);
        searchView.setSearchDataListener(new CommolySearchView.SearchDatas<String>()
        {
            @Override
            public List<String> filterDatas(List<String> datas, List<String> filterdatas, String inputstr)
            {
                for (int i = 0; i < datas.size(); i++)
                {
                    // 筛选条件
                    String fileName = new File(datas.get(i)).getName();
                    String fileTitle = fileName.substring(0,fileName.indexOf("."));
                    String fileSize = FileSizeUtil.getFileOrFilesSize(datas.get(i),1);
                    if ((fileTitle.contains(inputstr))|| fileSize.contains(inputstr))
                    {
                        filterdatas.add(datas.get(i));
                    }
                }
                return filterdatas;
            }
        });
        //功夫瑜伽
//------------------------------------------------------------------------------------------------------------------------------
        //点击
        adapter.setOnItemClickListener(new RecyclerAdapter.OnItemClickListener() {
            @Override
            public void OnItemClickListener(View view, final int position) {

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
                    adapter.notifyDataSetChanged();
                    setSelectNum();
                }else
                {

                    if (soundPaths.size()>0&&soundPaths!=null)
                    {
                        searchView.showOrHideSoftInput(getActivity());
                        //播放声音
                        /*
                        Bundle bundle = new Bundle();
                        bundle.putInt("position",position);
                        activity.showFragment2(SoundListFragment4.this,SoundListFragment3.class,
                                "SoundListFragment3",bundle,true);*/
                        /*TextView time_now;
                        TextView time_total;
                        SeekBar play_progress;
                        TextView sound_name;
                        ImageView play_switch;
                        playSoundDialog = new PlaySoundDialog(getActivity(),R.style.MyDialog);


                        playSoundDialog.setCanceledOnTouchOutside(true);
                        View dlgView = playSoundDialog.getDialogView();
                        time_now = dlgView.findViewById(R.id.time_now);
                        time_total = dlgView.findViewById(R.id.time_total);
                        play_progress = dlgView.findViewById(R.id.play_progress);
                        sound_name = dlgView.findViewById(R.id.sound_name);
                        play_switch = dlgView.findViewById(R.id.play_switch);
                        playSoundDialog.setTitle(getSoundName(soundPaths.get(position)));
                        playSoundDialog.show();*/
                        final PlaySoundDialog playSoundDialog = new PlaySoundDialog(getActivity(), R.style.MyDialog, soundPaths.size(),position);
                        playSoundDialog.setListener(new PlaySoundDialog.PlayComplishListener() {
                            @Override
                            public void onStateChangeHandle(int inc) {
                                //playSoundDialog.dismiss();
                                //AppLog.i (TAG,"position: "+position+", inc: "+inc);
                                // 此处显示文件名不正确
                                playSoundDialog.setTitle(getSoundName(soundPaths.get(inc)));
                                playSoundDialog.play(soundPaths.get(inc));
                            }
/*
                            @Override
                            public void onKeyLeft(int inc) {
                                AppLog.i (TAG,"position: "+position+", inc: "+inc);
                                if (position+inc > 0) {
                                    playSoundDialog.setTitle(getSoundName(soundPaths.get(position + inc)));
                                    playSoundDialog.play(soundPaths.get(position+inc));
                                }
                            }

                            @Override
                            public void onkeyRight(int inc) {
                                AppLog.i (TAG,"position: "+position+", inc: "+inc);
                                if (position+inc < soundPaths.size()) {
                                    playSoundDialog.setTitle(getSoundName(soundPaths.get(position + inc)));
                                    playSoundDialog.play(soundPaths.get(position+inc));
                                }
                            }
                            */
                        });
                        //playSoundDialog.setTitle(getSoundName(soundPaths.get(position)));
                        playSoundDialog.show();
                        playSoundDialog.startPlay(soundPaths.get(position));
                        playSoundDialog.setTitle(getSoundName(soundPaths.get(position)));    // 放在前面显示不正常

                    }
                }

            }
        });

        //长按
        adapter.setOnItemLongClickListener(new RecyclerAdapter.OnItemLongClickListener() {
            @Override
            public void OnItemLongClickListener(View view, int position) {

                searchView.showOrHideSoftInput(getActivity());
                adapter.setIsState(true );
                if (!isState) {

                    for (int i = 0; i < soundPaths.size(); i++) {//首先将记录选中item的容器selectItems全部初始化为false，
                        // 当某一个item倍选中时，就将改item的状态变为true记录在给item中
                        selectItems.add(false);            //让其的长度和items的长度保持一致。
                    }
                    CheckBox box = (CheckBox) view.findViewById(R.id.check_box);
                    box.setChecked(true);
                    selectItems.set(position, true);
                    setState(true);//显示titlebar，改变isState的值
                    adapter.setIsState(true);
                    setSelectNum();
                    isState =true;
                }
            }
        });

    }

    @Override
    public void onResume() {
        super.onResume();

    }

    //======================================================================================================================

    public ArrayList<Boolean> getSelectItems() {return selectItems;}

    //设置当前状态 是否在多选模式
    private void setState(boolean b) {
        isState = b;
        if (b) {
            headerLayout.setVisibility(View.VISIBLE);
            AnimUtils.pushIn(ll_botton_menue,getActivity());


        } else {
            headerLayout.setVisibility(View.GONE);
            AnimUtils.pushOut(ll_botton_menue,getActivity());
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

    //    //全选
    private void setSelectAll(boolean b) {
        for (int i = 0; i < selectItems.size(); i++) {
            selectItems.set(i, b);
            adapter.notifyDataSetChanged();
            setSelectNum();
        }
        //全选的时候全选按钮消失，清除按钮出现，反之则亦然
        btnSelectAll.setVisibility(b? View.GONE : View.VISIBLE);
        btnClear.setVisibility(b ? View.VISIBLE : View.GONE);
    }


    /**
     * 初始化数据
     */
    public void initDatas() {
        soundPaths = new ArrayList<>();
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

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.btn_cancel:
                selectItems.clear();
                numText.setText("已选择1项");
                adapter.setIsState(false);
                setState(false);
                break;
            case R.id.btn_clear:
                setSelectAll(false);
                break;
            case R.id.btn_select_all:
                setSelectAll(true);
                break;
            case R.id.btn_search_cancle:
                searchView.setVisibility(View.GONE);
                btn_search_cancle.setVisibility( View.GONE);
                iv_search_icon.setVisibility( View.VISIBLE );
                boolean state = searchView.isSoftShowing(getActivity());
                if (state){
                    searchView.hideSoftInput();
                }
                break;
            case R.id.iv_search_icon:
                isSearch = !isSearch;
                searchView.setVisibility(isSearch ? View.VISIBLE : View.GONE);
                btn_search_cancle.setVisibility(isSearch ? View.VISIBLE : View.GONE);
                iv_search_icon.setVisibility(!isSearch ? View.VISIBLE : View.GONE);
                break;
            case R.id.tv_sc:
                id = R.id.tv_sc;
                showDialog2(selectItems.size(),"上传至服务器","确定上传？");
                break;
            case R.id.tv_mark:
                id = R.id.tv_mark;
                showDialog2(7,"标记为重点文件","确定全部标记？");
                break;
        }
    }



    /**
     * 进度条最大值
     * @param max
     */
    void  showDialog2(int max, final String title, String desc){

        final HandleDialog dialog = new HandleDialog(getActivity());
        dialog.setTitle(title).setDesc(desc).setProgressBarMax(getSelectedItems().size()).setProgress(0).
                setOnNegativeListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        dialog.setProgress(0);
                        dialog.dismiss();
                        Setup.isCancle =true;
                        Setup.fileCount =0;
                    }
                })
                .setOnPositiveListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Setup.isCancle = false;
                        switch (id){
                            case R.id.tv_sc:
                                dialog.setProgress(0);
                                TransferClient1 transferClient1 = new TransferClient1(  );
                                transferClient1.service(new TransferClient1.MyCallBack() {
                                    @Override
                                    public void onFileStartUpload() {

                                        dialog.setProgress(0);
                                    }
                                    @Override
                                    public void onFileFinishedUpload(int fileCount) {

                                    }
                                    @Override
                                    public void onFileCancleUpload() {

                                    }
                                });

                                break;
                            case R.id.tv_mark:

                                SelectHandleUtil.mark(getActivity(), getSelectedItems(), new SelectHandleUtil.OnFileHandleListener() {
                                    @Override
                                    public void onFileHandleStart() {

                                        dialog.setDesc("正在标记...");
                                    }
                                    @Override
                                    public void onOneFileHandleFinished(int progress) {

                                        dialog.setProgress(progress);
                                    }
                                    @Override
                                    public void onAllFileHandleFinished() {

                                        dialog.dismiss();
                                        initDatas();
                                        searchView.setDatas(soundPaths);
                                        selectItems.clear();
                                        numText.setText("已选择1项");
                                        adapter.setIsState(false);
                                        setState(false);
                                        adapter.addData(soundPaths);
                                        Toast.makeText(getActivity(), "标记完成", Toast.LENGTH_SHORT).show();
                                    }
                                    @Override
                                    public void onFileCancle() {

                                        dialog.setProgress(0);
                                    }
                                });


                                break;
                        }
                    }
                }).show();
    }

    private List<String> getSelectedItems(){

        if (!beSelectedItems.isEmpty()){
            beSelectedItems.clear();
        }
        for (int i=0;i<selectItems.size();i++){
            if (selectItems.get(i)){
                String path = soundPaths.get(i);
                beSelectedItems.add(path);
            }
        }
        return beSelectedItems;
    }
}
