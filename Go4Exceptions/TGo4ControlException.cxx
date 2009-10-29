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

#include "TGo4ControlException.h"

#include "TApplication.h"

#include "TGo4Log.h"

TGo4ControlException::TGo4ControlException()
{
   TRACE((14,"TGo4ControlException::TGo4ControlException() constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Control Exception --!!!";
}

TGo4ControlException::TGo4ControlException(const TGo4ControlException &right)
   :TGo4RuntimeException(right)
{
   TRACE((14,"TGo4ControlException::TGo4ControlException() copy constructor",__LINE__, __FILE__));
}


TGo4ControlException::~TGo4ControlException()
{
  TRACE((14,"TGo4ControlException::~TGo4ControlException() destructor",__LINE__, __FILE__));
}


TGo4ControlException & TGo4ControlException::operator=(const TGo4ControlException &right)
{
  TRACE((14,"TGo4ControlException::operator=",__LINE__, __FILE__));
  if (&right!=this)
    {
      TRACE((13,"TGo4ControlException::operator= processing copy",__LINE__, __FILE__));
      TGo4RuntimeException::operator=(right); // copy base class members
      // put additional member copies here...
      return *this;
    }
  else
    {
      // copy is already source object
      TRACE((13,"TGo4ControlException::operator= source and destination objects are identical",__LINE__, __FILE__));
      return *this;
    }
}

Int_t TGo4ControlException::Handle()
{
   TRACE((14,"TGo4ControlException::Handle()",__LINE__, __FILE__));

   TGo4Exception::Handle(); // superclass handler: output of exception
   TGo4Log::Debug("\n !!! Unspecified Control Exception, aborting application !!! \n");
   gApplication->Terminate(0);
   return 0;
}

