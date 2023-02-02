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

#include "TGo4UserStoreParameter.h"

#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4UserStoreParameter::TGo4UserStoreParameter(const char *name)
: TGo4EventStoreParameter(name, GO4EV_USER)
{
   SetTitle(name);
}

TGo4UserStoreParameter::TGo4UserStoreParameter()
: TGo4EventStoreParameter("Default Go4 UserStore", GO4EV_USER)
{
   SetTitle("Go4UserStore");
}

TGo4UserStoreParameter::~TGo4UserStoreParameter()
{
}

void TGo4UserStoreParameter::Print(Option_t *) const
{
   TGo4EventStoreParameter::Print();
   TGo4Status::PrintLine("  Expression: %s", GetExpression());
}


Bool_t TGo4UserStoreParameter::UpdateFrom(TGo4Parameter *rhs)
{
   auto filepar = dynamic_cast<TGo4UserStoreParameter*>(rhs);

   if(filepar && TGo4EventStoreParameter::UpdateFrom(rhs)) {
      SetExpression(filepar->GetExpression());
      return kTRUE;
   }

   return kFALSE;
}
