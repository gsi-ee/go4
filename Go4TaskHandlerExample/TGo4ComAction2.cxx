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

#include "TGo4ComAction2.h"

#include "TGo4Log.h"
#include "TGo4ExampleApplication.h"

Int_t TGo4ComAction2::ExeCom()
{
   GO4TRACE((12,"TGo4ComAction2::ExeCom() dtor",__LINE__, __FILE__));

   if (fxReceiverBase!=0) {
      GO4TRACE((11,"TGo4ComAction2::ExeCom() - found valid receiver",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! ComAction2 ''%s'': Action2 requested ", GetName());
      ((TGo4ExampleApplication*) fxReceiverBase) ->CommandAction2();
   } else {
      GO4TRACE((11,"TGo4ComAction2::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! ComAction2 ''%s'': NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   return -1;
}

TGo4ComAction2::~TGo4ComAction2()
{
   GO4TRACE((12,"TGo4ComAction2::~TGo4ComAction2() dtor",__LINE__, __FILE__));
}

TGo4ComAction2::TGo4ComAction2() :TGo4ExampleCommand()
{
   GO4TRACE((12,"TGo4ComAction2::TGo4ComAction2() ctor",__LINE__, __FILE__));
   SetName("EXAction2");
   SetDescription("executes example application action2 method");
}
