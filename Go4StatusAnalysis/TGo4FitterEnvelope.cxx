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

#include "TGo4FitterEnvelope.h"

#include "TGo4Status.h"

TGo4FitterEnvelope::TGo4FitterEnvelope()
: TGo4Parameter("Fitter envelope")
{
}

TGo4FitterEnvelope::TGo4FitterEnvelope(const char* name, TGo4Fitter* fitter)
: TGo4Parameter(name)
{
   fxFitter = fitter;
}

TGo4FitterEnvelope::~TGo4FitterEnvelope()
{
   if(fxFitter) delete fxFitter;
}

void TGo4FitterEnvelope::Print(Option_t*) const
{
   TGo4Parameter::Print();
   if(fxFitter)
      TGo4Status::PrintLine("\n This object contains the fitter: %s",fxFitter->GetName());
   else
      TGo4Status::PrintLine(" \t This object contains no fitter");
}

Bool_t TGo4FitterEnvelope::UpdateFrom(TGo4Parameter *pp)
{
   auto from = dynamic_cast<TGo4FitterEnvelope *> (pp);

   if(!from) return kFALSE;

   if(fxFitter) delete fxFitter;
   fxFitter = from->GetFitter(kTRUE);// change ownership to us
   return kTRUE;
}

void TGo4FitterEnvelope::Clear(Option_t* opt)
{
   if(fxFitter) fxFitter->Clear(opt);
}

TGo4Fitter* TGo4FitterEnvelope::GetFitter(Bool_t chown)
{
   TGo4Fitter* rev=fxFitter;
   if(chown) fxFitter = nullptr;
   return rev;
}

void TGo4FitterEnvelope::SetFitter(TGo4Fitter* fitter)
{
   if(fxFitter) delete fxFitter;
   fxFitter = fitter;
}
