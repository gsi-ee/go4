#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #include "TClassTable.h"
#include "TROOT.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TRint.h"
#include "TApplication.h"
#include "TSystem.h"
#include "Riostream.h"

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
   cout << "GO4 analysis runnable " << __GO4RELEASE__ << endl;
   cout << "S. Linev, GSI, Darmstadt" << endl;
   cout << "calling:                " << endl;
   cout << "" << endl;
   cout << "   go4analysis [RUN] [ANALYSIS] [STEP1] [STEP2] ... " << endl;
   cout << "" << endl;
   cout << "RUN: configuration, relevant for application run mode" << endl;
   cout << "  -lib name                   : user library to load (default: libGo4UserLibrary)" << endl;
   cout << "  -server [name]              : run analysis in server mode, name - optional analysis name" << endl;
   cout << "  -gui name guihost guiport   : run analysis in gui mode, used by GUI launch analysis" << endl;
   cout << "  -run                        : run analysis in server mode (defualt only run if source specified)" << endl;
   cout << "  -norun                      :  exclude automatical run" << endl;
   cout << "  -number NUMBER              :  process NUMBER events in batch mode" << endl;
   cout << "  -hserver [name [passwd]]    :  start histogram server with optional name and password" << endl;
   cout << "  -log [filename]             :  enable log output into filename (default:go4logfile.txt)" << endl;
   cout << "  -help                       :  show this help" << endl;
   cout << "" << endl;
   cout << "ANALYSIS: common analysis configurations" << endl;
   cout << "  -name name             :  specify analysis instance name" << endl;
   cout << "  -asf filename          :  set autosave filename and enable it, default <Name>ASF.root" << endl;
   cout << "  -enable-asf [interval] :  enable store of autosave file, optionally interval in seconds" << endl;
   cout << "  -disable-asf           :  disable usage of asf" << endl;
   cout << "" << endl;
   cout << "STEP: individual step configurations" << endl;
   cout << "  -step name           :  select step by it's name, if not defined, first step is used" << endl;
   cout << "  -step number         :  select step by it's number" << endl;
   cout << "  -enable-step         :  enable step processing" << endl;
   cout << "  -disable-step        :  disable step processing" << endl;
   cout << "  -file filename.lmd   :  use lmd file as event source" << endl;
   cout << "  -file filename.lml   :  use lml file as event source" << endl;
   cout << "  -transport server    :  connect to MBS transport server" << endl;
   cout << "  -stream server       :  connect to MBS stream server" << endl;
   cout << "  -evserv server       :  connect to MBS event server" << endl;
   cout << "  -revserv server port :  connect to remote event server" << endl;
   cout << "  -random              :  use random generator as source" << endl;
   cout << "  -user name           :  create user-defined event source" << endl;
   cout << "  -source filename     :  read step input from the root file" << endl;
   cout << "  -enable-source       :  enable step source" << endl;
   cout << "  -disable-source      :  disable step source" << endl;
   cout << "  -store filename      :  write step output into the root file" << endl;
   cout << "  -backstore name      :  create backstore for online tree draw" << endl;
   cout << "  -enable-store        :  enable step store" << endl;
   cout << "  -disable-store       :  disable step store" << endl;
   cout << "  -enable-errstop      :  enable stop-on-error mode" << endl;
   cout << "  -disable-errstop     :  disable stop-on-error mode" << endl;
   cout << endl;

   exit(err ? -1 : 0);
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

TList* GetClassesList(TList* prev = 0)
{
   TClassTable::Init();
   char* name = 0;

   TList* lst = new TList;
   lst->SetOwner(kTRUE);

   while ((name = TClassTable::Next()) != 0) {
      if (prev && prev->FindObject(name)) continue;

      TNamed* obj = new TNamed(name, name);
      lst->Add(obj);
      if (prev!=0) cout << "New class = " << name << endl;

   }
   return lst;
}

TGo4Analysis* CreateDefaultAnalysis(TList* lst, const char* name)
{
   TIter iter(lst);
   TObject* obj(0);

   TClass *proc_cl(0), *ev_cl(0), *an_cl(0);

   while ((obj = iter()) != 0) {
      TClass* cl = TClass::GetClass(obj->GetName());
//      cout << "Check class = " << obj->GetName() << " class = " << cl << endl;

      if (cl==0) continue;

      if (cl->InheritsFrom(TGo4EventProcessor::Class())) {
         if ((cl!=TGo4EventProcessor::Class()) && (proc_cl==0)) proc_cl = cl;
      } else
      if (cl->InheritsFrom(TGo4EventElement::Class())) {
         if ((cl!=TGo4EventElement::Class()) && (ev_cl==0)) ev_cl = cl;
      } else
      if (cl->InheritsFrom(TGo4Analysis::Class())) {
         if ((cl!=TGo4Analysis::Class()) && (an_cl==0)) an_cl = cl;
      }
   }

   if (proc_cl==0) return 0;

   cout << "!!! Create default analysis with processor class " << proc_cl->GetName() << endl;
   if (ev_cl)
      cout << "!!! Use class " << ev_cl->GetName() << " as output event" << endl;

   TGo4Analysis* analysis = TGo4Analysis::Instance();
   analysis->SetAnalysisName(name);

   TGo4StepFactory* factory = new TGo4StepFactory("Factory");
   factory->DefEventProcessor("Processor", proc_cl->GetName());// object name, class name
   if (ev_cl!=0)
      factory->DefOutputEvent("OutputEvent", ev_cl->GetName()); // object name, class name
   else
      factory->DefOutputEvent("OutputEvent", "TGo4EventElement"); // object name, class name

   TGo4MbsFileParameter* sourcepar = new TGo4MbsFileParameter("/GSI/lea/gauss.lmd");

   TGo4AnalysisStep* step = new TGo4AnalysisStep("Analysis", factory, sourcepar);

   step->SetSourceEnabled(kTRUE);
   step->SetStoreEnabled(kFALSE);
   step->SetProcessEnabled(kTRUE);
   step->SetErrorStopEnabled(kTRUE);

   // Now the first analysis step is set up.
   // Other steps could be created here
   analysis->AddAnalysisStep(step);

   return analysis;

}


