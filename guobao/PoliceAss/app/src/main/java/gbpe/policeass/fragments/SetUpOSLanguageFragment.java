package gbpe.policeass.fragments;

import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.Locale;

import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.SPUtils;
import gbpe.policeass.zfysim.WelcomeActivity;


/**
 * Created by Administrator on 2016/9/13.
 */
public class SetUpOSLanguageFragment extends BaseFragment implements View.OnClickListener {

    private Configuration config;
    private DisplayMetrics dm;
    private Resources resources;
    private View view;
    private ImageView ivChinese,ivEnglish;
    private TextView tvEnglish,tvChinese;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.activity_set_oslanguage,container,false);
        view.findViewById(R.id.rl_chinese).setOnClickListener(this);
        view.findViewById(R.id.rl_english).setOnClickListener(this);
       // ivBack = (ImageView) view.findViewById(R.id.image_back);
        ivChinese = (ImageView)view.findViewById(R.id.iv_chinese);
        ivEnglish = (ImageView)view.findViewById(R.id.iv_english);
        tvEnglish = (TextView)view.findViewById(R.id.tv_english);
        tvChinese = (TextView)view.findViewById(R.id.tv_Chinese);
        view.findViewById(R.id.image_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popSelf();
            }
        });


        return view;
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()){
            case R.id.rl_chinese:
                Setup.LanguageZfy = true;
                SPUtils.put(mContext,"LanguageZfy",true);

                break;
            case R.id.rl_english:
                Setup.LanguageZfy =false;
                SPUtils.put(mContext,"LanguageZfy",false);
                break;

            default:break;
        }
        setSysLanguage();
        activity. go(WelcomeActivity.class);
        activity.finish();
    }
    @Override
    public void onResume() {
        super.onResume();
        resources =getResources();
        config = resources.getConfiguration();
        dm = resources.getDisplayMetrics();

        setSysLanguage();
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // tag by ycb, set onKeyUp handle
        return false;
    }

    @Override
    protected boolean onBackPressed() {
        return false;
    }

    private void setSysLanguage()
    {
        if (Setup.LanguageZfy)
        {
            ivChinese.setVisibility(View.VISIBLE);
            ivEnglish.setVisibility(View.INVISIBLE);
            tvChinese.setTextColor(getResources().getColor(R.color.yellow));
            tvEnglish.setTextColor(getResources().getColor(R.color.gray));

            config.locale = Locale.SIMPLIFIED_CHINESE;
        }
        else
        {
            config.locale = Locale.US;
            ivEnglish.setVisibility(View.VISIBLE);
            ivChinese.setVisibility(View.INVISIBLE);
            tvChinese.setTextColor(getResources().getColor(R.color.gray));
            tvEnglish.setTextColor(getResources().getColor(R.color.yellow));
        }
        resources.updateConfiguration(config, dm);

    }
}
