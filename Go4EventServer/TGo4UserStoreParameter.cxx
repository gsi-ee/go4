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

#include "TGo4UserStoreParameter.h"

#include "go4iostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4UserStoreParameter::TGo4UserStoreParameter(const char* name)
: TGo4EventStoreParameter(name, GO4EV_USER)
{
   SetTitle(name);
}

TGo4UserStoreParameter::TGo4UserStoreParameter()
: TGo4EventStoreParameter("Default Go4 UserStore", GO4EV_USER)
{
   SetTitle("Go4UserStore");
}

TGo4UserStoreParameter::~TGo4UserStoreParameter()
{

}

Int_t TGo4UserStoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
 TRACE((12,"TGo4UserStoreParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   Int_t delta=TGo4EventStoreParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Expression: %s\n",GetExpression());
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


Bool_t TGo4UserStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TGo4UserStoreParameter* filepar=dynamic_cast<TGo4UserStoreParameter*>(rhs);

   if(filepar!=0)
      return TGo4EventStoreParameter::UpdateFrom(rhs);

   return kFALSE;
}
