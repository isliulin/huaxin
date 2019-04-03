#include <string.h>
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "gbpe_policeass_utils_RgbInterpolation.h"
#include <android/log.h>
#include <include/libavutil/pixfmt.h>
#include <stdio.h>
#include <include/libavutil/log.h>
#include <include/libavutil/parseutils.h>
#include <include/libswscale/swscale.h>
#include <include/libavutil/pixdesc.h>
#include <include/libavutil/imgutils.h>

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)

JNIEXPORT void JNICALL Java_gbpe_policeass_utils_RgbInterpolation_photoEnlarge
        (JNIEnv * env, jobject obj, jstring pDest, jint nDestWidth, jint nDestHeight, jint nDestBits, jstring pSrc, jint nSrcWidth, jint nSrcHeight, jint nSrcBits)
{
    //enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUYV422;
    //enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_YUV420P;
    enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_RGB24;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
    struct SwsContext *sws_ctx;

    //const char *dst_filename = NULL;
    //const char *src_filename = NULL;
    const char *dst_size = NULL;
    const char *src_size = NULL;
    uint8_t *src_data[4];
    uint8_t *dst_data[4];
    int src_linesize[4];
    int dst_linesize[4];
    int src_bufsize;
    int dst_bufsize;
    int src_w ;
    int src_h ;
    int dst_w ;
    int dst_h ;
    int i;
    int ret;

    const char *filename_src = (*env)->GetStringUTFChars(env,pSrc, NULL);
    const char *filename_dst = (*env)->GetStringUTFChars(env,pDest,NULL);

    FILE *src_file;
    FILE *dst_file;

    if ((pDest == NULL) || (pSrc == NULL)) {
        av_log(NULL, AV_LOG_ERROR,
               "输入文件名输出文件名有误");
        return;
    } else {
        av_log (NULL, AV_LOG_VERBOSE, "from %s to %s", pSrc, pDest);
    }

    char buf[10] = {0};
    sprintf(buf,"%dx%d",nSrcHeight,nSrcHeight );
    src_size = buf;
    if (av_parse_video_size(&src_w,&src_h,src_size) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid size %s, must be in the form WxH or a valid size abbreviation\n",dst_size);
        return;
    }

    src_file = fopen(filename_src, "rb");
    if (!src_file) {
        av_log(NULL, AV_LOG_ERROR, "Could not open source file %s\n",src_file);
        return;
    }

    /*destination file parse*/
    sprintf(buf, "%dx%d", nDestWidth, nDestHeight);
    dst_size = buf;
    if (av_parse_video_size(&dst_w,&dst_h,dst_size) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid size %s, must be in the form WxH or a valid size abbreviation\n",dst_size);
        return;
    }

    dst_file = fopen(filename_dst,"wb");
    if (!dst_file) {
        av_log(NULL, AV_LOG_ERROR, "Could not open destination file %s\n", filename_dst);
        return;
    }

    /* create scaling context */
    sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        av_log(NULL, AV_LOG_ERROR,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);
        ret = AVERROR(EINVAL);
        goto end;
    }

    /* allocate source and destination image buffers */
    if ((ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pix_fmt, 16)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate source image\n");
        goto end;
    }

    src_bufsize = ret;

    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate destination image\n");
        goto end;
    }

    dst_bufsize = ret;

    fread(src_data[0], 1, src_bufsize, src_file);
    sws_scale(sws_ctx, (const uint8_t * const*)src_data,  src_linesize, 0, src_h, dst_data, dst_linesize);

    av_log(NULL, AV_LOG_ERROR, "Scaling succeeded. Play the output file with the command:\n"
                    "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
            av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h, filename_dst);

    /* write scaled image to file */
    fwrite(dst_data[0], 1, dst_bufsize, dst_file);

    end:
    fclose(dst_file);
    fclose(src_file);
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
    return;


}