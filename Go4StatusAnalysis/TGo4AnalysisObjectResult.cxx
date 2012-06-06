// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4AnalysisObjectResult.h"

#include "go4iostream.h"
#include "TROOT.h"

#include "TGo4Log.h"

#include "TGo4AnalysisObjectNames.h"


TGo4AnalysisObjectResult::TGo4AnalysisObjectResult() :
   TGo4Status(),
   fxNamesList(0),
   fxFullName(),
   fiAction(kGo4ActionNul),
   fxMessage()
{
}

TGo4AnalysisObjectResult::TGo4AnalysisObjectResult(const char* name) :
   TGo4Status(name),
   fxNamesList(0),
   fxFullName(),
   fiAction(kGo4ActionNul),
   fxMessage()
{
}

TGo4AnalysisObjectResult::~TGo4AnalysisObjectResult()
{
   // names list is not owned by the result object
   //  delete fxNamesList;
   // fxNamesList=0;
}

TGo4AnalysisObjectNames* TGo4AnalysisObjectResult::GetNamesList(Bool_t chown)
{
   TGo4AnalysisObjectNames* reval = fxNamesList;
   if(chown) fxNamesList=0;
   return reval;
}

Int_t TGo4AnalysisObjectResult::PrintStatus(Text_t* buffer, Int_t buflen)
{
   if(buflen<=0 && buffer!=0)
      return 0;
   gROOT->SetDirLevel(0);
   Int_t locallen=64000;
   Text_t localbuf[64000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   current=PrintBuffer(localbuf,restlen,"G-OOOO-> Analysis Object Result Printout <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");

   if(fxNamesList) {
      Int_t delta=fxNamesList->PrintStatus(current,restlen);
      restlen-=delta;
      current+= delta;
   }
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Full object name: \t%s\n",GetObjectFullName());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Message: \t%s\n",GetMessage());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Action: \t%d\n",Action());
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
