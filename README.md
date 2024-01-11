```
--------------------------------------------------------------
         Go4 Release Package v6.3.x (build 60399)
                      spring 2024
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
  * openSUSE Leap 42.2, openSUSE Tumblweed 10.2022
  * Debian 7.0, Debian 8.10 (32 bit and 64bit), Debian 9, Debian 11
  * macOS 10.14.6
  * compilers: gcc8.2, gcc 10.2, gcc 11.2, gcc 12.2

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
* qt6 - Qt 6.2.x and higher
Most modern Linux distributions provide Qt libraries, utilities and include files.
Typically one should install libqt5-devel packages.
Normally Go4 able to detect major version of such Qt installation
and will compile correspondent version of Go4 GUI.
If both qt5 and qt6 are installed, use "cmake -Dqt6=ON <path_to_source>" to compile
go4 with Qt6.

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
    -Dqt6=ON - use Qt6 for GUI compilation
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

