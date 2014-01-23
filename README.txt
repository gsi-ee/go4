--------------------------------------------------------------
         Go4 Release Package v4.6.3 (build 40603)
                      23-January-2014
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at EE department, GSI
------------------------------------------------------------
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
These package was tested on
    Suse 10.2 (x86_64), SuSe 11.x (x86_64), SuSe 12.x (x86_64)
    Debian 3.1, Debian 4.0, Debian 5.0, Debian 6.0, Debian 7.0 (32 bit and 64bit)
    Ubuntu 11.10
    compilers: gcc 3.3.5, gcc 4.1.2, gcc 4.3.x, gcc 4.4.x, gcc 4.5.1, gcc 4.6.1, gcc 4.7.2
    SunOS 5.2 (Solaris) with CC 5.8
    Windows XP, 7
    MacOSX Darwin (10.6) with gcc 4.2.1

REQUIREMENTS: ROOT and Qt4/Qt5 (or Qt3)  


ROOT INSTALLATION

  Install the ROOT framework Version >=5.28/00 (older version up to 5.17 
  can also be used, but not recommended). 
  See instruction how download and compile ROOT on http://root.cern.ch.
  There are several ways to install ROOT framework on the machine:
     1) Keep compiled ROOT executables and libraries where they are and
        correctly setup ROOTSYS, LD_LIBRARY_PATH and PATH variables.
        Typically one uses thisroot.sh script from $ROOTSYS/bin directory like:
           . your_root_directory/bin/thisroot.sh
     2) Install ROOT binaries, includes and libraries in some default locations 
        like /usr/local/bin, /usr/local/lib, /usr/local/include. 
        For that "./configure --prefix=/usr/local" should be called. 
        Prefix location depends from the Linux distribution.
     3) Install ROOT from RPM packages. There are Linux distributions, where ROOT 
        is available in form of precompiled package. Normally it should be enough 
        to install such package on the system.
  Check, that ROOT installed correctly, by calling:

     shell> root-config --version

  If actual version is printed, such ROOT installation can be used for go4.
  If command fails, one should add path to that script into PATH environment variable like

     shell> export PATH=$PATH:/usr/local/bin


QT INSTALLATION

  This Go4 distribution can be built with following Qt versions:
     qt3 - Qt 3.3.x and higher
     qt4 - Qt 4.4.x and higher (recommended)
     qt5 - Qt 5.2.x and higher 
  Most modern Linux distributions provide Qt libraries, utilities and include files.
  Typically one should install libqt4-devel packages (or libqt3-devel).
  Normally Go4 able to detect major version of such Qt installation 
  and will compile either correspondent version of Go4 GUI.
  If both Qt3 and Qt4 version are installed, use "withqt" makefile flag to select 
  prefered version of qt. It is especially necessary for systems, where Qt3 installation 
  automatically sets QTDIR variable. In that case call "make withqt=4" to compile 
  go4 with Qt4. It is recommended to use Qt4 version of GUI. 
  
  If there is no Qt installed on your system, or Go4 is not able to use it correctly,
  one should compile Qt from the source package. For that download from
  http://www.qtsoftware.com/downloads package like qt-everywhere-opensource-src-4.6.2.tar.gz
  Configure it with following flags:
  
     shell> ./configure -prefix $PWD 
  
  To compile Qt, call:
  
     shell> gmake
     
  Optionally, one can cleanup all intermediate files to save at least 1.5G disk space:
     
     shell> gmake clean
     
  After compilation set environment variables e.g.
  
     shell> export QTDIR=$PWD 
     shell> export PATH=$PATH:$QTDIR/bin
     shell> export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib


QT installation on Mac OS X

  Go4 QtROOT GUI does not work for Qt build with native MacOSX graphics 
  like carbon or cocoa. One need to install Qt4 using X11 graphics on Mac 
  (fink package qt4-x11). Go4 was tested with Mac OS X 10.6 (Darwin) and Qt 4.5.2.
  Qt3 is not supported for MacOS! 
  On Mac OS shell variable name is DYLD_LIBRARY_PATH instead LD_LIBRARY_PATH.


