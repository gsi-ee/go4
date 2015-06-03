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

#include "TGo4MbsEvent.h"

#include "TObjArray.h"

#include "TGo4Log.h"
#include "Go4Exceptions.h"
#include "TGo4MbsSubEvent.h"
#include "TGo4MbsSource.h"
#include "TGo4RevServ.h"
#include "TGo4TreeSource.h"
#include "TGo4FileSource.h"


extern "C" {
   #include "f_ut_utime.h"
}

TGo4MbsEvent::TGo4MbsEvent() :
   TGo4EventElement(),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0),
   fbIsReference(kFALSE)
{
   GO4TRACE((12,"TGo4MbsEvent::TGo4MbsEvent()",__LINE__, __FILE__));
   //TGo4Log::Info( "MBS Event default ctor");
}

TGo4MbsEvent::TGo4MbsEvent(UInt_t subnum, Short_t* subids, UInt_t datasize) :
   TGo4EventElement("MbsEvent101"),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0),
   fbIsReference(kFALSE)
{
   GO4TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Short_t, UInt_t)",__LINE__, __FILE__));
   fxSubEvArray=new TObjArray(subnum+5);
   fxSubEvArray->SetOwner(kTRUE); // important for streamer
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
   TGo4EventElement("MbsEvent101"),
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0),
   fbIsReference(kFALSE)
{
   GO4TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(UInt_t, Char_t*, Char_t*, Short_t* UInt_t)",__LINE__, __FILE__));
   fxSubEvArray = new TObjArray(subnum+5);
   fxSubEvArray->SetOwner(kTRUE); // important for streamer
   for (UInt_t t=0;t<subnum;++t) {
      TGo4MbsSubEvent* subeve = new TGo4MbsSubEvent(datasizes[t]);
      fxSubEvArray->AddLast(subeve);
      subeve->SetSubcrate(subcrates[t]);
      subeve->SetControl(controls[t]);
      subeve->SetProcid(procids[t]);
   }
   Clear();
}


TGo4MbsEvent::TGo4MbsEvent(const char* ) :
   TGo4EventElement("MbsEvent101"), // note that name parameter is dummy to be consistent with file source!
   fxHeader(),
   fxSubEvArray(0),
   fiSubEvIndex(0),
   fbIsReference(kFALSE)
{
   GO4TRACE((12,"TGo4MbsEvent::TGo4MbsEvent(const char)",__LINE__, __FILE__));
   SimpleInit();
}

void TGo4MbsEvent::SimpleInit()
{
   fxSubEvArray = new TObjArray(5);
   fxSubEvArray->SetOwner(kTRUE); // important for streamer
   // we just add one arbitrary subevent to the array.
   // actually subevents will be appended dynamically later
   TGo4MbsSubEvent* subeve = new TGo4MbsSubEvent(1024);
   fxSubEvArray->AddLast(subeve);
   subeve->SetSubcrate(0);
   subeve->SetControl(0);
   subeve->SetProcid(0);
   Set();
   Clear();
}

TGo4MbsEvent::~TGo4MbsEvent()
{
   GO4TRACE((12,"TGo4MbsEvent::~TGo4MbsEvent()",__LINE__, __FILE__));
   //TGo4Log::Info( "MBS Event dtor...");
   if(!fbIsReference)
   {
      if(fxSubEvArray) fxSubEvArray->Delete();
      delete fxSubEvArray;
      //TGo4Log::Info( "MBS Event dtor deleted obj array");
   }
}

void TGo4MbsEvent::Clear(Option_t *)
{
   GO4TRACE((11,"TGo4MbsEvent::Clear()",__LINE__, __FILE__));
   if(!fbIsReference) {
      // here iterate all subevents and clear them
      TGo4MbsSubEvent* sub(0);
      ResetIterator();
      while ((sub = NextSubEvent(kTRUE))!=0) sub->Clear();
   }
}

