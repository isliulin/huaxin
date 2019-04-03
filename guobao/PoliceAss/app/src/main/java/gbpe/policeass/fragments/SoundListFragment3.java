package gbpe.policeass.fragments;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.utils.AppLog;

/**
 * Created by Administrator on 2017/3/10.
 */
public class SoundListFragment3 extends BaseFragment implements View.OnClickListener{

    View view;
    //private TextView nameTextView;
    private SeekBar seekBar;
    private ListView listView;
    public static  List<HashMap<String, String>> data
            = new ArrayList<HashMap<String, String>>();;
    private int current;
    private MediaPlayer player;
    private Handler handler = new Handler();
    private ImageButton ppButton,preButton,nextButton;
    private boolean isPause=true;
    private boolean isStartTrackingTouch;
    private PhoneListener phoneListener;
    private ImageView ib_sound_back;
	private MyCustomAdapter myAdapter;
	private static final String TAG = "SoundListFragment3";

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_sound_temp,null);

        return view;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_LEFT:
				previous();
				break;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				next();
				break;
		}
        return true;
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        phoneListener = new PhoneListener();
        //nameTextView = (TextView) view.findViewById(R.id.name);
        seekBar = (SeekBar) view.findViewById(R.id.seekBar);
        listView = (ListView) view.findViewById(R.id.list);
        ppButton = (ImageButton)view. findViewById(R.id.pp);
        ppButton.setOnClickListener(this);
        preButton = (ImageButton)view.findViewById(R.id.previous);
        preButton.setOnClickListener(this);
        nextButton = (ImageButton)view.findViewById(R.id.next);
        nextButton.setOnClickListener(this);

        ib_sound_back = (ImageView)view.findViewById(R.id.image_back);
        ib_sound_back.setOnClickListener(this);
        LayoutInflater inflater = getLayoutInflater();


        //创建一个音乐播放器
        player = new MediaPlayer();

        //显示音乐播放列表
        generateListView(inflater);

        //进度条监听器
        seekBar.setOnSeekBarChangeListener(new MySeekBarListener());

        //播放器监听器
        player.setOnCompletionListener(new MyPlayerListener());

        //意图过滤器
        IntentFilter filter = new IntentFilter();

        //播出电话暂停音乐播放
        filter.addAction("android.intent.action.NEW_OUTGOING_CALL");
        activity.registerReceiver(phoneListener, filter);

        //创建一个电话服务
        TelephonyManager manager = (TelephonyManager)
                activity.getSystemService(activity.TELEPHONY_SERVICE);

        //监听电话状态，接电话时停止播放
        manager.listen(new MyPhoneStateListener(), PhoneStateListener.LISTEN_CALL_STATE);

        // ycb:20180914 此处直接播放没有声音?
        current = getArguments().getInt("position");
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                play();
            }
        },500);

     }


    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.pp:{
                pp(v);
                break;
            }
            case R.id.previous:{
                previous(v);
                break;
            }
            case R.id.next:{
                next(v);
                break;
            }
            case R.id.image_back:{
                popSelf();
                break;
            }
        }
    }

    /*
     * 监听电话状态
     */
    private final class MyPhoneStateListener extends PhoneStateListener {
        public void onCallStateChanged(int state, String incomingNumber) {
            pause();
        }
    }

    /*
     * 播放器监听器
     */
    private final class MyPlayerListener implements MediaPlayer.OnCompletionListener {
        //歌曲播放完后自动播放下一首歌曲
        public void onCompletion(MediaPlayer mp) {
            //next();
            nextNoLoop();
        }
    }

    /*
     * 下一首按钮
     */
    public void next(View view) {
        next();
    }

    /*
     * 前一首按钮
     */
    public void previous(View view) {
        previous();
    }

    /*
     * 播放前一首歌
     */
    private void previous() {
        current = current - 1 < 0 ? data.size() - 1 : current - 1;
        play();
    }

    /*
     * 播放下一首歌
     */
    private void next() {
        if (0==data.size()){
            return;
        }

		// 循环播放
        current = (current + 1) % data.size();

		// add by ycb, for highlight the selected item
		//myAdapter.setSelectItem(current);						
        //myAdapter.notifyDataSetInvalidated();

        play();
    }

	private void nextNoLoop()
    {
		if (0==data.size())
		{
			return;
		}

		if (current < (data.size()-1))
		{
			current ++;

			// add by ycb, for highlight the selected item
			//myAdapter.setSelectItem(current);						
	        //myAdapter.notifyDataSetInvalidated();

			play();
		}else
		{
			current = 0;
			//ppButton.setText("▶");
            ppButton.setImageDrawable(getResources().getDrawable(R.mipmap.snd_play_start));
			myAdapter.setSelectItem(-1);
			myAdapter.notifyDataSetInvalidated();
		}
	}

    /*
     * 进度条监听器
     */
    private final class MySeekBarListener implements SeekBar.OnSeekBarChangeListener {
        //移动触发
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        }

        //起始触发
        public void onStartTrackingTouch(SeekBar seekBar) {
            isStartTrackingTouch = true;
        }

        //结束触发
        public void onStopTrackingTouch(SeekBar seekBar) {
            player.seekTo(seekBar.getProgress());
            isStartTrackingTouch = false;
        }
    }

    /*
     * 显示音乐播放列表
     */
    private void generateListView(LayoutInflater li) {
//        final   List<File> list = new ArrayList<File>();
//        //获取sdcard中的所有歌曲
//
//
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//                findAll(new File(Setup.soundPaths), list);
//                activity.runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//
//
//                        //播放列表进行排序，字符顺序
//                        Collections.sort(list);


//                        for (File file : list) {
//                            HashMap<String, String> map = new HashMap<String, String>();
//                            map.put("name", file.getName());
//                            map.put("path", file.getAbsolutePath());
//                            data.add(map);
//                        }

//          SimpleAdapter adapter = new SimpleAdapter(getActivity(),
//           data, R.layout.fragment_soundlist2, new String[] { "name" }, new int[] { R.id.mName });

		  myAdapter = new MyCustomAdapter(li, data,
					R.layout.fragment_soundlist2);

          //listView.setAdapter(adapter);
          listView.setAdapter(myAdapter);
          listView.setOnItemClickListener(new MyItemListener());
    }


    private final class MyItemListener implements AdapterView.OnItemClickListener {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            current = position;
			//myAdapter.setSelectItem(current);						
            //myAdapter.notifyDataSetInvalidated();
            play();
			
        }
    }

    private void play() {
        AppLog.i(TAG,"开始播放");
        try {
            //重播
            player.reset();

            //获取歌曲路径
            player.setDataSource(data.get(current).get("path"));

            //缓冲
            player.prepare();

            //开始播放
            player.start();

            //显示歌名
            String name = data.get(current).get("name");
            String finalName = name.substring(0,name.indexOf("."));
            //nameTextView.setText(""+finalName);

            //设置进度条长度
            seekBar.setMax(player.getDuration());

            //播放按钮样式
            //ppButton.setText("||");
            ppButton.setImageDrawable(getResources().getDrawable(R.mipmap.snd_play_stop));

			// add by ycb, for highlight the selected item
			myAdapter.setSelectItem(current);						
            myAdapter.notifyDataSetInvalidated();

            //发送一个Runnable, handler收到之后就会执行run()方法
            handler.post(new Runnable() {
                public void run() {
                    // 更新进度条状态
                    if (!isStartTrackingTouch)
                        if (player ==null){
                            return;
                        }
                        seekBar.setProgress(player.getCurrentPosition());
                    // 1秒之后再次发送
                    handler.postDelayed(this, 1000);
                }
            });
        } catch (Exception e) {
            AppLog.e(TAG,"播放错误");
            e.printStackTrace();
        }
    }

    /**
     * 查找文件路径中所有mp3文件
     * @param file 要找的目录
     * @param list 用来装文件的List
     */
    private void findAll(File file, List<File> list) {
        File[] subFiles = file.listFiles();
        if (subFiles != null)
            for (File subFile : subFiles) {
                if (subFile.isFile() && subFile.getName().endsWith(".mp3")
                        ||subFile.isFile() && subFile.getName().endsWith(".wav"))
                    list.add(subFile);
                else if (subFile.isDirectory())//如果是目录
                    findAll(subFile, list); //递归
            }
    }

    /*
     * 暂停/播放按钮
     */
    public void pp(View view) {

        //默认从第一首歌曲开始播放
        if (!player.isPlaying() && !isPause) {
            play();
            return;
        }

        ImageButton imgButton = (ImageButton)view;
        if (isPause){
            resume();
            imgButton.setImageDrawable(getResources().getDrawable(R.mipmap.snd_play_stop));
            AppLog.i(TAG,"resume");
        }else{
            pause();
            imgButton.setImageDrawable(getResources().getDrawable(R.mipmap.snd_play_start));
            AppLog.i(TAG,"play");
        }
/*
        Button button = (Button) view;
        //暂停/播放按钮
        if ("||".equals(button.getText())) {
            pause();
            button.setText("▶");
        } else {
            resume();
            button.setText("||");
        }*/
    }

    /*
     * 开始操作
     */
    private void resume() {
        if (isPause) {
            player.start();
            isPause = false;
        }
    }

    /*
     * 暂停操作
     */
    private void pause() {
        if (player != null && player.isPlaying()) {
            player.pause();
            isPause = true;
        }
    }

    /*
     * 收到广播时暂停
     */
    private final class PhoneListener extends BroadcastReceiver {
        public void onReceive(Context context, Intent intent) {
            pause();
        }
    }

    /*
     * 恢复播放
     * @see android.app.Activity#onResume()
     */
    public void onResume() {
        super.onResume();
        //resume();
        //current = getArguments().getInt("position");
        //play();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (player.isPlaying()||player !=null){
            player.stop();
            player.release();
            player = null;
        }
        activity.unregisterReceiver(phoneListener);
    }

    @Override
    protected boolean onBackPressed() {
        return false;
    }


	// 自定义 List Adapter
	public class MyCustomAdapter extends BaseAdapter
	{
		private LayoutInflater customInflater;
		//private List<ReadyTask> list;
		public List<HashMap<String, String>> m_MscList;
		private int layoutID;



		public class ViewHolder{
			TextView m_soundName;

		}

		public MyCustomAdapter(LayoutInflater customInflater, List<HashMap<String, String>> list,
				int layoutID) {
			this.customInflater =customInflater;
			this.m_MscList = list;
			this.layoutID = layoutID;

		}

		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return m_MscList.size();
		}

		@Override
		public Object getItem(int position) {
			// TODO Auto-generated method stub
			return m_MscList.get(position);
		}

		@Override
		public long getItemId(int position) {
			// TODO Auto-generated method stub
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			// final ReadyTask rTask = (ReadyTask)getItem(position);
			final HashMap<String,String> rTask = (HashMap<String,String>)getItem(position);
			 
			ViewHolder viewHolder = null;
			if (convertView == null) {
				convertView = customInflater.inflate(layoutID, null);
				viewHolder = new ViewHolder();
				//viewHolder.m_order=(TextView) convertView.findViewById(R.id.m_order);
				viewHolder.m_soundName = (TextView)convertView.findViewById(R.id.mName);
				convertView.setTag(viewHolder);
				
			} else 
			{
				viewHolder = (ViewHolder) convertView.getTag();
			}
			if (m_MscList!=null&&m_MscList.size()>0)
			{
				String sPath = rTask.get("path");
				String str = new File(sPath.trim()).getName();
				viewHolder.m_soundName.setText(str);
				//viewHolder.m_soundName.setText(rTask.get("path"));
			}
	        if (position == selectItem) {   
	            convertView.setBackgroundColor(Color.CYAN);
	        }    
	        else {   
	            convertView.setBackgroundColor(Color.TRANSPARENT);   
	        }
			return convertView;
		}

	    public  void setSelectItem(int selectItem) {   
	        this.selectItem = selectItem;   
	   }   
	   private int  selectItem=-1;   

	}
	
}
