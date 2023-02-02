// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4AnalysisClientStatus.h"

#include "TDatime.h"

TGo4AnalysisClientStatus::TGo4AnalysisClientStatus() :
  TGo4ClientStatus()
{
}

TGo4AnalysisClientStatus::TGo4AnalysisClientStatus(const char *name) :
  TGo4ClientStatus(name)
{
}

TGo4AnalysisClientStatus::~TGo4AnalysisClientStatus()
{
}

void TGo4AnalysisClientStatus::Print(Option_t *) const
{
   PrintLine("G-OOOO->    Analysis Client Status Class %s Printout:  <-OOOO-G", GetName());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintLine("G-OOOO-> Analysis Running State: %d",IsAnalysisRunning());
   PrintLine("G-OOOO-> Current event rate: %f",GetRate());
   PrintLine("G-OOOO-> Average event rate: %f",GetAvRate());
   PrintLine("G-OOOO-> Total event counts: %llu", (long long unsigned) GetCurrentCount());
   PrintLine("G-OOOO-> Total time: %f", GetTime());
   PrintLine("G-OOOO-> Current Event source: %s", GetCurrentSource());
   PrintLine("G-OOOO-> Creation date: %s", GetDateTime());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   TGo4ClientStatus::Print();
   PrintLine("G-OOOO-> END Analysis Client Status Class Printout END <-OOOO-G");
}

void TGo4AnalysisClientStatus::SetRates(Double_t rate, Double_t average, ULong64_t counts, Double_t time)
{
   fdRate = rate;
   fdAveRate = average;
   fuCurrentCount = counts;
   fdTime = time;
   TDatime now;
   fxDateString=now.AsSQLString();
}
