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

#include "TGo4AnalysisStep.h"

#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisStepStatus.h"
#include "TGo4AnalysisStepException.h"

#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4EventProcessorParameter.h"

#include "TGo4EventCalibration.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4FileStoreParameter.h"

#include "TGo4EventElement.h"
#include "TGo4EventFactory.h"
#include "TGo4EventStore.h"

TGo4AnalysisStep::TGo4AnalysisStep(const char* name, TGo4EventFactory* eventfactory, TGo4EventSourceParameter* sourcetype, TGo4EventStoreParameter* storetype, TGo4EventProcessorParameter* processortype)
:TNamed(name,"This is a Go4 analysis step"),
   fxPrevious(nullptr), fxEventStore(nullptr), fxEventSource(nullptr), fxEventProcessor(nullptr),
   fxInputEvent(nullptr), fxOutputEvent(nullptr),
   fxSourceType(sourcetype), fxStoreType(storetype), fxProcessorType(processortype),
   fbSourceEnabled(kFALSE), fbSourceImplemented(kFALSE),
   fbStoreEnabled(kFALSE), fbStoreImplemented(kFALSE),
   fbProcessEnabled(kTRUE),
   fbErrorStopEnabled(kFALSE), fbErrorStopped(kFALSE)

{
   GO4TRACE((15,"TGo4AnalysisStep::TGo4AnalysisStep(const char*, TGo4EventFactory*)",__LINE__, __FILE__));
   fxOwner = TGo4Analysis::Instance(); // note: we always have the analysis framework!
   fxEventFactory = eventfactory;
   if(fxStoreType)
      fxStoreType->SetTitle(GetName()); // set title of eventstore parameter to analysis step name
                                        // this might be used to indicate tree name
   SetStatusMessage(" Analysis Step is created ");
}


TGo4AnalysisStep::TGo4AnalysisStep()
:TNamed("Default Analysis Step","This is a Go4 analysis step"),
   fxPrevious(nullptr), fxEventStore(nullptr), fxEventSource(nullptr), fxEventProcessor(nullptr),
   fxInputEvent(nullptr), fxOutputEvent(nullptr),
   fxSourceType(0), fxStoreType(0), fxProcessorType(0),
   fbSourceEnabled(kFALSE), fbSourceImplemented(kFALSE),
   fbStoreEnabled(kFALSE), fbStoreImplemented(kFALSE),
   fbProcessEnabled(kTRUE),
   fbErrorStopEnabled(kFALSE), fbErrorStopped(kFALSE)
{
   GO4TRACE((15,"TGo4AnalysisStep::TGo4AnalysisStep()",__LINE__, __FILE__));

   fxOwner = nullptr;
   fxEventFactory = nullptr;
}

TGo4AnalysisStep::~TGo4AnalysisStep()
{
   GO4TRACE((15,"TGo4AnalysisStep::~TGo4AnalysisStep()",__LINE__, __FILE__));
   if(fxOwner) {
      Close();
      delete fxEventFactory;
      delete fxSourceType;
      delete fxStoreType;
      delete fxProcessorType;
   }
}

void TGo4AnalysisStep::InitEventClasses()
{
   GO4TRACE((15,"TGo4AnalysisStep::InitEventClasses()",__LINE__, __FILE__));
   // note: the order is important here, from source to drain...
   if(fbProcessEnabled) {
      NewEventSource(fxSourceType);
      NewInputEvent();
      NewEventProcessor(fxProcessorType);
      NewOutputEvent();
      NewEventStore(fxStoreType);
   }
}

