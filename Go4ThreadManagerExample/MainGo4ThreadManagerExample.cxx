// MainGo4ThreadManager.cxx:
// This executable just creates one ThreadManager subclass.
// example: TGo4TestThreadManager as user class
// all runnables and threads are defined in ctor of TGo4TestThreadManager.
// On calling theApp.Run(), the threads are started.
// (J.Adamczewski, GSI)

#include "Riostream.h"

#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4TestThreadManager.h"

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file
   TGo4TestThreadManager* myManager = new TGo4TestThreadManager("MyTestEnvironment");
   cout << "Created thread manager "<< myManager->GetName()<< endl;
   theApp.Run();
   return 0;
}
