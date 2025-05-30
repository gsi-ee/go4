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

#include "TGo4AnalysisClientImp.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TDatime.h"
#include "TDataType.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#include "TGo4CommandInvoker.h"
#include "TGo4AnalysisCommandList.h"
#include "TGo4ComServerQuit.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Task.h"
#include "TGo4TaskHandler.h"
#include "TGo4AnalysisClientStatus.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4AnalysisSniffer.h"
#include "TGo4HistogramServer.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisStep.h"

#include "TGo4AnalysisMainRunnable.h"
#include "TGo4AnalysisWatchRunnable.h"
#include "TGo4Ratemeter.h"

const char *TGo4AnalysisClient::fgcWATCHTHREAD = "WATCH-";
const char *TGo4AnalysisClient::fgcMAINTHREAD = "MAIN-";
const UInt_t TGo4AnalysisClient::fguSTATUSUPDATE = 1000; // events between two updates
const Double_t TGo4AnalysisClient::fgdSTATUSTIMEOUT = 2; // maximum seconds between two updates
const UInt_t TGo4AnalysisClient::fguCINTTIMERPERIOD = 200; // frequency of timer for cint lock


TGo4AnalysisClient::TGo4AnalysisClient(const char *name,
                                       TGo4Analysis *analysis,
                                       const char *host,
                                       UInt_t negport,
                                       Bool_t histoserver,
                                       const char *basename,
                                       const char *passwd,
                                       Bool_t servermode,
                                       Bool_t autorun,
                                       Bool_t cintmode,
                                       Bool_t loadprefs,
                                       Bool_t showrate) :
   TGo4Slave(name, servermode, host, negport),
   fdBufferUpdateTime(0),
   fbAutoStart(autorun),
   fbCintMode(kFALSE),
   fbLoadPrefs(loadprefs),
   fbShowRate(showrate)
{
   GO4TRACE((15,"TGo4AnalysisClient::TGo4AnalysisClient(const char *,...)",__LINE__, __FILE__));

   if(!analysis) {
      TGo4Log::Debug("!!! AnalysisClient ''%s'': no external analysis specified !!",GetName());
      fxAnalysis = TGo4Analysis::Instance();
   } else {
      fxAnalysis = analysis;
   }
   fxAnalysis->SetAnalysisClient(this);

   Constructor(histoserver, basename, passwd);

   SetCintMode(cintmode);
}

TGo4AnalysisClient::TGo4AnalysisClient(int argc, char **argv,
                                       TGo4Analysis *analysis,
                                       Bool_t histoserver,
                                       const char *basename,
                                       const char *passwd,
                                       Bool_t servermode,
                                       Bool_t autorun) :
   TGo4Slave(argv[2], servermode, argv[3], (argc>4) ? atoi(argv[4]) : 5000),
   fdBufferUpdateTime(0),
   fbAutoStart(autorun),
   fbCintMode(kFALSE),
   fbLoadPrefs(kTRUE),
   fbShowRate(kFALSE)
{
   GO4TRACE((15,"TGo4AnalysisClient::TGo4AnalysisClient(int, char**...)",__LINE__, __FILE__));

   if (argc < 5) {
      TGo4Log::Error("!!! AnalysisClient: missing commandline arguments, aborting !!");
      gApplication->Terminate();
   }
   if (!strcmp("-lGUI", argv[1])) {
      TGo4Log::Error(" !!! AnalysisClient: GUI mode not specified, aborting !!");
      gApplication->Terminate();
   }

   if (!analysis) {
      TGo4Log::Debug(" !!! AnalysisClient ''%s'': no external analysis specified !!", GetName());
      fxAnalysis = TGo4Analysis::Instance();
   } else {
      fxAnalysis = analysis;
   }
   fxAnalysis->SetAnalysisClient(this);
   Constructor(histoserver, basename, passwd);
}

