package gbpe.policeass.views;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;

import gbpe.policeass.R;
import gbpe.policeass.utils.SimUtil;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/5/19 08:51
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/5/19 08:51
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class MemoryView extends ImageView {
    public MemoryView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MemoryView(Context context) {
        super(context);
    }

    public MemoryView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public void setImgSrc(){
        int resId = 0;

       if (SimUtil.hasSimCard(getContext())){
	   		//modified by ycb
            //resId = R.mipmap.nc1;
            resId = R.mipmap.ic_call_signal_strength_0;
        } else {
            resId = R.mipmap.nc;
        }
        setImageResource(resId);
    }

	public void setSignalStrength(int level)
    {
    	int resId = 0;
		if (level < 1)
		{
			resId = R.mipmap.ic_call_signal_strength_0;
		}else if (level < 2)
		{
			resId = R.mipmap.ic_call_signal_strength_1;
		}else if (level < 3)
		{
			resId = R.mipmap.ic_call_signal_strength_2;
		}else
		{
			resId = R.mipmap.ic_call_signal_strength_3;
		}
		setImageResource(resId);
	}
}
