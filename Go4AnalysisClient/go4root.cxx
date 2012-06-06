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

#include "go4iostream.h"

#include "TGo4Log.h"

#include "TRint.h"
#include "TGo4AnalysisClient.h"

void usage();

/**
* This nice executable starts a rootcint session with a
* go4 slave server ("analysisclient" in server mode) in the
* background. Since TGo4Analysis::Instance()
* is available from the macro line, user might register any object
* and let it be visible from the go4 gui.
* @author Joern Adamczewski
* @since 06-apr-2005
*/



int main(int argc, char **argv)
{
   TRint theApp("Go4CINT", &argc, argv);
   // define debug output section:
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file
   // use demo with internal dummy analysis base class
   TGo4AnalysisClient* myclient = new TGo4AnalysisClient(
                                             "Go4CintServer", // name
                                             0, // external analysis (use internal if 0)
                                             "localhost", // node (dummy)
                                             1234, // port (dummy)
                                             0,  // histogram server on/off
                                             "", // histogram server basename
                                             "", // histogram server passwd
                                             kTRUE // start this slave as server
                                             );
    myclient->SetCintMode(kTRUE);
    cout << "Created Slave Instance: "<<myclient->GetName()<<endl; // dummy action for warnings
    theApp.Run();
    return 0;
}





