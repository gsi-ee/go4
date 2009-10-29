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

#include "TGo4RevServParameter.h"

#include "Riostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"


TGo4RevServParameter::TGo4RevServParameter(const char* name, Int_t defport) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_REVSERV),
   fiPort(defport)
{
   TRACE((14,"TGo4MbsRevServParameter::TGo4MbsRevServParameter(const char*,...)", __LINE__, __FILE__));
}

TGo4RevServParameter::TGo4RevServParameter() :
   TGo4MbsSourceParameter("default mbs remote event server", GO4EV_MBS_REVSERV),
   fiPort(0)
{
TRACE((14,"TGo4MbsRevServParameter::TGo4MbsRevServParameter()", __LINE__, __FILE__));
}

TGo4RevServParameter::~TGo4RevServParameter()
{
   TRACE((14,"TGo4MbsRevServParameter::~TGo4MbsRevServParameter()", __LINE__, __FILE__));
}


Int_t TGo4RevServParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4RevServParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta=TGo4MbsSourceParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Revserv port: \t%d \n",GetPort());
   if(buffer==0)
      {
          cout << localbuf << endl;
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

Bool_t TGo4RevServParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4RevServParameter::UpdateFrom()",__LINE__, __FILE__));
   if((rhs!=0) && rhs->InheritsFrom(TGo4RevServParameter::Class())) {
      TGo4RevServParameter* mbspar=dynamic_cast<TGo4RevServParameter*>(rhs);
      if(!mbspar) return kFALSE;
      if(!TGo4MbsSourceParameter::UpdateFrom(rhs)) return kFALSE;
      SetPort(mbspar->GetPort());
      return kTRUE;
   }

   return kFALSE;
}
