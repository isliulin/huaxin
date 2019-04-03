package gbpe.policeass.Photograph.imgdetial;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.support.v4.view.PagerAdapter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import com.bumptech.glide.Glide;
import java.io.File;
import java.util.ArrayList;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import uk.co.senab.photoview.PhotoView;

public class ImgDetialActivity extends AppCompatActivity {
    ArrayList<String>   imgPath;
    public static void startMe(Context context, ArrayList<String> imgPath,int imgPosition){
        Intent intent = new Intent(context,ImgDetialActivity.class);
        intent.putExtra("imgPath",imgPath);
        context.startActivity(intent);
    }

    private JazzyViewPager mJazzy;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_img_detial);
        Intent intent = getIntent();
        imgPath = intent.getStringArrayListExtra("imgPath");
        setupJazziness(JazzyViewPager.TransitionEffect.Tablet);
     }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        menu.add("Toggle Fade");
        String[] effects = this.getResources().getStringArray(
                R.array.jazzy_effects);
        for (String effect : effects)
            menu.add(effect);
        return true;
    }

    /**
     * 根据标题栏设置mJazzy翻页效果
     * @param item
     * @return
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        if (item.getTitle().toString().equals("Toggle Fade"))
        {
            mJazzy.setFadeEnabled(!mJazzy.getFadeEnabled());
        } else
        {
            JazzyViewPager.TransitionEffect effect = JazzyViewPager.
                    TransitionEffect.valueOf(item.getTitle()
                    .toString());
            setupJazziness(effect);
        }
        return true;
    }

    /**
     * 设置界面切换效果
     * @param effect
     */
    private void setupJazziness(JazzyViewPager.TransitionEffect effect)
    {
        mJazzy = (JazzyViewPager) findViewById(R.id.jazzy_pager);
        mJazzy.setTransitionEffect(effect);
		mJazzy.setAdapter(new MainAdapter());
		mJazzy.setPageMargin(imgPath.size());//设置最大浏览张数
//		mJazzy.setOutlineEnabled(true);
   //     initData();
    }

    private class MainAdapter extends PagerAdapter
    {
        //初始化数据
        @Override
        public Object instantiateItem(ViewGroup container, final int position)
        {
//			TextView text = new TextView(MainActivity.this);
//			text.setGravity(Gravity.CENTER);
//			text.setTextSize(30);
//			text.setTextColor(Color.WHITE);
//			text.setText("Page " + position);
//			text.setPadding(30, 30, 30, 30);

            PhotoView iv = new PhotoView(ImgDetialActivity.this);
            //随机获得颜色
//            int bg = Color.rgb((int) Math.floor(Math.random() * 128) + 64,
//                    (int) Math.floor(Math.random() * 128) + 64,
//                    (int) Math.floor(Math.random() * 128) + 64);
            //设置背景为黑色
            iv.setBackgroundColor(getResources().getColor(R.color.dark));

            /* modified by ycb, 20181128, 与 Arcface 用的Glide库冲突*/
            Glide
                    .with(ImgDetialActivity.this)
                    .load( Uri.fromFile( new File( imgPath.get(position) ) ) )
                    .placeholder(R.drawable.ic_stub)
                    .error(R.drawable.videoerror)
                    .into( iv);
             container.addView(iv, ViewGroup.LayoutParams.WRAP_CONTENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
            mJazzy.setObjectForPosition(iv, position);
            return iv;
        }

        @Override
        public void destroyItem(ViewGroup container, int position, Object obj)
        {
            container.removeView(mJazzy.findViewFromObject(position));
        }

        //item的个数
        @Override
        public int getCount()
        {
            return imgPath.size();
        }

        @Override
        public boolean isViewFromObject(View view, Object obj)
        {
            if (view instanceof OutlineContainer)
            {
                return ((OutlineContainer) view).getChildAt(0) == obj;
            } else
            {
                return view == obj;
            }
        }
    }
//    private void initData()
//    {
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//                        mJazzy.setAdapter(new MainAdapter());
//                        mJazzy.setPageMargin(imgInfo.size());
//                    }
//                });
//            }
//        }).start();
//    }
}
