:: adjust here actual path where go4 was extracted 

@echo Start go4 executable
@set GO4SYS=%cd%\go4
@set ROOTSYS=%cd%\root
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%

start go4.exe