void TGo4AnalysisStep::Process()
{
   GO4TRACE((12,"TGo4AnalysisStep::Process()",__LINE__, __FILE__));
   if(!fbProcessEnabled) return;
   if(!fxEventProcessor) {
      SetStatusMessage("! AnalysisStep -- Process Error: no event processor !");
      throw TGo4AnalysisStepException(this);
   }

   TGo4EventElement* input = nullptr;
   fiProcessStatus = 0;
   ////////// source part:
   if (fbSourceEnabled && fxEventSource) {
      // fill input event from own source
      fxInputEvent->SetEventSource(fxEventSource);
      if(!fxEventProcessor->IsKeepInputEvent())
         fxInputEvent->Fill(); // do not overwrite current input event contents
      input = fxInputEvent;
   } else
     // get input event structure from previous step
     input = fxOwner->GetOutputEvent();


   ///////////// processor part:
   if(!fxEventProcessor->IsKeepInputEvent())
       fxEventProcessor->SetInputEvent(input); // do not change current input event reference

   //fxEventProcessor->SetKeepInputEvent(kFALSE); // automatic reset of keep input flags before processor execution

   if(fxOutputEvent) {
      fxOutputEvent->SetEventSource(fxEventProcessor);
      if(fxEventProcessor->IsKeepOutputEvent()) fxOutputEvent->SetKeepContents(kTRUE); // suppress inplicit Clear()
      //fxEventProcessor->SetKeepOutputEvent(kFALSE); // automatic reset of keep output flags before processor execution

      fxOutputEvent->Fill(); // this calls processor build event

      fxOwner->SetOutputEvent(fxOutputEvent);
      if(fbStoreEnabled && fxEventStore && fxOutputEvent->IsValid() && !fxEventProcessor->IsKeepOutputEvent())
         fxEventStore->Store(fxOutputEvent);
   } else {
      SetStatusMessage("! AnalysisStep -- Process Error: no output event !");
      throw TGo4AnalysisStepException(this);
   }
}

void TGo4AnalysisStep::Close()
{
   GO4TRACE((14,"TGo4AnalysisStep::Close()",__LINE__, __FILE__));
   CloseEventStore();
   DeleteOutputEvent();
   CloseEventProcessor();
   DeleteInputEvent();
   CloseEventSource();
}

void TGo4AnalysisStep::CloseEventStore()
{
   GO4TRACE((14,"TGo4AnalysisStep::CloseEventStore()",__LINE__, __FILE__));
   if(fxEventStore) {
      TTree* atree = fxEventStore->GetTree();
      fxOwner->RemoveTree(atree);
      fxOwner->RemoveEventStore(fxEventStore);
      delete fxEventStore;
      fxEventStore = nullptr;
      fbStoreImplemented=kFALSE;
   }
}


void TGo4AnalysisStep::CloseEventSource()
{
   GO4TRACE((14,"TGo4AnalysisStep::CloseEventSource()",__LINE__, __FILE__));
   if(fxEventSource) {
      fxOwner->RemoveEventSource(fxEventSource);
      delete fxEventSource;
      fxEventSource = nullptr;
      fbSourceImplemented=kFALSE;
   }
}

void TGo4AnalysisStep::CloseEventProcessor()
{
   GO4TRACE((14,"TGo4AnalysisStep::CloseEventProcessor()",__LINE__, __FILE__));
   if(fxEventProcessor) {
      fxOwner->RemoveEventProcessor(fxEventProcessor);
      delete fxEventProcessor;
      fxEventProcessor = nullptr;
   }
}

void TGo4AnalysisStep::DeleteInputEvent()
{
   GO4TRACE((14,"TGo4AnalysisStep::DeleteInputEvent()",__LINE__, __FILE__));
   if(fxInputEvent) {
      fxOwner->RemoveEventStructure(fxInputEvent);
      delete fxInputEvent;
      fxInputEvent = nullptr;
   }
}

void TGo4AnalysisStep::DeleteOutputEvent()
{
   GO4TRACE((14,"TGo4AnalysisStep::DeleteOutputEvent()",__LINE__, __FILE__));
   if(fxOutputEvent) {
      fxOwner->RemoveEventStructure(fxOutputEvent);
      delete fxOutputEvent;
      fxOutputEvent = nullptr;
   }
}

