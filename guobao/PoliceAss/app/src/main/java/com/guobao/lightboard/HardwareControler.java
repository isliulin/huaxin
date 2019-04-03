package com.guobao.lightboard;
import android.util.Log;

public class HardwareControler
{
	
 
	//Light控制概述：
	//参数LightState设定为1，灯亮；参数LightState设定为0，灯灭；其它值错误.
	//返回值：正确返回0；错误返回-1；
    //例如：激光灯控制：（LaserState = 1 ：激光灯开；LaserState = 0 ：激光灯关）
	
	//激光灯
	static public native int SetLaserState(int LaserState);
	
	//充电指示灯
	//static public native int SetChargingIndicatorState(int LightState);
	
	//白光灯
	static public native int SetWhiteLightState(int LightState);
	
	//红外灯
	static public native int SetInfraredLaserState(int LightState);
	
	//红灯
	static public native int SetRedLightState(int LightState);
	
	//绿灯
	static public native int SetGreenLightState(int LightState);
	
	//蓝灯
	static public native int SetBlueLightState(int LightState);

	//黄灯

	static public int SetYellowLightState(int LightState)////听说硬件要改动，把蓝灯的接口使用来控制黄灯。那么你按照我图中的java文件，
	                                                         // 修改一下黄灯的代码，你源码的所有的工作，就不用修改了。
	{
		int ret = -1;
		ret = SetBlueLightState(LightState);
//		if (LightState == 1) {
//			ret = SetRedLightState(1);
//			if (ret == -1) { return ret;}
//			ret = SetGreenLightState(1);
//		} else if (LightState == 0) {
//			ret = SetRedLightState(0);
//			if (ret == -1) { return ret;}
//			ret = SetGreenLightState(0);
//		}
		return ret;
	}



	//光敏检测 （返回值1：高电平，0：低电平；-1：错误）
	static public native int GetphotosensitiveState();



	//IR-CUT配套电机正反转控制
	//参数Direction为1：正转；参数Direction为0：反转；其它值：错误
	static public native int SetIrCutMotorRotation(int Direction);
	//停止IR-CUT电机的转动；正转或反转操作与停止操作最好有两秒钟以上，也不能太长，否则烧毁电机
	static public native int StopIrCutMotor();
	
	//建议ChipPowerOn()必须使用在所有IR-CUT操作之前，而且最好有一定的时间间隔
	//IR-CUT控制单元上电
	static public native int ChipPowerOn();
	//断电最好是不再对IR-CUT操作，或长时间不操作，最好对其断电；
	//IR-CUT控制单元断电；
	static public native int ChipPowerOFF();




    /* GPIO */
    static public native int exportGPIOPin(int pin);
    static public native int unexportGPIOPin(int pin);
    //GPIOEnum.LOW or GPIOEnum.HIGH
    static public native int setGPIOValue(int pin, int value);
    static public native int getGPIOValue(int pin);
    //GPIOEnum.IN or GPIOEnum.OUT
    static public native int setGPIODirection(int pin, int direction);
    static public native int getGPIODirection(int pin);

	
	
	/* test: print Hello World */
    static public native String HelloWorld();
    static public native int sum(int x,int y);
    
    static {
        try {        	
            System.loadLibrary("Hardware");

        } catch (UnsatisfiedLinkError e) {
            Log.e("HardwareControler", "libHardware library not found!");
        }
    }

}
