package gbpe.policeass.adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Switch;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.R;


/**
 * Created by Administrator on 2016/9/12.
 */
public class MyBaseAdapter extends BaseAdapter {
    private Context context;
    private List<String> list;
    private LayoutInflater inflater;
    private int TYPE = 2;
    private int TYPEONE = 0;
    private int TYPETWO = 1;
    private List<View> mviews;

    public MyBaseAdapter(Context context,List<String> list) {
        this.context = context;
        this.list = list;
        inflater = LayoutInflater.from(context);
        mviews = new ArrayList<>();

    }

    @Override
    public int getCount() {
        //return list==null? 0:list.size();
        return 5;
    }

    @Override
    public int getViewTypeCount() {
        return TYPE;
    }


    @Override
    public int getItemViewType(int position) {
        if (position<5){
            return TYPEONE;
        }
        else
            return TYPETWO;
     }

    @Override
    public Object getItem(int position) {
        return list.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        MyViewHolder myViewHolder = null;
        switch (getItemViewType(position)) {
            case 0:
                if (convertView == null){
                    myViewHolder = new MyViewHolder();
                    convertView = inflater.inflate(R.layout.item_setup,null);
                    myViewHolder.textView = (TextView)convertView.findViewById(R.id.tv_setup_name);

                }
                else {
                    myViewHolder = (MyViewHolder) convertView.getTag();
                }
                myViewHolder.textView.setText(list.get(position));
                break;

            case 1:
                if (convertView == null){
                    myViewHolder = new MyViewHolder();
                    convertView = inflater.inflate(R.layout.item_setup2,null);
                    myViewHolder.textView = (TextView)convertView.findViewById(R.id.tv_setup_name);
                    myViewHolder.aSwitch = (Switch)convertView.findViewById(R.id.sw_setup);
                    convertView.setTag(myViewHolder);
                }
                else {
                    myViewHolder = (MyViewHolder)convertView.getTag();
                }
                myViewHolder.textView.setText(list.get(position));

                break;


        }


        return convertView;
    }
    class MyViewHolder{
        TextView textView;
        Switch aSwitch;
    }
}
