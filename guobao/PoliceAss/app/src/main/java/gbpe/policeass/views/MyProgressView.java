package gbpe.policeass.views;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import gbpe.policeass.R;
import gbpe.policeass.utils.AppLog;


public class MyProgressView extends View {


    int COLOR_BLUE = 0xff538ede;
    int COLOR_GREY = 0xffd8dbde;

    int width;
    int height;
    //int progressheight = 30;
    int progressheight = 10;
    int textSize = 10;
    int proCicrle = progressheight / 2;   //圆角半径
    int color_bg;
    int color_progress;
    int progress = 0;   //当前的进度
    int speed = 240;     //下载速度
    private static final int SPEED_NULL = -2;  //当前仅仅绘制进度，不绘制速度
    private boolean isDrawSpeed = false;      //是否绘制速度


    public void setProgress(int progress) {
        setProgress(progress, SPEED_NULL);
    }

    public void setProgress(int progress, int speed) {
        isDrawSpeed = false;
        if (speed != SPEED_NULL) {
            isDrawSpeed = true;
        }
        this.speed = speed;
        this.progress = progress;
        invalidate();
    }

    public MyProgressView(Context context) {
        this(context, null);
    }

    public MyProgressView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MyProgressView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        TypedArray ta = getContext().obtainStyledAttributes(attrs, R.styleable.MyProgressView);
        color_progress = ta.getColor(R.styleable.MyProgressView_progresscolor, COLOR_BLUE);
        color_bg = ta.getColor(R.styleable.MyProgressView_progressbg, COLOR_GREY);
        ta.recycle();
        init();
    }

    //通用画笔
    Paint mPaint;
    Path path;

    private void init() {
        path = new Path();
        //初始化进度画笔
        mPaint = new Paint();
        mPaint.setAntiAlias(true);  //抗锯齿
        mPaint.setStyle(Paint.Style.FILL);//充满
        mPaint.setDither(true);// 设置抖动,颜色过渡更均匀
        mPaint.setStrokeCap(Paint.Cap.ROUND);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        width = getWidth();
        height = getHeight();

        drawBg(canvas);  //绘制背景
        drawProgress(canvas);
        drawTextProgress(canvas);  //绘制底色文字
        drawTextSpeed(canvas);
    }

    /**
     * 绘制下载速度
     *
     * @param canvas
     */
    private void drawTextSpeed(Canvas canvas) {
        if (!isDrawSpeed) {
            return;
        }
        mPaint.setColor(color_progress);
        mPaint.setTextSize(textSize);
        String textCanvas = speed + "kb";
        Rect bounds = new Rect();
        mPaint.getTextBounds(textCanvas, 0, textCanvas.length(), bounds);
        int textWith = bounds.width();
        canvas.drawText(textCanvas, width - (textWith * 3 / 2), (height / 2) + (progressheight * 4 / 3), mPaint);
    }


    /***
     * 绘制下载进度
     * @param canvas
     */
    private void drawTextProgress(Canvas canvas) {
        mPaint.setColor(color_progress);
        mPaint.setTextSize(textSize);
        String textCanvas = progress + "%";
        Rect bounds = new Rect();
        mPaint.getTextBounds(textCanvas, 0, textCanvas.length(), bounds);
        int textWith = bounds.width();
        int right = width * progress / 100;
        canvas.drawText(textCanvas, right - textWith / 2, height / 2 - progressheight, mPaint);
    }

    private void drawProgress(Canvas canvas) {
        mPaint.setColor(color_progress);
        int top = height / 2 - progressheight / 2;
        int bottom = height / 2 + progressheight / 2;
        int right = width * progress / 100;
        AppLog.i("main", "right  ==" + right + "  /width =" + width);
        RectF oval3 = new RectF(0, top, right, bottom);// 设置个新的长方形
        canvas.drawRoundRect(oval3, 20, 15, mPaint);//第二个参数是x半径，第三个参数是y半径
    }

    private void drawBg(Canvas canvas) {
        mPaint.setColor(color_bg);
        int top = height / 2 - progressheight / 2;
        int bottom = height / 2 + progressheight / 2;
        RectF rectBg = new RectF(0, top, width, bottom);// 设置个新的长方形
        canvas.drawRoundRect(rectBg, proCicrle, proCicrle, mPaint);//第二个参数是x半径，第三个参数是y半径
    }

}
