package gbpe.policeass.fragments.quaryfragments;

import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Spinner;
import android.widget.Toast;

import java.io.File;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.GalleryAdapter;
 import gbpe.policeass.utils.FileUtils;

public class ContactsFragment extends Fragment implements AdapterView.
		OnItemSelectedListener {
	private RecyclerView recyclerView;
	private List<String> videoPath = new LinkedList<>();
	GalleryAdapter mAdapter;
	private Spinner spinner;
	View contactsView;
	@Override
	public View onCreateView(LayoutInflater inflater,ViewGroup container,Bundle
			savedInstanceState){
		super.onCreateView(inflater, container, savedInstanceState);
		contactsView = inflater.inflate
				(R.layout.activity_tab_contacts, container,false);
		recyclerView = (RecyclerView) contactsView.
				findViewById(R.id.id_recyclerview_horizontal);
		if (videoPath.size()>0){
			videoPath.clear();
		}
		getFilePath(Setup.videoPaths);
		initViews();
		return contactsView;
	}

	@Override
	public void onResume() {
		super.onResume();
		if (videoPath.size()<=0){
			Toast.makeText(getActivity(), "无音频文件", Toast.LENGTH_SHORT).show();
		}
 		mAdapter = new GalleryAdapter(getActivity(), videoPath);
		mAdapter.notifyDataSetChanged();
		mAdapter.setOnItemClickLitener(new GalleryAdapter.OnItemClickLitener()
		{
			@Override
			public void onItemClick(View view, int position)
			{
				Toast.makeText(getActivity(), position+"", Toast.LENGTH_SHORT)
						.show();
			}
		});
		recyclerView.setAdapter(mAdapter);
	}


	private void initViews() {
		spinner = (Spinner)contactsView.findViewById(R.id.sp_rank);
		spinner.setOnItemSelectedListener(this);
		LinearLayoutManager layoutManager = new LinearLayoutManager(getActivity());
		layoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);
		recyclerView.setLayoutManager(layoutManager);
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
			}
			else
			{
				if (files[i].getAbsolutePath().
						endsWith(Setup.soundFilePostfix))
				{
					videoPath.add(files[i].getAbsolutePath());
					System.out.println("---->>fil"+files[i].getAbsolutePath());
				}
			}
		}
	}


	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
		//获得下拉列表上的信息。
		String ss =   parent.getItemAtPosition(position).toString();
		if (ss.equals("一般")){//标记为一般文件
				//markFile(videoPath.get(0));
			Toast.makeText(getActivity(), "标记完成", Toast.LENGTH_SHORT).show();

		}else if (ss.equals("重要"))//标记为重点文件
		{
			//markFile(videoPath.get(0));
			Toast.makeText(getActivity(), "标记完成", Toast.LENGTH_SHORT).show();
		}
	}


	@Override
	public void onNothingSelected(AdapterView<?> parent) {}

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

	//

}
