@echo off

cd %~dp0
ftp -i -n -s:fcmd
pause
