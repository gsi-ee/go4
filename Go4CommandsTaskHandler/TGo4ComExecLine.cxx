#include "TGo4ComExecLine.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComExecLine::TGo4ComExecLine() : TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THExec");
   SetDescription("Execute line as CINT macro");
}

TGo4ComExecLine::TGo4ComExecLine(const char* macro) : TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THExec");
   SetDescription("Execute line as CINT macro");
   SetLine(macro);
}

TGo4ComExecLine::~TGo4ComExecLine()
{
}

Int_t TGo4ComExecLine::ExeCom()
{
   TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli)
      {
         cli->SendStatusMessage(-1,kTRUE,"Executing: %s",GetLine());
         cli->ExecuteString(GetLine());
         cli->ExecuteString("Int_t res=fflush(stdout);");
         return 0;
      }
   else
      {
         TGo4Log::Debug(" !!! ComExecLine ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
