package gbpe.policeass.views;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.MotionEvent;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/28 15:24
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/28 15:24
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class HackyViewPager extends ViewPager {


    public HackyViewPager(Context context) {
        super(context);

    }

    public HackyViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);

    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {

        try {
            return super.onInterceptTouchEvent(ev);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            return false;
        }

    }


}