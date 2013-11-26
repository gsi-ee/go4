--------------------------------------------------------------
              Go4 Threadmanager v4.6.0 (build 40600)
                      28-November-2013
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at DVEE department, GSI
------------------------------------------------------------
authors:
   Joern Adamczewski-Musch   (1999-)
   Marc Hemberger         	 (1999-2000)
   Sergey Linev              (2002-)
---------------------------------------------------------------

Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------
This packages are
tested on
    Linux (LK 2.4.20), RedHat 7.3, Scientific Linux 4
    SuSe 8.1, SuSe 9.2,  SuSe 9.3 (x86_64), SuSe 10.3
    Fedora core 2/ core 3/ core 4 / core 5,
    Debian 3.1 (32 bit and AMD 64bit), Debian 4.0, Ubuntu 6.10
        compilers: gcc 3.2, gcc 3.3.5, gcc 3.4.3, gcc 4.1.x, gcc 4.3.x
    SunOS 5.2 (Solaris) with CC 5.8
    ROOT: recommended >=4.04; tested for ROOT versions <=5.23-04

--------------------------------------------------------------

This is package contains:

Go4ThreadManagerExample
Go4ThreadManager
Go4LockGuard
Go4Exceptions
Go4Log
libraries of the Go4 Project.


INSTALLATION:
-Install ROOT package (see http://root.cern.ch)
 If necessary, set your ROOT environment variables $ROOTSYS,
   $LD_LIBRARY_PATH, $PATH.
-Create a working directory for the Go4ThreadManager and unpack
   this tarball there.
-In the working directory, run the init script
   . go4.init
   This will set $GO4SYS variable required for Makefile
-"make all"
   Compiles all subpackages, creates shared libraries and
   copies the libs into $GO4SYS/lib directory.

________________________________________________________________________
DESCRIPTION:

------------------------------------------------------------------------
Package Go4Log:
   Contains basic mechanism to trace sourcecode and for debug output
------------------------------------------------------------------------
Package Go4Exceptions:
   Contains go4 exception base classes
------------------------------------------------------------------------
Package Go4LockGuard:
   Contains lockguard class (independent from threadmanager):
TGo4LockGuard:
   Object to guard any critical action by means of a mutex lock.
   Mutex can be external (local), or internal (global).

------------------------------------------------------------------------
Package Go4ThreadManager:
   The thread manager with framework classes, base classes,
   and exception classes.
TGo4ThreadManager:
   Base class for all user frameworks working with threads.
TGo4AppControlTimer:
   Timer component of the TGo4ThreadManager. Does the initialization
   and can block the ROOT system loop.
TGo4ThreadHandler:
   A threadsafe list of threads, aggregate of the
   TGo4ThreadManager class.
TGo4Thread:
   Thread object, an entry of the threadhandler.
   Connects a root TThread to a go4 thread
   work function (runnable).
TGo4Runnable:
   Abstract base class for thread workfunction object.
TGo4ThreadException:
   Exception base class for all thread related control
   exceptions with different actins, such as:
TGo4CancelException,TGo4CreateException, TGo4RemoveException,
TGo4ReplaceException, TGo4RestartException, TGo4StartException.

TGo4TerminateException:
   Exception which terminates application in a thread friendly
   way.

----------------------------------------------------------------------------
USAGE:
To apply the package for any user purpose, subclasses of
TGo4ThreadManager and TGo4Runnable have to be implemented.

The TGo4Runnable::Run() virtual method defines which action shall be
run in a thread; other virtual methods exist to change behaviour on exception
thrown in this thread (see sourcecode comments for details).

In the constructor of the TGo4ThreadManager subclass, instances
of the user runnables can be added to the list of go4 threads:

// first create runnable objects:
MyControlRunnable* run1= new MyControlRunnable;
MyAnalysisRunnable* run2= new MyAnalysisRunnable;
MyAnalysisRunnable* run3= new MyAnalysisRunnable;
MyPlotRunnable* run4= new MyPlotRunnable;
// all these classes are subclasses of TGo4Runnable
// with different Run() methods

// add runnables to list of threads:
fxWorkHandler->NewThread("Controller", run1);
fxWorkHandler->NewThread("FirstAnalysis", run2);
fxWorkHandler->NewThread("SecondAnalysis", run3);
fxWorkHandler->NewThread("Drawing", run4);

At the end of the threadmanager subclass ctor, method
Launch();
must be called to start the threads.

----------------------------------------------------------------------------
EXAMPLES:
Examples of the thread manager usage are in subdirectory
Go4ThreadManagerExample.

Classes TGo4TestThreadManager and TGo4TestRunnable are
"user" subclasses of the threadmanager base classes.

Executable MainGo4ThreadManager creates a TPad with two canvases.
It starts 5 threads with different actions
(filling and drawing histograms;
  starting and stopping runnable workfunctions;
  creating and canceling threads;
  terminating the application.)
The threaded actions are defined by constructor parameter of the
TGo4TestRunnable (see TGo4TestRunnable.h comments).

------------------------------------------------------------------
See source code comments for additional information.

31-May-2007   Joern Adamczewski (J.Adamczewski@gsi.de)




