#include "Riostream.h"
#include <string.h>
#include <stdlib.h>

// #include "TClassTable.h"
#include "TROOT.h"
#include "TClass.h"
#include "TRint.h"
#include "TApplication.h"
#include "TSystem.h"

#include "TGo4Version.h"
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
   cout << "* go4analysis [RUNMODE] [CONFIG]" << endl;
   cout << "* RUNMODE: -server [name]       : run analysis in server mode, name - optional analysis name" << endl;
   cout << "*     -gui name guihost guiport : run analysis in gui mode, used by GUI launch analysis" << endl;
   cout << "* CONFIG : -lib name            :  user library to load (default: libGo4UserLibrary)" << endl;
   cout << "*          -file filename.lmd   :  open lmd file" << endl;
   cout << "*          -file filename.lml   :  open lml file" << endl;
   cout << "*          -transport server    :  connect to MBS transport server" << endl;
   cout << "*          -stream server       :  connect to MBS stream server" << endl;
   cout << "*          -evserv server       :  connect to MBS event server" << endl;
   cout << "*          -revserv server port :  connect to remote event server" << endl;
   cout << "*          -random              :  use random generator as input" << endl;
   cout << "*          -user name           :  create user-defined event source" << endl;
   cout << "*          -number NUMBER       :  process NUMBER events in batch mode" << endl;
   cout << "*          -source filename     :  read step input from the root file" << endl;
   cout << "*          -store filename      :  write step output into the root file" << endl;
   cout << "*          -backstore name      :  create backstore for online tree draw" << endl;
   cout << "*          -step name           :  select step for configuration (default: first active step)" << endl;
   cout << "*          -asf filename        :  set autosave filename and enable it, default <Name>ASF.root" << endl;
   cout << "*          -enable-asf [interval] :  enable store of autosave file, optionally interval in seconds" << endl;
   cout << "*          -disable-asf         :  disable usage of asf" << endl;
   cout << "*          -run                 :  run analysis in server mode (defualt only run if source specified)" << endl;
   cout << "*          -norun               :  exlude automatical run" << endl;
   cout << "*          -name name           :  specify analysis name for batch mode" << endl;
   cout << endl;

   exit(err ? -1 : 0);
}

TClass* FindDerivedClass(const char* baseclass)
{
   TClass* base_cl = gROOT->GetClass(baseclass);
   if (base_cl==0) usage(Form("Cannot find %s class!"));

   TIter iter(gROOT->GetListOfClasses());
   Int_t cnt(0);
   TClass *cl(0), *find_cl(0);

   while ((cl = (TClass*)iter())!=0) {
      cout << "Class = " << cl->GetName() << endl;
      if (!cl->InheritsFrom(base_cl) || (cl==base_cl)) continue;
      if (++cnt == 1) find_cl = cl;
      else cout <<" Too many classes, derived from " << baseclass << endl;
   }

   return find_cl;
}

typedef const char* (UserDefFunc)();
typedef TGo4Analysis* (UserCreateFunc)(const char* name);

const char* GetArgValue(int argc, char **argv, const char* argname, int* pos = 0)
{
   int n = pos ? *pos : 0;

   while (++n<argc)
      if (strcmp(argv[n], argname)==0) {
         if ((n+1<argc) && (argv[n+1][0]!='-')) {
            if (pos) *pos = n+1;
            return argv[n+1];
         }
      }

   if (pos) *pos = 0;
   return 0;
}


