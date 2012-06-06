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

#include "TGo4AnalysisClientStatus.h"

#include "go4iostream.h"

#include "TDatime.h"

#include "TGo4Log.h"

TGo4AnalysisClientStatus::TGo4AnalysisClientStatus() :
  TGo4ClientStatus(),
  fuCurrentCount(0),
  fdRate(0),
  fdAveRate(0),
  fdTime(0),
  fbAnalysisRunning(false)
{
}

TGo4AnalysisClientStatus::TGo4AnalysisClientStatus(const char* name) :
  TGo4ClientStatus(name),
  fuCurrentCount(0),
  fdRate(0),
  fdAveRate(0),
  fdTime(0),
  fbAnalysisRunning(false)
{
   TRACE((12,"TGo4AnalysisClientStatus::TGo4AnalysisClientStatus(const char*)",__LINE__, __FILE__));
}

TGo4AnalysisClientStatus::~TGo4AnalysisClientStatus()
{
}

Int_t TGo4AnalysisClientStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4AnalysisClientStatus::PrintStatus()",__LINE__, __FILE__));
   //
   Int_t locallen=4096;
   Text_t localbuf[4096];
   if(buflen<=0 && buffer!=0) return 0;
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;
   current=PrintBuffer(current,restlen,"G-OOOO->    Analysis Client Status Class %s Printout:  <-OOOO-G\n",GetName());
   current=PrintBuffer(current,restlen,"G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintBuffer(current,restlen,"G-OOOO-> Analysis Running State: %d\n",IsAnalysisRunning());
   current=PrintBuffer(current,restlen,"G-OOOO-> Current event rate: %f\n",GetRate());
   current=PrintBuffer(current,restlen,"G-OOOO-> Average event rate: %f\n",GetAvRate());
   current=PrintBuffer(current,restlen,"G-OOOO-> Total event counts: %d\n",GetCurrentCount());
   current=PrintBuffer(current,restlen,"G-OOOO-> Total time: %f\n",GetTime());
   current=PrintBuffer(current,restlen,"G-OOOO-> Current Event source: %s\n",GetCurrentSource());
   current=PrintBuffer(current,restlen,"G-OOOO-> Creation date: %s\n",GetDateTime());
   current=PrintBuffer(current,restlen,"G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   Int_t delta=TGo4ClientStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintBuffer(current,restlen,"G-OOOO-> END Analysis Client Status Class Printout END <-OOOO-G\n");
   if(buffer==0)
      {
         cout << localbuf << endl;
      }
   else
      {
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

void TGo4AnalysisClientStatus::SetRates(Double_t rate, Double_t average, ULong64_t counts, Double_t time)
{
   TRACE((12,"TGo4AnalysisClientStatus::SetRates(...)",__LINE__, __FILE__));
   fdRate=rate;
   fdAveRate=average;
   fuCurrentCount=counts;
   fdTime=time;
   TDatime now;
   fxDateString=now.AsSQLString();
}
