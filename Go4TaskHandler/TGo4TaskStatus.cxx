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

#include "TGo4TaskStatus.h"

#include "go4iostream.h"

#include "TGo4TaskHandlerStatus.h"

TGo4TaskStatus::TGo4TaskStatus() :
   TGo4Status(),
   fxTaskHandlerStatus(0)
{
}

TGo4TaskStatus::TGo4TaskStatus(const char* name) :
   TGo4Status(name),
   fxTaskHandlerStatus(0)
{
}

TGo4TaskStatus::~TGo4TaskStatus()
{
   delete fxTaskHandlerStatus;
}

void TGo4TaskStatus::SetFlags(Bool_t blocking,
                                 Bool_t autocreate,
                                 Bool_t autostart,
                                 Bool_t terminating,
                                 Bool_t initdone)
{
   fbAppBlocking=blocking;
   fbAutoCreate=autocreate;
   fbAutoStart=autostart;
   fbTerminating=terminating;
   fbInitDone=initdone;
}
void TGo4TaskStatus::SetTaskHandlerStatus(TGo4TaskHandlerStatus* thstate)
{
   fxTaskHandlerStatus=thstate;

}
Int_t TGo4TaskStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   //
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=4096;
   Text_t localbuf[4096];
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;
   current=PrintBuffer(current,restlen, "G-OOOO->    Client Status Class %s Printout:  <-OOOO-G\n",GetName());
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> Blockingmode: %d\n",fbAppBlocking);
   current=PrintBuffer(current,restlen, "G-OOOO-> Autocreate: %d\n",fbAutoCreate);
   current=PrintBuffer(current,restlen, "G-OOOO-> Autostart: %d\n",fbAutoStart);
   current=PrintBuffer(current,restlen, "G-OOOO-> InitDone: %d\n",fbInitDone);
   current=PrintBuffer(current,restlen, "G-OOOO-> IsTerminating: %d\n",fbTerminating);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   Int_t delta=fxTaskHandlerStatus->PrintStatus(current,restlen);
   restlen-=delta;
   current+= delta;
   current=PrintBuffer(current,restlen, "G-OOOO-> END Client Status Class Printout END <-OOOO-G\n");
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
