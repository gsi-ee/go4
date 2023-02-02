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

#include "TGo4DynamicListException.h"

#include "TGo4Log.h"
#include "TGo4DynamicEntry.h"

TGo4DynamicListException & TGo4DynamicListException::operator= (const TGo4DynamicListException & right)
{
   GO4TRACE((14, "TGo4DynamicListException:operator=", __LINE__, __FILE__));
   if (&right != this) {
      TGo4RuntimeException::operator=(right); // copy base class members
      fxEntry = right.fxEntry;
      fxEntryName = right.fxEntryName;
      fxEntryClass = right.fxEntryClass;
      fxStatusMessage = right.fxStatusMessage;
   }

   return *this;
}

TGo4DynamicListException::TGo4DynamicListException(const TGo4DynamicListException &right) : TGo4RuntimeException(right)
{
   GO4TRACE((14, "TGo4DynamicListException:TGo4DynamicListException(right)", __LINE__, __FILE__));

   fxEntry = right.fxEntry;
   fxEntryName = right.fxEntryName;
   fxEntryClass = right.fxEntryClass;
   fxStatusMessage = right.fxStatusMessage;
}

Int_t TGo4DynamicListException::Handle()
{
   GO4TRACE((12, "TGo4AnalyisStepException::Handle()", __LINE__, __FILE__));

   TGo4Log::Debug("%s \n Entry %s:%s, %s\n \t--Entry was disabled.", What(), GetEntryClass(), GetEntryName(),
                  GetStatusMessage());
   if (fxEntry)
      fxEntry->EnableProcessing(kFALSE);
   return 0;
}

TGo4DynamicListException::TGo4DynamicListException(TGo4DynamicEntry *entry, const char *message)
{
   GO4TRACE((14, "TGo4DynamicListException:TGo4DynamicListException (TGo4DynamicEntry *,..)", __LINE__, __FILE__));
   fxDescription = "!!!-- Go4 Dynamic List Exception --!!!";

   fxEntry = entry;
   if (fxEntry) {
      fxEntryName = fxEntry->GetName();
      fxEntryClass = fxEntry->ClassName();
   }
   fxStatusMessage = message;
}
