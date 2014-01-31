rem Warper to PanthLogConverter
rem Colorizes html file
rem Requires config file in program dir

rem LogConvert file.pantlog

set current=%cd%

PanthLogFormatter -c %current%\config %1
ansifilter -H -s 11 -F "Consolas" -i %1_a -o %1.html
