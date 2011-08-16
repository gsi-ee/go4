:: adjust here actual path where go4 was extracted 

@echo Set environment for root and go4
@set ROOTSYS=Q:\root
@set GO4SYS=Q:\go4
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%


:: uncommented and adjusted actual location of Qt (required to run go4, compiled from source)

:: set QTDIR=E:\Soft\Qt471
:: @set LIB=%LIB%;%QTDIR%\lib
:: @set PATH=%PATH%;%QTDIR%\bin

:: if one need to compile user anaysis, following lines should be 
:: uncommented and adjusted to actual location of MS Visual C++

:: call E:\Soft\VC2010\VC\vcvarsall.bat

 