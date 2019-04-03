cd "D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP"
echo "excute mainboard program"
Stm32Code.exe
cd "D:\WorkReserve\project\NewStep\CanMstOsal\Projects\zstack\Samples\SampleApp com\CC2530DB\CoordinatorEB-Pro\Exe"
echo "excute rfboard program"
CC2530CODE.exe
copy "D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP\firmware.bin" "D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP\md5\firmware.bin"
copy "D:\WorkReserve\project\NewStep\CanMstOsal\Projects\zstack\Samples\SampleApp com\CC2530DB\CoordinatorEB-Pro\Exe\cc2530.bin" "D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP\md5\cc2530.bin"
cd D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP\md5
echo "excute combin program"
md5combin.exe
copy "D:\WorkReserve\project\NewStep\SpeedLight\canon\canon\IAP\md5\HxStex-1.fmw" "C:\Users\FreeMan\Desktop\issue\HxStex-1.fmw"
echo "finished!"
