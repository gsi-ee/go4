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

#include "TGo4ParameterStatus.h"

#include "TROOT.h"
#include "TClass.h"
#include "TObjArray.h"

#include "TGo4Log.h"
#include "TGo4Parameter.h"

TGo4ParameterStatus::TGo4ParameterStatus() :
   TGo4ObjectStatus()
{
}

TGo4ParameterStatus::TGo4ParameterStatus(TGo4Parameter *par, Bool_t membervalues) :
   TGo4ObjectStatus(par)
{
   if (par && membervalues) {
      fxMemberValues = new TObjArray();
      fxMemberValues->SetOwner(kTRUE);
      par->GetMemberValues(fxMemberValues);
   }
}

TGo4ParameterStatus::TGo4ParameterStatus(const char *parname, const char *parclass, TObjArray* items) :
   TGo4ObjectStatus()
{
   fxMemberValues = items;
   SetName(parname);
   SetTitle("TGo4ParameterStatus title");
   fxObjectClass = parclass;
}


TGo4ParameterStatus::~TGo4ParameterStatus()
{
   if (fxMemberValues) {
      delete fxMemberValues;
      fxMemberValues = nullptr;
   }
}

TObjArray* TGo4ParameterStatus::GetMemberValues(Bool_t takeit)
{
   TObjArray* res = fxMemberValues;
   if (takeit) fxMemberValues = nullptr;
   return res;
}

Bool_t TGo4ParameterStatus::UpdateParameterValues(TGo4Parameter *par)
{
   if (!par || !fxMemberValues) return kFALSE;
   if (strcmp(GetObjectClass(), par->ClassName()) != 0) return kFALSE;

   if (par->CustomUpdateFrom()) {
      TGo4Parameter *copypar = CreateParameter();
      par->UpdateFrom(copypar);
      delete copypar;
   } else {
      par->SetMemberValues(fxMemberValues);
   }

   return kTRUE;

}

TGo4Parameter *TGo4ParameterStatus::CreateParameter()
{
   auto parclass = gROOT->GetClass(GetObjectClass());
   if (!parclass || !fxMemberValues) return nullptr;

   TGo4Parameter *par = (TGo4Parameter*) parclass->New();

   if (!par) return nullptr;

   par->SetName(GetName());
   par->SetMemberValues(fxMemberValues);

   return par;
}

void TGo4ParameterStatus::Print(Option_t*) const
{
   TGo4ObjectStatus::Print();
   PrintLine("G-OOOO-> Parameter Status Class Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
}
