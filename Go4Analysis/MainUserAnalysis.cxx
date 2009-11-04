// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "TROOT.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TDataType.h"
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

void showerror(const char* msg)
{
   cerr << "Error: " << msg << endl;
   cerr << "Call  'go4analysis -help'  to see list of available arguments" << endl;
   exit(1);
}

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
   cout << "  -run                        : run analysis in server mode (default only run if source specified)" << endl;
   cout << "  -norun                      : exclude automatical run" << endl;
   cout << "  -number NUMBER              : process NUMBER events in batch mode" << endl;
   cout << "  -hserver [name [passwd]]    : start histogram server with optional name and password" << endl;
   cout << "  -log [filename]             : enable log output into filename (default:go4logfile.txt)" << endl;
   cout << "  -v -v0 -v1 -v2 -v3          : change log output verbosity (0 - maximum, 1 - info, 2 - warn, 3 - errors)" << endl;
   cout << "  -help                       : show this help" << endl;
   cout << "" << endl;
   cout << "ANALYSIS: common analysis configurations" << endl;
   cout << "  -name name             :  specify analysis instance name" << endl;
   cout << "  -asf [filename]        :  enable store autosave file and set autosave filename (default <Name>ASF.root)" << endl;
   cout << "  -enable-asf [interval] :  enable store of autosave file, optionally interval in seconds" << endl;
   cout << "  -disable-asf           :  disable usage of asf" << endl;
   cout << "  -prefs [filename]      :  load preferences (analysis configuration) from specified file (default Go4AnalysisPrefs.root)" << endl;
   cout << "  -no-prefs              :  disable preferences loading" << endl;
   cout << "  -args [userargs]       :  create user analysis with constructor (int argc, char** argv) signature" << endl;
   cout << "                            all following arguments will be provided as array of strings, first argument - analysis name" << endl;
   cout << "" << endl;
   cout << "STEP: individual step configurations" << endl;
   cout << "  -step name           :  select step by it's name, if not defined, first step is used" << endl;
   cout << "  -step number         :  select step by it's number" << endl;
   cout << "  -enable-step         :  enable step processing" << endl;
   cout << "  -disable-step        :  disable step processing" << endl;
   cout << "  -file filename       :  use file filename (lmd or lml) as MBS event source" << endl;
   cout << "  -transport server    :  connect to MBS transport server" << endl;
   cout << "  -stream server       :  connect to MBS stream server" << endl;
   cout << "  -evserv server       :  connect to MBS event server" << endl;
   cout << "  -revserv server port :  connect to remote event server" << endl;
   cout << "  -random              :  use random generator as source" << endl;
   cout << "  -user name           :  create user-defined event source" << endl;
   cout << "  -source filename     :  read step input from the root file" << endl;
   cout << "  -mbs-select first last step : select events interval from mbs source" << endl;
   cout << "  -timeout tm          :  specify timeout parameter for event source" << endl;
   cout << "  -enable-source       :  enable step source" << endl;
   cout << "  -disable-source      :  disable step source" << endl;
   cout << "  -store filename [split buffersize compression] :  write step output into the root file" << endl;
   cout << "  -overwrite-store     :  overwrite file, when store output" << endl;
   cout << "  -append-store        :  append to file, when store output" << endl;
   cout << "  -backstore name      :  create backstore for online tree draw" << endl;
   cout << "  -enable-store        :  enable step store" << endl;
   cout << "  -disable-store       :  disable step store" << endl;
   cout << "  -enable-errstop      :  enable stop-on-error mode" << endl;
   cout << "  -disable-errstop     :  disable stop-on-error mode" << endl;
   cout << endl;

   exit(0);
}

typedef TGo4Analysis* (UserCreateFunc)(const char* name);

int FindArg(int argc, char **argv, const char* argname)
{
   if ((argname==0) || (strlen(argname)==0)) return -1;
   for (int n=0;n<argc;n++)
      if (strcmp(argv[n], argname)==0) return n;
   return -1;
}

