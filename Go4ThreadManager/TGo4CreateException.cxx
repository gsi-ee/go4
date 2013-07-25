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

#include "TGo4CreateException.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4CreateException::TGo4CreateException(const TGo4CreateException &right)
  :TGo4ThreadException(right)
{
   GO4TRACE((14,"TGo4CreateException::TGo4CreateException() copy constructor",__LINE__, __FILE__));
}

TGo4CreateException::TGo4CreateException (TGo4Runnable* runnable, const char* threadname)
:TGo4ThreadException(runnable,threadname)
{
   GO4TRACE((14,"TGo4CreateException::TGo4CreateException(TGo4Runnable*, const char*) constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Create Exception --!!!";
}


TGo4CreateException::~TGo4CreateException()
{
 GO4TRACE((14,"TGo4CreateException::~TGo4CreateException() destructor",__LINE__, __FILE__));
}


TGo4CreateException & TGo4CreateException::operator=(const TGo4CreateException &right)
{
   GO4TRACE((14,"TGo4CreateException::operator=",__LINE__, __FILE__));
   if (&right!=this)
      {
         GO4TRACE((13,"TGo4CreateException::operator= processing copy",__LINE__, __FILE__));
         TGo4ThreadException::operator=(right); // copy base class members
         // put additional member copies here...
         return *this;
      }
   else
      {
         // copy is already source object
         GO4TRACE((13,"TGo4CreateException::operator= source and destination objects are identical",__LINE__, __FILE__));
         return *this;
      }
}

Int_t TGo4CreateException::Handle ()
{
   GO4TRACE((14,"TGo4CreateException::Handle()",__LINE__, __FILE__));
   std::cout << "\t This is the TGo4CreateException::Handler"<<std::endl;
   Int_t rev=-1;
   if(GetThreadName()==0)
      // no threadname specified, operate on thread of runnable
      {
         GO4TRACE((13,"TGo4CreateException::Handle() -- creating thread of runnable",__LINE__, __FILE__));
         rev=fxRunnable->GetThread()->Create();
      }
   else
      // threadname given, use thread handler to cancel
      {
         GO4TRACE((13,"TGo4CreateException::Handle() -- creating thread by name",__LINE__, __FILE__));
         rev=fxThreadHandler->Create(GetThreadName());
      }
   // note: boolean return values of called methods are implicitly casted to int
   return rev;
}




