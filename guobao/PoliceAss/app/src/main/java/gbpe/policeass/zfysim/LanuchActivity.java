package gbpe.policeass.zfysim;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import gbpe.policeass.Activity.MainActivity;
import gbpe.policeass.R;
import gbpe.policeass.Setup.Setup;

public class LanuchActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lanuch);


    }

    @Override
    protected void onResume() {
        super.onResume();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if (!Setup.isLanuched){
                        Setup.isLanuched = true;
                        Thread.sleep(1000);
                    }
                    Intent intent = new Intent(LanuchActivity.this,MainActivity.class);
                    startActivity(intent);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

            }
        }).start();
    }
}
