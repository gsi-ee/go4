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

#include "TGo4ComAction1.h"

#include "TGo4Log.h"
#include "TGo4ExampleApplication.h"

Int_t TGo4ComAction1::ExeCom()
{
   GO4TRACE((12,"TGo4ComAction1::ExeCom() dtor",__LINE__, __FILE__));

   if (fxReceiverBase!=0)
      {
      GO4TRACE((11,"TGo4ComAction1::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction1 ''%s'': Action1 requested ",
            GetName());
         ((TGo4ExampleApplication*) fxReceiverBase)->CommandAction1();
      }
   else
      {
      GO4TRACE((11,"TGo4ComAction1::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction1 ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
TGo4ComAction1::~TGo4ComAction1()
{
   GO4TRACE((12,"TGo4ComAction1::~TGo4ComAction1() dtor",__LINE__, __FILE__));
}

TGo4ComAction1::TGo4ComAction1()
:TGo4ExampleCommand()
{
   GO4TRACE((12,"TGo4ComAction1::TGo4ComAction1() ctor",__LINE__, __FILE__));
   SetName("EXAction1");
   SetDescription("executes example application action1 method");
}
