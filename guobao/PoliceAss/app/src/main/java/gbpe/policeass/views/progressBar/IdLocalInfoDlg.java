package gbpe.policeass.views.progressBar;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import gbpe.policeass.R;

public class IdLocalInfoDlg extends Dialog {

    private TextView mTitle;
    private ImageView imageView;
    private TextView mIdNum;

    private String title;
    private Bitmap pic;
    private String num;


    public IdLocalInfoDlg(@NonNull Context context) {
        super(context);
        View view = LayoutInflater.from(getContext()).inflate(R.layout.id_reg_dlg_local,null);
        mTitle = view.findViewById(R.id.id_local_title);
        imageView = view.findViewById(R.id.id_local_view);
        mIdNum = view.findViewById(R.id.id_local_numbers);
    }

    public void setInfo(Bitmap bitmap, String title, String mIdNum) {
        this.title = title;
        pic = bitmap;
        num = mIdNum;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (pic!=null) {
            imageView.setImageBitmap(pic);
        }
        if (title!=null) {
            mTitle.setText(title);
        }
        if (num!=null) {
            mIdNum.setText(num);
        }
    }
}
