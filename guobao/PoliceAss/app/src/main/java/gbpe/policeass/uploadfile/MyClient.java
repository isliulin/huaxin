package gbpe.policeass.uploadfile;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.Setup.Setup;


public class MyClient extends Thread {

    public  MyClient(String str){
        if (str.equals("一键上传"))
        {
            dirPath = Setup.imagePaths;
            getFilePath(dirPath);
            dirPath = Setup.videoPaths;
            getFilePath(dirPath);
            dirPath = Setup.soundPaths;
            getFilePath(dirPath);
        }
        else {
            getFilePath(dirPath);
        }
    }
    public MyClient(List<String> fileList){ // 上传选择的图片至服务器
        this.fileList = fileList;
    }

    private List<String> fileList = new ArrayList<>();

    private static final String SERVER_IP ="192.168.1.211";
    private static final int SERVER_PORT =2016;
    private static  String dirPath = Setup.imagePaths;
    private int pliceNumber = 888888888;
    private Socket client;
    private FileInputStream fis;
    private DataOutputStream dos;
    @Override
    public void run() {

        for (int i = 0; i < fileList.size(); i++) {
            try {
                try {
                    System.out.println("dddd");
                    client = new Socket(SERVER_IP, SERVER_PORT);
                    System.out.println("连接服务器....");
                    //向服务端传送文件
                    File file = new File(fileList.get(i));
                    fis = new FileInputStream(file);
                    dos = new DataOutputStream(client.getOutputStream());

                    //文件名和长度
                    dos.writeUTF(file.getName());
                    dos.flush();
                    dos.writeLong(file.length());
                    dos.flush();
                    //警员编号：
                    dos.writeInt(pliceNumber);
                    dos.flush();

                    //传输文件
                    byte[] sendBytes = new byte[1024];
                    int length = 0;
                    while ((length = fis.read(sendBytes, 0, sendBytes.length)) > 0) {
                        dos.write(sendBytes, 0, length);
                        dos.flush();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    if (fis != null)
                        fis.close();
                    if (dos != null)
                        dos.close();
                    if (client!=null){
                        client.close();
                    }

                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    //递归法获得文件的路径
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
    //递归法创建文件夹
    public static void mkDir(File file){
        if(file.getParentFile().exists()){
            file.mkdir();
        }else{
            mkDir(file.getParentFile());
            System.out.println(file.getName());
            file.mkdir();
        }
    }

}


