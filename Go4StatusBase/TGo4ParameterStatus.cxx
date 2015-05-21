// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ParameterStatus.h"

#include "Riostream.h"
#include "TObjArray.h"
#include "TROOT.h"
#include "TClass.h"

#include "TGo4Log.h"
#include "TGo4Parameter.h"

TGo4ParameterStatus::TGo4ParameterStatus() :
   TGo4ObjectStatus(),
   fxMemberValues(0)
{
}

TGo4ParameterStatus::TGo4ParameterStatus(TGo4Parameter* par, Bool_t membervalues) :
   TGo4ObjectStatus(par),
   fxMemberValues(0)
{
   if ((par!=0) && membervalues) {
      fxMemberValues = new TObjArray();
      fxMemberValues->SetOwner(kTRUE);
      par->GetMemberValues(fxMemberValues);
   }
}

TGo4ParameterStatus::TGo4ParameterStatus(const char* parname, const char* parclass, TObjArray* items) :
   TGo4ObjectStatus(),
   fxMemberValues(items)
{
   SetName(parname);
   SetTitle("TGo4ParameterStatus title");
   fxObjectClass = parclass;
}


TGo4ParameterStatus::~TGo4ParameterStatus()
{
   if (fxMemberValues!=0) delete fxMemberValues;
}

TObjArray* TGo4ParameterStatus::GetMemberValues(Bool_t takeit)
{
   TObjArray* res = fxMemberValues;
   if (takeit) fxMemberValues = 0;
   return res;
}

Bool_t TGo4ParameterStatus::UpdateParameterValues(TGo4Parameter* par)
{
   if ((par==0) || (fxMemberValues==0)) return kFALSE;
   if (strcmp(GetObjectClass(), par->ClassName())!=0) return kFALSE;

   if (par->CustomUpdateFrom()) {
      TGo4Parameter* copypar = CreateParameter();
      par->UpdateFrom(copypar);
      delete copypar;
   } else {
      par->SetMemberValues(fxMemberValues);
   }

   return kTRUE;

}

TGo4Parameter* TGo4ParameterStatus::CreateParameter()
{
   TClass* parclass = gROOT->GetClass(GetObjectClass());
   if ((parclass==0) || (fxMemberValues==0)) return 0;

   TGo4Parameter* par = (TGo4Parameter*) parclass->New();

   if (par==0) return 0;

   par->SetName(GetName());
   par->SetMemberValues(fxMemberValues);

   return par;
}

Int_t TGo4ParameterStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   GO4TRACE((12,"TGo4ParameterStatus::PrintStatus()",__LINE__, __FILE__));
   if(buflen<=0 && buffer!=0) return 0;
   Int_t locallen = 128000;
   Text_t localbuf[128000];
   Int_t size=0;
   Text_t* current = localbuf;
   Int_t restlen = locallen;
   Int_t delta = TGo4ObjectStatus::PrintStatus(current,restlen);
   restlen -= delta;
   current += delta;
   current = PrintIndent(current, restlen);
   current = PrintBuffer(current, restlen, "G-OOOO-> Parameter Status Class Printout <-OOOO-G\n");
   current = PrintIndent(current, restlen);
   current = PrintBuffer(current, restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   // put printout of condition infos here:

   ////
   if(buffer==0) {
      std::cout << localbuf << std::endl;
   } else {
      size = locallen-restlen;
      if(size>buflen-1) size = buflen-1;
      strncpy(buffer,localbuf,size);
   }
   return size;
}