void TGo4AnalysisClient::Constructor(Bool_t starthistserv, const char *basename,  const char *passwd)
{
   if (IsServer()) {
      if (fxAnalysis->fServerObserverPass.Length()>0)
         TGo4TaskHandler::SetObservAccount(nullptr, fxAnalysis->fServerObserverPass.Data());
      if (fxAnalysis->fServerCtrlPass.Length()>0)
         TGo4TaskHandler::SetCtrlAccount(nullptr, fxAnalysis->fServerCtrlPass.Data());
      if (fxAnalysis->fServerAdminPass.Length()>0)
         TGo4TaskHandler::SetAdminAccount(nullptr, fxAnalysis->fServerAdminPass.Data());
   }

   fxRatemeter = new TGo4Ratemeter;
   TGo4Log::Debug(" AnalysisClient ''%s'' started ",GetName());

   fcMainName = TString::Format("%s%s", fgcMAINTHREAD, GetName());
   fcWatchName = TString::Format("%s%s", fgcWATCHTHREAD, GetName());

   TGo4AnalysisMainRunnable* mainrun =
      new TGo4AnalysisMainRunnable(TString::Format("MainRunnable of %s",GetName()).Data(), this);
   TGo4AnalysisWatchRunnable* watchrun =
      new TGo4AnalysisWatchRunnable(TString::Format("WatchRunnable of %s",GetName()).Data(), this);

      // adding runnables to thread handler who takes over the responsibility...:
   TGo4ThreadHandler* th = GetThreadHandler();
   th->NewThread(fcMainName.Data(), mainrun);
   th->NewThread(fcWatchName.Data(), watchrun);

   TGo4CommandInvoker::Instance(); // make sure we have an invoker instance!
   TGo4CommandInvoker::SetCommandList(new TGo4AnalysisCommandList);
   TGo4CommandInvoker::Register("AnalysisClient",this); // register as command receiver at the global invoker
   TGo4Slave::Stop();// prevent analysis to run before init
   UpdateStatusBuffer();
   if(starthistserv) StartObjectServer(basename, passwd);
   GetTask()->Launch();
}


TGo4AnalysisClient::~TGo4AnalysisClient()
{
   GO4TRACE((15,"TGo4AnalysisClient::~TGo4AnalysisClient()",__LINE__, __FILE__));
//   if(GetTask())
//      {
//         GetTask()->GetTaskHandler()->DisConnect(); // disconnect before we autosave etc.
//         GetTask()->GetTaskHandler()->SetAborting(); // for superclass dtor: set fast abort mode
//      }
   fxAnalysis->LockAutoSave();   // wait until mainthread finishes the
                                 // final autosaving before we cancel
   StopObjectServer(); // first cancel histoserver and its thread!
   if(GetThreadHandler()) // make sure threads wont work on application when its deleted
       GetThreadHandler()->CancelAll();

   fxAnalysis->UnLockAutoSave();

   delete fxRatemeter;
   delete fxAnalysis;
   TGo4CommandInvoker::UnRegister(this);
}


Int_t TGo4AnalysisClient::Initialization()
{
   TGo4LockGuard mainguard; // threads are already up, protect next actions!
   SendStatusMessage(1, kTRUE, TString::Format("AnalysisClient %s starting initialization...",GetName()));

   if(!fbAutoStart) { // normal mode: load last prefs and wait for submit

      // startup of analysis: get last saved status

      if (fbLoadPrefs) {
         if(fxAnalysis->LoadStatus()) { // will load, close analysis and set the new status
            // we have a status from file, i.e. go4 analysis: wait with init until gui command
            SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s: Status loaded from %s",
                  GetName(), TGo4Analysis::fgcDEFAULTSTATUSFILENAME));
         } else {
            SendStatusMessage(2,kTRUE, TString::Format("AnalysisClient %s: Could not load status from %s",
                  GetName(), TGo4Analysis::fgcDEFAULTSTATUSFILENAME));
         }
      }
      // recover objects and dynamic list links from last autosave file:
      if(!fxAnalysis->IsAutoSaveOn()){
          SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s: AUTOSAVE is DISABLED, Initialization did NOT load analysis objects!",
                GetName()));
      }
      else if(fxAnalysis->LoadObjects()) {
         SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s: Objects loaded.",GetName()));
      }
      else {
         //TGo4Log::Debug(" !!! Analysis Client Initialization --  Could not load dynamic list!!! ");
         SendStatusMessage(2,kTRUE, TString::Format("AnalysisClient %s: Initialization could not load analysis objects!",GetName()));
      }

      SendStatusMessage(1,kTRUE, TString::Format("Analysis Slave %s waiting for submit and start commands...",GetName()));
      TGo4Slave::Stop(); // wait for command from master for start.
   } else {
      // in server mode, analysis slave will begin with analysis run
      // before the master is connected. May not need master anyway!
      // note: we do not recover preferences from file here anymore, all left to command line pars
      SendStatusMessage(1,kTRUE, TString::Format("AnalysisSlave %s initializing analysis...",GetName()));
      if(fxAnalysis->InitEventClasses()) {
         if(IsCintMode()) {
            SendStatusMessage(1,kTRUE, TString::Format("Analysis CINTServer %s in MainCycle suspend mode.",GetName()));
            TGo4Slave::Stop(); // no UserPostLoop
         } else {
            SendStatusMessage(1,kTRUE, TString::Format("AnalysisSlave %s starting analysis...",GetName()));
            Start(); // UserPreLoop execution here!
         }
      } else {
         SendStatusMessage(2,kTRUE, TString::Format("AnalysisSlave %s failed initializing analysis!",GetName()));
         TGo4Slave::Stop();
      }
   } // if(!fbAutoStart)
   SendAnalysisStatus(); // only send status if connections are up!
   UpdateStatusBuffer();   // we need this for gui
   //SendAnalysisClientStatus(); // JAM15 deadlock danger in analysis server mode!
   SendStatusMessage(1,kFALSE, TString::Format("AnalysisClient %s has finished initialization.",GetName()));
   return 0;
}


