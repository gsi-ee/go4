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

#include "TMeshUnpackProc.h"

#include "TGo4MbsEvent.h"
#include "TGo4Log.h"

#include "TMeshRawEvent.h"
#include "TMeshB1InputEvent.h"
#include "TMeshB2InputEvent.h"

//***********************************************************
TMeshUnpackProc::TMeshUnpackProc() :
   TGo4EventProcessor("Unpacker")
{
}
//***********************************************************
// this one is used in TXXXUnpackFact.cxx
TMeshUnpackProc::TMeshUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
   TGo4Log::Info("TMeshUnpackProc: Create %s", name);
}

//***********************************************************
TMeshUnpackProc::~TMeshUnpackProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshUnpackProc::Unpack(TMeshRawEvent* poutevt)
{
   if(poutevt==0) return;
   // process subcomponents independently
   // note: this will run subevent loop each time, which is not performance optimized in this example
   // however, for the general case this unpacker decoupling is more suitable
   UnpackBranch1(&(poutevt->fxSub1));
   UnpackBranch2(&(poutevt->fxSub2));
   UnpackBranch3(&(poutevt->fxSub3));
   poutevt->SetValid(kTRUE); // to store
}


void TMeshUnpackProc::UnpackBranch1(TMeshB1InputEvent* poutevt)
{
   fInput = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
   if(fInput==0) {
      TGo4Log::Error("UnpackBranch1: no input event !");
      return;
   }

   Int_t index(0), value(0), lwords(0);
   Int_t *pdata(0);
   TGo4MbsSubEvent* psubevt(0);
   fInput->ResetIterator();
   while ((psubevt = fInput->NextSubEvent()) != 0) // subevent loop
   {
      if( psubevt->GetSubcrate() == 1)
      {
         pdata=psubevt->GetDataField();
         lwords= (psubevt->GetDlen() -2) * sizeof(Short_t)/sizeof(Int_t);
         if(lwords >= 8) lwords=8; // take only first 8 lwords
         for(Int_t i = 0; i<lwords; ++i)
         {
            index =  *pdata&0xfff;      // in case low word is index
            value = *pdata;             // save for 2d histogram
            //value = (*pdata>>16)&0xfff; // in case high word is data
            if((value != 0) && (index>=0)) {
               poutevt->fiCrate1[i] = value; // fill output event
            }
            pdata++;
         } // for SEW LW
      } // if (subcrate)

   }  // while
   poutevt->SetValid(kTRUE); // to store
}

void TMeshUnpackProc::UnpackBranch2(TMeshB2InputEvent* poutevt)
{
  fInput = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
  if(fInput==0) {
     TGo4Log::Error("UnpackBranch2: no input event !");
     return;
  }

  Int_t index(0), value(0), lwords(0);
  Int_t *pdata(0);
  TGo4MbsSubEvent* psubevt(0);
  fInput->ResetIterator();
  while ((psubevt = fInput->NextSubEvent()) != 0) // subevent loop
  {
     if( psubevt->GetSubcrate() == 2)
     {
        pdata=psubevt->GetDataField();
        lwords= (psubevt->GetDlen() -2) * sizeof(Short_t)/sizeof(Int_t);
        if(lwords >= 8) lwords=8;
        for(Int_t i = 0; i<lwords; ++i)
        {
           index = *pdata&0xfff;
           //value=(*pdata>>16)&0xfff;
           value = *pdata;
           if((value != 0) && (index>=0)) {
              poutevt->fiCrate2[i] = value;
           }
           pdata++;
        } // for SEW LW
     } // if (subcrate)
  }  // while
  poutevt->SetValid(kTRUE); // to store
}

void TMeshUnpackProc::UnpackBranch3(TMeshB3InputEvent* poutevt)
{
  fInput = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
  if(fInput==0) {
     TGo4Log::Error("UnpackBranch3: no input event !");
     return;
  }

  Int_t index(0), value(0), lwords(0);
  Int_t *pdata(0);
  TGo4MbsSubEvent* psubevt(0);
  fInput->ResetIterator();
  while ((psubevt = fInput->NextSubEvent()) != 0) // subevent loop
  {
     if( psubevt->GetSubcrate() == 3)
     {
        pdata=psubevt->GetDataField();
        lwords= (psubevt->GetDlen() -2) * sizeof(Short_t)/sizeof(Int_t);
        if(lwords >= 8) lwords=8; // take only first 8 lwords
        for(Int_t i = 0; i<lwords; ++i)
        {
           index =  *pdata&0xfff;      // in case low word is index
           //value = (*pdata>>16)&0xfff; // in case high word is data
           value = *pdata;
           if((value != 0) && (index>=0)) {
              poutevt->fiCrate3[i] = value; // fill output event
           }
           pdata++;
        } // for SEW LW
     } // if (subcrate)
  }  // while
  poutevt->SetValid(kTRUE); // to store
}
