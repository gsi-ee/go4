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

#include "TGo4MsgList.h"

#include "TDatime.h"

TGo4MsgList::TGo4MsgList() :
   TNamed(),
   fMsgs(),
   fLimit(0),
   fCounter(0),
   fSelect(),
   fStrCounter()
{
}

TGo4MsgList::TGo4MsgList(const char* name, const char* title, Int_t limit) :
   TNamed(name, title),
   fMsgs(),
   fLimit(limit > 0 ? limit : 1),
   fCounter(0),
   fSelect(),
   fStrCounter()
{
   fMsgs.SetOwner(kTRUE);

   // counter initialized from current time
   // if application restarted, id will be bigger and request from GUI will not lead to messages lost
   // of course, if more than 1000 messages per second are generated, one could have mismatch

   fCounter = ((Long64_t) TDatime().Get()) * 1000;
}

TGo4MsgList::~TGo4MsgList()
{
   fMsgs.Clear();
}

void TGo4MsgList::AddMsg(const char* msg)
{
   // add message to the list
   // if number of stored messages bigger than configured, old messages will be removed
   // zero (msg==0) messages will not be add to the list

   while (fMsgs.GetSize() >= GetLimit()) {
      TObject* last = fMsgs.Last();
      fMsgs.RemoveLast();
      delete last;
   }
   if (msg==0) return;

   fMsgs.AddFirst(new TObjString(msg));
   fCounter++;
}

TList* TGo4MsgList::Select(Int_t max, Long64_t id)
{
   TIter iter(&fMsgs);
   TObject* obj = 0;
   Long64_t curr = fCounter;
   fSelect.Clear();

   if (max<=0) max = fMsgs.GetSize() + 1;

   // add current id as first string in the list
   fStrCounter.SetString(TString::LLtoa(fCounter, 10));

   fSelect.Add(&fStrCounter);
   while (((obj = iter()) != 0) && (--curr >= id) && (--max >= 0))
      fSelect.Add(obj);

   return &fSelect;
}

