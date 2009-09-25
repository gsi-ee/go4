#include "TGo4AnalysisStepManager.h"

#include "Riostream.h"

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
   fxStepMutex = new TMutex(kTRUE);
   fxStepIterator = fxStepList->MakeIterator();
}

TGo4AnalysisStepManager::~TGo4AnalysisStepManager()
{
   delete fxStepMutex;
   delete fxStepIterator;
   delete fxStepList;
}



void TGo4AnalysisStepManager::CloseAnalysis()
{
TRACE((14,"TGo4AnalysisStepManager::CloseAnalysis()",__LINE__, __FILE__));
   //
   TGo4Analysis::Instance()->Message(0,"Analysis Step Manager  --  Closing Analysis Steps...");
   TGo4AnalysisStep* step=0;
      {
      //TGo4LockGuard   listguard(fxStepMutex);
         fxStepIterator->Reset();
         while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
            {
               step->Close();
            }
      } // lock guard
   TGo4Analysis::Instance()->Message(-1,"Analysis Step Manager  --  Analysis Steps were closed.");
}

Bool_t TGo4AnalysisStepManager::InitEventClasses()
{
TRACE((14,"TGo4AnalysisStepManager::InitEventClasses()",__LINE__, __FILE__));
   //
   Bool_t rev=kTRUE;
   Bool_t firststepfound=kFALSE;
   Bool_t laststepfound=kFALSE;
   TGo4Analysis::Instance()->Message(0,"Analysis StepManager --  Initializing EventClasses...");
      TGo4AnalysisStep* step=0;
         {
         //TGo4LockGuard  listguard(fxStepMutex);
            fxStepIterator->Reset();
            fiCurrentStepIndex=0;
            //cout <<"IIIIIIII InitEventClasses with checking: "<<IsStepChecking() << endl;
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
                        //cout << "match testing of analysis step " << step->GetName() << endl;
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
         } // lock guard
      TGo4Analysis::Instance()->Message(-1,"AnalysisStepManager  --  Initializing EventClasses done.");
      return rev;
}
Bool_t TGo4AnalysisStepManager::SetFirstStep(const char* name)
{
TRACE((12,"TGo4AnalysisStepManager::SetFirstStep(const char*)",__LINE__, __FILE__));
   //
   Bool_t result=kFALSE;
   if(name==0)
      {
         // reset to defaults:
         fiFirstStepIndex=0; // beginning of steplist
         TGo4Analysis::Instance()->Message(0,"Analysis: Setting first step to beginning of steplist");
         result=kTRUE;
      }
   else
      {
         TObject* obj=fxStepList->FindObject(name);
         if(obj==0)
            {
               result=kFALSE;
               TGo4Analysis::Instance()->Message(3,"!!! Analysis: SetFirstStep ERROR - no such step %s",
                        name);
            }
         else
            {
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
TRACE((12,"TGo4AnalysisStepManager::SetLastStep(const char*)",__LINE__, __FILE__));
   //
   Bool_t result=kTRUE;
   if(name==0)
      {
         // reset to defaults:
         fiLastStepIndex=fxStepList->GetLast() ; // end of steplist
         if(fiLastStepIndex<0)
            fiLastStepIndex=0; // case of empty steplist
         TGo4Analysis::Instance()->Message(0,"Analysis: Setting last step to end of steplist");

         result=kTRUE;
      }
   else
      {
         TObject* obj=fxStepList->FindObject(name);
         if(obj==0)
            {
               result=kFALSE;
               TGo4Analysis::Instance()->Message(3,"!!! Analysis: SetLastStep ERROR - no such step %s",
                        name);

            }
         else
            {
               Int_t ix=fxStepList->IndexOf(obj);
               if(ix >= fiFirstStepIndex)
                  {
                     fiLastStepIndex=ix;
                     TGo4Analysis::Instance()->Message(0,"Analysis: Setting last step to %s",
                              name);
                  }
               else
                  {
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
TRACE((12,"TGo4AnalysisStepManager::SetStepStorage(const char*,Bool_t)",__LINE__, __FILE__));
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
TRACE((12,"TGo4AnalysisStepManager::NewStepStore(const char *, TGo4EventStoreParameter*)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=0;
   if(name==0)
      {
         // zero name: use last step
         step=dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(fiLastStepIndex));
      }
   else
      {
         // step specified by name:
         step=GetAnalysisStep(name);
      }
   if(step)
      {
         //step->SetEventStore(par); // remember parameter for next init
         step->NewEventStore(par); // create new store now
         result=kTRUE;
      }
   else
      {
         result=kFALSE;
      }

   return result;

}
Bool_t TGo4AnalysisStepManager::NewStepSource(const char * name, TGo4EventSourceParameter * par)
{
TRACE((12,"TGo4AnalysisStepManager::NewStepSource(const char *, TGo4EventSourceParameter *)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=0;
   if(name==0)
      {
         // zero name: use first step
         step=dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(fiFirstStepIndex));
         //cout << "new step source: zero name"<< endl;
      }
   else
      {
         // step specified by name:
         step=GetAnalysisStep(name);
         //cout << "new step source: name="<< name << endl;
      }
   if(step)
      {
         //step->SetEventSource(par); // remember parameter for next init
         step->NewEventSource(par); // delete old, and create the new source now
         result=kTRUE;
         //cout << "new step source: step found"<< endl;
      }
   else
      {
         result=kFALSE;
         //cout << "new step source: step not found"<< endl;
      }
   return result;
}

Bool_t TGo4AnalysisStepManager::NewStepProcessor(const char * name, TGo4EventProcessorParameter * par)
{
TRACE((12,"TGo4AnalysisStepManager::NewStepProcessor(const char *, TGo4EventProcessorParameter *)",__LINE__, __FILE__));
   Bool_t result=kFALSE;
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step)
      {
         //step->SetEventProcessor(par); // remember parameter for next init
         step->NewEventProcessor(par); // create processor now
         result=kTRUE;
      }
   else
      {
         result=kFALSE;
      }
   return result;
}


Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Parameter* par)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step!=0)
      return step->Store(par);
   else
      return 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Condition* con)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step!=0)
      return step->Store(con);
   else
      return 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TGo4Fitter* fit)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step)
      return(step->Store(fit));
   else
      return 1;
}

