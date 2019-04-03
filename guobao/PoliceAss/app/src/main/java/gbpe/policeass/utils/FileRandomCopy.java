package gbpe.policeass.utils;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.os.Build;

import java.io.RandomAccessFile;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * =====================================================================================
 *
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 *
 * 作者：Administrator on 2017/5/18 19:19
 *
 * 邮箱：xjs250@163.com
 *
 * 创建日期：2017/5/18 19:19
 *
 * 描述：
 * =====================================================================================
 */
public class FileRandomCopy {
    /**
     * 源文件
     */
    public static String SRC_FILE = "F:/BaiduYunDownload/Android源码级分析.7z";
    /**
     * 目标文件
     */
    public static String DES_FILE = "G:/CaoJin/caojin.7z";

    /**
     * 缓冲区大小
     */
    public static int BYTE_ARRAY_SIZE = 1024 * 1024;
    /**
     * 线程数目
     */
    public static final int THREAD_COUNT = 4;


    /**
     * 拷贝文件
     * @param SRC_FILE 源文件
     * @param DES_FILE 目标文件
     */
    @SuppressLint("NewApi")
    public static void CopyFile(String SRC_FILE, String DES_FILE,OnFileCopyListener copyListener){
        System.out.println("RandomCopy 开始复制！");
        if (copyListener !=null){
            copyListener.onFileCopyStart();
        }
        long start = System.nanoTime();

        long fileLength = 0;
        try( // 首先创建一个大小为和原文件一样大的 空文件
             RandomAccessFile srcFile = new RandomAccessFile(SRC_FILE, "r");
             RandomAccessFile desFile = new RandomAccessFile(DES_FILE, "rw");){
            fileLength = srcFile.length();
            desFile.setLength(fileLength);
        }catch (Exception e) {
            e.printStackTrace();
        }

        CountDownLatch finishSignal = new CountDownLatch(THREAD_COUNT); // 记录所有子线程的完成状态
        ExecutorService threadPool = Executors.newFixedThreadPool(THREAD_COUNT);
        long copyLength = fileLength / THREAD_COUNT; // 每次复制的文件长度
        int i;
        for (i = 0; i < THREAD_COUNT - 1; i++){
            threadPool.execute(new RandomCopy(""+i,SRC_FILE, DES_FILE, i * copyLength, copyLength, finishSignal));
        }
        // 最后一个线程复制复制文件的剩部分，起始位置为i * copyLength，复制长度为fileLength - i*copyLength
        threadPool.execute(new RandomCopy(""+i,SRC_FILE, DES_FILE, i * copyLength, fileLength - i*copyLength, finishSignal));
        threadPool.shutdown(); // 关闭线程池入口

        try {
            finishSignal.await(); // 阻塞，直到 THREAD_COUNT 个线程全部完成才向下继续执行
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (copyListener !=null){
            copyListener.onFileCopyFinished();
        }
        System.out.println("RandomCopy用时：" + (System.nanoTime()-start)/Math.pow(10, 9));
    }





    private static   class RandomCopy implements Runnable{
        private String name; // 线程名字
        private String src; // 源文件路径
        private String des; // 目标文件路径
        private long offset; // 复制的起始位置
        private long copyLength; // 当前线程负责复制的长度
        private CountDownLatch finishSignal; // 本线程结束的标识
        public RandomCopy (String name,String src, String des, long offset, long copyLength, CountDownLatch finishSignal){
            this(name, src, des, offset, copyLength);
            this.finishSignal = finishSignal;
        }
        public RandomCopy(String name,String src, String des, long offset, long copyLength){
            this.name = name;
            this.src = src;
            this.des = des;
            this.offset = offset;
            this.copyLength = copyLength;
        }
        @TargetApi(Build.VERSION_CODES.KITKAT)
        @Override
        public void run() {
            System.out.println(name + " 开启");
            try( RandomAccessFile srcFile = new RandomAccessFile(src, "r");
                 RandomAccessFile desFile = new RandomAccessFile(des, "rw")){
                byte[] b = new byte[BYTE_ARRAY_SIZE];
                long hasCopyed = 0;
                int readOnce = 0;
                srcFile.seek(offset); // 找到开始复制的位置
                desFile.seek(offset); // 找到开始写的位置

                while( hasCopyed < copyLength ){
                    readOnce = srcFile.read(b);
                    if (readOnce < 0){
                        break;
                    }
                    desFile.write(b, 0, readOnce);
                    hasCopyed += readOnce;
                }
                System.out.println(name +" 执行完毕！");
            }catch (Exception e) {
                e.printStackTrace();
            }
            finishSignal.countDown(); // 通知当前线程已结束
        }
    }


    public interface OnFileCopyListener{
        public void onFileCopyStart();
        public void onFileCopyFinished();
    }
}


