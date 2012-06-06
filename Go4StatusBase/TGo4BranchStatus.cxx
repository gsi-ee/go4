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

#include "TGo4BranchStatus.h"

#include "go4iostream.h"

#include "TGo4Log.h"

#include "TBranch.h"
#include "TBranchElement.h"

TGo4BranchStatus::TGo4BranchStatus() :
   TGo4ObjectStatus()
{
}

TGo4BranchStatus::TGo4BranchStatus(TBranch* branch) :
   TGo4ObjectStatus(branch, kFALSE)
{
   if (branch!=0)
      {
         if(branch->InheritsFrom(TBranchElement::Class()))
            fxObjectClass = (dynamic_cast<TBranchElement*>(branch))->GetTypeName();
         else
            fxObjectClass = branch->GetClassName();
      }
   if (fxObjectClass.Length()==0)
     fxObjectClass = branch ? branch->ClassName() : "BranchElement";
}

TGo4BranchStatus::~TGo4BranchStatus()
{
}

Int_t TGo4BranchStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   //return 0;
   TRACE((12,"TGo4BranchStatus::PrintStatus()",__LINE__, __FILE__));
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=128000;
   Text_t localbuf[128000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta= TGo4ObjectStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> TreeBranch Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
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
