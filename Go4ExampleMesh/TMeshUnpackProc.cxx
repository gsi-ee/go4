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

#include "TMeshUnpackProc.h"

#include "Riostream.h"

#include "TGo4MbsEvent.h"
//#include "TGo4Analysis.h"
//#include "TGo4EventElement.h"


#include "TMeshRawEvent.h"
#include "TMeshB1InputEvent.h"
#include "TMeshB2InputEvent.h"

//***********************************************************
TMeshUnpackProc::TMeshUnpackProc()
  : TGo4EventProcessor("Unpacker")
{
}
//***********************************************************
// this one is used in TXXXUnpackFact.cxx
TMeshUnpackProc::TMeshUnpackProc(const char* name)
  : TGo4EventProcessor(name)
{
  cout << "**** TMeshUnpackProc: Create" << endl;


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
  TGo4MbsSubEvent* psubevt;
  Int_t index=0;
  Int_t value=0;
  Int_t lwords;
  Int_t *pdata;
  fInput    = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
  if(fInput)
    {

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
                 value = *pdata; // save for 2d histogram
                 //value = (*pdata>>16)&0xfff; // in case high word is data
                 if(value != 0)
                  {
                     poutevt->fiCrate1[i] = value; // fill output event
                  }
                pdata++;
               } // for SEW LW
             } // if (subcrate)

         }  // while
      poutevt->SetValid(kTRUE); // to store
    } // if(fInput)
  else    cout << "UnpackBranch1: no input event !"<< endl;
}

void TMeshUnpackProc::UnpackBranch2(TMeshB2InputEvent* poutevt)
{
  TGo4MbsSubEvent* psubevt;
  Int_t index=0;
  Int_t value=0;
  Int_t lwords;
  Int_t *pdata;
  fInput    = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
  if(fInput)
    {

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
                    index=*pdata&0xfff;
                    //value=(*pdata>>16)&0xfff;
                    value=*pdata;
                    if(value != 0)
                      {
                        poutevt->fiCrate2[i] = value;
                      }
                    pdata++;
                  } // for SEW LW
             } // if (subcrate)
         }  // while
      poutevt->SetValid(kTRUE); // to store
    } // if(fInput)
  else    cout << "UnpackBranch2: no input event !"<< endl;
}

void TMeshUnpackProc::UnpackBranch3(TMeshB3InputEvent* poutevt)
{
  TGo4MbsSubEvent* psubevt;
  Int_t index=0;
  Int_t value=0;
  Int_t lwords;
  Int_t *pdata;
  fInput    = dynamic_cast<TGo4MbsEvent*> (GetInputEvent()); // from this
  if(fInput)
    {

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
                    if(value != 0)
                      {
                        poutevt->fiCrate3[i] = value; // fill output event
                      }
                    pdata++;
                  } // for SEW LW
             } // if (subcrate)
         }  // while
      poutevt->SetValid(kTRUE); // to store
    } // if(fInput)
  else    cout << "UnpackBranch3: no input event !"<< endl;
}
