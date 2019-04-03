package gbpe.policeass.views.progressBar;

import android.app.Dialog;
import android.content.Context;
import android.support.annotation.NonNull;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import gbpe.policeass.R;

public class IdRegInfoDlg extends Dialog {
    private TextView mName;
    private TextView mId;
    private TextView mAddr;
    private TextView mTitle;

    public IdRegInfoDlg(@NonNull Context context) {
        super(context, R.style.CustomDialog);
        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_id_recognizer,null);
        mName = view.findViewById(R.id.id_dlg_name);
        mId = view.findViewById(R.id.id_dlg_id);
        mAddr = view.findViewById(R.id.id_dlg_addr);
        mTitle = view.findViewById(R.id.id_dlg_title);
    }

    public void setIdInfo(String title, String name, String id, String addr) {
        if (title!=null) {
            mTitle.setText(title);
        }
        if (id!=null) {
            mId.setText(id);
        }
        if (name!=null) {
            mName.setText(name);
        }
    }


}
