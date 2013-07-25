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

#include "TGo4EventTimeoutException.h"

#include "TGo4Log.h"

TGo4EventTimeoutException::TGo4EventTimeoutException (TGo4EventSource* eventsource)
: TGo4EventSourceException(eventsource)
{
   GO4TRACE((16,"TGo4EventTimeoutException::TGo4EventTimeoutException (TGo4EventSource*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 TaskHandler Event Timeout Exception --!!!";
}

TGo4EventTimeoutException::TGo4EventTimeoutException(const TGo4EventTimeoutException &right)
  :TGo4EventSourceException(right)
{
 GO4TRACE((16,"TGo4EventTimeoutException::TGo4EventTimeoutException(right)", __LINE__, __FILE__));   ;
}

TGo4EventTimeoutException & TGo4EventTimeoutException::operator= (const TGo4EventTimeoutException & right)
{
 GO4TRACE((16,"TGo4EventTimeoutException::operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TGo4EventSourceException::operator=(right); // copy base class members
      // put additional member copies here...
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }
}

TGo4EventTimeoutException::~TGo4EventTimeoutException()
{
   GO4TRACE((16,"TGo4EventTimeoutException::~TGo4EventTimeoutException()", __LINE__, __FILE__));
}
