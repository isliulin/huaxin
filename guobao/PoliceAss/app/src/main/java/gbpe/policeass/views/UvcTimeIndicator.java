package gbpe.policeass.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import gbpe.policeass.R;

public class UvcTimeIndicator extends TimeIndicator
{

    //private static ImageView iv_point;
    //private static TextView tv_counter;
    //private static LinearLayout ll_indicator;

    public UvcTimeIndicator(Context context) {
        this(context,null);
    }

    public UvcTimeIndicator(Context context, AttributeSet attrs) {
        this(context,attrs,0);
    }

    public UvcTimeIndicator(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
/*
        LayoutInflater.from(context).inflate(R.layout.indicator,this,true);
        iv_point = (ImageView)findViewById(R.id.iv_counter);
        tv_counter = (TextView) findViewById(R.id.uvc_tc_time);
        ll_indicator = (LinearLayout)findViewById(R.id.rl_indicate);*/
    }

}