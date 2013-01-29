// $Id: TXXXAnalysis.cxx 487 2009-11-03 17:06:10Z goofy $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXAnalysis.h"

#include <stdlib.h>

#include "TH1.h"
#include "TFile.h"
#include "TSystem.h"

extern "C" {
   #include "s_filhe_swap.h"
   #include "s_bufhe_swap.h"
   #include "f_ut_utime.h"
}

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
   TGo4Log::Error("Wrong constructor TXXXAnalysis()!");
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
      TGo4Log::Error("Go4 version mismatch");
      exit(-1);
   }

   TGo4Log::Info("TXXXAnalysis: Create %s", argv[0]);

   TString kind, input, out1, out2;

   // this is a way to get user-specific arguments in batch mode, like:
   //   shell> go4analysis -x -file|-transport|-stream|-random name
   // in this case argv[0] will be analysis name (default is "Go4Analysis")
   //              argv[1] should be type of source
   //              argv[2] should be "name" of file or MBS node
   // any kind of additional arguments can be supplied

   switch(argc){
      case 1:
         TGo4Log::Info("Configure with default parameters");
         kind = "-file";
         input = "test";
         out1 = "Output_Calib";
         out2 = "Output_Anl";
         break;

      case 2: // default kind
         TGo4Log::Info("Configure with user-specified parameters");
         kind = "-file";
         input = TString::Format("%s", argv[1]);
         out1 = TString::Format("%s_Calib", argv[1]);
         out2 = TString::Format("%s_Anl", argv[1]);
         break;
      case 3:
      default:
         TGo4Log::Info("Configure with user-specified parameters");
         kind = TString::Format("%s", argv[1]);
         input = TString::Format("%s", argv[2]);
         out1 = TString::Format("%s_Calib", argv[2]);
         out2 = TString::Format("%s_Anl", argv[2]);
         break;
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

   // uncomment following line to define custom passwords for analysis server
   // DefineServerPasswords("XXXadmin", "XXXctrl", "XXXview");

   //////////////// Parameter //////////////////////////
   // At this point, autosave file has not yet been read!
   // Therefore parameter values set here will be overwritten
   // if an autosave file is there.
   fPar = new TXXXParameter("XXXPar1");
   fPar->frP1 = 100;
   fPar->frP2 = 200;
   AddParameter(fPar);
   fPar = new TXXXParameter("XXXPar2");
   fPar->frP1 = 1000;
   fPar->frP2 = 2000;
   AddParameter(fPar);

   // check that file setup.C is existing in current directory

   ExecuteScript(Form("setup.C(\"%s\",\"%s\")", kind.Data(), input.Data()));
}

//***********************************************************
TXXXAnalysis::~TXXXAnalysis()
{
   TGo4Log::Info("TXXXAnalysis: Delete");
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPreLoop()
{
   TGo4Log::Info("TXXXAnalysis: PreLoop");
   Print(); // printout the step settings
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
  // we use a fitter envelope parameters to exchange fit results:
  fFitEnvSize=(TGo4FitterEnvelope*) GetParameter("sizefitter");
  if(fFitEnvSize==0)
   {
     TGo4Fitter* fitter=new TGo4Fitter("Gaussfit", TGo4Fitter::ff_ML_Poisson, kTRUE);;
      // add histogram to fitter, which should be fitted
      fitter->AddH1("data1", fSize, kFALSE);
      // create polynom of first order
      //fitter->AddPolynomX("data1", "Pol", 1);
      // create gaussian
      fitter->AddGauss1("data1", "Gauss1",15,5,1000);
      fFitEnvSize=new TGo4FitterEnvelope("sizefitter",fitter);
      AddParameter(fFitEnvSize);
   }
   fFitEnvSpectrum=(TGo4FitterEnvelope*) GetParameter("specfitter");
   if(fFitEnvSpectrum==0)
   {
     TGo4Fitter* fitter=new TGo4Fitter("Multilines", TGo4Fitter::ff_ML_Poisson, kTRUE);;
      // add histogram to fitter, which should be fitted
      fitter->AddH1("spectrum", fSize, kFALSE);
      // create polynom of first order
      fitter->AddPolynomX("spectrum", "Pol", 1);
      // create gaussian
      fitter->AddGauss1("spectrum", "Gauss1",500,20,1000);
      fFitEnvSpectrum=new TGo4FitterEnvelope("specfitter",fitter);
      AddParameter(fFitEnvSpectrum);
   }

   return 0;
}
//-----------------------------------------------------------
Int_t TXXXAnalysis::UserPostLoop()
{
   TGo4Log::Info("TXXXAnalysis: PostLoop");
   TGo4Log::Info("Last event: %d  Total events: %d", fLastEvent, fEvents);
  if(fMbsEvent)
    {
      // we can check some properties of last event here:
      //fMbsEvent->PrintEvent(); // header and data content

      // fileheader structure:
     fMbsEvent->PrintMbsFileHeader();

     // mbs buffer header structure:
     fMbsEvent->PrintMbsBufferHeader();
   }
////////////////////////////////////////////////
// Uncomment this if you want to perform a fit
// each time analysis stops:
////////// first fitter:
//  if(fFitEnvSize)
//     {
//         std::cout <<"Fitting event size..." << std::endl;
//         TGo4Fitter* fitter=fFitEnvSize->GetFitter();
//         if(fitter)
//         {
//            //std::cout <<"Fitter setting histogram and fitting..." << std::endl;
//            fitter->SetObject("data1", fSize, kFALSE);
//            fitter->DoActions();
//            fitter->PrintLines();
//         }
//     }
////////////////////////////////////////////////
// second fitter:
//   if(fFitEnvSpectrum)
//     {
//         std::cout <<"Fitting sum spectrum..." << std::endl;
//         TGo4Fitter* fitter=fFitEnvSpectrum->GetFitter();
//         if(fitter)
//         {
//            //std::cout <<"Fitter setting histogram and fitting..." << std::endl;
//            TH1* his=GetHistogram("Sum2");
//            fitter->SetObject("spectrum", his, kFALSE);
//            fitter->DoActions();
//            fitter->PrintLines();
//         }
//     }
//////////////////////////////////////////////


//////////////////////////////////////////////////////
//////// This section is example how to store objects together with events tree:
//   std::cout <<"Storing parameter fPar to Unpack:" << std::endl;
//   StoreParameter("Unpack",fPar);
//   std::cout <<"Storing parameter fPar to Analysis:" << std::endl;
//   StoreParameter("Analysis",fPar);
//   if(fFitEnvSpectrum)
//     {
//         std::cout <<"Storing fitter"<<fFitEnvSpectrum->GetName() <<" to analysis" << std::endl;
//         TGo4Fitter* fitter=fFitEnvSpectrum->GetFitter();
//         StoreFitter("Analysis",fitter);
//     }
//
//   TGo4Condition* winar=GetAnalysisCondition("winconar");
//   if(winar)
//      {
//         std::cout <<"Storing condition"<< winar->GetName()<<"to analysis" << std::endl;
//         StoreCondition("Analysis",winar);
//      }
//   std::cout <<"Storing all conditions to unpack." << std::endl;
//   StoreFolder("Unpack","Conditions");
//////////////////////////////////////////////////////////////////////

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
         count = fMbsEvent->GetCount();
         TGo4Log::Info("First event #: %d", count);

         // mbs buffer header structure:
         fMbsEvent->PrintMbsBufferHeader();
      }
      SetNewInputFile(kFALSE); // we have to reset the newfile flag
   }
   fLastEvent = count;
   return 0;
}
