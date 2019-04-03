#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#include <linux/usbdevice_fs.h>



#define  LOG_TAG    "WebCamJNI"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define ERROR_LOCAL -1
#define SUCCESS_LOCAL 0

struct buffer {
        void *                  start;
        size_t                  length;
};

#pragma pack(4)
typedef struct _camera {
	int fd;
	unsigned int n_buffers;
	int img_width;
	int img_height;
	int *rgb;
	int *ybuf;
	char *yuv420;
	char *yuyv;
	char *mjpeg;
	struct buffer * buffers;
}camera;

//camera cam0 = {.id = 0; .img_width = 640; .img_height = 480};
//camera cam1 = {.id = 1; .img_width = 640; .img_height = 480};
static camera gcam[2];

int errnoexit(const char *s);

int xioctl(int fd, int request, void *arg);

//int checkCamerabase(void);
int opendevice(int camid, int videobase);
int initdevice(int camid);
int initmmap(int camid);
int startcapturing(int camid);

int readframeonce(int camid);
int readframe(int camid);
void processimage (const void *p, int camid);

int stopcapturing(int camid);
int uninitdevice(int camid);
int closedevice(int camid);

void yuyv422toABGRY(unsigned char *src, int camid);

jint Java_com_camera_usbwebcam_CameraPreview_prepareCamera( JNIEnv* env,jobject thiz, jint camid, jint videobase);
jint Java_com_camera_usbwebcam_CameraPreview_prepareCameraWithBase( JNIEnv* env,jobject thiz, jint camid, jint videobase);
void Java_com_camera_usbwebcam_CameraPreview_processCamera( JNIEnv* env,jobject thiz, jint camid);
void Java_com_camera_usbwebcam_CameraPreview_stopCamera(JNIEnv* env,jobject thiz, jint camid);
void Java_com_camera_usbwebcam_CameraPreview_pixeltobmp( JNIEnv* env,jobject thiz,jobject bitmap,jint camid);    
jbyteArray JNICALL Java_com_camera_usbwebcam_CameraPreview_getyuv( JNIEnv* env,jobject thiz, jint camid);                                                  
void Java_com_camera_usbwebcam_CameraPreview_setimgsize( JNIEnv* env,jobject thiz, jint camid, jint width, jint height);

