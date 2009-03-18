#include "TGo4ExampleController.h"

#include "Riostream.h"

#include "TObject.h"
#include "TCanvas.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4Command.h"

#include "TGo4ComAction1.h"

TGo4ExampleController::TGo4ExampleController() :
  fxPreviousData(0), fxCanvas(0)
{
  TRACE((15,"TGo4ExampleController::TGo4ExampleController() constructor",__LINE__, __FILE__));
  fxCanvas=new TCanvas("Example Display","Go4 controller canvas",3);
}

TGo4ExampleController::~TGo4ExampleController()
{
  TRACE((15,"TGo4ExampleController::~TGo4ExampleController() destructor",__LINE__, __FILE__));
  delete fxPreviousData;
  delete fxCanvas;
}

void TGo4ExampleController::DisplayData(TObject* data)
{
   TRACE((12,"TGo4ExampleController::DisplayData(TNamed*)",__LINE__, __FILE__));
  if(data)
    {
     delete fxPreviousData;
     fxPreviousData=data;
      cout << "ExampleController received data object:"<<endl;
      cout << "Classname:"<< data->ClassName()<<", Name:" << data->GetName()<< endl;
      fxCanvas->cd();
      fxPreviousData->Draw();
      fxCanvas->Modified();
      fxCanvas->Update();
    }
  else
    {
      cout << "ExampleController has NULL data object"<<endl;
    }
}

void TGo4ExampleController::DisplayLog(TGo4Status* status)
{
  TRACE((12,"TGo4ExampleController::DisplayLog(TGo4Status*)",__LINE__, __FILE__));
  if(status)
    {
      cout << "ExampleController received status object:"<<endl;
      if(status->InheritsFrom("TGo4ExampleClientStatus"))
         {
            status->PrintStatus(); // display client status object
         }
     else
      {
            cout << status->GetName()<< endl; // status message
      }
      delete status; // avoid memory leak
    }
  else
    {
      cout << "ExampleController has NULL status object"<<endl;
    }

}

TGo4Command* TGo4ExampleController::GetCommand()
{
  TRACE((12,"TGo4ExampleController::GetCommand()",__LINE__, __FILE__));
    // this function simulates a user input of a command, e.g. using a cli or a gui
    TGo4Command* com = new TGo4ComAction1();
    return com;
}
