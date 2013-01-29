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

#include "TGo4ObjectStatus.h"

#include "Riostream.h"
#include "TDatime.h"
#include "TClass.h"

#include "TGo4Log.h"
#include "TGo4Picture.h"

TGo4ObjectStatus::TGo4ObjectStatus() :
   TGo4Status(),
   fiObjectSize(0),
   fbResetProtect(kFALSE),
   fbDeleteProtect(kTRUE),
   fxObjectClass(),
   fxStatusTime()
{
}

TGo4ObjectStatus::TGo4ObjectStatus(TObject* object, Bool_t withtime) :
   TGo4Status("Initname","InitTitle"),
   fiObjectSize(0),
   fbResetProtect(kFALSE),
   fbDeleteProtect(kTRUE),
   fxObjectClass(),
   fxStatusTime()
{
   TRACE((15,"TGo4ObjectStatus::TGo4ObjectStatus(TNamed*)",__LINE__, __FILE__));
   if(object) {
      SetName(object->GetName());
      SetTitle(object->GetTitle());
      fxObjectClass = object->ClassName();
      fiObjectSize = object->IsA()->Size();
      if (object->InheritsFrom(TGo4Picture::Class()))
         fiObjectSize = ((TGo4Picture*) object)->GetTotalSize();
      fbDeleteProtect = !object->TestBit(TGo4Status::kGo4CanDelete);
      fbResetProtect = object->TestBit(TGo4Status::kGo4NoReset);
   } else {
      SetName("Unknown Name");
      SetTitle("Unknown Title");
      fxObjectClass="Unknown Class";
   }

   if (withtime) {
      TDatime now;
      fxStatusTime = now.AsSQLString();
   }
   // we might suppress time information for substatus objects
   // for performance reason
}

TGo4ObjectStatus::~TGo4ObjectStatus()
{
   TRACE((15,"TGo4ObjectStatus::~TGo4ObjectStatus()",__LINE__, __FILE__));

}

Int_t TGo4ObjectStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4ObjectStatus::PrintStatus()",__LINE__, __FILE__));
   if(buflen<=0 && buffer!=0) return 0;
   Int_t locallen=2048;
   Text_t localbuf[2048];
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen,"G-OOOO-> Object Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object name: \t\t%s\n",GetName());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object title: \t\t%s\n",GetTitle());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object class: \t\t%s\n",GetObjectClass());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object size: \t\t%d byte\n",GetObjectSize());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Clear protection: \t\t%d\n",IsResetProtect());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Delete protection: \t\t%d\n",IsDeleteProtect());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Status creation time: \t%s\n",GetTimeString());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
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
