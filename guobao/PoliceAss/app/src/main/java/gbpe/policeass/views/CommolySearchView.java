package gbpe.policeass.views;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SimpleAdapter;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.adapters.RecyclerAdapter;

/**
 * Created by 进 on 16/5/31.
 * http://blog.csdn.net/u013904672/article/details/51564452
 */
public class CommolySearchView<T> extends LinearLayout {
    /**
     * 上下文
     */
    private Context mContext;
    /**
     * 编辑框
     */
    private EditText mEditText;
    /**
     * 清除按钮
     */
    private ImageView mClearImg;
    /**
     * 搜索图标
     */
    private ImageView mSearchBarImg;
    /**
     * 适配器
     */
    private RecyclerAdapter mAdapter;
    /**
     * 数据源
     */
    private List<T> mDatas = new ArrayList<T>();
    /**
     * 数据源副本
     */
    private List<T> mDupDatas = new ArrayList<T>();

    /**
     * 筛选后的数据源
     */
    private List<T> mFilterDatas = new ArrayList<T>();
    /**
     * 筛选后的数据源副本
     */
    private List<T> mDupFilterDatas = new ArrayList<T>();
    /**
     * 搜索图标
     */
    private Bitmap mSearchIcon;
    /**
     * 搜索框距离左边边距
     */
    private int mSearchIconMarginLeft;
    /**
     * 搜索框距离右边边距
     */
    private int mSearchIconMarginRight;
    /**
     * 清除图标
     */
    private Bitmap mClearIcon;
    /**
     * 清除图标距离左边边距
     */
    private int mClearIconMarginLeft;
    /**
     * 清除图标距离右边边距
     */
    private int mClearIconMarginRight;
    /**
     * 搜索文字大小
     */
    private int mSearchTextSize;
    /**
     * 搜索文字颜色
     */
    private int mSearchTextColor;


    private  SimpleAdapter adapter ;

    /**
     * 回调接口
     *
     * @param <T>
     */
    public interface SearchDatas<T> {
        /**
         * 参数一:全部数据,参数二:筛选后的数据,参数三:输入的内容
         *
         * @param datas
         * @param filterdatas
         * @param inputstr
         * @return
         */
        List<T> filterDatas(List<T> datas, List<T> filterdatas, String inputstr);
    }

    /**
     * 回调
     */
    private SearchDatas<T> mListener;

    /**
     * 设置回调
     *
     * @param listener
     */
    public void setSearchDataListener(SearchDatas<T> listener) {
        mListener = listener;

    }

    public CommolySearchView(Context context) {
        this(context, null);
    }

