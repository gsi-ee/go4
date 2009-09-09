//////////////////////////////////////////////////////////////////
// Package Go4AnalysisClient - description of MainGo4CintServer -
//  (Joern Adamczewski 25-Apr-2005)
-----
Overview:

   Starting from the Go4 version 2.10, there is a possibility to
   observe and control execution of normal ROOT macros from the
   Go4 GUI. This allows development of analysis code without respect
   of Go4 analysis framework classes (like TGo4EventProcessors,
   TGo4AnalysisStep and so on) keeping a possibility of remote access
   to running enviroment of user analysis. Go4 provides two alternative
   way: user can run special executable - MainGo4CintServer,
   which provides together with standard root CINT enviroment running
   Go4 analysis client or user can just add simple initialisation call
   of go4_init.C script to allow observation of existing histograms.

==================================================
I. Using of MainGo4CintServer (recommendet)

   MainGo4CintServer executable starts a ROOT interpreter
   environment TRint with an underlying Go4 server task.
   Any Go4 GUI may connect to this environment, control the
   Go4 loop running state and fetch registered objects.

Usage and Startup:

   1) Start CINTServer:
      "bash> MainGo4CINTServer"
      After the ROOT welcome message, go4 will printout the
      number of the connection port, e.g.
         "Waiting for client connection on PORT: 5000"
   2) Start the Go4 GUI in client mode:
      "bash> MainGo4GUI -client".
      In contrast to the usual mode, the gui is now configured to
      connect to an analysis server as client.
   3) Connect the GUI to the CINTServer process:
      In clientmode, the "LaunchClient" Window of the GUI will just
      accept a node name and the portnumber to connect. Specify
      the same number as printed out from the CINTServer (e.g. 5000)
   4) After successfull connection, it is possible to request
      the list of registered objects from the CINTServer (Refresh
      button in the analysis tab). Unless any object was registered
      to the go4 framework in the CINT Session, this list is of
      course empty! It is also possible to control the Go4 running state
      with the start and stop buttons.
   5) Run your ROOT script inside the CINT Server. Depending on the
      registration of objects and some go4 control methods (see below),
      the macro objects may be inspected from the GUI

Go4 Script control methods:

   The Go4 framework is available from the CINTServer environment
   by the global method
   TGo4Analysis* go4= TGo4Analysis::Instance();
   After this call, variable "go4" can access any method of the
   analysis framework.
   Object registration:
      - go4->AddHistogram(his); // make script histogram TH1* his available
                                // in the Go4 GUI
      - go4->AddAnalysisCondition(conny); // dito for TGo4Conditions
      - go4->AddParameter(par); // dito for TGo4Parameters
      - go4->AddPicture(pic); // dito for TGo4Pictures
      - etc. (see Go4 Reference Manual for available Add... methods here)

      - go4->AddTree(mytree); // register TTree, but do not change Tree ownership to Go4
      - go4->RemoveTree(mytree); // unregister TTree: important to cleanup
                                 // reference in Go4 if tree is removed from root (closing TFile !)

   For usage in scripts:

      - Int_t seconds=go4->WaitForStart();
         // Polls until the Go4 is set into the "running" state
         // (by Start button on gui or SetRunning() method)
         // with 1 second interval.
         // Return value:  number of seconds from begin of wait
                           until "running" is switched true.
                           If negative value is returned,
                           a ROOT interrupt has happened during wait
                           (e.g. Ctrl-C on CINT Canvas).

      - Int_t state=go4->Process();
         // Process one main cycle of Go4 eventloop from macro.
         // Will first execute any command from gui in the queue,
         // then call MainCycle to process anaylsis steps,
         // user event function and dynamic list (if existing).
         // This call is required inside any explicit loop in the
         // macro to process go4 framework analysis actions.
         // Note that thread locking is done within this function.
         // Return value state is <0 if running state is stopped,
         // otherwise 0.

      - go4->SetRunning(Bool_t on);
         // Switch Go4 running state from inside a macro.
         // Useful to react on analysis conditions

      - Bool_t on=go4->IsRunning();
         // check the running state of the Go4. Maybe obsolete,
         // since this is done implicitely in methods
         // WaitForStart() and Process().
         // However, macro loop may be controlled from GUI independent
         // of Go4 Mainloop processing.


Examples:

      hsimplego4.C -
         A variation of the popular hsimple example
         from the ROOT tutorials to be run in the Go4 CINTServer.

         After client-server connection,
         execute ".x hsimplego4.C". Macro
         will wait until the Go4 start button is pressed and then
         run the random filling in infinite loop (mind your disk
         space, since a TNTuple is filled into a file here!)
         Registered objects may be monitored. The loop
         can be started and stopped at any time from the Go4 GUI.
         Please try the remote tree draw on the TNTuple from
         the Go4 GUI and view the newly created histograms...
         Try to launch the TBrowser before executing the macro
         and inspect the content of the "Go4" folders locally...

      treedraw.C -
         Execute .x treedraw.C("filename"). "filename" specifies
         a root file "filename.root" that contains a TTree.
         Note: first tree found in file will be used.

         This macro contains 2 examples on trees.
         1) Direct TTree::Draw() expressions are
         executed; after finishing, a message is sent to the
         Go4GUI and the output histograms may be viewed here.
         2) After registration of the TTree, the go4 Process()
         will be executed in a loop. Please try the remote tree
         draw on the TTree from the GUI and view the result
         histograms. Loop may be controlled by the Start/Stop
         buttons as in example hsimplego4.

==================================================
II. Using of go4_init.C script

   This methods allows with minimal effort observe histograms,
   produced and filled by practically any running ROOT script.
   go4_init.C script initialises Go4 communication classes and
   scan current directory for existing histogram.

Usage:
   1) Copy $GO4SYS/Go4AnalysisClient/go4_init.C file to
      directory, where other scripts are situated.
   2) Add call of go4_init() function to script at the place,
      where histogram are already created, but analysis event loop
      is not yet running. One should avoid to put go4_init() call
      inside any kind of loop while this slowdown calculation speed
      dramatically.
   3) Run normal root session and load go4_init.C script by command:
        root [0] .L go4_init.C
   4) Run user script
        root [1] .x user_script.C
   5) When go4_init() function will be called, go4 will initialise
      connection socket and prinout number of port for connection
         "Waiting for client connection on PORT: 5000"
   6) After this point on the user should run and use Go4 GUI
      in client mode as it is described in previos section. The only
      difference that user can not control exection of the script
      with Start/Stop buttons.

Examples:
    hsimple.C -
       This is a standard ROOT example from $ROOTSYS/tutorials.
       The only modification, what was done - call of go4_init()
       function at line 43 and increase of loop limit at line 49.
       To run this example:

          shell> cd $GO4SYS/Go4AnalysisClient
          shell> root -l

          root [0] .L go4_init.C
          root [1] .x hsimple.C




