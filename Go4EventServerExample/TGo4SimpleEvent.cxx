// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
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

TGo4SimpleEvent::TGo4SimpleEvent() :
   TGo4EventElement()
{
   GO4TRACE((12,"TGo4SimpleEvent::TGo4SimpleEvent()",__LINE__, __FILE__));

   fiMaxSlot = 5;
   fxSubEventArray = new TClonesArray("TGo4SimpleSubEvent", fiMaxSlot);
   fxIterator = fxSubEventArray->MakeIterator();
}


TGo4SimpleEvent::TGo4SimpleEvent(Int_t subevtnum) :
   TGo4EventElement()
{
   GO4TRACE((12,"TGo4SimpleEvent::TGo4SimpleEvent(Int_t)",__LINE__, __FILE__));

   fiMaxSlot = subevtnum;
   fxSubEventArray = new TClonesArray("TGo4SimpleSubEvent", fiMaxSlot);
   fxIterator = fxSubEventArray->MakeIterator();
   // start test section
   fiArrLen = 10;
   for (Int_t z = 0; z < 10; ++z) {
      fiTestArray[z] = z * 5;
   }
   // end test section

   for (Int_t t = 0; t < subevtnum; ++t) {
      new ((*fxSubEventArray)[t]) TGo4SimpleSubEvent();
   }
}

TGo4SimpleEvent::~TGo4SimpleEvent()
{
   GO4TRACE((12,"TGo4SimpleEvent::~TGo4SimpleEvent()",__LINE__, __FILE__));
   delete fxIterator;
   fxSubEventArray->Delete();
   delete fxSubEventArray;
}


void TGo4SimpleEvent::PrintEvent()
{
   GO4TRACE((11,"TGo4SimpleEvent::PrintEvent()",__LINE__, __FILE__));

   TGo4EventElement::PrintEvent();
   TGo4Log::Debug( " Simple Event Header printout: ");
   TGo4Log::Debug( "\tCount:    %d ", GetCount() );

   ResetIterator();
   while (auto sub = NextSubEvent())
      sub->PrintEvent();
}

void TGo4SimpleEvent::Clear(Option_t *)
{
   GO4TRACE((11,"TGo4SimpleEvent::Clear()",__LINE__, __FILE__));
   // here iterate all subevents and clear them
   // too slow!!
   //   ResetIterator();
   //   while (auto sub = NextSubEvent())
   //      {
   //         sub->Clear();
   //      }
   for (Int_t i = 0; i < fiMaxSlot; ++i) {
      auto sub = dynamic_cast<TGo4SimpleSubEvent *>(fxSubEventArray->UncheckedAt(i));
      sub->Clear();
   }
   fiCount = 0;
}


Int_t TGo4SimpleEvent::Fill()
{
   GO4TRACE((11,"TGo4SimpleEvent::Fill()",__LINE__, __FILE__));
   //
   // check for different source types
   Int_t rev = -1;

   if(CheckEventSource("TGo4SimpleEventProcessor")) {
      auto proc = (TGo4SimpleEventProcessor *) GetEventSource();
      if (proc->BuildSimpleEvent(this))
         rev = 0;
      else
         rev = 1;
   } else if(CheckEventSource("TGo4TreeSource")) {
      // find out here if tree contains correct event structure...
      // under constr.
      auto source = (TGo4TreeSource *) GetEventSource();
      Clear();
      if(source->BuildEvent(this))
         rev = 0;
      else
         rev = 1; // error, may be end of tree..
   } else if(CheckEventSource("TGo4FileSource")) {
      // find out here if tree contains correct event structure...
      // under constr.
      auto source = (TGo4FileSource *) GetEventSource();
      Clear();
      if(source->BuildEvent(this))
         rev = 0;
      else
         rev = 1; // error, may be end of tree..
   } else {
      rev = 1;
   }

   return rev;
}

TGo4SimpleSubEvent *TGo4SimpleEvent::GetSubEvent(Short_t procid)
{
   GO4TRACE((11,"TGo4SimpleEvent::GetSubEvent(Short_t)",__LINE__, __FILE__));
   // this is slow:
   //   ResetIterator();
   //   while (auto sub = NextSubEvent())
   //      {
   //         if(sub->fsProcid == procid)
   //               return sub;
   //      }
   // Int_t i = 0;
   //   while ((sub = dynamic_cast<TGo4SimpleSubEvent*>( fxSubEventArray->At(i++)) ) != nullptr)
   for (Int_t i = 0; i < fiMaxSlot; ++i) {
      auto sub = dynamic_cast<TGo4SimpleSubEvent *>(fxSubEventArray->UncheckedAt(i));
      if (sub->fsProcid == procid)
         return sub;
   }

   return nullptr;
}

Int_t  TGo4SimpleEvent::GetCount() const
{
   return fiCount;
}

TGo4SimpleSubEvent * TGo4SimpleEvent::AddSubEvent(Short_t procid)
{
   GO4TRACE((11,"TGo4SimpleEvent::AddSubEvent(Short_t)",__LINE__, __FILE__));

   TGo4SimpleSubEvent* result = GetSubEvent(procid);
   if(!result)  {
      // nothing found, use the last one
      if(fiLastSlot+1<fiMaxSlot)
      {
         fiLastSlot++;
         result = new( (*fxSubEventArray) [fiLastSlot] ) TGo4SimpleSubEvent();
         result->fsProcid = procid;
         TGo4Log::Debug(" SimpleEvent: Added subevent with procid %d:  ", procid);
      }
      else
      {
         TGo4Log::Error("simple event: clonesarray is full, cannot add another subevent");
         result = nullptr;
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
   return (TGo4SimpleSubEvent *) fxIterator ->Next();
}
