:: adjust here actual path where go4 was extracted 

@echo Set environment for root and go4
@set ROOTSYS=C:\Soft\root
@set GO4SYS=C:\Soft\go4
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%


:: uncommented and adjusted actual location of Qt (required to run go4, compiled from source)

:: set QTDIR=C:\Soft\qt482
:: @set LIB=%LIB%;%QTDIR%\lib
:: @set PATH=%PATH%;%QTDIR%\bin

:: if one need to compile user analysis, following lines should be 
:: uncommented and adjusted to actual location of MS Visual C++

:: call E:\Soft\VC2010\VC\vcvarsall.bat

 