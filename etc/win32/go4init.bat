:: adjust here actual path where go4 was extracted 

@echo Set environment for root and go4
@set ROOTSYS=%cd%\root
@set GO4SYS=%cd%\go4
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%


:: uncommented and adjusted actual location of Qt (required to run go4, compiled from source)

:: set QTDIR=C:\Soft\qt482
:: @set LIB=%LIB%;%QTDIR%\lib
:: @set PATH=%PATH%;%QTDIR%\bin

:: if one need to compile user analysis, following lines should be 
:: uncommented and adjusted to actual location of MS Visual 

:: call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

 