#include "TGo4TaskHandlerCommandList.h"

#include "TGo4Log.h"

#include "TGo4ComQuit.h"
#include "TGo4ComStart.h"
#include "TGo4ComStop.h"
#include "TGo4ComExecLine.h"
#include "TGo4ComServerQuit.h"
#include "TGo4ComRemoveClient.h"
#include "TGo4ComMasterQuit.h"
#include "TGo4ComDisconnectSlave.h"

TGo4TaskHandlerCommandList::TGo4TaskHandlerCommandList(const char* name) :
   TGo4CommandProtoList(name)
{
    // all valid TaskHandler commands should be added to prototype list...
    AddCommand (new TGo4ComQuit);
    AddCommand (new TGo4ComMasterQuit);
    AddCommand (new TGo4ComStart);
    AddCommand (new TGo4ComStop);
    AddCommand (new TGo4ComDisconnectSlave);
    AddCommand (new TGo4ComServerQuit);
    AddCommand (new TGo4ComRemoveClient);
    AddCommand (new TGo4ComExecLine);
}

TGo4TaskHandlerCommandList::~TGo4TaskHandlerCommandList()
{
}
