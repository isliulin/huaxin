package gbpe.policeass.utils;

import java.util.HashMap;
import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;

import gbpe.policeass.Setup.Setup;

public class SoundPlay {

	// 音效的音量
	int streamVolume;

	// 定义SoundPool 对象
	private SoundPool soundPool;

	// 定义HASH表
	private HashMap<Integer, Integer> soundPoolMap;

	/***************************************************************
	 * Function: initSounds(); Parameters: null Returns: None. Description:
	 * 初始化声音系统 Notes: none.
	 ***************************************************************/
	@SuppressLint("UseSparseArrays")
	public void initSounds(Context context) {
		// 初始化soundPool 对象,第一个参数是允许有多少个声音流同时播放,第2个参数是声音类型,第三个参数是声音的品质
		soundPool = new SoundPool(3, AudioManager.STREAM_MUSIC, 100);

		// 初始化HASH表
		soundPoolMap = new HashMap<Integer, Integer>();

		// 获得声音设备和设备音量
		AudioManager mgr = (AudioManager) context
				.getSystemService(Context.AUDIO_SERVICE);
		streamVolume = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
	}

	/**
	 * 把资源中的音效加载到指定的ID(播放的时候就对应到这个ID播放就行了)
	 * Function: loadSfx(); Parameters: null Returns: None. Description: 加载音效资源
	 * Notes: none.
	 */
	public void loadSfx(Context context, int raw, int ID) {
		soundPoolMap.put(ID, soundPool.load(context, raw, 1));
	}

	/***************************************************************
	 * Function: play(); Parameters: sound:要播放的音效的ID, loop:循环次数 Returns: None.
	 * Description: 播放声音 Notes: none.
	 ***************************************************************/
	public void play(int sound, int uLoop) {
//		第二个参数为左声道音量;第三个参数为右声道音量;第四个参数为优先级；第五个参数为循环次数，0不循环，-1循环;第六个参数为速率，速率最低0.5最高为2，1代表正常速度
		soundPool.play(soundPoolMap.get(sound), streamVolume, streamVolume, 1,
				uLoop, 1f);
	}


	/**
	 * 停止播放音频文件
	 * @param id
     */
	public void stop(int id){
		soundPool.stop(id);
	}

	public void Reserve(){


	}

}
