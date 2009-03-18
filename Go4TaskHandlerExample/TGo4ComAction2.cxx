#include "TGo4ComAction2.h"

#include "TGo4Log.h"
#include "TGo4ExampleApplication.h"

Int_t TGo4ComAction2::ExeCom()
{
   TRACE((12,"TGo4ComAction2::ExeCom() dtor",__LINE__, __FILE__));

   if (fxReceiverBase!=0)
      {
      TRACE((11,"TGo4ComAction2::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction2 ''%s'': Action2 requested ",
            GetName());
         ((TGo4ExampleApplication*) fxReceiverBase) ->CommandAction2();
      }
   else
      {
      TRACE((11,"TGo4ComAction2::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction2 ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}

TGo4ComAction2::~TGo4ComAction2()
{
   TRACE((12,"TGo4ComAction2::~TGo4ComAction2() dtor",__LINE__, __FILE__));
}

TGo4ComAction2::TGo4ComAction2() :TGo4ExampleCommand()
{
   TRACE((12,"TGo4ComAction2::TGo4ComAction2() ctor",__LINE__, __FILE__));
   SetName("EXAction2");
   SetDescription("executes example application action2 method");
}
