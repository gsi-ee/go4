# adjust here actual path where go4 was extracted 

@echo Set enviroment for root and go4
@set ROOTSYS=Q:\root
@set GO4SYS=Q:\go4
@set PATH=%PATH%;%ROOTSYS%\bin;%GO4SYS%\bin
@set LIB=%ROOTSYS%\lib;%GO4SYS%\lib;%LIB%



# if one need to compile user anaysis, following lines should be 
# uncommented and adjusted to actual location of MS Visual C++, SDK and IDE

#set VC_PATH=E:\Soft\VS\VC
#set SDK_PATH=E:\Soft\SDK
#set IDE_PATH=E:\Soft\VS\Common7\IDE
#set GO4_OS=Win32
#set INCLUDE=%VC_PATH%\include;%SDK_PATH%\include;
#set LIB=%LIB%;%VC_PATH%\lib;%SDK_PATH%\Lib
#set PATH=%PATH%;%VC_PATH%\bin;%SDK_PATH%\bin;%IDE_PATH%

 