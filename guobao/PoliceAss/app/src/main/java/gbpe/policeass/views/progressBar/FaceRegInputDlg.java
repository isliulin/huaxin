package gbpe.policeass.views.progressBar;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import gbpe.policeass.R;

public class FaceRegInputDlg extends Dialog {

    private EditText mName;
    private EditText mId;
    private EditText mRemark;
    private Button btnPositive, btnNegative;
    private ImageView mPhotoView;
    private Bitmap mImg;
    private TextView mTitle;

    public FaceRegInputDlg(Context context, Bitmap img) {
        super(context, R.style.CustomDialog);
        setFaceRegInputDlg();
        mImg = img;
    }

    private void setFaceRegInputDlg() {
        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_face_registor,null);
        mName = view.findViewById(R.id.register_name);
        mId = view.findViewById(R.id.register_id);
        mRemark = view.findViewById(R.id.register_remark);
        btnPositive = view.findViewById(R.id.btn_positive);
        btnNegative = view.findViewById(R.id.btn_negative);
        mPhotoView = view.findViewById(R.id.face_register_image);
        mTitle = view.findViewById(R.id.arc_face_title);
        super.setContentView(view);
    }

    public void setFaceInfo(String title, String name, String id, String remark) {
        if (title != null) {
            mTitle.setText(title);
        }
        if (id != null) {
            mId.setText(id);
        }
        if (remark != null) {
            mRemark.setText(remark);
        }
        if (name != null) {
            mName.setText(name);
        }

    }

    public void setEditDisable() {
        mName.setFocusable(false);
        mId.setFocusable(false);
        mRemark.setFocusable(false);
		mName.setFocusableInTouchMode(false);
		mId.setFocusableInTouchMode(false);
		mRemark.setFocusableInTouchMode(false);

    }

    @Override
    public void setContentView(int layoutResID) {
    }

    @Override
    public void setContentView(@NonNull View view) {
    }

    @Override
    public void setContentView(@NonNull View view, @Nullable ViewGroup.LayoutParams params) {
    }

    public void setOnPositiveListener(View.OnClickListener listener) {
        btnPositive.setOnClickListener(listener);
    }

    public void setOnNegativeListener(View.OnClickListener listener) {
        btnNegative.setOnClickListener(listener);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPhotoView.setImageBitmap(mImg);
    }

    public String getName(){
        return mName.getText().toString().trim();
    }

    public String getId() {
        return mId.getText().toString().trim();
    }

    public String getRemark() {
        return mRemark.getText().toString().trim();
    }
}