void TGo4AnalysisClient::UpdateStatus(TGo4TaskStatus *state)
{
   GO4TRACE((12,"TGo4AnalysisClient::UpdateStatus(TGo4TaskStatus *)",__LINE__, __FILE__));
   TGo4Slave::UpdateStatus(state); // fill superclass attributes
   TGo4AnalysisClientStatus* anstate = dynamic_cast<TGo4AnalysisClientStatus*> (state);
   if (anstate) {
      anstate->SetRates(fxRatemeter->GetRate(), fxRatemeter->GetAvRate(), fxRatemeter->GetCurrentCount(), fxRatemeter->GetTime());
      // new: set true running state
      anstate->SetRunning(fxAnalysis->IsRunning());
      // new: set name of current eventsource
      TGo4AnalysisStep *firststep = fxAnalysis->GetAnalysisStep(nullptr);
      if(firststep) {
         anstate->SetCurrentSource(firststep->GetEventSourceName());
         fxRatemeter->SetCurrentSource(firststep->GetEventSourceName());
      } else {
         anstate->SetCurrentSource("- No event source -");
         fxRatemeter->SetCurrentSource("- No event source -");
      }
   }
}

TGo4TaskStatus *TGo4AnalysisClient::CreateStatus()
{
   GO4TRACE((12,"TGo4AnalysisClient::CreateStatus()",__LINE__, __FILE__));
   // note: ratemeter update done by mainthread loop
   TGo4AnalysisClientStatus* stat= new TGo4AnalysisClientStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;
}

void TGo4AnalysisClient::Start()
{
   GO4TRACE((12,"TGo4AnalysisClient::Start()",__LINE__, __FILE__));
   if(fxAnalysis->IsInitDone())
      {
         if(GetThreadHandler()) GetThreadHandler()->Start(fcMainName.Data()); // this is useful anyway...
         if(!MainIsRunning()) fxAnalysis->PreLoop(); // only call once
         TGo4Slave::Start();
         fxRatemeter->Reset();
         fdBufferUpdateTime = TTimeStamp().AsDouble();
         SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s has started analysis processing.",GetName()));
         UpdateRate(-2); // fake rate to display green light :)
         UpdateStatusBuffer();
         //SendAnalysisClientStatus();  // JAM15 deadlock danger in analysis server mode! never do this from main thread
      }
   else
      {
         // user did not initialize analysis, we do not start!
         SendStatusMessage(2,kTRUE, TString::Format("Analysis %s was not initialized! Please SUBMIT settings first.",fxAnalysis->GetName()));
      }
}

void TGo4AnalysisClient::SendAnalysisObject(const char *name)
{
   GO4TRACE((12,"TGo4AnalysisClient::SendAnalysisObject(const char *name)",__LINE__, __FILE__));
   TNamed *ob = fxAnalysis->GetObject(name);
   SendObject(ob);
}

void TGo4AnalysisClient::SendAnalysisStatus()
{
   GO4TRACE((12,"TGo4AnalysisClient::SendAnalysisStatus()",__LINE__, __FILE__));
   TGo4Analysis *ana = GetAnalysis();
   TGo4Log::Debug(" AnalysisClient -  sending current analysis settings ");
   if(ana) {
      TGo4AnalysisStatus *state = ana->CreateStatus();
      SendStatus(state);
      delete state;
   } else {
      SendStatusMessage(3,kFALSE, "ERROR sending analysis status: no analysis ");
   }
}


void TGo4AnalysisClient::SendAnalysisClientStatus()
{
   GO4TRACE((12,"TGo4AnalysisClient::SendAnalysisClientStatus()",__LINE__, __FILE__));

   TGo4Log::Debug(" AnalysisClient -  sending current analysis client status ");
   SendStatusBuffer();
  //TGo4Log::SetIgnoreLevel(0);
//   TGo4Log::Debug("current rate:\t%f",fxRatemeter->GetRate());
//   TGo4Log::Debug("average rate:\t%f",fxRatemeter->GetAvRate());
//   TGo4Log::Debug("running time:\t%f",fxRatemeter->GetTime());
//   TGo4Log::Debug("event number:\t%d",fxRatemeter->GetCurrentCount());
}

