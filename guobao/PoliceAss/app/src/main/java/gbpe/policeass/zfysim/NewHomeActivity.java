package gbpe.policeass.zfysim;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import gbpe.policeass.Activity.BaseActivity;
import gbpe.policeass.Activity.FaceRecogniseActivity;
import gbpe.policeass.fragments.BaseFragment;
import gbpe.policeass.R;
import gbpe.policeass.fragments.NewHomeFragment;
import gbpe.policeass.utils.AppLog;
import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

import android.support.v4.app.FragmentManager;
import android.widget.Toast;

import com.arcsoft.arcfacedemo.common.Constants;
import com.arcsoft.face.ErrorInfo;
import com.arcsoft.face.FaceEngine;

import java.util.List;

public class NewHomeActivity extends BaseActivity
                implements FaceRecogniseActivity.EngineErrorHandle{

    private static final String TAG = "NewHomeActivity";
    private static final int ACTION_REQUEST_PERMISSIONS = 0x001;
	private static final String FACE_ENGINE_ACTIVED = "face_engine_actived";
    //private  NewHomeFragment newHomeFragment;
    /**
     * 所需的所有权限信息
     */
    private static final String[] NEEDED_PERMISSIONS = new String[]{
            Manifest.permission.CAMERA,
            Manifest.permission.READ_PHONE_STATE
    };

    private Fragment createFragment(){
        return new NewHomeFragment();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fragment);

        FragmentManager fragmentManager = getSupportFragmentManager();
        Fragment fragment = fragmentManager.findFragmentById(R.id.fragment_container);
        //newHomeFragment = (NewHomeFragment) fragmentManager.findFragmentById(R.id.fragment_container);

        if (fragment == null){
            fragment = createFragment();
            fragmentManager.beginTransaction()
                    .add(R.id.fragment_container,fragment)
                    .commit();
        }

        if(!getFaceActiveFlag()) {
			// 激活引擎
            activeEngine(null);
            //AppLog.d(TAG,"初次使用，激活人脸引擎");
            //ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
        }
        
        
/*
        if (!checkPermissions(NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
        } else {
            activeEngine(null);
        }*/
    }

    private boolean getFaceActiveFlag(){
        return PreferenceManager.getDefaultSharedPreferences(this).getBoolean(FACE_ENGINE_ACTIVED,false);
    }

    private void setFaceActiveFlag(boolean actived) {
        PreferenceManager.getDefaultSharedPreferences(this)
                .edit()
                .putBoolean(FACE_ENGINE_ACTIVED,actived)
                .commit();
    }

    @Override
    protected void handler(Message msg) {

    }

    @Override
    protected void initView() {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                                WindowManager.LayoutParams.FLAG_FULLSCREEN);

    }

    public void showFragment2(Fragment beHideFragment, Class<? extends BaseFragment>
            beShowedFragment, String fragmentTag, Bundle bundle, boolean addBackStack) {

        FragmentManager manager = getSupportFragmentManager();
        transaction = manager.beginTransaction();
        Fragment fragment = manager.findFragmentByTag(fragmentTag);

        transaction.setCustomAnimations(R.anim.back_left_in, R.anim.back_right_out,
                R.anim.push_left_in, R.anim.push_left_out);

        try {
            fragment = beShowedFragment.newInstance();
            if (bundle != null) {
                fragment.setArguments(bundle);
            }
            if (beHideFragment !=null)
                transaction.hide(beHideFragment).add(android.R.id.content, fragment,
                        fragmentTag);
            else
                transaction.add(android.R.id.content, fragment, fragmentTag);

        } catch (Exception e) {
            AppLog.i (TAG, e.toString());
        }

        if (addBackStack) {
            transaction.addToBackStack(fragmentTag);
        }

        transaction.commit();
    }
    /**
     * @param beShowedFragment
     * @param fragmentTag ： 该fragment的tag
     * @param bundle :
     * @param addBackStack :
     */
    public void showFragment(Class<? extends BaseFragment> beShowedFragment,
                             String fragmentTag, Bundle bundle, boolean addBackStack) {
        FragmentManager manager = getSupportFragmentManager();
        transaction = manager.beginTransaction();
        Fragment fragment = manager.findFragmentByTag(fragmentTag);

        // transaction.setCustomAnimations(R.anim.back_left_in, R.anim.back_right_out,
        // R.anim.push_left_in, R.anim.push_left_out);

        try {
            fragment = beShowedFragment.newInstance();
            if (bundle != null) {
                fragment.setArguments(bundle);
            }
            transaction.add(android.R.id.content, fragment, fragmentTag);
        } catch (Exception e) {

        }

        if (addBackStack) {
            transaction.addToBackStack(fragmentTag);
        }

        transaction.commit();
    }


    public void showFragment3(Class<? extends Fragment> beShowedFragment,
                              String fragmentTag, Bundle bundle, boolean addBackStack) {
        FragmentManager manager = getSupportFragmentManager();
        transaction = manager.beginTransaction();
        Fragment fragment = manager.findFragmentByTag(fragmentTag);

        transaction.setCustomAnimations(R.anim.back_left_in, R.anim.back_right_out,
                R.anim.push_left_in, R.anim.push_left_out);

        try {
            fragment = beShowedFragment.newInstance();
            if (bundle != null) {
                fragment.setArguments(bundle);
            }
            transaction.add(android.R.id.content, fragment, fragmentTag);
        } catch (Exception e) {
            AppLog.i (TAG, e.toString());
        }

        if (addBackStack) {
            transaction.addToBackStack(fragmentTag);
        }

        transaction.commit();
    }
