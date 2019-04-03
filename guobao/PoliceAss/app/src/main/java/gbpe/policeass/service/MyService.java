package gbpe.policeass.service;

/**
 * Created by Administrator on 2017/3/6.
 */
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;


/**
 * service一旦启动会在后台一直运行
 */
public class MyService extends Service {
    private RunAlways runAlways;

    @Override
    public void onCreate() {
        //只要在service中new一个对象，然后start方法
        runAlways = new RunAlways(this);
        runAlways.start();
     }

    @Override
    public void onDestroy() {
        //最后在onDestory中close就行
        runAlways.close();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    /*
    * PNODE add(PNODE pHEAD,int len,int val)
    * {
    *   if(isEmpty(pHEAD)){
    *    return;
    *   }
    *  PNODE pTAIL = pHEAD;
    *  for(int i=0;i<len;i++){
    *  PNODE pNEW = (PNODE)malloc(sizeof(NODE));
    *  pNEW->data = val;
    *  pTAIL->pNext = pNEW;// PNODE pTAIL = pHEAD;两个指针变量都指向了头结点
    *  pNEW->pNext = NULL;
    *  pTAIL = PNEW;
    *  }
    * }
    *
    * */
}
