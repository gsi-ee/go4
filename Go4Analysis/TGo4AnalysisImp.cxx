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

#include "TGo4AnalysisImp.h"

#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "TSystem.h"
#include "TInterpreter.h"
#include "TApplication.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

#include "TTree.h"
#include "TFile.h"
#include "TKey.h"
#include "TThread.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TCutG.h"
#include "TStopwatch.h"
#include "TTimeStamp.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4CommandInvoker.h"
#include "TGo4Ratemeter.h"
#include "TGo4AnalysisCommandList.h"
#include "TGo4UserException.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4ShapedCond.h"
#include "TGo4ListCond.h"
#include "TGo4RollingGraph.h"

#include "TGo4Version.h"
#include "TGo4AnalysisStepManager.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisWebStatus.h"
#include "TGo4AnalysisStep.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisSniffer.h"
#include "TGo4DynamicListException.h"
#include "TGo4Condition.h"
#include "TGo4Parameter.h"
#include "TGo4ObjEnvelope.h"
#include "TGo4EventElement.h"
#include "TGo4EventProcessor.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4EventEndException.h"
#include "TGo4AnalysisStepException.h"
#include "TGo4EventStoreException.h"

class TGo4InterruptHandler : public TSignalHandler {
public:
   TGo4InterruptHandler() : TSignalHandler(kSigInterrupt, kFALSE) {}

   Bool_t Notify() override
   {
      if (TGo4Analysis::Exists())
         TGo4Analysis::Instance()->ProcessCrtlCSignal();

      return kTRUE;
   }
};

#ifdef _MSC_VER

namespace {
   void InstallGo4CtrlCHandler(bool on);
}

#endif


// _________________________________________________________________________________

TGo4Analysis *TGo4Analysis::fxInstance = nullptr;
Bool_t TGo4Analysis::fbExists = kFALSE;
Int_t TGo4Analysis::fiRunningMode = 0;
const Int_t TGo4Analysis::fgiAUTOSAVECOUNTS = 500;
const Int_t TGo4Analysis::fgiDYNLISTINTERVAL = 1000;
const Int_t TGo4Analysis::fgiMACROSTARTPOLL = 1000; //polling time for macro in WaitForStart
const char *TGo4Analysis::fgcDEFAULTFILENAME = "Go4AutoSave.root";
const char *TGo4Analysis::fgcDEFAULTSTATUSFILENAME = "Go4AnalysisPrefs.root";
const char *TGo4Analysis::fgcDEFAULTFILESUF = ".root";

const char TGo4Analysis::fgcPYPROMPT = '$';
const char *TGo4Analysis::fgcPYINIT = "python/go4init.py";

TGo4Analysis *TGo4Analysis::Instance()
{
   GO4TRACE((14,"TGo4Analysis::Instance()",__LINE__, __FILE__));
   if(!fxInstance) {
      fxInstance = new TGo4Analysis;
      fbExists = kTRUE;
   }
   return fxInstance;
}

Bool_t TGo4Analysis::Exists()
{
   return fbExists;
}

Bool_t TGo4Analysis::IsBatchMode()
{
   return fiRunningMode == 0;
}

Bool_t TGo4Analysis::IsClientMode()
{
   return fiRunningMode == 1;
}

Bool_t TGo4Analysis::IsServerMode()
{
   return fiRunningMode == 2;
}

void TGo4Analysis::SetRunningMode(int mode)
{
   fiRunningMode = mode;
}

TGo4Analysis::TGo4Analysis(const char *name) :
   TObject(),TGo4CommandReceiver(),
   fbInitIsDone(kFALSE),
   fbAutoSaveOn(kTRUE),
   fxAnalysisSlave(nullptr),
   fxStepManager(nullptr),
   fxObjectManager(nullptr),
   fiAutoSaveCount(0),
   fiAutoSaveInterval(TGo4Analysis::fgiAUTOSAVECOUNTS),
   fiAutoSaveCompression(5),
   fxAutoFile(nullptr),
   fbAutoSaveOverwrite(kFALSE),
   fbNewInputFile(kFALSE),
   fxCurrentInputFileName(),
   fbAutoSaveFileChange(kFALSE),
   fxSampleEvent(nullptr),
   fxObjectNames(nullptr),
   fxDoWorkingFlag(flagInit),
   fxInterruptHandler(nullptr),
   fAnalysisName(),
   fBatchLoopCount(-1),
   fServerAdminPass(),
   fServerCtrlPass(),
   fServerObserverPass(),
   fbMakeWithAutosave(kTRUE),
   fbObjMade(kFALSE),
   fbPythonBound(kFALSE),
   fNumCtrlC(0),
   fSniffer(nullptr),
   fxRate(nullptr)
{
   GO4TRACE((15,"TGo4Analysis::TGo4Analysis(const char *)",__LINE__, __FILE__));

   if (name) SetAnalysisName(name);

   Constructor();
}


TGo4Analysis::TGo4Analysis(int argc, char **argv) :
   TObject(),TGo4CommandReceiver(),
   fbInitIsDone(kFALSE),
   fbAutoSaveOn(kTRUE),
   fxAnalysisSlave(nullptr),
   fxStepManager(nullptr),
   fxObjectManager(nullptr),
   fiAutoSaveCount(0),
   fiAutoSaveInterval(TGo4Analysis::fgiAUTOSAVECOUNTS),
   fiAutoSaveCompression(5),
   fxAutoFile(nullptr),
   fbAutoSaveOverwrite(kFALSE),
   fbNewInputFile(kFALSE),
   fxCurrentInputFileName(),
   fbAutoSaveFileChange(kFALSE),
   fxSampleEvent(nullptr),
   fxObjectNames(nullptr),
   fxDoWorkingFlag(flagInit),
   fxInterruptHandler(nullptr),
   fAnalysisName(),
   fBatchLoopCount(-1),
   fServerAdminPass(),
   fServerCtrlPass(),
   fServerObserverPass(),
   fbMakeWithAutosave(kTRUE),
   fbObjMade(kFALSE),
   fbPythonBound(kFALSE),
   fNumCtrlC(0),
   fSniffer(nullptr),
   fxRate(nullptr)
{
   GO4TRACE((15,"TGo4Analysis::TGo4Analysis(const char *)",__LINE__, __FILE__));

   if ((argc > 0) && argv[0]) SetAnalysisName(argv[0]);

   Constructor();
}

void TGo4Analysis::Constructor()
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      // wrong version number between framework and user executable
      Message(-1,"!!!! Analysis Base class:\n\t User Analysis was built with wrong \t\tGo4 Buildversion %d !!!!!",
            TGo4Version::Instance()->GetBuildVersion());
      Message(-1,"\t Please rebuild your analysis with current \tGo4 Buildversion %d ", __GO4BUILDVERSION__);
      Message(-1,"\t >>make clean all<<");
      Message(-1,"Aborting in 20 s...");
      gSystem->Sleep(20000);
      exit(-1);
   } else {
      // may not disable output of version number:
      Message(-1,"Welcome to Go4 Analysis Framework Release %s (build %d) !", __GO4RELEASE__, __GO4BUILDVERSION__);
   }

   if(!fxInstance) {
      gROOT->SetBatch(kTRUE);
      fxStepManager = new TGo4AnalysisStepManager("Go4 Analysis Step Manager");
      fxObjectManager = new TGo4AnalysisObjectManager("Go4 Central Object Manager");
      SetDynListInterval(TGo4Analysis::fgiDYNLISTINTERVAL);
      fxAutoSaveMutex = new TMutex(kTRUE);
      fxAutoSaveClock = new TStopwatch;
      fxAutoSaveClock->Stop();
      fxAutoFileName = fgcDEFAULTFILENAME;
      if (fAnalysisName.Length()>0) fxAutoFileName = TString::Format("%sASF.root", fAnalysisName.Data());
      fxConfigFilename = fgcDEFAULTSTATUSFILENAME;
      fxDefaultTestFileName=TString::Format("%s/data/test.lmd",getenv("GO4SYS"));

      TGo4CommandInvoker::Instance(); // make sure we have an invoker instance!
      TGo4CommandInvoker::SetCommandList(new TGo4AnalysisCommandList);
      TGo4CommandInvoker::Register("Analysis",this); // register as command receiver at the global invoker
      fxInstance = this; // for ctor usage from derived user subclass
      fbExists = kTRUE;

      fxInterruptHandler = new TGo4InterruptHandler();
      fxInterruptHandler->Add();

#ifdef _MSC_VER
      InstallGo4CtrlCHandler(true);
#endif

   } else {
      // instance already there
      Message(2,"Analysis BaseClass ctor -- analysis singleton already exists !!!");
   }
   // settings for macro execution
   TInterpreter* theI = gROOT->GetInterpreter();
   theI->SetProcessLineLock(kTRUE); // mandatory for ROOT > 6.12

   gROOT->ProcessLineSync("TGo4Analysis *go4 = TGo4Analysis::Instance();");
   gROOT->ProcessLineSync(TString::Format(".x %s", TGo4Log::subGO4SYS("macros/anamacroinit.C").Data()).Data());
}


TGo4Analysis::~TGo4Analysis()
{

#ifdef _MSC_VER
   InstallGo4CtrlCHandler(false);
#endif

   if (fxInterruptHandler) {
      delete fxInterruptHandler;
      fxInterruptHandler = nullptr;
   }

   GO4TRACE((15,"TGo4Analysis::~TGo4Analysis()",__LINE__, __FILE__));
   CloseAnalysis();
   CloseAutoSaveFile();
   delete fxStepManager;
   delete fxObjectManager;
   delete fxObjectNames;
   delete fxAutoSaveClock;
   delete fxSampleEvent;
   TGo4CommandInvoker::UnRegister(this);
   fxInstance = nullptr; // reset static singleton instance pointer
   gROOT->ProcessLineSync(TString::Format(".x %s", TGo4Log::subGO4SYS("macros/anamacroclose.C").Data()).Data());
}


void TGo4Analysis::ProcessCrtlCSignal()
{
   switch(fNumCtrlC++) {
      case 0:
         StopWorking();    // for batch mode and server mode
         ShutdownServer(); // only for server mode
         break;
     case 1:
        // if shutdown should hang, we do second try closing the files directly
        CloseAnalysis();
        CloseAutoSaveFile();
        TGo4Log::CloseLogfile();
        if (gApplication) gApplication->Terminate();
        break;
     case 2:
     default:
        exit(1); // the hard way if nothing helps
        break;
   }
}


