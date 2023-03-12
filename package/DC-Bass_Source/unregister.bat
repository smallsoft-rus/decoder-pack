@echo off
if "%OS%"=="Windows_NT" goto NT
%WINDIR%\system\regsvr32.exe /U DCBassSource.ax
goto END
:NT
regsvr32.exe /U DCBassSource.ax
:END
