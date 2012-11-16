--------------------------------------------------------------
              Go4 Taskhandler v4.5.4 (build 40504)
                      16-November-2012
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at EE department, GSI
------------------------------------------------------------
authors:
   Joern Adamczewski-Musch   (1999-)
   Marc Hemberger            (1999-2000)        
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

This is an example package containing
Go4ThreadManager
Go4LockGuard
Go4Exceptions
Go4Log
Go4Queue
Go4Socket
Go4StatusBase
Go4CommandsBase
Go4CommandsTaskHandler
Go4TaskHandler
Go4TaskHandlerExample

libraries of the Go4 Project.


INSTALLATION:
-Install ROOT package (see http://root.cern.ch)
 If necessary, set your ROOT environment variables $ROOTSYS,
   $LD_LIBRARY_PATH, $PATH.
-Create a working directory for the taskhandler and unpack
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
------------------------------------------------------------------------
Package Go4ThreadManager:
   The thread manager with framework classes, base classes,
   and exception classes.
------------------------------------------------------------------------
Package Go4Queue:
   The threadsafe pointer queues for TObject and TBuffer. Includes
   streaming mechanism for ROOT objects into TBuffers.
------------------------------------------------------------------------
Package Go4Socket:
   Socket implementation based on ROOT TSocket class.
------------------------------------------------------------------------
Package Go4StatusBase:
   Go4 Status baseclasses.
------------------------------------------------------------------------
Package Go4CommandsBase:
   Go4 Command pattern base classes.
------------------------------------------------------------------------
Package Go4CommandsTaskHandler:
   Commands to communicate between taskhandler server and client
------------------------------------------------------------------------
Package Go4TaskHandler:
   The taskhandler itself. contains TGo4ClientTask and TGo4ServerTask
   framework base classes. These can be subclassed by the user to
   implement threaded client-server connection.
------------------------------------------------------------------------
Package Go4TaskHandlerExample:
   Contains example Client and Server classes which communicate
   via the taskhandler framework.
----------------------------------------------------------------------------
----------------------------------------------------------------------------
USAGE:
   Package TGo4TaskHandler contains the baseclasses of the framework.
   The user should define subclasses of TGo4ServerTask and TGo4ClientTask
   that match his requirements. The server task is meant to control the
   client task that does the actual job (analysis or calculation). Therefore
   the direction of the communication channels (sockets) between client and
   server task is fixed: Command objects (type TGo4Command) can only be send
   from server to client; data objects (type TNamed) and status objects
   (type TGo4Status) can only be send from client to server.(Note: data
   that shall be send from server to client should be encapsulated in a
   command object).
   The GUI or command line interface should be implemented as a TGo4ServerTask,
   while the working application should be implemented as TGo4ClientTask.
   To put and get the command, data and status objects,
   there are e.g. methods SubmitCommand, NextObject, NextStatus
   available in TGo4ServerTask,  and methods NextCommand, SendObject,
   SendStatus, SendStatusMessage in TGo4ClientTask classes, respectively.
   Internally, the methods NextObject, NextCommand, and NextStatus use queues;
   these methods might wait until any object is filled into the queue from
   the remote socket, or they might be called in a polling mode that
   returns 0 if the queue is empty.
   The framework applies threads to establish the client server
   connections and to handle the object transfer between the sockets and the
   queues. Additional user threads are required to work with the transferred
   objects. These should be defined as user subclasses of TGo4Runnable,
   and they should be created in the constructors of the user subclasses of
   TGo4ServerTask and TGo4ClientTask, respectively.
   If the user wants to apply new command classes in SubmitCommand("comname")
   method, these have to be registered in the command list of the server task.
   This can be done without changing the default command list by means of
   TGo4ServerTask::AddUserCommand method. Moreover, the user subclass of
   TGo4ServerTask could override the CreateCommandList method and return
   an own subclass of TGo4TaskHandlerCommandList here.
   All user classes that contain methods to be called from a command object
   must inherit from TGo4CommandReceiver, and they must register themselves
   in their constructors at the TGo4CommandInvoker singleton by name.
   The name of this command receiver must be specified in the command object
   constructor, too; if the command receiver names do not match, the
   Go4 command pattern mechanism will disable the execution of the command.
   All default taskhandler commands have the TGo4ClientTask instance
   as receiver, and they are known by the TGo4TaskHandlerCommandList.
   In addition to the commands that are executed by means of the command
   pattern mechanism from a user thread, there are some special
   "emergency" commands that are executed directly in the internal command thread
   of the TGo4ClientTask on receiving them.
   These special commands can be submitted by name:
   TGo4ServerTask::SubmitCommand("THEMQuit");
      - calls TGo4ClientTask::Quit() or derived method; should
         quit and disconnect the client
   TGo4ServerTask::SubmitCommand("THEMKill");
      - calls TGo4ClientTask::KillMain() or derived method;
         might kill the main user thread
   TGo4ServerTask::SubmitCommand("THEMRestart");
      - calls TGo4ClientTask::RestartMain() or derived method;
         might kill and restart the main user thread or job
By overriding the respective methods of TGo4ClientTask, the user can
define what should happen when one of these commands is send.
Moreover, methods TGo4ClientTask::Start()  and TGo4ClientTask::Stop() may
be also overridden in the user subclass to modify the behaviour of the
taskhandler commands "THStart", and "THStop", respectively.

----------------------------------------------------------------------------
----------------------------------------------------------------------------
EXAMPLES:
   An Example of special client task and server task implementations
   is in subdirectory TGo4TaskHandlerExample.
   First execute "MainGo4TaskHandlerServer" in one shell. This will start
   a server process which waits for connection of a client.
   In a second shell, execute
   "MainGo4TaskHandlerClient name hostname portnumber"
   to start a client process. The command line parameters are here:
   name       - any arbitrary name for this client
   hostname    - nodename of the computer which runs server task
   portnumber   - number of connection socket where server is waiting.
               (this number is printed out on the server shell)
   Actually, the client task startup could look like this:
    "MainGo4TaskHandlerClient myclient localhost 5000"

   The client will then connect to the running server.
   In the example, the client task has two threads
   (TGo4MainRunnable, TGo4WatchRunnable classes) and operates on an
   "application object" (TGo4ExampleApplication) that represents e.g.
   an analysis framework. Here the TGo4MainRunnable executes all command
   objects received from the server, and fills a histogram that belongs
   to the TGo4ExampleApplication. The TGo4WatchRunnable periodically
   creates a status object of the client that is streamed into an internal
   root TBuffer and then send via sockets to the server task.
   The server task has the two threads TGo4ControllerRunnable and
   TGo4LoggingRunnable, and a TGo4ExampleController object that represents
   the controlling user interface (e.g. a GUI or a CLI).
   The TGo4ControllerRunnable gets new commands from the TGo4ExampleController
   and sends them to the client. It also displays data objects
   which are send by the client on the terminal. The TGo4LoggingRunnable
   waits for any status object from the client and displays them on the
   terminal, too.
   Methods TGo4ServerTask::StartWorkThreads and
   TGo4ServerTask::StopWorkThreads are overwritten in the user
   server class to start and stop all user threads. In StopWorkThreads,
   every thread that is waiting for an object from a queue
   (NextObject, NextStatus methods) should be released by means of dummy objects
   that are fed into the queue. Then the thread runnable is stopped
   (not cancelled; see Go4ThreadManager for details). In StartWorkThreads,
   the runnables should be started again. These methods are necessary to
   properly change the current client for the user threads; otherwise
   the threads might still work on an old data queue when the client
   is removed and connected again. Note that it is possible to connect
   more than one client task to the server task. The latest connected
   client will be the active client which is processed by the
   user display threads here. However, the currently active client can be
   chosen by name using TGo4ServerTask:SetCurrentTask method.
   In this example, we use text printouts of the received
   objects, and one root canvas to display the histogram.
   There is only one command which is created by the controller
   (TGo4ComAction1); this lets the client send a copy of the application
   histogram to the server. After receiving this histogram, the controller
   runnable submits a second command by name "EXAction2"
   (class TGo4ComAction2) , which performs a dummy printout on the
   client only.
------------------------------------------------------------------
See source code comments for additional information.

31-May-2007   Joern Adamczewski (J.Adamczewski@gsi.de)




