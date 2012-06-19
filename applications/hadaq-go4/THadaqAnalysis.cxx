
#include "THadaqAnalysis.h"

#include <stdlib.h>

#if __GO4BUILDVERSION__ > 40502
#include "go4iostream.h"
#else
#include "Riostream.h"
#endif


#include "TCanvas.h"
#include "TH1.h"
#include "TFile.h"

#include "Go4EventServer.h"
#include "TGo4AnalysisStep.h"
#include "TGo4Version.h"

#include "THadaqUnpackEvent.h"
#include "THadaqUnpackProc.h"
#include "TGo4StepFactory.h"
#include "TGo4MbsEvent.h"

//***********************************************************
THadaqAnalysis::THadaqAnalysis() :
   TGo4Analysis(),
   fRawEvent(0),
   fUnpackEvent(0),
   fSize(0),
   fEvents(0)
{
  cout << "Wrong constructor THadaqAnalysis()!" << endl;
}
//***********************************************************
// this constructor is used
THadaqAnalysis::THadaqAnalysis(int argc, char** argv) :
   TGo4Analysis(argc, argv),
   fRawEvent(0),
   fUnpackEvent(0),
   fSize(0),
   fEvents(0)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cout << "****  Go4 version mismatch" << endl;
      exit(-1);
   }


   cout << "**** THadaqAnalysis: Create" << endl;

   // the step definitions can be changed in the GUI
   // first step definitions:
   // the name of the step can be used later to get event objects
   TGo4StepFactory* factory1 = new TGo4StepFactory("Unpack-factory");

   factory1->DefUserEventSource("THadaqEventSource"); // class name of user event source
   factory1->DefInputEvent("RawEvent","TGo4MbsEvent"); // object name, class name
   factory1->DefEventProcessor("UnpackProc", "THadaqUnpackProc");// object name, class name
   factory1->DefOutputEvent("UnpackEvent", "THadaqUnpackEvent"); // object name, class name

   TGo4UserSourceParameter* source1  = new TGo4UserSourceParameter("test.hld");
   TGo4FileStoreParameter*  store1   = new TGo4FileStoreParameter(Form("%sOutput", argv[0]));
   store1->SetOverwriteMode(kTRUE);
   TGo4AnalysisStep*        step1    = new TGo4AnalysisStep("Unpack",factory1,source1,store1,0);

   step1->SetSourceEnabled(kTRUE);
   step1->SetStoreEnabled(kFALSE);  // disable output
   step1->SetProcessEnabled(kTRUE);
   step1->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step1);

}

//***********************************************************
THadaqAnalysis::~THadaqAnalysis()
{
   cout << "**** THadaqAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t THadaqAnalysis::UserPreLoop()
{
   cout << "**** THadaqAnalysis: PreLoop" << endl;
   // we update the pointers to the current event structures here:
   fRawEvent = dynamic_cast<TGo4MbsEvent*>    (GetInputEvent("Unpack"));   // of step "Unpack"
   fUnpackEvent = dynamic_cast<THadaqUnpackEvent*> (GetOutputEvent("Unpack"));
   fEvents=0;

   // create histogram for UserEventFunc
   // At this point, the histogram has been restored from autosave file if any.
   fSize=(TH1D*)GetHistogram("Eventsize");
   if(fSize==0) {
      // no autosave read, create new and register
      fSize = new TH1D ("Eventsize", "Read columns",16384,0,Hadaq_BUFSIZE);
      AddHistogram(fSize);
   }
   //ClearObjects("Histograms"); // reset all histograms to 0 before run
   // name specifies folder to clear
   return 0;
}
//-----------------------------------------------------------
Int_t THadaqAnalysis::UserPostLoop()
{
   cout << "**** THadaqAnalysis: PostLoop" << endl;
   cout << " Total events: " << fEvents << endl;
   fUnpackEvent = 0; // reset to avoid invalid pointer if analysis is changed in between
   fRawEvent = 0;
   fEvents=0;
   return 0;
}

//-----------------------------------------------------------
Int_t THadaqAnalysis::UserEventFunc()
{
   //// This function is called once for each event.
   Int_t value=0;
   if(fRawEvent) value = fRawEvent->GetDlen();
   fSize->Fill(value); // fill histogram
   fEvents++;
   return 0;
}
