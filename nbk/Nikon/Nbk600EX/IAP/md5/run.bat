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
echo "NBK600EXRT combin program"
md5combin.exe
copy HxStex-1.fmw SpeedLight600ex.fmw
echo "finished!"
pause
