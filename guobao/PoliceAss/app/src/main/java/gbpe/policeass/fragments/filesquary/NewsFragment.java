package gbpe.policeass.fragments.filesquary;

/**
 * Created by Administrator on 2016/11/17.
 */
import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import java.io.File;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.fragments.BaseFragment;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.adapters.LogAdapter;
import gbpe.policeass.utils.FileUtils;

public class NewsFragment extends BaseFragment {

    private ListView listView;
    LogAdapter logAdapter;
    @Override
    public void onAttach(Activity activity) {

        super.onAttach(activity);
    }

    private View view;
    private List<String> logPaths;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                            Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_log,null);

        listView = (ListView)view.findViewById(R.id.lv_log);
        initData();
        return view;
    }
    @Override
    public void setArguments(Bundle bundle) {
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
    public void onResume() {
        super.onResume();
        logAdapter = new LogAdapter(getActivity(),logPaths);
        listView.setAdapter(logAdapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                FileUtils.openFile(new File(logPaths.get(position)),getActivity());
            }
        });
        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
                Toast.makeText(getActivity(), "长按"+position, Toast.LENGTH_SHORT).show();

                return true;
            }
        });
    }

    private void initData(){
        logPaths = new LinkedList<>();
        getFilePath(Setup.LOG_PATH_SDCARD_DIR);
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
                        endsWith(Setup.logFilePostfix)) {
                    logPaths.add(files[i].getAbsolutePath());
                }
            }
        }
    }


}
