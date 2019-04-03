package gbpe.policeass.adapters;

/**
 * Created by Administrator on 2016/9/2.
 */
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by chuyh on 2016/6/6.
 */
public abstract class CommonAdapter<T> extends BaseAdapter {
    protected List<T> mList;
    protected LayoutInflater mInflater;
    protected int mItemLayoutId;
    protected Context mContext;

    public CommonAdapter(Context context, int itemLayoutId) {
        this(context, itemLayoutId, null);
    }

    public CommonAdapter(Context context, int itemLayoutId, List<T> datas) {
        this.mContext = context;
        this.mItemLayoutId = itemLayoutId;
        this.mInflater = LayoutInflater.from(context);
        if (datas != null) {
            this.mList = datas;
        } else {
            this.mList = new ArrayList<>();
        }
    }

    public void bindData(List<T> datas) {
        if (datas != null) {
            this.mList = datas;
            this.notifyDataSetChanged();
        }
    }

    public void addData(List<T> datas) {
        if (datas != null) {
            this.mList.clear();
            this.mList.addAll(datas);
            this.notifyDataSetChanged();
        }
    }

    @Override
    public int getCount() {
        return this.mList.size();
    }

    @Override
    public T getItem(int position) {
        return mList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder = ViewHolder.get(mContext, convertView, parent, mItemLayoutId, position);
        conver(holder, getItem(position));
        return holder.getConvertView();
    }

    public abstract void conver(ViewHolder holder, T t);
}
