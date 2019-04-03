package gbpe.policeass.views;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.VideoView;

/**
 * Created by Administrator on 2016/12/24.
 */
public class MyVideoView extends VideoView {

        public MyVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
            super(context, attrs, defStyleAttr);
            // TODO Auto-generated constructor stub
        }


        public MyVideoView(Context context, AttributeSet attrs) {
            super(context, attrs);
            // TODO Auto-generated constructor stub
        }

        public MyVideoView(Context context) {
            super(context);
            // TODO Auto-generated constructor stub
        }
        @Override
        protected void onMeasure(int widthMeasureSpec,int heightMeasureSpec){
            int width = getDefaultSize(0, widthMeasureSpec);
            int height = getDefaultSize(0, heightMeasureSpec);
            setMeasuredDimension(height, width);
        }

}
