package gbpe.policeass.utils;

import android.content.Context;
import android.media.AudioManager;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/24 17:26
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/24 17:26
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class AudioUtil {

    private AudioManager mAudioManager;
    private static AudioUtil mInstance;

    private AudioUtil(Context context){
        mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
    }

    public synchronized static AudioUtil getInstance(Context context){
        if(mInstance == null){
            mInstance = new AudioUtil(context);
        }
        return mInstance;
    }

    //获取多媒体最大音量
    public int getMediaMaxVolume(){
        return mAudioManager.getStreamMaxVolume( AudioManager.STREAM_MUSIC );
    }
    //获取多媒体音量
    public int getMediaVolume(){
        return mAudioManager.getStreamVolume( AudioManager.STREAM_MUSIC );
    }

    //获取通话最大音量
    public int getCallMaxVolume(){
        return mAudioManager.getStreamMaxVolume( AudioManager.STREAM_VOICE_CALL );
    }

    //获取系统音量最大值
    public int getSystemMaxVolume(){

        return mAudioManager.getStreamMaxVolume(AudioManager.STREAM_SYSTEM );
    }
    //获取系统音量
    public int getSystemVolume(){

        return mAudioManager.getStreamVolume(AudioManager.STREAM_SYSTEM );
    }

    //获取提示音量最大值
    public int getAlermMaxVolume(){
        return mAudioManager.getStreamMaxVolume(AudioManager.STREAM_ALARM );
    }

    /**
     * 设置多媒体音量
     * 这里我只写了多媒体和通话的音量调节，其他的只是参数不同，大家可仿照
     */
    public void setMediaVolume(int volume){
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, //音量类型
                volume,
                AudioManager.FLAG_PLAY_SOUND
                        | AudioManager.FLAG_SHOW_UI);
    }

    //设置通话音量
    public void setCallVolume(int volume){
        mAudioManager.setStreamVolume( AudioManager.STREAM_VOICE_CALL,
                volume,
                AudioManager.STREAM_VOICE_CALL);
    }

    // 关闭/打开扬声器播放
    public void setSpeakerStatus(boolean on) {
        if (on) { //扬声器
            mAudioManager.setSpeakerphoneOn(true);
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
        } else {
            // 设置最大音量
            int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL);
            mAudioManager.setStreamVolume(AudioManager.STREAM_VOICE_CALL, max, AudioManager.STREAM_VOICE_CALL);
            // 设置成听筒模式
            mAudioManager.setMode(AudioManager.MODE_IN_COMMUNICATION);
            mAudioManager.setSpeakerphoneOn(false);// 关闭扬声器
            mAudioManager.setRouting(AudioManager.MODE_NORMAL, AudioManager.ROUTE_EARPIECE, AudioManager.ROUTE_ALL);
        }
    }
}