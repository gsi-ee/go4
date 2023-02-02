// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXAnalysis.h"

#include "TH1.h"

#include "TGo4Log.h"
#include "TGo4AnalysisStep.h"
#include "TGo4StepFactory.h"

#include "TXXXUnpackEvent.h"
#include "TXXXAnlEvent.h"
#include "TGo4Version.h"

//***********************************************************
TXXXAnalysis::TXXXAnalysis() :
   TGo4Analysis()
{
   TGo4Log::Error("Wrong constructor TXXXAnalysis()!");
}

//***********************************************************
// this constructor is called by go4analysis executable
TXXXAnalysis::TXXXAnalysis(int argc, char **argv) :
   TGo4Analysis(argc, argv)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      TGo4Log::Error("Go4 version mismatch");
      exit(-1);
   }

   TGo4Log::Info("TXXXAnalysis: Create %s", GetName());

// Create step 1 Unpack.
   TGo4StepFactory* factory1 = new TGo4StepFactory("UnpackFactory");
   factory1->DefEventProcessor("UnpackProc", "TXXXUnpackProc");// object name, class name
   factory1->DefOutputEvent("UnpackEvent", "TXXXUnpackEvent"); // object name, class name
   TGo4AnalysisStep *step1 = new TGo4AnalysisStep("Unpack", factory1, nullptr, nullptr, nullptr);
   step1->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step1);
// These settings will be overwritten by setup.C
   step1->SetSourceEnabled(kTRUE);
   step1->SetStoreEnabled(kFALSE);
   step1->SetProcessEnabled(kTRUE);

// Create step 2 Analysis.
   TGo4StepFactory* factory2 = new TGo4StepFactory("AnalysisFactory");
   factory2->DefInputEvent("UnpackEvent", "TXXXUnpackEvent"); // object name, class name
   factory2->DefEventProcessor("AnlProc", "TXXXAnlProc"); // object name, class name
   factory2->DefOutputEvent("AnlEvent", "TXXXAnlEvent"); // object name, class name
   TGo4AnalysisStep *step2    = new TGo4AnalysisStep("Analysis", factory2, nullptr, nullptr, nullptr);
   step2->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step2);
// These settings will be overwritten by setup.C
   step2->SetSourceEnabled(kFALSE);
   step2->SetStoreEnabled(kFALSE);
   step2->SetProcessEnabled(kTRUE);

   // This condition is used in both steps.
   // Therefore we create it here
   fWinCon1 = MakeWinCond("wincon1", 50, 2000);
}

//-----------------------------------------------------------
TXXXAnalysis::~TXXXAnalysis()
{
   TGo4Log::Info("TXXXAnalysis: Delete");
}

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPreLoop()
{
   TGo4Log::Info("TXXXAnalysis: PreLoop");
   Print(); // printout the step settings
   // we update the pointers to the current event structures here:
   fMbsEvent = dynamic_cast<TGo4MbsEvent*>    (GetInputEvent("Unpack"));   // of step "Unpack"
   fRawEvent = dynamic_cast<TXXXUnpackEvent*> (GetOutputEvent("Unpack"));
   fCalEvent = dynamic_cast<TXXXAnlEvent*>    (GetOutputEvent("Analysis"));
   fEvents = 0;
   fLastEvent = 0;

   // create histogram for UserEventFunc
   // At this point, the histogram has been restored
   // from auto-save file if any.
   fSize = MakeTH1('D', "Eventsize", "Event size [b]", 160, 1., 160.);
   return 0;
}

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPostLoop()
{
   TGo4Log::Info("TXXXAnalysis: PostLoop");
   TGo4Log::Info("Last event: %d, total events: %d", fLastEvent, fEvents);
   if(fMbsEvent) {
      // we can check some properties of last event here:
      fMbsEvent->PrintEvent(); // header and data content

      // fileheader structure:
      fMbsEvent->PrintMbsFileHeader();

      // mbs buffer header structure:
      fMbsEvent->PrintMbsBufferHeader();
    }

   fMbsEvent = nullptr; // reset to avoid invalid pointer if analysis is changed in between
   fRawEvent = nullptr;
   fCalEvent = nullptr;
   fEvents = 0;
   return 0;
}

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserEventFunc()
{
//// This function is called once for each event.
   Int_t value = 0;
   Int_t count = 0;
   if(fMbsEvent) value = fMbsEvent->GetDlen()/2+2; // total longwords
   fSize->Fill(value); // fill histogram
   fEvents++;
   if(fEvents == 1 || IsNewInputFile()) {
      if(fMbsEvent) {
         count = fMbsEvent->GetCount();
         TGo4Log::Info("First event #: %d", count);
         fMbsEvent->PrintMbsBufferHeader();
      }
   }
   fLastEvent = count;
   return 0;
}
