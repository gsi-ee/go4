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

#include "TGo4AnalysisStepManager.h"

#include "TObjArray.h"
#include "TFolder.h"
#include "TList.h"
#include "TThread.h"
#include "TH1.h"
#include "TTree.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Condition.h"
#include "TGo4MainTree.h"
#include "TGo4AnalysisStep.h"
#include "TGo4TreeStructure.h"

#include "TGo4AnalysisStepException.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisImp.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventProcessor.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4CompositeEvent.h"
#include "TGo4BackStore.h"

TGo4AnalysisStepManager::TGo4AnalysisStepManager(const char* name) :
   TNamed(name,"The Go4 Analysis Step Manager"),
   fiFirstStepIndex(0),
   fiLastStepIndex(0),
   fiCurrentStepIndex(0),
   fbStepCheckingMode(kTRUE),
   fxCurrentStep(0),
   fxOutputEvent(0)
{
   fxStepList = new TObjArray;
   fxStepIterator = fxStepList->MakeIterator();
}

TGo4AnalysisStepManager::~TGo4AnalysisStepManager()
{
   delete fxStepIterator;
   delete fxStepList;
}

void TGo4AnalysisStepManager::CloseAnalysis()
{
   GO4TRACE((14,"TGo4AnalysisStepManager::CloseAnalysis()",__LINE__, __FILE__));
   //
   TGo4Log::Debug("Analysis Step Manager  --  closing analysis steps...");
   TGo4AnalysisStep* step=0;
   fxStepIterator->Reset();
   while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
   {
      step->Close();
   }
   TGo4Log::Debug("Analysis Step Manager  --  analysis steps were closed.");
}

Bool_t TGo4AnalysisStepManager::InitEventClasses()
{
   GO4TRACE((14,"TGo4AnalysisStepManager::InitEventClasses()",__LINE__, __FILE__));
   //
   Bool_t rev=kTRUE;
   Bool_t firststepfound=kFALSE;
   Bool_t laststepfound=kFALSE;
   TGo4Log::Debug("Analysis StepManager --  Initializing EventClasses...");
   TGo4AnalysisStep* step=0;
   fxStepIterator->Reset();
   fiCurrentStepIndex=0;
   //std::cout <<"IIIIIIII InitEventClasses with checking: "<<IsStepChecking() << std::endl;
   while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
   {
      step->InitEventClasses();
      // last enabled step:
      // for step checking off, take last step in list
      if(firststepfound)
      {
         if((IsStepChecking() && step->IsProcessEnabled())
               || !IsStepChecking())
         {
            fiLastStepIndex=fiCurrentStepIndex;
            laststepfound=kTRUE;
         }
      }
      // first enabled step is first step of chain:
      // except for step checking is off, then take first
      if(!firststepfound)
      {
         if((IsStepChecking() && step->IsProcessEnabled())
               || !IsStepChecking())
         {
            fiFirstStepIndex=fiCurrentStepIndex;
            firststepfound=kTRUE;
         }
      }
      fiCurrentStepIndex++;
   }
   if(!laststepfound) fiLastStepIndex=fiFirstStepIndex; // for single step analysis

   if(IsStepChecking())
   {
      // Test for steps valid:
      fxStepIterator->Reset();
      fiCurrentStepIndex=0;
      while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
      {
         //std::cout << "match testing of analysis step " << step->GetName() << std::endl;
         if(! step->IsMatchingPrevious() )
         {
            rev=kFALSE;
            TGo4Analysis::Instance()->Message(3,"!!! AnalysisStepManager  --  ERROR: step %s is not matching previous !!!",
                  step->GetName() );
            break;
         }
         else
         {
            rev=kTRUE;
         }
      } // while
   }//if(IsStepChecking())

   TGo4Log::Debug("AnalysisStepManager  --  Initializing EventClasses done.");
   return rev;
}

