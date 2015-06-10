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

#include "TGo4MsgList.h"

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

TList* TGo4MsgList::Select(Long64_t id)
{
   TIter iter(&fMsgs);
   TObject* obj = 0;
   Long64_t curr = fCounter;
   fSelect.Clear();

   // add current id as first string in the list
   fStrCounter.SetString(TString::LLtoa(fCounter, 10));

   fSelect.Add(&fStrCounter);
   while ((obj = iter()) != 0) {
      if (--curr < id) break;
      fSelect.Add(obj);
   }

   return &fSelect;
}

