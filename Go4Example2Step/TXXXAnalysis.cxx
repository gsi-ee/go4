#include "TXXXAnalysis.h"

#include <stdlib.h>
#include "Riostream.h"

#include "TH1.h"
#include "TFile.h"

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

// !!! uncomment this line if signature of user analysis class constructor will change !!

// extern "C" TGo4Analysis* CreateUserAnalysis(const char* name) { return new TXXXAnalysis(name); }

//***********************************************************
TXXXAnalysis::TXXXAnalysis() :
   TGo4Analysis(),
   fMbsEvent(0),fRawEvent(0),fCalEvent(0)
{
  cout << "Wrong constructor TXXXAnalysis()!" << endl;
}
//***********************************************************
// this constructor is used
TXXXAnalysis::TXXXAnalysis(const char* name) :
   TGo4Analysis(name),
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

   TString input = "/GSI/lea/gauss.lmd";
   TString out1 = Form("%s_Calibr", name);
   TString out2 = Form("%s_Anl", name);

   TGo4StepFactory* factory1 = new TGo4StepFactory("UnpackFactory");
   factory1->DefEventProcessor("UnpackProc", "TXXXUnpackProc");// object name, class name
   factory1->DefOutputEvent("UnpackEvent", "TXXXUnpackEvent"); // object name, class name

   TGo4MbsFileParameter* source1 = new TGo4MbsFileParameter(input.Data());
   TGo4FileStoreParameter* store1 = new TGo4FileStoreParameter(out1.Data());
   store1->SetOverwriteMode(kTRUE); // overwrite file
   TGo4AnalysisStep* step1 = new TGo4AnalysisStep("Unpack",factory1,source1,store1,0);

   AddAnalysisStep(step1);

   step1->SetSourceEnabled(kTRUE);
   step1->SetStoreEnabled(kFALSE);
   step1->SetProcessEnabled(kTRUE);
   step1->SetErrorStopEnabled(kTRUE);

   // second step definitions:
   // If source is enabled, take output file from step 1 as input.
   // otherwise we use output event from step 1 (set in factory)
   TGo4StepFactory* factory2 = new TGo4StepFactory("AnalysisFactory");
   factory2->DefInputEvent("UnpackEvent", "TXXXUnpackEvent"); // object name, class name
   factory2->DefEventProcessor("AnlProc", "TXXXAnlProc"); // object name, class name
   factory2->DefOutputEvent("AnlEvent", "TXXXAnlEvent"); // object name, class name
   TGo4FileSourceParameter* source2  = new TGo4FileSourceParameter(out1.Data());
   TGo4FileStoreParameter* store2   = new TGo4FileStoreParameter(out2.Data());
   store2->SetOverwriteMode(kTRUE);
   TGo4AnalysisStep* step2    = new TGo4AnalysisStep("Analysis",factory2,source2,store2,0);

   step2->SetSourceEnabled(kFALSE);  // disable file input (output file of step 1)
   step2->SetStoreEnabled(kFALSE);
   step2->SetProcessEnabled(kTRUE);
   step2->SetErrorStopEnabled(kTRUE);
   AddAnalysisStep(step2);

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
            void* timptr=&(bufheader->l_time[0]); // satisfy compiler warning
            cout <<"\tTime: "<<ctime((const time_t*) timptr);
            cout << "\t\t\t + "<<bufheader->l_time[1] << " µs"<<endl;
         }


    }

////////////////////////////////////////////////
// Uncomment this if you want to perform a fit
// each time analysis stops:
////////// first fitter:
//  if(fFitEnvSize)
//     {
//         cout <<"Fitting event size..." << endl;
//         TGo4Fitter* fitter=fFitEnvSize->GetFitter();
//         if(fitter)
//         {
//            //cout <<"Fitter setting histogram and fitting..." << endl;
//            fitter->SetObject("data1", fSize, kFALSE);
//            fitter->DoActions();
//            fitter->PrintLines();
//         }
//     }
////////////////////////////////////////////////
// second fitter:
//   if(fFitEnvSpectrum)
//     {
//         cout <<"Fitting sum spectrum..." << endl;
//         TGo4Fitter* fitter=fFitEnvSpectrum->GetFitter();
//         if(fitter)
//         {
//            //cout <<"Fitter setting histogram and fitting..." << endl;
//            TH1* his=GetHistogram("Sum2");
//            fitter->SetObject("spectrum", his, kFALSE);
//            fitter->DoActions();
//            fitter->PrintLines();
//         }
//     }
//////////////////////////////////////////////


//////////////////////////////////////////////////////
//////// This section is example how to store objects together with events tree:
//   cout <<"Storing parameter fPar to Unpack:" << endl;
//   StoreParameter("Unpack",fPar);
//   cout <<"Storing parameter fPar to Analysis:" << endl;
//   StoreParameter("Analysis",fPar);
//   if(fFitEnvSpectrum)
//     {
//         cout <<"Storing fitter"<<fFitEnvSpectrum->GetName() <<" to analysis" << endl;
//         TGo4Fitter* fitter=fFitEnvSpectrum->GetFitter();
//         StoreFitter("Analysis",fitter);
//     }
//
//   TGo4Condition* winar=GetAnalysisCondition("winconar");
//   if(winar)
//      {
//         cout <<"Storing condition"<< winar->GetName()<<"to analysis" << endl;
//         StoreCondition("Analysis",winar);
//      }
//   cout <<"Storing all conditions to unpack." << endl;
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
         count=fMbsEvent->GetCount();
         cout << "\nFirst event #: " << count  << endl;
         s_bufhe* bufheader=fMbsEvent->GetMbsBufferHeader();
         if(bufheader) {
            cout <<"First Buffer:"<<endl;
            cout <<"\tNumber: "<<bufheader->l_buf << endl;
            void* timptr=&(bufheader->l_time[0]); // satisfy compiler warning
            cout <<"\tTime: "<<ctime((const time_t*) timptr);
            cout << "\t\t\t + "<<bufheader->l_time[1] << " µs"<<endl;
         }
      }
      SetNewInputFile(kFALSE); // we have to reset the newfile flag
   }
   fLastEvent = count;
   return 0;
}