Bool_t TGo4AnalysisStepManager::SetFirstStep(const char* name)
{
   GO4TRACE((12,"TGo4AnalysisStepManager::SetFirstStep(const char*)",__LINE__, __FILE__));
   //
   Bool_t result=kFALSE;
   if(name==0) {
      // reset to defaults:
      fiFirstStepIndex=0; // beginning of steplist
      TGo4Analysis::Instance()->Message(0,"Analysis: Setting first step to beginning of steplist");
      result=kTRUE;
   } else {
      TObject* obj=fxStepList->FindObject(name);
      if(obj==0) {
         result=kFALSE;
         TGo4Analysis::Instance()->Message(3,"!!! Analysis: SetFirstStep ERROR - no such step %s",
               name);
      } else {
         Int_t ix=fxStepList->IndexOf(obj);
         if(ix <= fiLastStepIndex)
         {
            fiFirstStepIndex=ix;
            TGo4Analysis::Instance()->Message(0,"Analysis: Setting first step to %s",
                  name);
         }
         else
         {
            fiFirstStepIndex=fiLastStepIndex;
            TGo4Analysis::Instance()->Message(0,"Analysis: Range WARNING - Setting first step to last step");
         }

         result=kTRUE;
      }

   }
   return result;
}

Bool_t TGo4AnalysisStepManager::SetLastStep(const char* name)
{
   GO4TRACE((12,"TGo4AnalysisStepManager::SetLastStep(const char*)",__LINE__, __FILE__));
   //
   Bool_t result=kTRUE;
   if(name==0) {
      // reset to defaults:
      fiLastStepIndex=fxStepList->GetLast() ; // end of steplist
      if(fiLastStepIndex<0)
         fiLastStepIndex=0; // case of empty steplist
      TGo4Analysis::Instance()->Message(0,"Analysis: Setting last step to end of steplist");

      result=kTRUE;
   } else {
      TObject* obj=fxStepList->FindObject(name);
      if(obj==0) {
         result=kFALSE;
         TGo4Analysis::Instance()->Message(3,"!!! Analysis: SetLastStep ERROR - no such step %s",
               name);

      } else {
         Int_t ix=fxStepList->IndexOf(obj);
         if(ix >= fiFirstStepIndex) {
            fiLastStepIndex=ix;
            TGo4Analysis::Instance()->Message(0,"Analysis: Setting last step to %s",
                  name);
         } else {
            fiLastStepIndex=fiFirstStepIndex;
            TGo4Analysis::Instance()->Message(0," Analysis: Range WARNING - Setting last step to first step");

         }

         result=kTRUE;
      }


   }
   return result;
}

Bool_t TGo4AnalysisStepManager::SetStepStorage(const char* name, Bool_t on)
{
GO4TRACE((12,"TGo4AnalysisStepManager::SetStepStorage(const char*,Bool_t)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step)
      {
         step->SetStoreEnabled(on);
         result=kTRUE;
      }
   else
      {
         result=kFALSE;
      }

   return result;

}

Bool_t TGo4AnalysisStepManager::NewStepStore(const char * name, TGo4EventStoreParameter* par)
{
   GO4TRACE((12,"TGo4AnalysisStepManager::NewStepStore(const char *, TGo4EventStoreParameter*)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=0;
   if(name==0) {
      // zero name: use last step
      step=dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(fiLastStepIndex));
   } else {
      // step specified by name:
      step=GetAnalysisStep(name);
   }

   if(step) {
      //step->SetEventStore(par); // remember parameter for next init
      step->NewEventStore(par); // create new store now
      result=kTRUE;
   } else {
      result=kFALSE;
   }

   return result;
}

Bool_t TGo4AnalysisStepManager::NewStepSource(const char * name, TGo4EventSourceParameter * par)
{
   GO4TRACE((12,"TGo4AnalysisStepManager::NewStepSource(const char *, TGo4EventSourceParameter *)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=0;
   if(name==0) {
      // zero name: use first step
      step=dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(fiFirstStepIndex));
      //std::cout << "new step source: zero name"<< std::endl;
   } else {
      // step specified by name:
      step=GetAnalysisStep(name);
      //std::cout << "new step source: name="<< name << std::endl;
   }

   if(step) {
      //step->SetEventSource(par); // remember parameter for next init
      step->NewEventSource(par); // delete old, and create the new source now
      result=kTRUE;
      //std::cout << "new step source: step found"<< std::endl;
   } else {
      result=kFALSE;
      //std::cout << "new step source: step not found"<< std::endl;
   }
   return result;
}

Bool_t TGo4AnalysisStepManager::NewStepProcessor(const char * name, TGo4EventProcessorParameter * par)
{
   GO4TRACE((12,"TGo4AnalysisStepManager::NewStepProcessor(const char *, TGo4EventProcessorParameter *)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step) {
      //step->SetEventProcessor(par); // remember parameter for next init
      step->NewEventProcessor(par); // create processor now
      result=kTRUE;
   } else {
      result=kFALSE;
   }
   return result;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Parameter* par)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   return (step!=0) ? step->Store(par) : 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Condition* con)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   return (step!=0) ? step->Store(con) : 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Fitter* fit)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   return (step!=0) ? step->Store(fit) : 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TFolder* folder)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   return (step!=0) ? step->Store(folder) : 1;
}


TGo4EventElement* TGo4AnalysisStepManager::GetInputEvent(const char* stepname)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetInputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=GetAnalysisStep(stepname);
   if(step) {
      TGo4EventProcessor* pro=step->GetEventProcessor();
      if(pro)
         rev=pro->GetInputEvent(); // get true input event
   } else {
      rev=0;
   }
   return rev;
}

