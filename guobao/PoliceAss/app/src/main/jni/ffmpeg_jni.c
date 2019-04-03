/**
  description:基于ffmpeg的视频水印添加
 *
 */

#include <string.h>
 #include <ffmpeg.h>
#include "gbpe_policeass_utils_VideoHandleUtils.h"
#ifdef ANDROID
 #include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  LOGE("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  LOGE("(^_^) " format "\n", ##__VA_ARGS__)
#endif


int ffmpegmain(int argc, char **argv);

//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char* fmt, va_list vl){

	//To TXT file

	FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
	if(fp){
		vfprintf(fp,fmt,vl);
		fflush(fp);
		fclose(fp);
	}
	//To Logcat
	//LOGE(fmt, vl);
}

JNIEXPORT jint JNICALL Java_gbpe_policeass_utils_VideoHandleUtils_ffmpegcore
( JNIEnv * env, jobject thiz, jint cmdnum, jobjectArray cmdline)
{
  //FFmpeg av_log() callback
  av_log_set_callback(custom_log);

  int argc=cmdnum;
  //动态分配内存(为字符串)
  char** argv=(char**)malloc(sizeof(char*)*argc);
  int i=0;
  for(i=0;i<argc;i++)
  {
  //获得jstring对象
    jstring string=(*env)->GetObjectArrayElement(env,cmdline,i);
    //获得jstring对象所代表的 c下面的字符串。
    const char* tmp=(*env)->GetStringUTFChars(env,string,0);
   //动态为argv分配内存空间。（读取每一个命令参数将其装入到argv数组中。）
      argv[i]=(char*)malloc(sizeof(char)*1024);
     //将输入的命令行参数保存在argv[]中
    strcpy(argv[i],tmp);

  }
 // LOGE("开始添加水印.");
  ffmpegmain(argc,argv);
  //LOGE("水印添加完成.");
//释放字符串占用的内存
  for(i=0;i<argc;i++){
    free(argv[i]);
  }
  free(argv);
  return 0;
}

char*   Jstring2CStr(JNIEnv*   env,   jstring   jstr)
{
	 char*   rtn   =   NULL;
	 jclass   clsstring   =   (*env)->FindClass(env,"java/lang/String");
	 jstring   strencode   =   (*env)->NewStringUTF(env,"GB2312");
	 jmethodID   mid   =   (*env)->GetMethodID(env,clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
	 jbyteArray   barr=   (jbyteArray)(*env)->CallObjectMethod(env,jstr,mid,strencode); // String .getByte("GB2312");
	 jsize   alen   =   (*env)->GetArrayLength(env,barr);
	 jbyte*   ba   =   (*env)->GetByteArrayElements(env,barr,JNI_FALSE);
	 if(alen   >   0)
	 {
	  rtn   =   (char*)malloc(alen+1);         //"\0"
	  memcpy(rtn,ba,alen);
	  rtn[alen]=0;
	 }
	 (*env)->ReleaseByteArrayElements(env,barr,ba,0);  //
	 return rtn;
}


