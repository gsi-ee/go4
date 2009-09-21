#include "Riostream.h"
#include <stdlib.h>
#include "TROOT.h"
#include "TRint.h"
#include "TApplication.h"

#include "TGo4Log.h"
#include "TYYYAnalysis.h"
#include "TGo4AnalysisClient.h"

void usage(); // print announcement

TROOT go4application("The GO4","This is the Go4");

#define kGUI   2
#define kBatch 1


//================== YYY analysis main program ============================
int main(int argc, char **argv)
{
// argv[0] program
// argv[1] "-gui" when started by GUI.
//         In this case the following args are:
// argv[2] analysis name as specified in GUI
// argv[3] hostname of GUI
// argv[4] connector port of GUI
//         if not GUI the args are:
// argv[1] LMD filename
// argv[2] optional number of events

// define here the standard suffix of your data:
   const char* inputsuffix=".scf";

   printf("\n%s started with args:\n",argv[0]);
   for(int i=1;i<argc;i++)printf("%d: %s\n",i,argv[i]);

   if(argc < 2) {
     usage(); // no argument: print usage
     exit(0);
   }

   Int_t runningMode = kGUI;    // runningMode of analysis, kGUI or kBatch
   if(strcmp(argv[1],"-gui") != 0) runningMode = kBatch; // no GUI
   // Now we know the running mode

   TApplication theApp("App", 0, 0); // init root application loop

   // set some output verbosity:
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
   TGo4Log::OpenLogfile("go4YYYlog.txt","--Logfile for go4 YYY --",kFALSE);
   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file


   //================= Begin  YYY analysis ==================================
   // Prepare the YYY analysis
   // In batch mode the arguments are
   // argv[1] filename
   // argv[2] optional number of events or -1

   Int_t maxevents = -1; // number of events (batch mode)
   Int_t len;
   unsigned int iport;
   char input[128];
   if(runningMode == kGUI){
      len = strlen(argv[2]);
      strncpy(input,argv[2],110); // analysis name
      iport = (argc>4) ? atoi(argv[4]) : 5000; // port of GUI server
   }
   else{
      len = strlen(argv[1]);
      strncpy(input,argv[1],110); // input file name
      if(argc > 2) maxevents=atoi(argv[2]);
   }

   // we construct filenames
   char lmd[1024]; // input GSI listmode
   char asf[1024]; // root autosave file
   char raw[1024]; // root unpacked events
   char anl[1024]; // root analyzed events

   char *pc;
   if((pc=strstr(input,".lmd")) != 0)*pc=0;
   // strip any path information from input file name:
   // outname is name without path
   char* lastname = input;
   char* outname = lastname;
   lastname = strstr(outname,"/");
   while(lastname!=0)
   {
      outname=lastname+1;
      lastname=strstr(outname,"/");
   }
   strncpy(lmd,input,len+4); // input file with full path
   strncpy(asf,outname,len+4);
   strncpy(raw,outname,len+4);
   strncpy(anl,outname,len+4);
   strcat(lmd,inputsuffix); // input with user suffix
   strcat(asf,"_ASF"); // root autosave file
   strcat(raw,"_UPA"); // root unpacked events
   strcat(anl,"_ANL"); // root analyzed events
   printf("Files used: \nFile input: %s\nAutosave:   %s\nUnpacked:   %s\nAnalyzed: %s\n",
         lmd,asf,raw,anl);

   // Now setup the YYY analysis itself
   TYYYAnalysis* analysis = new TYYYAnalysis(lmd,raw,anl);
   // Enable autosave. Autosave is done when analysis loop is stopped
   // or intervall os over.
   //analysis->SetAutoSave(kFALSE);         // no autosave
   analysis->SetAutoSaveFile(asf);        // optional
   analysis->SetAutoSaveInterval(0); // events , 0 = termination of event loop

   // ===================== End YYY analysis ================================
   // At this point the analysis is set up

   //TBenchmark bench;         // optional

   if(runningMode == kBatch)
   {
      cout << "*  starting analysis in batch mode ...  " << endl;
      if (analysis->InitEventClasses() )
      {
         // bench.Start("Go4EventServer"); // optional
         // prtimer.Start();               // optional
         analysis->RunImplicitLoop(maxevents);
         // prtimer.Stop();                // optional
         // prtimer.Resume();              // optional
         // bench.Show("Go4EventServer");  // optional
         delete analysis;
         cout << "\t Done!"<<endl;
         gApplication->Terminate();
      }
      else
      {
         cout << "Init event classes failed, aborting!"<<endl;
         gApplication->Terminate();
      }
   }

   if(runningMode == kGUI)
   {
      cout << "*  starting analysis in client mode ..." << endl;
      TGo4AnalysisClient* client = new TGo4AnalysisClient(argc,argv,analysis,
            kTRUE,"YYY","demo");
      cout << "Created AnalysisClient Instance: "<<client->GetName()<<endl;
   }
   theApp.Run();
   return 0;
}

void usage()
{
   cout << endl;
   cout << " ************************************************" << endl;
   cout << "* GO4 YYY analysis with user input example      *" << endl;
   cout << "* H. Essel, J.Adamczewski , GSI, Darmstadt      *" << endl;
   cout << "* usage: MainUserAnalysis file [events]         *" <<endl;
   cout << " ************************************************" << endl;
}
