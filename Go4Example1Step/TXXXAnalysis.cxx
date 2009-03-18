#include "TXXXAnalysis.h"

#include "Riostream.h"
#include <time.h>

#include "Go4EventServer.h"
#include "TGo4StepFactory.h"
#include "TGo4AnalysisStep.h"
#include "TXXXParam.h"
#include "TXXXControl.h"

//***********************************************************
TXXXAnalysis::TXXXAnalysis() : fUserFile(0),fMbsEvent(0){}
//***********************************************************
// this constructor is used in Main program
TXXXAnalysis::TXXXAnalysis(const char* input, Int_t type, Int_t port, const char* output, Bool_t enable)
  : fUserFile(0),
    fMbsEvent(0),
    fEvents(0),
    fLastEvent(0)
{
// input: input  file name (*.lmd)  is overwritten in analysis configuration menu
// type:  type of input
// output:   output file name (*.root) is overwritten in analysis configuration menu
// enable:output

// all these parameter objects will be used for the different MBS inputs
  TGo4MbsFileParameter*        mbsfile;
  TGo4MbsTransportParameter*   mbstrans;
  TGo4MbsStreamParameter*      mbsstream;
  TGo4RevServParameter*        mbsrev;
  TGo4MbsEventServerParameter* mbsevent;
  TGo4FileStoreParameter*      store;
  TGo4EventProcessorParameter* proc;
// We will use only one analysis step (factory)
  TGo4StepFactory*             factory;
  TGo4AnalysisStep*            step;

  factory = new TGo4StepFactory("Factory");
  store   = new TGo4FileStoreParameter(output);
  proc    = new TGo4EventProcessorParameter("ProcType",4); // arbitrary number
  switch (type){
    case GO4EV_MBS_FILE:
       mbsfile   = new TGo4MbsFileParameter(input);
       step      = new TGo4AnalysisStep("Analysis",factory,mbsfile,store,proc);
       cout << "**** Analysis: Create file input " << input << endl;
       break;
    case GO4EV_MBS_STREAM:
       mbsstream = new TGo4MbsStreamParameter(input);
       step      = new TGo4AnalysisStep("Analysis",factory,mbsstream,store,proc);
       cout << "**** Analysis: Create stream input "  << input << endl;
       break;
    case GO4EV_MBS_TRANSPORT:
       mbstrans  = new TGo4MbsTransportParameter(input);
       step      = new TGo4AnalysisStep("Analysis",factory,mbstrans,store,proc);
       cout << "**** Analysis: Create transport input " << input  << endl;
       break;
    case GO4EV_MBS_REVSERV:
       mbsrev    = new TGo4RevServParameter(input);
       mbsrev->SetPort(port);
       step      = new TGo4AnalysisStep("Analysis",factory,mbsrev,store,proc);
       cout << "**** Analysis: Create remote event server input " << input << " port " << port <<endl;
       break;
    case GO4EV_MBS_EVENTSERVER:
       mbsevent  = new TGo4MbsEventServerParameter(input);
       step      = new TGo4AnalysisStep("Analysis",factory,mbsevent,store,proc);
       cout << "**** Analysis: Create mbs event server input "  << input << endl;
       break;
    default:
       step      = 0;
       break;
  }
  // tell the factory the names of processor and output event
  // both will be created by the framework later
  // Input event is by default an MBS event
  factory->DefEventProcessor("XXXProc","TXXXProc");// object name, class name
  factory->DefOutputEvent("XXXEvent","TXXXEvent"); // object name, class name

  store->SetOverwriteMode(kTRUE); // overwrite file

  step->SetSourceEnabled(kTRUE);
  step->SetStoreEnabled(enable);  // en-disable output
  step->SetProcessEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  AddAnalysisStep(step);

  // Now the first analysis step is set up.
  // Other steps could be created here

  //////////////// Parameter //////////////////////////
  // At this point, autosave file has not yet been read!
  // Therefore parameter values set here will be overwritten
  // if an autosave file is there.
      fPar = new TXXXParam("Par1");
      AddParameter(fPar);
      fCtl = new TXXXControl("Control");
      AddParameter(fCtl);
}
//***********************************************************
TXXXAnalysis::~TXXXAnalysis()
{
  cout << "**** TXXXAnalysis: Delete instance" << endl;
}

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPreLoop()
{
  // all this is optional:
  cout << "**** TXXXAnalysis: PreLoop" << endl;
  // get pointer to input event (used in postloop and event function):
  fMbsEvent = dynamic_cast<TGo4MbsEvent*> (GetInputEvent("Analysis"));   // of step "Analysis"
  if(fMbsEvent)
    {
      // fileheader structure (lmd file only):
      s_filhe* fileheader=fMbsEvent->GetMbsSourceHeader();
      if(fileheader)
         {
           cout <<"\nInput file: "<<fileheader->filhe_file << endl;
           cout <<"Tapelabel:\t" << fileheader->filhe_label<<endl;
           cout <<"UserName:\t" << fileheader->filhe_user<<endl;
           cout <<"RunID:\t" << fileheader->filhe_run<<endl;
           cout <<"Explanation: "<<fileheader->filhe_exp <<endl;
           cout <<"Comments: "<<endl;
           Int_t numlines=fileheader->filhe_lines;
           for(Int_t i=0; i<numlines;++i)
            {
               cout<<"\t"<<fileheader->s_strings[i].string << endl;
            }
         }
    }
   fEvents=0; // event counter
   fLastEvent=0; // number of last event processed
   return 0;
}
//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPostLoop()
{
  // all this is optional:
   cout << "**** TXXXAnalysis: PostLoop" << endl;
   cout << "Last event  #: " << fLastEvent << " Total events: " << fEvents << endl;
   fMbsEvent = 0; // reset to avoid invalid pointer if analysis is changed in between
   fEvents=0;
   return 0;
}

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserEventFunc()
{
  // all this is optional:
  // This function is called once for each event after all steps.
  if(fMbsEvent)
    {
     fEvents++;
     fLastEvent=fMbsEvent->GetCount();
    }
  if(fEvents == 1 || IsNewInputFile())
    {
      cout << "First event #: " << fLastEvent  << endl;
      SetNewInputFile(kFALSE); // we have to reset the newfile flag
    }
   return 0;
}
