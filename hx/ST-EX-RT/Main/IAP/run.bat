@echo off
for /f "tokens=2 delims==" %%a in ('wmic path win32_operatingsystem get LocalDateTime /value') do (
  set t=%%a
)
set Today=%t:~0,4%-%t:~4,2%-%t:~6,2%
echo %Today%
set "ymd=CanonNikonInOne_%Today%.bin"
echo %ymd%
set dest="%ymd:-=%"
echo %dest%
BinEncrypto.exe "CanStm32.bin" "MakeTomorrowBest" "firmware.bin"