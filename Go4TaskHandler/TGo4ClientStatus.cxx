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

#include "TGo4ClientStatus.h"

#include "Riostream.h"
#include "TGo4Log.h"


TGo4ClientStatus::TGo4ClientStatus() :
   TGo4TaskStatus()
{
}

TGo4ClientStatus::TGo4ClientStatus(const char* name) :
   TGo4TaskStatus(name)
{
}

TGo4ClientStatus::~TGo4ClientStatus()
{
}

Int_t TGo4ClientStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=4096;
   Text_t localbuf[4096];
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;
   current=PrintBuffer(current,restlen, "G-OOOO->    Client Status Class %s Printout:  <-OOOO-G\n",GetName());
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> Remote Server Hostname: %s\n",fxServerHostName.Data());
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   Int_t delta=TGo4TaskStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintBuffer(current,restlen, "G-OOOO-> END Client Status Class Printout END <-OOOO-G\n");
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

void TGo4ClientStatus::SetNames(const char* serverhost)
{
   fxServerHostName = serverhost;
}
