package gbpe.policeass.views;

/**
 * Created by 进 on 2016/11/27.
 */

import android.annotation.SuppressLint;
import android.app.Dialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.TextView;

import gbpe.policeass.R;
import gbpe.policeass.uploadfile.uploadprogressbar.FlikerProgressBar;


/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/5/15 16:09
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/5/15 16:09
 * 
 * 描述：自定义是否上传服务器或是否标记为重点文件对话框
 * =====================================================================================
 */
public class HandleDialog extends Dialog {
     private Button positiveButton, negativeButton;
    private TextView title,tv_desc,tv_progress,tv_rate;
    private static int max;



    private FlikerProgressBar bar;

    public HandleDialog(Context context) {
       super(context, R.style.DialogTheme);
        setCustomDialog();
        setCancelable(false);
    }

    private void setCustomDialog() {
        View mView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_layout, null);
         title = (TextView) mView.findViewById(R.id.tvTitle);
         tv_desc = (TextView) mView.findViewById(R.id.tv_desc);
         tv_progress = (TextView) mView.findViewById(R.id.tv_progress);
         tv_rate = (TextView) mView.findViewById(R.id.tv_rate);
         bar = (FlikerProgressBar)mView.findViewById(R.id.myProssBarhandle) ;
         positiveButton = (Button) mView.findViewById(R.id.f_quchecbutton_btn_queding);
         negativeButton = (Button) mView.findViewById(R.id.f_quchecbutton_btn_quxiao);
         super.setContentView(mView);
    }


    @Override
    public void setContentView(int layoutResID) {
    }

    @Override
    public void setContentView(View view, LayoutParams params) {
    }

    @Override
    public void setContentView(View view) {
    }

    public HandleDialog setProgressBarMax(int max){
        //bar.setMax(max);
        bar.MAX_PROGRESS = max;
        this.max = max;
        setProgressRate(0);
         return this;
    }
    /**
     * 确定键监听器
     * @param listener
     */
    public HandleDialog setOnPositiveListener(View.OnClickListener listener){
        positiveButton.setOnClickListener(listener);
        return this;
    }
    /**
     * 取消键监听器
     * @param listener
     */
    public HandleDialog setOnNegativeListener(View.OnClickListener listener){
        negativeButton.setOnClickListener(listener);
        return this;
    }


    public FlikerProgressBar getBar() {
        return bar;
    }

    public HandleDialog setProgressRate(int progress){
        tv_rate.setText(""+progress+"/"+this.max);
         return this;
    }
    public HandleDialog setTitle(String title) {
        this.title .setText(""+title);
        return this;
    }


    public HandleDialog setDesc(String desc) {
        this.tv_desc .setText(""+desc);
        return this;
    }



    @SuppressLint("SetTextI18n")
    public HandleDialog setProgress(int progress) {

         this.tv_progress.setText(((Float.parseFloat(""+progress)/ Float.parseFloat(""+max)))*100+"%");
        this.bar.setProgress(progress);
        setProgressRate(progress);
        return this;
    }



    public HandleDialog setRate(String rate) {
        this.tv_rate.setText(""+rate);
        return this;
    }
}