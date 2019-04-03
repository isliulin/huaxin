
package gbpe.policeass.zfysim;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.bumptech.glide.Glide;

import java.io.File;
import java.util.ArrayList;

import gbpe.policeass.R;
import gbpe.policeass.views.HackyViewPager;
import uk.co.senab.photoview.PhotoView;

/**
 * 查看大图的Activity界面。
 * 
 * @author guolin
 */
public class ImageDetailsActivity extends Activity implements
		OnPageChangeListener {

	/**
	 * 用于管理图片的滑动
	 */
	private HackyViewPager viewPager;

	/**
	 * 显示当前图片的页数
	 */
	private TextView pageText;
	private ArrayList<String> imageUrls;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		//     取消状态栏
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.image_details);
		int imagePosition = getIntent().getIntExtra("image_position", 0);
		imageUrls = getIntent().getStringArrayListExtra("img_paths");

		pageText = (TextView) findViewById(R.id.page_text);
		viewPager = (HackyViewPager) findViewById(R.id.view_pager);
		ViewPagerAdapter adapter = new ViewPagerAdapter();
		viewPager.setAdapter(adapter);
		viewPager.setCurrentItem(imagePosition);
		viewPager.setOnPageChangeListener(this);
		viewPager.setEnabled(false);
		// 设定当前的页数和总页数
		pageText.setText((imagePosition + 1) + "/" + imageUrls.size());
	}

	/**
	 * ViewPager的适配器
	 * 
	 * @author guolin
	 */
	class ViewPagerAdapter extends PagerAdapter {

		@Override
		public Object instantiateItem(ViewGroup container, int position) {

			View view = LayoutInflater.from(ImageDetailsActivity.this).inflate(
					R.layout.zoom_image_layout, null);
			PhotoView zoomImageView = (PhotoView) view
					.findViewById(R.id.zoom_image_view);
			/* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
			Glide
					.with( ImageDetailsActivity.this)
					.load( Uri.fromFile( new File(imageUrls.get(position))) )
					.placeholder(R.mipmap.tm)
					.error(R.mipmap.empty_photo)
					.into( zoomImageView );


			container.addView(view);

			return view;
		}

		@Override
		public int getCount() {
			return imageUrls.size();
		}

		@Override
		public boolean isViewFromObject(View arg0, Object arg1) {
			return arg0 == arg1;
		}

		@Override
		public void destroyItem(ViewGroup container, int position, Object object) {
			View view = (View) object;
			container.removeView(view);
		}

	}



	@Override
	public void onPageScrollStateChanged(int arg0) {

	}

	@Override
	public void onPageScrolled(int arg0, float arg1, int arg2) {

	}

	@Override
	public void onPageSelected(int currentPage) {
		// 每当页数发生改变时重新设定一遍当前的页数和总页数
		pageText.setText((currentPage + 1) + "/" + imageUrls.size());
	}

}