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

#include "TGo4ExampleController.h"

#include <iostream>
#include "TCanvas.h"

#include "TGo4Log.h"
#include "TGo4Status.h"

#include "TGo4ComAction1.h"

TGo4ExampleController::TGo4ExampleController()
{
   GO4TRACE((15,"TGo4ExampleController::TGo4ExampleController()",__LINE__, __FILE__));
   fxCanvas = new TCanvas("Example Display","Go4 controller canvas",3);
}

TGo4ExampleController::~TGo4ExampleController()
{
   GO4TRACE((15,"TGo4ExampleController::~TGo4ExampleController()",__LINE__, __FILE__));
   delete fxPreviousData;
   delete fxCanvas;
}

void TGo4ExampleController::DisplayData(TObject *data)
{
   GO4TRACE((12,"TGo4ExampleController::DisplayData(TObject *)",__LINE__, __FILE__));
   if (data) {
      delete fxPreviousData;
      fxPreviousData = data;
      std::cout << "ExampleController received data object:" << std::endl;
      std::cout << "Classname:" << data->ClassName() << ", Name:"
            << data->GetName() << std::endl;
      fxCanvas->cd();
      fxPreviousData->Draw();
      fxCanvas->Modified();
      fxCanvas->Update();
   } else {
      std::cout << "ExampleController has NULL data object" << std::endl;
   }
}

void TGo4ExampleController::DisplayLog(TGo4Status *status)
{
   GO4TRACE((12,"TGo4ExampleController::DisplayLog(TGo4Status *)",__LINE__, __FILE__));
   if (status) {
      std::cout << "ExampleController received status object:" << std::endl;
      if (status->InheritsFrom("TGo4ExampleClientStatus")) {
         status->Print(); // display client status object
      } else {
         std::cout << status->GetName() << std::endl; // status message
      }
      delete status; // avoid memory leak
   } else {
      std::cout << "ExampleController has NULL status object" << std::endl;
   }
}

TGo4Command *TGo4ExampleController::GetCommand()
{
   GO4TRACE((12,"TGo4ExampleController::GetCommand()",__LINE__, __FILE__));
   // this function simulates a user input of a command, e.g. using a cli or a gui
   TGo4Command *com = new TGo4ComAction1();
   return com;
}