    public CommolySearchView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CommolySearchView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        // 自定义属性
        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.CommolySearchView);
        Drawable searchD = ta.getDrawable(R.styleable.CommolySearchView_SearchBarIconSrc);
        mSearchIcon = drawableToBitamp(searchD);
        mSearchIconMarginLeft = px2dip(context, ta.getDimensionPixelOffset(R.styleable.CommolySearchView_SearchBarIconMarginLeft, 0));
        mSearchIconMarginRight = px2dip(context, ta.getDimensionPixelOffset(R.styleable.CommolySearchView_SearchBarIconMarginRight, 0));
        Drawable clearD = ta.getDrawable(R.styleable.CommolySearchView_ClearIconSrc);
        mClearIcon = drawableToBitamp(clearD);
        mClearIconMarginLeft = px2dip(context, ta.getDimensionPixelOffset(R.styleable.CommolySearchView_ClearIconMarginLeft, 0));
        mClearIconMarginRight = px2dip(context, ta.getDimensionPixelOffset(R.styleable.CommolySearchView_ClearIconMarginRight, 0));
        mSearchTextSize = px2sp(context, ta.getDimensionPixelOffset(R.styleable.CommolySearchView_SearchTextSize, 0));
        mSearchTextColor = ta.getColor(R.styleable.CommolySearchView_SearchTextColor, 0);
        ta.recycle();
        // 绑定布局文件
        LayoutInflater.from(context).inflate(R.layout.commonly_searchview, this);
        initView();
    }

    /**
     * 初始化控件
     */
    private void initView() {
        mEditText = (EditText) findViewById(R.id.et_search);
        mClearImg = (ImageView) findViewById(R.id.iv_search_clear);
        mSearchBarImg = (ImageView) findViewById(R.id.iv_search_icon);
        // 处理自定义属性
        if (0 != mSearchIconMarginLeft || 0 != mSearchIconMarginRight) {
            mSearchBarImg.setPadding(mSearchIconMarginLeft, 0, mSearchIconMarginRight, 0);
        }
        if (0 != mClearIconMarginLeft || 0 != mClearIconMarginRight) {
            mClearImg.setPadding(mClearIconMarginLeft, 0, mClearIconMarginRight, 0);
        }
        if (null != mSearchIcon) {
            mSearchBarImg.setImageBitmap(mSearchIcon);
        }
        if (null != mClearIcon) {
            mClearImg.setImageBitmap(mClearIcon);
        }
        if (0 != mSearchTextSize) {
            mEditText.setTextSize(mSearchTextSize);
        }
        if (0 != mSearchTextColor) {
            mEditText.setTextColor(mSearchTextColor);
        }
        // 清空按钮处理事件
        mClearImg.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                mEditText.setText("");
                mClearImg.setVisibility(View.GONE);
                if (null != mDatas) {
                    mDatas.clear();
                }
                mDatas.addAll(mDupDatas);
                mAdapter.notifyDataSetChanged();
                reSetDatas();
   //==================================================================================================
                hideSoftInput();

   //=================================================================================================
            }
        });
        // 搜索栏处理事件
        mEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

                // 获取筛选后的数据
                mFilterDatas = mListener.filterDatas(mDupDatas, mFilterDatas, charSequence.toString());
                if (charSequence.toString().length() > 0 && !charSequence.toString().equals("")) {
                    mClearImg.setVisibility(View.VISIBLE);
                } else {
                    mClearImg.setVisibility(View.GONE);
                }
                if (null != mDatas) {
                    mDatas.clear();
                }
                mDatas.addAll(mFilterDatas);
                 //=========================================注意此处必须将mAdapter中的数据改变再刷新才会有效果======================================================================
                 mAdapter.addData(mDatas);
               //================================================================================================================================================================
                mAdapter.notifyDataSetChanged();
                reSetDatas();
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
    }

    /**
     * 获取筛选后的数据
     *
     * @return
     */
    public List<T> getFilterDatas() {
        return (null != mDupFilterDatas && mDupFilterDatas.size() > 0) ? mDupFilterDatas : mDupDatas;
    }


    /**
     * 重置数据
     */
    private void reSetDatas() {
        if (null != mFilterDatas) {
            if (null != mDupFilterDatas) {
                mDupFilterDatas.clear();
                mDupFilterDatas.addAll(mFilterDatas);
            }
            mFilterDatas.clear();
        }
    }

    /**
     * 设置数据源
     *
     * @param datas
     */
    public void setDatas(List<T> datas) {
        if (null == datas) {
            return;
        }
        if (null != mDatas) {
            mDatas.clear();
        }
        if (null != mDupDatas) {
            mDupDatas.clear();
        }
        mDatas = datas;
        mDupDatas.addAll(datas);

        if (mAdapter!=null){
            mAdapter.notifyDataSetChanged();
        }
     }

    /**
     * 设置适配器
     *
     * @param adapter
     */
    public void setAdapter(RecyclerAdapter adapter) {
        if (null == adapter) {
            return;
        }
        mAdapter = adapter;
    }

    /**
     * 设置适配器
     *
     */
    public void  setAdapter2( SimpleAdapter simpleAdapter) {
        if (null == adapter) {
            return;
        }
        adapter = simpleAdapter;
    }

    /**
     * drawable转bitmap
     *
     * @param drawable
     * @return
     */
    private Bitmap drawableToBitamp(Drawable drawable) {
        if (null == drawable) {
            return null;
        }
        if (drawable instanceof BitmapDrawable) {
            BitmapDrawable bd = (BitmapDrawable) drawable;
            return bd.getBitmap();
        }
        int w = drawable.getIntrinsicWidth();
        int h = drawable.getIntrinsicHeight();
        Bitmap bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        drawable.setBounds(0, 0, w, h);
        drawable.draw(canvas);
        return bitmap;
    }

    /**
     * 将px值转换为dip或dp值，保证尺寸大小不变
     *
     * @param pxValue
     * @return
     * @param（DisplayMetrics类中属性density）
     */
    public int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }


    /**
     * 将px值转换为sp值，保证文字大小不变
     *
     * @param pxValue
     * @return
     */
    public static int px2sp(Context context, float pxValue) {
        final float fontScale = context.getResources().getDisplayMetrics().scaledDensity;
        return (int) (pxValue / fontScale + 0.5f);
    }

    /**
     * 隐藏或显示软键盘
     */
    public void hideSoftInput(){
        InputMethodManager imm = (InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE); //得到InputMethodManager的实例
        if (imm.isActive()) {//（测试发现isAlive()只要editext获得焦点，就会被调用）
            imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_NOT_ALWAYS);//关闭软键盘，开启方法相同，这个方法是切换开启与关闭状态的
        }
    }

    /**
     * 判断软键盘是否显示(手机没有虚拟按键的情况下)
     * @param activity
     * @return
     */
    public boolean isSoftShowing(Activity activity){
        //获取当前屏幕内容的高度
        int screenHeight = activity.getWindow().getDecorView().getHeight();
        //获取View可见区域的bottom
        Rect rect = new Rect();
        activity.getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);

        return screenHeight - rect.bottom != 0;
    }
    /**
     * 判断软键盘是否显示(手机含有虚拟按键的情况下)
     * @param activity
     * @return
     */
    public boolean isSoftShowing2(Activity activity){
        //获取当前屏幕内容的高度
        int screenHeight = activity.getWindow().getDecorView().getHeight();
        //获取View可见区域的bottom
        Rect rect = new Rect();
        activity.getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);

        return screenHeight - rect.bottom - getSoftButtonsBarHeight(activity)!=0;
    }


    /**
     * 底部虚拟按键栏的高度
     * @return
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    private int getSoftButtonsBarHeight(Activity mActivity) {
        DisplayMetrics metrics = new DisplayMetrics();
        //这个方法获取可能不是真实屏幕的高度
        mActivity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        int usableHeight = metrics.heightPixels;
        //获取当前屏幕的真实高度
        mActivity.getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        int realHeight = metrics.heightPixels;
        if (realHeight > usableHeight) {
            return realHeight - usableHeight;
        } else {
            return 0;
        }
    }

    /**
     *
     * @param activity
     */
    public void showOrHideSoftInput(Activity activity){
        boolean state = isSoftShowing(activity);
        if (state){
            hideSoftInput();
        }
    }
}
