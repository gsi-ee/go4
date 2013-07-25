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

#include "TGo4MbsFileParameter.h"

#include "TObjString.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

const char* TGo4MbsFile__fgcNOTAGFILE = "GO4-NOLMDTAG";

TGo4MbsFileParameter::TGo4MbsFileParameter() :
    TGo4MbsSourceParameter(),
    fxTagFile(),
    fxMoreFiles()
{
   GO4TRACE((14,"TGo4MbsFileParameter::TGo4MbsFileParameter()", __LINE__, __FILE__));
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::TGo4MbsFileParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_FILE),
   fxTagFile(),
   fxMoreFiles()
{
   GO4TRACE((14,"TGo4MbsFileParameter::TGo4MbsFileParameter(const char*,...)", __LINE__, __FILE__));
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::~TGo4MbsFileParameter()
{
   GO4TRACE((14,"TGo4MbsFileParameter::~TGo4MbsFileParameter()", __LINE__, __FILE__));
}

void TGo4MbsFileParameter::AddMoreFile(const char* more)
{
   TGo4Log::Debug("Add more lmd file %s", more);

   fxMoreFiles.SetOwner(kTRUE);
   fxMoreFiles.Add(new TObjString(more));
}

const char* TGo4MbsFileParameter::GetMoreName(Int_t n) const
{
   return (n>=0) && (n<=fxMoreFiles.GetLast()) ? fxMoreFiles.At(n)->GetName() : 0;
}


Int_t TGo4MbsFileParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   GO4TRACE((12,"TGo4MbsFileParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen = locallen;
   Text_t* current = localbuf;

   Int_t delta = TGo4MbsSourceParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+= delta;
   current=TGo4Status::PrintBuffer(current,restlen, "Tagfile: %s \n",GetTagName());
   if(buffer==0)
      {
      std::cout << localbuf << std::endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
            size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

Bool_t TGo4MbsFileParameter::UpdateFrom(TGo4Parameter* rhs)
{
   GO4TRACE((12,"TGo4MbsFileParameter::UpdateFrom()",__LINE__, __FILE__));
   if((rhs!=0) && rhs->InheritsFrom(TGo4MbsFileParameter::Class())) {
      TGo4MbsFileParameter* mbspar=dynamic_cast<TGo4MbsFileParameter*>(rhs);
      if(!mbspar) return kFALSE;
      if(!TGo4MbsSourceParameter::UpdateFrom(rhs)) return kFALSE;
      SetTagName(mbspar->GetTagName());
      return kTRUE;
   }

   return kFALSE;
}
