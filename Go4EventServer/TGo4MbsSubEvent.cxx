#include "TGo4MbsSubEvent.h"

#include "Riostream.h"
#include "TGo4Log.h"
#include "TGo4MbsSource.h"
#include "TGo4Log.h"
#include "Go4Exceptions.h"

TGo4MbsSubEvent::TGo4MbsSubEvent() :
   TGo4EventElement(),
   fbIsFilled(kFALSE),
   fbIsDataOwner(kFALSE),
   fxHeader(),
   fiAllocLen(0),
   fiData(0)
{
   TRACE((12,"TGo4MbstSubEvent::TGo4MbsSubEvent()",__LINE__, __FILE__));
}

TGo4MbsSubEvent::TGo4MbsSubEvent(Int_t datasize) :
   TGo4EventElement(),
   fbIsFilled(kFALSE),
   fbIsDataOwner(kTRUE),
   fxHeader(),
   fiAllocLen(0),
   fiData(0)
{
   TRACE((12,"TGo4MbsSubEvent::TGo4MbsSubEvent(UInt_t)",__LINE__, __FILE__));
   if(fbIsDataOwner) {
      fiData= new Int_t[datasize];
      fiAllocLen=datasize;
   }
}

TGo4MbsSubEvent::~TGo4MbsSubEvent()
{
   TRACE((12,"TGo4MbsSubEvent::~TGo4MbsSubEvent()",__LINE__, __FILE__));
   // check if Clear with only the used field elements worked correctly
   Clear();
   if(fbIsDataOwner) {
      for(Int_t t=0; t<GetAllocatedLength(); t++)
         if(fiData[t]!=0)
            TGo4Log::Debug( " MBS SubEvent dtor WARNING: Data(%d) not zero after Clear !!!  ",t);
      delete [] fiData;
   }
}


void TGo4MbsSubEvent::PrintEvent()
{
   TRACE((11,"TGo4MbsSubEvent::PrintEvent()",__LINE__, __FILE__));
//////// old style using logger instance:
//   TGo4Log::Info( " MBS SubEvent Header printout: ");
//   TGo4Log::Info( "\tl_dlen    %d ", GetDlen() );
//   TGo4Log::Info( "\ti_type    %d", GetType() );
//   TGo4Log::Info( "\ti_subtype %d", GetSubtype() );
//   TGo4Log::Info( "\ti_procid   %d", GetProcid() );
//   TGo4Log::Info( "\th_subcrate %d", GetSubcrate() );
//   TGo4Log::Info( "\th_control  %d", GetControl() );
//   TGo4Log::Info( "\tAllocatedLen  %d", GetAllocatedLength() );
//   TGo4Log::Info( "\tIsFilled  %d", IsFilled() );
//   TGo4Log::Info( "\tDatafield:");
//   for(Int_t i=0; i<GetAllocatedLength();++i)
//      {
//         if(i+10>GetAllocatedLength())
//           TGo4Log::Info("\t   %d",fiData[i]);
//         else
//            TGo4Log::Info("\t   %d %d %d %d %d %d %d %d %d %d",
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i++],
//               fiData[i]);
//      }

   //// new style just with cout:
   Bool_t printhexa=kFALSE;
   Int_t* pData = (Int_t *) GetDataField();
   cout << "   Mbs Subevent    t/s "
        <<      dec << setw(4) << (Int_t)GetType()
        << " "      << setw(4) << (Int_t)GetSubtype()
        << " len "  << setw(8) << (Int_t)GetDlen()
        << " procid "   << setw(4) << (Int_t)GetProcid()
        << " ctrl " << setw(4) << (Int_t)GetControl()
        << " cr "   << setw(4) << (Int_t)GetSubcrate()
        << endl;
   if(printhexa) cout << hex; else cout << dec;
   for(Int_t i=0; i < GetDlen()/2-1; i++) {
      cout << setw(12) << *pData << " ";
      if((i-3) % 4 == 0) cout << endl;
      pData++;
   }
   cout << endl;
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
   TRACE((11,"TGo4MbsSubEvent::Clear()",__LINE__, __FILE__));
   fbIsFilled=kFALSE;
   if(fbIsDataOwner) {
      // clear array of data
      Int_t dleng= GetDlen();
      if(dleng==0) dleng=2; // default value for dleng is not zero!!
      // only clear regions which were used by the previous fill...
      //cout <<"Mbssubevent::Clear: dlen=" << dleng << endl;
      Int_t fieldlength = (dleng-2) / TGo4MbsSource::fguLONGBYSHORT ; // field is Int_t
      if(fieldlength>fiAllocLen)
         fieldlength=fiAllocLen;

      if(fieldlength==0)
         fiData[0]=0; // clear in case of zero subevents!
      for(Int_t t=0; t<fieldlength;++t) {
         fiData[t]=0;
      }
      //cout<<endl<< "Subevent: cleared "<< fiAllocLen << "int field at "<<fiData << endl;
      fxHeader.Clear();
      Set(dleng); // set to default values, but remember last datalength
   }//if(fbIsDataOwner)
}

void TGo4MbsSubEvent::ReAllocate(Int_t newsize)
{
   TRACE((11,"TGo4MbsSubEvent::ReAllocate(UInt_t)",__LINE__, __FILE__));
   if(!fbIsDataOwner) return;
   if( newsize <= fiAllocLen ) {
      // newsize is smaller, we do not reallocate
   } else {
      delete [] fiData;
      fiData= new Int_t[newsize];
      //cout<< "Subevent: Reallocating " << newsize << ", previous:"<< fiAllocLen<<endl;
      TGo4Log::Debug(" MbsSubEvent: Reallocating Data field from %d to %d longwords ",fiAllocLen,newsize);
      fiAllocLen=newsize;
      Clear();
   }
}
