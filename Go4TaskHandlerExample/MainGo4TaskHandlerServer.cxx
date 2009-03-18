#include "Riostream.h"

#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4ExampleServer.h"

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file

   TGo4ExampleServer* myserver = new TGo4ExampleServer("MyExampleServer", 0, kFALSE);
      // second parameter is negotiation portnumber
   cout << "Created Example Server Instance:"<<myserver->GetName()<<endl; // dummy action for warnings
   theApp.Run();
   return 0;
}