/*
    private boolean isLongPressKey;
    private boolean lockLongPressKey;
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        switch (keyCode){
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (event.getRepeatCount() == 0){
                    event.startTracking();
                    isLongPressKey = false;
                }else {
                    isLongPressKey = true;
                }
                break;
        }
        return super.onKeyDown(keyCode, event);
    }*/

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        switch (keyCode){
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                /*if (!lockLongPressKey) {
                    newHomeFragment.onKeyUp(keyCode, event);
                }
                isLongPressKey = false;
                lockLongPressKey = false;*/
                AppLog.i(TAG,"key up");
                //newHomeFragment.onKeyUp(keyCode, event);
                 BaseFragment baseFragment = (BaseFragment) getVisibleFragment();
                 baseFragment.onKeyUp(keyCode,event);
                break;

        }
        return super.onKeyUp(keyCode, event);
    }

    public Fragment getVisibleFragment(){
        FragmentManager fragmentManager = getSupportFragmentManager();
        List<Fragment> fragments = fragmentManager.getFragments();
        for(Fragment fragment : fragments){
            if(fragment != null && fragment.isVisible())
                return fragment;
        }
        return null;
    }

    private boolean checkPermissions(String[] neededPermissions) {
        if (neededPermissions == null || neededPermissions.length == 0) {
            return true;
        }
        boolean allGranted = true;
        for (String neededPermission : neededPermissions) {
            allGranted &= ContextCompat.checkSelfPermission(this, neededPermission) == PackageManager.PERMISSION_GRANTED;
        }
        return allGranted;
    }

    /**
     *  激活引擎
     **/
    /**
     * 激活引擎
     *
     * @param view
     */
    public void activeEngine(final View view) {
        // android 5.1已经声明了摄像机和电话权限
        /*if (!checkPermissions(NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
            return;
        }*/
        if (view != null) {
            view.setClickable(false);
        }
        Observable.create(new ObservableOnSubscribe<Integer>() {
            @Override
            public void subscribe(ObservableEmitter<Integer> emitter) throws Exception {
                FaceEngine faceEngine = new FaceEngine();
                int activeCode = faceEngine.active(NewHomeActivity.this, Constants.APP_ID, Constants.SDK_KEY);
                emitter.onNext(activeCode);
            }
        })
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Observer<Integer>() {
                    @Override
                    public void onSubscribe(Disposable d) {

                    }

                    @Override
                    public void onNext(Integer activeCode) {
                        if (activeCode == ErrorInfo.MOK) {
                            showToast(getString(R.string.active_success));
                            setFaceActiveFlag(true);
                        } else if (activeCode == ErrorInfo.MERR_ASF_ALREADY_ACTIVATED) {
                            showToast(getString(R.string.already_activated));
                            setFaceActiveFlag(true);
                        } else {
                            showToast(getString(R.string.active_failed, activeCode));
                        }

                        if (view != null) {
                            view.setClickable(true);
                        }
                    }

                    @Override
                    public void onError(Throwable e) {

                    }

                    @Override
                    public void onComplete() {

                    }
                });

    }

    private Toast toast;
    private void showToast(String s) {
        if (toast == null) {
            toast = Toast.makeText(this, s, Toast.LENGTH_SHORT);
            toast.show();
        } else {
            toast.setText(s);
            toast.show();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == ACTION_REQUEST_PERMISSIONS) {
			AppLog.i(TAG,"face 权限获取");
            boolean isAllGranted = true;
            for (int grantResult : grantResults) {
                isAllGranted &= (grantResult == PackageManager.PERMISSION_GRANTED);
            }
            if (isAllGranted) {
				AppLog.d(TAG,"获取权限成功，激活引擎");
				try{
                    activeEngine(null);
                } catch (Exception e) {
				    AppLog.e (TAG,"active engine error: "+e.toString());
                }

            } else {
                Toast.makeText(this, R.string.permission_denied, Toast.LENGTH_SHORT).show();
            }
        }
    }

    @Override
    public void onEngineInitError() {
        setFaceActiveFlag(false);
    }

/*
    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {
        AppLog.i(TAG,"long press: "+keyCode);
        switch (keyCode){
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                newHomeFragment.onKeyUp(keyCode,event);
                lockLongPressKey = true;
                break;
        }

        return super.onKeyLongPress(keyCode, event);
    }
*/
}
