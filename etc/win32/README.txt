--------------------------------------------------------------
         Go4 Release Package v5.0.0 (build 50000)
                      25-June-2015
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

This package is compiled and tested with CYGWIN on Windows 7
with Microsoft Visual C++ 12.0 (2013) Express Edition,
using ROOT 5-34-30 and Qt 5.4.2.


1. INSTALLATION

- Run installation program. Full path name, where package is extracted,
  SHOULD NOT content any spaces. Therefore location like
  "C:\Program files" should be avoided. Default installation 
  location is "C:\Users\<username>\go4".
- Installation includes ROOT binraires, which can be found in "root" subfolder


2. STARTING ANALYSIS

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


3. STARTING GUI

- Installation creates shortcut icon and item in programs for Go4 GUI.
- After main window appear, one can use "Connect to HTTP server"
  button for connection with analysis, running in http server mode.
- Or one can start analysis directly from the GUI via 
  "Launch analysis" menu command.    
- Go4 GUI in Windows has mostly the same functionality as in Linux 
  beside the fact that analysis cannot be started from the GUI on 
  other node as is possible under Linux via ssh


4. STARTING GO4 browser in ROOT

- Run root from command prompt after go4init.bat was called
    C:\> root
- In CINT shell instantiate TGo4Interface class
    root [0] new TGo4Interface
  All go4 classes are available in ROOT via rootmap mechanism.  
- After TGo4Interface instance is created, two variables will be accessible:
  go4 (class TGo4Interface) and br (class TGo4BrowserProxy)
- Connect to running analysis server by command like
    root [1] go4->ConnectAnalysis("lxi001",5000,0,"password");
- If connection is successful (see log output), just start ROOT
  browser by command
    root [2] new TBrowser
- In browser new Go4 folder should exists, which represent structure
  of remote analysis. Draw of histograms and pictures are possible.
  Via right mouse menu monitoring flag for histograms can be switched
  on/off and monitoring can be toggled.
- Via go4 instance a various commands can be executed like:
    root [3] go4->StartAnalysis();
    root [4] go4->StopAnalysis();
  For more information see online documentation on TGo4Interface class
  on htpp://go4.gsi.de site


5. COMPILE USER ANALYSIS UNDER WINDOWS

  First of all, Microsoft VC++ should be installed.
  There is a free edition of Microsoft Visual C++ Express 12.0 (2013) on
       http://msdn.microsoft.com/vstudio/express/visualc/
  After VC++ is installed, there are two alternatives, 
  how example can be compiled.

  5.1. Compile analysis with CYGWIN

  First of all, CYGWIN (http://cygwin.com/) should be installed.
  This provides Unix like environment and allows to use shell-based 
  build scripts. To be able use VisualC from cygwin, one should call
  vcvarsall.bat file from the Cygwin.bat before bash is started. 
  One should add line like this:

     call "C:\Program files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"

  One also must ensure that link.exe from VisualC is used. 
  Problem description and several solutions can be found here:
     https://developer.mozilla.org/en/Windows_build_prerequisites_using_cygwin#Fix_cygwin_path
  Simplest solution - just delete or rename link.exe from cygwin;
  preferable - change sequence of $PATH. 

  After CYGWIN installed and configured, one should adjust "go4login" script, 
  specifying where ROOT, Qt and Go4 are installed. One must avoid path with spaces,
  therefore location "C:\Program Files\root" will not work.
   
  Than copy your analysis (or one of Go4 examples, for instance, Go4Example2Step) to separate location.
  To compile example:
     /cygdrive/q> cd /cygdrive/f/user/app1
     /cygdrive/f/user/app1> make clean 
     /cygdrive/f/user/app1> make all
  This should compile user analysis library. To start analysis, just call:
     /cygdrive/f/user/app1> go4analysis -random -http 8090

  5.2. Compile with Windows native nmake tool

  This method does not require CYGWIN. One should adjust go4init.bat
  (as described before) and prepare special Makefile for nmake tool.
  There is example: $GO4SYS\Go4Example2Step\Makefile.win, which
  is written for Go4Example2Step. To adopt it for other user analysis,
  one should specify correct list of source files in this Makefile. 
  To run compilation, just do:
     C:\Soft\> F:
     F:\> cd \User\App1
     F:\User\App1> nmake clean all -f Makefile.win
  Once library compiled, analysis can be started with command:
     F:\User\App1> go4analysis -random -http 8090


6. COMPILATION OF GO4 FROM SOURCE

- Download and install root (ROOT Windows binaries are enough)
- Install and configure CYGWIN
- Install Microsoft VC++
- Install Qt 4.8.x binaries package for MS VC compiler like
    https://download.qt.io/archive/qt/5.4/5.4.2/qt-opensource-windows-x86-msvc2013-5.4.2.exe
- Download go4 sources from http://go4.gsi.de web site.
- Find and modify $GO4SYS/etc/win32/go4login file to actual location of all 
  components, copy it to home cygwin directory
- Run Cygwin shell, call ". go4login", change to go4 directory and call "make"
- Hopefully, this will compile Go4 under Windows!