void TGo4AnalysisClient::SendNamesList()
{
   GO4TRACE((12,"TGo4AnalysisClient::SendNamesList()",__LINE__, __FILE__));

   fxAnalysis->UpdateNamesList();
   TGo4AnalysisObjectNames *state= fxAnalysis->GetNamesList();
   if(state)
      {
         TGo4Log::Debug(" AnalysisClient - sending names list ");
         //SendStatus(state);
         SendObject(state);
      }
   else
      {
         SendStatusMessage(3,kTRUE,"Analysis Client: Send Names List - ERROR:  no nameslist !!! ");
      }
}

void TGo4AnalysisClient::KillMain()
{
   GO4TRACE((12,"TGo4AnalysisClient::KillMain()",__LINE__, __FILE__));
   if(GetThreadHandler()) GetThreadHandler()->Stop(fcMainName.Data());
   // put dummy buffer to command queue. This will wake up the main thread from command wait.
   if(GetTask()) GetTask()->WakeCommandQueue(); // note that the dummy command will not have the termination id here!
   if(GetThreadHandler()) GetThreadHandler()->Cancel(fcMainName.Data());
   SendStatusMessage(2,kTRUE,TString::Format("AnalysisClient %s has killed main analysis thread.",GetName()));
}

void TGo4AnalysisClient::RestartMain()
{
   GO4TRACE((12,"TGo4AnalysisClient::RestartMain()",__LINE__, __FILE__));
   if(GetThreadHandler()) GetThreadHandler()->Stop(fcMainName.Data());
   // put dummy buffer to command queue. This will wake up the main thread from command wait.
   if(GetTask()) GetTask()->WakeCommandQueue(); // note that the dummy command will not have the termination id here!
   if(GetThreadHandler()) {
      GetThreadHandler()->ReCreate(fcMainName.Data());
      GetThreadHandler()->Start(fcMainName.Data());
   }
   fxRatemeter->Reset();
   SendStatusMessage(2,kTRUE,TString::Format("AnalysisClient %s has killed and relaunched main analysis thread.",GetName()));
}

void TGo4AnalysisClient::Stop()
{
   GO4TRACE((12,"TGo4AnalysisClient::Stop()",__LINE__, __FILE__));

   if(MainIsRunning()) fxAnalysis->PostLoop(); // only call postloop once
   TGo4Slave::Stop(); // will stop for command queue wait
   SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s has STOPPED analysis processing.",GetName()));
   if(GetTask()->IsTerminating()) return; // do not update status buffer from terminating state
   //// test for immediate ratemeter update with zero rate:
   UpdateRate(-1);
   UpdateStatusBuffer();

   //SendAnalysisClientStatus(); // JAM15 deadlock danger in analysis server mode! never do this from main thread

//   if (fxAnalysis && fxAnalysis->IsStopWorking()) {
//      if (IsCintMode()) {
//         fxAnalysis->ResumeWorking();
//      }  else {
//         TGo4Log::StartTracing(); // debug
//         fxAnalysis->CloseAnalysis();
//         TGo4Log::CloseLogfile();
//         exit(0);
//      }
//   }
}

void TGo4AnalysisClient::UpdateRate(Int_t counts)
{
   GO4TRACE((12,"TGo4AnalysisClient::UpdateRate(Int_t)",__LINE__, __FILE__));
   if (fxRatemeter->Update(counts)) {

      auto an = TGo4Analysis::Instance();
      fxRatemeter->SetRunning(an ? an->IsRunning() : kFALSE);

      TDatime dt;
      fxRatemeter->SetDateTime(dt.AsSQLString());

      auto sniff = fxAnalysis->GetSniffer();
      if (sniff) sniff->RatemeterUpdate(fxRatemeter);

      if (fbShowRate)
         TGo4Log::PrintRate(fxRatemeter->GetCurrentCount(), fxRatemeter->GetRate());
   }
}

UInt_t TGo4AnalysisClient::GetCurrentCount() const
{
   return fxRatemeter->GetCurrentCount();
}

Bool_t TGo4AnalysisClient::TestRatemeter()
{
   return fxRatemeter->TestUpdate();
}

