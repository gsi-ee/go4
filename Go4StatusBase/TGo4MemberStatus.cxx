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

#include "TGo4MemberStatus.h"

#include "Riostream.h"

TGo4MemberStatus::TGo4MemberStatus() :
   TGo4ObjectStatus()
{
}

TGo4MemberStatus::TGo4MemberStatus(const char* name, const char* datatypename) :
   TGo4ObjectStatus(0,kFALSE)
{
   SetName(name);
   SetTitle(datatypename);
   fxObjectClass = datatypename;
}

TGo4MemberStatus::~TGo4MemberStatus()
{
}

Int_t TGo4MemberStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=128000;
   Text_t localbuf[128000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta   = TGo4ObjectStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> Datamember Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
  if(buffer==0)
      {
     std::cout << localbuf << std::endl;
      }
   else
      {
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}