////////////////////////////////////////////////////
// Initialization and analysis defining methods:

Bool_t TGo4Analysis::InitEventClasses()
{
   GO4TRACE((14,"TGo4Analysis::InitEventClasses()",__LINE__, __FILE__));
   //
   Bool_t rev = kTRUE;
   if(!fbInitIsDone) {
      try {
         TGo4Log::Debug("Analysis --  Initializing EventClasses...");
         LoadObjects(); // always use autosave file to get last objects list
         rev = fxStepManager->InitEventClasses();
         UpdateNamesList();
         TGo4Log::Info("Analysis --  Initializing EventClasses done.");
         fbInitIsDone = kTRUE;
         if (!fxAnalysisSlave) {
            if (fxDoWorkingFlag == flagClosed)
               fxDoWorkingFlag = flagPause;
            else if (fxDoWorkingFlag == flagInit)
               fxDoWorkingFlag = flagPause;
         }

      } catch(TGo4EventErrorException& ex) {
         Message(ex.GetPriority(), "%s", ex.GetErrMess());
         rev = kFALSE;
      }
   } else
      TGo4Log::Info("Analysis BaseClass --  EventClasses were already initialized.");
   return rev;
}

/////////////////////////////////////////////////////////
// main event cycle methods:

Int_t TGo4Analysis::MainCycle()
{
   GO4TRACE((11,"TGo4Analysis::MainCycle()",__LINE__, __FILE__));
   if(!fbInitIsDone)
      throw TGo4UserException(3,"Analysis not yet initialized");

   {
      TGo4LockGuard mainlock; // protect analysis, but not status buffer

      ProcessAnalysisSteps();

      UserEventFunc();

      fxObjectManager->ProcessDynamicList();

      if (fbAutoSaveOn && (fiAutoSaveInterval != 0)) {
         fiAutoSaveCount++;
         Double_t rt = fxAutoSaveClock->RealTime();
         if (rt > (Double_t) fiAutoSaveInterval) {
            Message(0, "Analysis: Main Cycle Autosaving after %.2f s (%d events).",rt,fiAutoSaveCount);
            AutoSave();
            fiAutoSaveCount = 0;
            fxAutoSaveClock->Start(kTRUE);
         } else
            fxAutoSaveClock->Continue();
      }

      SetNewInputFile(kFALSE); // reset flag of new input file

   } //TGo4LockGuard main;

   if(fxAnalysisSlave) {
      fxAnalysisSlave->UpdateRate();
      // note: creation of status buffer uses mainlock internally now
      // status mutex required to be outside main mutex always JA
      if (fxAnalysisSlave->TestBufferUpdateConditions())
         fxAnalysisSlave->UpdateStatusBuffer();
   }

   return 0;
}

Int_t TGo4Analysis::UserEventFunc()
{
   GO4TRACE((11,"TGo4Analysis::UserEventFunc()",__LINE__, __FILE__));
   return 0;
}

void TGo4Analysis::ProcessEvents()
{
   if (fSniffer) fSniffer->ProcessSnifferEvents();
}

Int_t TGo4Analysis::Process()
{
   GO4TRACE((11,"TGo4Analysis::Process()",__LINE__, __FILE__));
   Int_t rev = 0;

   try
   {
      ProcessEvents();

      if(fxAnalysisSlave)
      {
         gSystem->ProcessEvents(); // ensure cintlocktimer to give mainlock back
         //if(!IsRunning()) TGo4Thread::Sleep(200);
         //TGo4Thread::Sleep(50);   // give  other threads time to operate
      }
      {
         //TGo4LockGuard mainlock; // moved to MainCycle
         if(!IsRunning())
         {
            rev=-1;
         }
         //return -1;
         else
         {
            TDirectory *savdir = gDirectory;
            gROOT->cd(); // necessary for dynamic list scope
            MainCycle();
            savdir->cd();
         }
      }

   }
   /////////////////////////////////////////////////////////////////////////////////
   // begin catch block
   catch(TGo4EventTimeoutException& ex)
   {
      {
         TGo4LockGuard global;
         ex.Handle(); // display exception on terminal in case of debug
      }
      if(TGo4Log::GetIgnoreLevel()<1)
      {
         // only display message if debug output enabled
         Message(2,"Analysis %s TIMEOUT for eventsource %s:%s.",
               GetName(), ex.GetSourceClass(), ex.GetSourceName());
      }
      //return 0;
   }
   catch(TGo4EventEndException& ex)
   {
      Message(2,"End of event source %s:  name:%s - msg:%s",
                 ex.GetSourceClass(), ex.GetSourceName(), ex.GetErrMess());
      if(IsErrorStopEnabled()) {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop();
         //return -1;
         rev=-1;
      }
      //return 0;
   }
   catch(TGo4EventErrorException& ex)
   {
      //ex.Handle();
      Int_t prio=ex.GetPriority();
      if(prio == 0)
      {
         // SL:12.2011 even erre display can be skipped, continue without stop
         // Message(1,"Event source %s: %s - %s",
         //      ex.GetSourceClass(), ex.GetSourceName(),ex.GetErrMess());
      }
      else
      {
         Message(3,"Analysis %s ERROR: %s from event source %s:\n     %s",
               GetName(),ex.GetErrMess(),
               ex.GetSourceClass(), ex.GetSourceName());
         if(IsErrorStopEnabled())
         {
            if(fxAnalysisSlave) fxAnalysisSlave->Stop();
            //return -1;
            rev=-1;
         }
      }
      //return 0;
   }
   catch(TGo4EventSourceException& ex)
       {
          Message(3,"Event source throws exception: %s",ex.GetErrMess());
          if(IsErrorStopEnabled()) {
             if(fxAnalysisSlave) fxAnalysisSlave->Stop();
             rev=-1;
          }
          //return 0;
       }

   catch(TGo4EventStoreException& ex)
     {
        Message(3,"Event store throws exception: %s",ex.GetErrMess());
        if(IsErrorStopEnabled()) {
           if(fxAnalysisSlave) fxAnalysisSlave->Stop();
           rev=-1;
        }
        //return 0;
     }

   catch(TGo4DynamicListException& ex)
   {
      {
         TGo4LockGuard global;
         ex.Handle();
      }
      Message(3,"Analysis %s ERROR: %s from dynamic list entry %s:%s",
            GetName(),ex.GetStatusMessage(),
            ex.GetEntryName(), ex.GetEntryClass());
      if(IsErrorStopEnabled())
      {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop();
         //return -1;
         rev=-1;
      }
      //return 0;
   }

   catch(TGo4AnalysisStepException& ex)
   {
      TGo4LockGuard global;
      ex.Handle();
      Message(3,"Analysis %s ERROR: %s in Analysis Step %s",
            GetName(), ex.GetStatusMessage(), ex.GetStepName());
      if(IsErrorStopEnabled())
      {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop();
         //return -1;
         rev=-1;
      }
      //return 0;
   }

   catch(TGo4UserException& ex)
   {
      // { TGo4LockGuard global; ex.Handle(); }

      if(strlen(ex.GetMessage()) != 0)
          Message(ex.GetPriority(), "%s", ex.GetMessage());
      if(IsErrorStopEnabled() && (ex.GetPriority() > 2)) {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop(); // only stop for errors, warnings and infos continue loop!
         //return -1;
         rev=-1;

      }
      //return 0;
   }

   catch(std::exception& ex) // treat standard library exceptions
   {
      Message(3,"Analysis %s got standard exception %s", GetName(), ex.what());
      if(IsErrorStopEnabled()) {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop();
         //return -1;
         rev=-1;
      }
      //return 0;
   }
   // end catch block
   ////////////////////////////

   return rev;
}

