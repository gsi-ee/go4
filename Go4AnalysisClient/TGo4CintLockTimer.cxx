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

#include "TGo4CintLockTimer.h"

#include "TThread.h"
#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClient.h"

UInt_t TGo4CintLockTimer::fguTHREADTIME = 50;//50

TGo4CintLockTimer::TGo4CintLockTimer (TGo4AnalysisClient* parent,Long_t msperiod, Bool_t mode) :
   TTimer(msperiod,mode), fbHaveLock(kFALSE), fxParent(parent)
{
}

TGo4CintLockTimer::~TGo4CintLockTimer()
{
   if(fbHaveLock) fxParent->UnLockAll();
}

Bool_t TGo4CintLockTimer::Notify()
{
   //TurnOff();
   if(fbHaveLock)
   {
      fxParent->UnLockAll();
      //std::cout <<"U TGo4CintLockTimer unlocked" << std::endl;
      fbHaveLock=kFALSE;
   }

   gSystem->Sleep(fguTHREADTIME);
   if(!fbHaveLock)
   {
      fxParent->LockAll();
      //std::cout <<"L TGo4CintLockTimer has lock" << std::endl;
      fbHaveLock=kTRUE;
   }
   Reset();
   //TurnOn();
   return kFALSE;
}