Bool_t TGo4AnalysisClient::TestBufferUpdateConditions()
{
   Double_t currenttime = TTimeStamp().AsDouble();
   Double_t deltatime = currenttime - fdBufferUpdateTime;
   UInt_t currentcount = GetCurrentCount();
   if( (currentcount && (currentcount % fguSTATUSUPDATE == 0) && (deltatime>0.1*fgdSTATUSTIMEOUT))
      || (deltatime > fgdSTATUSTIMEOUT) ) {
      // buffer shall be updated if certain number of events is processed or the time is up
      fdBufferUpdateTime = currenttime;
      return kTRUE;
   }

   return kFALSE;
}


void TGo4AnalysisClient::StartObjectServer(const char *basename,  const char *passwd)
{
    StopObjectServer(); // shutdown existing one with old basename/passwd
    fxHistoServer = new TGo4HistogramServer(this,basename,passwd,kFALSE);
    // switch last boolean true if you want to use Go4 object server support
    // default will only enable gsi histogram server JA 9/2005
    //SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s Started Object server.",GetName()));
}

void TGo4AnalysisClient::StopObjectServer()
{
   if(fxHistoServer) {
      delete fxHistoServer;
      fxHistoServer = nullptr;
      //SendStatusMessage(1,kTRUE, TString::Format("AnalysisClient %s: Object server was stopped.",GetName()));
   }
}

void TGo4AnalysisClient::Quit()
{
   Stop();
   fxAnalysis->CloseAnalysis();
}

void TGo4AnalysisClient::Terminate(Bool_t termapp)
{
   SetCintMode(kFALSE);
   StopObjectServer(); // shutdown objectserver and its threads properly
   if(GetTask())
      GetTask()->TGo4ThreadManager::Terminate(termapp); // stops all remaining threads and sets termination flag
}

void TGo4AnalysisClient::TerminateFast()
{
   StopObjectServer(); // shutdown objectserver and its threads properly
   TGo4Log::Debug("TGo4AnalysisClient::TerminateFast with delete analysis");
   if(GetThreadHandler()) {
      GetThreadHandler()->StopAll(); // this will not stop immediately, therefore:
      GetThreadHandler()->Cancel(fcWatchName.Data());
      GetThreadHandler()->Cancel(fcMainName.Data()); // maybe we not need this...
      GetThreadHandler()->Cancel(GetTask()->GetTaskHandler()->GetDatName());
      GetThreadHandler()->Cancel(GetTask()->GetTaskHandler()->GetStatName());
   }
   delete fxAnalysis;
   fxAnalysis = nullptr;
   gApplication->Terminate();
}

void TGo4AnalysisClient::SubmitShutdown()
{
   if(GetTask()) {
      auto com = new TGo4ComServerQuit();
      //TGo4Log::StartTracing(); // debug
      GetTask()->SubmitLocalCommand(com); // shutdown will be performed in local command thread
   }
}

void TGo4AnalysisClient::ExecuteString(const char *command)
{
   if(strstr(command,"ANHServStart")) {
      TString buffer = command;
      strtok((char *) buffer.Data(), ":"); // first find the command itself
      TString base = strtok(nullptr, ":");
      TString pass = strtok(nullptr, ":");
      StartObjectServer(base.Data(), pass.Data());
   } else if (!strcmp(command,"ANHServStop")) {
      StopObjectServer();
   } else{
      fxAnalysis->ExecuteLine(command); // all scripting is handled in analysis singleton
      fflush(stdout);
   }
}

Int_t TGo4AnalysisClient::StartWorkThreads()
{
   TGo4TaskOwner::StartWorkThreads();
   if(GetThreadHandler()) {
      GetThreadHandler()->Start(fcMainName.Data());
      GetThreadHandler()->Start(fcWatchName.Data());
   }
   return 0;
}

Int_t TGo4AnalysisClient::StopWorkThreads()
{
   TGo4TaskOwner::StopWorkThreads();
   if(GetThreadHandler()) {
      GetThreadHandler()->Stop(fcMainName.Data());
      GetThreadHandler()->Stop(fcWatchName.Data());
   }
   return 0;
}

void TGo4AnalysisClient::SetCintMode(Bool_t on)
{
   fbCintMode = on;

   if (fbCintMode) {
      gROOT->SetBatch(kFALSE);
      fxAnalysis->SetAutoSave(kFALSE);
   }
}

void TGo4AnalysisClient::SendStatusMessage(Int_t level, Bool_t printout, const TString &text)
{
    TGo4Slave::SendStatusMessage(level, printout, text);

    auto sniff = fxAnalysis->GetSniffer();
    if (sniff) sniff->StatusMessage(level, printout, text);
}
