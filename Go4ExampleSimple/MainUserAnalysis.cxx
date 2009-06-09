#include "Riostream.h"
#include <string.h>
#include <stdlib.h>

#include "TROOT.h"
#include "TRint.h"
#include "TApplication.h"
#include "snprintf.h"

#include "TGo4StepFactory.h"
#include "TGo4AnalysisStep.h"
#include "TGo4Analysis.h"
#include "TGo4AnalysisClient.h"
#include "Go4EventServer.h"
#include "Go4EventServerTypes.h"

void usage(const char* err = 0)
{
   if (err) cout << "Error: " << err << endl;
   cout << endl;
   cout << "* GO4  online analysis    " << endl;
   cout << "* H. Essel, GSI, Darmstadt" << endl;
   cout << "* calling:                " << endl;
   cout << "* MainUserAnalysis -server [CONFIG]                   : run analysis in server mode" << endl;
   cout << "* MainUserAnalysis -gui name guihost guiport [CONFIG] : connect analysis to prepared gui" << endl;
   cout << "* MainUserAnalysis [CONFIG]                           : run analysis in batch mode" << endl;
   cout << "* CONFIG : -file filename.lmd   :  open lmd file" << endl;
   cout << "*          -file filename.lml   :  open lml file" << endl;
   cout << "*          -transport server    :  connect to MBS transport server" << endl;
   cout << "*          -stream server       :  connect to MBS stream server" << endl;
   cout << "*          -evserv server       :  connect to MBS event server" << endl;
   cout << "*          -revserv server      :  connect to remote event server" << endl;
   cout << "*          -random              :  use random generator as input" << endl;
   cout << "*          -number NUMBER       :  process NUMBER events in batch mode" << endl;
   cout << "*          -asf filename        :  write autosave filename, default AnalysisASF.root" << endl;
   cout << endl;

   exit(err ? -1 : 0);
}

