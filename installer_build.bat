rem installer build script
rem Copies all components into tmp directory
rem must be run from .
rem requires nsis installer on path

echo off
setlocal enabledelayedexpansion

set CURRENT_DIR=%CD%
rem root of project
IF EXIST tmp rmdir tmp /s /q
mkdir tmp

rem copy dlls from bin
copy bin\* tmp
rem copy installer
copy Configs\LV_Extensions.nsi tmp
rem copy dep
mkdir tmp\Windows
copy External_dep\libtiff\lib\glut32.dll tmp\Windows 
copy External_dep\libtiff\lib\jpeg62.dll tmp\Windows 
copy External_dep\libtiff\lib\libimage.dll tmp\Windows 
copy External_dep\libtiff\lib\libpng13.dll tmp\Windows 
copy External_dep\libtiff\lib\libtiff3.dll tmp\Windows 
copy External_dep\libtiff\lib\zlib1.dll tmp\Windows 
rem copy History
copy src\Changes.txt tmp

rem clean installer dir
del /Q Installer\*.*

cd tmp
rem prepare version to add to filename
..\tools\verbuild ver.h -c
rem wait 6 sec as required by verbuild
timeout /nobreak 6
..\tools\verbuild ver.h 0.0.+.0 -d2014
..\tools\grep VERSION_BUILDNO ver.h | ..\tools\sed "s/[^0-9]//g" > tmpfile
set /p vervar= < tmpfile
echo %vervar%
del tmpfile

makensis LV_Extensions.nsi
IF %ERRORLEVEL% NEQ 0 GOTO :END

rem copy ready installer to Installer
copy LV_Extensions_ver%vervar%.exe ..\Installer

rem cleaning
:END
cd %CURRENT_DIR%
rmdir tmp /s /q
