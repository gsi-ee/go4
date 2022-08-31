```
--------------------------------------------------------------
         Go4 Release Package v6.2.99 (build 60299)
                      31-Aug-2022
--------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at DVEE department, GSI
--------------------------------------------------------------
Authors: Joern Adamczewski-Musch      (1999-)
         Mohammad Al-Turany           (2000-2004)
         Denis Bertini                (2001-2004)
         Hans Georg Essel             (1999-2010)
         Marc Hemberger               (1999-2000)
         Sergey Linev                 (2002-)
---------------------------------------------------------------
Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------
```

## This is obsolete README for compilation of older go4 versions

This package is compiled and tested on Windows 10
with Microsoft Visual C++ 16 (2019), using ROOT 6-26-04 and Qt 5.15.6.


## INSTALLATION

- Run installation program. Full path name, where package is extracted,
  SHOULD NOT content any spaces. Therefore location like
  "C:\Program files" should be avoided. Default installation
  location is "C:\Users\<username>\go4".
- Installation includes ROOT binaries, which can be found in "root" subfolder


##  STARTING ANALYSIS

- Run windows command prompt with go4 variables set - provided as link
  in go4 program group menu
- Change to directory with user analysis:
    C:\Users\user1\go4> cd go4\Go4ExampleSimple
- Start analysis by command:
    C:\Users\user1\go4\go4\Go4ExampleSimple> go4analysis -random
- Start analysis with http server enabled:
    C:\Users\user1\go4\go4\Go4ExampleSimple> go4analysis -random -http 8080
- Analysis running http server, can be connected with any modern web browser
- Analysis also can be started and connected directly from the Go4 GUI


##  STARTING GUI

- Installation creates shortcut icon and item in programs for Go4 GUI.
- After main window appear, one can use "Connect to HTTP server"
  button for connection with analysis, running in http server mode.
- Or one can start analysis directly from the GUI via
  "Launch analysis" menu command.
- Go4 GUI in Windows has mostly the same functionality as in Linux
  beside the fact that analysis cannot be started from the GUI on
  other node as is possible under Linux via ssh


##  COMPILE USER ANALYSIS UNDER WINDOWS

  First of all, Microsoft VC++ should be installed.
  There is a free Visual Studio Community  on https://visualstudio.microsoft.com/
  After VC++ is installed, one just use cmake


## COMPILATION OF GO4 FROM SOURCE

- download and install qt5.15 version with webengine
- start VC **x64** console
- build ROOT 64 bit on windows with "-Dqt5web=ON" configured
- in VC console run:

    set PATH=%PATH%;C:\Qt5\5.15.2\msvc2019_64\bin
    call C:\Soft\root_64\bin\thisroot.bat
    cmake -G"Visual Studio 16 2019" -A x64 c:\git\go4
    cmake --build . --config Release -- /maxcpucount

- in normal windows shell start go4

   call C:\Soft\go4_64\go4login.bat
   go4
