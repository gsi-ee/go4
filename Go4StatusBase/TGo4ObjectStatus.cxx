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

#include "TGo4ObjectStatus.h"

#include "TDatime.h"
#include "TClass.h"

#include "TGo4Log.h"
#include "TGo4Picture.h"

TGo4ObjectStatus::TGo4ObjectStatus() :
   TGo4Status()
{
}

TGo4ObjectStatus::TGo4ObjectStatus(TObject *object, Bool_t withtime) :
   TGo4Status("Initname","InitTitle")
{
   GO4TRACE((15,"TGo4ObjectStatus::TGo4ObjectStatus(TNamed*)",__LINE__, __FILE__));
   if(object) {
      SetName(object->GetName());
      SetTitle(object->GetTitle());
      fxObjectClass = object->ClassName();
      fiObjectSize = object->IsA()->Size();
      if (object->InheritsFrom(TGo4Picture::Class()))
         fiObjectSize = ((TGo4Picture *) object)->GetTotalSize();
      fbDeleteProtect = !object->TestBit(TGo4Status::kGo4CanDelete);
      fbResetProtect = object->TestBit(TGo4Status::kGo4NoReset);
   } else {
      SetName("Unknown Name");
      SetTitle("Unknown Title");
      fxObjectClass = "Unknown Class";
   }

   if (withtime) {
      TDatime now;
      fxStatusTime = now.AsSQLString();
   }
   // we might suppress time information for substatus objects
   // for performance reason
}

TGo4ObjectStatus::~TGo4ObjectStatus()
{
   GO4TRACE((15,"TGo4ObjectStatus::~TGo4ObjectStatus()",__LINE__, __FILE__));

}

void TGo4ObjectStatus::Print(Option_t*) const
{
   PrintLine("G-OOOO-> Object Status Class Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintLine(" Object name: \t\t%s", GetName());
   PrintLine(" Object title: \t\t%s", GetTitle());
   PrintLine(" Object class: \t\t%s", GetObjectClass());
   PrintLine(" Object size: \t\t%d byte", GetObjectSize());
   PrintLine(" Clear protection: \t\t%d", IsResetProtect());
   PrintLine(" Delete protection: \t\t%d", IsDeleteProtect());
   PrintLine(" Status creation time: \t%s", GetTimeString());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
}
