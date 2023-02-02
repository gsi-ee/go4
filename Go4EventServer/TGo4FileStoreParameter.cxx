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

#include "TGo4FileStoreParameter.h"

#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4FileStoreParameter::TGo4FileStoreParameter(const char *name,
                                               Int_t splitlevel,
                                               Int_t bufsize,
                                               Int_t compression,
                                               Int_t autosave)
: TGo4EventStoreParameter(name, GO4EV_FILE),
   fiSplit(splitlevel), fiBufsize(bufsize),
   fiCompression(compression), fiAutosavesize(autosave), fbOverwrite(1)
{
   SetTitle(name);
}

TGo4FileStoreParameter::TGo4FileStoreParameter()
: TGo4EventStoreParameter("Default Go4 FileStore", GO4EV_FILE)
{
   SetTitle("Go4FileStore-Tree");
}

TGo4FileStoreParameter::~TGo4FileStoreParameter()
{
}

void TGo4FileStoreParameter::Print(Option_t *) const
{
   TGo4EventStoreParameter::Print();
   TGo4Status::PrintLine("  Split level: \t%d", fiSplit);
   TGo4Status::PrintLine("  Buffer size: \t%d bytes", fiBufsize);
   TGo4Status::PrintLine("  Compression level: \t%d", fiCompression);
   TGo4Status::PrintLine("  Tree autosave at: \t%d bytes", fiAutosavesize);
}

Bool_t TGo4FileStoreParameter::UpdateFrom(TGo4Parameter *rhs)
{
   auto filepar = dynamic_cast<TGo4FileStoreParameter*>(rhs);
   if (!filepar) return kFALSE;

   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetSplitlevel(filepar->GetSplitlevel());
   SetBufsize(filepar->GetBufsize());
   SetCompression(filepar->GetCompression());
   SetOverwriteMode(filepar->IsOverwriteMode());
   SetAutosaveSize(filepar->GetAutosaveSize());
   return kTRUE;
}
