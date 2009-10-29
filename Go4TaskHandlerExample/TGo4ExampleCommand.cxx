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

#include "TGo4ExampleCommand.h"

#include "TGo4Log.h"

TGo4ExampleCommand::TGo4ExampleCommand() : TGo4Command()
{
   TRACE((12,"TGo4ExampleCommand::TGo4ExampleCommand() ctor",__LINE__, __FILE__));
   //fxReceiver=0;

   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("ExampleApplication");  // sets the receiver name checked by command invoker
                                             // receiver name neednt be class name!
}

TGo4ExampleCommand::TGo4ExampleCommand(const TGo4ExampleCommand &right)
  :TGo4Command(right)
{
   TRACE((12,"TGo4ExampleCommand::TGo4ExampleCommand() copy constructor",__LINE__, __FILE__));
   //fxReceiver=right.fxReceiver;
}
TGo4ExampleCommand::~TGo4ExampleCommand()
{
  TRACE((12,"TGo4ExampleCommand::~TGo4ExampleCommand() dtor",__LINE__, __FILE__));
}

TGo4ExampleCommand::TGo4ExampleCommand(const char* name, const char* desc)
: TGo4Command(name,desc,6)
{
   TRACE((12,"TGo4ExampleCommand::TGo4ExampleCommand(const char*, const char*) ctor",__LINE__, __FILE__));
   //fxReceiver=0;

   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("ExampleApplication");  // sets the receiver name checked by command invoker
                                           // receiver name neednt be class name!
}
