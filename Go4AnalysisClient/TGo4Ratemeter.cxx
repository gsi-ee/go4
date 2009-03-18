#include "TGo4Ratemeter.h"

#include "TStopwatch.h"

const Double_t TGo4Ratemeter::fgdUPDATEINTERVAL= 1.0; // time in s


TGo4Ratemeter::TGo4Ratemeter()
: fuCurrentCount(0), fuLastCount(0), fdRate(0), fdAveRate(0),  fdTime(0), fdLastTime(0),fdDeltaTime(0)
{
   fxClock=new TStopwatch;
   fxClock->Stop();
}

TGo4Ratemeter::~TGo4Ratemeter()
{
   delete fxClock;
}

void TGo4Ratemeter::Reset()
{
   fdTime=0;
   fdDeltaTime=0;
   fdRate=0;
   fdAveRate=0;
   fuLastCount=0;
   fdLastTime=0;
   fuCurrentCount=0;
   fxClock->RealTime();
   fxClock->Start(kTRUE);
}

void TGo4Ratemeter::Update(Int_t increment)
{
   if(increment<0)
      {
         if(increment==-2)
            fdRate=1; // first update after start: dummy rate
         else
            fdRate=0; // case of stopped analysis: zero rate
         // keep last values of time, average rate, eventnumber
      }
   else
     {
         // normal operation
         fdTime=fxClock->RealTime();
         fxClock->Continue();
         fdDeltaTime=fdTime-fdLastTime; // difference since last update
         fuCurrentCount+= (UInt_t) increment;
         UInt_t deltacount=fuCurrentCount-fuLastCount;
         if( fdDeltaTime > TGo4Ratemeter::fgdUPDATEINTERVAL)
         {
          // we reach the update point, calculate the rate
            if(fdDeltaTime>0 && deltacount>0)
                  fdRate= deltacount/fdDeltaTime;
            else
                  fdRate=0; // reset rate in case of stopped analysis
            if(fdTime>0)
                  fdAveRate= fuCurrentCount/fdTime;
            else
               ; //fdAveRate=0;
            fdLastTime=fdTime; // remember updata time and count
            fuLastCount=fuCurrentCount;
      }
   else
      {
               // too short time since last update, we do not calculate...
      } // if( fdDeltaTime > TGo4Ratemeter::fgdUPDATEINTERVAL)
     } //if(increment<0)
   fbUpdateDone=kTRUE; // tell watch thread we did the update
}

Bool_t TGo4Ratemeter::TestUpdate()
{
   Bool_t rev=fbUpdateDone;
   fbUpdateDone=kFALSE;
   return rev;
}

