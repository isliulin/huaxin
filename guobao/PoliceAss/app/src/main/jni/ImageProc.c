#include "ImageProc.h"
#include "utils.h"
//01-24 07:43:06.439: E/TEST(3756): VIDIOC_DQBUF error 22, Invalid argument

int errnoexit(const char *s)
{
	LOGE("%s error %d, %s", s, errno, strerror (errno));
	return ERROR_LOCAL;
}

int xioctl(int fd, int request, void *arg)
{
	int r;

	do r = ioctl (fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}


//打开video设备
int opendevice(int camid, int videobase)
{

	struct stat st;
	static char dev_name[16];

	sprintf(dev_name,"/dev/video%d",camid+videobase);
	LOGI(" opendevice %s", dev_name);
	//stat() 获得文件属性，并判断是否为字符设备文件
	if (-1 == stat (dev_name, &st)) {
		LOGE("Cannot identify '%s': %d, %s", dev_name, errno, strerror (errno));
		return ERROR_LOCAL;
	}

	if (!S_ISCHR (st.st_mode)) {
		LOGE("%s is no device", dev_name);
		return ERROR_LOCAL;
	}

	gcam[camid].fd = open (dev_name, O_RDWR);

	if (-1 == gcam[camid].fd) {
		LOGE("Cannot open '%s': %d, %s", dev_name, errno, strerror (errno));
		return ERROR_LOCAL;
	}

	LOGI(" gcam[%d].fd %d", camid, gcam[camid].fd);
	return SUCCESS_LOCAL;
}
//初始化设备
int initdevice(int camid) 
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;
	//VIDIOC_QUERYCAP 命令 来获得当前设备的各个属性
	if (-1 == xioctl (gcam[camid].fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			//LOGE("%s is no V4L2 device", dev_name);
			return ERROR_LOCAL;
		} else {
			return errnoexit ("VIDIOC_QUERYCAP");
		}
	}
	//V4L2_CAP_VIDEO_CAPTURE 0x00000001
	// 这个设备支持 video capture 的接口，即这个设备具备 video capture 的功能
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		//LOGE("%s is no video capture device", dev_name);
		return ERROR_LOCAL;
	}
	//V4L2_CAP_STREAMING 0x04000000
	// 这个设备是否支持 streaming I/O 操作函数
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		//LOGE("%s does not support streaming i/o", dev_name);
		return ERROR_LOCAL;
	}
	//获得设备对 Image Cropping 和 Scaling 的支持
	CLEAR (cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl (gcam[camid].fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; 

		if (-1 == xioctl (gcam[camid].fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
				case EINVAL:
					break;
				default:
					break;
			}
		}
	} else {
	}
	//设置图形格式
	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	fmt.fmt.pix.width       = gcam[camid].img_width; //IMG_WIDTH; 
	fmt.fmt.pix.height      = gcam[camid].img_height; //IMG_HEIGHT;

	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	//fmt.fmt.pix.field       = V4L2_FIELD_ANY;
	
	//检查流权限
	if (-1 == xioctl (gcam[camid].fd, VIDIOC_S_FMT, &fmt))
		return errnoexit ("VIDIOC_S_FMT");

	min = fmt.fmt.pix.width * 2;
	//每行像素所占的 byte 数
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	LOGI("initdevice camid = %d w=%d, h=%d", camid, gcam[camid].img_width, gcam[camid].img_height);
	return initmmap (camid);

}
//I/O模式选择
int initmmap(int camid)
{
	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (gcam[camid].fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			LOGE("%d does not support memory mapping", camid);
			return ERROR_LOCAL;
		} else {
			return errnoexit ("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		LOGE("Insufficient buffer memory on %d", camid);
		return ERROR_LOCAL;
 	}

	gcam[camid].buffers = calloc(req.count, sizeof (*gcam[camid].buffers));

	if (!gcam[camid].buffers) {
		LOGE("Out of memory");
		return ERROR_LOCAL;
	}

	for (gcam[camid].n_buffers = 0; gcam[camid].n_buffers < req.count; ++gcam[camid].n_buffers) {
		struct v4l2_buffer buf;

		 CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = gcam[camid].n_buffers;

		if (-1 == xioctl (gcam[camid].fd, VIDIOC_QUERYBUF, &buf))
			return errnoexit ("VIDIOC_QUERYBUF");

		gcam[camid].buffers[gcam[camid].n_buffers].length = buf.length;
		gcam[camid].buffers[gcam[camid].n_buffers].start =
		mmap (NULL ,
			buf.length,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			gcam[camid].fd, buf.m.offset);

		if (MAP_FAILED == gcam[camid].buffers[gcam[camid].n_buffers].start)
			return errnoexit ("mmap");
	}
	LOGI("initmmap success");
	return SUCCESS_LOCAL;
}

int startcapturing(int camid)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < gcam[camid].n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;

		if (-1 == xioctl (gcam[camid].fd, VIDIOC_QBUF, &buf))
			return errnoexit ("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (gcam[camid].fd, VIDIOC_STREAMON, &type))
		return errnoexit ("VIDIOC_STREAMON");

	LOGI("startcapturing success n_buffers=%d", gcam[camid].n_buffers);

	return SUCCESS_LOCAL;
}

int readframeonce(int camid)
{
	for (;;) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO (&fds);
		FD_SET (gcam[camid].fd, &fds);

		tv.tv_sec = 2;
		tv.tv_usec = 0;

		r = select (gcam[camid].fd + 1, &fds, NULL, NULL, &tv);

		if (-1 == r) {
			if (EINTR == errno)
				continue;

			return errnoexit ("select");
		}

		if (0 == r) {
			LOGE("select timeout");
			return ERROR_LOCAL;

		}

		if (readframe (camid)==1)
			break;

	}

	//LOGI("readframeonce success ");
	return SUCCESS_LOCAL;
}