void TGo4AnalysisStep::StoreCalibration()
{
   GO4TRACE((14,"TGo4AnalysisStep::StoreCalibration()",__LINE__, __FILE__));
   TGo4EventCalibration* cali = nullptr;
   if(fxEventProcessor)
      cali = fxEventProcessor->GetCalibration();
   if (fxEventStore && fbStoreEnabled)
      fxEventStore->Store(cali);
}

Int_t TGo4AnalysisStep::Store(TGo4Parameter* cali)
{
   if (fxEventStore && fbStoreEnabled)
      return fxEventStore->Store(cali);
   return -1;
}

Int_t TGo4AnalysisStep::Store(TGo4Condition* conny)
{
   if (fxEventStore && fbStoreEnabled)
      return fxEventStore->Store(conny);
   return -1;
}

Int_t TGo4AnalysisStep::Store(TGo4Fitter* fitter)
{
   if (fxEventStore && fbStoreEnabled)
      return fxEventStore->Store(fitter);
   return -1;
}

Int_t TGo4AnalysisStep::Store(TFolder* fold)
{
   if (fxEventStore && fbStoreEnabled)
      return fxEventStore->Store(fold);
   return -1;
}


Bool_t TGo4AnalysisStep::IsMatchingPrevious()
{
   GO4TRACE((14,"TGo4AnalysisStep::IsMatchingPrevious(TGo4AnalysisStep*)",__LINE__, __FILE__));
   if(!IsProcessEnabled()) return kTRUE;
      // only check if this step is active, otherwise no event classes are initialized!

   if (!fxPrevious) return kTRUE;

   TGo4EventElement* prevevent = fxPrevious->GetOutputEvent();
   if (!prevevent) return kTRUE;

   return !strcmp(prevevent->ClassName(),fxInputEvent->ClassName()) &&
          !strcmp(prevevent->GetName(),fxInputEvent->GetName());
}

const char* TGo4AnalysisStep::GetEventStoreName()
{
   GO4TRACE((12,"TGo4AnalysisStep::GetEventStoreName()",__LINE__, __FILE__));

   return !fxEventStore ? nullptr : fxEventStore->GetName();
}

const char* TGo4AnalysisStep::GetEventSourceName()
{
   return !fxEventSource ? nullptr : fxEventSource->GetActiveName();
}

void TGo4AnalysisStep::NewEventSource(TGo4EventSourceParameter * kind)
{
   GO4TRACE((12,"TGo4AnalysisStep::NewEventSource(Int_t)",__LINE__, __FILE__));
   const char* sourcename = "";
   if (kind) sourcename = kind->GetName();

   CloseEventSource();
   if(fxEventFactory) {
      gROOT->cd(); // make sure that any histograms of source are not assigned
      // to a file possibly opened before...
      if(fbSourceEnabled) {
         if(kind && kind->InheritsFrom("TGo4TreeSourceParameter")) {
            SetStatusMessage("Did not init TGo4TreeSource, please use TGo4FileSource instead !");
            throw TGo4AnalysisStepException(this);
         }
         if(fxPrevious) {
            // check if previous step would overwrite our event source:
            const char* evstorename=fxPrevious->GetEventStoreName();
            if(evstorename && !strcmp(evstorename, sourcename) && fxPrevious->IsStoreEnabled()) {
               TGo4Analysis::Instance()->Message(2,"AnalysisStep %s: Event source %s not created: previous store of same name !",
                     GetName(), sourcename);
               fxEventSource = nullptr;
            } else {
               fxEventSource = fxEventFactory->CreateEventSource(kind);
            } // if(evstorename!= ... )
         } else {
            fxEventSource=fxEventFactory->CreateEventSource(kind);
         } // if(fxPrevious)
         gROOT->cd(); // back to global directory; rootfile as eventsource would be cd here!
      } else {
         fxEventSource = nullptr;// if not enabled, do not open files or connect...
      } // if(fbSourceEnabled)

      if(fxEventSource) {
         fbSourceImplemented=kTRUE;
         fxOwner->AddEventSource(fxEventSource); // reference to eventsource class
         // we have to set reference to new source in input event:
         if(fxInputEvent) {
            fxInputEvent->SetEventSource(fxEventSource);
            fxInputEvent->Init(); // this should check if source is correct
         }
      } else {
         TGo4Analysis::Instance()->Message(0,"AnalysisStep %s : No EventSource instance created by EventFactory",
               GetName());
         fbSourceImplemented=kFALSE;
      } // if(fxEventSource)
   } else {
      TGo4Analysis::Instance()->Message(3,"AnalysisStep -- NewEventSource Error: no event factory ");
   } // if(fxEventFactory)
   // for InitEventClasses we must store the source type:
   SetEventSource(kind);
}

