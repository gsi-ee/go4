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

#include "TGo4LogicException.h"

#include "TGo4Log.h"

TGo4LogicException::TGo4LogicException()
{
  GO4TRACE((14,"TGo4LogicException::TGo4LogicException() constructor",__LINE__, __FILE__));
  fxDescription= "!!!-- Go4 Logic Exception --!!!";
}

TGo4LogicException::TGo4LogicException(const TGo4LogicException &right)
  :TGo4Exception(right)
{
   GO4TRACE((14,"TGo4LogicException::TGo4LogicException() copy constructor",__LINE__, __FILE__));
}


TGo4LogicException::~TGo4LogicException()
{
  GO4TRACE((14,"TGo4LogicException::~TGo4LogicException() destructor",__LINE__, __FILE__));
}


TGo4LogicException & TGo4LogicException::operator=(const TGo4LogicException &right)
{
  GO4TRACE((14,"TGo4LogicException::operator=",__LINE__, __FILE__));
  if (&right!=this)
    {
      GO4TRACE((13,"TGo4LogicException::operator= processing copy",__LINE__, __FILE__));
      TGo4Exception::operator=(right); // copy base class members
      // put additional member copies here...
      return *this;
    }
  else
    {
      // copy is already source object
      GO4TRACE((13,"TGo4LogicException::operator= source and destination objects are identical",__LINE__, __FILE__));
      return *this;
    }
}

Int_t TGo4LogicException::Handle ()
{
   GO4TRACE((14,"TGo4LogicException::Handle()",__LINE__, __FILE__));
   TGo4Exception::Handle(); // basic handle method printsout exception type
   return 0;
}