//==================  analysis main program ============================
int main(int argc, char **argv)
{
   if (argc<2) usage("Too few arguments");

   Bool_t batchMode(kTRUE);  // GUI or Batch

   Bool_t servermode(kFALSE);            // run analysis slave as servertask
   const char* hostname = "localhost";   // gui host name
   Int_t iport(5000);                    // port number used by GUI

   Bool_t autorun(kFALSE);   // immediately run analysis on start
   long  maxevents(-1);      // number of events (batch mode)
   const char* asf(0);       // name of austosave file

   //------ process arguments -------------------------

   int narg = 1;

   if(strcmp(argv[narg], "-gui") == 0) {
      batchMode = kFALSE;
      if (argc <= narg + 3) usage("Not all -gui arguments specified");
      narg++; // -gui
      narg++; // name
      hostname = argv[narg++];
      if (narg<argc)
         iport = atoi(argv[narg++]); // port of GUI server
   } else
   if(strstr(argv[narg], "-server")) {
      // set up analysis server mode started from GUI -client
      batchMode = kFALSE;
      servermode = kTRUE;
      narg++;
   } else {
      // set up arguments for batch mode
      batchMode = kTRUE;
   }

   TGo4EventSourceParameter* sourcepar(0);
   TGo4FileStoreParameter*   storepar(0);

   while (narg < argc) {
      if (strstr(argv[narg],"-fi")) {
         if (++narg < argc)
            sourcepar = new TGo4MbsFileParameter(argv[narg++]);
         else
            usage("LMD/LML file name not specified");
      } else
      if(strstr(argv[narg],"-tr")) {
         if (++narg < argc)
            sourcepar = new TGo4MbsTransportParameter(argv[narg++]);
         else
            usage("MBS Transport server name not specified");
      } else
      if(strstr(argv[narg],"-st")) {
         if (++narg < argc)
            sourcepar = new TGo4MbsStreamParameter(argv[narg++]);
         else
            usage("MBS Stream server name not specified");
      } else
      if(strstr(argv[narg],"-ev")) {
         if (++narg < argc)
            sourcepar = new TGo4MbsEventServerParameter(argv[narg++]);
         else
            usage("MBS Event server name not specified");
      } else
      if(strstr(argv[narg],"-ra")) {
         narg++;
         sourcepar = new TGo4MbsRandomParameter("Random");
      } else
      if(strstr(argv[narg],"-re")) {
         if (++narg < argc - 1)
            sourcepar = new TGo4RevServParameter(argv[narg++], atoi(argv[narg++]));
         else
            usage("Remote event server name or port are not specified");
      } else
      if (strstr(argv[narg],"-nu")) {
         if (++narg < argc) {
            if (sscanf(argv[narg++],"%ld",&maxevents)!=1) maxevents = -1;
         } else
            usage("number of events to process not specified");
      } else
      if (strstr(argv[narg],"-asf")) {
         if (++narg < argc)
            asf = argv[narg++];
         else
            usage("name of autosave file not specified");
      } else
         usage(Form("Unknown argument %d %s", narg, argv[narg]));
   }

   if (sourcepar==0)
      sourcepar = new TGo4MbsFileParameter("/GSI/lea/gauss.lmd");
   else
      if (servermode) autorun = kTRUE;

   // We will use only one analysis step (factory)
   // we use only standard components
   TGo4Analysis*     analysis = TGo4Analysis::Instance();
   TGo4StepFactory*  factory  = new TGo4StepFactory("Factory");
   TGo4AnalysisStep* step     = new TGo4AnalysisStep("Analysis", factory, sourcepar, storepar);
   step->SetSourceEnabled(kTRUE);
   step->SetStoreEnabled(kFALSE); // disabled, nothing to store up to now
   step->SetProcessEnabled(kTRUE);
   step->SetErrorStopEnabled(kTRUE);
   analysis->AddAnalysisStep(step);

   if ((asf==0) && (batchMode || autorun)) asf = "AnalysisASF.root";

   analysis->SetAutoSaveFile(asf);   // optional
   analysis->SetAutoSaveInterval(0); // after n seconds , 0 = at termination of event loop
   analysis->SetAutoSave(kFALSE);    // optional

   // tell the factory the names of processor and output event
   // both will be created by the framework later
   // Input event is by default an MBS event
   factory->DefEventProcessor("XXXProc","TXXXProc");// object name, class name
   factory->DefOutputEvent("XXXEvent","TGo4EventElement"); // object name, class name

   if(servermode) {

      //==================== password settings for gui login (for analysis server only)
      analysis->SetAdministratorPassword("XXXadmin");
      analysis->SetControllerPassword("XXXctrl");
      analysis->SetObserverPassword("XXXview");
      // note: do not change go4 default passwords for analysis in client mode
      // autoconnect to gui server will not work then!!!
   }

   //------ start the analysis -------------------------
   if(batchMode) {
      cout << "**** Main: starting analysis in batch mode ...  " << endl;
      analysis->SetAutoSave(kTRUE);   // optional enable auto-save
      if (analysis->InitEventClasses()) {
         if (maxevents < 0) maxevents = 1999999999L;

         analysis->RunImplicitLoop(maxevents);
         delete analysis;
         cout << "**** Main: Done!"<<endl;
      } else
         cout << "**** Main: Init event classes failed, aborting!"<<endl;
   } else {
      if (hostname==0) hostname = "localhost";

      TApplication theApp("Go4App", 0, 0);

      if(servermode)  cout << "**** Main: starting analysis in server mode ..." << endl;
      else            cout << "**** Main: starting analysis in slave mode ..." << endl;
      // to start histogram server: kTRUE,"base","password"

      TGo4AnalysisClient* client = new TGo4AnalysisClient("UserClient", analysis, hostname, iport, kFALSE, "", "", servermode, autorun);

      cout << "**** Main: created AnalysisClient Instance: " << client->GetName() << endl;
      cout << "**** Main: Run application loop" << endl;
      theApp.Run();
   }
   //=================  start root application loop ==========================
   return 0;
}
