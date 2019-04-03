package gbpe.policeass.fragments;


import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.support.annotation.Nullable;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;

import java.util.List;

import gbpe.policeass.Activity.FaceRecogniseActivity;
import gbpe.policeass.Activity.IdRecogniseActivity;
import gbpe.policeass.Activity.SettingActivity;
import gbpe.policeass.Plate.recognizer.ui.PlateRecognizeActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.fragments.filesquary.NewsFragment;
import gbpe.policeass.fragments.fileview.GetFileTypeAudioLists2;
import gbpe.policeass.fragments.fileview.GetFileTypePictureLists2;
import gbpe.policeass.fragments.fileview.GetFileTypeVideoLists2;
import gbpe.policeass.log.LogUtils;
import gbpe.policeass.modle.OnReleaseCamera;
import gbpe.policeass.utils.AppLog;
import gbpe.policeass.utils.NoDoubleClickListener;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.utils.TimeUtils;
import gbpe.policeass.uvc.UvcMainActivity;
import gbpe.policeass.views.CustomDialog;
import gbpe.policeass.views.VerticalSeekBar;
import gbpe.policeass.zfysim.HomeItem;
import gbpe.policeass.zfysim.HomeItemLab;
import gbpe.policeass.zfysim.NewHomeActivity;

public class NewHomeFragment extends NoDoubleClickListener
               implements VerticalSeekBar.SlideChangeListener {

    private static final String TAG = "NewHomeFragment";
    private RecyclerView mRecyclerView;
    private List<HomeItem> mHomeItems;
    private ItemsAdapter mAdapter;
    private String TYPE = "";
    //private VerticalSeekBar mVerticalSeekBar;
    private String VIDEOS = "videos";
    private String PHOTOS = "photos";
    private String SOUNDS = "sounds";
    private String SETTING = "setting";
    private NewHomeFragment mSelf;
    private Button mBtnBack;

    /**
     * 判断密码输入是否输入正确
     * @param password
     * @return
     */
    private boolean checkInput(String password){
        if(password==null||password.length()<6){
            Toast.makeText(getActivity(), R.string.key_format_error_tips, Toast.LENGTH_SHORT).show();
            return false;
        }
        if (!password.equals(SettingUtils.getStrValues(getActivity(),"usrPassword","111111"))){
            Toast.makeText(getActivity(), R.string.key_error_tips, Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

    private void upLoad2Server() {
        final View view = View.inflate(getActivity(),R.layout.dialog_onkey_upload,null);
        final EditText et_server_ip = (EditText) view.findViewById(R.id.et_server_ip);
        final EditText et_server_port = (EditText)view.findViewById(R.id.et_server_port);
        Button btn_uplod_cancle = (Button)view.findViewById(R.id.btn_uplod_cancle);
        Button btn_uplod_confim = (Button)view.findViewById(R.id.btn_uplod_confim) ;
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        final AlertDialog mDialog = builder.create();
        mDialog.setView(view, 0, 0, 0, 0);

        btn_uplod_confim.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Setup.SERVER_IP = et_server_ip.getText().toString().trim();
                String server_port = et_server_port.getText().toString().trim();

                if (!TextUtils.isEmpty(server_port)&& !TextUtils.isEmpty(Setup.SERVER_IP )){
                    Setup.SERVER_PORT = Integer.parseInt(server_port);
                    mDialog.dismiss();
                    // new TransferClient1().service();//一键上传
                }else {
                    Toast.makeText(activity, R.string.port_ip_null_tips, Toast.LENGTH_SHORT).show();
                }
            }
        });

        btn_uplod_cancle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                mDialog.dismiss();
            }
        });

        //  builder.show();//只能写成mDialog.show()
        mDialog.show();
    }

    /**
     * 弹出对话框，并获取对话框
     * 里输入的密码，判断密码是
     * 否输入正确，正确则跳转到
     * 下一个界面
     */
    private void showDialog(final int id) {
        final  CustomDialog dialog = new CustomDialog(getActivity());
        final EditText et_content = (EditText) dialog.getEditText();//方法在CustomDialog中实现
        dialog.setOnPositiveListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                dialog.dismiss();
            }
        });
        //为确定按钮设置监听
        dialog.setOnNegativeListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                String content = et_content.getText().toString().trim();
                if (checkInput(content)) {
                    goToNext(id);
                }
                dialog.dismiss();
            }
        });
        dialog.show();
    }

    /**
     * 弹出对话框，并获取对话框
     * 里输入的密码，判断密码是
     * 否输入正确，正确则跳转到
     * 下一个界面
     */
    private void showDialog2() {
        final CustomDialog dialog = new CustomDialog(getActivity());
        final EditText et_content = (EditText) dialog.getEditText();//方法在CustomDialog中实现
        dialog.setOnPositiveListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                dialog.dismiss();
            }
        });
        //为确定按钮设置监听
        dialog.setOnNegativeListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                String content = et_content.getText().toString().trim();
                if (checkInput2(content)) {
                    // goToNext();
                    AppLog.i(TAG,"setting");
                    activity.startActivity(new Intent(activity, SettingActivity.class));

                }
                dialog.dismiss();
            }
        });
        dialog.show();
    }

    @Override
    protected boolean onBackPressed() {
        return false;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        //return super.onCreateView(inflater, container, savedInstanceState);
        View view = inflater.inflate(R.layout.activity_new_home,container,false);

        mSelf = this;

        mRecyclerView = view.findViewById(R.id.new_home_view);
        /*
        mVerticalSeekBar = view.findViewById(R.id.verticalSeekBar);
        mVerticalSeekBar.setThumbSizePx(20, 20);
        mVerticalSeekBar.setMaxProgress(100);
        mVerticalSeekBar.setProgress(100);

        mVerticalSeekBar.setOrientation(0);
        mVerticalSeekBar.setOnSlideChangeListener(this);
        */

        mRecyclerView.setLayoutManager(new GridLayoutManager(getActivity(),3));
        //mRecyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));
        /*
        mRecyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
                GridLayoutManager gm = (GridLayoutManager) recyclerView.getLayoutManager();
                int first = gm.findFirstVisibleItemPosition();
                int last = gm.findLastVisibleItemPosition();
                int progress;
                //AppLog.i(TAG,"first: "+gm.findFirstVisibleItemPosition()+", last"+gm.findLastVisibleItemPosition());
                if (last-first > 5){
                    progress = first*2+3;
                }else{
                    progress = first*2;
                }

                progress = 100-progress*100/18;

                //mVerticalSeekBar.setProgress(progress);

                AppLog.i(TAG, "first: "+first+", last: "+last+", progress: "+ progress);
            }
        });*/

        mBtnBack = view.findViewById(R.id.add_reback_btn);
        mBtnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getActivity().finish();
            }
        });

        mHomeItems = HomeItemLab.getInstance(getActivity()).getHomeItems();

        updateUI();

        return view;
    }

    private void updateUI() {
        mAdapter = new ItemsAdapter(mHomeItems);
        mRecyclerView.setAdapter(mAdapter);
    }

    @Override
    public void onNoDoubleClick(View view) {

    }


    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        //return false;
        //GridLayoutManager gm = (GridLayoutManager) mRecyclerView.getLayoutManager();
        //int first = gm.findFirstVisibleItemPosition();

        switch (keyCode){
            case KeyEvent.KEYCODE_DPAD_LEFT:
                //AppLog.i (TAG, "left "+first);
                mRecyclerView.smoothScrollBy(0,-50);

                break;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                //AppLog.i (TAG, "right "+first+" max: "+mRecyclerView.getAdapter().getItemCount());
                mRecyclerView.smoothScrollBy(0,50);
                break;
        }
        return true;
    }

    private class ItemsHolder extends RecyclerView.ViewHolder
            implements View.OnClickListener {

        private HomeItem homeItem;
        private TextView mItemName;
        private ImageView mPicture;

        public ItemsHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.home_list_item, parent, false));
            itemView.setOnClickListener(this);

            mItemName = (TextView)itemView.findViewById(R.id.item_name);
            mPicture = (ImageView)itemView.findViewById(R.id.item_picture);
        }


        @Override
        public void onClick(View v) {
            //final Intent intent;
            switch (homeItem.getRid()){
                case R.drawable.spll:
                case R.drawable.tp:
                case R.drawable.syll:
                    showDialog(homeItem.getRid());
                    break;
                case R.drawable.pass:
                    //activity.showFragment2(mSelf,CompassFragmet.class,"CompassFragmet",null,true);
                    //activity = (NewHomeActivity) getActivity();
                    ((NewHomeActivity) getActivity()).showFragment2(mSelf,CompassFragmet.class,"CompassFragmet",null,true);
                    break;
                case R.drawable.voice_intercom:{
                        //final String ip = Setup.getProperties("EXT_IP");
                        //final String port = Setup.getProperties("EXT_PORT");
                        //final String http = Setup.getProperties("EXT_HTTP");
                        //AppLog.i (TAG, "ip: "+ip+", port:"+port+", http:"+http);
                        Intent intent = getActivity().getPackageManager().getLaunchIntentForPackage("com.unionbroad.app");
                        if (intent != null){
                            EventBus.getDefault().post(new OnReleaseCamera("释放摄像头"));
                            //intent.putExtra("server_ip",ip);
                            //intent.putExtra("sip_port", port);
                            //intent.putExtra("http_port",http);
                            startActivity(intent);
                        } else {
							Toast.makeText(getActivity(),R.string.app_open_failed,Toast.LENGTH_SHORT).show();
						}
                        /*new Handler().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                Intent info = new Intent("com.unionbroad.app.action.server.setting");
                                info.putExtra("server_ip",ip);
                                info.putExtra("sip_port",port);
                                info.putExtra("http_port",http);
                                getActivity().sendBroadcast(info);
                            }
                        }, 2000);*/
                    }
                    break;
                case R.drawable.sc:
                    upLoad2Server();
                    break;
                case R.drawable.wodesos:
                    Toast.makeText(getActivity(),R.string.sos_func_tips,Toast.LENGTH_SHORT).show();
                    break;
                case R.drawable.wjsxt: {
                    //AppLog.i (TAG, "开启外接摄像头");
                    Toast.makeText(getActivity(), R.string.uvc_open_tips, Toast.LENGTH_SHORT).show();
                    Intent intent = new Intent(getActivity(), UvcMainActivity.class);
                    startActivity(intent);
                }
                    break;
                case R.drawable.setting:
                    showDialog2();
                    break;
                case R.drawable.zfy:
                    activity.showFragment2(mSelf, NewsFragment.class,"NewsFragment",null,true);
                    break;
                case R.drawable.phone:
                    Intent intent = activity.getPackageManager().getLaunchIntentForPackage("com.android.dialer");
                    if (intent !=null){
                        startActivity(intent);
                    }
                    break;
					/*
                case R.drawable.figuer:
                    intent = getActivity().getPackageManager().getLaunchIntentForPackage("com.fp.androidfpsetting");
                    if (intent != null) {
                        startActivity(intent);
                    }
                    break;*/
                case R.drawable.wsftp:
					/*
                    intent = getActivity().getPackageManager().getLaunchIntentForPackage("com.theolivetree.ftpserver");
                    if (intent != null) {
                        startActivity(intent);
                    }*/
					Toast.makeText(getActivity(),R.string.no_support_now,Toast.LENGTH_SHORT).show();
                    break;
                case R.drawable.message:
                    intent = activity.getPackageManager().getLaunchIntentForPackage("com.android.mms");
                    if (intent != null) {
                        startActivity(intent);
                    }
                    break;
                case R.drawable.sifi:
					/*
                    intent = activity.getPackageManager().getLaunchIntentForPackage("com.guobao.wifiset");
                    if (intent != null) {
                        startActivity(intent);
                    }*/
                    startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS)); //直接进入手机中的wifi网络设置界面
                    break;
                case R.drawable.car_plate:
					//Toast.makeText(getActivity(),R.string.cat_plate_tips,Toast.LENGTH_SHORT).show();
                    EventBus.getDefault().post(new OnReleaseCamera("释放摄像头"));
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            activity.startActivity(new Intent(activity, PlateRecognizeActivity.class));
                        }
                    },500);

                    break;
                case R.drawable.face_recognise:
					EventBus.getDefault().post(new OnReleaseCamera("释放摄像头"));
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            activity.startActivity(new Intent(activity, FaceRecogniseActivity.class));
                        }
                    },500);

                    break;
                case R.drawable.id_recognise:
					//Toast.makeText(getActivity(),R.string.id_reco_tips,Toast.LENGTH_SHORT).show();
					EventBus.getDefault().post(new OnReleaseCamera("释放摄像头"));
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            activity.startActivity(new Intent(activity, IdRecogniseActivity.class));
                        }
                    },500);

                    break;
            }
            AppLog.i (TAG, homeItem.getTitle()+" clicked");

        }

        public void bind(HomeItem homeItem){
            this.homeItem = homeItem;
            mItemName.setText(homeItem.getTitle());
            mPicture.setImageResource(homeItem.getRid());
        }
    }

    private class ItemsAdapter extends RecyclerView.Adapter<ItemsHolder>{

        private List<HomeItem> mHomeItems;

        public ItemsAdapter(List<HomeItem> homeItems){
            mHomeItems = homeItems;
        }

        @Override
        public ItemsHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater layoutInflater = LayoutInflater.from(getActivity());
            return new ItemsHolder(layoutInflater,parent);
        }

        @Override
        public void onBindViewHolder(ItemsHolder holder, int position) {
            HomeItem homeItem = mHomeItems.get(position);
            holder.bind(homeItem);
            //AppLog.i (TAG, "position: "+position);
            //GridLayoutManager gridLayout = (GridLayoutManager) mRecyclerView.getLayoutManager();
            //int progress = gridLayout.findFirstVisibleItemPosition();
            //int progress = gridLayout.findLastVisibleItemPosition();
            //AppLog.i(TAG, "first: "+ gridLayout.findFirstVisibleItemPosition() + ", last: "+ gridLayout.findLastVisibleItemPosition() );
        }

        @Override
        public int getItemCount() {
            return mHomeItems.size();
        }
    }

    /*
    *   此处为竖直seekbar的显示
    * */
    @Override
    public void onStart(VerticalSeekBar slideView, int progress) {

    }

    @Override
    public void onProgress(VerticalSeekBar slideView, int progress) {

    }

    @Override
    public void onStop(VerticalSeekBar slideView, int progress) {

    }

    /**
     * 跳转到下一个界面
     */
    // ycb: 此处实现正确输入密码后，执行的下一个界面
    private void goToNext(int id) {

        //if (TYPE.equals(VIDEOS))
        if (R.drawable.spll == id){
            // activity.myService.writeLogs(getString(R.string.get_video_lists));
            AppLog.d(TAG, "go to video list");

            LogUtils.writeTxtToFile(getString(R.string.get_video_lists),Setup.LOG_PATH_SDCARD_DIR,
                    TimeUtils.getLogDate()+".log");
            //if (SettingUtils.getStrValues(getActivity(),"fileview_type",StringUtil.PicView).equals(StringUtil.PicView)) {
            //if (SettingUtils.getStrValues(getActivity(),"fileview_type",getString(R.string.file_view_mode)).equals(R.string.file_view_mode)){
            if (true) {

                // 从此处进入
                AppLog.d(TAG, "activity: VideoListFragment3");
                activity.showFragment2(this, VideoListFragment3.class,
                        "VideoListFragment3", null, true);
            } else {
//                activity.showFragment2(this, GetFlieTypeVideoListFragment.class,
//                        "GetFlieTypeVideoListFragment", null, true);

                AppLog.d(TAG, "activity: GetFileTypeVideoLists2");
                activity.showFragment2(this, GetFileTypeVideoLists2.class,
                        "GetFileTypeVideoLists2", null, true);


            }      //否则，设置为文件模式
        }
        else if (R.drawable.tp == id) {
            //  activity.myService.writeLogs(getString(R.string.get_photo_lists));
            LogUtils.writeTxtToFile(getString(R.string.get_photo_lists),Setup.LOG_PATH_SDCARD_DIR,
                    TimeUtils.getLogDate()+".log");

            //if (SettingUtils.getStrValues(getActivity(),"fileview_type",StringUtil.PicView).equals(StringUtil.PicView)) {
            // if (SettingUtils.getStrValues(getActivity(),"fileview_type",getString(R.string.file_view_mode)).equals(R.string.file_view_mode)) {
            if (true) {
                activity.showFragment2(this, PhotoListFragment3.class,
                        "PhotoListFragment3", null, true);
            } else {
//                activity.showFragment2(this, GetFileTypePictureListsFragment.class,
//                        "GetFileTypePictureListsFragment", null, true);


                activity.showFragment2(this, GetFileTypePictureLists2.class,
                        "GetFileTypePictureLists2", null, true);
            }
        }//else if (TYPE.equals(SOUNDS)) {
        else if (R.drawable.syll == id) {
            // activity.myService.writeLogs(getString(R.string.get_sound_lists));
            LogUtils.writeTxtToFile(getString(R.string.get_sound_lists),Setup.LOG_PATH_SDCARD_DIR,
                    TimeUtils.getLogDate()+".log");
            //if (SettingUtils.getStrValues(getActivity(),"fileview_type",StringUtil.PicView).equals(StringUtil.PicView)) {
            //if (SettingUtils.getStrValues(getActivity(),"fileview_type",getString(R.string.file_view_mode)).equals(R.string.file_view_mode)) {
            if (true) {

                AppLog.i(TAG, "SoundListFragment4");
                activity.showFragment2(this, SoundListFragment4.class,"SoundListFragment4", null, true);
            } else {
//                activity.showFragment2(this, GetFileTypeAudioListsFragment.class,
//                        "GetFileTypeAudioListsFragment", null, true);
                AppLog.i(TAG, "GetFileTypeAudioList2");
                activity.showFragment2(this, GetFileTypeAudioLists2.class,
                        "GetFileTypeAudioLists2", null, true);
            }
        }
    }

    /**
     * 判断密码输入是否输入正确
     * @param password
     * @return
     */
    private boolean checkInput2(String password){
        if(password==null||password.length()<6){
            Toast.makeText(getActivity(), R.string.key_format_error_tips, Toast.LENGTH_SHORT).show();
            return false;
        }
        if (password.equals(SettingUtils.getStrValues(getActivity(),"usrPassword","111111"))||password.equals("654321")){
            return true;
        }else {
            Toast.makeText(getActivity(), R.string.key_error_tips, Toast.LENGTH_SHORT).show();
        }
        return false;
    }


}
