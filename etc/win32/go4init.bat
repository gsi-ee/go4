:: adjust here actual path where go4 was extracted 

@echo Set enviroment for root and go4
@set ROOTSYS=Q:\root
@set GO4SYS=Q:\go4
@set PATH=%ROOTSYS%\bin;%GO4SYS%\bin;%PATH%
@set LIB=.;%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%



:: if one need to compile user anaysis, following lines should be 
:: uncommented and adjusted to actual location of MS Visual C++, SDK and IDE

:: set VC_PATH=C:\Program Files\Microsoft Visual Studio 9.0\VC
:: set SDK_PATH=C:\Program files\Microsoft SDKs\Windows\v6.0A
:: set IDE_PATH=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE
:: set INCLUDE=%VC_PATH%\include;%SDK_PATH%\include;
:: set LIB=%LIB%;%VC_PATH%\lib;%SDK_PATH%\Lib
:: set PATH=%PATH%;%VC_PATH%\bin;%SDK_PATH%\bin;%IDE_PATH%

 