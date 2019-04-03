set ver="4.8"
echo "NBK910 mainboard program"
Stm32Code "NikonStm32.bin" "firmware.bin" "NeverGiveupNikon"
md5combin.exe "firmware.bin" "cc2530.bin" "cc2530_rear.jpg" "SpeedLight_N.fmw" %ver% >> log.txt
echo "finished!"
pause
