// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4AnalysisStatus.h"

#include "Riostream.h"

#include "TMutex.h"
#include "TObjArray.h"
#include "TROOT.h"

#include "TGo4LockGuard.h"
#include "TGo4Log.h"
#include "TGo4AnalysisStepStatus.h"

TGo4AnalysisStatus::TGo4AnalysisStatus() :
   TGo4Status("Go4 Default Analysis Status","Go4 Analysis Status Object"),
   fxStepArray(0),
   fxStepMutex(0),
   fxStepIterator(0),
   fbStepCheckingMode(1),
   fiFirstStepIndex(0),
   fiLastStepIndex(0),
   fiAutoSaveInterval(0),
   fiAutoSaveCompression(5),
   fbAutoSaveOverwrite(0),
   fbAutoSaveOn(1),
   fxAutoFileName(),
   fxConfigFileName()
{
  GO4TRACE((15,"TGo4AnalysisStatus::TGo4AnalysisStatus()",__LINE__, __FILE__));
}


TGo4AnalysisStatus::TGo4AnalysisStatus(const char* name) :
   TGo4Status(name,"Go4 Analysis Status Object"),
   fxStepArray(0),
   fxStepMutex(0),
   fxStepIterator(0),
   fbStepCheckingMode(1),
   fiFirstStepIndex(0),
   fiLastStepIndex(0),
   fiAutoSaveInterval(0),
   fiAutoSaveCompression(5),
   fbAutoSaveOverwrite(0),
   fbAutoSaveOn(1),
   fxAutoFileName(),
   fxConfigFileName()
{
  GO4TRACE((15,"TGo4AnalysisStatus::TGo4AnalysisStatus(const char*)",__LINE__, __FILE__));
  fxStepArray = new TObjArray;
  fxStepIterator = fxStepArray->MakeIterator();
  fxStepMutex = new TMutex;
}

TGo4AnalysisStatus::~TGo4AnalysisStatus()
{
  GO4TRACE((15,"TGo4AnalysisStatus::~TGo4AnalysisStatus()",__LINE__, __FILE__));
  delete fxStepMutex;
  delete fxStepIterator;
  fxStepArray->Delete();
  delete fxStepArray;
}

Int_t TGo4AnalysisStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   GO4TRACE((12,"TGo4AnalysisStatus::PrintStatus()",__LINE__, __FILE__));
   if(buflen<=0 && buffer!=0)
      {
         std::cout << "analysis status print has invalid buflen and nonzero buffer"<< std::endl;
         return 0;
      }
   Int_t size=0;
   Int_t locallen=512000;
   Text_t localbuf[512000];
   Text_t* current=localbuf;
   Int_t restlen=locallen;

   current=PrintBuffer(current,restlen, "----------------------------------------------  \n");
   current=PrintBuffer(current,restlen, "++++++ Status of %s ++++++\n", GetName());
   current=PrintBuffer(current,restlen, "First Analysis Step index: \t%d\n",GetFirstStepIndex());
   current=PrintBuffer(current,restlen, "Last Analysis Step index: \t%d\n",GetLastStepIndex());
   current=PrintBuffer(current,restlen, "Autosave Interval: \t\t%d s\n",GetAutoSaveInterval());
   current=PrintBuffer(current,restlen, "Autosave File: \t\t%s \n",GetAutoFileName());
   current=PrintBuffer(current,restlen, "Autosave File compression: \t%d \n",GetAutoSaveCompression());
   current=PrintBuffer(current,restlen, "Autosave overwrite mode: \t%d \n",IsAutoSaveOverwrite());
   current=PrintBuffer(current,restlen, "Autosave enabled: \t\t%d \n",IsAutoSaveOn());
   current=PrintBuffer(current,restlen, "----------------------------------------------  \n");
   TROOT::IncreaseDirLevel();
   ResetStepIterator();
   TGo4AnalysisStepStatus* step=0;
   while((step=NextStepStatus()) != 0)
      {
           Int_t delta=step->PrintStatus(current,restlen);
           restlen-=delta;
           current+= delta ;
      }
   TROOT::DecreaseDirLevel();
   current=PrintBuffer(current,restlen, "----------------------------------------------  \n");
   if(buffer==0)
      {
      std::cout << localbuf << std::endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

TGo4AnalysisStepStatus * TGo4AnalysisStatus::GetStepStatus(const char* name)
{
GO4TRACE((11,"TGo4Analysis::GetAnalysisStep(const char*)",__LINE__, __FILE__));
if(fxStepArray==0) return 0;
   TGo4AnalysisStepStatus* step=0;
      {
      TGo4LockGuard  listguard(fxStepMutex);
         step = dynamic_cast<TGo4AnalysisStepStatus*>( fxStepArray->FindObject(name) );
      }
   return step;
}

TGo4AnalysisStepStatus * TGo4AnalysisStatus::NextStepStatus()
{
GO4TRACE((11,"TGo4AnalysisStatus::NextStepStatus()",__LINE__, __FILE__));
if(fxStepIterator==0) return 0;
   TGo4AnalysisStepStatus* step=0;
      {
      TGo4LockGuard  listguard(fxStepMutex);
         step = dynamic_cast<TGo4AnalysisStepStatus*>( fxStepIterator->Next() );
      }
   return step;
}

void TGo4AnalysisStatus::ResetStepIterator()
{
   TGo4LockGuard listguard(fxStepMutex);
      delete fxStepIterator;
      if(fxStepArray)
         fxStepIterator=fxStepArray->MakeIterator();
      else
         fxStepIterator=0;
      // fxStepIterator->Reset();
}


Bool_t  TGo4AnalysisStatus::AddStepStatus(TGo4AnalysisStepStatus * next)
{
GO4TRACE((14,"TGo4AnalysisStatus::AddAnalysisStep(TGo4AnalysisStep*)",__LINE__, __FILE__));
//
if(fxStepArray==0) return kFALSE;
   Bool_t rev=kFALSE;
   if(next)
      {
      TGo4LockGuard  listguard(fxStepMutex);
          fxStepArray->AddLast(next);
          rev=kTRUE;
      } //  if(next) ; TGo4LockGuard
   else
      {
           rev=kFALSE;
//         TGo4Log::Debug(" Analysis: WARNING - did not add zero analysis step pointer to steplist ");
      }
   return rev;
}

void TGo4AnalysisStatus::ClearStepStatus()
{
  TGo4LockGuard  listguard(fxStepMutex);
  fxStepArray->Delete();
}


Int_t TGo4AnalysisStatus::GetNumberOfSteps()
{
    return fxStepArray==0 ? 0 : fxStepArray->GetLast()+1;
}


TGo4AnalysisStepStatus* TGo4AnalysisStatus::GetStepStatus(Int_t indx)
{
   if ((indx<0) || (indx>=GetNumberOfSteps())) return 0;
   return dynamic_cast<TGo4AnalysisStepStatus*> (fxStepArray->At(indx));
}

