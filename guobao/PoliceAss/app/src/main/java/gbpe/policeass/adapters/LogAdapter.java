package gbpe.policeass.adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import java.io.File;
import java.util.List;
import gbpe.policeass.R;
import gbpe.policeass.utils.FileSizeUtil;

/**
 * Created by Administrator on 2017/1/10.
 */
public class LogAdapter extends BaseAdapter {
    List<String> logPaths;
    Context context;
     LayoutInflater mInflater;


    public LogAdapter(Context context,List<String> logPaths) {
         this.logPaths = logPaths;
         this.context = context;
        mInflater = LayoutInflater.from(context);
    }

    @Override
    public int getCount() {
        return logPaths.size();
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView==null){
            holder = new ViewHolder();
            convertView = mInflater.inflate(R.layout.item_log,null);
            holder.tv_log_name = (TextView)convertView.findViewById(R.id.tv_log_title);
            holder.tv_log_size = (TextView)convertView.findViewById(R.id.tv_log_size);
            convertView.setTag(holder);
        }else
        {
            holder = (ViewHolder) convertView.getTag();
        }
        String name = new File(logPaths.get(position)).getName();
        holder.tv_log_name .setText(name);
        holder.tv_log_size.setText( FileSizeUtil.getFileOrFilesSize(logPaths.get(position),1));

        return convertView;
    }

    public  class ViewHolder
    {
        TextView tv_log_name;
        TextView tv_log_size;
    }
}
