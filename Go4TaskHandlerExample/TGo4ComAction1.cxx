#include "TGo4ComAction1.h"

#include "TGo4Log.h"
#include "TGo4ExampleApplication.h"

Int_t TGo4ComAction1::ExeCom()
{
   TRACE((12,"TGo4ComAction1::ExeCom() dtor",__LINE__, __FILE__));

   if (fxReceiverBase!=0)
      {
      TRACE((11,"TGo4ComAction1::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction1 ''%s'': Action1 requested ",
            GetName());
         ((TGo4ExampleApplication*) fxReceiverBase)->CommandAction1();
      }
   else
      {
      TRACE((11,"TGo4ComAction1::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAction1 ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
TGo4ComAction1::~TGo4ComAction1()
{
   TRACE((12,"TGo4ComAction1::~TGo4ComAction1() dtor",__LINE__, __FILE__));
}

TGo4ComAction1::TGo4ComAction1()
:TGo4ExampleCommand()
{
   TRACE((12,"TGo4ComAction1::TGo4ComAction1() ctor",__LINE__, __FILE__));
   SetName("EXAction1");
   SetDescription("executes example application action1 method");
}