void TGo4AnalysisStep::NewEventStore(TGo4EventStoreParameter * kind)
{
   GO4TRACE((12,"TGo4AnalysisStep::NewEventStore(Int_t)",__LINE__, __FILE__));
   CloseEventStore();
   if(fxEventFactory) {
      gROOT->cd(); // make sure that store is not assigned
      // to a file possibly opened before...
      if(fbStoreEnabled) {
         if(kind && kind->InheritsFrom("TGo4TreeStoreParameter"))
         {
            SetStatusMessage("!  Did not init TGo4TreeStore, please use TGo4FileStore instead !");
            throw TGo4AnalysisStepException(this);
         }

         fxEventStore = fxEventFactory->CreateEventStore(kind);
         //kind->Print();
      }

      if(fxEventStore) {
         fbStoreImplemented=kTRUE;
         TTree* atree= fxEventStore->GetTree();
         fxOwner->AddTree(atree); // reference to tree
         fxOwner->AddEventStore(fxEventStore); // reference to storage class
      } else {
         TGo4Analysis::Instance()->Message(0,"AnalysisStep %s : No EventStore instance created by EventFactory",
               GetName() );
         fbStoreImplemented=kFALSE;
      }
   } else {
      TGo4Analysis::Instance()->Message(3,"AnalysisStep -- NewEventStore Error: no event factory !");
   }
   gROOT->cd(); // make sure that objects assigned after us are in global dir!
   // for InitEventClasses :
   SetEventStore(kind);
}

void TGo4AnalysisStep::NewEventProcessor(TGo4EventProcessorParameter * kind)
{
   GO4TRACE((12,"TGo4AnalysisStep::NewEventProcessor(Int_t)",__LINE__, __FILE__));
   CloseEventProcessor();
   if(fxEventFactory) {
      gROOT->cd(); // make sure that any histograms of processor are not assigned
      // to a file possibly opened before...
      fxEventProcessor=fxEventFactory->CreateEventProcessor(kind);
      fxOwner->AddEventProcessor(fxEventProcessor);
      if(fxEventProcessor)
         fxEventProcessor->SetInputEvent(fxInputEvent);
      // method of processor might init the types here...
      if(fxOutputEvent) {
         fxOutputEvent->SetEventSource(fxEventProcessor);
         fxOutputEvent->Init(); // should check event source
      }
   } else {
      TGo4Analysis::Instance()->Message(3,"AnalysisStep -- NewEventProcessor Error: no event factory !");
   }
   // for InitEventClasses :
   SetEventProcessor(kind);
}
TGo4AnalysisStepStatus * TGo4AnalysisStep::CreateStatus()
{
   GO4TRACE((11,"TGo4AnalysisStep::CreateStatus()",__LINE__, __FILE__));
   TGo4AnalysisStepStatus* state= new TGo4AnalysisStepStatus( GetName() );
   state->SetProcessorPar(fxProcessorType);
   if(!fxSourceType) {
      // if user has defined the source as null, we provide dummy for gui
      fxSourceType= new TGo4FileSourceParameter("NoInputDefined");
      fbSourceEnabled=kFALSE;
   }
   state->SetSourcePar(fxSourceType);
   state->SetSourceEnabled(fbSourceEnabled);
   if(!fxStoreType) {
      fxStoreType = new TGo4FileStoreParameter("NoOutputDefined");
      fbStoreEnabled=kFALSE;
   }
   state->SetStorePar(fxStoreType);
   state->SetStoreEnabled(fbStoreEnabled);
   state->SetProcessEnabled(fbProcessEnabled);
   state->SetErrorStopEnabled(fbErrorStopEnabled);
   state->SetErrorStopped(fbErrorStopped);
   state->SetProcessStatus(fiProcessStatus);
   return state;
}

