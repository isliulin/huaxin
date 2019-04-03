package gbpe.policeass.utils;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.orhanobut.logger.Logger;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.fragments.SettingFragment;

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2017/5/4 08:30
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2017/5/4 08:30
 *
 * 描述：外置sd卡不可用或存储空间已满时，可以自动切换到内置sd卡，如果内置sd卡不可用，
 * 或存储空间不够，则不可以采集数据，否则采集数据
 * =====================================================================================
 */

public class FileUtils {
    private static final String TAG = "FileUtils";

    private FileUtils() {
    }

    /**
     * 递归的创建文件夹（即创建文件夹及其子文件夹）
     *
     * @param file 文件路径
     * @return 文件拓展名
     */
    public static void mkDir(File file){
        if(file.getParentFile().exists())
        {
            file.mkdir();
        }else {
            mkDir(file.getParentFile());
            //System.out.println(file.getName());
            Log.i(TAG,"mkDir "+file.getName());
            file.mkdir();
        }
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

    public static boolean deleteFolder(String url) {
        Log.e(TAG, "deleteFolder: ===========>>>删除url  " + url );
        File file = new File(url);
        Log.e(TAG, "deleteFolder: ===========>>>file  " + file.exists() );

        if (!file.exists()) {
            return false;
        }
        if (file.isFile()) {
            file.delete();
            return true;
        } else {
            File[] files = file.listFiles();
            for (int i = 0; i < files.length; i++) {
                String root = files[i].getAbsolutePath();//得到子文件或文件夹的绝对路径
                //System.out.println(root);
                deleteFolder(root);
            }
            file.delete();
            return true;
        }
    }


    /**
     * 复制单个文件
     * 以下路径指的是文件的绝对路径
     * @param oldPath String 原文件路径 如：c:/fqf.txt
     * @param newPath String 复制后路径 如：f:/fqf.txt
     * @return boolean
     */
    public static void copyFile(String oldPath, String newPath) throws Exception{

        if (fileCopyListener !=null){
            fileCopyListener.onFileCopyStart();
        }

        int bytesum = 0;
        int byteread = 0;

        File oldfile = new File(oldPath);
        if (!oldfile.exists()) {return ;}
        if (!oldfile.isFile()) {return ;}
        if (!oldfile.canRead()){return ;}

        File newfile = new File(newPath);
        if (newfile.exists()){
            newfile.delete();
        }

        if (oldfile.exists())
        { //文件存在时
            InputStream inStream = new FileInputStream(oldPath); //读入原文件
            FileOutputStream fs = new FileOutputStream(newPath);
            byte[] buffer = new byte[1444];
            int length;
            while ( (byteread = inStream.read(buffer)) != -1)
            {
                bytesum += byteread; //字节数 文件大小
                System.out.println(bytesum);
                fs.write(buffer, 0, byteread);
            }
            inStream.close();
        }
        //拷贝完将原文件从当前文件夹中删除。
        oldfile.delete();

        if (fileCopyListener !=null){
            fileCopyListener.onFileCopyFinished();
        }

    }


    /**
     * 复制整个文件夹内容
     * 以下路径指的是文件的绝对路径
     * @param oldPath String 原文件路径 如：c:/fqf
     * @param newPath String 复制后路径 如：f:/fqf/ff
     * @return boolean
     */
    public static void copyFolder(String oldPath, String newPath) throws FileNotFoundException, IOException {

        //try {
            (new File(newPath)).mkdirs(); //如果文件夹不存在 则建立新文件夹
            File a=new File(oldPath);
            String[] file=a.list();
            File temp=null;
            for (int i = 0; i < file.length; i++) {
                if(oldPath.endsWith(File.separator)){
                    temp=new File(oldPath+file[i]);
                }
                else{
                    temp=new File(oldPath+File.separator+file[i]);
                }

                if(temp.isFile()){
                    FileInputStream input = new FileInputStream(temp);
                    FileOutputStream output = new FileOutputStream(newPath + "/" +
                            (temp.getName()).toString());
                    byte[] b = new byte[1024 * 5];
                    int len;
                    while ( (len = input.read(b)) != -1) {
                        output.write(b, 0, len);
                    }
                    output.flush();
                    output.close();
                    input.close();
                }
                if(temp.isDirectory()){//如果是子文件夹
                    copyFolder(oldPath+"/"+file[i],newPath+"/"+file[i]);
                }
            }
        //}
        //catch (Exception e) {
        //    System.out.println("复制整个文件夹内容操作出错");
        //    e.printStackTrace();
        //}

    }

    /**
     * 标记文件（标记普通或重点文件）
     * @param filePath
     */
    public static String changeFileType(String filePath,Context context){

        String newFilePath = null;
        File file = new File(filePath);
        String fileName = file.getName();//获得文件名
        String tempFileName = fileName.substring(0,fileName.indexOf("."));//获取文件标题（即去文件名去掉后缀部分）
        String fileSuffix = fileName.substring(fileName.indexOf("."));//获取文件后缀
        //非重点文件可以被修改（加上IMP，拷贝到重点文件夹面）
        if (!emphasisFile(filePath))
        {
            String newFileName = "DCIM/"+Setup.emphFileFolder+tempFileName+"IMP"+fileSuffix;//标记文件
              if (SDCardUtils.isSDCardEnable()){
                  newFilePath = SDCardUtils.getSDCardPath()+newFileName;
            }

            try {
                copyFile(filePath,newFilePath);//将修改后的文件拷贝到新文件夹下面
            } catch (Exception e) {
                e.printStackTrace();
            }
            return newFilePath;
        }else//重点文件标记为非重点文件（去掉IMP,然后拷贝到普通文件夹）
        {
            String tempFileName2 = tempFileName.substring(0,tempFileName.indexOf("I"));//去掉重点文件中的IPM后缀。
            String newFileName = "DCIM/"+Setup.comFileFolder+tempFileName2+fileSuffix;//标记文件

           if (SDCardUtils.isSDCardEnable()){
                newFilePath = SDCardUtils.getSDCardPath()+newFileName;
            }

            try {
                copyFile(filePath,newFilePath);//将修改后的文件拷贝到新文件夹下面
            } catch (Exception e) {
                e.printStackTrace();
            }
            return newFilePath;
        }

    }

    public static String getNewFilePath(String filePath){
        File file = new File(filePath);
        String fileName = file.getName();//获得文件名
        String tempFileName = fileName.substring(0,fileName.indexOf("."));//获取文件标题（即去文件名去掉后缀部分）
        String fileSuffix = fileName.substring(fileName.indexOf("."));//获取文件后缀
        //非重点文件可以被修改（加上IMP，拷贝到重点文件夹面）
        if (!emphasisFile(filePath))
        {
            String newFileName = "DCIM/"+Setup.emphFileFolder+tempFileName+"IMP"+fileSuffix;//标记文件
            String newFilePath = SDCardUtils.getSDCardPath()+newFileName;
            return newFilePath;
        }else//重点文件标记为非重点文件（去掉IMP,然后拷贝到普通文件夹）
        {
            String tempFileName2 = tempFileName.substring(0,tempFileName.indexOf("I"));//去掉重点文件中的IPM后缀。
            String newFileName = "DCIM/"+Setup.comFileFolder+tempFileName2+fileSuffix;//标记文件
            String newFilePath = SDCardUtils.getSDCardPath()+newFileName;
            return newFilePath;
        }
    }

    /**
     * 判断是否为重点文件
     * @param fileName
     * @return
     */
    public static boolean emphasisFile(String fileName)
    {
        String tempFileName = fileName.substring(0,fileName.indexOf("."));//获取文件名
        if (tempFileName.contains("IMP"))
        {
            return true;
        }
        return false;
    }


    /**
     * 获得视频文件(重点或非重点)的路径
     *
     *     //  String videoFilePath =videoPaths+comFileFolder+policeNumber+"_"+devNum+"_"+ com.topvision.videodemo.util.TimeUtils.milliseconds2String2( System.currentTimeMillis())+"_"+ FileNumberFormat.formatter(titalFilesNum)+".mp4";
     *
     * @return
     */
    public static String getFileFolderPath(){
        String floderPath ="";
        //想要保存文重点文件(长按物理按键)
        if (Setup.isEmphasisFile)
        {   //重点文件夹路径
            floderPath = Setup.videoPaths+Setup.emphFileFolder;
        }else//未长按物理按键（分为短按和未按）//非重点文件夹的路径
        {
            if (Setup.VideoPrerecord && !Setup.isNormalRecording)//没有按下物理按键
            {
                floderPath = Setup.videoPaths+Setup.segmentedTemp;
            }else                                             //按下物理按键(短按)
            {
                floderPath = Setup.videoPaths+Setup.comFileFolder;
            }
        }

		Log.i(TAG, "file path: "+ floderPath);
        return floderPath;
    }


    /**
     * 获得视频文件(重点或非重点)的绝对路径
     * @return
     */
    public static String getFilePath(){

        String currFilePath = "";
        // 创建保存录制视频的文件夹
        File dir = new File(getFileFolderPath());
        if (!dir.exists()) {
            //dir.mkdir();
            FileUtils.mkDir(dir);//递归创建文件夹
        }
        //重点文件
        if (Setup.isEmphasisFile){
            //如果是重点文件，则文件名中包含"IPM"
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"IMP." + Setup.videoFilePostfix;
        }else//非重点文件名无"IPM"后缀
        {
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+ Setup.videoFilePostfix;
        }
        return currFilePath;
    }

    /**
     * 获得视频文件(重点或非重点)的绝对路径
     * @return
     */
    public static String getFilePath(Context context){

        String currFilePath = "";
        // 创建保存录制视频的文件夹
        String folder =getFileFolderPath(context);
        if (TextUtils.isEmpty(folder)){
            return "";
        }
        File dir = new File(folder);
        if (!dir.exists()) {
            FileUtils.mkDir(dir);//递归创建文件夹
        }

        if (Setup.isEmphasisFile){
            //如果是重点文件，则文件名中包含"IPM"
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"IMP." + Setup.videoFilePostfix;
        }else
        {
           //非重点文件名无"IPM"后缀
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+ Setup.videoFilePostfix;
        }
        return currFilePath;
    }


     /**
     *
     * @param context
     * @param sufixx 文件后缀
     * @return
     */
    public static String getFilePath2(Context context,String suffix){

        String currFilePath = "";
        // 创建保存录制视频的文件夹
        // String folder =getFileFolderPath(context);
        // modified by ycb, create date folder
        String folder = getFileFolderPathByDate(context);

		// tag by ycb, for get file name
		// add date folder
		Log.i(TAG, "file name: "+ folder);
		
        if (TextUtils.isEmpty(folder)){
			Log.i(TAG, "can not create file");
            return "";
        }
		
        File dir = new File(folder);
		
        if (!dir.exists()) 
		{
            FileUtils.mkDir(dir);//递归创建文件夹
        }


        if (suffix.equals(Setup.videoFilePostfix))
        {//如果是视频的
        /*
            if (Setup.VideoPrerecord)//预录的视频先放到/data/data/包名/files/目录下
            {
            	Log.d(TAG,"Setup.VideoPrerecord: "+Setup.VideoPrerecord);
				
                if (!Setup.isNormalRecording){

                    Logger.i("FileUtils"+Setup.pre_file_dir);
                    File file = new File(Setup.pre_file_dir);
                    if (!file.exists()){
                        file.mkdir();
                    }

					// modified by ycb 
                    // currFilePath =Setup.pre_file_dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_"
                    //        + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_" +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+suffix;
                    currFilePath =Setup.pre_file_dir + "/"
                             + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_" +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+suffix;

					Log.d(TAG, "Setup.isNormalRecording: "+Setup.isNormalRecording+", path: "+currFilePath);
                }else {

					// modified by ycb
                    currFilePath = getFileAbsolutePathShort(dir,suffix);

					Log.d(TAG, "Setup.isNormalRecording: "+Setup.isNormalRecording+", path: "+currFilePath);
                }
            }else
            {*/

			// modified by ycb: 不管是不是预录，产生一个一样的文件路径            
            currFilePath = getFileAbsolutePathShort(dir,suffix);

			Log.d(TAG,"Setup.VideoPrerecord: "+Setup.VideoPrerecord);
			Log.d(TAG,"path: "+currFilePath);

			//}
				
        }else //如果是图片或者录音文件
        {
            currFilePath = getFileAbsolutePathShort(dir,suffix);

			Log.d(TAG, "picture or sound recording file, path: "+currFilePath);
        }

		// Tag by ycb
		Log.i(TAG, "getFilePath2 "+currFilePath);

        return currFilePath;
    }



    private static String getFileAbsolutePath(File dir,String suffix){
        String currFilePath="";
        Log.e("TAG", "=====================>>getFileAbsolutePath: "+dir.getAbsolutePath() +" : "+suffix );
        if (Setup.isEmphasisFile){
            //如果是重点文件，则文件名中包含"IMP"
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"IMP." + suffix;
        }else
        {
            //非重点文件名无"IPM"后缀
            currFilePath = dir + "/"+Setup.policeNumber+"_"+Setup.devNum+"_" + TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+suffix;
        }
        return currFilePath;
    }

	private static String getFileAbsolutePathShort(File dir, String suffix)
    {
		String currFilePath="";
		Log.d("TAG", "getFileAbsolutePath: "+dir.getAbsolutePath() +" : "+suffix );
		if (Setup.isEmphasisFile){
            //如果是重点文件，则文件名中包含"IMP"
            currFilePath = dir + "/"+ TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"IMP." + suffix;
        }else
        {
            //非重点文件名无"IPM"后缀
            currFilePath = dir + "/"+ TimeUtils.milliseconds2String2(System.currentTimeMillis())+"_"
                    +FileNumberFormat.formatter(Setup.titalFilesNum)+"."+suffix;
        }
        return currFilePath;
		
	}

//=========================================================================================================================================================================


    /**
     * 获得视频文件(重点或非重点)的路径
     * 外置sd卡不可用时会自动保存到内置sd卡
     * @return
     */
    public static String getFileFolderPath(Context context){
        String floderPath ="";
        //想要保存文重点文件(长按物理按键)
        if (Setup.isEmphasisFile)
        {
            floderPath  =  getEmphFileFolder(context);

			// Tag by ycb
			Log.i(TAG, "IMP folder path: "+floderPath);

        }else//未长按物理按键（分为短按和未按）//非重点文件夹的路径
        {
            if (Setup.VideoPrerecord && !Setup.isNormalRecording)//没有按下物理按键
            {
                floderPath = getSegmentFilePath(context);

				// Tag by ycb
				Log.i(TAG, "Segment folder path: "+floderPath);
				
            }else                                             //按下物理按键(短按)
            {
                floderPath = getComFileFolder(context);

				// Tag by ycb
				Log.i(TAG, "Com folder path: "+floderPath);
            }
        }
        return floderPath;
    }

	// add by ycb, create file path with date and time
	public static String getFileFolderPathByDate(Context context)
    {
		String floderPath ="";
        //想要保存文重点文件(长按物理按键)
        if (Setup.isEmphasisFile)
        {
            floderPath  =  getEmphFileFolder(context);

			// 设备/警员 编号文件夹
			floderPath += ("/"+Setup.devNum);
			floderPath += ("/"+Setup.policeNumber);
			
			floderPath += ("/"+TimeUtils.getLogDate());
			// Tag by ycb
			Log.i(TAG, "IMP folder path: "+floderPath);

        }else//未长按物理按键（分为短按和未按）//非重点文件夹的路径
        {

			floderPath = getComFileFolder(context);

			// 设备/警员 编号文件夹
			floderPath += ("/"+Setup.devNum);
			floderPath += ("/"+Setup.policeNumber);
			floderPath += ("/"+TimeUtils.getLogDate());
			// Tag by ycb
			AppLog.i(TAG, "Com folder path: "+floderPath);
            /*if (Setup.VideoPrerecord && !Setup.isNormalRecording)//没有按下物理按键
            {
                floderPath = getSegmentFilePath(context);

				// 设备/警员 编号文件夹
				floderPath += ("/"+Setup.devNum);
				floderPath += ("/"+Setup.policeNumber);
				floderPath += ("/"+TimeUtils.getLogDate());
				// Tag by ycb
				Log.i(TAG, "Segment folder path: "+floderPath);
				
            }else                                             //按下物理按键(短按)
            {
                floderPath = getComFileFolder(context);

				// 设备/警员 编号文件夹
				floderPath += ("/"+Setup.devNum);
				floderPath += ("/"+Setup.policeNumber);
				floderPath += ("/"+TimeUtils.getLogDate());
				// Tag by ycb
				Log.i(TAG, "Com folder path: "+floderPath);
            }*/
        }
        return floderPath;
	}

    //递归获得每个文件的绝对路径
    public static List getFilePath(String dirPath){
        List<String> fileList = new LinkedList<>();
        File dir = new File(dirPath);
        File[] files = dir.listFiles();
        if(files == null){
            return null;
        }
        for(int i = 0; i < files.length; i++){
            if(files[i].isDirectory()){
                getFilePath(files[i].getAbsolutePath());
            }
            else {
                fileList.add(files[i].getAbsolutePath());
            }
        }
        return fileList;
    }





    //**********************************************************************************************************

    /**
     * 打开文件
     * @param file
     */
    public static void openFile(File file, Context context){

        Intent intent = new Intent();
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        //设置intent的Action属性
        intent.setAction(Intent.ACTION_VIEW);
        //获取文件file的MIME类型
        String type = getMIMEType(file);
        //设置intent的data和Type属性。
        intent.setDataAndType(/*uri*/Uri.fromFile(file), type);
        //跳转
        context.startActivity(intent);

    }

    /**
     * 根据文件后缀名获得对应的MIME类型。
     * @param file
     */
    public static String getMIMEType(File file) {

        String type="*/*";
        String fName = file.getName();
        //获取后缀名前的分隔符"."在fName中的位置。
        int dotIndex = fName.lastIndexOf(".");
        if(dotIndex < 0){
            return type;
        }
    /* 获取文件的后缀名*/
        String end=fName.substring(dotIndex,fName.length()).toLowerCase();
        if(end=="")return type;
        //在MIME和文件类型的匹配表中找到对应的MIME类型。
        for(int i=0;i<MIME_MapTable.length;i++){ //MIME_MapTable??在这里你一定有疑问，这个MIME_MapTable是什么？
            if(end.equals(MIME_MapTable[i][0]))
                type = MIME_MapTable[i][1];
        }
        return type;
    }


    private static final String[][] MIME_MapTable={
            //{后缀名，MIME类型}
            {".3gp",    "video/3gpp"},
            {".apk",    "application/vnd.android.package-archive"},
            {".asf",    "video/x-ms-asf"},
            {".avi",    "video/x-msvideo"},
            {".bin",    "application/octet-stream"},
            {".bmp",    "image/bmp"},
            {".c",  "text/plain"},
            {".class",  "application/octet-stream"},
            {".conf",   "text/plain"},
            {".cpp",    "text/plain"},
            {".doc",    "application/msword"},
            {".docx",   "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {".xls",    "application/vnd.ms-excel"},
            {".xlsx",   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            {".exe",    "application/octet-stream"},
            {".gif",    "image/gif"},
            {".gtar",   "application/x-gtar"},
            {".gz", "application/x-gzip"},
            {".h",  "text/plain"},
            {".htm",    "text/html"},
            {".html",   "text/html"},
            {".jar",    "application/java-archive"},
            {".java",   "text/plain"},
            {".jpeg",   "image/jpeg"},
            {".jpg",    "image/jpeg"},
            {".js", "application/x-javascript"},
            {".log",    "text/plain"},
            {".m3u",    "audio/x-mpegurl"},
            {".m4a",    "audio/mp4a-latm"},
            {".m4b",    "audio/mp4a-latm"},
            {".m4p",    "audio/mp4a-latm"},
            {".m4u",    "video/vnd.mpegurl"},
            {".m4v",    "video/x-m4v"},
            {".mov",    "video/quicktime"},
            {".mp2",    "audio/x-mpeg"},
            {".mp3",    "audio/x-mpeg"},
            {".mp4",    "video/mp4"},
            {".mpc",    "application/vnd.mpohun.certificate"},
            {".mpe",    "video/mpeg"},
            {".mpeg",   "video/mpeg"},
            {".mpg",    "video/mpeg"},
            {".mpg4",   "video/mp4"},
            {".mpga",   "audio/mpeg"},
            {".msg",    "application/vnd.ms-outlook"},
            {".ogg",    "audio/ogg"},
            {".pdf",    "application/pdf"},
            {".png",    "image/png"},
            {".pps",    "application/vnd.ms-powerpoint"},
            {".ppt",    "application/vnd.ms-powerpoint"},
            {".pptx",   "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
            {".prop",   "text/plain"},
            {".rc", "text/plain"},
            {".rmvb",   "audio/x-pn-realaudio"},
            {".rtf",    "application/rtf"},
            {".sh", "text/plain"},
            {".tar",    "application/x-tar"},
            {".tgz",    "application/x-compressed"},
            {".txt",    "text/plain"},
            {".wav",    "audio/x-wav"},
            {".wma",    "audio/x-ms-wma"},
            {".wmv",    "audio/x-ms-wmv"},
            {".wps",    "application/vnd.ms-works"},
            {".xml",    "text/plain"},
            {".z",  "application/x-compress"},
            {".zip",    "application/x-zip-compressed"},
            {"",        "*/*"}
    };





    /**
     * 获取重点文件夹路径
     * @param context
     * @return
     */
    private static String getEmphFileFolder(Context context)
    {
        String floderPath = null;


            if (SDCardUtils.isSDCardEnable()&&SDCardUtils.judgeSDCardFreeSizeIsPlenty(context))
            {
                floderPath = Setup.interVideoPaths+Setup.emphFileFolder;
            }else
            {
                Toast.makeText(context, "SD卡不可用", Toast.LENGTH_SHORT).show();
                return "";
        }
        return floderPath;
    }


    /**
     * 获取分段录制临时文件夹路径
     * @param context
     * @return
     */
    private static String getSegmentFilePath(Context context){

        String floderPath = "";

            if (SDCardUtils.isSDCardEnable() && SDCardUtils.judgeSDCardFreeSizeIsPlenty(context))
            {
                floderPath = Setup.interVideoPaths+Setup.segmentedTemp;
            }else
            {
                return "";
            }

        return floderPath;
    }

    /**
     * 获取普通文件路径
     * @param context
     * @return
     */
    private static String getComFileFolder(final Context context){
        String floderPath = null;

            if (SDCardUtils.isSDCardEnable() && SDCardUtils.judgeSDCardFreeSizeIsPlenty(context))
            {
                floderPath = Setup.interVideoPaths+Setup.comFileFolder;
            }else
            {
                return "";
            }
        return floderPath;
    }


//    public static String getExtFileSavedPath(Context context) {
//        if (SDCardUtils.isExternalSDCardEnable(context)) {
//            return Setup.videoPaths;
//        }
//        return "";
//    }
    public static String getInnerFileSavedPaht(Context context){
        if (SDCardUtils.isSDCardEnable()){
            return Setup.interVideoPaths;
        }
        return "";
    }


    public static List<String> getFilesPath(String dir){
        List<String> paths = new ArrayList<>();

        File file = new File(dir);
        if (!file.exists()){
            return  null;
        }
        File [] files = file.listFiles();
        for (int i=0;i<files.length;i++){
            paths.add(files[i].getAbsolutePath());
        }
        Collections.sort(paths);
        return paths;
    }


    public static boolean getFileNum(){
        File file = new File(Setup.pre_file_dir);
        File [] files = file.listFiles();
        if (files == null){
            return false;
        }
        if (files.length<1){
            FileUtils.deleteFolder(Setup.pre_file_dir);
            return false;
        }else {
            return true;
        }
    }



    public interface OnFileCopyListener{
        public void onFileCopyStart();
        public void onFileCopyFinished();
    }

    public OnFileCopyListener getFileCopyListener() {
        return fileCopyListener;
    }

    public void setFileCopyListener(OnFileCopyListener fileCopyListener) {
        this.fileCopyListener = fileCopyListener;
    }

    public static OnFileCopyListener fileCopyListener;
}