void processimage (const void *p, int camid){
	/* mjpg input */
		jpeg_decode((unsigned char **)&gcam[camid].yuyv, (unsigned char *)p, &gcam[camid].img_width, 
				&gcam[camid].img_height);
		yuyv422toABGRY((unsigned char *)gcam[camid].yuyv, camid);
		/*mtk*/
		yuyv422Toyuv420p((unsigned char *)gcam[camid].yuyv, camid);
		/*qcom*/
		//yuyv422Toyuv420sp((unsigned char *)gcam[camid].yuyv, camid);

	/* yuyv422 input */
		//yuyv422toABGRY((unsigned char *)p, camid);
		//yuyv422Toyuv420sp((unsigned char *)p, camid);
}

int readframe(int camid){

	struct v4l2_buffer buf;
	unsigned int i;

	CLEAR (buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	//buf.memory = V4L2_MEMORY_USERPTR;
	//LOGE("fd=%d,request=%d,buf=%d",fd,VIDIOC_DQBUF,&buf);
	if (-1 == xioctl (gcam[camid].fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
			case EAGAIN:
				return 0;
			case EIO:
			default:
				return errnoexit ("VIDIOC_DQBUF");
		}
	}

	assert (buf.index < gcam[camid].n_buffers);
	
	//LOGI("readframe start processimage start=%x", gcam[camid].buffers[buf.index].start);

	processimage(gcam[camid].buffers[buf.index].start, camid);

	if (-1 == xioctl (gcam[camid].fd, VIDIOC_QBUF, &buf))
		return errnoexit ("VIDIOC_QBUF");
	
	//LOGI("readframe finish");
	return 1;
}

int stopcapturing(int camid)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (gcam[camid].fd, VIDIOC_STREAMOFF, &type))
		return errnoexit ("VIDIOC_STREAMOFF");

	return SUCCESS_LOCAL;

}

int uninitdevice(int camid)
{
	unsigned int i;

	for (i = 0; i < gcam[camid].n_buffers; ++i)
		if (-1 == munmap (gcam[camid].buffers[i].start, gcam[camid].buffers[i].length))
			return errnoexit ("munmap");

	free (gcam[camid].buffers);

	return SUCCESS_LOCAL;
}
//关闭设备
int closedevice(int camid)
{
	if (-1 == close (gcam[camid].fd)){
		gcam[camid].fd = -1;
		return errnoexit ("close");
	}

	gcam[camid].fd = -1;
	return SUCCESS_LOCAL;
}

