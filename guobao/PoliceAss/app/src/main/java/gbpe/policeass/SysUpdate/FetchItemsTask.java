package gbpe.policeass.SysUpdate;

import android.os.AsyncTask;

import java.io.IOException;

import gbpe.policeass.utils.AppLog;

public class FetchItemsTask extends AsyncTask<Void,Void,Void> {

    private static final String TAG = "FetchItemsTask";

    private String ipaddr;
    private int port;
    private String romid;

    public FetchItemsTask(String ipaddr, int port, String romid) {
        this.ipaddr = ipaddr;
        this.port = port;
        this.romid = romid;
    }

    @Override
    protected Void doInBackground(Void... voids) {

        new OtaPackageFetchr().fetchItems(ipaddr,port,romid);
        return null;
    }
}
