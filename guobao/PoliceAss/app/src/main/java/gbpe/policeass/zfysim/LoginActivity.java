package gbpe.policeass.zfysim;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import gbpe.policeass.R;

public class LoginActivity extends Activity {
    EditText editText;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        editText = (EditText)findViewById(R.id.et);
        findViewById(R.id.btn_confiom).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (editText.getText().toString().trim().equals("111111")){
                    Intent intent1 = new Intent();
                    intent1.setAction("guobao.intent.action.E12_SET_MSDC");
                    intent1.putExtra("guobao", "E12_SET_MSDC");
                    intent1.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    sendBroadcast(intent1);
                    LoginActivity.this.finish();
                }
            }
        });
    }

//    @Override
//    public void onBackPressed() {
//        return;
//    }
}
