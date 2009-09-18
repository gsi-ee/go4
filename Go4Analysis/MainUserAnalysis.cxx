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
   cout << "* go4run -server [CONFIG]              : run analysis in server mode" << endl;
   cout << "* go4run -gui name guihost guiport [CONFIG] : connect analysis to prepared gui" << endl;
   cout << "* go4run [CONFIG]                           : run analysis in batch mode" << endl;
   cout << "* CONFIG : -file filename.lmd   :  open lmd file" << endl;
   cout << "*          -file filename.lml   :  open lml file" << endl;
   cout << "*          -transport server    :  connect to MBS transport server" << endl;
   cout << "*          -stream server       :  connect to MBS stream server" << endl;
   cout << "*          -evserv server       :  connect to MBS event server" << endl;
   cout << "*          -revserv server port :  connect to remote event server" << endl;
   cout << "*          -random              :  use random generator as input" << endl;
   cout << "*          -user name           :  connect to user-defined source" << endl;
   cout << "*          -number NUMBER       :  process NUMBER events in batch mode" << endl;
   cout << "*          -asf filename        :  write autosave filename, default AnalysisASF.root" << endl;
   cout << "*          -store filename      :  write step output into the root file" << endl;
   cout << "*          -backstore name      :  create backstore for online tree draw" << endl;
   cout << "*          -step name           :  select step for configuration (default: first active step)" << endl;
   cout << "*          -lib name            :  user library to load, should be very first argument (default: libGo4UserLibrary)" << endl;
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
typedef TGo4Analysis* (UserCreateFunc)();


TGo4Analysis* CreateDefaultAnalysis()
{
   const char* processor_name = "Processor";
   const char* processor_classname = 0;

   const char* outputevent_name = "OutputEvent";
   const char* outputevent_classname = 0;

   const char* eventsource_classname = 0;

   UserDefFunc* func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserProcessorClass");
   if (func!=0) processor_classname = func();

   if (processor_classname!=0) {
      TClass* proc_cl = TClass::GetClass(processor_classname);
      if (proc_cl==0) {
         cerr << "Processor class " << processor_classname << " not found" << endl;
         return 0;
      }

      func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserProcessorName");

      if (func!=0) processor_name = func();

      cout << "Event processor " << processor_classname << " of name " << processor_name << endl;
   } else {
      cerr << "Processor class not specifeid" << endl;
      return 0;
   }

   func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserOutputEventClass");
   if (func!=0) outputevent_classname = func();
   if (outputevent_classname==0) outputevent_classname = "TGo4EventElement";

   func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserEventSourceClass");
   if (func!=0) eventsource_classname = func();

   if (outputevent_classname!=0) {
      TClass* event_cl = TClass::GetClass(outputevent_classname);
      if (event_cl==0) {
         cerr << "Event class " << outputevent_classname << " not found" << endl;
         return 0;
      }
   }

   func = (UserDefFunc*) gSystem->DynFindSymbol("*", "UserOutputEventName");

   if (func!=0) outputevent_name = func();

   cout << "Output event " << outputevent_classname << " of name " << outputevent_name << endl;

//  use class table to find existing classes
//   TClassTable::PrintTable();

   // We will use only one analysis step (factory)
   // we use only standard components
   TGo4Analysis*     analysis = TGo4Analysis::Instance();
   TGo4StepFactory*  factory  = new TGo4StepFactory("Factory");
   TGo4AnalysisStep* step     = new TGo4AnalysisStep("Analysis", factory);
   step->SetStoreEnabled(kFALSE); // disabled, nothing to store up to now
   step->SetProcessEnabled(kTRUE);
   step->SetErrorStopEnabled(kTRUE);
   analysis->AddAnalysisStep(step);

   analysis->SetAutoSaveInterval(0); // after n seconds , 0 = at termination of event loop
   analysis->SetAutoSave(kFALSE);    // optional

   // tell the factory the names of processor and output event
   // both will be created by the framework later
   // Input event is by default an MBS event
   factory->DefEventProcessor(processor_name, processor_classname);// object name, class name
   factory->DefOutputEvent(outputevent_name, outputevent_classname); // object name, class name

   if (eventsource_classname) factory->DefEventSource(eventsource_classname);

   return analysis;
}


