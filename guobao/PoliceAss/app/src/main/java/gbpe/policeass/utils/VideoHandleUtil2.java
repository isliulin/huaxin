package gbpe.policeass.utils;

import android.util.Log;
import com.orhanobut.logger.Logger;
import com.topvision.videodemo.util.VideoSet;
import java.io.File;
import java.util.List;




public class VideoHandleUtil2 {

    String finalFileName;
   // String normalPath = "/storage/sdcard0/DCIM/101ANDRO/888888_1234567_20100101110454_0077.mp4";

    String normalPath = VideoSet.videoFilePath;

    private String TAG = "VideoHandleUtil2";


    /**
     * 合并预录部分视频
     * @param folder
     */
    public void combine(String folder ){

        Log.e(TAG, "combine: =====================================================>>" );
        if (!mp42Ts2(folder)) {
            return;
        }
        if ( !combineTs2Mp4(folder)) {
            return;
        }
        delSegVideoFile(folder);
        deleFile(normalPath);
        String videoTs = normalPath.substring(0,normalPath.indexOf("."))+".ts";
        deleFile(videoTs);
    }

	/* ffmpeg.exe –i 1.mp4 –vcodec copy –acodec copy –vbsf h264_mp4toannexb 1.ts */
    public    boolean  mp42Ts2(String folder) {
        StringBuilder sb = new StringBuilder();
        double dbfileSize = 0d;
        String tt = "";
        File file = new File(folder);
        File[] files = file.listFiles();
        int len =0;

        if (files==null||(len = files.length)<1){
            return false;
        }
        //Logger.i("len = "+len);

        for (int i = 0; i < len; i++)
        {
            File file1 = files[i];
            String fileName = file1.getName();
            if (!fileName.endsWith(".mp4")){
                continue;
            }
            if (i==0){
                finalFileName = fileName;
                Logger.i("finalFileName ===     "+finalFileName);
            }
            //Logger.i("fileName ===     "+fileName);
            String fileTitle = fileName.substring(0, fileName.indexOf("."));
            Logger.i("fileTitle ===     "+fileTitle);
            String fileSize = FileSizeUtil.getFileOrFilesSize
                    (file1.getAbsolutePath(), 1);
            //  Logger.i("fileSize ===     "+fileSize);
            if (fileSize.endsWith("MB")||fileSize.endsWith("GB"))
            {
                sb.append("ffmpeg -i " + file1.getAbsolutePath()
                        + " -vcodec copy -acodec copy -vbsf h264_mp4toannexb" + " "
                        +folder+ fileTitle + ".ts");

            } else if (fileSize.endsWith("KB"))
            {
                tt = fileSize.replace("KB", "").trim();
                dbfileSize = Double.parseDouble(tt);
                if (dbfileSize < 100)
                {
                    Log.e("TAG",fileName+"的大小是KB:"+fileSize+"被删除了....");
                    if (fileSizeCalculate(i,len,file1)){
                        continue;
                    }else {
                        break;
                    }
                }else {
                    sb.append( "ffmpeg -i " + file1.getAbsolutePath()
                            + " -vcodec copy -acodec copy -vbsf h264_mp4toannexb" + " "
                            +folder+ fileTitle + ".ts");
                }
            }else
            {
                Log.e("TAG",fileName+"的大小是B:"+fileSize+"被删除了....");
                if (fileSizeCalculate(i,len,file1)){
                    continue;
                }else {
                    break;
                }
            }
            Log.e("TAG","tsfilePath"+i+":"+sb.toString());
            final String[] argv=sb.toString().split(" ");
            final Integer argc=argv.length;
            ffmpegcore(argc,argv);
            sb.delete(0,sb.length());
        }


        String fileTitle2 = normalPath.substring(0,normalPath.indexOf("."));
        //  Logger.i("fileTitle2 ===   " + fileTitle2);
        sb.append("ffmpeg -i "  +normalPath
                + " -vcodec copy -acodec copy -vbsf h264_mp4toannexb" + " "
                + fileTitle2 + ".ts");
        //    Logger.i("sb    ===   " + sb.toString());
        String[] argv2=sb.toString().split(" ");
        Integer argc2=argv2.length;
        ffmpegcore(argc2,argv2);
        sb.delete(0,sb.length());
        return true;
    }


