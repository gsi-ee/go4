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

#include "TGo4EventStoreException.h"

#include "TGo4EventStore.h"
#include "TGo4Log.h"

TGo4EventStoreException::TGo4EventStoreException (TGo4EventStore* eventstore, const char *msg)
: fxEventStore(nullptr)
{
   GO4TRACE((16,"TGo4EventStoreException::TGo4EventStoreException (TGo4EventStore*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 EventStore Exception --!!!";
   if(eventstore)
      {
         fxEventStore=eventstore;
         fxStoreName=eventstore->GetName();
         fxStoreClass=eventstore->ClassName();
      }
   if(msg) fxErrMess=msg;
}


TGo4EventStoreException::TGo4EventStoreException(const TGo4EventStoreException &right)
  :TGo4RuntimeException(right)
{
 GO4TRACE((16,"TGo4EventStoreException::TGo4EventStoreException(right)", __LINE__, __FILE__));   ;
   fxEventStore=right.fxEventStore;
   fxErrMess=right.fxErrMess;
   fxStoreName=right.fxStoreName;
   fxStoreClass=right.fxStoreClass;
}

TGo4EventStoreException & TGo4EventStoreException::operator= (const TGo4EventStoreException & right)
{
   GO4TRACE((16, "TGo4EventStoreException::operator=", __LINE__, __FILE__));
   if (&right != this) {
      TGo4RuntimeException::operator=(right); // copy base class members
      fxEventStore = right.fxEventStore;
      fxErrMess = right.fxErrMess;
      fxStoreName = right.fxStoreName;
      fxStoreClass = right.fxStoreClass;
   }
   return *this;
}

TGo4EventStoreException::~TGo4EventStoreException()
{
   GO4TRACE((16,"TGo4EventStoreException::~TGo4EventStoreException", __LINE__, __FILE__));
}

Int_t TGo4EventStoreException::Handle()
{
   GO4TRACE((12,"TGo4EventStoreException::Handle()", __LINE__, __FILE__));

   TGo4Log::Debug("%s \n %s of name %s:\n\t%s",
            What(),
            GetStoreClass(), GetStoreName()
            ,GetErrMess());

   return 0;
}

