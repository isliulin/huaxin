package gbpe.policeass.fragments.quaryfragments;

import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;

import gbpe.policeass.fragments.BaseFragment;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.GalleryAdapter;
import gbpe.policeass.fragments.ImgDetialFragment;
import gbpe.policeass.utils.FileUtils;

public class FriendFragment extends BaseFragment implements AdapterView.
		OnItemSelectedListener {
 	private RecyclerView recyclerView;
	private ArrayList<String> videoPath = new ArrayList<>();
	GalleryAdapter mAdapter;
	private Spinner spinner;
	private 	View friendView;
	TextView tv_name,tv_pNumber,tv_startTime,tv_stopTime,tvSize,tv_seinor,tv_duration;
	@Override
	public View onCreateView(LayoutInflater inflater,ViewGroup container,
							 Bundle savedInstanceState)
	{
		super.onCreateView(inflater, container, savedInstanceState);
		 friendView = inflater.inflate(R.layout.activity_tab_friend,
				 container,false);
		initViews();
		getFilePath(Setup.videoPaths);
		getFilePath(Setup.interVideoPaths);

		return friendView;
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

	@Override
	public void onResume()
	{
		super.onResume();
		if (videoPath.size()<=0)
		{
			Toast.makeText(getActivity(), "无图片", Toast.LENGTH_SHORT).show();
		}
 		mAdapter = new GalleryAdapter(getActivity(), videoPath);
		mAdapter.notifyDataSetChanged();
		mAdapter.setOnItemClickLitener(new GalleryAdapter.OnItemClickLitener()
		{
			@Override
			public void onItemClick(View view, int position)
			{
				//ImgDetialActivity.startMe(getActivity(),videoPath,position);
				Bundle bundle = new Bundle();
				bundle.putInt("position",position);
				bundle.putStringArrayList("imgPaths",videoPath);
				activity.showFragment2(FriendFragment.this,ImgDetialFragment.class,
						"FriendFragment",bundle,true);
			}
		});
		recyclerView.setAdapter(mAdapter);
	}



	private void initViews() {
		recyclerView = (RecyclerView) friendView.
				findViewById(R.id.id_recyclerview_horizontal);
		spinner = (Spinner)friendView.findViewById(R.id.sp_rank);
		spinner.setOnItemSelectedListener(this);
		if (videoPath.size()>0) {
			videoPath.clear();
		}
		LinearLayoutManager layoutManager = new LinearLayoutManager(getActivity());
		layoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);
		recyclerView.setLayoutManager(layoutManager);

		tv_name = findView(R.id.tv_name);
		tv_pNumber = findView(R.id.tv_pNumber);
		tv_startTime = findView(R.id.startTime);
		tv_stopTime = findView(R.id.stopTime);
		tvSize = findView(R.id.tvSize);
		tv_seinor = findView(R.id.tv_seinor);
		tv_duration = findView(R.id.tv_duration);
	}

	//递归获得每个文件的绝对路径
	public  void getFilePath(String dirPath){

		File dir = new File(dirPath);
		File[] files = dir.listFiles();
		if(files == null)
		{
			return ;
		}

		for(int i = 0; i < files.length; i++)
		{
			if(files[i].isDirectory())
			{
				getFilePath(files[i].getAbsolutePath());
			} else
			{
				if (files[i].getAbsolutePath().
						endsWith(Setup.imgFilePostfix))
				{
					videoPath.add(files[i].getAbsolutePath());
				}
			}
		}
	}

	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
		//获得下拉列表上的信息。
		String ss =   parent.getItemAtPosition(position).toString();
		if (ss.equals("一般")){

			markFile(videoPath.get(0));
		}
		else if (ss.equals("重要"))
		{
			markFile(videoPath.get(0));
		}
		Toast.makeText(getActivity(), "标记完成", Toast.LENGTH_SHORT).show();

	}

	@Override
	public void onNothingSelected(AdapterView<?> parent) {

	}

	public <T extends View> T findView(int ViewId) {

		return (T) friendView.findViewById(ViewId);
	}


	/**
	 * 标记文件
	 * @param filePath
	 */
	public void markFile(final String filePath){
		new Thread(new Runnable() {
			@Override
			public void run() {

				FileUtils.changeFileType(filePath,getActivity());
			}
		}).start();
	}
}


//http://m.blog.csdn.net/article/details?id=38173061