package gbpe.policeass.SysUpdate;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.v4.media.session.MediaSessionCompat;

public class ThumbnailDownloader extends HandlerThread {
    private static final String TAG = "ThumbnailDownloader";
    private  Handler mResponseHandler;
    Listener<MediaSessionCompat.Token> mListener;

    public interface Listener<Token> {
        void onThumbnailDownLoad(Token token, Bitmap thumbnail);

    }

    public ThumbnailDownloader(Handler responseHandler) {
        super(TAG);
        mResponseHandler = responseHandler;
    }


}
