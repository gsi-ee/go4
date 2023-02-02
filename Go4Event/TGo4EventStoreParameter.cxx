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

#include "TGo4EventStoreParameter.h"

#include "TGo4Status.h"

TGo4EventStoreParameter::~TGo4EventStoreParameter()
{
}

TGo4EventStoreParameter::TGo4EventStoreParameter(const char *name, Int_t id)
: TGo4Parameter (name), fiID(id)
{
}

TGo4EventStoreParameter::TGo4EventStoreParameter()
: TGo4Parameter ("Default Event Store Parameter")
{
}

void TGo4EventStoreParameter::Print(Option_t *) const
{
   TString woparam = ClassName();
   woparam.ReplaceAll("Parameter","");
   TGo4Status::PrintLine("Eventstore:  \t\t%s",woparam.Data());
   TGo4Status::PrintLine("  Name:  \t\t%s", GetName());
   TGo4Status::PrintLine("  ID:  \t\t%d", GetID());
}

Bool_t TGo4EventStoreParameter::UpdateFrom(TGo4Parameter *rhs)
{
   auto storpar = dynamic_cast<TGo4EventStoreParameter*>(rhs);
   if (!storpar) return kFALSE;
   SetName(storpar->GetName());
   fiID = storpar->GetID();
   return kTRUE;
}
