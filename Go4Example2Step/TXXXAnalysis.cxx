// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXAnalysis.h"

#include <stdlib.h>
#include <time.h>
#include "Riostream.h"

#include "TH1.h"
#include "TFile.h"
#include "TSystem.h"

#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"
#include "TGo4Fitter.h"
#include "TGo4FitterEnvelope.h"
#include "TGo4AnalysisStep.h"
#include "TGo4StepFactory.h"
#include "TXXXParameter.h"
#include "TXXXUnpackEvent.h"
#include "TXXXAnlEvent.h"
#include "TGo4Version.h"

#include "Go4EventServer.h"

//***********************************************************
TXXXAnalysis::TXXXAnalysis() :
   TGo4Analysis(),
   fMbsEvent(0),
   fRawEvent(0),
   fCalEvent(0)
{
  cout << "Wrong constructor TXXXAnalysis()!" << endl;
}

//***********************************************************
// this constructor is called by go4analysis executable
TXXXAnalysis::TXXXAnalysis(int argc, char** argv) :
   TGo4Analysis(argc, argv),
   fMbsEvent(0),
   fRawEvent(0),
   fCalEvent(0),
   fSize(0),
   fEvents(0),
   fLastEvent(0)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cout << "****  Go4 version mismatch" << endl;
      exit(-1);
   }

   cout << "**** TXXXAnalysis: Create" << endl;

   TString input, out1, out2;

   // this is a way to get user-specific arguments in batch mode, like:
   //   shell> go4analysis -x -f|-t name
   // in this case argv[0] will be analysis name (default is "Go4Analysis")
   //              argv[1] should be -file or -transport
   //              argv[2] should be "name" of file or MBS node
   // any kind of additional arguments can be supplied

   if (argc>1) {
      cout << "**** Configure with user-specified parameters ****" << endl;
    	  input = Form("%s", argv[1]);
      out1 = Form("%s_Calib", argv[1]);
      out2 = Form("%s_Anl", argv[1]);
   } else {
	      cout << "**** Arguments: name ****" << endl;
	      cout << "**** Configure with default parameters ****" << endl;
      input = "gauss";
      out1 = "Output_Calib";
      out2 = "Output_Anl";
   }
// Create step 1 Unpack.
   TGo4StepFactory* factory1 = new TGo4StepFactory("UnpackFactory");
   factory1->DefEventProcessor("UnpackProc", "TXXXUnpackProc");// object name, class name
   factory1->DefOutputEvent("UnpackEvent", "TXXXUnpackEvent"); // object name, class name
   TGo4AnalysisStep* step1 = new TGo4AnalysisStep("Unpack",factory1,0,0,0);
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
   TGo4AnalysisStep* step2    = new TGo4AnalysisStep("Analysis",factory2,0,0,0);
   step2->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step2);
// These settings will be overwritten by setup.C
   step2->SetSourceEnabled(kFALSE);
   step2->SetStoreEnabled(kFALSE);
   step2->SetProcessEnabled(kTRUE);


   //////////////// Parameter //////////////////////////
   // At this point, auto-save file has not yet been read!
   // Therefore parameter values set here will be overwritten
   // if an auto-save file is there.
   fPar = new TXXXParameter("XXXPar1");
   AddParameter(fPar);
   fPar = new TXXXParameter("XXXPar2");
   AddParameter(fPar);

   // check that file setup.C is existing in current directory
   if (!gSystem->AccessPathName("setup.C"))
   gROOT->ProcessLine(Form(".x setup.C(\"%s\")", input.Data()));
   else
      cout << "**** Cannot find setup.C script in current directory ! ****" << endl;
}
//***********************************************************
TXXXAnalysis::~TXXXAnalysis()
{
  cout << "**** TXXXAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPreLoop()
{
  cout << "**** TXXXAnalysis: PreLoop" << endl;
  Print(); // printout the step settings
  cout << "**************************************" << endl;
   // we update the pointers to the current event structures here:
   fMbsEvent = dynamic_cast<TGo4MbsEvent*>    (GetInputEvent("Unpack"));   // of step "Unpack"
   fRawEvent = dynamic_cast<TXXXUnpackEvent*> (GetOutputEvent("Unpack"));
   fCalEvent = dynamic_cast<TXXXAnlEvent*>    (GetOutputEvent("Analysis"));
   fEvents=0;
   fLastEvent=0;



   // create histogram for UserEventFunc
   // At this point, the histogram has been restored from autosave file if any.
  fSize=(TH1D*)GetHistogram("Eventsize");
  if(fSize==0)
    { // no autosave read, create new and register
      fSize = new TH1D ("Eventsize", "Event size [b]",160,1,160);
      AddHistogram(fSize);
    }
   return 0;
}
//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPostLoop()
{
  cout << "**** TXXXAnalysis: PostLoop" << endl;
  cout << "Last event: " << fLastEvent << " Total events: " << fEvents << endl;
  if(fMbsEvent)
    {
      // we can check some properties of last event here:
      //fMbsEvent->PrintEvent(); // header and data content

      // fileheader structure:
      s_filhe* fileheader=fMbsEvent->GetMbsSourceHeader();
      if(fileheader)
         {
           cout <<"\nInput file was: "<<fileheader->filhe_file << endl;
           cout <<"Tapelabel:\t" << fileheader->filhe_label<<endl;
           cout <<"UserName:\t" << fileheader->filhe_user<<endl;
           cout <<"RunID:\t" << fileheader->filhe_run<<endl;
           cout <<"\tExplanation: "<<fileheader->filhe_exp <<endl;
           cout <<"\tComments: "<<endl;
           Int_t numlines=fileheader->filhe_lines;
           for(Int_t i=0; i<numlines;++i)
            {
               cout<<"\t\t"<<fileheader->s_strings[i].string << endl;
            }
         }

      // mbs buffer header structure:
      s_bufhe* bufheader=fMbsEvent->GetMbsBufferHeader();
      if(bufheader)
         {
            cout <<"Last Buffer:"<<endl;
            cout <<"\tNumber: "<<bufheader->l_buf << endl;
            cout <<"\tTime: " << ctime((const time_t*) (void*) &(bufheader->l_time[0]));
            cout << "\t\t\t + "<<bufheader->l_time[1] << " µs"<<endl;
         }


    }

   fMbsEvent = 0; // reset to avoid invalid pointer if analysis is changed in between
   fRawEvent = 0;
   fCalEvent = 0;
   fEvents=0;
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
         count=fMbsEvent->GetCount();
         cout << "\nFirst event #: " << count  << endl;
         s_bufhe* bufheader=fMbsEvent->GetMbsBufferHeader();
         if(bufheader) {
            cout <<"First Buffer:"<<endl;
            cout <<"\tNumber: "<<bufheader->l_buf << endl;
            cout <<"\tTime: " << ctime((const time_t*) (void*) &(bufheader->l_time[0]));
            cout << "\t\t\t + "<< bufheader->l_time[1] << " µs"<<endl;
         }
      }
      SetNewInputFile(kFALSE); // we have to reset the newfile flag
   }
   fLastEvent = count;
   return 0;
}
