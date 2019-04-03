package gbpe.policeass.utils;

import android.util.Log;
import com.orhanobut.logger.Logger;
import com.topvision.videodemo.util.VideoSet;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.appinfo.App;


public class VideoHandleUtils {

 //    String finalFileName = "";
    private String normalPath = VideoSet.videoFilePath;
    private static final String TAG = "VideoHandleUtils";

    private String finalFileName;
    private boolean isDeleteOld;
  //  String normalPath = "/storage/sdcard0/DCIM/101ANDRO/888888_1234567_20100101110454_0077.mp4";
    public VideoHandleUtils(boolean isDeleteOld){
        this.isDeleteOld = isDeleteOld;
    }


    /**
     * 合并预录部分视频
     * @param folder
     */
    public void combine(String folder ){
         if (!mp42Ts2(folder)) {
            return;
        }
        if ( !combineTs2Mp4(folder)) {
            return;
        }
        FileUtils.deleteFolder(folder);
        deleFile(normalPath);
        String videoTs = normalPath.substring(0,normalPath.indexOf("."))+".ts";
        deleFile(videoTs);
     }

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
        Log.e("TAG","normalPath"+":   "+sb.toString());

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
        Log.e("VideoHandleUtils", "combineTs2Mp4: =====================>"+folder);
        File file = new File(folder);
        File[] files = file.listFiles();


        if (files ==null||files.length<1){
            return false;
        }

        StringBuffer sb = new StringBuffer();
        sb.append("ffmpeg -i concat:");
        for (int i = 0; i < files.length; i++) {
            if (files[i].getAbsolutePath().endsWith(".ts")){
                sb.append(files[i].getAbsolutePath() + "|");
            }
        }


        Log.e("TAG", "预录那个傻逼:========= "+sb.toString() );
        String videoTs =normalPath.substring(0,normalPath.indexOf("."))+".ts";
        sb.append(videoTs);
        if (emphasisFile(normalPath)){
            sb.append(" -acodec copy -vcodec copy -absf aac_adtstoasc" + " "+
                    new File(normalPath).getParentFile().getAbsolutePath()+"/" + finalFileName.substring(0,finalFileName.indexOf("."))+"IMP.mp4");

        }else {
            sb.append(" -acodec copy -vcodec copy -absf aac_adtstoasc" + " "+
                    new File(normalPath).getParentFile().getAbsolutePath()+"/" + finalFileName);

        }

        Log.e("TAG","最终的sb=>"+sb.toString());
        Logger.i("sb    ===   " + sb.toString());
        final String[] argv=sb.toString().split(" ");
        final Integer argc=argv.length;
        ffmpegcore(argc,argv);
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

    public static native int ffmpegcore(int argc,String[] argv);

	/*here add by ycb: 20180913*/
	public void combine(ArrayList<String> files){

        ArrayList<String> tsList = null;
        ArrayList<String> mp4List;
        AppLog.i(TAG, "Video Combin" );

        try {
            tsList = mp42Ts2(files);
        } catch (Exception e){
            AppLog.e(TAG,"mp42Ts2 error");
        }

        if (tsList == null) {
            AppLog.e(TAG,"mp42Ts2 error");
            return;
        }
		AppLog.i(TAG,"mp4 to ts convert finished");
		
        mp4List = combineTs2Mp4(tsList);
        if (mp4List == null) {
            AppLog.e(TAG,"combineTs2Mp4 error");
            return;
        }

		AppLog.i(TAG,"ts to mp4 convert finished");

        for (int i=0;i<tsList.size();i++){
            File f = new File(tsList.get(i));
            f.delete();
        }

        if (isDeleteOld){
			/* the first file is use as the target file */
            for (int i=1;i<files.size();i++){
                File f = new File(files.get(i));
                f.delete();
            }
        }
    }

