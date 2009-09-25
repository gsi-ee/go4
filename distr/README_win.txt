--------------------------------------------------------------
         Go4 Release Package v4.4.0 (build 40400)
                      30-September-2009
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at DVEE department, GSI
------------------------------------------------------------
Authors: Joern Adamczewski      (1999-)
         Mohammad Al-Turany     (2000-2004)
         Denis Bertini          (2001-2004)
         Hans Georg Essel       (1999-)
         Marc Hemberger         (1999-2000)
         Sergey Linev           (2002-)
---------------------------------------------------------------
Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------

This package is compiled and tested under Windows XP SP2.

INSTALLATION:
- Extract content of binary Go4 archive. Full path name, where package
  is extracted, SHOULD NOT content any spaces. Therefore location like
  "C:\Program files" should not be used. Best if package will be extracted
  on top directory like "C:\" or "Q:\" or like that.
- After files are extracted, two top directories will be produced:
     root - binary distribution of ROOT (see http:\root.cern.ch)
     go4  - binary distribution of Go4
- Correct paths to ROOT and Go4 installation should be set in
  go4init.bat script, which is extracted in top directory.

STARTING GUI:
- Run windows command prompt via main windows menu command:
    "Start/Programs/Accessories/Command prompt"
- Change to directory, where package was extracted
- Execute go4init.bat batch file
    Q:\> go4init.bat
- Start GUI by command
    Q:\> go4
- One can create windows shortcut where all necessary variables are set
  and go4 gui can be started directly without command prompt
- Go4 GUI in windows has same functionality as in Linux beside two things:
  there is no possibility to create user gui (yet) and analysis should be started
  outside gui in command prompt like "MainUserAnalysis.exe -server"      

STARTING GO4 browser in ROOT

- Run root from command prompt after go4init.bat was called
    Q:\> root
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
  of remote analysis. Draw of histogram and picture are possible.
  Via right mouse menu monitoring flag for histograms can be switched
  on/off and monitoring can be toggled.
- Via go4 instance a various commands can be executed like:
    root [3] go4->StartAnalysis();
    root [4] go4->StopAnalysis();
  For more information see online documentation on TGo4Interface class
  on htpp://go4.gsi.de site

COMPILE USER ANALYSIS UNDER WINDOWS

  First of all, Microsoft VC++ should be installed.
  There is a free edition of Microsoft Visual C++ Express 2009 on
       http://msdn.microsoft.com/vstudio/express/visualc/
  Together with Visual C++ one should also install MS SDK.
  Some more instructions how it is can be installed see on ROOT web page:
       http://root.cern.ch/root/Procedure/
  After VC++ is installed, there are two alternatives, how example can be compiled.

  1. Compile with CYGWIN

  First of all, CYGWIN (http://cygwin.com/) should be installed.
  This provides Unix like environment and allows to use shell-based build scripts.
  After CYGWIN installed, one should adjust login script 
  (go4init.sh), where actual location of different components
  should be specified. Than copy your analysis (or one of Go4 examples, 
  for instance, Go4Example2Step) to separate location. 
  Before compile analysis, GO4SYS should be set as relative location
  to current directory. For instance, if Go4 installed in F:\Apps\go4 
  and user analysis in F:\User\App1, GO4SYS should be "..\..\Apps\go4". 
  Run cygwin shell, go to directory with user analysis and compile it with:
     /cygwin/f/user/app1> . go4init.sh
     /cygwin/f/user/app1> export GO4SYS=../../Apps/go4
     /cygwin/f/user/app1> make
  This should compile analysis and produce library and executable.

  2. Compile with Windows native nmake tool

  First of all, go4init.bat file should be adjusted - one should
  uncomment several lines, where path to Visual Studio are specified.
  Second, one need special Makefile, which is suited for nmake.exe tool.
  There is one example: $GO4SYS\Go4Example2Step\Makefile.win, which
  is written for Go4Example2Step. To adopt it for other user analysis,
  one should specify correct list of source files in this Makefile. 
  To run compilation, just do:
     F:\User\App1> nmake -f Makefile.win

COMPILATION OF GO4 FROM SOURCE:

  To compile Go4 from source on Windows, several steps should be done.
  
  1. Download and install root (ROOT Windows binaries is enough)
  2. Install and configure Cygwin
  3. Configure VC++ as described in http://root.cern.ch/root/Procedure/
  4. Download Qt 4.x source win tar ball (there is no binary version for VC++)
     and compile it 
  5. Download go4 sources from http://go4.gsi.de web site.
  6. Find and modify $GO4SYS/etc/win/go4init.sh file to actual location of all 
     components
  7. Run Cygwin shell, call ". go4init.sh", cd $GO4SYS and call "make"
  8. Hopefully, this will compile Go4 under Windows!

