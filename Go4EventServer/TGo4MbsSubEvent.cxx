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

#include "TGo4MbsSubEvent.h"

#include "TGo4Log.h"
#include "TGo4MbsSource.h"
#include "TGo4Log.h"
#include "Go4Exceptions.h"

TGo4MbsSubEvent::TGo4MbsSubEvent() :
   TGo4EventElement(),
   fbIsFilled(kFALSE),
   fbIsDataOwner(kTRUE),
   fxHeader(),
   fiAllocLen(0),
   fiData(0)
{
   GO4TRACE((12,"TGo4MbstSubEvent::TGo4MbsSubEvent()",__LINE__, __FILE__));
   //TGo4Log::Info( "MBS SubEvent default ctor.");
}

TGo4MbsSubEvent::TGo4MbsSubEvent(Int_t datasize) :
   TGo4EventElement(),
   fbIsFilled(kFALSE),
   fbIsDataOwner(kTRUE),
   fxHeader(),
   fiAllocLen(0),
   fiData(0)
{
   GO4TRACE((12,"TGo4MbsSubEvent::TGo4MbsSubEvent(UInt_t)",__LINE__, __FILE__));
   //TGo4Log::Info( "MBS SubEvent normal ctor.");
   if(fbIsDataOwner) {
      fiData = new Int_t[datasize];
      fiAllocLen = datasize;
      for(Int_t t=0; t<datasize; t++) fiData[t] = 0;
   }
}

TGo4MbsSubEvent::~TGo4MbsSubEvent()
{
   GO4TRACE((12,"TGo4MbsSubEvent::~TGo4MbsSubEvent()",__LINE__, __FILE__));
   // check if Clear with only the used field elements worked correctly
   //TGo4Log::Info( "MBS SubEvent dtor...");
   Clear();
   if(fbIsDataOwner) {
      for(Int_t t=0; t<GetAllocatedLength(); t++)
         if(fiData[t]!=0)
            TGo4Log::Debug( " MBS SubEvent dtor WARNING: Data(%d) not zero after Clear !!!  ",t);
      delete [] fiData;
      //TGo4Log::Info( "MBS SubEvent dtor deleted data field");
   }
}


void TGo4MbsSubEvent::PrintEvent()
{
   GO4TRACE((11,"TGo4MbsSubEvent::PrintEvent()",__LINE__, __FILE__));

   //// new style just with std::cout:
/*   Bool_t printhexa=kFALSE;
   Int_t* pData = (Int_t *) GetDataField();
   std::cout << "   Mbs Subevent    t/s "
        <<      dec << std::setw(4) << (Int_t)GetType()
        << " "      << std::setw(4) << (Int_t)GetSubtype()
        << " len "  << std::setw(8) << (Int_t)GetDlen()
        << " procid "   << std::setw(4) << (Int_t)GetProcid()
        << " ctrl " << std::setw(4) << (Int_t)GetControl()
        << " cr "   << std::setw(4) << (Int_t)GetSubcrate()
        << std::endl;
   if(printhexa) std::cout << hex; else std::cout << dec;
   for(Int_t i=0; i < GetDlen()/2-1; i++) {
      std::cout << std::setw(12) << *pData << " ";
      if((i-3) % 4 == 0) std::cout << std::endl;
      pData++;
   }
   std::cout << std::endl;
*/

   // very new style just using printf
   PrintMbsSubevent();
}


void TGo4MbsSubEvent::PrintMbsSubevent(Bool_t longw, Bool_t hexw, Bool_t dataw)
{
   // print header
   printf("  SubEv ID %6d Type/Subtype %5d %5d Length %5d[w] Control %2d Subcrate %2d\n",
         GetProcid(), GetType(), GetSubtype(), GetDlen(), GetControl(), GetSubcrate());

   if(!dataw) return;
   // print data

   Int_t *pl_data = GetDataField();
   Int_t ll = GetIntLen();

   if(longw) {
      /* In this case we assume data as one longword per channel */
      for(Int_t l=0; l<ll; l++) {
         if(l%8 == 0) printf("  ");
         if(hexw) printf("%04x.%04x ", (unsigned) ((*pl_data>>16) & 0xffff), (unsigned) (*pl_data & 0xffff));
             else printf("%8u ", (unsigned) *pl_data);
         pl_data++;
         if(l%8 == 7) printf("\n");
      }

      if (ll%8 != 0) printf("\n");
   } else {
      /* In this case we assume data as two words per channel */
      for(Int_t l=0;l<ll;l++) {
         if(l%4 == 0) printf("  ");
         if(hexw) printf("%6x%6x", (unsigned) (*pl_data&0xffff), (unsigned) ((*pl_data>>16)&0xffff));
             else printf("%8u%8u", (unsigned) (*pl_data&0xffff), (unsigned) ((*pl_data>>16)&0xffff));
         pl_data++;
         if(l%4 == 3) printf("\n");
      }
      if(ll%4 != 0) printf("\n");
   }
}

void TGo4MbsSubEvent::Set(Int_t dlen,
                          Short_t type,
                          Short_t subtype,
                          Short_t procid,
                          Char_t subcrate,
                          Char_t ctrl)
{
   SetDlen(dlen);
   SetType(type);
   SetSubtype(subtype);
   //SetProcid(procid); do not change procid and identifiers!!
   //SetSubcrate(subcrate);
   //SetControl(ctrl);
}

void  TGo4MbsSubEvent::Clear(Option_t *t)
{
   GO4TRACE((11,"TGo4MbsSubEvent::Clear()",__LINE__, __FILE__));
   fbIsFilled = kFALSE;
   if(fbIsDataOwner) {
      // clear array of data
      Int_t dleng = GetDlen();
      if(dleng==0) dleng=2; // default value for dleng is not zero!!
      // only clear regions which were used by the previous fill...
      //std::cout <<"Mbssubevent::Clear: dlen=" << dleng << std::endl;
      Int_t fieldlength = (dleng-2) / TGo4MbsSource::fguLONGBYSHORT ; // field is Int_t
      if(fieldlength>fiAllocLen)
         fieldlength=fiAllocLen;

      if(fieldlength==0)
         fiData[0]=0; // clear in case of zero subevents!

      for(Int_t t=0; t<fieldlength;++t)
         fiData[t]=0;
      //std::cout<< std::endl<< "Subevent: cleared "<< fiAllocLen << "int field at "<<fiData << std::endl;
      fxHeader.Clear();
      Set(dleng); // set to default values, but remember last datalength
   }//if(fbIsDataOwner)
}

void TGo4MbsSubEvent::ReAllocate(Int_t newsize)
{
   GO4TRACE((11,"TGo4MbsSubEvent::ReAllocate(UInt_t)",__LINE__, __FILE__));
   if(!fbIsDataOwner) return;
   if( newsize <= fiAllocLen ) {
      // newsize is smaller, we do not reallocate
   } else {
      delete [] fiData;
      fiData= new Int_t[newsize];
      //std::cout<< "Subevent: Reallocating " << newsize << ", previous:"<< fiAllocLen<< std::endl;
      TGo4Log::Debug(" MbsSubEvent: Reallocating Data field from %d to %d longwords ",fiAllocLen,newsize);
      fiAllocLen=newsize;
      Clear();
   }
}
