package gbpe.policeass.zfysim;

import android.support.annotation.StringRes;

public class HomeItem {
    //private String mTitle;
    private int mTitle;
    private int mResourceId;
    public ItemClicked mItemClicked;

    public @StringRes int getTitle(){
        return mTitle;
    }

    public int getRid () {
        return mResourceId;
    }

    public HomeItem(@StringRes int title, int rid, ItemClicked itemClicked){
        mItemClicked = itemClicked;
        mTitle = title;
        mResourceId = rid;
    }

    public interface ItemClicked{
        void onItemClicked(String tag);
    }

    public void response (String tag){
        mItemClicked.onItemClicked(tag);
    }
}