//==================  analysis main program ============================
int main(int argc, char **argv)
{
   if (!TGo4Version::Instance()->CheckVersion(__GO4BUILDVERSION__)) {
      cerr << "Please check your system configuration and restart analysis again" << endl;
      return -1;
   }


   if (argc<2) usage("Too few arguments");

   int narg = 1;

   const char* libname = "libGo4UserAnalysis";
   if (strcmp(argv[narg],"-lib")==0) {
      if (++narg < argc) libname = argv[narg++];
      else usage("library name not specified");
   }

   if (gSystem->Load(libname)!=0) return -1;

   TGo4Analysis* analysis = 0;

   UserCreateFunc* crfunc = (UserCreateFunc*) gSystem->DynFindSymbol("*", "CreateUserAnalysis");
   if (crfunc) analysis = crfunc();
          else analysis = CreateDefaultAnalysis();
   if (analysis==0) {
      cerr << "Analysis cannot be created" << endl;
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

   Bool_t autorun(kFALSE);   // immediately run analysis on start
   long  maxevents(-1);      // number of events (batch mode)

   //------ process arguments -------------------------


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
      // exclude dummy arguments from gui
      while ((narg<argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) narg++;
   } else {
      // set up arguments for batch mode
      batchMode = kTRUE;
   }

   while (narg < argc) {
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
         } else
            usage("LMD/LML file name not specified");
      } else
      if(strcmp(argv[narg],"-transport")==0) {
         if (++narg < argc) {
            TGo4MbsTransportParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
         } else
            usage("MBS Transport server name not specified");
      } else
      if(strcmp(argv[narg],"-stream")==0) {
         if (++narg < argc) {
            TGo4MbsStreamParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
         } else
            usage("MBS Stream server name not specified");
      } else
      if(strcmp(argv[narg],"-evserv")==0) {
         if (++narg < argc) {
            TGo4MbsEventServerParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
         } else
            usage("MBS Event server name not specified");
      } else
      if(strcmp(argv[narg],"-random")==0) {
         narg++;
         TGo4MbsRandomParameter sourcepar("Random");
         step->SetEventSource(&sourcepar);
         step->SetSourceEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-user")==0) {
         if (++narg < argc) {
            TGo4UserSourceParameter sourcepar("UserSource", argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
         } else
            usage("MBS Event server name not specified");
      } else
      if(strcmp(argv[narg],"-revserv")==0) {
         if (++narg < argc - 1) {
            const char* serv_name = argv[narg++];
            int serv_port = atoi(argv[narg++]);
            TGo4RevServParameter sourcepar(serv_name, serv_port);
            step->SetEventSource(&sourcepar);
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
         if (++narg < argc)
            analysis->SetAutoSaveFile(argv[narg++]);
         else
            usage("name of autosave file not specified");
      } else
         usage(Form("Unknown argument %d %s", narg, argv[narg]));
   }

   if (!analysis->IsAutoSaveFileName() && (batchMode || autorun))
      analysis->SetAutoSaveFile("AnalysisASF.root");

   if (!step->IsEventSourceParam()) {
      TGo4MbsFileParameter sourcepar("/GSI/lea/gauss.lmd");
      step->SetEventSource(&sourcepar);
      step->SetSourceEnabled(kTRUE);
   } else {
      if (servermode) autorun = kTRUE;
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
         if (maxevents < 0) maxevents = 1999999999L;

         analysis->RunImplicitLoop(maxevents);
         delete analysis;
         cout << "**** Main: Done!"<<endl;
      } else
         cout << "**** Main: Init event classes failed, aborting!"<<endl;
   } else {
      if (hostname==0) hostname = "localhost";

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
