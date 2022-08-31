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

#include "TGo4ExampleClientStatus.h"

#include "TH1.h"

#include "TGo4Log.h"

TGo4ExampleClientStatus::TGo4ExampleClientStatus(const char *name) : TGo4ClientStatus(name)
{
  GO4TRACE((12,"TGo4ExampleClientStatus::TGo4ExampleClientStatus(const char*)",__LINE__, __FILE__));
}

TGo4ExampleClientStatus::~TGo4ExampleClientStatus()
{
  GO4TRACE((12,"TGo4ExampleClientStatus::~TGo4ExampleClientStatus()",__LINE__, __FILE__));
}

void TGo4ExampleClientStatus::SetHistoStatus(TH1* histogram)
{
   GO4TRACE((12,"TGo4ClientStatus::SetHistoStatus(TH1*)",__LINE__, __FILE__));
   fxHistogramName = histogram->GetName();
   // here the complete internal histogram information may be stored; we leave that
   // as an exercise for the reader!
}

void TGo4ExampleClientStatus::SetNames(const char *main, const char *watch)
{
   GO4TRACE((12,"TGo4ClientStatus::SetNames(const char*,...)",__LINE__, __FILE__));
   fxMainName = main;
   fxWatchName = watch;
}

void TGo4ExampleClientStatus::Print(Option_t*) const
{
   PrintLine("G-OOOO->    ExampleClient Status Class %s Printout:  <-OOOO-G", GetName());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintLine("G-OOOO-> Main Runnable Name: %s", fxMainName.Data());
   PrintLine("G-OOOO-> Watch Runnable Name: %s", fxWatchName.Data());
   PrintLine("G-OOOO-> Demo Histogram Name: %s", fxHistogramName.Data());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   TGo4ClientStatus::Print(); // gives us the superclass data
   PrintLine("G-OOOO-> END Example Client Status Class Printout END <-OOOO-G");
}

