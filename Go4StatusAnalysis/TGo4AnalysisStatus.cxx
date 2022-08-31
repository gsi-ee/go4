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

#include "TGo4AnalysisStatus.h"

#include "TMutex.h"
#include "TObjArray.h"

#include "TGo4LockGuard.h"
#include "TGo4AnalysisStepStatus.h"
#include "TROOT.h"


TGo4AnalysisStatus::TGo4AnalysisStatus() :
   TGo4Status("Go4 Default Analysis Status","Go4 Analysis Status Object")
{
}


TGo4AnalysisStatus::TGo4AnalysisStatus(const char *name) :
   TGo4Status(name,"Go4 Analysis Status Object")
{
   fxStepArray = new TObjArray;
   fxStepIterator = fxStepArray->MakeIterator();
   fxStepMutex = new TMutex;
}

TGo4AnalysisStatus::~TGo4AnalysisStatus()
{
   delete fxStepMutex; fxStepMutex = nullptr;
   delete fxStepIterator; fxStepIterator = nullptr;
   if (fxStepArray) fxStepArray->Delete();
   delete fxStepArray; fxStepArray = nullptr;
}

void TGo4AnalysisStatus::Print(Option_t*) const
{
   PrintLine("----------------------------------------------");
   PrintLine("++++++ Status of %s ++++++", GetName());
   PrintLine("First Analysis Step index: \t%d", GetFirstStepIndex());
   PrintLine("Last Analysis Step index: \t%d", GetLastStepIndex());
   PrintLine("Autosave Interval: \t\t%d s", GetAutoSaveInterval());
   PrintLine("Autosave File: \t\t%s", GetAutoFileName());
   PrintLine("Autosave File compression: \t%d", GetAutoSaveCompression());
   PrintLine("Autosave overwrite mode: \t%d", IsAutoSaveOverwrite());
   PrintLine("Autosave enabled: \t\t%d",IsAutoSaveOn());
   PrintLine("----------------------------------------------");
   TROOT::IncreaseDirLevel();

   Int_t num = GetNumberOfSteps();
   for (int indx = 0; indx < num; ++indx) {
      auto step = GetStepStatus(indx);
      if (step) step->Print();
   }

   TROOT::DecreaseDirLevel();
   PrintLine("----------------------------------------------");
}

TGo4AnalysisStepStatus * TGo4AnalysisStatus::GetStepStatus(const char *name)
{
   if(!fxStepArray) return nullptr;

   TGo4LockGuard  listguard(fxStepMutex);
   return dynamic_cast<TGo4AnalysisStepStatus*>( fxStepArray->FindObject(name) );
}

TGo4AnalysisStepStatus * TGo4AnalysisStatus::NextStepStatus()
{
   if(!fxStepIterator) return nullptr;
   TGo4LockGuard  listguard(fxStepMutex);
   return dynamic_cast<TGo4AnalysisStepStatus*>( fxStepIterator->Next() );
}

void TGo4AnalysisStatus::ResetStepIterator()
{
   TGo4LockGuard listguard(fxStepMutex);
   delete fxStepIterator;
   if(fxStepArray)
      fxStepIterator = fxStepArray->MakeIterator();
   else
      fxStepIterator = nullptr;
}


Bool_t TGo4AnalysisStatus::AddStepStatus(TGo4AnalysisStepStatus * next)
{
   if(!fxStepArray) return kFALSE;
   if(next) {
      TGo4LockGuard  listguard(fxStepMutex);
      fxStepArray->AddLast(next);
      return kTRUE;
   }
   return kFALSE;
}

void TGo4AnalysisStatus::ClearStepStatus()
{
  TGo4LockGuard  listguard(fxStepMutex);
  fxStepArray->Delete();
}


Int_t TGo4AnalysisStatus::GetNumberOfSteps() const
{
   return !fxStepArray ? 0 : fxStepArray->GetLast()+1;
}


TGo4AnalysisStepStatus* TGo4AnalysisStatus::GetStepStatus(Int_t indx) const
{
   if ((indx < 0) || (indx >= GetNumberOfSteps())) return nullptr;
   return dynamic_cast<TGo4AnalysisStepStatus*> (fxStepArray->At(indx));
}