void TGo4MbsEvent::AssignReference(TGo4MbsEvent* ref)
{
   if(!ref) return;
   if(!fbIsReference) {
      // clean up previous data if existing
      if(fxSubEvArray) fxSubEvArray->Delete();
      delete fxSubEvArray;
   }
   fbIsReference = true;
   // copy event headers:
   fxHeader.fiCount=ref->fxHeader.fiCount;
   fxHeader.fsDummy=ref->fxHeader.fsDummy;
   fxHeader.fsTrigger=ref->fxHeader.fsTrigger;
   fxHeader.fxGSIHeader.fiDlen=ref->fxHeader.fxGSIHeader.fiDlen;
   fxHeader.fxGSIHeader.fsSubtype=ref->fxHeader.fxGSIHeader.fsSubtype;
   fxHeader.fxGSIHeader.fsType=ref->fxHeader.fxGSIHeader.fsType;

   //assign external array:
   fxSubEvArray=ref->fxSubEvArray;
}

void TGo4MbsEvent::RemoveReference()
{
   if(fbIsReference)
      SimpleInit();
   fbIsReference = kFALSE;
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
///// new style just using std::cout:
//   TGo4EventElement::PrintEvent();
//
//   std::cout << "MBS Event printout:"
//        << dec      << std::setw(8) << (Int_t) GetCount()
//        << " t/s "  << std::setw(4) << (Int_t) GetType()
//        << " "      << std::setw(4) << (Int_t) GetSubtype()
//        << " len "  << std::setw(8) << (Int_t) GetDlen()
//        << " trig   " << std::setw(4) << (Int_t) GetTrigger()
//        << std::endl;
//   TGo4MbsSubEvent *sub(0);
//   ResetIterator();
//   while ((sub=NextSubEvent())!=0) sub->PrintEvent();

   // very new style - just using printf
   PrintMbsEvent();
}

void TGo4MbsEvent::PrintMbsFileHeader()
{
   s_filhe* head = GetMbsSourceHeader();
   if (head==0) return;
   printf("***** File header:\n");
   printf("\tdatalen: %d \n", head->filhe_dlen);
   printf("\tfilename_l: %d \n", head->filhe_file_l);
   printf("\tfilename: %s\n", head->filhe_file);
   printf("\ttype: %d \n", head->filhe_type);
   printf("\tsubtype: %d\n", head->filhe_subtype);
   printf("\t#commentlines: %d\n", head->filhe_lines);
   for(Int_t i=0; i<head->filhe_lines;++i)
       printf("\t\t %s\n", head->s_strings[i].string);
}

void TGo4MbsEvent::PrintMbsBufferHeader()
{
   s_bufhe* head = GetMbsBufferHeader();
   if (head==0) return;

   printf("***** Buffer header:\n");
   printf("\tbuffernumber: %d \n", head->l_buf);
   printf("\tdatalen: %d \n", head->l_dlen);
   printf("\ttime lo: %u\n", head->l_time[0]); // seconds since epoch 1970
   printf("\ttime hi: %u\n", head->l_time[1]); // microseconds since time lo
   char sbuf[1000];
   f_ut_utime(head->l_time[0], head->l_time[1], sbuf);
   printf("\ttimestring: %s\n", sbuf);
   printf("\ttype: %d \n", head->i_type);
   printf("\tsubtype: %d \n", head->i_subtype);
}


void TGo4MbsEvent::PrintMbsEvent(Int_t subid, Bool_t longw, Bool_t hexw, Bool_t dataw, Bool_t bufhead, Bool_t filhead)
{
   static TString oldfname = "";
   static TString timestring = "";
   if (filhead) {
      s_filhe* head = GetMbsSourceHeader();
      if (head  && (TString(head->filhe_file) != oldfname)) {
         oldfname=head->filhe_file;
         PrintMbsFileHeader();
      }
   }
   static int oldbnum = 0;
   if (bufhead) {
      s_bufhe* head = GetMbsBufferHeader();
      if ( head  && (head->l_buf != oldbnum) ) {
         // only print if buffernumber has changed
         oldbnum = head->l_buf;
         char sbuf[1000];
         f_ut_utime(head->l_time[0], head->l_time[1], sbuf);
         timestring = sbuf;

         PrintMbsBufferHeader();
      }
   }

   if (GetType()==10) {
      printf("Event   %9d Type/Subtype %5d %5d Length %5d[w] Trigger %2d %s\n",
               GetCount(), GetType(), GetSubtype(), GetDlen(), GetTrigger(), timestring.Data());
   } else {
      printf("Event type %d, subtype %d, data longwords %d",
               GetType(), GetSubtype(), GetDlen()/2);
   }


   TGo4MbsSubEvent *sub(0);
   ResetIterator();
   while ((sub=NextSubEvent())!=0) {

      if ((subid>=0) && (sub->GetProcid()!=subid)) continue;

      sub->PrintMbsSubevent(longw, hexw, dataw);
   }
}


