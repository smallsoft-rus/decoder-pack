@echo off
if "%OS%"=="Windows_NT" goto NT
%WINDIR%\system\regsvr32.exe DCBassSource.ax
goto END
:NT
regsvr32.exe DCBassSource.ax
:END
