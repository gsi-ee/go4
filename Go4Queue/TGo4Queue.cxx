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

#include "TGo4Queue.h"

#include "TList.h"
#include "TMutex.h"
#include "TCondition.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4RuntimeException.h"

TGo4Queue::TGo4Queue(const char* name) :
    TNamed(name ? name : "Default Queue", "This is a Go4 Queue"),
    fiEntries(0),
    fiMaxEntries(100),
    fbWakeUpCall(kFALSE)
{
   fxMutex = new TMutex;
   fxCondition = new TCondition;
   fxList = new TList;
}

TGo4Queue::~TGo4Queue()
{
   delete fxList; fxList = 0;
   delete fxCondition; fxCondition = 0;
   delete fxMutex; fxMutex = 0;
}

void TGo4Queue::Clear(Option_t* opt)
{
   TGo4LockGuard qguard(fxMutex);
   fxList->Clear();
}

TObject* TGo4Queue::Wait()
{
   if(IsEmpty()) {
      //std::cout <<"WWWWWWWWWWWWWWWWWW --- TGo4Queue "<< GetName() <<" is in Condition Wait..." << std::endl;
      fxCondition->Wait();
      if(fbWakeUpCall) {
         fbWakeUpCall = kFALSE;
         return 0; // signal by Wake(), give null back!
      }
   }
   return Next();
}

TObject* TGo4Queue::Next()
{
   TGo4LockGuard qguard(fxMutex);
   return (fxList->Remove(fxList->LastLink()) );
}

void TGo4Queue::Add(TObject* ob)
{
   {
    TGo4LockGuard qguard(fxMutex);
       if(fxList->GetSize() <= fiMaxEntries)
         {
            fxList->AddFirst(ob);
            //std::cout <<"QA Queue " <<GetName() <<" added obj, size="<<fxList->GetSize()<< std::endl;
         }
       else
          {
            //std::cout <<" Queue Add Error: queue "<< GetName() <<" is full !!!"<< std::endl;
            throw TGo4RuntimeException();
          }
   } //TGo4LockGuard qguard(fxMutex);
   fxCondition->Signal();
}

Bool_t TGo4Queue::IsEmpty()
{
   TGo4LockGuard qguard(fxMutex);
   return fxList->IsEmpty();
}

void TGo4Queue::Wake()
{
   //std::cout <<"TGo4Queue::Wake() in "<<GetName() << std::endl;
   fbWakeUpCall = kTRUE;
   fxCondition->Signal();
}
