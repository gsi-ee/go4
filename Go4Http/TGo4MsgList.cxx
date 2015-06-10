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
   fCounter(0)
{
}

TGo4MsgList::TGo4MsgList(const char* name, const char* title) :
   TNamed(name, title),
   fMsgs(),
   fLimit(0),
   fCounter(0)
{
   fMsgs.SetOwner(kTRUE);
}

TGo4MsgList::~TGo4MsgList()
{
   fMsgs.Delete();
}
