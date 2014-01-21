rem Czyœci wszystkie wystapienia:
rem 1. CMakeCache
rem 2. cmake_install
rem 3. CMakeFiles
rem 4. Katalog bin

rem Dodatkowo, jeœli parametr wejsciowy jest ALL to czyœci:
rem 5. CMake_trash (ponowny download)
rem 6. doc/html
rem 7. Makefile
rem 8. Zawartoœæ Third_Party (ponowna kompilacja)

echo off
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 goto errorext

FOR /R %%A IN (*) DO IF %%~nA%%~xA==cmake_install.cmake del /q %%~fA
FOR /R %%A IN (*) DO IF %%~nA%%~xA==CMakeCache.txt del /q %%~fA
FOR /D /R %%X IN (CMakeFiles*) DO rmdir "%%X" /s /q
rmdir bin /s /q
IF "%1"=="ALL" goto :CLEAR_ALL
goto :EOF

:CLEAR_ALL
echo Cleaning everything...
rmdir CMake_trash /s /q
rmdir doc\html /s /q
FOR /R %%A IN (*) DO IF %%~nA%%~xA==Makefile del /q %%~fA
FOR /D /R %%X IN (Third_Party\*) DO rmdir "%%X" /s /q
goto :EOF

:errorext
echo Unable to enable extensions