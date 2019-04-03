package gbpe.policeass.Photograph.imgdetial;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Message;
import android.os.Parcelable;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
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
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.R;




/**
 * ViewPager页面显示Activity
 * @author Sergey Tarasevich (nostra13[at]gmail[dot]com)
 */
public class ImgDetialActivity2 extends BaseActivity {

    private static final String STATE_POSITION = "STATE_POSITION";

    DisplayImageOptions options;
    List<String> images;

    ViewPager pager;

    public static void startMe(Context context, int position, ArrayList<String> soundNames) {
        Intent intent = new Intent(context, ImgDetialActivity2.class);
        intent.putExtra("position", position);
        intent.putExtra("list", soundNames);
        context.startActivity(intent);
    }

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_image_pager);
// Uri=content://media/external/images/thumbnails, isVideo=false

        Bundle bundle = getIntent().getExtras();
        images = bundle.getStringArrayList("list");

        //	imageUrls = new String[images.size()];


//		for (int i=0;i<images.size();i++)
//		{
//			imageUrls[i] = images.get(i);
//			System.out.println("---->>>images"+images.get(i));
//		}
        //	String[] imageUrls = bundle.getStringArray(Constants.Extra.IMAGES);

        // 当前显示View的位置
        int pagerPosition = bundle.getInt("position", 0);

        // 如果之前有保存用户数据
        if (savedInstanceState != null) {
            pagerPosition = savedInstanceState.getInt(STATE_POSITION);
        }

        options = new DisplayImageOptions.Builder()
                .showImageForEmptyUri(R.mipmap.ic_empty)
                .showImageOnFail(R.mipmap.ic_error)
                .resetViewBeforeLoading(true)
                .cacheOnDisc(true)
                .imageScaleType(ImageScaleType.EXACTLY)
                .bitmapConfig(Bitmap.Config.RGB_565)
                .displayer(new FadeInBitmapDisplayer(300))
                .build();

        pager = (ViewPager) findViewById(R.id.pager);
        pager.setAdapter(new ImagePagerAdapter(images));
        pager.setCurrentItem(pagerPosition);    // 显示当前位置的View
    }

    @Override
    protected void handler(Message msg) {

    }

    @Override
    protected void initView() {

    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        // 保存用户数据
        outState.putInt(STATE_POSITION, pager.getCurrentItem());
    }

    @Override
    public void onClick(View v) {

    }

    private class ImagePagerAdapter extends PagerAdapter {

        private List<String> images;
        private LayoutInflater inflater;

        ImagePagerAdapter(List<String> images) {
            this.images = images;
            inflater = getLayoutInflater();
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
            System.out.println("----->>>>"+fileName);
            String uri = "";
            if (fileName.contains("I"))
            {
               uri = "file:///mnt/sdcard/DCIM/101ANDRO/" + fileName;//文件夹一定不能写错
            }else
            {
                uri = "file:///mnt/sdcard/DCIM/100ANDRO/" + fileName;//文件夹一定不能写错
            }
           // String uri = "file:///mnt/sdcard/DCIM/" + fileName;//文件夹一定不能写错
            imageLoader.displayImage(uri, imageView, options, new SimpleImageLoadingListener() {
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
                    Toast.makeText(ImgDetialActivity2.this, message, Toast.LENGTH_SHORT).show();

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