TGo4EventElement* TGo4AnalysisStepManager::GetInputEvent(Int_t stepindex)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetInputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev = 0;
   TGo4AnalysisStep* step = dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(stepindex) );
   if(step) {
      TGo4EventProcessor* pro = step->GetEventProcessor();
      if(pro) rev=pro->GetInputEvent(); // get true input event
      //rev=step->GetInputEvent();
   } else {
      rev=0;
   }
   return rev;
}

TGo4EventElement* TGo4AnalysisStepManager::GetOutputEvent(const char* stepname)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetOutputEvent(const char*)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=GetAnalysisStep(stepname);
   if(step) {
      rev = step->GetOutputEvent();
   } else {
      rev=0;
   }
   return rev;
}

TGo4EventElement* TGo4AnalysisStepManager::GetOutputEvent(Int_t stepindex)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetOutputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=0;
   step= dynamic_cast<TGo4AnalysisStep*> ( fxStepList->At(stepindex) );
   if(step) {
      rev=step->GetOutputEvent();
   } else {
      rev=0;
   }
   return rev;
}

Bool_t  TGo4AnalysisStepManager::AddAnalysisStep(TGo4AnalysisStep* next)
{
   GO4TRACE((14,"TGo4AnalysisStepManager::AddAnalysisStep(TGo4AnalysisStep*)",__LINE__, __FILE__));
   //
   Bool_t rev=kFALSE;
   if (next) {
      if(fxStepList->FindObject(next)==0)
         // is object already in list?
      {
         //no, add the new object
         GO4TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Adding new analysis step",__LINE__, __FILE__));
         fxStepList->AddLast(next);
         // set previous step:
         Int_t ix=fxStepList->IndexOf(next);
         if(ix>0)
         {
            TGo4AnalysisStep* previous= dynamic_cast<TGo4AnalysisStep*> ( fxStepList->At(ix-1) );
            next->SetPreviousStep(previous);
         }
         else
         {
            next->SetPreviousStep(0);
         }
         fiLastStepIndex=ix;
         rev=kTRUE;
         TGo4Analysis::Instance()->Message(1,"Analysis: Added analysis step %s",
               next->GetName());
      }
      else
      {
         // yes, do nothing
         GO4TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Analysis step was already there",__LINE__, __FILE__));
         rev=kFALSE;
         TGo4Analysis::Instance()->Message(2,"Analysis: WARNING - analysis step %s was already in steplist",
               next->GetName() );
      }
   } //  if(next)
   else
   {
      GO4TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Zero Analysis step pointer",__LINE__, __FILE__));
      rev=kFALSE;
      TGo4Analysis::Instance()->Message(2,"Analysis: WARNING - did not add zero analysis step pointer to steplist");
   }
   return rev;
}

TGo4AnalysisStep * TGo4AnalysisStepManager::GetAnalysisStep(const char* name)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetAnalysisStep(const char *)",__LINE__, __FILE__));
   TGo4AnalysisStep* step=0;
   if(name==0)
      step=dynamic_cast<TGo4AnalysisStep*>( fxStepList->At(fiFirstStepIndex));
   else
      step = dynamic_cast<TGo4AnalysisStep*>( fxStepList->FindObject(name) );
   return step;
}

Int_t TGo4AnalysisStepManager::GetNumberOfAnalysisSteps()
{
   return fxStepList ? fxStepList->GetLast() + 1 : 0;
}

TGo4AnalysisStep* TGo4AnalysisStepManager::GetAnalysisStepNum(Int_t number)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::GetAnalysisStepNum(Int_t)",__LINE__, __FILE__));
   if ((number<0) || (number>fxStepList->GetLast())) return 0;
   return dynamic_cast<TGo4AnalysisStep*>(fxStepList->At(number));
}


