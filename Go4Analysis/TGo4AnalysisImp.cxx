#include "TGo4AnalysisImp.h"

#include <stdexcept>
#include "stdio.h"
#include "Riostream.h"

#include "TSystem.h"
#include "TInterpreter.h"
#include "TNamed.h"
#include "TApplication.h"
#include "TH1.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TFile.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Thread.h"
#include "TGo4CommandInvoker.h"
#include "TGo4AnalysisCommandList.h"
#include "TGo4UserException.h"
#include "TGo4TaskHandler.h"

#include "TGo4Version.h"
#include "TGo4AnalysisStepManager.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisStep.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4HistogramStatus.h"
#include "TGo4DynamicListException.h"
#include "TGo4Condition.h"
#include "TGo4Parameter.h"
#include "TGo4Picture.h"
#include "TGo4Fitter.h"
#include "TGo4ObjectStatus.h"
#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4EventProcessorParameter.h"
#include "TGo4EventElement.h"
#include "TGo4EventStore.h"
#include "TGo4EventSource.h"
#include "TGo4EventProcessor.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4EventEndException.h"
#include "TGo4CompositeEvent.h"
#include "TGo4AnalysisStepException.h"
#include "TGo4TreeStructure.h"

#if ROOT_VERSION_CODE > ROOT_VERSION(5,2,0)
#include "TCint.h"
#endif
const Int_t TGo4Analysis::fgiGO4VERSION = __GO4BUILDVERSION__;

TGo4Analysis* TGo4Analysis::fxInstance = 0;
Bool_t TGo4Analysis::fbExists=kFALSE;
const Int_t TGo4Analysis::fgiAUTOSAVECOUNTS= 500;
const Int_t TGo4Analysis::fgiDYNLISTINTERVAL= 1000;
const Int_t TGo4Analysis::fgiMACROSTARTPOLL= 1000; //polling time for macro in WaitForStart
const Text_t TGo4Analysis::fgcDEFAULTFILENAME[]="Go4AutoSave.root";
const Text_t TGo4Analysis::fgcDEFAULTSTATUSFILENAME[]="Go4AnalysisPrefs.root";
const Text_t TGo4Analysis::fgcDEFAULTFILESUF[]=".root";
const Text_t TGo4Analysis::fgcTOPDYNAMICLIST[]="Go4DynamicList";

TGo4Analysis* TGo4Analysis::Instance()
{
TRACE((14,"TGo4Analysis::Instance()",__LINE__, __FILE__));
   if(fxInstance==0)
      {
         fxInstance=new TGo4Analysis;
         fbExists=kTRUE;
      }
   else
      {
         // instance already there
      }
   return fxInstance;
}

Bool_t TGo4Analysis::Exists()
{
   return fbExists;
}


TGo4Analysis::TGo4Analysis() :
   TGo4CommandReceiver(), TObject(),
   fbInitIsDone(kFALSE),fbAutoSaveOn(kTRUE),fxAnalysisSlave(0),
   fxStepManager(0), fxObjectManager(0),
   fiAutoSaveCount(0), fiAutoSaveInterval(TGo4Analysis::fgiAUTOSAVECOUNTS), fiAutoSaveCompression(5),
   fxAutoFile(0), fbAutoSaveOverwrite(kFALSE), fbNewInputFile(kFALSE), fbAutoSaveFileChange(kFALSE), fxSampleEvent(0), fxObjectNames(0)
{
   TRACE((15,"TGo4Analysis::TGo4Analysis()",__LINE__, __FILE__));
   //
   if(! ( TGo4Version::Instance()->CheckVersion(TGo4Analysis::fgiGO4VERSION) ) )
   {
      // wrong version number between framework and user executable
      Message(-1,"!!!! Analysis Base class:\n\t User Analysis was built with wrong \t\tGo4 Buildversion %d !!!!!",
            TGo4Version::Instance()->GetBuildVersion());
      Message(-1,"\t Please rebuild your analysis with current \tGo4 Buildversion %d ",
            fgiGO4VERSION);
      Message(-1,"\t >>make clean all<<");
      Message(-1,"Aborting in 20 s...");
      gSystem->Sleep(20000);
      gApplication->Terminate();
   }
   else
   {
      // may not disable output of version number:
      Message(-1,"Welcome to Go4 Analysis Framework Release %s (build %d) !",
            __GO4RELEASE__ , fgiGO4VERSION);
   }
   if(fxInstance==0)
   {
      gROOT->SetBatch(kTRUE);
      fxStepManager=new TGo4AnalysisStepManager("Go4 Analysis Step Manager");
      fxObjectManager=new TGo4AnalysisObjectManager("Go4 Central Object Manager");
      SetDynListInterval(TGo4Analysis::fgiDYNLISTINTERVAL);
      fxAutoSaveMutex =   new TMutex(kTRUE);
      fxAutoSaveClock=new TStopwatch;
      fxAutoSaveClock->Stop();
      fxAutoFileName=fgcDEFAULTFILENAME;
      fxConfigFilename=fgcDEFAULTSTATUSFILENAME;
      TGo4CommandInvoker::Instance(); // make sure we have an invoker instance!
      TGo4CommandInvoker::SetCommandList(new TGo4AnalysisCommandList);
      TGo4CommandInvoker::Register("Analysis",this); // register as command receiver at the global invoker
      fxInstance=this; // for ctor usage from derived user subclass
      fbExists=kTRUE;
   }
   else
   {
      // instance already there
      Message(2,"Analysis BaseClass ctor -- analysis singleton already exists !!!");
   }
   // settings for macro execution
   gROOT->ProcessLine("TGo4Analysis *go4 = TGo4Analysis::Instance();");
   gROOT->ProcessLine(".x $GO4SYS/Go4Analysis/anamacroinit.C");
}