    /* ffmpeg.exe –i 1.mp4 –vcodec copy –acodec copy –vbsf h264_mp4toannexb 1.ts */
    public ArrayList<String> mp42Ts2(ArrayList<String> fileArray){
        StringBuilder sb = new StringBuilder();
        double dbfileSize = 0d;
        String tt = "";
		ArrayList<String> tsList = new ArrayList<>();

        int len =fileArray.size();
        //File[] files = new File[len];
        ArrayList<File> files = new ArrayList<>();

        if ( len < 1 ){
            return null;
        }

        for (int i=0;i<len;i++){
            //files[i] = new File(fileArray.get(i));
            File f = new File(fileArray.get(i));
            if (f.exists()){
                files.add(f);
            }else {
                AppLog.i(TAG,fileArray.get(i)+" not exit");
            }
        }

        for (int i = 0; i < len; i++){
			sb.append("ffmpeg -i " + files.get(i).getAbsolutePath()
                        + " -vcodec copy -acodec copy -vbsf h264_mp4toannexb" + " "
                        + files.get(i).getAbsolutePath() + ".ts");
            AppLog.i(TAG,"ts["+i+"]"+": "+files.get(i).getAbsolutePath() + ".ts");
			String[] argv=sb.toString().split(" ");
            Integer argc=argv.length;
            ffmpegcore(argc,argv);
            tsList.add(files.get(i).getAbsolutePath() + ".ts");
            sb.delete(0,sb.length());
			
        }
        return tsList;
    }

	// ffmpeg.exe –icombine.ts –acodec copy –vcodec copy –absf aac_adtstoasc combine.mp4
    public   ArrayList<String> combineTs2Mp4(ArrayList<String> fileArray) {
        //File file = new File(folder)
        ArrayList<String> mp4List = new ArrayList<>();

        if ((fileArray==null)||(fileArray.size()<=0)){
            return null;
        }

        //File[] files = new File[fileArray.size()];
        ArrayList<File> files = new ArrayList<>();

        for(int i=0;i<fileArray.size();i++){
            //files[i] = new File(fileArray.get(i));
            File f = new File(fileArray.get(i));
            if (f.exists()){
                files.add(f);
            } else {
                AppLog.i (TAG,fileArray.get(i)+" not exit");
            }
        }

        StringBuilder sb = new StringBuilder();
        sb.append("ffmpeg -i concat:");
        if (files.size() > 1){
            for (int i = 0; i < files.size(); i++) {
                sb.append(files.get(i).getAbsolutePath() + "|");
                mp4List.add(files.get(i).getParent()+files.get(i).getName().replace(".ts",""));
            }
        }else{
            sb.append(files.get(0).getAbsolutePath());
            mp4List.add(files.get(0).getParent()+files.get(0).getName().replace(".ts",""));
        }

        String newPath = files.get(0).getParent()+"/"+files.get(0).getName().replace(".ts","");
        AppLog.i(TAG, "combine name "+newPath);
        sb.append(" -acodec copy -vcodec copy -absf aac_adtstoasc" + " "+newPath);


        AppLog.i("TAG","sb"+sb.toString());
        //Logger.i("sb    ===   " + sb.toString());
        final String[] argv=sb.toString().split(" ");
        final Integer argc=argv.length;
        ffmpegcore(argc,argv);

        AppLog.i(TAG, "combineTs2Mp4 OK");

        return mp4List;
		
    }

    // ffmpeg -i xxx.jpg -s 1624x1236 -pix_fmt yuvj420p xxx.yuv
    public static boolean photoResize(String src, int wt, int ht, String dst, int nwt, int nht) {
        String strCmd = "ffmpeg -i "+src+" -s "+nwt+"x"+nht+" -pix_fmt "+dst;
        AppLog.i (TAG, "ffmpeg cmd: "+strCmd);
        String[] argv=strCmd.split(" ");
        Integer argc=argv.length;
        ffmpegcore(argc,argv);
        return true;
    }


}
