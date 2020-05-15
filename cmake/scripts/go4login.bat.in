:: detect path where script is located 

@set OLDPATH=%cd%
@set THIS=%0
@set THIS=%THIS:~0,-11%.
@cd /D %THIS%
@set ROOTSYS=%cd%\root
@set GO4SYS=%cd%\go4
@cd /D %OLDPATH%

@echo Set environment for root and go4
@echo    GO4SYS = %GO4SYS%
@echo    ROOTSYS = %ROOTSYS%
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%


:: uncommented and adjusted actual location of Qt (required to run go4, compiled from source)

:: set QTDIR=C:\Soft\qt482
:: @set LIB=%LIB%;%QTDIR%\lib
:: @set PATH=%PATH%;%QTDIR%\bin

:: if one need to compile user analysis, following lines should be 
:: uncommented and adjusted to actual location of MS Visual 

:: call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
