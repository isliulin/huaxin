package gbpe.policeass.utils;

import android.content.Context;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import gbpe.policeass.R;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/4/27 11:33
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/4/27 11:33
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class AnimUtils {
    public static void  PopInAinm(Context context, RelativeLayout layout){
        Animation animation = AnimationUtils.loadAnimation(context, R.anim.bottom_bar_anim);
        layout.setAnimation(animation);
        layout.startAnimation(animation);
        layout.setVisibility(View.VISIBLE);

    }

    public static void  PopOutAinm(Context context, final RelativeLayout layout){
        Animation animation = AnimationUtils.loadAnimation(context, R.anim.bottom_bar_anim2);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                layout.setVisibility(View.GONE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
        layout.setAnimation(animation);
        layout.startAnimation(animation);

    }


        public static void pushIn(LinearLayout layout, Context context){
            Animation animation = AnimationUtils.loadAnimation(context,R.anim.actionsheet_dialog_in);
            layout.setAnimation(animation);
            layout.startAnimation(animation);
            layout.setVisibility(View.VISIBLE);

        }
        public static void pushOut(LinearLayout layout, Context context){
            Animation animation = AnimationUtils.loadAnimation(context,R.anim.actionsheet_dialog_out);
            layout.setAnimation(animation);
            layout.startAnimation(animation);
            layout.setVisibility(View.GONE);
        }

    }