const char* GetArgValue(int argc, char **argv, const char* argname, int* pos = 0, bool incomplete = false)
{
   int n = pos ? *pos : 0;

   while (++n<argc)
      if (strcmp(argv[n], argname)==0) {
         if ((n+1<argc) && (argv[n+1][0]!='-')) {
            if (pos) *pos = n+1;
            return argv[n+1];
         } else
         if (incomplete) {
            if (pos) *pos = n+1;
            return "";
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

   if (prev!=0) TGo4Log::Debug("Search user classes in loaded library");

   while ((name = TClassTable::Next()) != 0) {
      if (prev && prev->FindObject(name)) continue;

      TNamed* obj = new TNamed(name, name);
      lst->Add(obj);

      if (prev!=0) TGo4Log::Debug("New class %s", name);
   }
   return lst;
}

TGo4Analysis* CreateDefaultAnalysis(TList* lst, const char* name, int user_argc, char** user_argv)
{
   TIter iter(lst);
   TObject* obj(0);

   TClass *proc_cl(0), *ev_cl(0), *an_cl(0);

   while ((obj = iter()) != 0) {
      TClass* cl = TClass::GetClass(obj->GetName());

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

   if (an_cl!=0) {

      TGo4Log::Info("Find user analysis class %s", an_cl->GetName());

      TMethod* meth = an_cl->GetMethodWithPrototype(an_cl->GetName(), "int,char**");
      if (meth!=0) {
         TGo4Log::Info("!!! Find constructor with prototype %s::%s(int, char**)", an_cl->GetName(), an_cl->GetName());

         if ((user_argc>0) && (user_argv!=0))
            user_argv[0] = (char*) name;
         else {
            user_argc = 1;
            user_argv = (char**) &name;
         }

         TString cmd = Form("new %s(%d, (char**)%p);", an_cl->GetName(), user_argc, user_argv);
         Int_t err = 0;

         TGo4Log::Info("Process: %s", cmd.Data());

         TGo4Analysis* analysis = (TGo4Analysis*) gROOT->ProcessLineFast(cmd.Data(), &err);

         if ((analysis!=0) && (err==0)) return analysis;

         TGo4Log::Error("Cannot create analysis class %s instance with (int, char**) prototype", an_cl->GetName());
         TGo4Log::Error("Implement correct analysis constructor with such signature in user library");
         exit(1);
      }


      meth = an_cl->GetMethodWithPrototype(an_cl->GetName(), "const char*");
      if (meth!=0) {
         TGo4Log::Info("!!! Find constructor with prototype %s::%s(const char*)", an_cl->GetName(), an_cl->GetName());

         TString cmd = Form("new %s(\"%s\");", an_cl->GetName(), name);
         Int_t err = 0;

         TGo4Log::Info("Process: %s", cmd.Data());

         TGo4Analysis* analysis = (TGo4Analysis*) gROOT->ProcessLineFast(cmd.Data(), &err);

         if ((analysis!=0) && (err==0)) return analysis;

         TGo4Log::Error("Cannot create analysis class % instance with (const char*) prototype", an_cl->GetName());
         TGo4Log::Error("Implement correct analysis constructor with such signature in user library");
         exit(1);
      }

      // search for non-default analysis constructor
      TIter iter(an_cl->GetListOfMethods());
      while ((meth = (TMethod*) iter()) != 0) {
         if (strcmp(meth->GetName(), an_cl->GetName()) != 0) continue;
         if (meth->GetListOfMethodArgs()->GetSize()==0) continue;
         break;
      }

      if (meth==0) {
         TGo4Log::Error("Cannot find non-default constructor for class %s", an_cl->GetName());
         TGo4Log::Error("Implement analysis constructor with (const char*) or (int,char**) signature");
         TGo4Log::Error("Or define TGo4Analysis* CreateUserAnalysis(const char*) function in user library");
         exit(1);
      }

      TGo4Log::Info("Find constructor with %d arguments", meth->GetListOfMethodArgs()->GetSize());

      TMethodArg *argument = 0;
      TIter next(meth->GetListOfMethodArgs());

      TString cmd = Form("new %s(", an_cl->GetName());

      int counter = 0;

      while ((argument = (TMethodArg *) next())) {

         if (counter>0) cmd+=", ";
         counter++;

         TDataType *datatype  = gROOT->GetType(argument->GetTypeName());

         TString basictype = (datatype==0) ? "int" :  datatype->GetTypeName();

         TGo4Log::Debug("Argument %s type %s basictype %s", argument->GetName(), argument->GetTitle(), basictype.Data());

         TString argDflt = argument->GetDefault() ? argument->GetDefault() : "";
         if (argDflt.Length()>0) {
            cmd += argDflt;
            continue;
         }

         bool isptr = strchr(argument->GetTitle(), '*') != 0;

         if ((datatype==0) && !isptr) {
            TGo4Log::Error("Cannot specify any value for argument %s of class %s constructor", argument->GetName(), an_cl->GetName());
            TGo4Log::Error("Add CreateUserAnalysis(const char*) function in user library");
            exit(1);
         }

         if (isptr) {
            if ((basictype == "char") || (basictype="Text_t")) cmd+="\"\"";
                                                          else cmd+="0";
         } else {
            if ((counter==2) && (basictype=="int")) {
               TGo4Log::Info("Special treatment for second integer argument, suppose MBS input type");
               cmd+=Form("%d", GO4EV_MBS_FILE);
            } else
            if (basictype=="bool") {
               cmd+="false";
            } else
               cmd+="0";
         }
      }

      cmd += ");";

      Int_t err = 0;
      TGo4Log::Info("Process: %s", cmd.Data());
      TGo4Analysis* analysis = (TGo4Analysis*) gROOT->ProcessLineFast(cmd.Data(), &err);
      if ((analysis!=0) && (err==0)) return analysis;

      TGo4Log::Error("Cannot create analysis class % instance", an_cl->GetName());
      TGo4Log::Error("Add CreateUserAnalysis(const char*) function in user library");

      exit(1);
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

   if ((argc<2) || (FindArg(argc, argv, "-help")>0)) usage();

   int user_argc = 0;
   char** user_argv = 0;

   int userargspos = FindArg(argc, argv, "-args");
   if (userargspos<0) userargspos = FindArg(argc, argv, "-x");
   if (userargspos>0) {
      user_argc = argc - userargspos;
      user_argv = argv + userargspos;
      // hide user-defined arguments
      argc = userargspos;
   }

   const char* logfile = GetArgValue(argc, argv, "-log", 0, true);
   if (logfile!=0) {

      TGo4Log::Instance();

      TGo4Log::LogfileEnable(kTRUE);
      if (strlen(logfile)==0) logfile = TGo4Log::GetDefaultLogname();
      TGo4Log::OpenLogfile(logfile, 0, kTRUE);

      TString info = "go4analysis";
      for (int n=1;n<argc;n++) { info += " "; info += argv[n]; }

      TGo4Log::WriteLogfile(info.Data(), true);
   }

   if ((FindArg(argc, argv, "-v")>0) || (FindArg(argc, argv, "-verbose")>0)) TGo4Log::SetIgnoreLevel(-1);
   else if (FindArg(argc, argv, "-v0")>0) TGo4Log::SetIgnoreLevel(0);
   else if (FindArg(argc, argv, "-v1")>0) TGo4Log::SetIgnoreLevel(1);
   else if (FindArg(argc, argv, "-v2")>0) TGo4Log::SetIgnoreLevel(2);
   else if (FindArg(argc, argv, "-v3")>0) TGo4Log::SetIgnoreLevel(3);

   const char* analysis_name = GetArgValue(argc, argv, "-server");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-gui");
   if (analysis_name==0) analysis_name = GetArgValue(argc, argv, "-name");
   if (analysis_name==0) analysis_name = "Go4Analysis";

   TList* lst0 = GetClassesList();

   int argpos = 0;
   bool isanylib = false;
   const char* libname = 0;
   while ((libname = GetArgValue(argc, argv, "-lib", &argpos))!=0) {
      TGo4Log::Info("Reading library: %s", libname);
      if (gSystem->Load(libname)<0) return -1;
      isanylib = true;
   }

   if (!isanylib) {
      libname = "libGo4UserAnalysis";
      TGo4Log::Info("Reading library: %s", libname);
      if (gSystem->Load(libname)<0) return -1;
   }

   TList* lst1 = GetClassesList(lst0);

   TGo4Analysis* analysis = 0;

   UserCreateFunc* crfunc = (UserCreateFunc*) gSystem->DynFindSymbol("*", "CreateUserAnalysis");
   if (crfunc) analysis = crfunc(analysis_name);
          else analysis = CreateDefaultAnalysis(lst1, analysis_name, user_argc, user_argv);

   if (analysis==0) {
      cerr << "!!! Analysis instance cannot be created" << endl;
      cerr << "!!! PLEASE check your analysis library " << libname << endl;
      cerr << "!!! One requires user subclass for TGo4Analysis class in user library" << endl;
      cerr << "!!! Alternatively, CreateUserAnalysis(const char*) function can be implemented" << endl;
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
   app_argv[0] = new char[1024];
   app_argv[1] = new char[1024];
   strncpy(app_argv[0], argv[0], 1024);
   strncpy(app_argv[1], "-b", 1024);
   TApplication theApp("Go4App", &app_argc, app_argv);

   Bool_t batchMode(kTRUE);  // GUI or Batch
   Bool_t servermode(kFALSE);            // run analysis as server task
   Bool_t hserver(kFALSE);               // enable histogram server
   Bool_t loadprefs(kTRUE);              // loading preferences by client
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
         if (argc <= narg + 3) showerror("Not all -gui arguments specified");
         narg++; // -gui
         narg++; // name
         hostname = argv[narg++];
         iport = atoi(argv[narg++]); // port of GUI server
      } else
      if(strcmp(argv[narg], "-lib") == 0) {
         // skip library name
         if (++narg >= argc) showerror("library name not specified");
         narg++;
      } else
      if(strcmp(argv[narg], "-name") == 0) {
         // skip analysis name
         if (++narg >= argc) showerror("analysis name not specified");
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
            if (step==0) showerror("step not found");
         } else
            showerror("step name not specified");
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
            showerror("LMD/LML file name not specified");
      } else
      if(strcmp(argv[narg],"-transport")==0) {
         if (++narg < argc) {
            TGo4MbsTransportParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Transport server name not specified");
      } else
      if(strcmp(argv[narg],"-stream")==0) {
         if (++narg < argc) {
            TGo4MbsStreamParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Stream server name not specified");
      } else
      if(strcmp(argv[narg],"-evserv")==0) {
         if (++narg < argc) {
            TGo4MbsEventServerParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Event server name not specified");
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
            showerror("MBS Event server name not specified");
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
            showerror("Remote event server name or port are not specified");
      } else
      if(strcmp(argv[narg],"-mbs-select")==0) {
         narg++;
         TGo4MbsSourceParameter* param = dynamic_cast<TGo4MbsSourceParameter*> (step->GetEventSource());
         if (param==0) showerror("only in MBS source parameters can be selected");
         unsigned cnt=0;
         while ((cnt<3) && (narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) {
            unsigned value(0);
            if ((cnt==1) && (strcmp(argv[narg],"all")==0)) value = 0; else
            if (sscanf(argv[narg],"%u",&value)!=1)
               showerror(Form("Value error %s", argv[narg]));
            if (cnt==0) param->SetStartEvent(value); else
            if (cnt==1) param->SetStopEvent(value); else
            if (cnt==2) param->SetEventInterval(value);
            narg++; cnt++;
         }
      } else
      if(strcmp(argv[narg],"-store")==0) {
         if (++narg >= argc) showerror("Store name not specified");

         const char* sourcename = argv[narg++];
         int splitlevel = 1;
         int buffersize = 64000;
         int compression = 5;

         unsigned cnt=0;
         while ((cnt<3) && (narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) {
            int value(0);
            if (sscanf(argv[narg],"%d",&value)!=1)
               showerror(Form("Value error %s", argv[narg]));
            if (cnt==0) splitlevel = value; else
            if (cnt==1) buffersize = value; else
            if (cnt==2) compression = value;
            narg++; cnt++;
         }

         TGo4FileStoreParameter storepar(sourcename, splitlevel, buffersize, compression);
         step->SetEventStore(&storepar);
         step->SetStoreEnabled(kTRUE);
      } else
      if(strcmp(argv[narg],"-timeout")==0) {
         if (++narg >= argc) showerror("Timeout value not specified");
         if (step->GetEventSource()==0) showerror("No source parameter configured");
         int value(0);
         if (sscanf(argv[narg],"%d",&value)!=1)
           showerror(Form("Timeout value error %s", argv[narg]));
         narg++;
         step->GetEventSource()->SetTimeout(value);
      } else
      if(strcmp(argv[narg],"-overwrite-store")==0) {
         narg++;
         TGo4FileStoreParameter* par = dynamic_cast<TGo4FileStoreParameter*> (step->GetEventStore());
         if (par) par->SetOverwriteMode(kTRUE);
            else showerror("No file-store parameter available");
      } else
      if(strcmp(argv[narg],"-append-store")==0) {
         narg++;
         TGo4FileStoreParameter* par = dynamic_cast<TGo4FileStoreParameter*> (step->GetEventStore());
         if (par) par->SetOverwriteMode(kFALSE);
            else showerror("No file-store parameter available");
      } else
      if(strcmp(argv[narg],"-backstore")==0) {
         if (++narg < argc) {
            TGo4BackStoreParameter storepar(argv[narg++]);
            step->SetEventStore(&storepar);
            step->SetStoreEnabled(kTRUE);
         } else
            showerror("Backstore name not specified");
      } else
      if (strcmp(argv[narg],"-number")==0) {
         if (++narg < argc) {
            if (sscanf(argv[narg++],"%ld",&maxevents)!=1) maxevents = -1;
         } else
            showerror("number of events to process not specified");
      } else
      if (strcmp(argv[narg],"-asf")==0) {
         narg++;
         analysis->SetAutoSave(kTRUE);
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            analysis->SetAutoSaveFile(argv[narg++]);
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
      if (strcmp(argv[narg],"-prefs")==0) {
         narg++;
         const char* fname = 0;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            fname = argv[narg++];
         analysis->LoadStatus(fname);
         loadprefs = kFALSE;
      } else
      if (strcmp(argv[narg],"-no-prefs")==0) {
         narg++;
         loadprefs = kFALSE;
      } else
      if(strcmp(argv[narg],"-log")==0) {
         narg++;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) narg++;
      } else
      if((strcmp(argv[narg],"-v")==0) || (strcmp(argv[narg],"-v0")==0) || (strcmp(argv[narg],"-v1")==0) || (strcmp(argv[narg],"-v2")==0) || (strcmp(argv[narg],"-v3")==0)) {
         narg++;
      } else
      if(strcmp(argv[narg],"-run")==0) {
         narg++;
         autorun = true;
      } else
      if(strcmp(argv[narg],"-norun")==0) {
         narg++;
         canrun = -1;
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
         showerror(Form("Unknown argument %d %s", narg, argv[narg]));
   }

   //------ start the analysis -------------------------
   if(batchMode) {
      cout << "**** Main: starting analysis in batch mode ...  " << endl;
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

      TGo4AnalysisClient* client = new TGo4AnalysisClient("UserClient", analysis, hostname, iport, hserver, hname, hpasswd, servermode, autorun, kFALSE, loadprefs);

      cout << "**** Main: created AnalysisClient Instance: " << client->GetName() << endl;
      cout << "**** Main: Run application loop" << endl;
      theApp.Run();
   }

   TGo4Log::CloseLogfile();

   //=================  start root application loop ==========================
   return 0;
}