/* mtk */
int yuyv422Toyuv420p(unsigned char *src, int camid) {
	int i = 0; 
	int y, u , v;
	unsigned char* dst = gcam[camid].yuv420;
	int len = gcam[camid].img_width * gcam[camid].img_height * 2;
	y = 0 ; u = gcam[camid].img_width * gcam[camid].img_height;
	v = u + (gcam[camid].img_width * gcam[camid].img_height)/4 ;
	for (i = 0 ; i < len ; i += 4 ) {
		dst[y++] = src[i];  //y1
		dst[y++] = src[i + 2];  //y2
		if((i/(gcam[camid].img_width*2))%2 == 0) {
			dst[u] = src[i + 1];  //u
			u += 1;
		}
		else {
			dst[v] = src[i + 3];   //v
			v += 1;
		}
	}
	return 1;
}

/* qcom */
int yuyv422Toyuv420sp(unsigned char *src, int camid) {
	int i = 0; 
	int y, u , v;
	unsigned char* dst = gcam[camid].yuv420;
	int len = gcam[camid].img_width * gcam[camid].img_height * 2;
	y = 0 ; u = gcam[camid].img_width * gcam[camid].img_height; v = u + 1 ;
	for (i = 0 ; i < len ; i += 4 ) {
		dst[y++] = src[i];  //y1
		dst[y++] = src[i + 2];  //y2
		if((i/(gcam[camid].img_width*2))%2 == 0) {
			dst[u] = src[i + 1];  //u
			u += 2;
		}
		else {
			dst[v] = src[i + 3];   //v
			v += 2;
		}
	}
	return 1;
}


static int yuv_tbl_ready=0;
int y1192_tbl[256];
int v1634_tbl[256];
int v833_tbl[256];
int u400_tbl[256];
int u2066_tbl[256];

void yuyv422toABGRY(unsigned char *src, int camid)
{

	int width=0;
	int height=0;
	
	width = gcam[camid].img_width;
	height = gcam[camid].img_height;

	int frameSize =width*height*2;

	int i;

	if((!gcam[camid].rgb || !gcam[camid].ybuf)){
		return;
	}
	int *lrgb = NULL;
	int *lybuf = NULL;
		
	lrgb = &gcam[camid].rgb[0];
	lybuf = &gcam[camid].ybuf[0];

	if(yuv_tbl_ready==0){
		for(i=0 ; i<256 ; i++){
			y1192_tbl[i] = 1192*(i-16);
			if(y1192_tbl[i]<0){
				y1192_tbl[i]=0;
			}

			v1634_tbl[i] = 1634*(i-128);
			v833_tbl[i] = 833*(i-128);
			u400_tbl[i] = 400*(i-128);
			u2066_tbl[i] = 2066*(i-128);
		}
		yuv_tbl_ready=1;
	}

	for(i=0 ; i<frameSize ; i+=4){
		unsigned char y1, y2, u, v;
		y1 = src[i];
		u = src[i+1];
		y2 = src[i+2];
		v = src[i+3];

		

		
		int y1192_1=y1192_tbl[y1];
		int r1 = (y1192_1 + v1634_tbl[v])>>10;
		int g1 = (y1192_1 - v833_tbl[v] - u400_tbl[u])>>10;
		int b1 = (y1192_1 + u2066_tbl[u])>>10;

		int y1192_2=y1192_tbl[y2];
		int r2 = (y1192_2 + v1634_tbl[v])>>10;
		int g2 = (y1192_2 - v833_tbl[v] - u400_tbl[u])>>10;
		int b2 = (y1192_2 + u2066_tbl[u])>>10;

		r1 = r1>255 ? 255 : r1<0 ? 0 : r1;
		g1 = g1>255 ? 255 : g1<0 ? 0 : g1;
		b1 = b1>255 ? 255 : b1<0 ? 0 : b1;
		r2 = r2>255 ? 255 : r2<0 ? 0 : r2;
		g2 = g2>255 ? 255 : g2<0 ? 0 : g2;
		b2 = b2>255 ? 255 : b2<0 ? 0 : b2;

		*lrgb++ = 0xff000000 | b1<<16 | g1<<8 | r1;
		*lrgb++ = 0xff000000 | b2<<16 | g2<<8 | r2;

		if(lybuf!=NULL){
			*lybuf++ = y1;
			*lybuf++ = y2;
		}
	}

}


