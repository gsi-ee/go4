{
//----------------------------------------------------------------------------------
// This example shows  how to run a precompiled analysis
// within a CINT macro, using the implicit analysis loop.
// To use the convenient ROOT auto load one has to create
// a library load map by command:
// make map
// This creates a local file .rootmap
// When changing Go4UserAnalysisLinkDef.h the map must be created new.
// 18-Nov-2004 JA, HE

//----------------------------------------------------------------------------------
// set up parameters used in the macro here:
#define kGUI   2
#define kBatch 1
Int_t   runningMode   = kBatch;               // kGUI or kBatch
Int_t   maxevents     = 0;                // number of events for batch mode, 0 - endless
TString inputfile     = "/GSI/lea/gauss.lmd"; // input file
TString autosavefile  = "gauss_AS.root";      // auto save file
Bool_t  writeautosave = kTRUE;                // write auto-save file
Int_t   autosavesec   = 0;                    // interval for autosave, 0: at end
TString hostname      = "lxxxxx";            // host for gui mode
UInt_t  guiport       = 5000;                 // port number used by GUI
TString mbsnode       = "r2xxxx";              // hostname of mbs
TString revnode       = "lxxxxx";            // hostname of remote event server
UInt_t  revport       = 6003;                 // portnumber of remote event server
Bool_t  startbrowser  = kTRUE;                // start ROOT browser at the end

// uncomment one of the standard inputs:
TGo4MbsFileParameter         * par = new TGo4MbsFileParameter(inputfile.Data());
//  TGo4MbsStreamParameter     * par = new TGo4MbsStreamParameter(mbsnode.Data());
//  TGo4MbsTransportParameter  * par = new TGo4MbsTransportParameter(mbsnode.Data());
//  TGo4MbsEventServerParameter* par = new TGo4MbsEventServerParameter(mbsnode.Data());
//  TGo4MbsRevServParameter    * par = new TGo4MbsRevServParameter(mbsnode.Data());
//  par->SetPort(revport); // for TGo4MbsRevServParameter only

// the following code need not to be modified:
//------ create the analysis -------------------------
  TGo4Analysis*      analysis = TGo4Analysis::Instance();
  TGo4StepFactory*   factory  = new TGo4StepFactory("Factory");
  TGo4AnalysisStep* step      = new TGo4AnalysisStep("Analysis",factory,0,0,0);
  factory->DefEventProcessor("XXXProc","TXXXProc");// object name, class name
  factory->DefOutputEvent("XXXEvent","TGo4EventElement"); // object name, class name
  step->SetEventSource(par);// set input source
  step->SetSourceEnabled(kTRUE);
  step->SetStoreEnabled(kFALSE);  // en-disable output
  step->SetProcessEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);
  analysis->AddAnalysisStep(step);
  analysis->SetAutoSaveFile(autosavefile.Data());
  analysis->SetAutoSaveInterval(autosavesec); // after n seconds , 0 = at termination of event loop
  analysis->SetAutoSave(writeautosave);    // optional

//------ start the analysis -------------------------
 if(runningMode == kBatch)
   {
     cout << "**** Main: starting analysis in batch mode ...  " << endl;
     gROOT->SetBatch(kFALSE); // to plot histograms from TBrowser. Go4 defaultmode is canvas disabled!
     if (analysis->InitEventClasses() )
       {
          analysis->RunImplicitLoop(maxevents);
          delete analysis;
          cout << "**** Main: Done!"<<endl;
          if(startbrowser) x=TBrowser();
       }
     else
       {
          cout << "**** Main: Init event classes failed!!!"<<endl;
       }
   }
 else if(runningMode == kGUI)
   {
     cout << "**** Main: starting analysis in client mode ..." << endl;
     // to start histogram server: kTRUE,"base","password"
     TGo4AnalysisClient* client = new
     TGo4AnalysisClient("Client",analysis, hostname.Data(), guiport, kFALSE,"","");
     cout << "**** Main: created AnalysisClient Instance: "<<client->GetName()<<endl;
   }
}
