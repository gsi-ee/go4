--------------------------------------------------------------
         Go4 Release Package v4.2-1 (build 40201)
                      30-April-2009
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at EE department, GSI
------------------------------------------------------------
Authors: Joern Adamczewski-Musch      (1999-)
         Mohammad Al-Turany           (2000-2004)
         Denis Bertini                (2001-2004)
         Hans Georg Essel             (1999-)
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
These package was tested on
    Suse 10.2 (x86_64), SuSe 11.1
    Debian 3.1 , Debian 4.0 (32 bit and AMD 64bit)
    compilers: gcc 3.3.5, gcc 4.1.2, gcc 4.3.x
    SunOS 5.2 (Solaris) with CC 5.8

REQUIREMENTS:
   ROOT version: >=5.19/01
   
   Qt version: either 3.3.x or 4.4.x

   NEW: this Go4 distribution can be built with Qt3 or Qt4.
        Makefile recognizes which QT is active (by QTDIR or qmake version),
        and builds the corresponding QT depend branch of Go4.

INSTALLATION:

- Install the ROOT framework Version >=5.22/00
  (download at http://root.cern.ch)
  NOTE: choose installation mode for one generic $ROOTSYS directory,
  DO NOT INSTALL root in "fixed location" as system libraries, i.e.
  DO NOT use e.g. "--prefix=/usr/local" when configuring root install!
  set environment variables
      $(ROOTSYS), $(LD_LIBRARY_PATH), $(PATH)
      e.g. in a startup script rootlogin

- Prepare Qt on your machine. Most modern Linux distributions provide 
  Qt libraries and include files in default locations like /usr/bin and 
  /usr/include/qt3. Go4 able to detect major version of such Qt installation 
  and will compile either qt3 or qt4 GUI. In this case it is not necessary
  to set QTDIR location (sometime it is even does not exists). 
  If there is no Qt installed on your system, or Go4 is not able to use it correctly,
  install Qt graphics library X11-free-4.4.x from source 
  (download at http://trolltech.com/developer/downloads/qt/x11).
  In the $QTDIR, before building Qt configure it like:
      ./configure -prefix $PWD -shared 
   Set environment variables
      $(QTDIR), $(LD_LIBRARY_PATH), $(PATH)
      e.g. in a startup script qtlogin

- Unpack this Go4 distribution in the future Go4 System directory,
  e.g. /usr/local/pub/go4_4_2.00

- In the Go4 System directory, execute script ". go4.init".
  This will set the environment $GO4SYS to the current directory 
  (e.g. export GO4SYS=`pwd`), also sets PATH and LD_LIBRARY_PATH variables.
  
- Be sure that environment variables for ROOT and Qt libraries are set
  correctly to your local installations!

- "gmake all" will start the build of the Go4.

- To compile under Win32 or Solaris, one should specify GO4_OS variable respectively.
  For instance, export GO4_OS=Solaris

- The file $GO4SYS/etc/Go4LaunchClientPrefs.txt contains the command format
  strings to be used on remote startup of the analysis from the gui.
  Here the call to the xterm (/usr/local/bin/xterm) is specified with
  absolute path. Please check if xterm is installed on your system at this
  location. You might have to change this string here to run go4 in the
  external xterm (ssh mode).

- The analysis startup script $GO4SYS/etc/Go4ClientStartup.ksh specifies
  in the first line the shell to execute it, i.e "#!/usr/bin/ksh". Please
  check if this is the correct location of the ksh executable on your system
  (type "which ksh" in shell). If not, you should edit this line to indicate
  the absolute path. If ksh should not be installed at all on your linux, you
  can use bash instead. The same is true for the user startup script
  AnalysisStart.sh that is in the user analysis working directory,
  e.g. in $GO4SYS/Go4Example2Step

- Edit your go4login script (see go4login.sh):
  Define ROOTSYS, QTDIR (optionally) and GO4SYS according to your local
  installation. You might call your local rootlogin and qtlogin scripts
  here instead explicitly defining ROOTSYS and QTDIR again. Be sure that
  LD_LIBRARY_PATH contains $ROOTSYS/lib:$QTDIR/lib:$GO4SYS/lib
  and PATH contains $ROOTSYS/bin, $QTDIR/bin and $GO4SYS/bin.
  Put the script at a location in your global $PATH.

- ". go4login.sh" from any new shell sets the environment. Type "go4" to
  start the Go4 Main GUI with File/Object Browser and remote analysis control.

- This completes the framework installation. To use Go4 with your analysis,
  install the Go4 User Analysis package (UserAnalysis.tar.gz) at the future
  user working directory. See README.txt of this package for details.

- Read the Go4 Manual $GO4SYS/docs/Go4Introduction.pdf for further information on 
  adopting your analysis and how to use the GUI.

- Read the ROOT Users guide (available at  http://root.cern.ch) for any questions
  concerning the ROOT system itself.

- Please contact the Go4 developer team for any bug reports and wishes!
________________________________________________________________________