TGo4Analysis::~TGo4Analysis()
{
   TRACE((15,"TGo4Analysis::~TGo4Analysis()",__LINE__, __FILE__));
   CloseAnalysis();
   //cout <<"after close analysis." << endl;
   CloseAutoSaveFile();
   delete fxStepManager;
   delete fxObjectManager;
   if(fxObjectNames) delete fxObjectNames;
   delete fxAutoSaveClock;
   delete fxSampleEvent;
   TGo4CommandInvoker::UnRegister(this);
   fxInstance=0; // reset static singleton instance pointer
   //gROOT->ProcessLine(".x $GO4SYS/Go4Analysis/anamacroclose.C");
   //cout <<"end of dtor" << endl;
}

////////////////////////////////////////////////////
// Initialization and analysis defining methods:

Bool_t TGo4Analysis::InitEventClasses()
{
   TRACE((14,"TGo4Analysis::InitEventClasses()",__LINE__, __FILE__));
   //
   Bool_t rev=kTRUE;
   if(!fbInitIsDone) {
      Message(0,"Analysis BaseClass --  Initializing EventClasses...");
      LoadObjects(); // always use autosave file to get last objects list
      rev = fxStepManager->InitEventClasses();
      UpdateNamesList();
      Message(-1,"Analysis BaseClass --  Initializing EventClasses done.");
      fbInitIsDone = kTRUE;
   } else
      Message(-1,"Analysis BaseClass --  EventClasses were already initialized.");
   return rev;
}

/////////////////////////////////////////////////////////
// main event cycle methods:

Int_t TGo4Analysis::MainCycle()
{
   TRACE((11,"TGo4Analysis::MainCycle()",__LINE__, __FILE__));
   if(!fbInitIsDone)
      throw TGo4UserException(3,"Analysis not yet initialized");

   {
      TGo4LockGuard mainlock; // protect analysis, but not status buffer

      ProcessAnalysisSteps();

      UserEventFunc();

      fxObjectManager->ProcessDynamicList();

      if (fbAutoSaveOn && (fiAutoSaveInterval!=0)) {
         fiAutoSaveCount++;
         Double_t rt = fxAutoSaveClock->RealTime();
         if (rt > (Double_t) fiAutoSaveInterval) {
            Message(0,"Analysis: Main Cycle Autosaving after %.2f s (%d events).",rt,fiAutoSaveCount);
            AutoSave();
            fiAutoSaveCount = 0;
            fxAutoSaveClock->Start(kTRUE);
         } else
            fxAutoSaveClock->Continue();
      }
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
   TRACE((11,"TGo4Analysis::UserEventFunc()",__LINE__, __FILE__));
   return 0;
}


Int_t TGo4Analysis::Process()
{
TRACE((11,"TGo4Analysis::Process()",__LINE__, __FILE__));
Int_t rev=0;
#if ROOT_VERSION_CODE > ROOT_VERSION(5,2,0)
Bool_t unlockedcint=kFALSE;
if(gCINTMutex)
    {
        gCINTMutex->UnLock();
        unlockedcint=kTRUE;
        //cout <<"Process() Unlocked cint mutex..." << endl;
    }
#endif

try
{
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
       TDirectory* savdir=gDirectory;
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
      } else{}
    //return 0;
}
catch(TGo4EventEndException& ex)
{
Message(2,"End of event source %s:\n     %s - %s",
                                 ex.GetSourceClass(),
                                 ex.GetSourceName(),ex.GetErrMess());
if(IsErrorStopEnabled())
   {
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
   if(prio==0)
      {
         // only display message without stop
         Message(1,"Event source %s:\n     %s - %s",
                                 ex.GetSourceClass(),
                                 ex.GetSourceName(),ex.GetErrMess());
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
   //{TGo4LockGuard global;
   //ex.Handle();}
   Message(ex.GetPriority(), ex.GetMessage() );
   if(IsErrorStopEnabled() && ex.GetPriority()>2)
         {
            if(fxAnalysisSlave) fxAnalysisSlave->Stop(); // only stop for errors, warnings and infos continue loop!
            //return -1;
            rev=-1;

         }
   //return 0;
}

catch(std::exception& ex) // treat standard library exceptions
{
   Message(3,"Analysis %s got standard exception %s",
                           GetName(), ex.what());
   if(IsErrorStopEnabled())
      {
         if(fxAnalysisSlave) fxAnalysisSlave->Stop();
         //return -1;
         rev=-1;
      }
   //return 0;
}
// end catch block
////////////////////////////
#if ROOT_VERSION_CODE > ROOT_VERSION(5,2,0)
/// test: need to unlock cintmutex here to enable streaming!
if(gCINTMutex && unlockedcint)
{
    gCINTMutex->Lock();
    //cout <<"PPPProcess() locked cint mutex..." << endl;
}

#endif
return rev;
}



Int_t TGo4Analysis::RunImplicitLoop(Int_t times)
{
TRACE((11,"TGo4Analysis::RunImplicitLoop(Int_t)",__LINE__, __FILE__));
   Int_t i=0; // number of actually processed events
try
{
   PreLoop();
   Message(-1,"Analysis Implicit Loop for %d cycles is starting...",
            times);
   for(i=0; i<times; ++i)
      {
            try
               {
                  MainCycle();
               }
            catch(TGo4UserException& ex)
               {
                  if(ex.GetPriority()>2)
                     {
                        PostLoop();
                        throw;   // errors: stop event loop
                     }
                  else
                     {
                        ex.Handle(); // warnings and infos: continue loop after display message
                     }
               }
            catch(TGo4EventEndException& ex)
              {
                      Message(1,"End of event source %s:\n      %s %s",ex.GetSourceClass(),
                         ex.GetSourceName(),ex.GetErrMess());
                       PostLoop();
                       throw;   // errors: stop event loop
              }

            catch(TGo4EventErrorException& ex)
              {
                 if(ex.GetPriority()>0)
                    {
                       Message(ex.GetPriority(),"%s",ex.GetErrMess());
                       PostLoop();
                       throw;   // errors: stop event loop
                    }
                 else
                    {
                       Message(1,"Eventsource %s:%s %s",ex.GetSourceClass(),
                                ex.GetSourceName(),ex.GetErrMess());
                       ex.Handle(); // infos: continue loop after display message
                    }
              }

            catch(...)
               {
                  PostLoop(); // make sure that postloop is executed for all exceptions
                  throw;
               }
            ////// end inner catch
      }// for

   Message(-1,"Analysis Implicit Loop has finished after %d cycles.",
            i);
   PostLoop();
} //  try

catch(TGo4Exception& ex)
{
  Message(-1,"%s appeared after %d cycles.",
            ex.What(),i);
  ex.Handle();
}
catch(std::exception& ex) // treat standard library exceptions
{
  Message(-1,"standard exception %s appeared after %d cycles.",
            ex.what(),i);
}
catch(...)
{
  Message(-1,"!!! Unexpected exception after %d cycles !!!",i);
}
/////////// end outer catch block
   return i;
}

////////////////////////////////////////////////////////////
// dynamic list stuff:


Bool_t TGo4Analysis::RemoveDynamicEntry(const Text_t * entryname, const Text_t* listname)
{
   TRACE((11,"TGo4Analysis::RemoveDynamicEntry(const Text_t *, const Text_t* )",__LINE__, __FILE__));
   Bool_t rev=fxObjectManager->RemoveDynamicEntry(entryname);
   if(rev) UpdateNamesList();
   return rev;
}



//////////////////////////////////////////////////////////////
// status related methods:

void TGo4Analysis::UpdateStatus(TGo4AnalysisStatus* state)
{
TRACE((11,"TGo4Analysis::UpdateStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   fxStepManager->UpdateStatus(state);
   if(state!=0)
      {
         state->SetAutoSaveInterval(fiAutoSaveInterval);
         state->SetAutoFileName(fxAutoFileName);
         state->SetAutoSaveCompression(fiAutoSaveCompression);
         state->SetAutoSaveOverwrite(fbAutoSaveOverwrite);
         state->SetAutoSaveOn(fbAutoSaveOn);
         state->SetConfigFileName(fxConfigFilename.Data());
      } // if(state!=0)
   else
      { }
}

void TGo4Analysis::SetStatus(TGo4AnalysisStatus * state)
{
   TRACE((11,"TGo4Analysis::SetStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state!=0)
      {
         // first we close down exisiting  analysis:
         CloseAnalysis();
         SetAutoSaveInterval(state->GetAutoSaveInterval());
         SetAutoSave(state->IsAutoSaveOn());
         SetAutoSaveFile(state->GetAutoFileName(),
                         state->IsAutoSaveOverwrite(),
                         state->GetAutoSaveCompression()
                                                            );
         fxStepManager->SetStatus(state);
      } // if(state!=0)
   else
      {
         // no state to set
      }
}


Bool_t TGo4Analysis::LoadStatus(const char* filename)
{
   TRACE((11,"TGo4Analysis::LoadStatus(const char*)",__LINE__, __FILE__));
   //
   Bool_t rev=kFALSE;
   Int_t buflen=1024;
   Text_t buffer[1024];
   if(filename)
      {
          strncpy(buffer,filename,buflen-100);
      }
   else
      {
         strncpy(buffer,fgcDEFAULTSTATUSFILENAME,buflen-100);
      }

   if(!strstr(buffer,fgcDEFAULTFILESUF))
      {
         strcat(buffer,fgcDEFAULTFILESUF); // file suffix if not given by user
      }
   else {}

   TFile* statusfile = new TFile(buffer,"READ");
   if(statusfile && statusfile->IsOpen())
      {
         TGo4AnalysisStatus* state=
            dynamic_cast<TGo4AnalysisStatus*>( statusfile->Get( GetName() ) );
               // name of status object is name of analysis itself
         if(state)
            {
               Message(1,"Analysis: Found status object in file %s: %s",
                  buffer,GetName());
               SetStatus(state);
               fxConfigFilename=buffer; // remember last configuration file name
               Message(0,"Analysis: New analysis state is set.");
               rev=kTRUE;
            }
         else
            {
               Message(3,"Analysis LoadStatus: Could not find status %s in file %s",
                    GetName(),buffer);
               rev=kFALSE;
            }   // if(state)
         delete statusfile;

      }
   else
      {
         Message(3,"Analysis LoadStatus: Failed to open file %s",
                    buffer);
               rev=kFALSE;
         delete statusfile;
      }  //  if(statusfile && statusfile->IsOpen())

   return rev;
}

Bool_t TGo4Analysis::SaveStatus(const char* filename)
{
   TRACE((11,"TGo4Analysis::SaveStatus(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   Int_t buflen=1024;
   Text_t buffer[1024];
   if(filename)
      {
         strncpy(buffer, filename, buflen-100);
      }
   else
      {
         strncpy(buffer,fgcDEFAULTSTATUSFILENAME,buflen-100);
      }
   if(!strstr(buffer,fgcDEFAULTFILESUF))
      {
         strcat(buffer,fgcDEFAULTFILESUF); // file suffix if not given by user
      }
   else {}
   TFile* statusfile = new TFile(buffer,"RECREATE");
   if(statusfile && statusfile->IsOpen())
      {
         fxConfigFilename=buffer; // remember name of status
         statusfile->cd();
         TGo4AnalysisStatus* state= CreateStatus();
         if(state)
            {
               state->Write();
               delete state;
               delete statusfile;
               rev=kTRUE;
               Message(-1,"Analysis SaveStatus: Saved Analysis settings to file %s",
                           buffer);
            }
         else
            {
               Message(3,"Analysis SaveStatus: FAILED to create status object !!!");
               rev=kFALSE;
            }
      }
   else
      {
         Message(3,"Analysis SaveStatus: Failed to open file %s ",
                    buffer);
               rev=kFALSE;
      }  // if(statusfile && statusfile->IsOpen())
   return rev;
}

TGo4AnalysisStatus* TGo4Analysis::CreateStatus()
{
   TRACE((11,"TGo4Analysis::CreateStatus()",__LINE__, __FILE__));
   TDirectory* filsav=gDirectory;
   gROOT->cd();
   TGo4AnalysisStatus* state= new TGo4AnalysisStatus(GetName());
   UpdateStatus(state);
   filsav->cd();
   return state;
}

void TGo4Analysis::Print(Option_t*) const
{
   TGo4Analysis* localthis=const_cast<TGo4Analysis*>(this);
   TGo4AnalysisStatus* state=localthis->CreateStatus();
   state->PrintStatus();
   delete state;
}

TTree* TGo4Analysis::CreateSingleEventTree(TGo4EventElement* event)
{
TRACE((11,"TGo4Analysis::CreateSingleEventTree(TGo4EventElement*)",__LINE__, __FILE__));
   //
   if(event==0) return 0;
   TDirectory* filsav=gDirectory;
   gROOT->cd();
   delete fxSampleEvent;
   fxSampleEvent=(TGo4EventElement*) event->Clone();
   TTree* thetree= new TTree(fxSampleEvent->GetName(), "Single Event Tree");
   thetree->SetDirectory(0);
   TBranch *topbranch=
      thetree->Branch("Go4EventSample", fxSampleEvent->ClassName(), &fxSampleEvent, 64000, 99);
   if (fxSampleEvent->InheritsFrom("TGo4CompositeEvent"))
       dynamic_cast<TGo4CompositeEvent*>  (fxSampleEvent)->makeBranch(topbranch);
   thetree->Fill();
   filsav->cd();
   return thetree;

}

TTree* TGo4Analysis::CreateSingleEventTree(const Text_t* name, Bool_t isoutput)
{
   TRACE((11,"TGo4Analysis::CreateSingleEventTree(const Text_t*, Bool_t)",__LINE__, __FILE__));
   //
   TGo4EventElement* event=0;
   if(isoutput) event=GetOutputEvent(name);
           else event=GetInputEvent(name);
   if(event==0)
      {
         // event step of name does not exists, we search event in folder:
         event=GetEventStructure(name);
      }
   return CreateSingleEventTree(event);
}





void TGo4Analysis::CloseAnalysis()
{
TRACE((14,"TGo4Analysis::CloseAnalysis()",__LINE__, __FILE__));
   //
   if(fbInitIsDone)
      {
         AutoSave();
         fxStepManager->CloseAnalysis();
         fxObjectManager->CloseAnalysis();
         fbInitIsDone=kFALSE;
      }
   else {}
}

Int_t TGo4Analysis::PreLoop()
{
TRACE((11,"TGo4Analysis:PreLoop()",__LINE__, __FILE__));
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
      // avoid conflict with possible user object modifications during startup autosave!
   Int_t rev=0;
   fiAutoSaveCount=0;
   rev=UserPreLoop();
   fxAutoSaveClock->Start(kTRUE);
   return rev;
}

Int_t TGo4Analysis::PostLoop()
{
TRACE((11,"TGo4Analysis::PostLoop()",__LINE__, __FILE__));
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   Int_t rev=0;
   ////////////////////////Test of single event tree
//   TTree* mytree=CreateSingleEventTree("Unpack");
//   TFile* myfile=new TFile("eventsample.root","RECREATE");
//   mytree->SetDirectory(myfile);
//   mytree->Write();
//   delete myfile;
//   cout <<"___________Wrote eventsample to file eventsample.root" << endl;
   /////////////////////
   if(fbInitIsDone)   rev=UserPostLoop(); // do not call userpostloop after error in initialization
   return rev;
}

Int_t TGo4Analysis::UserPreLoop()
{
TRACE((11,"TGo4Analysis::UserPreLoop()",__LINE__, __FILE__));
   //
   Message(0,"Analysis BaseClass --  executing default User Preloop");
   return 0;
}

Int_t TGo4Analysis::UserPostLoop()
{
TRACE((11,"TGo4Analysis::UserPostLoop()",__LINE__, __FILE__));
   //
   Message(0,"Analysis BaseClass --  executing default User Postloop");
   return 0;
}

void TGo4Analysis::SetAutoSaveFile(const Text_t * filename, Bool_t overwrite, Int_t compression)
{
//   if(!fbAutoSaveOn) return; // do not set autosave file if disabled!
   //TGo4LockGuard  autoguard(fxAutoSaveMutex);
   Int_t buflen=1024;
   Text_t buffer[1024];
   if(filename)
      {
         strncpy(buffer,filename,buflen);
      }
   else
      {
         strncpy(buffer,fgcDEFAULTFILENAME,buflen);
      }
   if(!strstr(buffer,fgcDEFAULTFILESUF))
      {
         strncat(buffer,fgcDEFAULTFILESUF,buflen-1); // file suffix if not given by user
      }
   else {}
   // new: just set parameters, do not open file here:
   fxAutoFileName=buffer;
   fiAutoSaveCompression=compression;
   fbAutoSaveOverwrite=overwrite;
}


Int_t TGo4Analysis::LockAutoSave()
{
TRACE((12,"TGo4Analysis::LockAutoSave()",__LINE__, __FILE__));
   Int_t rev;
   if(TThread::Exists()>0 && fxAutoSaveMutex)
      {
         rev=fxAutoSaveMutex->Lock();
      }
   else
      {
         rev=-1;
      }

   return rev;
}

Int_t TGo4Analysis::UnLockAutoSave()
{
TRACE((12,"TGo4Analysis::UnLockAutoSave()",__LINE__, __FILE__));
   Int_t rev;
   if(TThread::Exists()>0 && fxAutoSaveMutex)
      {
         rev=fxAutoSaveMutex->UnLock();
      }
   else
      {
         rev=-1;
      }

   return rev;
}



void TGo4Analysis::AutoSave()
{
   TRACE((12,"TGo4Analysis::AutoSave()",__LINE__, __FILE__));
   //
   if(!fbAutoSaveOn) return;
   TGo4LockGuard  autoguard(fxAutoSaveMutex);
   Message(0,"Analysis  --  AutoSaving....");
   //UpdateNamesList();
   fxStepManager->AutoSave();
   Bool_t oldmode=fbAutoSaveOverwrite;
   fbAutoSaveOverwrite=kTRUE; // force overwriting old file when saving
   OpenAutoSaveFile();
   fxObjectManager->SaveObjects(fxAutoFile);
   CloseAutoSaveFile();
   fbAutoSaveOverwrite=oldmode;
   Message(0,"Analysis  --  AutoSave done.");
}

void TGo4Analysis::OpenAutoSaveFile()
{
if(!fbAutoSaveOn) return;
TGo4LockGuard  autoguard(fxAutoSaveMutex);
gROOT->cd();
if(fbAutoSaveOverwrite)
   {
      delete fxAutoFile;
      fxAutoFile = new TFile(fxAutoFileName.Data() ,"RECREATE");
      Message(-1,"Opening AutoSave file %s , RECREATE mode",fxAutoFileName.Data());
   }
else
   {
      if(fxAutoFile==0)
         {
         fxAutoFile = new TFile(fxAutoFileName.Data(),"UPDATE");
         Message(-1,"Opening AutoSave file %s , UPDATE mode",fxAutoFileName.Data());
         }
      else
         {
             Message(-1,"Reusing AutoSave file %s , UPDATE mode",fxAutoFileName.Data());
         }
  } // if(fbAutoSaveOverwrite)
fxAutoFile->SetCompressionLevel(fiAutoSaveCompression);
gROOT->cd();
}


void TGo4Analysis::CloseAutoSaveFile()
{
if(fxAutoFile)
      {
         TGo4LockGuard  autoguard(fxAutoSaveMutex);
         delete fxAutoFile;
         fxAutoFile=0;
         Message(-1,"AutoSave file %s was closed.",fxAutoFileName.Data());
      }
else {}

}



void TGo4Analysis::UpdateNamesList()
{
TRACE((11,"TGo4Analysis::UpdateNamesList()",__LINE__, __FILE__));
   //
//cout <<"UpdateNamesList: current dir:"<< gDirectory->GetName() << endl;
// first try: update all
delete fxObjectNames;
fxObjectNames = CreateNamesList();
Message(0,"Analysis BaseClass --  Nameslist updated.");
// debug:
//   fxObjectNames->PrintStatus();
}



Bool_t TGo4Analysis::LoadObjects(const char* filename)
{
TGo4LockGuard  autoguard(fxAutoSaveMutex);
Bool_t rev=kTRUE;
if(filename) fxAutoFileName=filename;
OpenAutoSaveFile();
if(fxAutoFile && fxAutoFile->IsOpen())
   {
     Message(-1,"Analysis LoadObjects: Loading from autosave file %s ",
           fxAutoFile->GetName());
     rev=fxObjectManager->LoadObjects(fxAutoFile);
   }
else
   {
      Message(-1,"Analysis LoadObjects: Failed to load from file %s",
        fxAutoFileName.Data());
      rev=kFALSE;
   }
CloseAutoSaveFile();
gROOT->cd();
return rev;
}

void TGo4Analysis::Message(Int_t prio, const Text_t* text,...)
{
   Text_t txtbuf[TGo4Log::fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, TGo4Log::fguMESLEN, text, args);
   va_end(args);
   SendMessageToGUI(prio,kTRUE, txtbuf);
}

void TGo4Analysis::SendMessageToGUI(Int_t level, Bool_t printout, const char* text)
{
   if(fxAnalysisSlave)
      {
         // gui mode: send Text via status channel
         fxAnalysisSlave->SendStatusMessage(level,printout,text);
      }
   else
      {
         // batch mode: no gui connection, handle local printout
         Bool_t previousmode=TGo4Log::IsOutputEnabled();
         TGo4Log::OutputEnable(printout); // override the messaging state
         TGo4Log::Message(level,text);
         TGo4Log::OutputEnable(previousmode);
      } // if (fxAnalysisSlave)
}

void TGo4Analysis::SendObjectToGUI(TNamed * ob)
{
   if(fxAnalysisSlave)
      {
         fxAnalysisSlave->SendObject(ob);
      }
   else
      {
         if(ob) Message(2,"Could not send object %s to GUI in batch mode.",ob->GetName());
      }
}

Bool_t TGo4Analysis::IsRunning()
{
   if(fxAnalysisSlave)
      return fxAnalysisSlave->MainIsRunning();
   else
      return TGo4Analysis::fbExists; // should be kTRUE
}

void TGo4Analysis::SetRunning(Bool_t on)
{
if(fxAnalysisSlave==0) return;
if(on)
   fxAnalysisSlave->Start();
else
   fxAnalysisSlave->Stop();

}


Int_t TGo4Analysis::WaitForStart()
{
   #if ROOT_VERSION_CODE > ROOT_VERSION(5,2,0)
   /// test: need to unlock cintmutex here to enable streaming!
   Bool_t unlockedcint=kFALSE;
   if(gCINTMutex)
    {
        gCINTMutex->UnLock();
        unlockedcint=kTRUE;
        //cout <<"WWWWWWWait for Start Unlocked cint mutex..." << endl;
    }
   #endif
   /////////
   Int_t cycles=0;
   fbStopWatingFlag = kFALSE;
   while(!IsRunning())
   {
      //cout <<"WWWWWWWait for Start before Sleep..." << endl;
      gSystem->Sleep(fgiMACROSTARTPOLL);
      cycles++;
      Bool_t sysret=gSystem->ProcessEvents();
//      cout <<"  after process events with ret="<<sysret << endl;
//      cout <<"  stopwatingflag is="<<fbStopWatingFlag << endl;
      if (sysret || fbStopWatingFlag)
         {
            //cout <<"wait sees root IsInterrupted..." << endl;
            cycles=-1;
            break;
         // allows cint canvas menu "Interrupt" to get us out of here!
         // additionally, without ProcessEvents no connect/disconnect of Go4
         // would be possible from this wait loop
         }
   }
   //if (fbStopWatingFlag) return -1;
   #if ROOT_VERSION_CODE > ROOT_VERSION(5,2,0)
     /// test: need to unlock cintmutex here to enable streaming!
   if(gCINTMutex && unlockedcint)
    {
        gCINTMutex->Lock();
        //cout <<"WWWWWWWait for Start locked cint mutex..." << endl;

    }
   /////// end test
   #endif
   return cycles;
}

void TGo4Analysis::StopWaiting()
{
   fbStopWatingFlag = kTRUE;
}

void TGo4Analysis::StartObjectServer(const Text_t* basename,  const Text_t* passwd)
   {
     if(fxAnalysisSlave)
      {
         Message(1,"Start object server not yet enabled.");
         //fxAnalysisSlave->StartObjectServer(basename,passwd);
      }
     else
      {
         Message(2,"Could not start object server in batch mode.");
      }
   }

void TGo4Analysis::StopObjectServer()
   {
    if(fxAnalysisSlave)
      {
         //fxAnalysisSlave->StopObjectServer();
         Message(1,"Start object server not yet enabled.");
      }
     else
      {
         Message(2,"Could not stop object server in batch mode.");
      }
   }



void TGo4Analysis::ShowEvent(const Text_t* stepname, Bool_t isoutput)
{
TTree* sevtree=CreateSingleEventTree(stepname,isoutput);
if(sevtree)
   {
       if(isoutput)
          Message(1,"Showing Output Event %s of step %s",sevtree->GetName(),stepname);
       else
          Message(1,"Showing Input  Event %s of step %s",sevtree->GetName(),stepname);
       sevtree->Show(0);
       cout << endl;
       delete sevtree;
   }

}



/////////////////////////////////////////////////////////////////////////////////
// Methods that forward to object manager:


Bool_t TGo4Analysis::AddDynamicHistogram(const Text_t* name,
                                  const Text_t* histo,
                                  const Text_t* hevx, const Text_t* hmemx,
                                  const Text_t* hevy, const Text_t* hmemy,
                                  const Text_t* hevz, const Text_t* hmemz,
                                  const Text_t* condition,
                                  const Text_t* cevx, const Text_t* cmemx,
                                  const Text_t* cevy, const Text_t* cmemy)
{
return (fxObjectManager->AddDynamicHistogram(name,
                                  histo, hevx, hmemx, hevy, hmemy, hevz, hmemz,
                                  condition, cevx, cmemx, cevy, cmemy));
}

Bool_t TGo4Analysis::AddTreeHistogram(const char* hisname, const char* treename, const char* varexp, const char* cutexp)
{
   Bool_t rev=fxObjectManager->AddTreeHistogram(hisname,treename,varexp,cutexp);
   if(rev) UpdateNamesList();
   return rev;
}

Bool_t TGo4Analysis::AddEventProcessor(TGo4EventProcessor * pro)
{
   return (fxObjectManager->AddEventProcessor(pro));
}

Bool_t TGo4Analysis::AddEventSource(TGo4EventSource * source)
{
   return (fxObjectManager->AddEventSource(source));
}

Bool_t TGo4Analysis::AddEventStore(TGo4EventStore * store)
{
   return (fxObjectManager->AddEventStore(store));
}

Bool_t TGo4Analysis::AddEventStructure(TGo4EventElement * ev)
{
   return (fxObjectManager->AddEventStructure(ev));
}

Bool_t TGo4Analysis::AddHistogram(TH1 * his , const Text_t* subfolder, Bool_t replace)
{
   return (fxObjectManager->AddHistogram(his,subfolder, replace));
}

Bool_t TGo4Analysis::AddObject(TNamed * anything, const Text_t* subfolder, Bool_t replace)
{
   return (fxObjectManager->AddObject(anything,subfolder,replace));
}

Bool_t TGo4Analysis::AddParameter(TGo4Parameter * par, const Text_t* subfolder)
{
   return (fxObjectManager->AddParameter(par,subfolder));
}

Bool_t TGo4Analysis::AddPicture(TGo4Picture * pic, const Text_t* subfolder)
{
   return (fxObjectManager->AddPicture(pic,subfolder));
}

Bool_t TGo4Analysis::AddCanvas(TCanvas * can, const Text_t* subfolder)
{
   return (fxObjectManager->AddCanvas(can,subfolder));
}



Bool_t TGo4Analysis::AddTree(TTree* tree, const Text_t* subfolder)
{
  if(tree) tree->ResetBit(TGo4Status::kGo4BackStoreReset);
  return (fxObjectManager->AddTree(tree, subfolder));
}



Bool_t TGo4Analysis::AddAnalysisCondition(TGo4Condition * con, const Text_t* subfolder)
{
  return (fxObjectManager->AddAnalysisCondition(con,subfolder));
}

TGo4Condition * TGo4Analysis::GetAnalysisCondition(const Text_t * name)
{
  return (fxObjectManager->GetAnalysisCondition(name));
}

TGo4AnalysisStep* TGo4Analysis::GetAnalysisStep(const Text_t* name)
{
  return fxStepManager->GetAnalysisStep(name);
}

Int_t TGo4Analysis::GetDynListInterval()
{
   return fxObjectManager->GetDynListInterval();
}

TGo4EventElement * TGo4Analysis::GetEventStructure(const Text_t * name)
{
  return (fxObjectManager->GetEventStructure(name));
}

TH1* TGo4Analysis::GetHistogram(const Text_t * name)
{
  return (fxObjectManager->GetHistogram(name));
}

TNamed * TGo4Analysis::GetObject(const Text_t * name, const Text_t* folder)
{
  return (fxObjectManager->GetObject(name,folder));
}

TGo4Parameter * TGo4Analysis::GetParameter(const Text_t * name)
{
  return (fxObjectManager->GetParameter(name));
}

TGo4Picture * TGo4Analysis::GetPicture(const Text_t * name)
{
  return (fxObjectManager->GetPicture(name));
}

TCanvas * TGo4Analysis::GetCanvas(const Text_t * name)
{
  return (fxObjectManager->GetCanvas(name));
}


TTree * TGo4Analysis::GetTree(const Text_t * name)
{
  return (fxObjectManager->GetTree(name));
}

TGo4AnalysisObjectNames * TGo4Analysis::CreateNamesList()
{
  return (fxObjectManager->CreateNamesList());
}

TFolder * TGo4Analysis::GetObjectFolder()
{
  return (fxObjectManager->GetObjectFolder());
}


TGo4ObjectStatus * TGo4Analysis::CreateObjectStatus(const Text_t * name, const Text_t* folder)
{
  return (fxObjectManager->CreateObjectStatus(name,folder));
}

TGo4TreeStructure * TGo4Analysis::CreateTreeStructure(const Text_t* treename)
{
  return (fxObjectManager->CreateTreeStructure(treename));
}

Bool_t TGo4Analysis::RemoveEventSource(TGo4EventSource* source)
{
  return (fxObjectManager->RemoveEventSource(source));
}

Bool_t TGo4Analysis::RemoveEventStore(TGo4EventStore * store)
{
  return (fxObjectManager->RemoveEventStore(store));
}

Bool_t TGo4Analysis::RemoveEventStructure(TGo4EventElement * ev)
{
  return (fxObjectManager->RemoveEventStructure(ev));
}

Bool_t TGo4Analysis::RemoveHistogram(const Text_t * name, Bool_t del)
{
  return (fxObjectManager->RemoveHistogram(name,del));
}

Bool_t TGo4Analysis::RemoveObject(const Text_t * name, Bool_t del)
{
  return (fxObjectManager->RemoveObject(name,del));
}



Bool_t TGo4Analysis::RemoveParameter(const Text_t * name)
{
  return (fxObjectManager->RemoveParameter(name));
}

Bool_t TGo4Analysis::RemovePicture(const Text_t * name)
{
  return (fxObjectManager->RemovePicture(name));
}

Bool_t TGo4Analysis::RemoveCanvas(const Text_t * name)
{
  return fxObjectManager->RemoveCanvas(name);
}

Bool_t TGo4Analysis::RemoveTree(TTree * tree, const char* stepname)
{
  return fxObjectManager->RemoveTree(tree, stepname);
}

Bool_t TGo4Analysis::RemoveAnalysisCondition(const Text_t* name)
{
  return fxObjectManager->RemoveAnalysisCondition(name);
}

Bool_t TGo4Analysis::RemoveEventProcessor(TGo4EventProcessor * pro)
{
  return (fxObjectManager->RemoveEventProcessor(pro));
}

Bool_t TGo4Analysis::DeleteObjects(const Text_t * name)
{
  return (fxObjectManager->DeleteObjects(name));
}

Bool_t TGo4Analysis::ClearObjects(const Text_t * name)
{
  return (fxObjectManager->ClearObjects(name));
}

Bool_t TGo4Analysis::ProtectObjects(const Text_t * name, const Option_t* flags)
{
  return (fxObjectManager->ProtectObjects(name, flags));
}

Bool_t  TGo4Analysis::ResetBackStores(Bool_t clearflag)
{
 return (fxObjectManager->ResetBackStores(clearflag));
}

Bool_t TGo4Analysis::SetAnalysisCondition(const Text_t * name, TGo4Condition* con,
                                   Bool_t counter)
{
  return (fxObjectManager->SetAnalysisCondition(name, con, counter));
}

Bool_t TGo4Analysis::SetParameter(const char* name, TGo4Parameter* par)
{
  return (fxObjectManager->SetParameter(name, par));
}

Bool_t TGo4Analysis::SetParameterStatus(const char* name, TGo4ParameterStatus* par)
{
  return (fxObjectManager->SetParameterStatus(name, par));
}

Bool_t TGo4Analysis::SetPicture(const Text_t * name, TGo4Picture * pic)
{
  return (fxObjectManager->SetPicture(name, pic));
}


void TGo4Analysis::SetDynListInterval(Int_t val)
{
   fxObjectManager->SetDynListInterval(val);
}

void TGo4Analysis::PrintConditions(const Text_t* expression)
{
   fxObjectManager->PrintConditions(expression);
}

void TGo4Analysis::PrintHistograms(const Text_t* expression)
{
   fxObjectManager->PrintHistograms(expression);
}

void TGo4Analysis::PrintDynamicList()
{
   fxObjectManager->PrintDynamicList();
}

TObject* TGo4Analysis::NextMatchingObject(const Text_t* expr,
                        const Text_t* folder,
                        Bool_t reset)
{
return (fxObjectManager->NextMatchingObject(expr,folder,reset));
}




/////////////////////////////////////////////////////////////////////////////////
// Methods that forward to stepmanager:

TGo4EventElement* TGo4Analysis::GetInputEvent(Int_t stepindex)
{
   return (fxStepManager->GetInputEvent(stepindex));
}

TGo4EventElement* TGo4Analysis::GetInputEvent(const char* stepname)
{
     return (fxStepManager->GetInputEvent(stepname));
}

TGo4EventElement* TGo4Analysis::GetOutputEvent()
{
   return fxStepManager->GetOutputEvent();
}

TGo4EventElement* TGo4Analysis::GetOutputEvent(Int_t stepindex)
{
  return (fxStepManager->GetOutputEvent(stepindex));
}

TGo4EventElement* TGo4Analysis::GetOutputEvent(const char* stepname)
{
     return (fxStepManager->GetOutputEvent(stepname));
}

Bool_t TGo4Analysis::NewStepProcessor(const Text_t* name, TGo4EventProcessorParameter * par)
{
     return (fxStepManager->NewStepProcessor(name,par));
}

Bool_t TGo4Analysis::NewStepSource(const Text_t* name, TGo4EventSourceParameter * par)
{
     return (fxStepManager->NewStepSource(name,par));
}

Bool_t TGo4Analysis::NewStepStore(const Text_t* name, TGo4EventStoreParameter* par)
{
     return (fxStepManager->NewStepStore(name,par));
}

Bool_t TGo4Analysis::AddDynamicEntry(TGo4DynamicEntry* entry)
{
  return fxObjectManager->AddDynamicEntry(entry);
}

void TGo4Analysis::SetStepChecking(Bool_t on)
{
   fxStepManager->SetStepChecking(on);
}


Bool_t TGo4Analysis::SetFirstStep(const Text_t* name)
{
  return (fxStepManager->SetFirstStep(name));
}

Bool_t TGo4Analysis::SetLastStep(const Text_t * name)
{
  return (fxStepManager->SetLastStep(name));
}

Bool_t TGo4Analysis::SetStepStorage(const Text_t * name, Bool_t on)
{
   return (fxStepManager->SetStepStorage(name,on));
}

Int_t TGo4Analysis::IsErrorStopEnabled()
{
   return fxStepManager->IsErrorStopEnabled();
}

Bool_t TGo4Analysis::AddAnalysisStep(TGo4AnalysisStep* next)
{
   return ( fxStepManager->AddAnalysisStep(next) );
}

Int_t TGo4Analysis::ProcessAnalysisSteps()
{
   return (fxStepManager->ProcessAnalysisSteps());
}

void TGo4Analysis::SetOutputEvent(TGo4EventElement * event)
{
   fxStepManager->SetOutputEvent(event);
}

Int_t TGo4Analysis::StoreParameter(const Text_t * name, TGo4Parameter* par)
{
return (fxStepManager->Store(name, par));
}

Int_t TGo4Analysis::StoreCondition(const Text_t * name, TGo4Condition* con)
{
return (fxStepManager->Store(name, con));
}

Int_t TGo4Analysis::StoreFitter(const Text_t * name, TGo4Fitter* fit)
{
return (fxStepManager->Store(name, fit));
}

Int_t TGo4Analysis::StoreFolder(const Text_t * name, TFolder* folder)
{
return (fxStepManager->Store(name, folder));
}

Int_t TGo4Analysis::StoreFolder(const Text_t * stepname, const Text_t * foldername)
{
TFolder* myfolder=fxObjectManager->FindSubFolder(GetObjectFolder(), foldername, kFALSE);
if(myfolder)
   return (fxStepManager->Store(stepname, myfolder));
else
   return 2;
}


void TGo4Analysis::SetObserverPassword(const char* passwd)
{
   TGo4TaskHandler::SetObservAccount(0,passwd);
}

void TGo4Analysis::SetControllerPassword(const char* passwd)
{
   TGo4TaskHandler::SetCtrlAccount(0,passwd);
}

void TGo4Analysis::SetAdministratorPassword(const char* passwd)
{
   TGo4TaskHandler::SetAdminAccount(0,passwd);
}




