:: detect path where script is located

:: configure external Qt5 location

:: @set QTDIR=C:/Qt5/5.15.2/msvc2019
:: @set LIB=%QTDIR%/lib;%LIB%
:: @set PATH=%QTDIR%/bin;%PATH%

:: configure external ROOT

:: call C:/Soft/root/bin/thisroot.bat

:: detect current dir

@set OLDPATH=%cd%
@set THIS=%0
@set THIS=%THIS:~0,-12%.
@cd /D %THIS%
@set GO4SYS=%cd%
@cd /D %OLDPATH%

:: configure Qt5 distributed with go4

@set QTDIR=%GO4SYS%/qt5
@set PATH=%QTDIR%/bin;%PATH%

:: configure ROOT distributed with go4

call %GO4SYS%/root/bin/thisroot.bat

:: configure Go4 pathes

@set PATH=%GO4SYS%\bin;%PATH%
@set LIB=.;%GO4SYS%\lib;%LIB%

:: show configuration

@echo Set environment for root and go4
@echo    GO4SYS = %GO4SYS%
@echo    ROOTSYS = %ROOTSYS%
@echo    QTDIR = %QTDIR%

:: if one need to compile user analysis, following lines should be
:: uncommented and adjusted to actual location of MS Visual

:: call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
