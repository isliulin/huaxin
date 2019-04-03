set ver="5.0"

echo "NBK600EXRT mainboard program"
cd ..
Stm32Code.exe
cd "md5"
echo "NBK600EXRT rfboard program"
cd "..\..\..\Nbk600EX - RF\Projects\zstack\Samples\SampleApp com\CC2530DB\CoordinatorEB\Exe"
CC2530CODE.exe
cd "..\..\..\..\..\..\..\..\Nbk600EX\IAP\md5"
copy "..\firmware.bin" "firmware.bin"
copy "..\..\..\Nbk600EX - RF\Projects\zstack\Samples\SampleApp com\CC2530DB\CoordinatorEB\Exe\cc2530.bin" "cc2530.bin"
echo "NBK600EXRT combin program, "MCU 1# firmware", "MCU 2# firmware", "Tail data", "Output file name", "version""
md5combin.exe "firmware.bin" "cc2530.bin" "cc2530_rear.jpg" "SpeedLight600ex.fmw" %ver% >> log.txt
::copy HxStex-1.fmw SpeedLight600ex.fmw
echo "finished!"
pause
