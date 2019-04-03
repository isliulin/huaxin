package gbpe.policeass.utils;

import android.content.Context;
import android.speech.tts.TextToSpeech;
import android.widget.Toast;
import java.util.Locale;


/**
 * 语音播报功能,实现
 */
public class TextToSpeechUtil implements TextToSpeech.OnInitListener {

    private TextToSpeech textToSpeech;
    private Context context;

    public TextToSpeechUtil(Context mContext) {
        this.context = mContext.getApplicationContext();
        textToSpeech = new TextToSpeech(context,this);
    }


    /**
     * 用来初始化TextToSpeech引擎
     * status:SUCCESS或ERROR这2个值
     * setLanguage设置语言，帮助文档里面写了有22种
     * TextToSpeech.LANG_MISSING_DATA：表示语言的数据丢失。
     * TextToSpeech.LANG_NOT_SUPPORTED:不支持
     */
    @Override
    public void onInit(int status) {
        if (status == TextToSpeech.SUCCESS) {
            int result = textToSpeech.setLanguage(Locale.CHINA);
            if (result == TextToSpeech.LANG_MISSING_DATA
                    || result == TextToSpeech.LANG_NOT_SUPPORTED) {
                // Toast.makeText(context, "数据丢失或不支持", Toast.LENGTH_SHORT).show();
            }

        }
    }

    /**
     * 语音转文字，
     * @param content
     */
    public void speak(String content){
        if (textToSpeech != null && !textToSpeech.isSpeaking()) {
            textToSpeech.setPitch(0.5f);// 设置音调，值越大声音越尖（女生），值越小则变成男声,1.0是常规
            textToSpeech.speak(content,
                    TextToSpeech.QUEUE_FLUSH, null);
        }
     }

    /**
     * 系统退出后释放资源,在MainActivity
     * 的onDestroy里面释放资源
     */
    public void releaseTextToSpeech(){
        if (textToSpeech !=null){
            textToSpeech.stop();
            textToSpeech.shutdown();
        }
    }

    /**
     * 语音播报
     * @param text
     */
    public void textToSpeech(final String text){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(1000);
                    speak(text);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
}
