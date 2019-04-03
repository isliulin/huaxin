package gbpe.policeass.uploadfile;

import android.util.Log;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.RandomAccessFile;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.utils.SDCardUtils;


public class TransferClient1 {
    //socket 多线程网络传输多个文件
    public static boolean isFirstSendFileToServer = true;
    private static ArrayList<String> fileList = new ArrayList<String>();
    private static String dirPath = "";
    private String sendFilePath = "D:\\Videos";
    private static int policeNumber = Integer.parseInt(Setup.policeNumber);
    private static final String TAG = "TAG";
    private static int fileCount;
    public static boolean isCancle = false;
    /**
     * 带参数的构造器，用户设定需要传送文件的文件夹
     * @param filePath
     */
    public TransferClient1(String filePath){

        getFilePath(filePath);
    }


//    /**
//     * 不带参数的构造器。使用默认的传送文件的文件夹
//     */
//    public TransferClient1(){
//        getFilePath(sendFilePath);
//    }



    /**
     * 一键上传
     */
    public TransferClient1(){
        //getFilePath(sendFilePath);
        getDirPath();
    }

    /**
     * 带参数的构造器，用户设定需要传送文件的文件夹

     */
    public TransferClient1(ArrayList<String> fileList){
        this.fileList = fileList;
    }


    public void service(MyCallBack callBack){
        //ExecutorService executorService = Executors.newCachedThreadPool();//创建和上传的文件数一样多的线程，当文件较多时，会占满手机内存，适合少量文件，频繁上传。
        ExecutorService executorService =
                Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors());//根据系统资源设置线程次的大小


        for(int i=0;i<fileList.size();i++){
            if (isCancle){
                fileCount =0;
                isCancle = false;
            }
            String filePath = fileList.get(i);
            executorService.execute(sendFile(filePath,callBack));
        }
         fileCount =0;
    }



    //递归获得每个文件的绝对路径
    private void getFilePath(String dirPath){
        File dir = new File(dirPath);
        File[] files = dir.listFiles();
        if(files == null){
            return;
        }
        for(int i = 0; i < files.length; i++){
            if(files[i].isDirectory()){
                getFilePath(files[i].getAbsolutePath());
            }
            else {
                fileList.add(files[i].getAbsolutePath());
            }
        }
    }

    //每次开辟一个新的线程传送文件（每个文件对应一个线程）
    private static Runnable sendFile(final String filePath, final MyCallBack callBack){
        return new Runnable(){
            private Socket socket = null;
            private String ip = Setup.SERVER_IP;// 服务器IP地址
            private int port = Setup.SERVER_PORT;//服务器 端口号
            public void run() {
                if (callBack !=null){
                    callBack.onFileStartUpload();
                }
                Log.d(TAG,"开始发送文件:" + filePath);
                File file = new File(filePath);

                if(createConnection()){         //判断是否连接成功
                    int bufferSize = 8192;
                    byte[] buf = new byte[bufferSize];
                    try {
                        DataInputStream fis = new DataInputStream(
                                new BufferedInputStream(new FileInputStream(filePath)));//从文件中读取数据
                        DataOutputStream dos = new DataOutputStream(socket.getOutputStream());

                        DataInputStream dis = new DataInputStream(socket.getInputStream());//从服务器中读取数据

                        dos.writeUTF(file.getName());//先向服务器发送文件的名字
                        dos.flush();
                        dos.writeLong(file.length());//向服务器发送文件的长度
                        dos.flush();
                        dos.writeInt(policeNumber);//向服务器发送一个整形
                        dos.flush();

                        String marklength= dis.readUTF();//发送前先从服务器读取命令

                        if("noexsits".equals(marklength) || isFirstSendFileToServer ){//服务器端不存在该文件，故第一次上传该文件
                            isFirstSendFileToServer = false;
                            Log.d(TAG,"文件["+file.getName()+"]直接传输!");
                            int num = 0;
                            long numlth =0;
                            while((num=fis.read(buf))!=-1){
                                dos.write(buf,0,num);
                                dos.flush();
                                numlth+=num;
                                //Setup.setCurrentProgress(num);//设置当前该文件的进度值

                         		System.out.println("文件["+file.getName()+"]已传送"+numlth*100L/file.length()+"%");
                            }

                            dos.flush();
                            this.socket.shutdownOutput();
                            String record = dis.readUTF();//从服务器获取传输文件传输成功的命令
                            if (record.equals("90000")){//如何文件上传服务器成功就将文件从手机或其它Android设备中删除（上传不成功则不删除）
                                Log.e("Main","不存在直接传送了");
                                file.delete();//删除该文件
                            }
                            Log.d(TAG,"文件["+file.getName()+"]传输结束1:"+record);
                        }else{         // 文件传送时被中断，再次连接网络时从断点处开始传送（断点续传）

                            System.out.println("读取服务器端["+file.getName()+"]文件长度!");
                            long serlth = Long.parseLong(marklength);
                            RandomAccessFile raf = new RandomAccessFile(filePath,"r");
                            raf.skipBytes((int)serlth);
                            int serstart =0;

                            while((serstart=raf.read(buf))!=-1){
                                dos.write(buf,0,serstart);
                                dos.flush();
                                serlth+=serstart;//改变指针指向
                                
                             //   Setup.setCurrentProgress(serstart);

//                        		System.out.println("文件["+file.getName()+"]已传送"+serlth*100L/file.length()+"%");
                            }

                            dos.flush();
                            raf.close();
                            this.socket.shutdownOutput();
                            String record = dis.readUTF();//从服务器获取传输文件传输成功的命令
                            if (record.equals("90000")){//如何文件上传服务器成功就将文件从手机或其它Android设备中删除（上传不成功则不删除）
                                Log.e("Main","续传完成删除文件");
                                file.delete();//删除该文件
                            }
                            System.out.println("文件["+file.getName()+"]传输结束2:"+record);
                        }

                        fis.close();
                        dos.close();
                        dis.close();
                        socket.close();

                        if (callBack !=null){
                            callBack.onFileFinishedUpload(++fileCount);
                        }
//                       System.out.println("文件 " + filePath + "传输完成!");
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }

            private boolean createConnection() {
                try {
                    socket = new Socket(ip, port);
                    Log.d(TAG,"连接服务器成功！");
                    return true;
                } catch (Exception e) {
                    Log.d(TAG,"连接服务器失败！");
                    return false;
                }
            }

        };
    }

    private void getDirPath(){
//            dirPath = Setup.imagePaths;
//            getFilePath(dirPath);
//            dirPath = Setup.videoPaths;
//            getFilePath(dirPath);
//            dirPath = Setup.soundPaths;
//            getFilePath(dirPath);
        dirPath = Setup.videoPaths;
        getFilePath(dirPath);
    }



    public interface MyCallBack{//定义客户端回调函数，让fragment实现该回调接口（将当前的文件读取进度传给UI线程，更新UI的值即进度条的进度值）
        public void onFileStartUpload();
        public void onFileFinishedUpload(int fileCount);
        public void onFileCancleUpload();

     }

}