Int_t TGo4Analysis::RunImplicitLoop(Int_t times, Bool_t showrate, Double_t process_event_interval, Bool_t iswebserver)
{
   GO4TRACE((11,"TGo4Analysis::RunImplicitLoop(Int_t)",__LINE__, __FILE__));
   Int_t cnt = 0; // number of actually processed events
   Int_t nextcnt = 0; // number of actually processed events
   if (process_event_interval>1.) process_event_interval = 1.;

   if (times < 0) times = fBatchLoopCount;

   fxRate = new TGo4Ratemeter();
   Bool_t userate = showrate || (process_event_interval > 0.);
   Bool_t process_events = kFALSE;
   if (userate)
      fxRate->SetUpdateInterval(process_event_interval > 0. ? process_event_interval : 1.);
   if (showrate) fxRate->Reset();

   TTimeStamp last_update;

   try
   {
      if (times > 0)
         Message(1, "Analysis loop for %d cycles is starting...", times);
      else
         Message(1, "Analysis loop is starting...");

      while (fxDoWorkingFlag != flagStop) {

         if ((times > 0) && (cnt >= times)) {
            if (!iswebserver)
               break;
            if (fxDoWorkingFlag == flagRunning) {
               PostLoop();
               fxDoWorkingFlag = flagClosed;
            }
         }

         if (userate && fxRate->Update(nextcnt)) {

            process_events = kTRUE;

            bool need_update = false;
            TTimeStamp now;
            if ((now.AsDouble() - last_update.AsDouble()) >= 1.) {
               last_update = now; need_update = true;
            }

            if (need_update) {
               fxRate->SetRunning(fxDoWorkingFlag == flagRunning);

               TDatime dt;
               fxRate->SetDateTime(dt.AsSQLString());

               TGo4AnalysisStep *firststep = GetAnalysisStep(nullptr);
               if(firststep)
                  fxRate->SetCurrentSource(firststep->GetEventSourceName());
               else
                  fxRate->SetCurrentSource("- No event source -");

               if (showrate)
                  TGo4Log::PrintRate(fxRate->GetCurrentCount(), fxRate->GetRate());

               if (fSniffer)
                  fSniffer->RatemeterUpdate(fxRate);
            }
         }

         nextcnt = 0;

         try
         {
            if (process_events) {
               // put events processing in exception-handling area
               process_events = kFALSE;
               gSystem->ProcessEvents();
               ProcessEvents();
            }

             if (fxDoWorkingFlag == flagRunning) {
               MainCycle();
               cnt++; // account completely executed cycles
               nextcnt = 1; // we process one event
             } else {
               gSystem->Sleep(100);
             }

         }
         catch(TGo4UserException& ex)
         {
            if(ex.GetPriority()>2)
            {
               PostLoop();
               if(iswebserver)
                 {
                   fxDoWorkingFlag = flagPause; // errors: stop event loop
                   ex.Handle();
                 }
               else
                 {
                   throw; // return to shell if not remotely controlled
                 }
            }
            else
            {
               ex.Handle(); // warnings and infos: continue loop after display message
            }
         }
         catch(TGo4EventEndException& ex)
         {
            Message(1,"End of event source %s: name:%s msg:%s",ex.GetSourceClass(), ex.GetSourceName(),ex.GetErrMess());
            PostLoop();
            if(iswebserver)
            {
                fxDoWorkingFlag = flagPause; // errors: stop event loop
                ex.Handle();
            }
            else
              {
                throw; // return to shell if not remotely controlled
              }
         }

         catch(TGo4EventErrorException& ex)
         {
            if(ex.GetPriority()>0)
            {
               Message(ex.GetPriority(),"%s",ex.GetErrMess());
               PostLoop();
               if(iswebserver)
                 {
                   fxDoWorkingFlag = flagPause;// errors: stop event loop
                   ex.Handle();
                 }
               else
                 {
                   throw; // return to shell if not remotely controlled
                 }
            }
            else
            {
               // SL:12.2011 even error display can be skipped, continue without stop
               // Message(1,"Eventsource %s:%s %s",ex.GetSourceClass(),
               //      ex.GetSourceName(),ex.GetErrMess());
               ex.Handle(); // infos: continue loop after display message
            }
         }

         catch(TGo4EventTimeoutException& ex)
         {
            ex.Handle(); // just show timeout message, continue event loop
         }
         catch (TGo4EventSourceException& ex)
         {
           Message(3, "%s", ex.GetErrMess());
           PostLoop();
           if (iswebserver) {
             fxDoWorkingFlag = flagPause;    // errors: stop event loop
             ex.Handle();
           }
           else
           {
             throw;    // return to shell if not remotely controlled
           }
         }
         catch (TGo4EventStoreException& ex)
          {
            Message(3, "%s", ex.GetErrMess());
            PostLoop();
            if (iswebserver)
            {
              fxDoWorkingFlag = flagPause;    // errors: stop event loop
              ex.Handle();
            }
            else
            {
              throw;    // return to shell if not remotely controlled
            }
          }
         catch(...)
         {
            PostLoop(); // make sure that postloop is executed for all exceptions
            if(iswebserver)
            {
              fxDoWorkingFlag = flagPause;
              Message(1, "!!! Unexpected exception in %d cycle !!!", cnt);
            }
            else
            {
              throw;
            }
         }
         ////// end inner catch
      }// while loop

      Message(1, "Analysis implicit Loop has finished after %d cycles.", cnt);

      // postloop only if analysis not yet closed
      if (fxDoWorkingFlag != flagClosed)
         PostLoop();
   } //  try

   catch(TGo4Exception &ex) {
      Message(1, "%s appeared in %d cycle.", ex.What(), cnt);
      ex.Handle();
   }
   catch(std::exception &ex) { // treat standard library exceptions
      Message(1, "standard exception %s appeared in %d cycle.", ex.what(), cnt);
   }
   catch(...) {
      Message(1, "!!! Unexpected exception in %d cycle !!!", cnt);
   }

   if (showrate) {
      printf("\n"); fflush(stdout);
   }
   delete fxRate;
   fxRate = nullptr;
   /////////// end outer catch block
   return cnt;
}

////////////////////////////////////////////////////////////
// dynamic list stuff:

Bool_t TGo4Analysis::RemoveDynamicEntry(const char *entryname, const char *listname)
{
   GO4TRACE((11,"TGo4Analysis::RemoveDynamicEntry(const char *, const char *)",__LINE__, __FILE__));
   Bool_t rev=fxObjectManager->RemoveDynamicEntry(entryname);
   if(rev) UpdateNamesList();
   return rev;
}

//////////////////////////////////////////////////////////////
// status related methods:

void TGo4Analysis::UpdateStatus(TGo4AnalysisStatus *state)
{
   GO4TRACE((11,"TGo4Analysis::UpdateStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   fxStepManager->UpdateStatus(state);
   if(state) {
      state->SetAutoSaveInterval(fiAutoSaveInterval);
      state->SetAutoFileName(fxAutoFileName);
      state->SetAutoSaveCompression(fiAutoSaveCompression);
      state->SetAutoSaveOverwrite(fbAutoSaveOverwrite);
      state->SetAutoSaveOn(fbAutoSaveOn);
      state->SetConfigFileName(fxConfigFilename.Data());
   }
}

void TGo4Analysis::SetStatus(TGo4AnalysisStatus * state)
{
   GO4TRACE((11,"TGo4Analysis::SetStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state) {
      // first we close down existing  analysis:
      CloseAnalysis();
      SetAutoSaveInterval(state->GetAutoSaveInterval());
      SetAutoSave(state->IsAutoSaveOn());
      SetAutoSaveFile(state->GetAutoFileName(),
                      state->IsAutoSaveOverwrite(),
                      state->GetAutoSaveCompression());
      fxStepManager->SetStatus(state);
   }
}


Bool_t TGo4Analysis::LoadStatus(const char *filename)
{
   GO4TRACE((11,"TGo4Analysis::LoadStatus(const char *)",__LINE__, __FILE__));
   //
   Bool_t rev = kFALSE;
   TString fname = filename ? filename : fgcDEFAULTSTATUSFILENAME;

   // file suffix if not given by user
   if(!fname.Contains(fgcDEFAULTFILESUF)) fname.Append(fgcDEFAULTFILESUF);

   TFile *statusfile = TFile::Open(fname.Data(), "READ");
   if(statusfile && statusfile->IsOpen()) {
      TGo4AnalysisStatus *state=
         dynamic_cast<TGo4AnalysisStatus*>(statusfile->Get(GetName()));
      if (!state) {
         TIter iter(statusfile->GetListOfKeys());
         TKey *key = nullptr;
         while ((key = (TKey *)iter()) != nullptr) {
            if (strcmp(key->GetClassName(),"TGo4AnalysisStatus") == 0) break;
         }

         if (key) state = dynamic_cast<TGo4AnalysisStatus *>(statusfile->Get(key->GetName()));
      }

      // name of status object is name of analysis itself
      if(state) {
         Message(1,"Analysis %s: Found status object %s in file %s",
               GetName(), state->GetName(), fname.Data());
         SetStatus(state);
         fxConfigFilename = fname; // remember last configuration file name
         Message(0,"Analysis: New analysis state is set.");
         rev = kTRUE;
      } else {
         Message(3,"Analysis LoadStatus: Could not find status %s in file %s", GetName(), fname.Data());
         rev = kFALSE;
      }   // if(state)
   } else {
      Message(3,"Analysis LoadStatus: Failed to open file %s", fname.Data());
      rev = kFALSE;
   }  //  if(statusfile && statusfile->IsOpen())

   delete statusfile;

   return rev;
}

Bool_t TGo4Analysis::SaveStatus(const char *filename)
{
   GO4TRACE((11,"TGo4Analysis::SaveStatus(const char *)",__LINE__, __FILE__));
   Bool_t rev = kFALSE;
   char buffer[1024];
   memset(buffer, 0, sizeof(buffer));
   if(filename)
      strncpy(buffer, filename, sizeof(buffer)-10);
   else
      strncpy(buffer,fgcDEFAULTSTATUSFILENAME, sizeof(buffer)-10);
   if(!strstr(buffer, fgcDEFAULTFILESUF))
      strncat(buffer, fgcDEFAULTFILESUF, sizeof(buffer)-10); // file suffix if not given by user
   auto statusfile = TFile::Open(buffer,"RECREATE");
   if(statusfile && statusfile->IsOpen()) {
      fxConfigFilename=buffer; // remember name of status
      statusfile->cd();
      TGo4AnalysisStatus *state = CreateStatus();
      if(state) {
         state->Write();
         delete state;
         delete statusfile;
         rev = kTRUE;
         Message(-1,"Analysis SaveStatus: Saved Analysis settings to file %s", buffer);
      } else {
         Message(3,"Analysis SaveStatus: FAILED to create status object !!!");
         rev = kFALSE;
      }
   } else {
      Message(3,"Analysis SaveStatus: Failed to open file %s ",
            buffer);
      rev = kFALSE;
   }  // if(statusfile && statusfile->IsOpen())
   return rev;
}

TGo4AnalysisStatus *TGo4Analysis::CreateStatus()
{
   GO4TRACE((11,"TGo4Analysis::CreateStatus()",__LINE__, __FILE__));
   TDirectory *filsav = gDirectory;
   gROOT->cd();
   TGo4AnalysisStatus *state= new TGo4AnalysisStatus(GetName());
   UpdateStatus(state);
   filsav->cd();
   return state;
}

TGo4AnalysisWebStatus *TGo4Analysis::CreateWebStatus()
{
   GO4TRACE((11,"TGo4Analysis::CreateWebStatus()",__LINE__, __FILE__));
   TDirectory *filsav = gDirectory;
   gROOT->cd();
   TGo4AnalysisWebStatus *state= new TGo4AnalysisWebStatus(GetName());
   UpdateStatus(state);
   filsav->cd();
   return state;
}

void TGo4Analysis::Print(Option_t *) const
{
   TGo4Analysis *localthis = const_cast<TGo4Analysis*>(this);
   TGo4AnalysisStatus *state = localthis->CreateStatus();
   state->Print();
   delete state;
}

TTree *TGo4Analysis::CreateSingleEventTree(TGo4EventElement *event)
{
   GO4TRACE((11,"TGo4Analysis::CreateSingleEventTree(TGo4EventElement *)",__LINE__, __FILE__));

   return event ? event->CreateSampleTree(&fxSampleEvent) : nullptr;
}

TTree *TGo4Analysis::CreateSingleEventTree(const char *name, Bool_t isoutput)
{
   GO4TRACE((11,"TGo4Analysis::CreateSingleEventTree(const char *, Bool_t)",__LINE__, __FILE__));
   //
   TGo4EventElement *event = nullptr;
   if(isoutput) event = GetOutputEvent(name);
           else event = GetInputEvent(name);
   if(!event) event = GetEventStructure(name);

   return CreateSingleEventTree(event);
}

void TGo4Analysis::CloseAnalysis()
{
   GO4TRACE((14,"TGo4Analysis::CloseAnalysis()",__LINE__, __FILE__));
   //
   if(fbInitIsDone) {
      AutoSave();
      fxStepManager->CloseAnalysis();
      fxObjectManager->CloseAnalysis();
      fbInitIsDone = kFALSE;
      if (!fxAnalysisSlave && (fxDoWorkingFlag != flagStop)) fxDoWorkingFlag = flagClosed;
   }
}

Int_t TGo4Analysis::PreLoop()
{
   GO4TRACE((11,"TGo4Analysis:PreLoop()",__LINE__, __FILE__));
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   // avoid conflict with possible user object modifications during startup autosave!
   fiAutoSaveCount = 0;
   Int_t rev = UserPreLoop();
   for (Int_t num = 0; num < fxStepManager->GetNumberOfAnalysisSteps(); num++) {
      TGo4AnalysisStep *step = fxStepManager->GetAnalysisStepNum(num);
      TGo4EventProcessor *proc = step ? step->GetEventProcessor() : nullptr;
      if (proc) proc->UserPreLoop();
   }

   fxAutoSaveClock->Start(kTRUE);
   return rev;
}

Int_t TGo4Analysis::PostLoop()
{
   GO4TRACE((11,"TGo4Analysis::PostLoop()",__LINE__, __FILE__));
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   Int_t rev = 0;
   ////////////////////////Test of single event tree
   //   TTree *mytree = CreateSingleEventTree("Unpack");
   //   auto myfile = TFile::Open("eventsample.root","RECREATE");
   //   mytree->SetDirectory(myfile);
   //   mytree->Write();
   //   delete myfile;
   //   std::cout <<"___________Wrote eventsample to file eventsample.root" << std::endl;
   /////////////////////
   if(fbInitIsDone) {
      for (Int_t num = 0; num < fxStepManager->GetNumberOfAnalysisSteps(); num++) {
         TGo4AnalysisStep *step = fxStepManager->GetAnalysisStepNum(num);
         TGo4EventProcessor *proc = step ? step->GetEventProcessor() : nullptr;
         if (proc) proc->UserPostLoop();
      }

      rev = UserPostLoop(); // do not call userpostloop after error in initialization
   }
   return rev;
}

Int_t TGo4Analysis::UserPreLoop()
{
   GO4TRACE((11,"TGo4Analysis::UserPreLoop()",__LINE__, __FILE__));
   //
   Message(0,"Analysis BaseClass --  executing default User Preloop");
   return 0;
}

Int_t TGo4Analysis::UserPostLoop()
{
   GO4TRACE((11,"TGo4Analysis::UserPostLoop()",__LINE__, __FILE__));
   //
   Message(0,"Analysis BaseClass --  executing default User Postloop");
   return 0;
}

void TGo4Analysis::SetAutoSaveFile(const char *filename, Bool_t overwrite, Int_t compression)
{
//   if(!fbAutoSaveOn) return; // do not set autosave file if disabled!
   //TGo4LockGuard  autoguard(fxAutoSaveMutex);
   TString buffer;
   if(filename) buffer = filename;
           else buffer = fgcDEFAULTFILENAME;
   if(!buffer.Contains(fgcDEFAULTFILESUF))
      buffer.Append(fgcDEFAULTFILESUF); // file suffix if not given by user
   // new: just set parameters, do not open file here:
   fxAutoFileName = buffer;
   fbAutoSaveOverwrite = overwrite;
   fiAutoSaveCompression = compression;
}

Bool_t TGo4Analysis::IsAutoSaveFileName() const
{
   return fxAutoFileName.Length() > 0;
}

Int_t TGo4Analysis::LockAutoSave()
{
   GO4TRACE((12,"TGo4Analysis::LockAutoSave()",__LINE__, __FILE__));
   Int_t rev = -1;
   if (TThread::Exists()>0 && fxAutoSaveMutex)
      rev = fxAutoSaveMutex->Lock();
   return rev;
}

Int_t TGo4Analysis::UnLockAutoSave()
{
   GO4TRACE((12,"TGo4Analysis::UnLockAutoSave()",__LINE__, __FILE__));
   Int_t rev = -1;
   if (TThread::Exists()>0 && fxAutoSaveMutex)
      rev = fxAutoSaveMutex->UnLock();

   return rev;
}

void TGo4Analysis::AutoSave()
{
   GO4TRACE((12,"TGo4Analysis::AutoSave()",__LINE__, __FILE__));

   if(!fbAutoSaveOn) return;
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   Message(0,"Analysis  --  AutoSaving....");
   //UpdateNamesList();
   fxStepManager->AutoSave();
   OpenAutoSaveFile(true);
   fxObjectManager->SaveObjects(fxAutoFile);
   CloseAutoSaveFile();
   Message(0,"Analysis  --  AutoSave done.");
}

void TGo4Analysis::OpenAutoSaveFile(bool for_writing)
{
   if(!fbAutoSaveOn) return;
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   gROOT->cd();
   if (for_writing) {
      delete fxAutoFile;
      fxAutoFile = TFile::Open(fxAutoFileName.Data() ,"RECREATE");
      if (!fxAutoFile)
         Message(3,"Fail to open AutoSave file %s", fxAutoFileName.Data());
      else
         Message(-1,"Opening AutoSave file %s with RECREATE mode",fxAutoFileName.Data());
      if (fxAutoFile) fxAutoFile->SetCompressionLevel(fiAutoSaveCompression);
   } else {
      if(!fxAutoFile) {
         if (!gSystem->AccessPathName(fxAutoFileName.Data()))
            fxAutoFile = TFile::Open(fxAutoFileName.Data(), "READ");
         if (!fxAutoFile)
            Message(3,"Fail to open AutoSave file %s", fxAutoFileName.Data());
         else
            Message(-1,"Opening AutoSave file %s with READ mode",fxAutoFileName.Data());

      } else {
         Message(-1,"Reusing AutoSave file %s with READ mode",fxAutoFileName.Data());
      }
   } // if(fbAutoSaveOverwrite)

   gROOT->cd();
}


void TGo4Analysis::CloseAutoSaveFile()
{
   if(fxAutoFile) {
      TGo4LockGuard  autoguard(fxAutoSaveMutex);
      delete fxAutoFile;
      fxAutoFile = nullptr;
      Message(-1,"AutoSave file %s was closed.",fxAutoFileName.Data());
   }
}


void TGo4Analysis::UpdateNamesList()
{
   GO4TRACE((11,"TGo4Analysis::UpdateNamesList()",__LINE__, __FILE__));
   // first try: update all
   delete fxObjectNames;
   fxObjectNames = CreateNamesList();
   Message(0,"Analysis BaseClass --  Nameslist updated.");
}


Bool_t TGo4Analysis::LoadObjects(const char *filename)
{
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   if(filename) fxAutoFileName = filename;

   if(!fbAutoSaveOn) {
      TGo4Log::Info("Analysis LoadObjects: AUTOSAVE file is DISABLED, did NOT read objects back from file %s", fxAutoFileName.Data());
      return kFALSE;
   }

   if (fbAutoSaveOverwrite) {
      TGo4Log::Info("Analysis LoadObjects: AUTOSAVE is in overwrite mode - no objects will be loaded from %s", fxAutoFileName.Data());
      return kTRUE;
   }

   Bool_t rev = kTRUE;
   OpenAutoSaveFile(false);
   if(fxAutoFile && fxAutoFile->IsOpen()) {
      TGo4Log::Info("Analysis LoadObjects: Loading from autosave file %s ", fxAutoFile->GetName());
      rev = fxObjectManager->LoadObjects(fxAutoFile);
   } else {
      TGo4Log::Info("Analysis LoadObjects: Failed to load from file %s", fxAutoFileName.Data());
      rev = kFALSE;
   }
   CloseAutoSaveFile();
   gROOT->cd();
   return rev;
}

void TGo4Analysis::Message(Int_t prio, const char *text,...)
{
   char txtbuf[TGo4Log::fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, TGo4Log::fguMESLEN, text, args);
   va_end(args);
   SendMessageToGUI(prio, kTRUE, txtbuf);
}

void TGo4Analysis::SendMessageToGUI(Int_t level, Bool_t printout, const char *text)
{
   if(fxAnalysisSlave) {
      // gui mode: send Text via status channel - also sniffer will be informed
      fxAnalysisSlave->SendStatusMessage(level, printout, text);
   } else {
      // batch mode: no gui connection, handle local printout
      Bool_t previousmode = TGo4Log::IsOutputEnabled();
      TGo4Log::OutputEnable(printout); // override the messaging state
      TGo4Log::Message(level, "%s", text);
      TGo4Log::OutputEnable(previousmode);

      if (fSniffer) fSniffer->StatusMessage(level, printout, text);
   } // if (fxAnalysisSlave)

}

void TGo4Analysis::SendObjectToGUI(TObject *ob)
{
   if (!ob) return;

   if(!fxAnalysisSlave) {
      Message(2,"Could not send object %s to GUI in batch mode.", ob->GetName());
      return;
   }

   TString pathname;

   if (ObjectManager()->FindObjectPathName(ob, pathname)) {
      auto envelope = new TGo4ObjEnvelope(ob, ob->GetName(), pathname.Data());
      fxAnalysisSlave->SendObject(envelope);
      delete envelope;
      return;
   }

   fxAnalysisSlave->SendObject(dynamic_cast<TNamed*>(ob));
}

Bool_t TGo4Analysis::IsRunning() const
{
   if(fxAnalysisSlave)
      return fxAnalysisSlave->MainIsRunning();

   return TGo4Analysis::fbExists; // should be kTRUE
}

void TGo4Analysis::SetRunning(Bool_t on)
{
   if(!fxAnalysisSlave) return;
   if(on)
      fxAnalysisSlave->Start();
   else
      fxAnalysisSlave->Stop();
}

Int_t TGo4Analysis::WaitForStart()
{
   Int_t cycles = 0;
   while(!IsRunning()) {
      gSystem->Sleep(fgiMACROSTARTPOLL);
      cycles++;
      Bool_t sysret = gSystem->ProcessEvents();
      if (sysret || (fxDoWorkingFlag == flagStop)) {
         cycles=-1;
         break;
         // allows cint canvas menu "Interrupt" to get us out of here!
         // additionally, without ProcessEvents no connect/disconnect of Go4
         // would be possible from this wait loop
      }
   }
   return cycles;
}

void TGo4Analysis::StartObjectServer(const char *basename,  const char *passwd)
{
   if(fxAnalysisSlave) {
      Message(1,"Start object server not yet enabled.");
      //fxAnalysisSlave->StartObjectServer(basename,passwd);
   } else {
      Message(2,"Could not start object server in batch mode.");
   }
}

void TGo4Analysis::StopObjectServer()
{
   if(fxAnalysisSlave) {
      //fxAnalysisSlave->StopObjectServer();
      Message(1,"Stop object server not yet enabled.");
   } else {
      Message(2,"Could not stop object server in batch mode.");
   }
}

void TGo4Analysis::ShutdownServer()
{
   // this method to be called from ctrl-c signal handler of analysis server
   if(fxAnalysisSlave) {
      TGo4Log::Message(1,"Analysis server is initiating shutdown after ctrl-c, please wait!!\n");
      fxAnalysisSlave->SubmitShutdown(); // shutdown will be performed in local command thread
   }

}

/////////////////////////////////////////////////////////////////////////////////
// Show event in specified step

void TGo4Analysis::ShowEvent(const char *stepname, Bool_t isoutput)
{
   TTree *sevtree = CreateSingleEventTree(stepname,isoutput);
   if(sevtree) {
      if(isoutput)
         Message(1, "Showing Output Event %s of step %s", sevtree->GetName(), stepname);
      else
         Message(1, "Showing Input  Event %s of step %s", sevtree->GetName(), stepname);
      sevtree->Show(0);
      std::cout << std::endl;
      delete sevtree;
   }
}

/////////////////////////////////////////////////////////////////////////////////
// Methods that forward to object manager:

Bool_t TGo4Analysis::AddDynamicHistogram(const char *name,
                                         const char *histo,
                                         const char *hevx, const char *hmemx,
                                         const char *hevy, const char *hmemy,
                                         const char *hevz, const char *hmemz,
                                         const char *condition,
                                         const char *cevx, const char *cmemx,
                                         const char *cevy, const char *cmemy)
{
   return fxObjectManager->AddDynamicHistogram(name,
                                               histo, hevx, hmemx, hevy, hmemy, hevz, hmemz,
                                               condition, cevx, cmemx, cevy, cmemy);
}

Bool_t TGo4Analysis::AddTreeHistogram(const char *hisname, const char *treename, const char *varexp, const char *cutexp)
{
   Bool_t rev=fxObjectManager->AddTreeHistogram(hisname,treename,varexp,cutexp);
   if(rev) UpdateNamesList();
   return rev;
}

Bool_t TGo4Analysis::AddEventProcessor(TGo4EventProcessor *pro)
{
   return fxObjectManager->AddEventProcessor(pro);
}

Bool_t TGo4Analysis::AddEventSource(TGo4EventSource *source)
{
   return fxObjectManager->AddEventSource(source);
}

Bool_t TGo4Analysis::AddEventStore(TGo4EventStore *store)
{
   return fxObjectManager->AddEventStore(store);
}

Bool_t TGo4Analysis::AddEventStructure(TGo4EventElement *ev)
{
   return fxObjectManager->AddEventStructure(ev);
}

Bool_t TGo4Analysis::AddHistogram(TH1 *his, const char *subfolder, Bool_t replace)
{
   return fxObjectManager->AddHistogram(his, subfolder, replace);
}

Bool_t TGo4Analysis::AddObject(TNamed *anything, const char *subfolder, Bool_t replace)
{
   return fxObjectManager->AddObject(anything, subfolder, replace);
}

Bool_t TGo4Analysis::AddParameter(TGo4Parameter *par, const char *subfolder)
{
   return fxObjectManager->AddParameter(par,subfolder);
}

Bool_t TGo4Analysis::AddPicture(TGo4Picture *pic, const char *subfolder)
{
   return fxObjectManager->AddPicture(pic,subfolder);
}

Bool_t TGo4Analysis::AddCanvas(TCanvas *can, const char *subfolder)
{
   return fxObjectManager->AddCanvas(can,subfolder);
}

Bool_t TGo4Analysis::AddTree(TTree *tree, const char *subfolder)
{
   if(tree) tree->ResetBit(TGo4Status::kGo4BackStoreReset);
   return fxObjectManager->AddTree(tree, subfolder);
}

Bool_t TGo4Analysis::AddAnalysisCondition(TGo4Condition *con, const char *subfolder)
{
   return fxObjectManager->AddAnalysisCondition(con,subfolder);
}

TGo4Condition *TGo4Analysis::GetAnalysisCondition(const char *name, const char *cond_cl)
{
   return fxObjectManager->GetAnalysisCondition(name, cond_cl);
}

TGo4AnalysisStep *TGo4Analysis::GetAnalysisStep(const char *name)
{
   return fxStepManager->GetAnalysisStep(name);
}

TGo4AnalysisStep *TGo4Analysis::GetAnalysisStepNum(Int_t number)
{
   return fxStepManager->GetAnalysisStepNum(number);
}

Int_t TGo4Analysis::GetDynListInterval() const
{
   return fxObjectManager->GetDynListInterval();
}

TGo4EventElement *TGo4Analysis::GetEventStructure(const char *name) const
{
   return fxObjectManager->GetEventStructure(name);
}

TH1 *TGo4Analysis::GetHistogram(const char *name)
{
   return fxObjectManager->GetHistogram(name);
}

TNamed *TGo4Analysis::GetObject(const char *name, const char *folder)
{
   return fxObjectManager->GetObject(name,folder);
}

TGo4Parameter *TGo4Analysis::GetParameter(const char *name, const char *par_class)
{
   return fxObjectManager->GetParameter(name, par_class);
}

TGo4Picture *TGo4Analysis::GetPicture(const char *name)
{
   return fxObjectManager->GetPicture(name);
}

TCanvas *TGo4Analysis::GetCanvas(const char *name)
{
   return fxObjectManager->GetCanvas(name);
}

TTree *TGo4Analysis::GetTree(const char *name)
{
   return fxObjectManager->GetTree(name);
}

TGo4AnalysisObjectNames *TGo4Analysis::CreateNamesList()
{
   return fxObjectManager->CreateNamesList();
}

TFolder *TGo4Analysis::GetObjectFolder()
{
   return fxObjectManager->GetObjectFolder();
}

TGo4ObjectStatus *TGo4Analysis::CreateObjectStatus(const char *name, const char *folder)
{
   return fxObjectManager->CreateObjectStatus(name,folder);
}

TGo4TreeStructure *TGo4Analysis::CreateTreeStructure(const char *treename)
{
   return fxObjectManager->CreateTreeStructure(treename);
}

Bool_t TGo4Analysis::RemoveEventSource(TGo4EventSource *source)
{
   return fxObjectManager->RemoveEventSource(source);
}

Bool_t TGo4Analysis::RemoveEventStore(TGo4EventStore *store)
{
   return fxObjectManager->RemoveEventStore(store);
}

Bool_t TGo4Analysis::RemoveEventStructure(TGo4EventElement *ev)
{
   return fxObjectManager->RemoveEventStructure(ev);
}

Bool_t TGo4Analysis::RemoveHistogram(const char *name, Bool_t del)
{
   return fxObjectManager->RemoveHistogram(name,del);
}

Bool_t TGo4Analysis::RemoveObject(const char *name, Bool_t del)
{
   return fxObjectManager->RemoveObject(name,del);
}

Bool_t TGo4Analysis::RemoveParameter(const char *name)
{
   return fxObjectManager->RemoveParameter(name);
}

Bool_t TGo4Analysis::RemovePicture(const char *name)
{
   return fxObjectManager->RemovePicture(name);
}

Bool_t TGo4Analysis::RemoveCanvas(const char *name)
{
   return fxObjectManager->RemoveCanvas(name);
}

Bool_t TGo4Analysis::RemoveTree(TTree *tree, const char *stepname)
{
   return fxObjectManager->RemoveTree(tree, stepname);
}

Bool_t TGo4Analysis::RemoveAnalysisCondition(const char *name)
{
   return fxObjectManager->RemoveAnalysisCondition(name);
}

Bool_t TGo4Analysis::RemoveEventProcessor(TGo4EventProcessor *pro)
{
   return fxObjectManager->RemoveEventProcessor(pro);
}

Bool_t TGo4Analysis::DeleteObjects(const char *name)
{
   return fxObjectManager->DeleteObjects(name);
}

Bool_t TGo4Analysis::ClearObjects(const char *name)
{
   return fxObjectManager->ClearObjects(name);
}

Bool_t TGo4Analysis::ProtectObjects(const char *name, const Option_t *flags)
{
   return fxObjectManager->ProtectObjects(name, flags);
}

Bool_t  TGo4Analysis::ResetBackStores(Bool_t clearflag)
{
   return fxObjectManager->ResetBackStores(clearflag);
}

Bool_t TGo4Analysis::SetAnalysisCondition(const char *name, TGo4Condition *con, Bool_t counter)
{
   return fxObjectManager->SetAnalysisCondition(name, con, counter);
}

Bool_t TGo4Analysis::SetParameter(const char *name, TGo4Parameter *par)
{
   return fxObjectManager->SetParameter(name, par);
}

Bool_t TGo4Analysis::SetParameterStatus(const char *name, TGo4ParameterStatus *par)
{
   return fxObjectManager->SetParameterStatus(name, par);
}

Bool_t TGo4Analysis::SetPicture(const char *name, TGo4Picture *pic)
{
   return fxObjectManager->SetPicture(name, pic);
}

void TGo4Analysis::SetDynListInterval(Int_t val)
{
   fxObjectManager->SetDynListInterval(val);
}

void TGo4Analysis::PrintConditions(const char *expression)
{
   fxObjectManager->PrintConditions(expression);
}

void TGo4Analysis::PrintHistograms(const char *expression)
{
   fxObjectManager->PrintHistograms(expression);
}

void TGo4Analysis::PrintParameters(const char *expression)
{
   fxObjectManager->PrintParameters(expression);
}

void TGo4Analysis::PrintDynamicList()
{
   fxObjectManager->PrintDynamicList();
}

TObject *TGo4Analysis::NextMatchingObject(const char *expr, const char *folder, Bool_t reset)
{
   return fxObjectManager->NextMatchingObject(expr,folder,reset);
}

/////////////////////////////////////////////////////////////////////////////////
// Methods that forward to stepmanager:

TGo4EventElement *TGo4Analysis::GetInputEvent(Int_t stepindex) const
{
   return fxStepManager->GetInputEvent(stepindex);
}

TGo4EventElement *TGo4Analysis::GetInputEvent(const char *stepname) const
{
   return fxStepManager->GetInputEvent(stepname);
}

TGo4EventElement *TGo4Analysis::GetOutputEvent() const
{
   return fxStepManager->GetOutputEvent();
}

TGo4EventElement *TGo4Analysis::GetOutputEvent(Int_t stepindex) const
{
   return fxStepManager->GetOutputEvent(stepindex);
}

TGo4EventElement *TGo4Analysis::GetOutputEvent(const char *stepname) const
{
   return fxStepManager->GetOutputEvent(stepname);
}

Bool_t TGo4Analysis::NewStepProcessor(const char *name, TGo4EventProcessorParameter *par)
{
   return fxStepManager->NewStepProcessor(name,par);
}

Bool_t TGo4Analysis::NewStepSource(const char *name, TGo4EventSourceParameter *par)
{
   return fxStepManager->NewStepSource(name,par);
}

Bool_t TGo4Analysis::NewStepStore(const char *name, TGo4EventStoreParameter *par)
{
   return fxStepManager->NewStepStore(name,par);
}

Bool_t TGo4Analysis::AddDynamicEntry(TGo4DynamicEntry *entry)
{
   return fxObjectManager->AddDynamicEntry(entry);
}

void TGo4Analysis::SetStepChecking(Bool_t on)
{
   fxStepManager->SetStepChecking(on);
}

Bool_t TGo4Analysis::SetFirstStep(const char *name)
{
   return fxStepManager->SetFirstStep(name);
}

Bool_t TGo4Analysis::SetLastStep(const char *name)
{
   return fxStepManager->SetLastStep(name);
}

Bool_t TGo4Analysis::SetStepStorage(const char *name, Bool_t on)
{
   return fxStepManager->SetStepStorage(name,on);
}

Int_t TGo4Analysis::IsErrorStopEnabled() const
{
   return fxStepManager->IsErrorStopEnabled();
}

Bool_t TGo4Analysis::AddAnalysisStep(TGo4AnalysisStep *next)
{
   return fxStepManager->AddAnalysisStep(next);
}

Int_t TGo4Analysis::ProcessAnalysisSteps()
{
   return fxStepManager->ProcessAnalysisSteps();
}

void TGo4Analysis::SetOutputEvent(TGo4EventElement *event)
{
   fxStepManager->SetOutputEvent(event);
}

Int_t TGo4Analysis::StoreParameter(const char *name, TGo4Parameter *par)
{
   return fxStepManager->Store(name, par);
}

Int_t TGo4Analysis::StoreCondition(const char *name, TGo4Condition *con)
{
   return fxStepManager->Store(name, con);
}

Int_t TGo4Analysis::StoreFitter(const char *name, TGo4Fitter *fit)
{
   return fxStepManager->Store(name, fit);
}

Int_t TGo4Analysis::StoreFolder(const char *name, TFolder *folder)
{
   return fxStepManager->Store(name, folder);
}

Int_t TGo4Analysis::StoreFolder(const char *stepname, const char *foldername)
{
   TFolder *myfolder = fxObjectManager->FindSubFolder(GetObjectFolder(), foldername, kFALSE);
   return myfolder ? fxStepManager->Store(stepname, myfolder) : 2;
}

void TGo4Analysis::DefineServerPasswords(const char *admin, const char *controller, const char *observer)
{
   fServerAdminPass = admin ? admin : "";
   fServerCtrlPass = controller ? controller : "";
   fServerObserverPass = observer ? observer : "";
}

void TGo4Analysis::SetObserverPassword(const char *passwd)
{
   fServerObserverPass = passwd ? passwd : "";
}

void TGo4Analysis::SetControllerPassword(const char *passwd)
{
   fServerCtrlPass = passwd ? passwd : "";
}

void TGo4Analysis::SetAdministratorPassword(const char *passwd)
{
   fServerAdminPass = passwd ? passwd : "";
}

Bool_t  TGo4Analysis::EvaluateFolderpath(const char *fullname, TString &objectname, TString &foldername)
{
   if (!fullname || (strlen(fullname) == 0))
      return kFALSE;
   const char *separ = strrchr(fullname, '/');
   if (separ) {
      objectname = separ + 1;
      foldername.Append(fullname, separ - fullname);
   } else
      objectname = fullname;
   return kTRUE;
}

TH1 *TGo4Analysis::MakeTH1(char type, const char *fullname, const char *title,
                           Int_t nbinsx, Double_t xlow, Double_t xup,
                           const char *xtitle, const char *ytitle)
{
   fbObjMade = kFALSE;
   TString foldername, histoname;
   if (!EvaluateFolderpath(fullname, histoname, foldername)) {
      TGo4Log::Error("Histogram name not specified, can be a hard error");
      return nullptr;
   }
//   const char *separ = strrchr(fullname, '/');
//   if (separ) {
//      histoname = separ + 1;
//      foldername.Append(fullname, separ - fullname);
//   } else
//      histoname = fullname;

   int itype = 0;
   const char *sclass = "TH1I";
   switch (type) {
      case 'I': case 'i': itype = 0; sclass = "TH1I"; break;
      case 'F': case 'f': itype = 1; sclass = "TH1F"; break;
      case 'D': case 'd': itype = 2; sclass = "TH1D"; break;
      case 'S': case 's': itype = 3; sclass = "TH1S"; break;
      case 'C': case 'c': itype = 4; sclass = "TH1C"; break;
      case 'L': case 'l': itype = 5; sclass = "TH1L"; break;
      default: TGo4Log::Error("There is no histogram type: %c, use I instead", type); break;
   }

   auto oldh = GetHistogram(fullname);

   if (oldh) {
      if (oldh->InheritsFrom(sclass) && fbMakeWithAutosave) {
         if (title) oldh->SetTitle(title);
         if (xtitle) oldh->GetXaxis()->SetTitle(xtitle);
         if (ytitle) oldh->GetYaxis()->SetTitle(ytitle);
         return oldh;
      }

      if (fbMakeWithAutosave)
         TGo4Log::Info("There is histogram %s with type %s other than specified %s, create new and reuse content",
                                 fullname, oldh->ClassName(), sclass);

      // do not delete histogram immediately
      RemoveHistogram(fullname, kFALSE);

      // prevent ROOT to complain about same name
      oldh->SetName("___");
   }

   TH1 *newh = nullptr;

   switch (itype) {
      case 0: newh = new TH1I(histoname, title, nbinsx, xlow, xup); break;
      case 1: newh = new TH1F(histoname, title, nbinsx, xlow, xup); break;
      case 2: newh = new TH1D(histoname, title, nbinsx, xlow, xup); break;
      case 3: newh = new TH1S(histoname, title, nbinsx, xlow, xup); break;
      case 4: newh = new TH1C(histoname, title, nbinsx, xlow, xup); break;
#if ROOT_VERSION_CODE >  ROOT_VERSION(6,32,0)
      case 5: newh = new TH1L(histoname, title, nbinsx, xlow, xup); break;
#endif
   }

   newh->SetTitle(title);

   if (xtitle) newh->GetXaxis()->SetTitle(xtitle);
   if (ytitle) newh->GetYaxis()->SetTitle(ytitle);

   if (oldh) {
      if ((oldh->GetDimension()==1) && fbMakeWithAutosave) newh->Add(oldh);
      delete oldh;
      oldh = nullptr;
   }

   if (foldername.Length() > 0)
      AddHistogram(newh, foldername.Data());
   else
      AddHistogram(newh);

   fbObjMade = kTRUE;

   return newh;
}

TH2 *TGo4Analysis::MakeTH2(char type, const char *fullname, const char *title,
                           Int_t nbinsx, Double_t xlow, Double_t xup,
                           Int_t nbinsy, Double_t ylow, Double_t yup,
                           const char *xtitle, const char *ytitle, const char *ztitle)
{
   fbObjMade = kFALSE;
   TString foldername, histoname;

   if (!EvaluateFolderpath(fullname, histoname, foldername)) {
      TGo4Log::Error("Histogram name not specified, can be a hard error");
      return nullptr;
   }

//   const char *separ = strrchr(fullname, '/');
//   if (separ) {
//      histoname = separ + 1;
//      foldername.Append(fullname, separ - fullname);
//   } else
//      histoname = fullname;

   int itype = 0;
   const char *sclass = "TH2I";
   switch (type) {
      case 'I': case 'i': itype = 0; sclass = "TH2I"; break;
      case 'F': case 'f': itype = 1; sclass = "TH2F"; break;
      case 'D': case 'd': itype = 2; sclass = "TH2D"; break;
      case 'S': case 's': itype = 3; sclass = "TH2S"; break;
      case 'C': case 'c': itype = 4; sclass = "TH2C"; break;
      case 'L': case 'l': itype = 5; sclass = "TH2L"; break;
      default: TGo4Log::Error("There is no histogram type: %c, use I instead", type); break;
   }

   auto oldh = GetHistogram(fullname);

   if (oldh) {
      if (oldh->InheritsFrom(sclass) && fbMakeWithAutosave) {
         if (title) oldh->SetTitle(title);
         if (xtitle) oldh->GetXaxis()->SetTitle(xtitle);
         if (ytitle) oldh->GetYaxis()->SetTitle(ytitle);
         return (TH2 *) oldh;
      }

      if (fbMakeWithAutosave)
         TGo4Log::Info("There is histogram %s with type %s other than specified %s, create new and reuse content",
                        fullname, oldh->ClassName(), sclass);

      // do not delete histogram immediately
      RemoveHistogram(fullname, kFALSE);

      // prevent ROOT to complain about same name
      oldh->SetName("___");
   }

   TH2 *newh = nullptr;

   switch (itype) {
      case 0: newh = new TH2I(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 1: newh = new TH2F(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 2: newh = new TH2D(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 3: newh = new TH2S(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 4: newh = new TH2C(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
#if ROOT_VERSION_CODE >  ROOT_VERSION(6,32,0)
      case 5: newh = new TH2L(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
#endif
   }

   newh->SetTitle(title);

   if (xtitle) newh->GetXaxis()->SetTitle(xtitle);
   if (ytitle) newh->GetYaxis()->SetTitle(ytitle);
   if (ztitle) newh->GetZaxis()->SetTitle(ztitle);

   if (oldh) {
      if ((oldh->GetDimension()==2) && fbMakeWithAutosave) newh->Add(oldh);
      delete oldh;
      oldh = nullptr;
   }

   if (foldername.Length() > 0)
      AddHistogram(newh, foldername.Data());
   else
      AddHistogram(newh);

   fbObjMade = kTRUE;

   return newh;
}

TGo4WinCond *TGo4Analysis::MakeWinCond(const char *fullname,
                                       Double_t xmin, Double_t xmax,
                                       const char *HistoName)
{
   fbObjMade = kFALSE;
   TString foldername, condname;

   if (!EvaluateFolderpath(fullname, condname, foldername)) {
      TGo4Log::Error("Condition name not specified, can be a hard error");
      return nullptr;
   }
   TGo4Condition *cond = GetAnalysisCondition(fullname);

   if (cond) {
      if (cond->InheritsFrom(TGo4WinCond::Class()) && fbMakeWithAutosave) {
         cond->ResetCounts();
         return (TGo4WinCond*) cond;
      }
      RemoveAnalysisCondition(fullname);
   }

   auto wcond = new TGo4WinCond(condname);
   wcond->SetValues(xmin, xmax);
   wcond->SetHistogram(HistoName);
   wcond->Enable();

   if (foldername.Length() > 0)
      AddAnalysisCondition(wcond, foldername.Data());
   else
      AddAnalysisCondition(wcond);

   fbObjMade = kTRUE;

   return wcond;
}

TGo4WinCond *TGo4Analysis::MakeWinCond(const char *fullname,
                                       Double_t xmin, Double_t xmax,
                                       Double_t ymin, Double_t ymax,
                                       const char *HistoName)
{
   fbObjMade = kFALSE;
   TString foldername, condname;

   if (!EvaluateFolderpath(fullname, condname, foldername)) {
      TGo4Log::Error("Condition name not specified, can be a hard error");
      return nullptr;
   }
   TGo4Condition *cond = GetAnalysisCondition(fullname);

   if (cond) {
      if (cond->InheritsFrom(TGo4WinCond::Class()) && fbMakeWithAutosave) {
         cond->ResetCounts();
         return (TGo4WinCond*) cond;
      }
      RemoveAnalysisCondition(fullname);
   }

   auto wcond = new TGo4WinCond(condname);
   wcond->SetValues(xmin, xmax, ymin, ymax);
   wcond->SetHistogram(HistoName);
   wcond->Enable();

   if (foldername.Length() > 0)
      AddAnalysisCondition(wcond, foldername.Data());
   else
      AddAnalysisCondition(wcond);

   fbObjMade = kTRUE;

   return wcond;
}

TGo4PolyCond *TGo4Analysis::MakePolyCond(const char *fullname,
                                         Int_t npoints,
                                         Double_t (*points) [2],
                                         const char *HistoName,
                                         Bool_t shapedcond)
{
   fbObjMade = kFALSE;
   TString foldername, condname;

   if (!EvaluateFolderpath(fullname, condname, foldername)) {
      TGo4Log::Error("Condition name not specified, can be a hard error");
      return nullptr;
   }

   TGo4Condition *cond = GetAnalysisCondition(fullname);

   if (cond) {
      if (cond->InheritsFrom(TGo4PolyCond::Class()) && fbMakeWithAutosave) {
         cond->ResetCounts();
         return (TGo4PolyCond*) cond;
      }
      RemoveAnalysisCondition(fullname);
   }

   TArrayD fullx(npoints+1), fully(npoints+1);

   for (int i = 0; i < npoints; i++) {
      fullx[i] = points[i][0];
      fully[i] = points[i][1];
   }

   // connect first and last points
   if ((fullx[0]!=fullx[npoints-1]) || (fully[0]!=fully[npoints-1])) {
       fullx[npoints] = fullx[0];
       fully[npoints] = fully[0];
       npoints++;
    }

   TCutG mycat("initialcut", npoints, fullx.GetArray(), fully.GetArray());
   TGo4PolyCond *pcond;
   if(shapedcond)
       pcond = new TGo4ShapedCond(condname);
     else
       pcond = new TGo4PolyCond(condname);
   pcond->SetValues(&mycat);
   pcond->Enable();

   pcond->SetHistogram(HistoName);

   if (foldername.Length() > 0)
      AddAnalysisCondition(pcond, foldername.Data());
   else
      AddAnalysisCondition(pcond);

   fbObjMade = kTRUE;

   return pcond;
}


TGo4ShapedCond *TGo4Analysis::MakeEllipseCond(const char *fullname,
       Int_t npoints,
       Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta,
       const char *HistoName)
{
   fbObjMade = kFALSE;
   TString foldername, condname;

   if (!EvaluateFolderpath(fullname, condname, foldername)) {
      TGo4Log::Error("Condition name not specified, can be a hard error");
      return nullptr;
   }

   TGo4Condition *cond = GetAnalysisCondition(fullname);

   if (cond) {
      if (cond->InheritsFrom(TGo4ShapedCond::Class()) && fbMakeWithAutosave) {
         cond->ResetCounts();
         return (TGo4ShapedCond *)cond;
      }
      RemoveAnalysisCondition(fullname);
   }

   auto econd = new TGo4ShapedCond(condname);
   econd->SetEllipse(cx, cy, a1, a2, theta, npoints);
   econd->Enable();
   econd->SetHistogram(HistoName);
   if (foldername.Length() > 0)
      AddAnalysisCondition(econd, foldername.Data());
   else
      AddAnalysisCondition(econd);

   fbObjMade = kTRUE;

   return econd;
}

TGo4ShapedCond *TGo4Analysis::MakeCircleCond(const char *fullname,
           Int_t npoints, Double_t cx, Double_t cy, Double_t r,
           const char *HistoName)
{
   TGo4ShapedCond *elli = MakeEllipseCond(fullname, npoints, cx, cy, r, r, 0, HistoName);
   elli->SetCircle(); // mark "circle shape" property
   return elli;
}

TGo4ShapedCond *TGo4Analysis::MakeBoxCond(const char *fullname, Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta,
               const char *HistoName)
{
   TGo4ShapedCond *elli = MakeEllipseCond(fullname, 4, cx, cy, a1, a2, theta, HistoName);
   elli->SetBox(); // convert to  "box shape" property
   return elli;
}

TGo4ShapedCond *TGo4Analysis::MakeFreeShapeCond(const char *fullname,
                                   Int_t npoints,
                                   Double_t (*points) [2],
                                   const char *HistoName)
{
   TGo4ShapedCond *elli=dynamic_cast<TGo4ShapedCond*>(MakePolyCond(fullname, npoints, points, HistoName,true));
   elli->SetFreeShape();
   return elli;
}

TGo4ListCond *TGo4Analysis::MakeListCond(const char *fullname, const char *title, const char *HistoName)
{
   fbObjMade = kFALSE;
   TString foldername, condname;

   if (!EvaluateFolderpath(fullname, condname, foldername)) {
      TGo4Log::Error("Condition name not specified, can be a hard error");
      return nullptr;
   }

   TGo4Condition *cond = GetAnalysisCondition(fullname);

   if (cond) {
      if (cond->InheritsFrom(TGo4ListCond::Class()) && fbMakeWithAutosave) {
         cond->ResetCounts();
         return (TGo4ListCond *)cond;
      }
      RemoveAnalysisCondition(fullname);
   }

   auto lcond = new TGo4ListCond(condname.Data(), title);
   lcond->SetHistogram(HistoName);
   lcond->Enable();

   if (foldername.Length() > 0)
      AddAnalysisCondition(lcond, foldername.Data());
   else
      AddAnalysisCondition(lcond);

   fbObjMade = kTRUE;

   return lcond;
}

TGo4ListCond *TGo4Analysis::MakeListCond(const char *fullname, const Int_t num, const Int_t * values,  const char *HistoName)
{
   TGo4ListCond *lcond=MakeListCond(fullname, "Go4 valuelist condition", HistoName);
   if(fbObjMade) lcond->SetValues(num,values);
   return lcond;
}


TGo4ListCond *TGo4Analysis::MakeListCond(const char *fullname, const Int_t start, const Int_t stop, const Int_t step,  const char *HistoName)
{
   TGo4ListCond *lcond=MakeListCond(fullname, "Go4 valuelist condition", HistoName);
   if(fbObjMade) lcond->SetValues(start,stop,step);
   return lcond;
}


TGraph *TGo4Analysis::MakeGraph(const char *fullname, const char *title, Int_t points, Double_t *xvalues, Double_t *yvalues)
{
   fbObjMade = kFALSE;
   TString foldername, graphname;
   if (!EvaluateFolderpath(fullname, graphname, foldername)) {
      TGo4Log::Error("TGraph name not specified, can be a hard error");
      return nullptr;
   }

   auto graph = dynamic_cast<TGraph *>(GetObject(fullname));
   if (graph)
      return graph;
   if (!points)
      graph = new TGraph();
   else
      graph = new TGraph(points, xvalues, yvalues);
   graph->SetName(graphname.Data());
   graph->SetTitle(title);

   if (foldername.Length() > 0)
      AddObject(graph, foldername.Data());
   else
      AddObject(graph);

   fbObjMade = kTRUE;
   return graph;
}

TGraph *TGo4Analysis::MakeGraph(const char *fullname, const char *title, TF1 *function)
{
   fbObjMade = kFALSE;
   TString foldername, graphname;
   if (!EvaluateFolderpath(fullname, graphname, foldername)) {
      TGo4Log::Error("TGraph name not specified, can be a hard error");
      return nullptr;
   }

   TGraph *graph = dynamic_cast<TGraph *>(GetObject(fullname));
   if (graph)
      return graph;
   graph = new TGraph(function);
   graph->SetName(graphname.Data());
   graph->SetTitle(title);
   if (foldername.Length() > 0)
      AddObject(graph, foldername.Data());
   else
      AddObject(graph);
   fbObjMade = kTRUE;
   return graph;
}

TGo4RollingGraph *TGo4Analysis::MakeRollingGraph(const char *fullname, const char *title, Int_t points, Int_t average)
{
   fbObjMade = kFALSE;
   TString foldername, graphname;
   if (!EvaluateFolderpath(fullname, graphname, foldername)) {
      TGo4Log::Error("TGraph name not specified, can be a hard error");
      return nullptr;
   }
   TGo4RollingGraph *graph = dynamic_cast<TGo4RollingGraph *>(GetObject(fullname));
   if (graph)
      return graph;
   graph = new TGo4RollingGraph(points, average);
   graph->SetName(graphname.Data());
   graph->SetTitle(title);
   if (foldername.Length() > 0)
      AddObject(graph, foldername.Data());
   else
      AddObject(graph);
   fbObjMade = kTRUE;
   return graph;
}

TGo4Parameter *TGo4Analysis::MakeParameter(const char *fullname,
                                           const char *classname,
                                           const char *newcmd)
{
   fbObjMade = kFALSE;
   TString foldername, paramname;

   if (!fullname || (strlen(fullname) == 0)) {
      TGo4Log::Error("Parameter name not specified, can be a hard error");
      return nullptr;
   }
   const char *separ = strrchr(fullname, '/');
   if (separ) {
      paramname = separ + 1;
      foldername.Append(fullname, separ - fullname);
   } else
      paramname = fullname;

   TGo4Parameter *param = GetParameter(fullname);

   if (param) {
      if (!param->InheritsFrom(classname)) {
         TGo4Log::Info("There is parameter %s with type %s other than specified %s, rebuild", fullname,
                       param->ClassName(), classname);
         RemoveParameter(fullname);
         param = nullptr;
      }
   }

   if (!param) {
      paramname = TString("\"") + paramname + TString("\"");

      TString cmd;
      if (newcmd && (strstr(newcmd, "new ") == newcmd))
         cmd = TString::Format(newcmd, paramname.Data());
      else
         cmd = TString::Format("new %s(%s)", classname, paramname.Data());

      auto res = gROOT->ProcessLineFast(cmd.Data());

      if (res == 0) {
         TGo4Log::Error("Cannot create parameter of class %s", classname);
         return nullptr;
      }

      param = (TGo4Parameter *)res;

      if (foldername.Length() > 0)
         AddParameter(param, foldername.Data());
      else
         AddParameter(param);

      fbObjMade = kTRUE;
   }

   if (newcmd && (strstr(newcmd, "set_") == newcmd)) {
      // executing optional set macro

      ExecuteScript(newcmd);
   }

   return param;
}

void TGo4Analysis::SetSortedOrder(Bool_t on)
{
   fxObjectManager->SetSortedOrder(on);
}

Bool_t TGo4Analysis::IsSortedOrder() const
{
   return fxObjectManager->IsSortedOrder();
}


Long64_t TGo4Analysis::ExecuteScript(const char *macro_name)
{
   if (!macro_name || (strlen(macro_name) == 0)) return -1;

   // exclude arguments which could be specified with macro name
   TString file_name(macro_name);
   Ssiz_t pos = file_name.First('(');
   if (pos > 0) file_name.Resize(pos);
   bool has_plus = false;
   pos = file_name.First('+');
   if (pos > 0) { file_name.Resize(pos); has_plus = true; }
   while ((file_name.Length() > 0) && (file_name[file_name.Length()-1] == ' '))
      file_name.Resize(file_name.Length()-1);
   while ((file_name.Length() > 0) && (file_name[0] == ' '))
      file_name.Remove(0, 1);

   if (gSystem->AccessPathName(file_name.Data())) {
      TGo4Log::Error("ROOT script %s not found", file_name.Data());
      return -1;
   }

   TGo4Log::Info("Executing ROOT script %s", macro_name);

   TString exec = ".x ";
   exec.Append(macro_name);
   int error = 0;

   std::ifstream t(file_name.Data());
   std::string content = std::string(std::istreambuf_iterator<char>(t), {});
   if (content.empty()) {
      TGo4Log::Error("Fail to load content of %s", file_name.Data());
      return -1;
   }

   TString func_name = file_name;
   pos = func_name.Last('/');
   if (pos != kNPOS) func_name.Remove(0, pos+1);
   pos = func_name.Index(".C");
   if (pos == kNPOS) pos = func_name.Index(".cxx");
   if (pos != kNPOS) func_name.Resize(pos);

   bool fall_back = false;
   if (has_plus) fall_back = true;
   if (func_name.IsNull()) fall_back = true;

   std::string search = func_name.Data(); search += "(";
   auto name_pos = content.find(search);
   if (name_pos == std::string::npos) fall_back = true;

   if (fall_back) {
      auto res = gROOT->ProcessLineSync(exec.Data(), &error); // here faster than ExecuteLine...
      return error ? -1 : res;
   }

   static std::vector<std::string> script_names;
   static std::vector<std::string> script_contents;
   static std::vector<std::string> script_func;
   static int findx = 1;

   int indx = -1;

   for (unsigned n = 0; n < script_names.size(); n++)
      if (script_names[n] == file_name.Data()) {
         indx = n; break;
      }

   if (indx < 0) {
      // register new code with its func name
      script_names.push_back(file_name.Data());
      script_contents.push_back(content);
      script_func.push_back(func_name.Data());
      indx = script_names.size() - 1;

      if (!gInterpreter->LoadText(content.c_str())) {
         TGo4Log::Error("Cannot parse code of %s script", file_name.Data());
         return -1;
      }
   } else if (script_contents[indx] != content) {
      // script was modified, has to reload with new function name
      std::string new_name = func_name.Data();
      new_name += std::to_string(findx++);
      script_contents[indx] = content;
      script_func[indx] = new_name;

      content.erase(name_pos, func_name.Length());
      content.insert(name_pos, new_name);

      if (!gInterpreter->LoadText(content.c_str())) {
         TGo4Log::Error("Cannot parse modified code of %s script", file_name.Data());
         return -1;
      }

      func_name = new_name.c_str();

   } else {
      // script is same, just call again same function
      func_name = script_func[indx].c_str();
   }

   TGo4Log::Error("Execute function %s from script %s", func_name.Data(), file_name.Data());

   // just call function
   func_name.Append("()");
   auto res = gROOT->ProcessLineSync(func_name.Data(), &error); // here faster than ExecuteLine...
   if (error) res = -1;

   return res;
}

Long64_t TGo4Analysis::ExecutePython(const char *macro_name, Int_t *errcode)
{
  if (!macro_name || (strlen(macro_name) == 0)) return -1;
  TString comstring=macro_name;
  comstring.Prepend(TGo4Analysis::fgcPYPROMPT); // emulate interactive command line mode here
  return ExecuteLine(comstring.Data(), errcode);
}


Long64_t TGo4Analysis::ExecuteLine(const char *command, Int_t *errcode)
{
  TString comstring;
  if (strchr(command,TGo4Analysis::fgcPYPROMPT) && (strchr(command,TGo4Analysis::fgcPYPROMPT) == strrchr(command,TGo4Analysis::fgcPYPROMPT))) {
    // this one is by Sven Augustin, MPI Heidelberg
    comstring = command;
    comstring = comstring.Strip(TString::kBoth);
    comstring = comstring.Strip(TString::kLeading,TGo4Analysis::fgcPYPROMPT);
    comstring = comstring.Strip(TString::kLeading);
    TGo4Log::Debug("Executing Python script: %s", comstring.Data());
    const TString PY_EXT = ".py";
    int imin = comstring.Index(PY_EXT + " ");
    int imax = comstring.Length();
    if (imin == -1) {
      imin = imax;
    } else {
      imin += PY_EXT.Length();
    }
    int ilen = imax - imin;
    TString scrstring = comstring(0, imin);
    TString argstring = comstring(imin, ilen);

    comstring  = "TPython::Exec(\"import sys; sys.argv = [\'" + scrstring + "\'] + \'" + argstring + "\'.split()\");";
    comstring += "TPython::LoadMacro(\"" + scrstring + "\");";

    if(!fbPythonBound) {
      TString go4sys = TGo4Log::GO4SYS();
      TString pyinit = TGo4Analysis::fgcPYINIT; // JAM todo: put python path and filename into settings
      comstring.Prepend("TPython::LoadMacro(\"" + go4sys + pyinit +"\");");
      comstring.Prepend("TPython::Bind(go4, \"go4\");" );
      fbPythonBound = kTRUE;
    }
  } else {
     comstring = "";
     const char *cursor = command;
     const char *at = nullptr;
     do {
        Ssiz_t len = strlen(cursor);
        at = strstr(cursor, "@");
        if (at) {
           len = (Ssiz_t)(at - cursor);
           comstring.Append(cursor, len);
           comstring.Append("TGo4Analysis::Instance()->");
           cursor = at + 1;
        } else {
           comstring.Append(cursor);
        }
     } while (at);
  }

  TGo4Log::Debug("ExecuteLine: %s", comstring.Data());

//  TInterpreter* theI = gROOT->GetInterpreter();
//  printf ("ExecuteLine sees interpreter of class %s \n",theI ? theI->IsA()->GetName() : "NULL");
//  printf ("IsProcessLineLocked is %s \n", theI->IsProcessLineLocked() ? "true" : "false");
//  printf ("gInterpreterMutex: 0x%x gGlobalMutex: 0x%x \n", gInterpreterMutex, gGlobalMutex);


//  theI->SetProcessLineLock(kFALSE);
  return gROOT->ProcessLineSync(comstring, errcode);
}

void TGo4Analysis::StopAnalysis()
{
   if (fxAnalysisSlave) {
      // fxAnalysisSlave->GetTask()->SubmitCommand("THStop");
      fxAnalysisSlave->Stop();
   } else {
      if (fbInitIsDone && (fxDoWorkingFlag != flagPause)) PostLoop();
      fxDoWorkingFlag = flagPause;
   }
}

void TGo4Analysis::StartAnalysis()
{
  if (fxAnalysisSlave)
  {
    fxAnalysisSlave->Start();
    // fxAnalysisSlave->GetTask()->SubmitCommand("THStart");
  }
  else
  {
    // JAM reproduce behavior of analysis client, check init state before executing preloop
    if (fbInitIsDone)
    {
      if (fxDoWorkingFlag != flagRunning)
      {
        PreLoop();
        if(fxRate) fxRate->Reset(); // reset counters and time whenever started again
      }
      fxDoWorkingFlag = flagRunning;
    }
    else
    {
      Message(2, "Analysis %s was not initialized! Please SUBMIT settings first.", GetName());
    }

  }
}


#ifdef _MSC_VER

#include "windows.h"

namespace {

   BOOL WINAPI Go4ConsoleSigHandler(DWORD sig)
   {
      // WinNT signal handler.

      switch (sig) {
         case CTRL_C_EVENT:
           if (TGo4Analysis::Exists())
               TGo4Analysis::Instance()->ProcessCrtlCSignal();
            return TRUE;
         case CTRL_BREAK_EVENT:
         case CTRL_LOGOFF_EVENT:
         case CTRL_SHUTDOWN_EVENT:
         case CTRL_CLOSE_EVENT:
         default:
            printf(" non CTRL-C signal - ignore\n");
            return TRUE;
      }
   }

   void InstallGo4CtrlCHandler(bool on)
   {
      SetConsoleCtrlHandler(Go4ConsoleSigHandler, on);
   }

}

#endif
