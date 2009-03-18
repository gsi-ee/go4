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
   fxIterator(0),
   fxMbsSource(0),
   fxTreeSource(0),
   fxFileSource(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent()",__LINE__, __FILE__));
}

TGo4MbsEvent::TGo4MbsEvent(UInt_t subnum, Short_t* subids, UInt_t datasize) :
   TGo4EventElement("MbsEvent-10-1"),
   fxHeader(),
   fxSubEvArray(0),
   fxIterator(0),
   fxMbsSource(0),
   fxTreeSource(0),
   fxFileSource(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Short_t, UInt_t)",__LINE__, __FILE__));
   fxSubEvArray=new TObjArray(subnum+5);
         // we choose dimension of array a bit larger
         // to avoid resize for later added subevts
   fxIterator = fxSubEvArray->MakeIterator();
   ResetIterator();
   TGo4MbsSubEvent* subeve;
   for (UInt_t t=0;t<subnum;++t)
      {
         subeve= new TGo4MbsSubEvent(datasize);
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
   fxIterator(0),
   fxMbsSource(0),
   fxTreeSource(0),
   fxFileSource(0)
{
   TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Char_t*, Char_t*, Short_t* UInt_t)",__LINE__, __FILE__));
   fxSubEvArray=new TObjArray(subnum+5);
         // we choose dimension of array a bit larger
         // to avoid TObjArray resize for later added subevts
   fxIterator = fxSubEvArray->MakeIterator();
   ResetIterator();
   TGo4MbsSubEvent* subeve;
   for (UInt_t t=0;t<subnum;++t)
      {
         subeve= new TGo4MbsSubEvent(datasizes[t]);
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
   delete fxIterator;
   if(fxSubEvArray) fxSubEvArray->Delete();
   delete fxSubEvArray;
}

Int_t TGo4MbsEvent::Fill()
{
   TRACE((11,"TGo4MbsEvent::Fill()",__LINE__, __FILE__));
   // check for different source types
   Int_t rev=0;
   if(fxMbsSource!=0)
      {
         Clear();
         rev=fxMbsSource->NextEvent();
         if(rev!=GETEVT__SUCCESS)
            {

               TGo4Log::Debug(" Mbs Event --  !!! NextEvent() error: %s !!! ",
                           fxMbsSource->GetErrMess());
               return rev;
            }
         else
            {
               fxMbsSource->BuildMbsEvent(this);
               return rev;
            }
      }

   else if(fxTreeSource!=0)
      {
         Clear();
         if(fxTreeSource->BuildEvent(this))
            {
               return 0;
            }
         else
            {
               // error, may be end of tree..
               return 1;
            }

      }
   else if(fxFileSource!=0)
      {
         Clear();
         if(fxFileSource->BuildEvent(this))
            {
               delete fxIterator; // need this to prepare correct iterator!
               if(fxSubEvArray)
                  fxIterator = fxSubEvArray->MakeIterator();
               else
                  fxIterator=0;
               return 0;
            }
         else
            {
               // error, may be end of tree..
               return 1;
            }

      }

   else
      {

         TGo4Log::Debug(" !!! MbsEvent: Fill ERROR:  unknown event source !!! ");
         return 1;
      }

   return 0;
}

Int_t TGo4MbsEvent::Init()
{
   TRACE((11,"TGo4MbsEvent::Init()",__LINE__, __FILE__));
   // Check event sources here and downcast the correct one:
   // will be called once before event processing is done
   if(CheckEventSource("TGo4MbsSource"))
      {
         fxMbsSource = dynamic_cast<TGo4MbsSource*> (GetEventSource());
         fxTreeSource=0;
         fxFileSource=0;
      }
   else if(CheckEventSource("TGo4TreeSource"))
      {
         fxTreeSource = dynamic_cast<TGo4TreeSource*> (GetEventSource());
         fxMbsSource=0;
         fxFileSource=0;
      }
   else if(CheckEventSource("TGo4FileSource"))
      {
         fxFileSource = dynamic_cast<TGo4FileSource*> (GetEventSource());
         fxTreeSource=0;
         fxMbsSource=0;
      }
   else
      {
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
   while ((sub = NextSubEvent(kTRUE)) !=0)
      {
         sub->Clear();
         //cout<< "cleared subevent"<< sub->GetProcid() << endl;
      }
//   fxHeader.Clear();
//   Set(); // set to default values
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

void TGo4MbsEvent::PrintEvent() {
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
   TGo4MbsSubEvent* sub;
   ResetIterator();
   while ((sub = NextSubEvent() ) !=0)
      {
         sub->PrintEvent();
      }



}

void TGo4MbsEvent::ResetIterator()
{
   TRACE((11,"TGo4MbsEvent::ResetIterator()",__LINE__, __FILE__));
   if(fxIterator!=0)
      {
         fxIterator->Reset();
      }
   else
      {
         if(fxSubEvArray)
            fxIterator = fxSubEvArray->MakeIterator();
      }
}

TGo4MbsSubEvent* TGo4MbsEvent::NextSubEvent(Bool_t all)
{
   TRACE((11,"TGo4MbsEvent::NextSubEvent()",__LINE__, __FILE__));
   if(fxIterator==0) return 0;
   TGo4MbsSubEvent*   sub=0;
   do
   {
      sub= dynamic_cast<TGo4MbsSubEvent*> (fxIterator ->Next());
      if(sub==0) break;
   }while(!all && !sub->IsFilled()); // skip not filled subevents
   return sub;
}





TGo4MbsSubEvent * TGo4MbsEvent::GetSubEvent(Char_t subcrate, Char_t ctrl, Short_t procid)
{
   TRACE((11,"TGo4MbsEvent::GetSubEvent(Char_t)",__LINE__, __FILE__));
   TGo4MbsSubEvent* result=0;
   TGo4MbsSubEvent* sub=0;
   ResetIterator();
   while ((sub = NextSubEvent() ) !=0)
      {
         if( (sub->GetSubcrate() == subcrate)
               && (sub->GetControl() == ctrl)
               && (sub->GetProcid()== procid)   )
            {
               result=sub;
               break;
            }
      }
   return result;
}

TGo4MbsSubEvent * TGo4MbsEvent::GetSubEvent(Short_t procid)
{
   TRACE((11,"TGo4MbsEvent::GetSubEvent(Short_t)",__LINE__, __FILE__));
   TGo4MbsSubEvent* result=0;
   TGo4MbsSubEvent* sub=0;
   ResetIterator();
   while ((sub = NextSubEvent() ) !=0)
      {
         if(sub->GetProcid()== procid)
            {
               result=sub;
               break;
            }
      }
   return result;
}






s_filhe * TGo4MbsEvent::GetMbsSourceHeader()
{
   if(fxMbsSource)
      return (fxMbsSource->GetInfoHeader());
   else
      return 0;
}


s_bufhe * TGo4MbsEvent::GetMbsBufferHeader()
{
   if(fxMbsSource)
      return (fxMbsSource->GetBufferHeader());
   else
      return 0;
}

void TGo4MbsEvent::SetPrintEvent(Int_t num, Int_t sid, Int_t longw, Int_t hexw, Int_t dataw)
{
   if(fxMbsSource)
      return (fxMbsSource->SetPrintEvent(num,sid,longw,hexw,dataw));
}