//==================  analysis main program ============================
int main(int argc, char **argv)
{
   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      cerr << "Please check your system configuration and restart analysis again" << endl;
      return -1;
   }

   if (argc<2) usage();

   const char* analysis_name = GetArgValue(argc, argv, "-server");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-gui");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-name");
   if (analysis_name==0) analysis_name = "Go4Analysis";

   TList* lst0 = GetClassesList();

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

   TList* lst1 = GetClassesList(lst0);

   TGo4Analysis* analysis = 0;

   UserCreateFunc* crfunc = (UserCreateFunc*) gSystem->DynFindSymbol("*", "CreateUserAnalysis");
   if (crfunc) analysis = crfunc(analysis_name);
          else analysis = CreateDefaultAnalysis(lst1, analysis_name);


   if (analysis==0) {
      cerr << "!!! Analysis cannot be created" << endl;
      cerr << "!!! PLEASE check your analysis library " << libname << endl;
      cerr << "!!! One requires CreateUserAnalysis() function to be defined in user library" << endl;
      cerr << "!!! See Go4ExampleSimple, Go4Example1Step or Go4Example2Step for details" << endl;
      return -1;
   }

   delete lst0; lst0 = 0;
   delete lst1; lst1 = 0;

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
   Bool_t hserver(kFALSE);               // enable histogram server
   const char* hname  = "";              // namebase for histogram server
   const char* hpasswd  = "";            // password for histogram server
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
            const char* step_name = argv[narg++];
            int step_number(-1);
            step = 0;
            if (sscanf(step_name, "%d", &step_number)==1)
               if (step_number>=0) step = analysis->GetAnalysisStepNum(step_number);
            if (step==0) step = analysis->GetAnalysisStep(step_name);
            if (step==0) usage("step not found");
         } else
            usage("step name not specified");
      } else
      if(strcmp(argv[narg],"-enable-step")==0) {
         narg++;
         step->SetProcessEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-disable-step")==0) {
         narg++;
         step->SetProcessEnabled(kFALSE);
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
      if(strcmp(argv[narg],"-log")==0) {
         narg++;
         TGo4Log::LogfileEnable(kTRUE);
         const char* logname = TGo4Log::fgcDEFAULTLOG;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            logname = argv[narg++];
         TGo4Log::OpenLogfile(logname, " -- This is logfile from go4analysis file --- ", kTRUE);
      } else
      if(strcmp(argv[narg],"-run")==0) {
         narg++;
         autorun = true;
      } else
      if(strcmp(argv[narg],"-norun")==0) {
         narg++;
         canrun = -1;
      } else
      if(strcmp(argv[narg],"-help")==0) {
         narg++;
         usage();
      } else
      if(strcmp(argv[narg],"-enable-store")==0) {
         narg++;
         step->SetStoreEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-disable-store")==0) {
         narg++;
         step->SetStoreEnabled(kFALSE);
      } else
      if(strcmp(argv[narg],"-enable-source")==0) {
         narg++;
         step->SetSourceEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-disable-source")==0) {
         narg++;
         step->SetSourceEnabled(kFALSE);
      } else
      if(strcmp(argv[narg],"-disable-errstop")==0) {
         narg++;
         step->SetErrorStopEnabled(kFALSE);
      } else
      if(strcmp(argv[narg],"-enable-errstop")==0) {
         narg++;
         step->SetErrorStopEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-hserver")==0) {
         narg++;
         hserver = true;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            hname = argv[narg++];
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            hpasswd = argv[narg++];
      } else
         usage(Form("Unknown argument %d %s", narg, argv[narg]));
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

      TGo4AnalysisClient* client = new TGo4AnalysisClient("UserClient", analysis, hostname, iport, hserver, hname, hpasswd, servermode, autorun);

      cout << "**** Main: created AnalysisClient Instance: " << client->GetName() << endl;
      cout << "**** Main: Run application loop" << endl;
      theApp.Run();
   }

   TGo4Log::CloseLogfile();

   //=================  start root application loop ==========================
   return 0;
}
