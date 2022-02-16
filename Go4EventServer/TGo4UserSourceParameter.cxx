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

#include "TGo4UserSourceParameter.h"

#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4UserSourceParameter::TGo4UserSourceParameter(const char* name, const char* expr, Int_t port)
: TGo4EventSourceParameter(name, GO4EV_USER), fiPort(port), fxExpression(expr)
{
}

TGo4UserSourceParameter::TGo4UserSourceParameter()
: TGo4EventSourceParameter("default user event source", GO4EV_USER)
{
}

TGo4UserSourceParameter::~TGo4UserSourceParameter()
{
}

void TGo4UserSourceParameter::Print(Option_t*) const
{
   TGo4EventSourceParameter::Print();
   if (GetPort() > 0)
      TGo4Status::PrintLine(" Port  %d", GetPort());
   TGo4Status::PrintLine(" Expression: %s", GetExpression());
}

Bool_t TGo4UserSourceParameter::UpdateFrom(TGo4Parameter* rhs)
{
   if(rhs && rhs->InheritsFrom(TGo4UserSourceParameter::Class())) {
      auto userpar = dynamic_cast<TGo4UserSourceParameter*>(rhs);
      if(!userpar) return kFALSE;
      if(!TGo4EventSourceParameter::UpdateFrom(rhs)) return kFALSE;
      SetPort(userpar->GetPort());
      SetExpression(userpar->GetExpression());
      return kTRUE;
   }
   return kFALSE;
}

