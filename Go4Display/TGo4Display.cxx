#include "TGo4Display.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Task.h"
#include "TGo4TaskManager.h"
#include "TGo4DisplayDrawerTimer.h"
#include "TGo4DisplayLoggerTimer.h"
#include "TGo4Status.h"

#include "TGo4AnalysisProxy.h"

TGo4Display::TGo4Display(Bool_t isserver)
   : TGo4Master("Display",
                isserver, // servermode
                "dummy", //for clientmode only
                1), // negotiationport
     fxAnalysis(0)
{
   // start gui timers instead of threads
   fxDrawTimer= new TGo4DisplayDrawerTimer(this, 30);
   fxLogTimer= new TGo4DisplayLoggerTimer(this, 500);
   fxDrawTimer->TurnOn();
   fxLogTimer->TurnOn();

    // Start the GUI Registry
   GetTask()->Launch();
}


TGo4Display::~TGo4Display()
{
   TRACE((15,"TGo4Display::~TGo4Display()", __LINE__, __FILE__));
   //fxWorkHandler->CancelAll(); // make sure threads wont work on gui instance after it is deleted
   if(GetTask())
      GetTask()->GetWorkHandler()->CancelAll();
   delete fxDrawTimer;
   delete fxLogTimer;
   if (fxAnalysis!=0)
     fxAnalysis->DisplayDeleted(this);
//   cout <<"------- TGO4DISPLAY DESTRUCTOR FINISHED. ------" << endl;
}

void TGo4Display::DisplayData(TObject* data)
{
   if (fxAnalysis!=0)
     fxAnalysis->ReceiveObject(dynamic_cast<TNamed*>(data));
   else
      delete data;
}

void TGo4Display::DisplayLog(TGo4Status * Status)
{
   if (fxAnalysis!=0)
     fxAnalysis->ReceiveStatus(Status);
   else
     delete Status;
}

Bool_t TGo4Display::DisconnectSlave(const char* name, Bool_t waitforslave)
{
// Note: taskhandlerabortexception and shutdown of analysis server
// both will schedule a TGo4ComDisconnectSlave into local command queue
// of master task, calling TGo4Master::DisconnectSlave()
// here we override this method to inform gui about this
///////////////////////////////////////////////////
//
// before disconnecting, gui might stop monitoring timers here....
//...
  Bool_t rev=TGo4Master::DisconnectSlave(name,waitforslave); // should do all required things for disconnection
  // after disconnecting, gui might react on result by cleaning up browser, window caption etc.
  // for example:
  if(rev)
     {
       if (fxAnalysis!=0)

          fxAnalysis->DisplayDisconnected(this);

//        cout <<"+++++++++ TGo4Display::DisconnectSlave success on disconnect!!!" << endl;
//        cout <<"+++++++++ Please add something to inform GUI here about disconnected analysis!!!" << endl;
     }
   else
     {
//        cout <<"+++++++++ TGo4Display::DisconnectSlave failed!!!" << endl;
//        cout <<"+++++++++ Please add something to inform GUI here..." << endl;
     }

  return rev;
}

