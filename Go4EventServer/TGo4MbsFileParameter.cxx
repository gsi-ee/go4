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

#include "TGo4MbsFileParameter.h"

#include "TObjString.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

const char *TGo4MbsFile__fgcNOTAGFILE = "GO4-NOLMDTAG";

TGo4MbsFileParameter::TGo4MbsFileParameter() :
    TGo4MbsSourceParameter()
{
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::TGo4MbsFileParameter(const char *name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_FILE)
{
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::~TGo4MbsFileParameter()
{
}

void TGo4MbsFileParameter::AddMoreFile(const char *more)
{
   TGo4Log::Debug("Add more lmd file %s", more);

   fxMoreFiles.SetOwner(kTRUE);
   fxMoreFiles.Add(new TObjString(more));
}

const char *TGo4MbsFileParameter::GetMoreName(Int_t n) const
{
   return (n >= 0) && (n <= fxMoreFiles.GetLast()) ? fxMoreFiles.At(n)->GetName() : nullptr;
}

void TGo4MbsFileParameter::Print(Option_t *) const
{
   TGo4MbsSourceParameter::Print();
   TGo4Status::PrintLine("Tagfile: %s", GetTagName());
}

Bool_t TGo4MbsFileParameter::UpdateFrom(TGo4Parameter *rhs)
{
   if(rhs && rhs->InheritsFrom(TGo4MbsFileParameter::Class())) {
      auto mbspar = dynamic_cast<TGo4MbsFileParameter*>(rhs);
      if(!mbspar) return kFALSE;
      if(!TGo4MbsSourceParameter::UpdateFrom(rhs)) return kFALSE;
      SetTagName(mbspar->GetTagName());
      return kTRUE;
   }

   return kFALSE;
}
