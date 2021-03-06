package gbpe.policeass.utils;

import android.util.Log;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import static java.lang.Runtime.getRuntime;

/**
 * Created by Administrator on 2016/11/23 0023.
 */

public class ShellUtils {
    //shell进程
    private Process process;
    //对应进程的3个流
    private BufferedReader successResult;
    private BufferedReader errorResult;
    private DataOutputStream os;
    //是否同步，true：run会一直阻塞至完成或超时。false：run会立刻返回
    private boolean bSynchronous;
    //表示shell进程是否还在运行
    private boolean bRunning = false;

	private static final String TAG = "ShellUtils";
    //同步锁
    ReadWriteLock lock = new ReentrantReadWriteLock();

    //保存执行结果
    private StringBuffer result = new StringBuffer();

    /**
     * 构造函数
     *
     * @param synchronous true：同步，false：异步
     */
    public ShellUtils(boolean synchronous) {
        bSynchronous = synchronous;
    }

    /**
     * 默认构造函数，默认是同步执行
     */
    public ShellUtils() {
        bSynchronous = true;
    }

    /**
     * 还没开始执行，和已经执行完成 这两种情况都返回false
     *
     * @return 是否正在执行
     */
    public boolean isRunning() {
        return bRunning;
    }

    /**
     * @return 返回执行结果
     */
    public String getResult() {
        Lock readLock = lock.readLock();
        readLock.lock();
        try {
            AppLog.i(TAG, "getResult");
            return new String(result);
        } finally {
            readLock.unlock();
        }
    }

    /**
     * 执行命令
     *
     * @param command eg: cat /sdcard/test.txt
     * 路径最好不要是自己拼写的路径，最好是通过方法获取的路径
     * example：Environment.getExternalStorageDirectory()
     * @param maxTime 最大等待时间 (ms)
     * @return this
     */
    public ShellUtils run(String command, final int maxTime) {
        AppLog.i(TAG, "run command:" + command + ",maxtime:" + maxTime);
        if (command == null || command.length() == 0) {
            return this;
        }

        try {
            process = Runtime.getRuntime().exec("su");//申请root权限
            //process.waitFor();
        } catch (Exception e) {
            AppLog.e(TAG,"申请root失败: "+e.toString());
            return this;
        }

        bRunning = true;
        successResult = new BufferedReader(new InputStreamReader(process.getInputStream()));
        errorResult = new BufferedReader(new InputStreamReader(process.getErrorStream()));
        os = new DataOutputStream(process.getOutputStream());

        try {
            //向sh写入要执行的命令
            os.write(command.getBytes());
            os.writeBytes("\n");
            os.flush();

            os.writeBytes("exit\n");
            os.flush();

            os.close();
            //如果等待时间设置为非正，就不开启超时关闭功能
            if (maxTime > 0) {
                //超时就关闭进程
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            Thread.sleep(maxTime);
                        } catch (Exception e) {
                        }
                        try {
                            int ret = process.exitValue();
                            AppLog.i(TAG, "exitValue Stream over"+ret);
                        } catch (IllegalThreadStateException e) {
                            AppLog.e(TAG, "take maxTime,forced to destroy process");
                            process.destroy();
                        }
                    }
                }).start();
            }

            //开一个线程来处理input流
            final Thread t1 = new Thread(new Runnable() {
                @Override
                public void run() {
                    String line;
                    Lock writeLock = lock.writeLock();
                    try {
                        while ((line = successResult.readLine()) != null) {
                            line += "\n";
                            writeLock.lock();
                            result.append(line);
                            writeLock.unlock();
                        }
                    } catch (Exception e) {
                        AppLog.i(TAG, "read InputStream exception:" + e.toString());
                    } finally {
                        try {
                            successResult.close();
                            AppLog.i(TAG, "read InputStream over");
                        } catch (Exception e) {
                            AppLog.e(TAG, "close InputStream exception:" + e.toString());
                        }
                    }
                }
            });
            t1.start();

            //开一个线程来处理error流
            final Thread t2 = new Thread(new Runnable() {
                @Override
                public void run() {
                    String line;
                    Lock writeLock = lock.writeLock();
                    try {
                        while ((line = errorResult.readLine()) != null) {
                            line += "\n";
                            writeLock.lock();
                            result.append(line);
                            writeLock.unlock();
                        }
                    } catch (Exception e) {
                        AppLog.i(TAG, "read ErrorStream exception:" + e.toString());
                    } finally {
                        try {
                            errorResult.close();
                            AppLog.i(TAG, "read ErrorStream over");
                        } catch (Exception e) {
                            AppLog.e(TAG, "read ErrorStream exception:" + e.toString());
                        }
                    }
                }
            });
            t2.start();

            Thread t3 = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        //等待执行完毕
                        t1.join();
                        t2.join();
                        process.waitFor();
                    } catch (Exception e) {
						AppLog.e(TAG,e.toString());
                    } finally {
                        bRunning = false;
                        AppLog.i(TAG, "run command process end");
                    }
                }
            });
            t3.start();

            if (bSynchronous) {
                AppLog.i(TAG, "run is go to end");
                t3.join();
                AppLog.i(TAG, "run is end");
            }
        } catch (Exception e) {
            AppLog.e(TAG, "run command process exception:" + e.toString());
        }
        return this;
    }


    public static String execRootCmd(String cmd) {
        String result = "";
        DataOutputStream dos = null;
        DataInputStream dis = null;
        AppLog.i(TAG, cmd);

        try {
            Process p = Runtime.getRuntime().exec("su");// 经过Root处理的android系统即有su命令
            dos = new DataOutputStream(p.getOutputStream());
            dis = new DataInputStream(p.getInputStream());

            dos.writeBytes(cmd + "\n");
            dos.flush();
            dos.writeBytes("exit\n");
            dos.flush();
            String line = null;
            while ((line = dis.readLine()) != null) {
                AppLog.d(TAG, line);
                result += line;
            }
            p.waitFor();
        } catch (Exception e) {
            e.printStackTrace();
            AppLog.e(TAG,"execRootCmd err: "+e.toString());
        } finally {
            if (dos != null) {
                try {
                    dos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (dis != null) {
                try {
                    dis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return result;
    }


	public static String execAdbCmd(String cmd) {
        BufferedReader reader;
		try {
		    Process process = Runtime.getRuntime().exec(cmd);
		    reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
		    StringBuffer output = new StringBuffer();
		    int read;
		    char[] buffer = new char[4096];
		    while ((read = reader.read(buffer)) > 0) {
		        output.append(buffer, 0, read);
		    }
		    reader.close();
		    return output.toString();
		} catch (IOException e) {
		    e.printStackTrace();
		}
		return null;
	}


}