Int_t TGo4AnalysisStepManager::Store(const char * name, TFolder* folder)
{
   TGo4AnalysisStep* step=GetAnalysisStep(name);
   if(step)
     return(step->Store(folder));
   else
      return 1;
}





TGo4EventElement* TGo4AnalysisStepManager::GetInputEvent(const char* stepname)
{
TRACE((11,"TGo4AnalysisStepManager::GetInputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=GetAnalysisStep(stepname);
   if(step)
      {
         TGo4EventProcessor* pro=step->GetEventProcessor();
         if(pro)
            rev=pro->GetInputEvent(); // get true input event
      }
   else
      {
         rev=0;
      }
   return rev;
}
TGo4EventElement* TGo4AnalysisStepManager::GetInputEvent(Int_t stepindex)
{
TRACE((11,"TGo4AnalysisStepManager::GetInputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=0;
   step= dynamic_cast<TGo4AnalysisStep*> (fxStepList->At(stepindex) );
   if(step)
      {
         TGo4EventProcessor* pro=step->GetEventProcessor();
         if(pro)
            rev=pro->GetInputEvent(); // get true input event
         //rev=step->GetInputEvent();
      }
   else
      {
         rev=0;
      }
   return rev;
}

TGo4EventElement* TGo4AnalysisStepManager::GetOutputEvent(const char* stepname)
{
TRACE((11,"TGo4AnalysisStepManager::GetOutputEvent(const char*)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=GetAnalysisStep(stepname);
   if(step)
      {
         rev=step->GetOutputEvent();
      }
   else
      {
         rev=0;
      }
   return rev;
}
TGo4EventElement* TGo4AnalysisStepManager::GetOutputEvent(Int_t stepindex)
{
TRACE((11,"TGo4AnalysisStepManager::GetOutputEvent(Int_t)",__LINE__, __FILE__));
   TGo4EventElement* rev=0;
   TGo4AnalysisStep* step=0;
   step= dynamic_cast<TGo4AnalysisStep*> ( fxStepList->At(stepindex) );
   if(step)
      {
         rev=step->GetOutputEvent();
      }
   else
      {
         rev=0;
      }
   return rev;
}

Bool_t  TGo4AnalysisStepManager::AddAnalysisStep(TGo4AnalysisStep* next)
{
TRACE((14,"TGo4AnalysisStepManager::AddAnalysisStep(TGo4AnalysisStep*)",__LINE__, __FILE__));
   //
   Bool_t rev=kFALSE;
   if(next)
      {
      //TGo4LockGuard  listguard(fxStepMutex);
      if(fxStepList->FindObject(next)==0)
         // is object already in list?
            {
               //no, add the new object
               TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Adding new analysis step",__LINE__, __FILE__));
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
               TGo4Analysis::Instance()->Message(-1,"Analysis: Added analysis step %s",
                  next->GetName());
            }
         else
            {
               // yes, do nothing
               TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Analysis step was already there",__LINE__, __FILE__));
               rev=kFALSE;
               TGo4Analysis::Instance()->Message(-1,"Analysis: WARNING - analysis step %s was already in steplist",
                  next->GetName() );
            }
      } //  if(next) ; TGo4LockGuard
   else
      {
       TRACE((12,"TGo4AnalysisStepManager::AddAnalysisStep -- Zero Analysis step pointer",__LINE__, __FILE__));
         rev=kFALSE;
         TGo4Analysis::Instance()->Message(-1,"Analysis: WARNING - did not add zero analysis step pointer to steplist");
      }
   return rev;
}

TGo4AnalysisStep * TGo4AnalysisStepManager::GetAnalysisStep(const char* name)
{
TRACE((11,"TGo4AnalysisStepManager::GetAnalysisStep(const char *)",__LINE__, __FILE__));
   TGo4AnalysisStep* step=0;
   if(name==0)
      {
         step=dynamic_cast<TGo4AnalysisStep*>( fxStepList->At(fiFirstStepIndex));
      }
      else
      {
      //TGo4LockGuard  listguard(fxStepMutex);
         step = dynamic_cast<TGo4AnalysisStep*>( fxStepList->FindObject(name) );
      }
   return step;
}

Int_t TGo4AnalysisStepManager::ProcessAnalysisSteps()
{
   TRACE((11,"TGo4AnalysisStepManager::ProcessAnalysisSteps()",__LINE__, __FILE__));
   //
   fxCurrentStep = 0;
   fiCurrentStepIndex = 0;
   Bool_t isfirststep = kTRUE;
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
      fxCurrentStep->Process();
   } // for(...)
   // finally, we update maintree header if the steps use treesource/store instances:
   if(TGo4MainTree::Exists()) TGo4MainTree::Instance()->Update();
   return 0;
}


void TGo4AnalysisStepManager::UpdateStatus(TGo4AnalysisStatus* state)
{
TRACE((11,"TGo4AnalysisStepManager::UpdateStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state!=0)
      {
         state->SetFirstStepIndex(fiFirstStepIndex);
         state->SetLastStepIndex(fiLastStepIndex);
         state->SetStepChecking(fbStepCheckingMode);
         {
            //TGo4LockGuard  listguard(fxStepMutex);
               fxCurrentStep=0;
               fxStepIterator->Reset();
               while((fxCurrentStep= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
                {
                   TGo4AnalysisStepStatus* stepstate= fxCurrentStep->CreateStatus();
                   state->AddStepStatus(stepstate);
                } // while(fxCurrentStep..)
         } // lockguard
      } // if(state!=0)
   else
      {
         // no state to update
      }

}

void TGo4AnalysisStepManager::SetStatus(TGo4AnalysisStatus * state)
{
   TRACE((11,"TGo4AnalysisStepManager::SetStatus(TGo4AnalysisStatus*)",__LINE__, __FILE__));
   if(state!=0)
      {
         fiFirstStepIndex=state->GetFirstStepIndex();
         fiLastStepIndex=state->GetLastStepIndex();
         fbStepCheckingMode=state->IsStepChecking();
         // note: the step indices are not used for
         // initialization of analysis any more!
         // update internal states of steps:
            {
            //TGo4LockGuard  listguard(fxStepMutex);
               fxCurrentStep=0;
               fxStepIterator->Reset();
               while((fxCurrentStep= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
                {
                   const char* name= fxCurrentStep->GetName();
                   TGo4AnalysisStepStatus* stepstate= state->GetStepStatus(name);
                   fxCurrentStep->SetStatus(stepstate);

                } // while(fxCurrentStep..)
            } // lockguard
      } // if(state!=0)
   else
      {
         // no state to set
      }
}

void TGo4AnalysisStepManager::AutoSave()
{
TRACE((12,"TGo4AnalysisStepManager::AutoSave()",__LINE__, __FILE__));
   //
   //TGo4LockGuard  autoguard(fxAutoSaveMutex);
   TGo4Analysis::Instance()->Message(0,"Analysis Step Manager  --  AutoSaving....");
   TGo4AnalysisStep* step=0;
      {
      //TGo4LockGuard   listguard(fxStepMutex);
         fxStepIterator->Reset();
         while((step= dynamic_cast<TGo4AnalysisStep*>( fxStepIterator->Next() ) ) !=0)
            {
               step->StoreCalibration();
            }
      } // lock guard

   // write maintree to file if existing...
  if(TGo4MainTree::Exists())
               {
                  TGo4MainTree::Instance()->Write();
               }   else { }
}

Int_t TGo4AnalysisStepManager::IsErrorStopEnabled()
{
   return kTRUE; // workaraound, to be removed later!!! JA
   if(fxCurrentStep!=0)
       return fxCurrentStep->IsErrorStopEnabled();
   else
       return kTRUE;
}
