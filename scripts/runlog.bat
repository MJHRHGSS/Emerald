@echo off
cd /d %~dp0
cd ..
gcc src\emerald.c -o out\out.o
for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd_HH-mm-ss"') do set timestamp=%%i
out\out.o tests\test.emr >> logs\emr_%timestamp%.log 2>&1
