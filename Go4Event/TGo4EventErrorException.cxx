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

#include "TGo4EventErrorException.h"

#include "TGo4Log.h"

TGo4EventErrorException::TGo4EventErrorException (TGo4EventSource* eventsource, Int_t prio)
: TGo4EventSourceException(eventsource)
{
   TRACE((16,"TGo4EventErrorException::TGo4EventErrorException (TGo4EventSource*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Eventsource Error Exception --!!!";
   fiPriority=prio;
}

TGo4EventErrorException::~TGo4EventErrorException()
{
   TRACE((16,"TGo4EventErrorException::~TGo4EventErrorException()", __LINE__, __FILE__));
}

TGo4EventErrorException::TGo4EventErrorException(const TGo4EventErrorException &right)
  :TGo4EventSourceException(right)
{
  TRACE((16,"TGo4EventErrorException::TGo4EventErrorException(right)", __LINE__, __FILE__));   ;
  fiPriority=right.fiPriority;
}

TGo4EventErrorException & TGo4EventErrorException::operator= (const TGo4EventErrorException & right)
{
 TRACE((16,"TGo4EventErrorException::operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TGo4EventSourceException::operator=(right); // copy base class members
      // put additional member copies here...
      fiPriority=right.fiPriority;
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }
}
