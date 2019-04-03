2015/06/17
1、改进了boot升级程序，在检测是否进入APP程序之后，才开始初始化usb接口。
2、增加了每次烧录程序前对烧录指针和CC2530的复位，防止误操作
2015/09/03
1、修改了睡眠低功耗设置，硬件上修改了在关闭3.3V和5.0V控制之后触发外部中断（PA15）。
2、EOS 750D的兼容性问题，
	① EOS 750D在有从灯链接时，在预闪信号后，会传递从灯色温信息。此时需要在预闪信号之后打开SPI功能。
	② 在预闪信号的下降沿，设置g_comStatue为IDLE状态，在预闪信号的上升沿，进入SPI初始设置，g_comStatue为START状态。
	③ 需要注意的是，在预闪信号上升沿已经设置了SPI的初始化，预闪之后，不要再SPI初始化，否则会触发一次SPI中断，但是状态机
	   错位。
2015/09/05
1、关闭3.3V和5V休眠，测试按键不能唤醒的原因：关闭3.3V导致RF模块断电，此时用于引导RF芯片进入升级模式的脚会处于拉低状态。
	 使得测试按钮输入一直为低。解决办法：在RF芯片引导进入升级模式的脚加二极管。
2016/10/26
1、在linkedshot模式下，ettl闪光从机不能触发主机快门，原因是在预闪之后，没有拉起输出脚。使得后面一段码值没有发出来。
注意：在引闪器新的方案上，需要做同样修改。

2017/04/06
1、Nbk600ex出现5d mark Iv的兼容问题，半按快门，辅助对角灯点不亮。ST-EX-RT没有该问题。测量发现nbk600ex在适应老相机提示充电完成的AF信号上使用的电阻参数不一样，将该电阻修改后正常
2、NBK600ex和ST-EX-RT新发现的问题，在按下对角点选择按钮时，通讯中断。在5d4相机上按对焦点选择按钮，会出现0xB7，0x00，之后通讯中断，在通讯超时处理函数判断sck 线是否为高，为高则继续等待接收数据。但该出设置后，会导致0xB7之后反复进入EXTI9_5_IRQHandler。在EXTI9_5_IRQHandler中会启动定时器，定时器超时又会进入EXTI9_5_IRQHandler。如此反复，具体原因未明，及时在UnlinkCmrHandle();中关闭中断也没用。暂时的方法为在EXTI9_5_IRQHandler增加SCK电平的判断

2017/08/07
1、对角灯的各个状态：
6D2是无通信状态0V，对焦信号2.7V，正常通信状态1.4V，按下快门瞬间3.5ms左右的0V跳变（储工测量）

2017/11/01
解决keil重复编译的办法：
1、project--option for target ‘xxx’---target---code generation--将use cross-Module optimization勾选去掉
2、project--option for target ‘xxx’---listing---将C Preprocessor Listing:\Listings*.l的勾选去掉
3、project--option for target ‘xxx’---output--将Create Batch File的勾选去掉

2017/12/3
佳能全光闪次数调整为全光40次
增加高温报警后延时恢复


