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

#include "TGo4DynamicEntry.h"

#include "TGo4Log.h"
#include "TGo4Status.h"

TGo4DynamicEntry::TGo4DynamicEntry() :
   TNamed(),
   fbProcessEntry(kTRUE)
{
   GO4TRACE((15,"TGo4DynamicEntry::TGo4DynamicEntry()",__LINE__, __FILE__));
}

TGo4DynamicEntry::TGo4DynamicEntry(const char* name, const char* title) :
   TNamed(name, title),
   fbProcessEntry(kTRUE)
{
   GO4TRACE((15,"TGo4DynamicEntry::TGo4DynamicEntry(char*)",__LINE__, __FILE__));
   SetBit(TGo4Status::kGo4CanDelete);
}

TGo4DynamicEntry::~TGo4DynamicEntry()
{
   GO4TRACE((15,"TGo4DynamicEntry::~TGo4DynamicEntry()",__LINE__, __FILE__));
}
