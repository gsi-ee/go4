:: adjust here actual path where go4 was extracted 

@echo Start go4 executable
@set ROOTSYS=C:\Soft\root
@set GO4SYS=C:\Soft\go4
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%

start go4.exe
