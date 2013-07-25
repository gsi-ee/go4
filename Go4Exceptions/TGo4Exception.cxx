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

#include "TGo4Exception.h"

#include "TGo4Log.h"

TGo4Exception::TGo4Exception()
  :fxDescription("!!!-- Go4 General Exception --!!!")
{
  GO4TRACE((14,"TGo4Exception::TGo4Exception() constructor",__LINE__, __FILE__));
}

TGo4Exception::TGo4Exception(const TGo4Exception &right)
{
   GO4TRACE((14,"TGo4Exception::TGo4Exception() copy constructor",__LINE__, __FILE__));
   fxDescription=right.fxDescription;
}


TGo4Exception::~TGo4Exception()
{
  GO4TRACE((14,"TGo4Exception::TGo4Exception() destructor",__LINE__, __FILE__));
}


TGo4Exception & TGo4Exception::operator=(const TGo4Exception &right)
{
  GO4TRACE((14,"TGo4Exception::operator=",__LINE__, __FILE__));
  if (&right!=this)
    {
      GO4TRACE((13,"TGo4Exception::operator= processing copy",__LINE__, __FILE__));
      fxDescription=right.fxDescription;
      return *this;
    }
  else
    {
      // copy is already source object
      GO4TRACE((13,"TGo4Exception::operator= source and destination objects are identical",__LINE__, __FILE__));
      return *this;
    }
}

const char* TGo4Exception::What ()
{
  GO4TRACE((14,"TGo4Exception::What()",__LINE__, __FILE__));
  return fxDescription.Data();
}

Int_t TGo4Exception::Handle ()
{
   GO4TRACE((14,"TGo4Exception::Handle()",__LINE__, __FILE__));

   TGo4Log::Debug(" %s was thrown! ",What());
   return 0;
}
