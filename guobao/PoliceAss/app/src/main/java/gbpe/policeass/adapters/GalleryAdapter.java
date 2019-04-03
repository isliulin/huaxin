package gbpe.policeass.adapters;

/**
 * Created by Administrator on 2016/12/27.
 */
import java.io.File;
import java.util.List;

import android.content.Context;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.thumbnail.NoMulitClickListener;

public class GalleryAdapter extends
        RecyclerView.Adapter<GalleryAdapter.ViewHolder>
{

    private LayoutInflater mInflater;
    private List<String> mDatas;
    private Context context;

    /**
     * ItemClick的回调接口
     * @author zhy
     *
     */
    public interface OnItemClickLitener
    {
        void onItemClick(View view, int position);
    }

    private OnItemClickLitener mOnItemClickLitener;

    public void setOnItemClickLitener(OnItemClickLitener mOnItemClickLitener)
    {
        this.mOnItemClickLitener = mOnItemClickLitener;
    }


    public GalleryAdapter(Context context, List<String> datats)
    {
        mInflater = LayoutInflater.from(context);
        mDatas = datats;
        this.context = context;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder
    {
        public ViewHolder(View arg0)
        {
            super(arg0);
        }

        ImageView mImg;
        TextView mTxt;
    }

    @Override
    public int getItemCount()
    {
        return mDatas.size();
    }

    /**
     * 创建ViewHolder
     */
    @Override
    public ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)
    {
        View view = mInflater.inflate(R.layout.item_query_video,
                viewGroup, false);
        ViewHolder viewHolder = new ViewHolder(view);

        viewHolder.mImg = (ImageView) view
                .findViewById(R.id.id_index_gallery_item_image);
        viewHolder.mTxt = (TextView)view.findViewById(R.id.id_index_gallery_item_text);
        return viewHolder;
    }

    /**
     * 设置值
     */
    @Override
    public void onBindViewHolder(final ViewHolder viewHolder, final int i) {
        //   viewHolder.mImg.setImageResource(mDatas.get(i));
        if (mDatas.get(i).endsWith(Setup.soundFilePostfix)) {
            viewHolder.mImg.setImageResource(R.drawable.myvoice);
        } else {
            /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
            Glide
                    .with(context)
                    .load(Uri.fromFile(new File(mDatas.get(i))))
                    .placeholder(R.drawable.ic_stub)
                    .error(R.drawable.videoerror)
                    .into(viewHolder.mImg);
        }
        File file = new File(mDatas.get(i));
        String name = file.getName();
        String title = name.substring(0,name.indexOf("."));
        String conten = title.substring(0,8);
        viewHolder.mTxt.setText(conten);

        //如果设置了回调，则设置点击事件
        if (mOnItemClickLitener != null) {
//            viewHolder.itemView.setOnClickListener(new View.OnClickListener() {
//                @Override
//                public void onClick(View v) {
//                    mOnItemClickLitener.onItemClick(viewHolder.itemView, i);
//                }
//            });
            viewHolder.itemView.setOnClickListener(new NoMulitClickListener() {
                @Override
                public void onNoDoubleClick(View view) {
                    mOnItemClickLitener.onItemClick(viewHolder.itemView, i);
                }
            });

        }
    }
}