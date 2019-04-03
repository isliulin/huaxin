package gbpe.policeass.views;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import gbpe.policeass.R;


public class PreferenceWithTip extends Preference {
    private static final String TAG = "PreferenceWithTip";
    String pTitle = null;
    String tipstring = null;
    public TextView pTitleView = null;
    public TextView pTipView = null;
    private ViewBindListener viewBindListener = null;

    public interface ViewBindListener{
        void onViewBinded();
    }

    public void setListener(ViewBindListener listener){
        viewBindListener = listener;
    }
    
    @SuppressLint("Recycle")
    public PreferenceWithTip(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        Log.i(TAG,"PreferenceWithTip invoked");
        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.PreferenceWithTip);
        tipstring = ta.getString(R.styleable.PreferenceWithTip_tipstring);
        pTitle = ta.getString(R.styleable.PreferenceWithTip_titlestring);
        ta.recycle();
        Exception e = new Exception("");
        e.printStackTrace();
    }

    public PreferenceWithTip(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        pTitleView = (TextView)view.findViewById(R.id.prefs_title);
        pTitleView.setText(pTitle);
        pTipView = (TextView)view.findViewById(R.id.prefs_tip);
        pTipView.setText(tipstring);
        if (viewBindListener != null) {
            viewBindListener.onViewBinded();
        }
    }


    @Override
    protected View onCreateView(ViewGroup parent) {
        super.onCreateView(parent);
        return LayoutInflater.from(getContext()).inflate(R.layout.preferencewithtip,
                parent, false);
    }
    

}