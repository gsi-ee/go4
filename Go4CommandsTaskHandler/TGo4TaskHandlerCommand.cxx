#include "TGo4TaskHandlerCommand.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4TaskHandlerCommand::TGo4TaskHandlerCommand(const char* name, const char* description)
: TGo4Command(name,description,5)
{
   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("Task"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   SetProtection(kGo4ComModeController); // only controller connection may execute these commands
}

TGo4TaskHandlerCommand::TGo4TaskHandlerCommand()
: TGo4Command()
{
   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("Task"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   SetProtection(kGo4ComModeController);
   fiCommandID=5;
}


TGo4TaskHandlerCommand::~TGo4TaskHandlerCommand()
{
}

TGo4TaskHandlerCommand::TGo4TaskHandlerCommand(const TGo4TaskHandlerCommand &right)
  :TGo4Command(right)
{
}

Int_t TGo4TaskHandlerCommand::RefuseCom()
{
TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
if(cli)
   {
      cli->SendStatusMessage(2,kTRUE, "%s::Command %s from master %s not allowed for %s (mode %d)",
                     GetTaskName(), GetName(), GetTaskName(),GetModeDescription(GetMode()), GetMode());
   }
else
   {
      TGo4Command::RefuseCom();
   }
return 0;
}
