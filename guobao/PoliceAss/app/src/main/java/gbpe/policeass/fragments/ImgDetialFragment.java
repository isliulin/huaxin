package gbpe.policeass.fragments;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.annotation.Nullable;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.assist.FailReason;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;
import com.nostra13.universalimageloader.core.assist.SimpleImageLoadingListener;
import com.nostra13.universalimageloader.core.display.FadeInBitmapDisplayer;

import java.io.File;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.utils.SDCardUtils;

/**
 * Created by Administrator on 2017/1/9.
 */
public class ImgDetialFragment extends BaseFragment {
    ImagePagerAdapter adapter;

    private static final String STATE_POSITION = "STATE_POSITION";

    DisplayImageOptions options;
    List<String> images;
    ViewPager pager;
    private View view;
    Bundle bundle;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        view = inflater.inflate(R.layout.fragment_img_detial,container,false);
        bundle = getArguments();
        images = bundle.getStringArrayList("imgPaths");
        view.findViewById(R.id.image_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });
        return view;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        // 当前显示View的位置

        int pagerPosition = bundle.getInt("position", 0);

//        // 如果之前有保存用户数据
//        if (savedInstanceState != null) {
//            pagerPosition = savedInstanceState.getInt(STATE_POSITION);
//            if (adapter !=null){
//                adapter.notifyDataSetChanged();
//            }
//        }

        options = new DisplayImageOptions.Builder()
                .showImageForEmptyUri(R.mipmap.ic_empty)
                .showImageOnFail(R.mipmap.ic_error)
                .resetViewBeforeLoading(true)
                .cacheOnDisc(true)
                .imageScaleType(ImageScaleType.EXACTLY)
                .bitmapConfig(Bitmap.Config.RGB_565)
                .displayer(new FadeInBitmapDisplayer(300))
                .build();

        pager = (ViewPager) view.findViewById(R.id.pager);
        adapter = new ImagePagerAdapter(images);
        pager.setAdapter(adapter);

        adapter.notifyDataSetChanged();
        pager.setCurrentItem(pagerPosition);    // 显示当前位置的View

    }

    @Override
    protected boolean onBackPressed() {
        return false;
    }
    @Override
    public void onSaveInstanceState(Bundle outState) {
        // 保存用户数据
        outState.putInt(STATE_POSITION, pager.getCurrentItem());
        if (adapter !=null){
            adapter.notifyDataSetChanged();
        }
    }


    private class ImagePagerAdapter extends PagerAdapter {

        private List<String> images;
        private LayoutInflater inflater;

        ImagePagerAdapter(List<String> images) {
            this.images = images;
            inflater = activity.getLayoutInflater();
        }


        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            ((ViewPager) container).removeView((View) object);

        }

        @Override
        public void finishUpdate(View container) {
        }

        @Override
        public int getCount() {
            return images.size();
        }

        @Override
        public Object instantiateItem(ViewGroup view, int position) {
            View imageLayout = inflater.inflate(R.layout.item_pager_image, view, false);
            ImageView imageView = (ImageView) imageLayout.findViewById(R.id.image);
            final ProgressBar spinner = (ProgressBar) imageLayout.findViewById(R.id.loading);


            //String imageUri = "file:///mnt/sdcard/111111.png";
            //加载指定sd卡上的图片 "file:///mnt/sdcard/指定文件或文件夹"
            String filePath = images.get(position);
            String fileName = new File(filePath).getName();
            String uri = "";
            if (fileName.contains("I"))
            {
              if (SDCardUtils.isSDCardEnable()){
                    uri = "file:///storage/sdcard0/DCIM/101ANDRO/" + fileName;//文件夹一定不能写错
                }
            }else
            {
               // uri = "file:///mnt/sdcard/DCIM/100ANDRO/" + fileName;//文件夹一定不能写错
             if (SDCardUtils.isSDCardEnable()){
                    uri = "file:///storage/sdcard0/DCIM/100ANDRO/" + fileName;//文件夹一定不能写错
                }
            }
            // String uri = "file:///mnt/sdcard/DCIM/" + fileName;//文件夹一定不能写错
            activity. imageLoader.displayImage(uri, imageView, options, new SimpleImageLoadingListener() {
                @Override
                public void onLoadingStarted(String imageUri, View view) {
                    spinner.setVisibility(View.VISIBLE);
                }


                @Override
                public void onLoadingFailed(String imageUri, View view, FailReason failReason) {
                    String message = null;
                    switch (failReason.getType()) {        // 获取图片失败类型
                        case IO_ERROR:                // 文件I/O错误
                            message = "Input/Output error";
                            break;
                        case DECODING_ERROR:        // 解码错误
                            message = "Image can't be decoded";
                            break;
                        case NETWORK_DENIED:        // 网络延迟
                            message = "Downloads are denied";
                            break;
                        case OUT_OF_MEMORY:            // 内存不足
                            message = "Out Of Memory error";
                            break;
                        case UNKNOWN:                // 原因不明
                            message = "Unknown error";
                            break;
                    }
                    Toast.makeText(getActivity(), message, Toast.LENGTH_SHORT).show();

                    spinner.setVisibility(View.GONE);
                }

                @Override
                public void onLoadingComplete(String imageUri, View view, Bitmap loadedImage) {
                    spinner.setVisibility(View.GONE);        // 不显示圆形进度条
                }
            });

            ((ViewPager) view).addView(imageLayout, 0);        // 将图片增加到ViewPager
            return imageLayout;
        }


        @Override
        public boolean isViewFromObject(View view, Object object) {
            return view.equals(object);
        }

        @Override
        public void restoreState(Parcelable state, ClassLoader loader) {
        }

        @Override
        public Parcelable saveState() {
            return null;
        }

        @Override
        public void startUpdate(View container) {}
    }



}
