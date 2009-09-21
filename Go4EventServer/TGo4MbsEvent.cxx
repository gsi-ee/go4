#include "TGo4MbsEvent.h"

#include "Riostream.h"
#include "TObjArray.h"

#include "TGo4Log.h"
#include "Go4Exceptions.h"
#include "TGo4MbsSubEvent.h"
#include "TGo4MbsSource.h"
#include "TGo4RevServ.h"
#include "TGo4TreeSource.h"
#include "TGo4FileSource.h"

TGo4MbsEvent::TGo4MbsEvent() :
   TGo4EventElement(),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent()",__LINE__, __FILE__));
}

TGo4MbsEvent::TGo4MbsEvent(UInt_t subnum, Short_t* subids, UInt_t datasize) :
   TGo4EventElement("MbsEvent-10-1"),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Short_t, UInt_t)",__LINE__, __FILE__));
   fxSubEvArray=new TObjArray(subnum+5);
   for (UInt_t t=0;t<subnum;++t) {
      TGo4MbsSubEvent* subeve = new TGo4MbsSubEvent(datasize);
      fxSubEvArray->AddLast(subeve);
      subeve->SetProcid(subids[t]);
   }
   Clear();
}

TGo4MbsEvent::TGo4MbsEvent(UInt_t subnum,
                           Char_t* subcrates,
                           Char_t* controls,
                           Short_t* procids,
                           UInt_t* datasizes) :
   TGo4EventElement("MbsEvent-10-1"),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Char_t*, Char_t*, Short_t* UInt_t)",__LINE__, __FILE__));
   fxSubEvArray = new TObjArray(subnum+5);
   for (UInt_t t=0;t<subnum;++t) {
      TGo4MbsSubEvent* subeve = new TGo4MbsSubEvent(datasizes[t]);
      fxSubEvArray->AddLast(subeve);
      subeve->SetSubcrate(subcrates[t]);
      subeve->SetControl(controls[t]);
      subeve->SetProcid(procids[t]);
   }
   Clear();
}

TGo4MbsEvent::~TGo4MbsEvent()
{
   TRACE((12,"TGo4MbsEvent::~TGo4MbsEvent()",__LINE__, __FILE__));
   if(fxSubEvArray) fxSubEvArray->Delete();
   delete fxSubEvArray;
}

Int_t TGo4MbsEvent::Fill()
{
   TRACE((11,"TGo4MbsEvent::Fill()",__LINE__, __FILE__));
   // check for different source types

   if (GetEventSource()==0) return 1;

   Clear();

   if (GetEventSource()->BuildEvent(this)) return 0;

   Int_t rev = GetEventSource()->GetEventStatus();

   return rev==0 ? 1 : rev;
}

Int_t TGo4MbsEvent::Init()
{
   TRACE((11,"TGo4MbsEvent::Init()",__LINE__, __FILE__));
   // Check event sources here and downcast the correct one:
   // will be called once before event processing is done

   if (GetEventSource()==0) {
      TGo4Log::Debug(" !!! MbsEvent: Init ERROR:  unknown event source !!! ");
      return 1;
   }

   return 0;
}

void TGo4MbsEvent::Clear(Option_t *t)
{
   TRACE((11,"TGo4MbsEvent::Clear()",__LINE__, __FILE__));
   // here iterate all subevents and clear them
   TGo4MbsSubEvent* sub;
   ResetIterator();
   while ((sub = NextSubEvent(kTRUE))!=0) sub->Clear();
}

void TGo4MbsEvent::Set(Int_t dlen, Short_t type, Short_t subtype,
                       Short_t dummy, Short_t trigger, Int_t count)
{
   SetDlen(dlen);
   SetType(type);
   SetSubtype(subtype);
   SetDummy(dummy);
   SetTrigger(trigger);
   SetCount(count);
}

void TGo4MbsEvent::PrintEvent()
{
   TGo4EventElement::PrintEvent();

/////////Old style using logger instance:
//   TGo4Log::Info( " MBS Event Header printout: ");
//   TGo4Log::Info( "\tl_dlen    %d ", GetDlen() );
//   TGo4Log::Info( "\ti_type    %d", GetType() );
//   TGo4Log::Info( "\ti_subtype %d", GetSubtype() );
//   TGo4Log::Info( "\ti_dummy   %d", GetDummy() );
//   TGo4Log::Info( "\th_trigger %d", GetTrigger() );
//   TGo4Log::Info( "\th_count  %d", GetCount() );

///// new style just using cout:
   cout << "MBS Event printout:"
        << dec      << setw(8) << (Int_t) GetCount()
        << " t/s "  << setw(4) << (Int_t) GetType()
        << " "      << setw(4) << (Int_t) GetSubtype()
        << " len "  << setw(8) << (Int_t) GetDlen()
        << " trig   " << setw(4) << (Int_t) GetTrigger()
        << endl;
   TGo4MbsSubEvent *sub(0);
   ResetIterator();
   while ((sub=NextSubEvent())!=0) sub->PrintEvent();
}

TGo4MbsSubEvent* TGo4MbsEvent::NextSubEvent(Bool_t all)
{
   TRACE((11,"TGo4MbsEvent::NextSubEvent()",__LINE__, __FILE__));

   if (fxSubEvArray==0) return 0;

   TGo4MbsSubEvent* sub(0);
   do {
      if (fiSubEvIndex>fxSubEvArray->GetLast()) return 0;
      sub = (TGo4MbsSubEvent*) (fxSubEvArray->At(fiSubEvIndex++));
      if(sub==0) break;
   } while(!all && !sub->IsFilled()); // skip not filled subevents
   return sub;
}


TGo4MbsSubEvent * TGo4MbsEvent::GetSubEvent(Char_t subcrate, Char_t ctrl, Short_t procid)
{
   TRACE((11,"TGo4MbsEvent::GetSubEvent(Char_t)",__LINE__, __FILE__));
   TGo4MbsSubEvent* sub(0);
   ResetIterator();
   while ((sub = NextSubEvent())!=0)
      if((sub->GetSubcrate()==subcrate) &&
         (sub->GetControl()==ctrl) &&
         (sub->GetProcid()==procid)) return sub;
   return 0;
}

TGo4MbsSubEvent * TGo4MbsEvent::GetSubEvent(Short_t procid)
{
   TRACE((11,"TGo4MbsEvent::GetSubEvent(Short_t)",__LINE__, __FILE__));
   TGo4MbsSubEvent* sub(0);
   ResetIterator();
   while ((sub = NextSubEvent())!=0)
      if(sub->GetProcid()== procid) return sub;
   return 0;
}


s_filhe * TGo4MbsEvent::GetMbsSourceHeader()
{
   TGo4MbsSource* src = dynamic_cast<TGo4MbsSource*> (GetEventSource());
   return src ? src->GetInfoHeader() : 0;
}


s_bufhe * TGo4MbsEvent::GetMbsBufferHeader()
{
   TGo4MbsSource* src = dynamic_cast<TGo4MbsSource*> (GetEventSource());
   return src ? src->GetBufferHeader() : 0;
}


void TGo4MbsEvent::SetPrintEvent(Int_t num, Int_t sid, Int_t longw, Int_t hexw, Int_t dataw)
{
   TGo4MbsSource* src = dynamic_cast<TGo4MbsSource*> (GetEventSource());
   if(src) src->SetPrintEvent(num,sid,longw,hexw,dataw);
}
