#include "Riostream.h"
#include <stdlib.h>
#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4ExampleClient.h"

void usage();

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file

   if(argc<4)
      {
         usage();

      }
   else
      {
         Text_t* name=argv[1];
         Text_t* hostname=argv[2];
         Text_t* connector=argv[3];
         UInt_t con=atoi(connector);
         cout << "Client:"<<name<<",\tHost:"<<hostname<<",\tConnector:"<<con<<endl;
         TGo4ExampleClient* myclient = new TGo4ExampleClient(name,hostname,con);
         cout << "Created ExampleClient Instance: "<<myclient->GetName()<<endl; // dummy action for warnings
         theApp.Run();
         return 0;
   } // if(argc<4)
}


void usage()
{
cout << "usage: MainGo4TaskHandlerClient clientname serverhostname connectorport"<<endl;
}
