echo off
rem 根据当前日期获取，年月日串  
set yyyy=%date:~,4%  
echo %yyyy%
set mm=%date:~5,2%  
echo %mm%
set day=%date:~8,2%   
echo %day%
set "YYYYmmdd=%yyyy%%mm%%day%" 
echo %YYYYmmdd%
rem 把年月日串中的空格替换为0  
set "YYYYmmdd=%YYYYmmdd: =%"  
echo "YYYYmmdd%YYYYmmdd%YYYYmmdd" 
pause