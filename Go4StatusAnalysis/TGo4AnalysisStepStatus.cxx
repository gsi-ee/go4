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

#include "TGo4AnalysisStepStatus.h"

#include "TROOT.h"

#include "TGo4Log.h"

TGo4AnalysisStepStatus::TGo4AnalysisStepStatus() :
   TGo4Status()
{
}

TGo4AnalysisStepStatus::TGo4AnalysisStepStatus(const char* name) :
   TGo4Status(name, "Go4 AnalysisStep Status Object")
{
   GO4TRACE((15,"TGo4AnalysisStepStatus::TGo4AnalysisStepStatus(const char*)",__LINE__, __FILE__));
}

TGo4AnalysisStepStatus::~TGo4AnalysisStepStatus()
{
   GO4TRACE((15,"TGo4AnalysisStepStatus::~TGo4AnalysisStepStatus()",__LINE__, __FILE__));
   delete fxSourceType; fxSourceType = nullptr;
   delete fxStoreType; fxStoreType = nullptr;
   delete fxProcessorType; fxProcessorType = nullptr;
}

void TGo4AnalysisStepStatus::Print(Option_t *) const
{
   PrintLine("++++ Analysis Step %s ++++", GetName());
   TROOT::IncreaseDirLevel();
   if (IsProcessEnabled()) {
      if (IsSourceEnabled()) {
         TGo4EventSourceParameter *spar = GetSourcePar();
         if (spar)
            spar->PrintParameter();
         else
            PrintLine("EventSource Type: undefined");
      } else {
         PrintLine("EventSource is disabled.");
      }

      TGo4EventProcessorParameter *ppar = GetProcessorPar();
      if (ppar)
         ppar->PrintParameter();

      if (IsStoreEnabled()) {
         TGo4EventStoreParameter *tpar = GetStorePar();
         if (tpar)
            tpar->PrintParameter();
         else
            PrintLine("EventStore Type: undefined");
      } else {
         PrintLine("EventStore is disabled.");
      }
      PrintLine("Process Status: %d", GetProcessStatus());
   } else {
      PrintLine("Step is disabled.");
   }
   TROOT::DecreaseDirLevel();
   PrintLine("---------------------------------------------- ");
}

void TGo4AnalysisStepStatus::SetSourcePar(TGo4EventSourceParameter* kind)
{
    if(fxSourceType) delete fxSourceType;
    if(kind)
        fxSourceType = dynamic_cast<TGo4EventSourceParameter*>(kind->Clone());
    else
        fxSourceType = nullptr;
}

TGo4EventSourceParameter* TGo4AnalysisStepStatus::TakeSourcePar()
{
   TGo4EventSourceParameter* res = fxSourceType;
   fxSourceType = nullptr;
   return res;
}

void TGo4AnalysisStepStatus::SetStorePar(TGo4EventStoreParameter* kind)
{
    if(fxStoreType) delete fxStoreType;
    if(kind)
        fxStoreType = dynamic_cast<TGo4EventStoreParameter*>(kind->Clone());
    else
        fxStoreType = nullptr;
}

void TGo4AnalysisStepStatus::SetProcessorPar(TGo4EventProcessorParameter* kind)
{
    if(fxProcessorType) delete fxProcessorType;
    if(kind)
        fxProcessorType = dynamic_cast<TGo4EventProcessorParameter*>(kind->Clone());
    else
        fxProcessorType = nullptr;
}
