package gbpe.policeass.adapters;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.List;

 ;import gbpe.policeass.R;
import gbpe.policeass.utils.thumbnail.NoMulitClickListener;

/**
 * Created by 橘子桑 on 2016/1/2.
 */
public abstract class RecyclerAdapter<T> extends RecyclerView.Adapter<RecycleHolder> {

    private Context mContext;
    private List<T> mDatas;
    private int mLayoutId;
    private LayoutInflater mInflater;
    private boolean isState;



    private OnItemLongClickListener onItemLongClickListener;

    public void setOnItemLongClickListener(OnItemLongClickListener onItemLongClickListener) {
        this.onItemLongClickListener = onItemLongClickListener;
    }

    private OnItemClickListener onItemClickListener;

    public void setOnItemClickListener(OnItemClickListener onItemClickListener) {
        this.onItemClickListener = onItemClickListener;
    }


    public RecyclerAdapter(Context mContext, List<T> mDatas, int mLayoutId) {
        this.mContext = mContext;
        this.mDatas = mDatas;
        this.mLayoutId = mLayoutId;
        mInflater = LayoutInflater.from(mContext);
    }

    @Override
    public RecycleHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new RecycleHolder(mInflater.inflate(mLayoutId, parent, false));
    }

    @Override
    public void onBindViewHolder(final RecycleHolder holder, int position) {
        convert(holder, mDatas.get(position), position);
        if (onItemClickListener != null) {
            //设置背景
            Log.i("TAG","点击");
            holder.itemView.setBackgroundResource(R.drawable.recycler_bg);
//            holder.itemView.setOnClickListener(new View.OnClickListener() {
//                @Override
//                public void onClick(View v) {
//                    //注意，这里的position不要用上面参数中的position，会出现位置错乱\
//                    onItemClickListener.OnItemClickListener(holder.itemView, holder.getLayoutPosition());
//                }
//            });
//            holder.itemView.setOnLongClickListener(new View.OnLongClickListener() {
//                @Override
//                public boolean onLongClick(View v) {
//                    onItemLongClickListener.OnItemLongClickListener(holder.itemView, holder.getLayoutPosition());
//                    return true;
//                }
//            });


            holder.itemView.setOnClickListener(new NoMulitClickListener() {//括号内为自定义点击监听器，防止多次或频繁点击按钮时造成多次点击事件
                @Override
                public void onNoDoubleClick(View view) {
                    onItemClickListener.OnItemClickListener(holder.itemView, holder.getLayoutPosition());
                }
            });

            holder.itemView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View v) {
                    onItemLongClickListener.OnItemLongClickListener(holder.itemView, holder.getLayoutPosition());
                    return true;
                }
            });
        }

    }
    public void setIsState(boolean isState) {
        this.isState = isState;//设置完后立即刷新适配器，
        // 当前item会根据isState的值来决定CheckBox是否显示出来
         notifyDataSetChanged();
    }

    public abstract void convert(RecycleHolder holder, T data, int position);

    @Override
    public int getItemCount() {
        return mDatas.size();
    }


    public interface OnItemClickListener {
       public void OnItemClickListener(View view, int position);
    }
    public interface  OnItemLongClickListener{
      public   void OnItemLongClickListener(View view, int position);
    }


    public void bindData(List<T> datas) {
        if (datas != null) {
            this.mDatas = datas;
            this.notifyDataSetChanged();
        }
    }

    public void addData(List<T> datas) {

        if (datas !=null){
            //   this.mDatas.clear();
            // this.mDatas.addAll(datas);
            this.mDatas = datas;
            this.notifyDataSetChanged();
        }



    }

}
