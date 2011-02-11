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

#include "TYYYAnalysis.h"

#include <stdlib.h>
#include "Riostream.h"

#include "TCanvas.h"
#include "TH1.h"
#include "TFile.h"

#include "Go4EventServer.h"
#include "TGo4AnalysisStep.h"
#include "TGo4Version.h"

#include "TYYYUnpackEvent.h"
#include "TYYYParameter.h"
#include "TYYYRawEvent.h"
#include "TGo4StepFactory.h"

//***********************************************************
TYYYAnalysis::TYYYAnalysis() :
   TGo4Analysis(),
   fRawEvent(0),
   fUnpackEvent(0),
   fSize(0),
   fPar(0),
   fEvents(0)
{
  cout << "Wrong constructor TYYYAnalysis()!" << endl;
}
//***********************************************************
// this constructor is used
TYYYAnalysis::TYYYAnalysis(int argc, char** argv) :
   TGo4Analysis(argc, argv),
   fRawEvent(0),
   fUnpackEvent(0),
   fSize(0),
   fPar(0),
   fEvents(0)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cout << "****  Go4 version mismatch" << endl;
      exit(-1);
   }


   cout << "**** TYYYAnalysis: Create" << endl;

   // the step definitions can be changed in the GUI
   // first step definitions:
   // the name of the step can be used later to get event objects
   TGo4StepFactory* factory1 = new TGo4StepFactory("Unpack-factory");

   factory1->DefUserEventSource("TYYYEventSource"); // class name of user event source
   factory1->DefInputEvent("RawEvent","TYYYRawEvent"); // object name, class name
   factory1->DefEventProcessor("UnpackProc", "TYYYUnpackProc");// object name, class name
   factory1->DefOutputEvent("UnpackEvent", "TYYYUnpackEvent"); // object name, class name

   TGo4UserSourceParameter* source1  = new TGo4UserSourceParameter("befoil50.scf");
   TGo4FileStoreParameter*  store1   = new TGo4FileStoreParameter(Form("%sOutput", argv[0]));
   store1->SetOverwriteMode(kTRUE);
   TGo4AnalysisStep*        step1    = new TGo4AnalysisStep("Unpack",factory1,source1,store1,0);

   step1->SetSourceEnabled(kTRUE);
   step1->SetStoreEnabled(kFALSE);  // disable output
   step1->SetProcessEnabled(kTRUE);
   step1->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step1);

   // uncomment following line to define custom passwords for analysis server
   // DefineServerPasswords("YYYadmin", "YYYctrl", "YYYview");

   //////////////// Parameter //////////////////////////
   // At this point, autosave file has not yet been read!
   // Therefore parameter values set here will be overwritten
   // if an autosave file is there.
   fPar = new TYYYParameter("YYYPar1");
   fPar->frP1 = 100;
   fPar->frP2 = 200;
   AddParameter(fPar);

   if (GetCanvas("TestCanvas")==0) {
      TCanvas* mycan =  new TCanvas("TestCanvas","Does this work?");
      mycan->Divide(2,2);
      AddCanvas(mycan);
   }
}

//***********************************************************
TYYYAnalysis::~TYYYAnalysis()
{
   cout << "**** TYYYAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TYYYAnalysis::UserPreLoop()
{
   cout << "**** TYYYAnalysis: PreLoop" << endl;
   // we update the pointers to the current event structures here:
   fRawEvent = dynamic_cast<TYYYRawEvent*>    (GetInputEvent("Unpack"));   // of step "Unpack"
   fUnpackEvent = dynamic_cast<TYYYUnpackEvent*> (GetOutputEvent("Unpack"));
   fEvents=0;

   // create histogram for UserEventFunc
   // At this point, the histogram has been restored from autosave file if any.
   fSize=(TH1D*)GetHistogram("Eventsize");
   if(fSize==0) {
      // no autosave read, create new and register
      fSize = new TH1D ("Eventsize", "Read columns",160,1,160);
      AddHistogram(fSize);
   }
   ClearObjects("Histograms"); // reset all histograms to 0 before run
   // name specifies folder to clear
   return 0;
}
//-----------------------------------------------------------
Int_t TYYYAnalysis::UserPostLoop()
{
   cout << "**** TYYYAnalysis: PostLoop" << endl;
   cout << " Total events: " << fEvents << endl;
   TCanvas* can = GetCanvas("TestCanvas");
   if(can) {
      TH1* hx=GetHistogram("Xfinal");
      TH1* hy=GetHistogram("Yfinal");
      TH1* hvx=GetHistogram("Vxfinal");
      TH1* hvy=GetHistogram("Vyfinal");
      can->cd(1);
      if(hx) hx->Draw();
      can->cd(2);
      if(hy) hy->Draw();
      can->cd(3);
      if(hvx) hvx->Draw();
      can->cd(4);
      if(hvy) hvy->Draw();
   }

   fUnpackEvent = 0; // reset to avoid invalid pointer if analysis is changed in between
   fRawEvent = 0;
   fEvents=0;
   return 0;
}

//-----------------------------------------------------------
Int_t TYYYAnalysis::UserEventFunc()
{
   //// This function is called once for each event.
   Int_t value=0;
   if(fRawEvent) value = fRawEvent->GetColumns();
   fSize->Fill(value); // fill histogram
   fEvents++;
   return 0;
}
