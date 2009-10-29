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

#include "TGo4EventSourceParameter.h"

#include "Riostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"

const Int_t TGo4EventSource__fgiTIMEOUTDEFAULT = 1;

TGo4EventSourceParameter::TGo4EventSourceParameter(const char* name, Int_t id)
: TGo4Parameter (name), fiID(id),
   fiTimeout(TGo4EventSource__fgiTIMEOUTDEFAULT)
{
   TRACE((12,"TGo4EventSourceParameter::TGo4EventSourceParameter(const char*)",__LINE__, __FILE__));
}


TGo4EventSourceParameter::TGo4EventSourceParameter()
: TGo4Parameter ("Default Event Source Parameter"), fiID(0),
   fiTimeout(TGo4EventSource__fgiTIMEOUTDEFAULT)
{
   TRACE((12,"TGo4EventSourceParameter::TGo4EventSourceParameter()",__LINE__, __FILE__));
}

TGo4EventSourceParameter::~TGo4EventSourceParameter()
{
   TRACE((12,"TGo4EventSourceParameter::~TGo4EventSourceParameter()",__LINE__, __FILE__));
}

Int_t TGo4EventSourceParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4EventSourceParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=64000;
   Text_t localbuf[64000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   current=TGo4Status::PrintIndent(current,restlen);
   TString woparam=ClassName();
   woparam.ReplaceAll("Parameter","");
   current=TGo4Status::PrintBuffer(current,restlen, "Eventsource: %s \n",woparam.Data());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Name: \t%s \n",GetName());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Timeout: \t%d s\n",GetTimeout());
//   current=TGo4Status::PrintIndent(current,restlen);
//   current=TGo4Status::PrintBuffer(current,restlen, "  ID: \t\t%d \n",GetID());
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

Bool_t TGo4EventSourceParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4EventSourceParameter::UpdateFrom()",__LINE__, __FILE__));
   TGo4EventSourceParameter* srcpar=dynamic_cast<TGo4EventSourceParameter*>(rhs);
   if (srcpar==0) return kFALSE;
   SetName(srcpar->GetName());
   SetTimeout(srcpar->GetTimeout());
   fiID=srcpar->GetID();
   return kTRUE;
}