DABC installation
  
  With DABC (version >= 2.6.0) one gets web interface in go4. 
  Installation instruction for DABC can be found on http://dabc.gsi.de web site. 
  Go4 will automatically use DABC installation once DABCSYS shell variable is set. 
  Typically one call from the shell:
  
     shell> . your_dabc_path/dabclogin 


GO4 COMPILATION

  Unpack this Go4 distribution in any suitable directory.
  
     shell> tar xzf go4-4.6.0.tar.gz
  
  This will create subdirectory go4-4.6.0. To compile go4, do:
  
     shell> cd go4-4.6.0
     shell> make all
  
  In most cases it will be enough to compile go4 libraries, gui and
  several analysis examples, included in distribution. Go4 makefile
  has additional capabilities, which are listed in the end of this README.
  
  
  
RUNNING GO4 GUI  
  
  After compilation completed, go4 can be started just like:
  
     shell> bin/go4
  
  During compilation "go4login" script is generated, where 
  all necessary environment variables for ROOT, Qt and Go4 are set. 
  It can be copied in any suitable location (accessible via PATH variable) 
  and called:
  
     shell> . go4login
     
  After that go4 gui and user analysis can be started from any directory.

  Please read the Go4 Manual $GO4SYS/docs/Go4Introduction.pdf for further 
  information on adopting your analysis and how to use the GUI.

  Please read the ROOT Users guide (available at http://root.cern.ch) 
  for any questions concerning the ROOT system itself.

  Please contact the Go4 developer team for any bug reports and wishes!


ADVANCED MAKE OPTIONS

  Compare to default options, there are additional flags, which can be specified 
  to the make:
  
  prefix=<location>  
     Compiles go4 in the mode, that it can be installed in specified location. 
     Directory <location> should exists. It can be /usr/local or /usr. 
     After compilation one can install go4 with "make install" command. 
     If directory <location>/bin included in PATH variable, no any extra
     login script is required to use go4 later. 

  withqt=3|4|no   
     Specifies version of qt, which should be used for gui compilation.
     Qt installation will be tested with pkg-config utility.
     If 'withqt' not specified, QTDIR variable will be tested.
     If none 'withqt' and QTDIR are set, first Qt4 and than Qt3 will
     be tried to used by go4.

  GO4_OS=Linux|Solaris|Win32|Darwin 
     Defines platform, on which go4 will be compiled.
     If not defined, tested via 'uname'. Default is Linux.

  rpath=true|false
     Use -rpath option of linker to include absolute library pathes, 
     where go4 executables should search for libraries. Default is true.
     This option allows to use go4 executable without any additional 
     LD_LIBRARY_PATH variable settings.

  withdabc=yes|no
     Add different components from DABC. Latest DABC version from
     http://subversion.gsi.de/dabc repository (version >= 2.6.0) should be installed
     and configured with dabclogin script. Use of DABC enables http 
     support in Go4. Default is yes
                    
  debug=1     
     Build all libraries with debug symbols. Default is off.

  nodepend=1
     Build go4 without dependencies - makes build process faster.
     
  force32=1
     Forces 32-bit compilation and linking on 64-bit platform. Implemented
     for such kind of MacOS machines, not tested (probably, not required) on Linux. 

  All flags, specified to the make, will be stored together with some environment settings
  in build/Makefile.gener. This allow to reuse same settings for user analysis compilation.
  
  After compilation completed, several more make commands can be executed:

  "make install"    Copy binaries, libraries and shared files into location,
                    specified by prefix option.

  "make clean"      Cleanup all generated files from the disk

  "make clean-bin"  Cleanup object/dependency files so that only go4 binaries 
                    and source files are remained
