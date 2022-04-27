```
--------------------------------------------------------------
         Go4 Release Package v6.2.0 (build 60200)
                      27-April-2022
--------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at EE department, GSI
--------------------------------------------------------------
Authors: Joern Adamczewski-Musch      (1999-)
         Mohammad Al-Turany           (2000-2004)
         Sven Augustin [Python]       (2015-)
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

# GO4 PACKAGE

This package was tested on
  * openSUSE Leap 42.2, openSUSE Tumblweed 04.2022
  * Debian 7.0, Debian 8.10 (32 bit and 64bit), Debian 9
  * macOS 10.14.6
  * compilers: gcc 4.8.5, gcc 4.9.2, gcc 6.3.0, gcc8.2, gcc 10.2, gcc 11.2

## REQUIREMENTS
   * ROOT 6.24 or higher (recommended 6.26)
   * Qt5.12 or higher (recommended 5.15)
   * or Qt6.2 or higher (experimental)


## ROOT INSTALLATION

Install the ROOT framework Version >=6.24/00.
See instruction how download and compile ROOT on http://root.cern.ch.
Recommended way to build ROOT - compile it and use from compilation path:

     [shell] tar xzf root_v6.24.00.source.tar.gz
     [shell] mkdir build_624
     [shell] cd build_624
     [shell] cmake ../root_v6.24.00
     [shell] make -j
     [shell] source bin/thisroot.sh

To use web-based graphics (required for Mac) qt5web option should be enabled:

     shell> cmake ../root_v6.24.00 -Droot7=ON -Dqt5web=ON

Check after ROOT compilation that everything is fine by calling:

     shell> root-config --version

If actual version is printed, such ROOT installation can be used for go4.
If command fails, one should configure environment call

     shell> source /path/to/root/bin/thisroot.sh


## QT INSTALLATION

This Go4 distribution can be built with following Qt versions:
* qt5 - Qt 5.6.x and higher (recommended 5.15.2)
* qt6 - Qt 6.0.x and higher
Most modern Linux distributions provide Qt libraries, utilities and include files.
Typically one should install libqt5-devel packages.
Normally Go4 able to detect major version of such Qt installation
and will compile correspondent version of Go4 GUI.
If several Qt versions are installed, use "withqt" makefile flag to select
preferred version of qt. It is especially necessary for systems, where Qt3 installation
automatically sets QTDIR variable. In that case call "make withqt=5" to compile
go4 with Qt5. It is recommended to use Qt5 version of GUI.
Qt6 version only can be build with cmake

If there is no Qt installed on your system, or Go4 is not able to use it correctly,
one should download it from https://www.qt.io/download anf follow instruction how it should be
installed. After compilation set environment variables e.g.

    shell> export QTDIR=$PWD
    shell> export PATH=$PATH:$QTDIR/bin
    shell> export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib


### QT installation on Mac OS X

Go4 QtROOT GUI on Mac works only with web-based ROOT graphics.
For that one requires latest qt5 with QWebEngine support.
Most simple way is to use brew (see http://macappstore.org/qt5/):

    shell> brew install qt5

Qt5 should be installed before ROOT and ROOT compilation should include -Dqt5web=ON flag.
After installing Qt5 libraries, one should set following shell variables:

    shell> export Qt5_DIR=/usr/local/opt/qt5/
    shell> export QTDIR=/usr/local/opt/qt5/
    shell> export PATH=$QTDIR/bin:$PATH
    shell> export DYLD_LIBRARY_PATH=$QTDIR/lib:$DYLD_LIBRARY_PATH


## GO4 COMPILATION WITH CMAKE

Create build directory and call

    shell> cmake "path_to_source_dir"
    shell> make -j

One also can specify several options for cmake:

    -Dhdf5=ON - enable HDF5 support, requires installed HDF5 library
    -DCMAKE_INSTALL_PREFIX=/home/user/custom/dir - configure custom directory for installation


## RUNNING GO4 GUI

During compilation "go4login" script is generated, where
all necessary environment variables for ROOT, Qt and Go4 are set.
It can be copied in any suitable location (accessible via PATH variable)
and called:

    shell> source <build_path>/go4login

After that go4 gui and user analysis can be started from any directory.

    shell> go4

Please read the Go4 Manual $GO4SYS/docs/Go4Introduction.pdf for further
information on adopting your analysis and how to use the GUI.

Please read the ROOT Users guide (available at https://root.cern)
for any questions concerning the ROOT system itself.

Please contact the Go4 developer team for any bug reports and wishes!


## GO4 INSTALLATION AFTER CMAKE BUILD

If go4 build cmake, it can be used directly from build directory.
If required, one can install it with the command:

    shell> make install

It will copy files to configured installation path



## GO4 ON WINDOWS

### Install using binary distribution

To use ROOT6 and Go4 on Windows, it is necessary to install Microsoft Visual Studio.
There is free "Community" version which can be found on
https://visualstudio.microsoft.com/downloads/ website. During installation C++ components
should be installed.

For the go4 there is windows installer file, which provides Go4 with correspondent ROOT and Qt5 libraries.
It is recommended to install it in directory without spaces like "C:\Soft\go4".
After installation Go4 icon created in the windows menu and on desktop - it runs go4 gui.


### User analysis compilation

To compile any user analysis, start "x86 Native Tools Command Prompt VS 2019" and:

    call C:\Soft\go4\go4login.bat
    mkdir C:\Soft\analysis
    cd C:\Soft\analysis
    cmake -G"Visual Studio 16 2019" -A Win32 -Thost=x64 c:\Soft\go4\examples\Go4ExampleSimple
    cmake --build . --config Release -- /maxcpucount

To run it, one should specify directory "C:\Soft\analysis\Release" as analysis directory in
"Launch Analysis" widget.


### Go4 compilation from sources on Windows

First of all, install Qt5 with WebEngine support on Windows.
Then start "x86 Native Tools Command Prompt VS 2019" and compile ROOT:

    set PATH=%PATH%;C:\Qt5\5.15.2\msvc2019\bin
    mkdir C:\Soft\root
    cd C:\Soft\root
    cmake -G"Visual Studio 16 2019" -A Win32 -Thost=x64 c:\git\root -Droot7=ON -DCMAKE_CXX_STANDARD=17 -Dwebgui=ON -Dqt5web=ON
    cmake --build . --config Release -- /maxcpucount

Then compile Go4:

    call C:\Soft\root\bin\thisroot.bat
    mkdir C:\Soft\go4
    cd C:\Soft\go4
    cmake -G"Visual Studio 16 2019" -A Win32 -Thost=x64 c:\git\go4
    cmake --build . --config Release -- /maxcpucount



## GO4 COMPILATION WITH MAKE (deprecated)

  Unpack this Go4 distribution in any suitable directory.

     shell> tar xzf go4-6.1.0.tar.gz

  This will create sub-directory go4-6.1.0. To compile go4, do:

     shell> cd go4-6.1.0
     shell> make -j

  In most cases it will be enough to compile go4 libraries, gui and
  several analysis examples, included in distribution. Go4 makefile
  has additional capabilities, which are listed in the end of this README.


### ADVANCED MAKE OPTIONS

  Compare to default options, there are additional flags, which can be specified
  to the make:

  prefix=<location>
     Compiles go4 in the mode, that it can be installed in specified location.
     Directory <location> should exists. It can be /usr/local or /usr.
     After compilation one can install go4 with "make install" command.
     If directory <location>/bin included in PATH variable, no any extra
     login script is required to use go4 later.

  withqt=3|4|5|no
     Specifies version of qt, which should be used for gui compilation.
     Qt installation will be tested with pkg-config utility.
     If 'withqt' not specified, QTDIR variable will be tested.
     If none 'withqt' and QTDIR are set, first Qt5 and than Qt3 will
     be tried to used by go4.

  noweb=1
     Disables building of web-based canvas. Normally web canvas build for ROOT 6.18,
     which is build with -Droot7=ON -Dqt5web=ON build options.

  nox11=1
     Build go4 gui without use of X11. Default for Mac and Windows. Required
     web support in ROOT.

  designer=1
     Specify to build Go4 with Qt designer plugin

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
     and configured with dabclogin script.

  debug=1
     Build all libraries with debug symbols. Default is off.

  nodepend=1
     Build go4 without dependencies - makes build process faster.


  All flags, specified to the make, will be stored together with some environment settings
  in build/Makefile.gener. This allow to reuse same settings for user analysis compilation.

  After compilation completed, several more make commands can be executed:

  "make install"    Copy binaries, libraries and shared files into location,
                    specified by prefix option.

  "make clean"      Cleanup all generated files from the disk

  "make clean-bin"  Cleanup object/dependency files so that only go4 binaries
                    and source files are remained
