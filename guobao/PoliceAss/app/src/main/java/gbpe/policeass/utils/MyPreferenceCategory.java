package gbpe.policeass.utils;

import android.content.Context;
import android.graphics.Color;
import android.preference.PreferenceCategory;
import android.util.AttributeSet;
import android.view.View;
import android.widget.TextView;

/**
 * Created by Administrator on 2017/3/16.
 */
public class MyPreferenceCategory extends PreferenceCategory {
    public MyPreferenceCategory(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        //view.setBackgroundColor(Color.parseColor("#ffffff"));
        if (view instanceof TextView) {
            TextView tv = (TextView) view;
            tv.setTextSize(12);
            tv.setTextColor(Color.parseColor("#17a05e"));
        }
    }
}