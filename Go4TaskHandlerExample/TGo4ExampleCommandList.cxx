#include "TGo4ExampleCommandList.h"

#include "TGo4ComAction1.h"
#include "TGo4ComAction2.h"

TGo4ExampleCommandList::~TGo4ExampleCommandList()
{
}

TGo4ExampleCommandList::TGo4ExampleCommandList()
:TGo4TaskHandlerCommandList("Go4 Example Application Command list")
{
// add all application specific commands here:
  AddCommand (new TGo4ComAction1);
  AddCommand (new TGo4ComAction2);
}