Int_t TGo4AnalysisStepManager::ProcessAnalysisSteps()
{
   GO4TRACE((11,"TGo4AnalysisStepManager::ProcessAnalysisSteps()",__LINE__, __FILE__));
   //
   fxCurrentStep = 0;
   fiCurrentStepIndex = 0;
   Bool_t isfirststep = kTRUE;
   // first evaluate actual beginning index for "keep input event" mode:
   Int_t repeatinputstart=-1;
   for(fiCurrentStepIndex = fiFirstStepIndex; fiCurrentStepIndex<=fiLastStepIndex;fiCurrentStepIndex++) {
      fxCurrentStep = (TGo4AnalysisStep*) (fxStepList->UncheckedAt(fiCurrentStepIndex));
      if(fxCurrentStep->IsKeepInputEvent()) {
         repeatinputstart=fiCurrentStepIndex;
         break;
      }
   }

   SetOutputEvent(0); // make sure that first step wont take output of last one
   for(fiCurrentStepIndex = fiFirstStepIndex; fiCurrentStepIndex<=fiLastStepIndex;fiCurrentStepIndex++) {
      fxCurrentStep = (TGo4AnalysisStep*) (fxStepList->UncheckedAt(fiCurrentStepIndex));
      if(fxCurrentStep==0) break;
      if(IsStepChecking() && isfirststep ) {
         // check first step source:
         isfirststep = kFALSE;
         if(fxCurrentStep->IsSourceImplemented())
            fxCurrentStep->SetSourceEnabled();
         else {
            fxCurrentStep->SetStatusMessage("!!! No Event Source for first analysis step !!!");
            throw TGo4AnalysisStepException(fxCurrentStep);
         }
      }
      if(fiCurrentStepIndex<repeatinputstart) continue; // skip steps before a step which is reprocessing same input event

      fxCurrentStep->Process();

      if(fxCurrentStep->IsKeepOutputEvent()) break; // skip all steps after incomplete output event
   } // for(...)
   // finally, we update maintree header if the steps use treesource/store instances:
   if(TGo4MainTree::Exists()) TGo4MainTree::Instance()->Update();
   return 0;
}

void TGo4AnalysisStepManager::UpdateStatus(TGo4AnalysisStatus* state)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::UpdateStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state!=0) {
      state->SetFirstStepIndex(fiFirstStepIndex);
      state->SetLastStepIndex(fiLastStepIndex);
      state->SetStepChecking(fbStepCheckingMode);
      fxCurrentStep=0;
      fxStepIterator->Reset();
      while((fxCurrentStep= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
      {
         TGo4AnalysisStepStatus* stepstate= fxCurrentStep->CreateStatus();
         state->AddStepStatus(stepstate);
      } // while(fxCurrentStep..)
   } // if(state!=0)
}

void TGo4AnalysisStepManager::SetStatus(TGo4AnalysisStatus * state)
{
   GO4TRACE((11,"TGo4AnalysisStepManager::SetStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state!=0) {
      fiFirstStepIndex=state->GetFirstStepIndex();
      fiLastStepIndex=state->GetLastStepIndex();
      fbStepCheckingMode=state->IsStepChecking();
      // note: the step indices are not used for
      // initialization of analysis any more!
      // update internal states of steps:
      fxCurrentStep=0;
      fxStepIterator->Reset();
      while((fxCurrentStep= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
      {
         const char* name= fxCurrentStep->GetName();
         TGo4AnalysisStepStatus* stepstate= state->GetStepStatus(name);
         fxCurrentStep->SetStatus(stepstate);

      } // while(fxCurrentStep..)
   } // if(state!=0)
}

void TGo4AnalysisStepManager::AutoSave()
{
   GO4TRACE((12,"TGo4AnalysisStepManager::AutoSave()",__LINE__, __FILE__));
   //
   //TGo4LockGuard  autoguard(fxAutoSaveMutex);
   TGo4Analysis::Instance()->Message(0,"Analysis Step Manager  --  AutoSaving....");
   TGo4AnalysisStep* step=0;
   fxStepIterator->Reset();
   while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
   {
      step->StoreCalibration();
   }

   // write maintree to file if existing...
   if(TGo4MainTree::Exists()) {
      TGo4MainTree::Instance()->Write();
   }
}

Int_t TGo4AnalysisStepManager::IsErrorStopEnabled()
{
   return kTRUE; // FIXME: workaround, to be removed later!!! JA

   return (fxCurrentStep!=0) ? fxCurrentStep->IsErrorStopEnabled() : kTRUE;
}
