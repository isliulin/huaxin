package gbpe.policeass.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.widget.Toast;

import gbpe.policeass.R;
import gbpe.policeass.db.FaceItem;
import gbpe.policeass.db.FaceLab;
import gbpe.policeass.receiver.FaceRegAndSearch.FaceServerLisener;
import gbpe.policeass.utils.AppLog;

public class FaceSampleReceiver extends BroadcastReceiver {

    private static final String TAG = "FaceSampleReceiver";
    public static final String FACE_DET_MSG = "gbpe.face.detect.sample";
    public static final String PATH = "sample.path";

    /*public static void nativeRegister(String path, final Context context, FaceServerLisener lisener) {
        AppLog.i (TAG, "nativeRegister");
        //FaceRegAndSearch.startInstance(context, path,lisener);
        FaceRegAndSearch.startRegister(context,path,lisener);
    }*/

    @Override
    public void onReceive(final Context context, Intent intent) {

        String action = intent.getAction();
        if (action.equals(FACE_DET_MSG)){
            final String path = intent.getStringExtra("path");
            AppLog.i (TAG, "receive msg: "+action+", path: "+path);
            //FaceLab faceLab = FaceLab.get(context);
            final Handler rstHandler = new Handler();

            //FaceRegAndSearch.startRegister(context,path, new FaceServerLisener() {
            FaceRegAndSearch.getInstance(context).startRegister(context,path, new FaceServerLisener() {
                @Override
                public void onRegisterFinished(int suc, int failed) {
                    AppLog.e (TAG, "注册成功"+suc+", 失败"+failed);
                }

                @Override
                public void onRegisterError(Exception e) {
                    AppLog.e (TAG, "注册"+path+"失败");
                    rstHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(context,context.getString(R.string.register_failed),Toast.LENGTH_SHORT).show();
                        }
                    });
                }

                @Override
                public void onSingleOk(final FaceItem reg, int cnt) {
                    FaceLab.get(context).addFaceItem(reg);
                    rstHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(context,context.getString(R.string.register)+reg.getName()+
                                    context.getString(R.string.success),Toast.LENGTH_SHORT).show();
                        }
                    });
                }

                @Override
                public void onShowMsg(final String msg) {
                    rstHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(context,msg,Toast.LENGTH_SHORT).show();
                        }
                    });

                }
            });

        }
    }

}
