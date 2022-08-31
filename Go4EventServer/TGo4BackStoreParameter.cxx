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

#include "TGo4BackStoreParameter.h"

#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4BackStoreParameter::TGo4BackStoreParameter(const char *name,
                                              Int_t splitlevel,
                                              Int_t bufsize)
: TGo4EventStoreParameter(name, GO4EV_BACK),
   fiSplit(splitlevel), fiBufsize(bufsize)
{
   SetTitle(name);
}

TGo4BackStoreParameter::TGo4BackStoreParameter()
: TGo4EventStoreParameter("Default Go4 FileStore", GO4EV_BACK)
{
   SetTitle("Go4FileStore-Tree");
}

TGo4BackStoreParameter::~TGo4BackStoreParameter()
{
}

void TGo4BackStoreParameter::Print(Option_t*) const
{
   TGo4EventStoreParameter::Print();
   TGo4Status::PrintLine("  Split level: \t%d", fiSplit);
   TGo4Status::PrintLine("  Buffer size: \t%d bytes", fiBufsize);
}

Bool_t TGo4BackStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   auto backpar = dynamic_cast<TGo4BackStoreParameter*>(rhs);
   if(!backpar) return kFALSE;
   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetSplitlevel(backpar->GetSplitlevel());
   SetBufsize(backpar->GetBufsize());
   return kTRUE;
}