    /**
     * 在这一步要判断正式录像的文件是否为重点文件
     * @param folder
     * @return
     */
    public   boolean combineTs2Mp4(String folder) {
        File file = new File(folder);
        File[] files = file.listFiles();
        if (files ==null){
            return false;
        }
        if (files.length<=1){
            return false;
        }
        StringBuilder sb = new StringBuilder();
        sb.append("ffmpeg -i concat:");
        for (int i = 0; i < files.length; i++) {
            if (files[i].getAbsolutePath().endsWith(".ts")){
                sb.append(files[i].getAbsolutePath() + "|");
            }
        }

        String videoTs =normalPath.substring(0,normalPath.indexOf("."))+".ts";
        sb.append(videoTs);
        if (emphasisFile(normalPath)){
            sb.append(" -acodec copy -vcodec copy -absf aac_adtstoasc" + " "+
                    new File(normalPath).getParentFile().getAbsolutePath()+"/" + finalFileName.substring(0,finalFileName.indexOf("."))+"IMP.mp4");

        }else {
            sb.append(" -acodec copy -vcodec copy -absf aac_adtstoasc" + " "+
                    new File(normalPath).getParentFile().getAbsolutePath()+"/" + finalFileName);

        }

        Log.e("TAG","sb"+sb.toString());
        Logger.i("sb    ===   " + sb.toString());
        final String[] argv=sb.toString().split(" ");
        final Integer argc=argv.length;
        ffmpegcore(argc,argv);

//        if (new File(new File(normalPath).getParentFile().getAbsolutePath()+"/" + finalFileName).exists()
//                ||new File(new File(normalPath).getParentFile().getAbsolutePath()+"/" +
//                finalFileName.substring(0,finalFileName.indexOf("."))+"IMP.mp4").exists())
//            return  true;
        return true;
    }




    public void delSegVideoFile(String folder){
        File file = new File(folder);
        File[] files = file.listFiles();
        for (int i = 0; i < files.length; i++) {
            String filePath = files[i].getAbsolutePath();
            File file1 = new File(filePath);
            String fileName = file1.getName();
            if ((fileName.endsWith(".mp4")||fileName.endsWith(".3gp")
                    || fileName.endsWith(".avi")||
                    fileName.endsWith(".ts"))){
                file1.delete();
                Log.e("TAG",fileName);
            }
        }

    }




    private void deleFile(String filePath){
        Logger.i("filePath = "+filePath);
        File file = new File(filePath);
        Logger.i("exists = "+ file.exists());

        if (file.exists()){
            file.delete();
        }
    }

    private   boolean fileSizeCalculate(int i,int length,File file1){

        file1.delete();
        if (i < length) {
            return true;
        } else {
            return false;
        }
    }
    /**
     * 判断是否为重点文件
     * @param fileName
     * @return
     */
    public static boolean emphasisFile(String fileName)
    {
//        String tempFileName = fileName.substring(0,fileName.indexOf("."));//获取文件名
        if (fileName.contains("IMP"))
        {
            return true;
        }
        return false;
    }


    /**
     * 递归的删除文件夹（即删除文件夹及其子文件夹）
     *
     * @param file 文件路径
     *
     */
    public static void delDirRecursion(File file){
        if(file.isDirectory()){
            if(file.listFiles().length > 0)
            {
                for(File f : file.listFiles())
                {
                    delDirRecursion(f);
                }
            }else {
                //目录为空，直接删除
                file.delete();
            }
        }else{
            file.delete();
        }
    }
    static{
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("sfftranscoder");
    }

    public native int ffmpegcore(int argc,String[] argv);

}
