package gbpe.policeass.adapters;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.view.ViewGroup;

import java.util.List;

/**
 * Created by Administrator on 2017/3/22.
 */
public class MyFragmentAdapter extends FragmentPagerAdapter {


    private  List<Fragment> list;

    public MyFragmentAdapter(FragmentManager fm, List<Fragment> list) {

        super(fm);

        this.list = list;
     }

    @Override
    public Fragment getItem(int position) {

 //       Fragment fragment =null;
//        switch (position){
//            case 0:
//                fragment =fragment_one;
//                break;
//            case 1:
//                fragment =fragment_two;
//                break;
//            case 2:
//                fragment =fragment_three;
//                break;
//            case 3:
//                fragment =fragment_four;
//                break;
//
//        }
        return list.get(position);
    }

    @Override
    public int getCount() {

        return list.size();
    }

    @Override
    public Object instantiateItem(ViewGroup container, int position) {
        return super.instantiateItem(container, position);
    }

    @Override
    public void destroyItem(ViewGroup container, int position, Object object) {
        super.destroyItem(container, position, object);
    }
}