//==================  analysis main program ============================
int main(int argc, char **argv)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cerr << "Please check your system configuration and restart analysis again" << endl;
      return -1;
   }


   if (argc<2) usage("Too few arguments");

   const char* analysis_name = GetArgValue(argc, argv, "-server");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-gui");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-name");
   if (analysis_name==0) analysis_name = "Go4Analysis";

   int argpos = 0;
   bool isanylib = false;
   const char* libname = 0;
   while ((libname = GetArgValue(argc, argv, "-lib", &argpos))!=0) {
      cout << "Reading library: " << libname << endl;
      if (gSystem->Load(libname)<0) return -1;
      isanylib = true;
   }

   if (!isanylib) {
      libname = "libGo4UserAnalysis";
      cout << "Reading library: " << libname << endl;
      if (gSystem->Load(libname)<0) return -1;
   }

   TGo4Analysis* analysis = 0;

   UserCreateFunc* crfunc = (UserCreateFunc*) gSystem->DynFindSymbol("*", "CreateUserAnalysis");
   if (crfunc) analysis = crfunc(analysis_name);
   if (analysis==0) {
      cerr << "!!! Analysis cannot be created" << endl;
      cerr << "!!! PLEASE check your analysis library " << libname << endl;
      cerr << "!!! One requires CreateUserAnalysis() function to be defined in user library" << endl;
      cerr << "!!! See Go4ExampleSimple, Go4Example1Step or Go4Example2Step for details" << endl;
      return -1;
   }

   TGo4AnalysisStep* step = analysis->GetAnalysisStep(0);
   if (step==0) {
      cerr << "No active step in analysis found" << endl;
      return -1;
   }

   int app_argc = 2;
   char* app_argv[2];
   app_argv[0] = new char[256];
   app_argv[1] = new char[256];
   strncpy(app_argv[0], argv[0], 256);
   strncpy(app_argv[1], "-b", 256);
   TApplication theApp("Go4App", &app_argc, app_argv);

   Bool_t batchMode(kTRUE);  // GUI or Batch
   Bool_t servermode(kFALSE);            // run analysis as server task
   const char* hostname = "localhost";   // gui host name
   Int_t iport(5000);                    // port number used by GUI

   bool autorun(false);    // immediately run analysis on start
   long  maxevents(-1);    // number of events (batch mode)
   Int_t canrun(0);        // -1 cannot run, 0 - only if source specify, 1 - always

   //------ process arguments -------------------------

   int narg = 1;

   while (narg < argc) {
      if (strcmp(argv[narg], "-server")==0) {
         narg++;
         batchMode = kFALSE;
         servermode = kTRUE;
         // skip name, if specified
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) narg++;
      } else
      if(strcmp(argv[narg], "-gui") == 0) {
         batchMode = kFALSE;
         if (argc <= narg + 3) usage("Not all -gui arguments specified");
         narg++; // -gui
         narg++; // name
         hostname = argv[narg++];
         iport = atoi(argv[narg++]); // port of GUI server
      } else
      if(strcmp(argv[narg], "-lib") == 0) {
         // skip library name
         if (++narg >= argc) usage("library name not specified");
         narg++;
      } else
      if(strcmp(argv[narg], "-name") == 0) {
         // skip analysis name
         if (++narg >= argc) usage("analysis name not specified");
         narg++;
      } else
      if (strcmp(argv[narg],"-step")==0) {
         if (++narg < argc) {
            step = analysis->GetAnalysisStep(argv[narg++]);
            if (step==0) usage("step not found");
         } else
            usage("step name not specified");
      } else
      if (strcmp(argv[narg],"-file")==0) {
         if (++narg < argc) {
            TGo4MbsFileParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("LMD/LML file name not specified");
      } else
      if(strcmp(argv[narg],"-transport")==0) {
         if (++narg < argc) {
            TGo4MbsTransportParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("MBS Transport server name not specified");
      } else
      if(strcmp(argv[narg],"-stream")==0) {
         if (++narg < argc) {
            TGo4MbsStreamParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("MBS Stream server name not specified");
      } else
      if(strcmp(argv[narg],"-evserv")==0) {
         if (++narg < argc) {
            TGo4MbsEventServerParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("MBS Event server name not specified");
      } else
      if(strcmp(argv[narg],"-random")==0) {
         narg++;
         TGo4MbsRandomParameter sourcepar("Random");
         step->SetEventSource(&sourcepar);
         step->SetSourceEnabled(kTRUE);
         autorun = true;
      } else
      if(strcmp(argv[narg],"-user")==0) {
         if (++narg < argc) {
            TGo4UserSourceParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("MBS Event server name not specified");
      } else
      if(strcmp(argv[narg],"-revserv")==0) {
         if (++narg < argc - 1) {
            const char* serv_name = argv[narg++];
            int serv_port = atoi(argv[narg++]);
            TGo4RevServParameter sourcepar(serv_name, serv_port);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            usage("Remote event server name or port are not specified");
      } else
      if(strcmp(argv[narg],"-store")==0) {
         if (++narg < argc) {
            TGo4FileStoreParameter storepar(argv[narg++]);
            step->SetEventStore(&storepar);
            step->SetStoreEnabled(kTRUE);
         } else
            usage("File name for store not specified");
      } else
      if(strcmp(argv[narg],"-backstore")==0) {
         if (++narg < argc) {
            TGo4BackStoreParameter storepar(argv[narg++]);
            step->SetEventStore(&storepar);
            step->SetStoreEnabled(kTRUE);
         } else
            usage("Backstore name not specified");
      } else
      if (strcmp(argv[narg],"-number")==0) {
         if (++narg < argc) {
            if (sscanf(argv[narg++],"%ld",&maxevents)!=1) maxevents = -1;
         } else
            usage("number of events to process not specified");
      } else
      if (strcmp(argv[narg],"-asf")==0) {
         if (++narg < argc) {
            analysis->SetAutoSaveFile(argv[narg++]);
            analysis->SetAutoSave(kTRUE);
         } else
            usage("name of autosave file not specified");
      } else
      if (strcmp(argv[narg],"-enable-asf")==0) {
         narg++;
         analysis->SetAutoSave(kTRUE);
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            analysis->SetAutoSaveInterval(atoi(argv[narg++]));
      } else
      if (strcmp(argv[narg],"-disable-asf")==0) {
         narg++;
         analysis->SetAutoSave(kFALSE);
      } else
      if(strcmp(argv[narg],"-run")==0) {
         narg++;
         autorun = true;
      } else
      if(strcmp(argv[narg],"-norun")==0) {
         narg++;
         canrun = -1;
      } else
         usage(Form("Unknown argument %d %s", narg, argv[narg]));
   }

   //==================== password settings for gui login in server mode

   if(servermode) {
      UserDefFunc* func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserAdministratorPassword");
      if (func) analysis->SetAdministratorPassword(func());
      func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserControllerPassword");
      if (func) analysis->SetControllerPassword(func());
      func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserObserverPassword");
      if (func) analysis->SetObserverPassword(func());
   }

   //------ start the analysis -------------------------
   if(batchMode) {
      cout << "**** Main: starting analysis in batch mode ...  " << endl;
      analysis->SetAutoSave(kTRUE);   // optional enable auto-save
      if (analysis->InitEventClasses()) {
         analysis->RunImplicitLoop(maxevents);
         delete analysis;
         cout << "**** Main: Done!" << endl;
      } else
         cout << "**** Main: Init event classes failed, aborting!"<<endl;
   } else {
      if (hostname==0) hostname = "localhost";

      if(servermode)  cout << "**** Main: starting analysis in server mode ..." << endl;
      else            cout << "**** Main: starting analysis in slave mode ..." << endl;
      // to start histogram server: kTRUE,"base","password"

      if (canrun<0) autorun = false;

      TGo4AnalysisClient* client = new TGo4AnalysisClient("UserClient", analysis, hostname, iport, kFALSE, "", "", servermode, autorun);

      cout << "**** Main: created AnalysisClient Instance: " << client->GetName() << endl;
      cout << "**** Main: Run application loop" << endl;
      theApp.Run();
   }
   //=================  start root application loop ==========================
   return 0;
}
