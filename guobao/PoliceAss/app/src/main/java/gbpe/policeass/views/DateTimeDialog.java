package gbpe.policeass.views;

/**
 * Created by 进 on 2016/11/27.
 */

import android.annotation.SuppressLint;
import android.app.Dialog;
import android.content.Context;
import android.support.annotation.NonNull;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.TextView;

import gbpe.policeass.R;


/**
 * @Function: 自定义对话框
 * @Date: 2016-11-28
 * @Time: 下午12:37:43
 * @author  Jin
 */
public class DateTimeDialog<T extends Object> extends Dialog {
    private EditText editText;
    private Button positiveButton, negativeButton;
    private TextView title;
    private T picker;

    public DateTimeDialog(Context context,int layout) {
      // super(context,R.style.CustomDialog);
        super(context);
        this.setCancelable(true);
        setCustomDialog(layout);
    }

    @SuppressLint("WrongViewCast")
    private void setCustomDialog(int layout) {
        View mView = LayoutInflater.from(getContext()).inflate(layout, null);
//        title = (TextView) mView.findViewById(R.id.title);
//        editText = (EditText) mView.findViewById(R.id.number);
        positiveButton =  mView.findViewById(R.id.tv_cancle);
        negativeButton =  mView.findViewById(R.id.tv_confim);
        if (layout ==R.layout.date){
            picker = (T) mView.findViewById(R.id.datePicker);
            setTitle("设置日期");
        }else {
            picker = (T)mView.findViewById(R.id.timePicker);
            setTitle("设置时间");
        }
        super.setContentView(mView);
    }

    @Override
    public boolean onKeyUp(int keyCode, @NonNull KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_HOME:
                positiveButton.callOnClick();
                break;

            case KeyEvent.KEYCODE_BACK:
                negativeButton.callOnClick();
                break;

        }
        return super.onKeyUp(keyCode, event);
    }

    public View getEditText(){
        return editText;
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
    public   T getPicker(){
        return picker;
    }

    /**
     * 确定键监听器
     * @param listener
     */
    public void setOnPositiveListener(View.OnClickListener listener){
        positiveButton.setOnClickListener(listener);
    }
    /**
     * 取消键监听器
     * @param listener
     */
    public void setOnNegativeListener(View.OnClickListener listener){
        negativeButton.setOnClickListener(listener);
    }
}