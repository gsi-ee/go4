--------------------------------------------------------------
         Go4 Release Package v4.3.2 (build 40302)
                      -September-2009
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
    Suse 10.2 (x86_64), SuSe 11.1 (x86_64)
    Debian 3.1, Debian 4.0 (32 bit and 64bit)
    compilers: gcc 3.3.5, gcc 4.1.2, gcc 4.3.x
    SunOS 5.2 (Solaris) with CC 5.8
    Windows XP, 7

REQUIREMENTS: ROOT and Qt3 or Qt4


ROOT INSTALLATION

  Install the ROOT framework Version >=5.22/00 (older version up to 5.17 
  can also be used, but not recommended). 
  See instruction how download and compile ROOT on http://root.cern.ch.
  There are several ways to install ROOT framework on the machine:
     1) Keep compiled ROOT executables and libraries where they are and
        correctly setup ROOTSYS, LD_LIBRARY_PATH and PATH variables.
        Typically one creates "rootlogin" script for that.
     2) Install ROOT binaries, includes and libraries in some default locations 
        like /usr/local/bin, /usr/local/lib, /usr/local/include. 
        For that "./configure --prefix=/usr/local" should be called. 
        Prefix location can be any..
     3) Install ROOT from RPM packages. There are Linux distributions, where ROOT 
        is available in form of precompiled package. Normally it should be enough 
        to install such package on the system.
  Check, that ROOT installed correctly, by calling:
  
     shell> root-config --version
      
  If actual version is printed, such ROOT installation can be used for go4.
  If command fails, one should add path to that script into PATH environment variable like
  
     shell> export PATH=$PATH:/usr/local/bin


QT INSTALLATION

  This Go4 distribution can be built with Qt 3.3.x or Qt 4.4.x and higher.
  Most modern Linux distributions provide 
  Qt libraries and include files in default locations. 
  Go4 able to detect major version of such Qt installation 
  and will compile either qt4 or qt3 based GUI. In this case it is not necessary
  to set QTDIR location (sometime it is even does not exists).
  If both qt3 and qt4 version are installed, use WITHQT makefile flag to select required 
  version. It is especially necessary for system, where Qt3 installation automatically
  sets QTDIR variable, therefore to compile Go4 with version Qt4, one should call
  make WITHQT=4. It is recommended to use Qt4 version of GUI. 
  
  If there is no Qt installed on your system, or Go4 is not able to use it correctly,
  one should compile Qt from the source package. For that download from
  http://www.qtsoftware.com/downloads package like qt-x11-opensource-src-4.5.2.tar.gz.
  Configure it with following flags:
  
     shell> ./configure -prefix $PWD -shared 
     
  After compilation set environment variables e.g.
  
     shell> export QTDIR=$PWD 
     shell> export PATH=$PATH:$QTDIR/bin
     shell> export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib


GO4 COMPILATION

  Unpack this Go4 distribution in any suitable directory.
  
     shell> tar xzf go4-4.3.2.tar.gz
  
  This will create subdirectory go4-4.3.2. To compile go4, do:
  
     shell> cd go4-4.3.2
     shell> make all
  
  In most cases it will be enough to compile go4 libraries, gui and
  several analysis examples, included in distribution. Go4 makefile
  has additional capabilities, which are listed in the end of this README.  
  
  
RUNNING GO4 GUI  
  
  After compilation completed, go4 can be started just like:
  
     shell> bin/go4
  
  During compilation "go4login" init script is generated, where 
  all necessary environment variables for ROOT, Qt and Go4 are set. 
  It can be copied in any suitable location (accessible with PATH variable) 
  and called:
  
     shell> . go4login
     
  After that go4 gui and user analysis can be started from any directory.    

  Please read the Go4 Manual $GO4SYS/docs/Go4Introduction.pdf for further 
  information on adopting your analysis and how to use the GUI.

  Please read the ROOT Users guide (available at  http://root.cern.ch) 
  for any questions concerning the ROOT system itself.

  Please contact the Go4 developer team for any bug reports and wishes!


ADVANCED MAKE OPTIONS

  Compare to default options, there are additional flags, which can be specified 
  to the make:
  
  prefix=<location>  compiles go4 in the mode, that it can be installed in
                     specified location. Directory <location> should exists.
                     It can be /usr/local or /usr. After compilation one can
                     install go4 with "make install" path. If directory 
                     <location>/bin included in PATH variable, no any extra
                     login script is required to use go4 later. 

  WITHQT=3|4|no   Specifies version of qt, which should be used for gui compilation.
                  Qt installation will be tested with pkg-config utility.
                  If WITHQT not specified, QTDIR variable will be tested.
                  if none WITHQT and QTDIR are set, first qt4 and than qt3 will
                  be tried to used by go4.
                     
  GO4_OS=Linux|Solaris|Win32 Defines platform, on which go4 will be compiled.
                             Default is Linux
                             
  rpath=true|false  Use -rpath option of linker to include absolute library pathes, 
                    where go4 executables should search for libraries. Default is true.
                    This option allows to use go4 executable without any additional 
                    LD_LIBRARY_PATH variable settings

  All flags, specified to the make, will be stored together with some environment settings
  in build/Makefile.gener. This allow to reuse same settings for user analysis compilation.
  
  After compilation completed, several more make commands can be executed:

  "make install"    Copy binaries, libraries and shared files into location,
                    specified by prefix option.                         
   
  "make clean"      Cleanup all generated files from the disk
  
  "make clean-bin"  Cleanup object/dependency files so that only go4 binaries 
                    and source files are remained
                    

LAUNCH USER ANALYSIS FROM GUI

  Go4 GUI provides ability to launch analysis on local or remote nodes.
  For that either exec call (for local nodes) or ssh/rsh for local and remote
  nodes are used. For terminal output either qtwindow or xterm or KDE konsole
  are used. Go4 includes etc/go4.prefs file, where different initialization
  and run parameters for all this application are listed. Values for that 
  parameters adjusted for current GSI Linux distribution (Debian Etch) and 
  may not fit to other Linux flavors. 
  One can customize value, either editing this file or creating go4.prefs
  file in current directory and setting those variables, which should be changed
  compared to default settings. 
  Another purpose of user-defined go4.prefs file is customize settings 
  of the node, where user analysis application should run. Default go4.prefs
  file supposes, that ROOT/Go4/Aanalysis configuration are similar on
  local (gui) node and remote (analysis) node. It is definitely true, when 
  launching analysis on localhost. It is also true on GSI Linux cluster,
  where public Go4/ROOT installation and user home filesystem identical on
  all nodes. But it also may happen, that remote node has other ROOT or Go4 versions,
  or it may be powerfull 64-bit multicore CPU. To let Go4 GUI apply custom
  launch settings in such situation, go4.prefs file can be used.
  There one can define host specific settings for any parameter. For instance,
  to run analysis on remote node with node-specific go4 version, one should 
  redefine several parameters with following syntax:
  
      hostname=suse11host: shellinitcmd:. go4login; 
      hostname=suse11host: konsole_options:
      hostname=suse11host: workdir:/usr/local/go4/head/Go4ExampleSimple
      hostname=suse11host: exename:/usr/local/go4/head/Go4ExampleSimple/MainUserAnalysis
      hostname=suse11host: exekind:0
  
  Here one defines, that before run host-specific go4login script will be executed.
  Here is also defined, that konsole_options is empty (while SuSE 11.1 konsole does not
  accept several parameters that was available before). One can also strictly define
  working directory and executable name any time when launching ananlys from the gui
  on this node. 
  For meaning of each parameter see main go4.prefs file and comments there.
