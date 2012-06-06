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

#include "go4iostream.h"
#include <stdlib.h>
#include "TGo4Log.h"

#include "TApplication.h"
#include "TGo4AnalysisClient.h"

void usage();

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
   // define debug output section:
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file


   if(argc<4)
      {
         usage();

      }
   else
      {
         const char* name=argv[1];
         const char* hostname=argv[2];
         const char* connector=argv[3];
         UInt_t con=atoi(connector);
         cout << "Client:"<<name<<",\tHost:"<<hostname<<",\tConnector:"<<con<<endl;

         // variant 1: use demo with internal plain analysis base class
         TGo4AnalysisClient* myclient = new TGo4AnalysisClient(name,0,hostname,con);

         // variant 2: create user analysis and pass it to client, client adopts it
//         TEbAnalysis* myanalysis= new TEbAnalysis;
//         myanalysis->SetAutoSaveFile("EbAutoSave");
//         myanalysis->SetAutoSaveInterval(200000);
//         TGo4AnalysisClient* myclient = new TGo4AnalysisClient(name,myanalysis,hostname,con);


         cout << "Created AnalysisClient Instance: "<<myclient->GetName()<<endl; // dummy action for warnings
         theApp.Run();

         return 0;

   } // if(argc<4)

//} // if (rev)

}


void usage()
{
cout << "usage: MainGo4AnalysisClient clientname serverhostname connectorport"<<endl;

}



