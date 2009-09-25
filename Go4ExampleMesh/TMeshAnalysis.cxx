#include "TMeshAnalysis.h"

#include <stdlib.h>

#include "Riostream.h"
#include "TH1.h"
#include "TMeshParameter.h"
#include "Go4EventServer.h"
#include "TGo4StepFactory.h"
#include "TGo4AnalysisStep.h"
#include "TGo4Version.h"


extern "C" TGo4Analysis* CreateUserAnalysis(const char* name) { return new TMeshAnalysis(name); }

//***********************************************************
TMeshAnalysis::TMeshAnalysis() :
   TGo4Analysis(),
   fMbsEvent(0),
   fSize(0),
   fEvents(0),
   fLastEvent(0)
{
  cout << "Wrong constructor TMeshAnalysis()!" << endl;
}
//***********************************************************
// this constructor is used
TMeshAnalysis::TMeshAnalysis(const char* name) :
   TGo4Analysis(name),
   fMbsEvent(0),
   fSize(0),
   fEvents(0),
   fLastEvent(0)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cout << "****  Go4 version mismatch" << endl;
      exit(-1);
   }

   cout << "**** TMeshAnalysis: Create" << endl;

   SetStepChecking(kFALSE); // necessary for non-subsequent mesh analysis

   // first step definitions:
   TGo4StepFactory* factory1 = new TGo4StepFactory("UnpackFactory");
   factory1->DefOutputEvent("RawEvent", "TMeshRawEvent");
   factory1->DefEventProcessor("Unpacker","TMeshUnpackProc");
   TGo4EventSourceParameter* source1 = new TGo4MbsFileParameter("/GSI/lea/gauss.lmd");
   TGo4EventStoreParameter* store1   = new TGo4FileStoreParameter("MeshRawEvent");
   TGo4AnalysisStep* step1  = new TGo4AnalysisStep("Unpack",factory1,source1,store1,0);
   AddAnalysisStep(step1);
   step1->SetSourceEnabled(kTRUE);
   step1->SetStoreEnabled(kFALSE);
   step1->SetProcessEnabled(kTRUE);

   //// second step definitions: Provider for raw component 1
   TGo4StepFactory* factory2 = new TGo4StepFactory("RawProvider1Factory");
   //  factory2->DefInputEvent("RawEvent", "TMeshRawEvent"); // read full raw event without partial io
   //  factory2->DefEventProcessor("RawEvent_1","TMeshProviderProc"); // processorname must match name of input event + "_"
   factory2->DefInputEvent("RawEvent.fxSub1", "TMeshRawEvent"); // try partial io: eventname must match branch name!
   factory2->DefEventProcessor("RawEvent.fxSub1_1","TMeshProviderProc"); // processorname must match name of input event +"_"
   factory2->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step2     = new TGo4AnalysisStep("Input1Provider",factory2,0,0,0);
   AddAnalysisStep(step2);
   step2->SetSourceEnabled(kFALSE);
   step2->SetStoreEnabled(kFALSE);
   step2->SetProcessEnabled(kTRUE);

   //// third step definitions: Provider for raw component 2
   TGo4StepFactory* factory3 = new TGo4StepFactory("RawProvider2Factory");
   //  factory3->DefInputEvent("RawEvent", "TMeshRawEvent"); // read full raw event without partial io
   //  factory3->DefEventProcessor("RawEvent_2","TMeshProviderProc"); // name must match name of input event!
   factory3->DefInputEvent("RawEvent.fxSub2", "TMeshRawEvent"); // build full raw event, but fill only subbranch as defined in name
   factory3->DefEventProcessor("RawEvent.fxSub2_1","TMeshProviderProc");
   factory3->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step3     = new TGo4AnalysisStep("Input2Provider",factory3,0,0,0);
   AddAnalysisStep(step3);
   step3->SetSourceEnabled(kFALSE);
   step3->SetStoreEnabled(kFALSE);
   step3->SetProcessEnabled(kTRUE);

   //// fourth step definitions: Provider for raw component 3
   TGo4StepFactory* factory4 = new TGo4StepFactory("RawProvider3Factory");
   //  factory4->DefInputEvent("RawEvent", "TMeshRawEvent"); // read full raw event without partial io
   //  factory4->DefEventProcessor("RawEvent_3","TMeshProviderProc"); // name must match name of input event!
   factory4->DefInputEvent("RawEvent.fxSub3", "TMeshRawEvent"); // build full raw event, but fill only subbranch as defined in name
   factory4->DefEventProcessor("RawEvent.fxSub3_1","TMeshProviderProc");
   factory4->DefOutputEvent("Dummy", "TMeshDummyEvent");
   // provider factories without definition of output event and proc
   TGo4AnalysisStep* step4     = new TGo4AnalysisStep("Input3Provider",factory4,0,0,0);
   AddAnalysisStep(step4);
   step4->SetSourceEnabled(kFALSE);
   step4->SetStoreEnabled(kFALSE);
   step4->SetProcessEnabled(kTRUE);

   //// fifth step definitions: Execution step to produce subunit 1 event
   TGo4StepFactory* factory5 = new TGo4StepFactory("Branch1Factory");
   factory5->DefOutputEvent("EventB1", "TMeshB1OutputEvent");
   factory5->DefEventProcessor("Branch1","TMeshB1AnlProc");
   TGo4EventStoreParameter* store5   = new TGo4FileStoreParameter("MeshB1");
   TGo4AnalysisStep* step5     = new TGo4AnalysisStep("Exec1",factory5,0,store5,0);
   // no own event source for execution step
   AddAnalysisStep(step5);
   step5->SetSourceEnabled(kFALSE);
   step5->SetStoreEnabled(kTRUE);
   step5->SetProcessEnabled(kTRUE);

   //// sixth step definitions: Execution step to produce subunit 2 event
   TGo4StepFactory* factory6 = new TGo4StepFactory("Branch2Factory");
   factory6->DefOutputEvent("EventB2", "TMeshB2OutputEvent");
   factory6->DefEventProcessor("Branch2","TMeshB2AnlProc");
   TGo4EventStoreParameter* store6   = new TGo4FileStoreParameter("MeshB2");
   TGo4AnalysisStep* step6     = new TGo4AnalysisStep("Exec2",factory6,0,store6,0);
   // no own event source for execution step
   AddAnalysisStep(step6);
   step6->SetSourceEnabled(kFALSE);
   step6->SetStoreEnabled(kTRUE);
   step6->SetProcessEnabled(kTRUE);

   //// seventh step definitions: Execution step to produce subunit 3 event
   TGo4StepFactory* factory7 = new TGo4StepFactory("Branch3Factory");
   factory7->DefOutputEvent("EventB3", "TMeshB3OutputEvent");
   factory7->DefEventProcessor("Branch3","TMeshB3AnlProc");
   TGo4EventStoreParameter* store7   = new TGo4FileStoreParameter("MeshB3");
   TGo4AnalysisStep* step7     = new TGo4AnalysisStep("Exec3",factory7,0,store7,0);
   // no own event source for execution step
   AddAnalysisStep(step7);
   step7->SetSourceEnabled(kFALSE);
   step7->SetStoreEnabled(kTRUE);
   step7->SetProcessEnabled(kTRUE);


   /// eigth step definitions: Provider for subunit 1 event
   TGo4StepFactory* factory8 = new TGo4StepFactory("OutProvider1Factory");
   factory8->DefInputEvent("EventB1", "TMeshB1OutputEvent");
   factory8->DefEventProcessor("EventB1_1","TMeshProviderProc"); // name must match name of input event!
   factory8->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step8     = new TGo4AnalysisStep("Output1Provider",factory8,0,0,0);
   AddAnalysisStep(step8);
   step8->SetSourceEnabled(kFALSE);
   step8->SetStoreEnabled(kFALSE);
   step8->SetProcessEnabled(kTRUE);

   /// ninth step definitions: Provider for subunit 2 event
   TGo4StepFactory* factory9 = new TGo4StepFactory("OutProvider2Factory");
   factory9->DefInputEvent("EventB2", "TMeshB2OutputEvent");
   factory9->DefEventProcessor("EventB2_1","TMeshProviderProc"); // name must match name of input event!
   factory9->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step9     = new TGo4AnalysisStep("Output2Provider",factory9,0,0,0);
   AddAnalysisStep(step9);
   step9->SetSourceEnabled(kFALSE);
   step9->SetStoreEnabled(kFALSE);
   step9->SetProcessEnabled(kTRUE);

   /// tenth step definitions: Provider for subunit 3 event
   TGo4StepFactory* factory10 = new TGo4StepFactory("OutProvider3Factory");
   factory10->DefInputEvent("EventB3", "TMeshB3OutputEvent");
   factory10->DefEventProcessor("EventB3_1","TMeshProviderProc"); // name must match name of input event!
   factory10->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step10     = new TGo4AnalysisStep("Output3Provider",factory10,0,0,0);
   AddAnalysisStep(step10);
   step10->SetSourceEnabled(kFALSE);
   step10->SetStoreEnabled(kFALSE);
   step10->SetProcessEnabled(kTRUE);

   //// eleventh step definitions: Execution step to produce combined 1/2 subunit event
   TGo4StepFactory* factory11 = new TGo4StepFactory("Branch12Factory");
   factory11->DefOutputEvent("EventB12", "TMeshB12OutputEvent");
   factory11->DefEventProcessor("Branch12","TMeshB12AnlProc");
   TGo4EventStoreParameter* store11   = new TGo4FileStoreParameter("MeshB12");
   TGo4AnalysisStep* step11     = new TGo4AnalysisStep("Exec12",factory11,0,store11,0);
   // no own event source for execution step
   AddAnalysisStep(step11);
   step11->SetSourceEnabled(kFALSE);
   step11->SetStoreEnabled(kTRUE);
   step11->SetProcessEnabled(kTRUE);

   /// twelth step definitions: Provider for subunit 1/2 combined event
   TGo4StepFactory* factory12 = new TGo4StepFactory("OutProvider12Factory");
   factory12->DefInputEvent("EventB12", "TMeshB12OutputEvent");
   factory12->DefEventProcessor("EventB12_1","TMeshProviderProc"); // name must match name of input event!
   factory12->DefOutputEvent("Dummy", "TMeshDummyEvent");
   TGo4AnalysisStep* step12     = new TGo4AnalysisStep("Output12Provider",factory12,0,0,0);
   AddAnalysisStep(step12);
   step12->SetSourceEnabled(kFALSE);
   step12->SetStoreEnabled(kFALSE);
   step12->SetProcessEnabled(kTRUE);

   //// thirteenth step definitions: Execution step to collect final event
   TGo4StepFactory* factory13 = new TGo4StepFactory("Branch13Factory");
   factory13->DefOutputEvent("FinalEvent", "TMeshFinalEvent");
   factory13->DefEventProcessor("Collector","TMeshFinalProc");
   TGo4EventStoreParameter* store13   = new TGo4FileStoreParameter("MeshFinal");
   TGo4AnalysisStep* step13     = new TGo4AnalysisStep("Final",factory13,0,store13,0);
   // no own event source for execution step
   AddAnalysisStep(step13);
   step13->SetSourceEnabled(kFALSE);
   step13->SetStoreEnabled(kTRUE);
   step13->SetProcessEnabled(kTRUE);


   // uncomment following line to define custom passwords for analysis server
   // DefineServerPasswords("Meshadmin", "Meshctrl", "Meshview");


   //////////////// Parameter //////////////////////////
   // At this point, autosave file has not yet been read!
   // Therefore parameter values set here will be overwritten
   // if an autosave file is there.
   fPar = new TMeshParameter("MeshSetup");
   AddParameter(fPar);
}

//***********************************************************
TMeshAnalysis::~TMeshAnalysis()
{
  cout << "**** TMeshAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshAnalysis::UserPreLoop()
{
  cout << "**** TMeshAnalysis: PreLoop" << endl;
   // we update the pointers to the current event structures here:
   fMbsEvent = dynamic_cast<TGo4MbsEvent*>    (GetInputEvent("Unpack"));   // of step "Unpack"
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


   return 0;
}
//-----------------------------------------------------------
Int_t TMeshAnalysis::UserPostLoop()
{
  cout << "**** TMeshAnalysis: PostLoop" << endl;
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
            cout <<"Last Buffer:" << endl;
            cout <<"\tNumber: " << bufheader->l_buf << endl;
            void* timptr=&(bufheader->l_time[0]); // satisfy compiler warning
            cout <<"\tTime: "<<ctime((const time_t*) timptr);
            cout << "\t\t\t + " << bufheader->l_time[1] << " µs"<<endl;
         }


    }

/////////////////////////////

   fMbsEvent = 0; // reset to avoid invalid pointer if analysis is changed in between
   fEvents=0;
   return 0;
}

//-----------------------------------------------------------
Int_t TMeshAnalysis::UserEventFunc()
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
