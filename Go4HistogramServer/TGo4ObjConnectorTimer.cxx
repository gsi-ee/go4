#include "TGo4ObjConnectorTimer.h"

#include "TGo4Log.h"
#include "TGo4HistogramServer.h"


TGo4ObjConnectorTimer::TGo4ObjConnectorTimer (TGo4HistogramServer* server, Long_t msperiod)
   :TTimer(msperiod,kTRUE), fxServer(server)
{
   TRACE((15,"TGo4ObjConnectorTimer::TGo4ObjConnectorTimer(TGo4ServerTask*,Long_t)",__LINE__, __FILE__));
}


TGo4ObjConnectorTimer::~TGo4ObjConnectorTimer()
{
   TRACE((15,"TGo4ObjConnectorTimer::~TGo4ObjConnectorTimer()",__LINE__, __FILE__));
}

Bool_t TGo4ObjConnectorTimer::Notify ()
{
//   TRACE((12,"TGo4ObjConnectorTimer::Notify()",__LINE__, __FILE__));
//   TurnOff();
   fxServer->TimerConnect();
   Reset();
//   TurnOn();
   return kFALSE;
}
