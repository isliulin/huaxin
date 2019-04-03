package gbpe.policeass.utils;

import android.app.Activity;
import android.content.Context;
//import android.util.Log;

import java.io.File;
import java.util.List;

import gbpe.policeass.Setup.Setup;


/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/5/15 16:31
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/5/15 16:31
 * 
 * 描述：
 * =====================================================================================
 */
public class SelectHandleUtil {

    private static final String TAG = "SelectHandleUtil";

      private SelectHandleUtil() {
     }

    /**
     * 网络（sokcet方式）上传文件到采集站
     * @param context
     * @param beSeletcedItemPaths
     * @param handleListener
     */
    public static void upLoad(final Context context,  final List<String>
                       beSeletcedItemPaths,OnFileHandleListener handleListener){


    }


    /**
     * 标记文件是否为重点文件
     * @param context
     * @param beSeletcedItemPaths
     * @param handleListener
     */
    public static void mark(final Context context, final List<String>
                  beSeletcedItemPaths, final OnFileHandleListener handleListener){

        new Thread(new Runnable()
        {
            @Override
            public void run()
            {

                if (beSeletcedItemPaths==null){
                    return;
                }
                for (int i=0;i<beSeletcedItemPaths.size();i++)
                {
                    //如果取消上传，则停止上传文件
                    if (Setup.isCancle) {
                        if (handleListener !=null) {
                            handleListener.onFileCancle();
                        }
                        Setup.isCancle = false;
                        Setup.fileCount=0;
                        return;
                    }
                    String filePath = beSeletcedItemPaths.get(i);
                    String newFilePath = changeFileType(filePath,context);
                   // copy(context,filePath,newFilePath,handleListener);
                    copy2(context,filePath,newFilePath,handleListener);
             }
                //所有的文件都拷贝完成之后的回调
                ((Activity)context).runOnUiThread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        Setup.isCancle = false;
                        Setup.fileCount=0;
                        if (handleListener !=null)
                        {
                            handleListener.onAllFileHandleFinished();
                        }
                    }
                });

            }
        }).start();
    }

    /**
     * 复制文件到另一个文件夹
     * @param context
     * @param oldfilePath
     * @param newFilePath
     * @param handleListener
     */
    private static void copy(final Context context, String oldfilePath,String newFilePath, final OnFileHandleListener handleListener){

        FileHelper.copy(oldfilePath, newFilePath, new FileHelper.OnFileCopyListener() {

                    @Override
                    public void onFileCopyStart() //开始拷贝文件
                    {
                        ((Activity)context).runOnUiThread(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                if (handleListener !=null)
                                {
                                    handleListener.onFileHandleStart();
                                }
                            }
                        });

                    }
                    //文件拷贝完成
                    @Override
                    public void onFileCopyFinished()
                    {
                        ((Activity)context).runOnUiThread(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                ++Setup.fileCount;
                                if (handleListener !=null)
                                {
                                       //  f.delete(); //拷贝完成之后删除当前文件夹下的文件
                                    handleListener.onOneFileHandleFinished(Setup.fileCount);
                                }
                            }
                        });
                    }
                });
    }


    /**
     * 标记文件（标记普通或重点文件）
     * @param filePath
     */
    public static String changeFileType(String filePath,Context context){

        //String newFilePath = null;
        File file = new File(filePath);
        String fileName = file.getName();//获得文件名
        // add by ycb
        //String oldPath = file.getPath();
        String oldPath = file.getParent()+"/";

        String tempFileName = fileName.substring(0,fileName.indexOf("."));//获取文件标题（即去文件名去掉后缀部分）
        String fileSuffix = fileName.substring(fileName.indexOf("."));//获取文件后缀
        //非重点文件可以被修改（加上IMP，拷贝到重点文件夹面）
        AppLog.i(TAG,"old path: "+oldPath);
        AppLog.i(TAG, "changeFileType: "+filePath+", tempFileName: "+tempFileName+", suffix: "+fileSuffix);

        if (!emphasisFile(filePath))
        {
            // 获得目标文件名
            String newPath = oldPath.replaceAll("100ANDRO","101ANDRO");
            AppLog.i(TAG,"IMP dest path: "+newPath);
            //File folder = new File(SDCardUtils.getSDCardPath()+"/DCIM/"+"101ANDRO/");
            File folder = new File(newPath);
            if (!folder.exists()){
               if (!folder.mkdirs())
               {
                   AppLog.i(TAG,"创建重点文件夹失败");
                   return null;
               }
            }
            //String newFileName = "DCIM/"+"101ANDRO/"+tempFileName+"IMP"+fileSuffix;//标记文件
            String newFileName = newPath + tempFileName +"IMP"+ fileSuffix;
            AppLog.i(TAG, "new file name: "+newFileName);
			return newFileName;


//            if (SDCardUtils.isExternalSDCardEnable(context)&&SDCardUtils.isExternalSDCardCanWriteOrRead(context)){
//                newFilePath = SDCardUtils.getExternalSDCardPath(context)+newFileName;
//            }else if (SDCardUtils.isSDCardEnable()){
//                newFilePath = SDCardUtils.getSDCardPath()+newFileName;
//            }
             //newFilePath = getNewFilePath(context,newFileName);
             //AppLog.i(TAG, "new file path: "+newFilePath);
            //  copyFile(filePath,newFilePath);//将修改后的文件拷贝到新文件夹下面
            //return newFilePath;
        }else//重点文件标记为非重点文件（去掉IMP,然后拷贝到普通文件夹）
        {
            String tempFileName2 = tempFileName.substring(0,tempFileName.indexOf("I"));//去掉重点文件中的IPM后缀。
            AppLog.i(TAG,"tempFileName2; "+tempFileName2);

            // String newFileName = "DCIM/"+"100ANDRO/"+tempFileName2+fileSuffix;//标记文件

            String newPath = oldPath.replaceAll("101ANDRO","100ANDRO");
            //File folder = new File(SDCardUtils.getSDCardPath()+"/DCIM/"+"100ANDRO/");
            AppLog.i(TAG,"newPath: "+newPath);
            File folder = new File(newPath);
            
            if (!folder.exists()){
                if (!folder.mkdirs())
                {
                    AppLog.i(TAG,"创建非重点文件夹失败");
                    return null;
                }
            }

            String newFileName = newPath + tempFileName2 + fileSuffix;
			AppLog.i(TAG,"new file name: "+newFileName);
			
            return newFileName;
//            if (SDCardUtils.isExternalSDCardEnable(context)&&SDCardUtils.isExternalSDCardCanWriteOrRead(context)){
//                newFilePath = SDCardUtils.getExternalSDCardPath(context)+newFileName;
//            }else if (SDCardUtils.isSDCardEnable()){
//                newFilePath = SDCardUtils.getSDCardPath()+newFileName;
//            }
//            newFilePath = getNewFilePath(context,newFileName);
            //  copyFile(filePath,newFilePath);//将修改后的文件拷贝到新文件夹下面
//            return newFilePath;
        }
    }

    /**
     * 获得新的文件的绝对路径
     * @param context
     * @param newFileName
     * @return
     */
    public static String getNewFilePath(Context context,String newFileName){
        String newFilePath = "";

      if (SDCardUtils.isSDCardEnable()){
            newFilePath = SDCardUtils.getSDCardPath()+newFileName;
        }

        return newFilePath;
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



    public static void copy2(final Context context, final String oldPath, String newPath, final OnFileHandleListener handleListener){
        FileRandomCopy.CopyFile(oldPath, newPath, new FileRandomCopy.OnFileCopyListener() {
            @Override
            public void onFileCopyStart() {
                ((Activity)context).runOnUiThread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        if (handleListener !=null)
                        {
                            handleListener.onFileHandleStart();
                        }
                    }
                });
            }

            @Override
            public void onFileCopyFinished() {
                ((Activity)context).runOnUiThread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        ++Setup.fileCount;
                        if (handleListener !=null)
                        {
                                new File(oldPath).delete();
                                 //f.delete(); //拷贝完成之后删除当前文件夹下的文件
                            handleListener.onOneFileHandleFinished(Setup.fileCount);
                        }
                    }
                });
            }
        });



    }
    /**
     * 文件处理接口
     */
    public interface  OnFileHandleListener{
        public void onFileHandleStart();//文件开始处理
        public void onOneFileHandleFinished(int progress);//一个文件处理完成之后
        public void onAllFileHandleFinished();//所有文件都处理完成之后
        public void onFileCancle();//取消处理文件的操作
    }
}
