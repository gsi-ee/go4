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

#include "TGo4SimpleEvent.h"

#include "Go4EventServer.h"

#include "TGo4Log.h"
#include "TGo4SimpleEventProcessor.h"
#include "TGo4SimpleSubEvent.h"


TGo4SimpleEvent::TGo4SimpleEvent(Int_t subevtnum)
: fiCount(0), fiLastSlot(0), fiMaxSlot(subevtnum), fxIterator(0)
{
  GO4TRACE((12,"TGo4SimpleEvent::TGo4SimpleEvent(Int_t)",__LINE__, __FILE__));
   // std::cout << "simple event ctor with "<< subevtnum << "subevents"<< std::endl;
   fxSubEventArray=new TClonesArray("TGo4SimpleSubEvent", subevtnum);
   fxIterator=fxSubEventArray->MakeIterator();
// start test section
   fiArrLen=10;
   for(Int_t z=0; z<10; ++z)
      {
         fiTestArray[z]=z*5;
      }
// end test section

   TGo4SimpleSubEvent* subeve;
   for (Int_t t=0;t<subevtnum;++t)
      {
         subeve= new( (*fxSubEventArray) [t] ) TGo4SimpleSubEvent();

      }


}


TGo4SimpleEvent::TGo4SimpleEvent()
: fiCount(0), fiLastSlot(0),fiMaxSlot(5), fxIterator(0)
{
GO4TRACE((12,"TGo4SimpleEvent::TGo4SimpleEvent()",__LINE__, __FILE__));
   //std::cout << "simple event default ctor"<< std::endl;
   fxSubEventArray=new TClonesArray("TGo4SimpleSubEvent", fiMaxSlot );
   fxIterator=fxSubEventArray->MakeIterator();

}




TGo4SimpleEvent::~TGo4SimpleEvent()
{
GO4TRACE((12,"TGo4SimpleEvent::~TGo4SimpleEvent()",__LINE__, __FILE__));
   delete fxIterator;
   fxSubEventArray->Delete();
   delete fxSubEventArray;


}





void TGo4SimpleEvent::PrintEvent() {
   GO4TRACE((11,"TGo4SimpleEvent::PrintEvent()",__LINE__, __FILE__));

   TGo4EventElement::PrintEvent();
   TGo4Log::Debug( " Simple Event Header printout: ");
   TGo4Log::Debug( "\tCount:    %d ", GetCount() );
   TGo4SimpleSubEvent* sub;
   ResetIterator();

   while ((sub = NextSubEvent() ) !=0)
      {
         sub->PrintEvent();
      }

}

void TGo4SimpleEvent::Clear(Option_t *t)
{
   GO4TRACE((11,"TGo4SimpleEvent::Clear()",__LINE__, __FILE__));
   // here iterate all subevents and clear them
   TGo4SimpleSubEvent* sub;
// too slow!!
//   ResetIterator();
//   while ((sub = NextSubEvent() ) !=0)
//      {
//         sub->Clear();
//      }
   Int_t i=0;
//   while ((sub = dynamic_cast<TGo4SimpleSubEvent*>( fxSubEventArray->At(i++)) ) !=0)
   for(i=0;i<fiMaxSlot; ++i )
      {
         sub = dynamic_cast<TGo4SimpleSubEvent*>( fxSubEventArray->UncheckedAt(i));
         sub->Clear();
      }
   fiCount=0;
}


Int_t TGo4SimpleEvent::Fill()
{
   GO4TRACE((11,"TGo4SimpleEvent::Fill()",__LINE__, __FILE__));
   //
   // check for different source types
   Int_t rev=-1;
   TGo4SimpleEventProcessor* proc;
   if(CheckEventSource("TGo4SimpleEventProcessor"))
      {
         proc = (TGo4SimpleEventProcessor*) GetEventSource();
         proc->BuildSimpleEvent(this);
         rev=0;
      }
   else if(CheckEventSource("TGo4TreeSource"))
      {
         // find out here if tree contains correct event structure...
         // under constr.
         TGo4TreeSource* source = (TGo4TreeSource*) GetEventSource();
         Clear();
         if(source->BuildEvent(this))
            {
               rev=0;
            }
         else
            {
               // error, may be end of tree..
               rev = 1;
            }
      }
 else if(CheckEventSource("TGo4FileSource"))
      {
         // find out here if tree contains correct event structure...
         // under constr.
         TGo4FileSource* source = (TGo4FileSource*) GetEventSource();
         Clear();
         if(source->BuildEvent(this))
            {
               rev=0;
            }
         else
            {
               // error, may be end of tree..
               rev = 1;
            }
      }


   else
      {
         rev=1;
      }

   return rev;

}


TGo4SimpleSubEvent * TGo4SimpleEvent::GetSubEvent(Short_t procid)
{
   GO4TRACE((11,"TGo4SimpleEvent::GetSubEvent(Short_t)",__LINE__, __FILE__));
   TGo4SimpleSubEvent* result=0;
   TGo4SimpleSubEvent* sub;
// this is slow:
//   ResetIterator();
//   while ((sub = NextSubEvent() ) !=0)
//      {
//         if(sub->fsProcid==procid)
//            {
//               result=sub;
//               break;
//            }
//         else
//            { }
//      }
   Int_t i=0;
//   while ((sub = dynamic_cast<TGo4SimpleSubEvent*>( fxSubEventArray->At(i++)) ) !=0)
   for(i=0;i<fiMaxSlot; ++i )
      {
         sub = dynamic_cast<TGo4SimpleSubEvent*>( fxSubEventArray->UncheckedAt(i));
         if(sub->fsProcid==procid)
            {
               result=sub;
               break;
            }
         else
            { }
      }

   return result;
}

Int_t  TGo4SimpleEvent::GetCount() const
{
   return fiCount;
}
TGo4SimpleSubEvent * TGo4SimpleEvent::AddSubEvent(Short_t procid)
{
   GO4TRACE((11,"TGo4SimpleEvent::AddSubEvent(Short_t)",__LINE__, __FILE__));

   TGo4SimpleSubEvent* result=0;
   if( (result=GetSubEvent(procid)) != 0 )
      {
         // we found the entry, return this one
      }
   else
      {
         // nothing found, use the last one
         if(fiLastSlot+1<fiMaxSlot)
            {
               fiLastSlot++;
               result=new( (*fxSubEventArray) [fiLastSlot] ) TGo4SimpleSubEvent();
               result->fsProcid=procid;
               //std::cout << "simple event: added subevent with procid " << procid << std::endl;
               TGo4Log::Debug(" SimpleEvent: Added subevent with procid %d:  ", procid);
            }
         else
            {
               TGo4Log::Error("simple event: clonesarray is full, cannot add another subevent");
               result=0;
            }

      }

   return result;
}
void TGo4SimpleEvent::ResetIterator()
{
   GO4TRACE((11,"TGo4SimpleEvent::ResetIterator()",__LINE__, __FILE__));
   delete fxIterator;
   fxIterator = fxSubEventArray->MakeIterator();
   // note: a mere Iterator Reset fails when run with events read from streamer ??
   //fxIterator->Reset();
}

TGo4SimpleSubEvent* TGo4SimpleEvent::NextSubEvent()
{
   GO4TRACE((11,"TGo4SimpleEvent::NextSubEvent()",__LINE__, __FILE__));
   TGo4SimpleSubEvent*   sub = (TGo4SimpleSubEvent*) fxIterator ->Next();
   return sub;
}
