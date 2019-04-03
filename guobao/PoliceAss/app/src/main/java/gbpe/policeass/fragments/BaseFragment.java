package gbpe.policeass.fragments;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;


//import gbpe.policeass.zfysim.HomeActivity;
//import gbpe.policeass.zfysim.Main2Activity;
import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.zfysim.NewHomeActivity;


/**
 * Author ： Caojin on 2016/8/25.
 */
public abstract class BaseFragment extends Fragment {
    protected InputMethodManager inputMethodManager;
    protected BaseActivity mBaseActivity;
    protected Bundle mBundle;
    public Context mContext;
    public FragmentManager manager;
    public FragmentTransaction ft;
    //public HomeActivity activity;
    //public MainActivity activity;
    public NewHomeActivity activity;

     protected BackHandledInterface mBackHandledInterface;
    public boolean isBack = false;

    public abstract boolean onKeyUp(int keyCode, KeyEvent event);

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBundle = getArguments();
        if (mBundle == null) {
            mBundle = new Bundle();
        }
//        if(!(getActivity() instanceof BackHandledInterface)){
//            throw new ClassCastException("Hosting Activity must implement BackHandledInterface");
//        }else{
//            this.mBackHandledInterface = (BackHandledInterface)getActivity();
//        }
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mContext = getActivity().getApplicationContext();
        manager = getActivity().getSupportFragmentManager();
        inputMethodManager = (InputMethodManager) getActivity().
                getSystemService(Context.INPUT_METHOD_SERVICE);
        mBaseActivity = (BaseActivity) getActivity();
      // activity = (MainActivity)getActivity();
      // activity = (HomeActivity) getActivity();
        activity = (NewHomeActivity)getActivity();
        initView(view);
        setListener();

    }

    protected void setListener() {}

    protected void initView(View view) {}


    /**
     * 返回键处理
     *
     * @return
     */

    protected abstract boolean onBackPressed();

    /**
     * 关闭Framgment
     */
    public void popSelf() {
        popBackStack();
   //     hideSoftKeyboard();
    }

    public void popSelf(String tag) {
        try {
            if (isDetached() || isRemoving() || getFragmentManager() == null) {
                return;
            }
            if (isResumed()) {
                getFragmentManager().popBackStackImmediate(tag,
                        FragmentManager.POP_BACK_STACK_INCLUSIVE);
            }
            hideSoftKeyboard();
        } catch (Exception e) {
          //  AppLog.instance().e(e.getMessage());
        }

    }

    private void popBackStack() {
        try {
            if (isDetached() || isRemoving() || getFragmentManager() == null) {
                return;
            }
            if (isResumed()) {
                getFragmentManager().popBackStackImmediate();
            }
        } catch (Exception e) {
          //  AppLog.instance().e(e.getMessage());
        }
    }
    protected void hideSoftKeyboard() {
        if (getActivity().getWindow().getAttributes().softInputMode
                != WindowManager.LayoutParams.SOFT_INPUT_STATE_HIDDEN) {
            if (getActivity().getCurrentFocus() != null)
                if (inputMethodManager != null) {
                    inputMethodManager.hideSoftInputFromWindow(getActivity().getCurrentFocus().getWindowToken(),
                            InputMethodManager.HIDE_NOT_ALWAYS);
                }
        }
    }


    /**
     * 所有继承BackHandledFragment的子类都将在这个方法中实现物理Back键按下后的逻辑
     * FragmentActivity捕捉到物理返回键点击事件后会首先询问Fragment是否消费该事件
     * 如果没有Fragment消息时FragmentActivity自己才会消费该事件
     */

    @Override
    public void onStart() {
        super.onStart();
        //告诉FragmentActivity，当前Fragment在栈顶
     //   mBackHandledInterface.setSelectedFragment(this);
    }
    public interface BackHandledInterface {
        public abstract void setSelectedFragment(BaseFragment selectedFragment);
    }

    @Override
    public void onResume() {
        super.onResume();
        isBack = true;
    }

    /**
     * 标记重点文件的diaolog
     */
     /*
    public void showMarkImpFileDialog(){
        View view = View.inflate(activity,R.layout.dialog_mark_file,null);
        TextView tv_dialog = (TextView) view.findViewById(R.id.tv_dialog);
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        final AlertDialog mDialog = builder.create();

        mDialog.setView(view, 0, 0, 0, 0);
        mDialog.show();
        tv_dialog.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(activity, "标记完成", Toast.LENGTH_SHORT).show();
                mDialog.dismiss();
            }
        });
    }*/




}
