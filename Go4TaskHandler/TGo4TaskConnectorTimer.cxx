#include "TGo4TaskConnectorTimer.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"

TGo4TaskConnectorTimer::TGo4TaskConnectorTimer (TGo4ServerTask* server, Long_t msperiod) :
   TTimer(msperiod,kTRUE),
   fxServer(server)
{
}

TGo4TaskConnectorTimer::~TGo4TaskConnectorTimer()
{
}

Bool_t TGo4TaskConnectorTimer::Notify ()
{
  // cout << "TGo4TaskConnectorTimer::Notify ()" << endl;
//   TurnOff();
   Int_t action=0;
   action=fxServer->TimerConnect();

//   TGo4Log::Debug(" Task Connector Timer --  starting new notify loop after action %d! ",
//      action);
   Reset();
//   TurnOn();

   return kFALSE;
}
