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

#include "TGo4EventSourceParameter.h"

#include "TGo4Status.h"
#include "TGo4Log.h"

TGo4EventSourceParameter::TGo4EventSourceParameter(const char* name, Int_t id) :
   TGo4Parameter (name), fiID(id)
{
}

TGo4EventSourceParameter::TGo4EventSourceParameter() :
   TGo4Parameter ("Default Event Source Parameter")
{
}

TGo4EventSourceParameter::~TGo4EventSourceParameter()
{
}

void TGo4EventSourceParameter::Print(Option_t*) const
{
   TString woparam = ClassName();
   woparam.ReplaceAll("Parameter","");
   TGo4Status::PrintLine("Eventsource: %s", woparam.Data());
   TGo4Status::PrintLine("  Name: \t%s",GetName());
   TGo4Status::PrintLine("  Timeout: \t%d s",GetTimeout());
   TGo4Status::PrintLine("  ID: \t\t%d",GetID());
}

Bool_t TGo4EventSourceParameter::UpdateFrom(TGo4Parameter* rhs)
{
   auto srcpar = dynamic_cast<TGo4EventSourceParameter*>(rhs);
   if (!srcpar) return kFALSE;
   SetName(srcpar->GetName());
   SetTimeout(srcpar->GetTimeout());
   fiID = srcpar->GetID();
   return kTRUE;
}
