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

#include "TGo4EventSourceException.h"

#include "TGo4ControlException.h"
#include "TGo4EventSource.h"
#include "TGo4Log.h"

TGo4EventSourceException::TGo4EventSourceException (TGo4EventSource* eventsource)
: fxEventSource(0),  fiCreateStatus(0), fiEventStatus(0)
{
   GO4TRACE((16,"TGo4EventSourceException::TGo4EventSourceException (TGo4EventSource*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 EventSource Exception --!!!";
   if(eventsource)
      {
         fxEventSource=eventsource;
         fxErrMess=eventsource->GetErrMess();
         fxSourceName=eventsource->GetName();
         fxSourceClass=eventsource->ClassName();
         fiCreateStatus=eventsource->GetCreateStatus();
         fiEventStatus=eventsource->GetEventStatus();
      }
}


TGo4EventSourceException::TGo4EventSourceException(const TGo4EventSourceException &right)
  :TGo4RuntimeException(right)
{
 GO4TRACE((16,"TGo4EventSourceException::TGo4EventSourceException(right)", __LINE__, __FILE__));   ;
   fxEventSource=right.fxEventSource;
   fxErrMess=right.fxErrMess;
   fxSourceName=right.fxSourceName;
   fxSourceClass=right.fxSourceClass;
   fiCreateStatus=right.fiCreateStatus;
   fiEventStatus=right.fiEventStatus;
}

TGo4EventSourceException & TGo4EventSourceException::operator= (const TGo4EventSourceException & right)
{
 GO4TRACE((16,"TGo4EventSourceException::operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TGo4RuntimeException::operator=(right); // copy base class members
      // put additional member copies here...
      fxEventSource=right.fxEventSource;
      fxErrMess=right.fxErrMess;
      fxSourceName=right.fxSourceName;
      fxSourceClass=right.fxSourceClass;
      fiCreateStatus=right.fiCreateStatus;
      fiEventStatus=right.fiEventStatus;
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }
}

TGo4EventSourceException::~TGo4EventSourceException()
{
 GO4TRACE((16,"TGo4EventSourceException::~TGo4EventSourceException", __LINE__, __FILE__));

}

Int_t TGo4EventSourceException::Handle()
{
   GO4TRACE((12,"TGo4EventSourceException::Handle()", __LINE__, __FILE__));

   TGo4Log::Debug("%s \n %s of name %s: \n\tCreateStatus: %d \n\tEventStatus:%d, \n\t%s",
            What(),
            GetSourceClass(), GetSourceName(),
            GetCreateStatus(), GetEventStatus(),GetErrMess());

   return 0;
}