void TGo4MbsEvent::SetPrintEvent(Int_t num, Int_t sid, Int_t longw, Int_t hexw, Int_t dataw)
{
   TGo4MbsSource* src = dynamic_cast<TGo4MbsSource*> (GetEventSource());
   if (src) src->SetPrintEvent(num, sid, longw, hexw, dataw);
}


TGo4MbsSubEvent* TGo4MbsEvent::NextSubEvent(Bool_t all)
{
   GO4TRACE((11,"TGo4MbsEvent::NextSubEvent()",__LINE__, __FILE__));

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
   GO4TRACE((11,"TGo4MbsEvent::GetSubEvent(Char_t)",__LINE__, __FILE__));
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
   GO4TRACE((11,"TGo4MbsEvent::GetSubEvent(Short_t)",__LINE__, __FILE__));
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

TGo4MbsSubEvent* TGo4MbsEvent::AddSubEvent(Int_t fullID, Short_t* source, Int_t datalength, Bool_t copydata)
{
   Int_t fieldlength =0;
   TGo4MbsSubEvent* subtarget=0; // the subevent in use
   TGo4MbsSubEvent* subtargetindex=0; // target subevent iterator
   if(datalength>2)
      fieldlength = (datalength-2) / (sizeof(Int_t) / sizeof(Short_t)); // field is Int_t
   else {
      TGo4Log::Debug(" !!! MbsSource --  EMPTY subevent #%d ", fullID);
      fieldlength =0; // for empty subevents (<- W.M.)
   }

   ResetIterator();
   while ( ( subtargetindex = NextSubEvent(kTRUE) ) !=0 ) {
      // get pointer to complete id longword in structures:
      Int_t* subtargetid= &((subtargetindex->fxHeader).fiFullid);
      if(*subtargetid == fullID) {
         // subevent ids match:
         if(!subtargetindex->fbIsFilled) {
            // this has not been filled before, we fill this one
            subtarget=subtargetindex;
            break; // leave the loop, fill later
         } else {
            // was already filled in this cycle, continue for next sub
         }
      } else {
         // no match, try next subevent
      }
   } // while (subtargetindex...)

   if(!subtarget) {
      // we found no matching id, create new TObjArray entry
      subtarget = new TGo4MbsSubEvent(fieldlength);
      Int_t* newsubtargetid= &((subtarget->fxHeader).fiFullid);
      if(!copydata) {
         subtarget->fbIsDataOwner = kFALSE;
         delete [] (subtarget->fiData); // remove default field
         subtarget->fiAllocLen=0;
      }
      *newsubtargetid = fullID;
      TGo4Log::Debug(" Created new output subevent for event\n\tpid:%d subcrate:%d ctrl:%d",
                      subtarget->GetProcid(),subtarget->GetSubcrate(), subtarget->GetControl());
      if (fxSubEvArray == 0) fxSubEvArray = new TObjArray(5);
      fxSubEvArray->AddLast(subtarget);
   }
   ////     fill header:
   subtarget->SetDlen(datalength);
   void* data = (void*) source;
   if(copydata) {
      subtarget->fbIsDataOwner = kTRUE;
      subtarget->ReAllocate(fieldlength); // reallocate field if necessary
      //// fill data into target field:
      if(datalength>2)
         memcpy((void*) (subtarget->fiData),
               data, (datalength-2)*sizeof(Short_t));
   } else {
      // set reference to external data field in subevent
      subtarget->fbIsDataOwner=kFALSE;
      subtarget->fiAllocLen=fieldlength;
      if(datalength>2)
         subtarget->fiData= (Int_t*) data;
      else
         subtarget->fiData= 0; // reset for empty subevent
   }// if(fbDataCopyMode)
   subtarget->fbIsFilled = kTRUE; // remember we filled this one, never overwrite!
   return subtarget;
}

