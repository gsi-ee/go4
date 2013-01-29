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

#include "Riostream.h"
#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4ExampleServer.h"

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file

   TGo4ExampleServer* myserver = new TGo4ExampleServer("MyExampleServer", 0, kFALSE);
      // second parameter is negotiation portnumber
   std::cout << "Created Example Server Instance:"<<myserver->GetName()<<std::endl; // dummy action for warnings
   theApp.Run();
   return 0;
}