void TGo4AnalysisStep::NewInputEvent()
{
   GO4TRACE((12,"TGo4AnalysisStep::NewInputEvent()",__LINE__, __FILE__));
   DeleteInputEvent();
   if(fxEventFactory) {
      fxInputEvent = fxEventFactory->CreateInputEvent();
      fxOwner->AddEventStructure(fxInputEvent);
      if(fxInputEvent)  {
         fxInputEvent->SetEventSource(fxEventSource);
         fxInputEvent->Init(); // this should check if source is correct
      }
   } else {
      TGo4Analysis::Instance()->Message(3,"AnalysisStep -- NewInputEvent Error: no event factory !");
   }
}

void TGo4AnalysisStep::NewOutputEvent()
{
   GO4TRACE((12,"TGo4AnalysisStep::NewOutputEvent()",__LINE__, __FILE__));
   DeleteOutputEvent();
   if(fxEventFactory) {
      fxOutputEvent=fxEventFactory->CreateOutputEvent();
      fxOwner->AddEventStructure(fxOutputEvent);
      if(fxOutputEvent) {
         fxOutputEvent->SetEventSource(fxEventProcessor);
         fxOutputEvent->Init(); // should check event source
      }
   } else {
      TGo4Analysis::Instance()->Message(3,"AnalysisStep -- NewOutputEvent Error: no event factory !");
   }
}


void TGo4AnalysisStep::SetStatus(TGo4AnalysisStepStatus * state)
{
   GO4TRACE((11,"TGo4AnalysisStep::SetStatus(TGo4AnalysisStepStatus*)",__LINE__, __FILE__));
   if(state) {
      SetEventProcessor(state->GetProcessorPar());
      SetEventSource(state->GetSourcePar());
      SetEventStore(state->GetStorePar());
      SetProcessEnabled(state->IsProcessEnabled());
      SetSourceEnabled(state->IsSourceEnabled());
      SetStoreEnabled(state->IsStoreEnabled());
      SetErrorStopEnabled(state->IsErrorStopEnabled());
      fbErrorStopped=kFALSE;   // reset run status parameters to default
      fiProcessStatus=0;      // dito
   }
}

void TGo4AnalysisStep::SetEventProcessor(TGo4EventProcessorParameter* kind)
{
   if(kind==fxProcessorType) return; // avoid deleting valid parameter
   if(fxProcessorType) delete fxProcessorType;
   if(kind)
       fxProcessorType=dynamic_cast<TGo4EventProcessorParameter*>(kind->Clone());
   else
       fxProcessorType=nullptr;
}

void TGo4AnalysisStep::SetEventSource(TGo4EventSourceParameter* kind)
{
   if(kind==fxSourceType) return; // avoid deleting valid parameter
   if(fxSourceType) delete fxSourceType;
   if(kind)
       fxSourceType = dynamic_cast<TGo4EventSourceParameter*>(kind->Clone());
   else
       fxSourceType = nullptr;
}

Bool_t TGo4AnalysisStep::IsEventSourceParam() const
{
   return fxSourceType != nullptr;
}

void TGo4AnalysisStep::SetEventStore(TGo4EventStoreParameter* kind)
{
   if(kind == fxStoreType) return; // avoid deleting valid parameter
   if (fxStoreType)
      delete fxStoreType;
   if (kind)
      fxStoreType = dynamic_cast<TGo4EventStoreParameter *>(kind->Clone());
   else
      fxStoreType = nullptr;
   if (fxStoreType)
      fxStoreType->SetTitle(GetName());
   // set title of eventstore parameter to analysis step name
   // this might be used to indicate tree name
}

Bool_t TGo4AnalysisStep::IsEventStoreParam() const
{
   return fxStoreType != nullptr;
}