void 
Java_com_camera_usbwebcam_CameraPreview_pixeltobmp( JNIEnv* env,jobject thiz,jobject bitmap,jint camid){

	jboolean bo;


	AndroidBitmapInfo  info;
	void*              pixels;
	int                ret;
	int i;
	int *colors;

	int width=0;
	int height=0;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return;
	}
    
	width = info.width;
	height = info.height;

	if(!gcam[camid].rgb || !gcam[camid].ybuf) return;

	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
	}

	colors = (int*)pixels;
	int *lrgb =NULL;
	lrgb = &gcam[camid].rgb[0];

	for(i=0 ; i<width*height ; i++){
		*colors++ = *lrgb++;
	}

	AndroidBitmap_unlockPixels(env, bitmap);

}

jbyteArray 
	JNICALL Java_com_camera_usbwebcam_CameraPreview_getyuv( JNIEnv* env,jobject thiz, jint camid) {
 
	jbyteArray array = (*env)->NewByteArray(env, (gcam[camid].img_width*gcam[camid].img_height*3/2));
	(*env)->SetByteArrayRegion(env, array, 0, (gcam[camid].img_width*gcam[camid].img_height*3/2), gcam[camid].yuv420);
	return array;
}

jint 
Java_com_camera_usbwebcam_CameraPreview_prepareCamera( JNIEnv* env,jobject thiz, jint camid, jint videobase){

	int ret;
	
	ret = opendevice(camid, videobase);

	if(ret != ERROR_LOCAL){
		ret = initdevice(camid);
	}
	if(ret != ERROR_LOCAL){
		ret = startcapturing(camid);

		if(ret != SUCCESS_LOCAL){
			stopcapturing(camid);
			uninitdevice (camid);
			closedevice (camid);
			LOGE("device resetted");	
		}

	}

	if(ret != ERROR_LOCAL){
		gcam[camid].rgb = (int *)malloc(sizeof(int) * (gcam[camid].img_width*gcam[camid].img_height));
		gcam[camid].ybuf = (int *)malloc(sizeof(int) * (gcam[camid].img_width*gcam[camid].img_height));
		gcam[camid].yuv420 = (unsigned char *)malloc((gcam[camid].img_width*gcam[camid].img_height*3/2));
		gcam[camid].mjpeg = (unsigned char *)malloc((gcam[camid].img_width*gcam[camid].img_height*2));
		gcam[camid].yuyv = (unsigned char *)malloc((gcam[camid].img_width*gcam[camid].img_height*2));
	}
	
	LOGI("prepareCamera finish!!");	
	
	return ret;
}	



jint 
Java_com_camera_usbwebcam_CameraPreview_prepareCameraWithBase( JNIEnv* env,jobject thiz, jint camid, jint videobase){
	
		int ret;	
		return Java_com_camera_usbwebcam_CameraPreview_prepareCamera(env,thiz,camid,videobase);
	
}

void 
Java_com_camera_usbwebcam_CameraPreview_processCamera( JNIEnv* env,
										jobject thiz, jint camid){

	readframeonce(camid);
}

void 
Java_com_camera_usbwebcam_CameraPreview_stopCamera(JNIEnv* env,jobject thiz, jint camid){

	stopcapturing (camid);

	uninitdevice (camid);

	closedevice (camid);

	if(gcam[camid].rgb) free(gcam[camid].rgb);
	if(gcam[camid].ybuf) free(gcam[camid].ybuf);
	if(gcam[camid].yuv420) free(gcam[camid].yuv420);
	if(gcam[camid].mjpeg) free(gcam[camid].mjpeg);
	if(gcam[camid].yuyv) free(gcam[camid].yuyv);
	gcam[camid].fd = -1;
}

void 
Java_com_camera_usbwebcam_CameraPreview_setimgsize( JNIEnv* env,jobject thiz, jint camid, jint width, jint height){
	
		int ret;

		gcam[camid].img_width = width;
		gcam[camid].img_height = height;		
		
}


