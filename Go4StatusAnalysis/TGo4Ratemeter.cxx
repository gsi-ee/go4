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

#include "TGo4Ratemeter.h"

const Double_t TGo4Ratemeter::fgdUPDATEINTERVAL= 1.0; // time in s


TGo4Ratemeter::TGo4Ratemeter() :
   TNamed(),
   fbRunning(kFALSE),
   fuCurrentCount(0),
   fdRate(0),
   fdTime(0),
   fxDateString(),
   fxEventSource(),
   fuLastCount(0),
   fLastTm(),
   fbUpdateDone(kFALSE),
   fuNextCheckCnt(0),
   fuCheckInterval(1),
   fdUpdateInterval(fgdUPDATEINTERVAL)
{
}

TGo4Ratemeter::~TGo4Ratemeter()
{
}

void TGo4Ratemeter::UpdateFrom(const TGo4Ratemeter* r)
{
   // update only persistent members

   if (r==0) return;
   fbRunning = r->fbRunning;
   fuCurrentCount = r->fuCurrentCount;
   fdRate = r->fdRate;
   fdTime = r->fdTime;
   fxDateString = r->fxDateString;
   fxEventSource = r->fxEventSource;
}

void TGo4Ratemeter::Reset()
{
   fbRunning = kFALSE;
   fdTime = 0;
   fdRate = 0;
   fuLastCount = 0;
   fuCurrentCount = 0;

   fLastTm.Set();
   fbUpdateDone = kFALSE;
   fuNextCheckCnt = 0;
   fuCheckInterval = 1; // in the beginning check every event
}

Bool_t TGo4Ratemeter::Update(Int_t increment)
{
   if(increment<0) {
      if(increment==-2)
         fdRate=1; // first update after start: dummy rate
      else
         fdRate=0; // case of stopped analysis: zero rate
      // keep last values of time, average rate, eventnumber
      fbUpdateDone = kTRUE; // tell watch thread we did the update
      return kTRUE;
   }

   fuCurrentCount += (UInt_t) increment;

   // check time if update count specified as 0
   if ((increment>0) && (fuCurrentCount < fuNextCheckCnt)) return kFALSE;

   TTimeStamp now;

   Double_t dist = now.AsDouble() - fLastTm.AsDouble();

   if (dist<fdUpdateInterval) {
      fuNextCheckCnt = fuCurrentCount + fuCheckInterval;
      return kFALSE;
   }

   fdRate = (fuCurrentCount - fuLastCount) / dist;

   // accumulate running time only when running flag set
   if (fbRunning)
      fdTime += dist;

   fLastTm = now;
   fuLastCount = fuCurrentCount;

   double check = fdRate * fdUpdateInterval * 0.1; // check about 10 times before next update
   if (check<2) fuCheckInterval = 1; else
   if (check>1000) fuCheckInterval = 1000; else fuCheckInterval = (ULong64_t) check;

   fuNextCheckCnt = fuCurrentCount + fuCheckInterval;

   fbUpdateDone = kTRUE; // tell watch thread we did the update
   return kTRUE;
}

Bool_t TGo4Ratemeter::TestUpdate()
{
   Bool_t rev = fbUpdateDone;
   fbUpdateDone = kFALSE;
   return rev;
}
