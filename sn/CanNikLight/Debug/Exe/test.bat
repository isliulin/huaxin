echo off
rem ���ݵ�ǰ���ڻ�ȡ�������մ�  
set yyyy=%date:~,4%  
echo %yyyy%
set mm=%date:~5,2%  
echo %mm%
set day=%date:~8,2%   
echo %day%
set "YYYYmmdd=%yyyy%%mm%%day%" 
echo %YYYYmmdd%
rem �������մ��еĿո��滻Ϊ0  
set "YYYYmmdd=%YYYYmmdd: =%"  
echo "YYYYmmdd%YYYYmmdd%YYYYmmdd" 
pause