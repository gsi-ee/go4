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

#include "TGo4EventStoreParameter.h"

#include "go4iostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"

TGo4EventStoreParameter::~TGo4EventStoreParameter()
{
   TRACE((12,"TGo4EventStoreParameter::~TGo4EventStoreParameter()",__LINE__, __FILE__));
}
TGo4EventStoreParameter::TGo4EventStoreParameter(const char* name, Int_t id)
: TGo4Parameter (name), fiID(id)
{
   TRACE((12,"TGo4EventStoreParameter::TGo4EventStoreParameter(const char*)",__LINE__, __FILE__));
}
TGo4EventStoreParameter::TGo4EventStoreParameter()
: TGo4Parameter ("Default Event Store Parameter"), fiID(0)
{
   TRACE((12,"TGo4EventStoreParameter::TGo4EventStoreParameter()",__LINE__, __FILE__));
}
Int_t TGo4EventStoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4EventStoreParameter::PrintParameter()",__LINE__, __FILE__));
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
   current=TGo4Status::PrintBuffer(current,restlen, "Eventstore:  \t\t%s \n",woparam.Data());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Name:  \t\t%s \n",GetName());
//   current=TGo4Status::PrintIndent(current,restlen);
//   current=TGo4Status::PrintBuffer(current,restlen, "  ID:  \t\t%d \n",GetID());
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

Bool_t TGo4EventStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4EventStoreParameter::UpdateFrom()",__LINE__, __FILE__));
   TGo4EventStoreParameter* storpar=dynamic_cast<TGo4EventStoreParameter*>(rhs);
   if (storpar==0) return kFALSE;
   SetName(storpar->GetName());
   fiID=storpar->GetID();
   return kTRUE;
}
