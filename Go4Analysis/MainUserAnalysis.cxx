// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
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
#include "TCanvas.h"
#include "TClassTable.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TDataType.h"
#include "TRint.h"
#include "TSystem.h"
#include "TObjString.h"
#include "Riostream.h"
#include "RVersion.h"
#include "TSysEvtHandler.h"
#include "TInterpreter.h"
#include "TStyle.h"
#include "TApplication.h"

#include "TGo4Version.h"
#include "TGo4StepFactory.h"
#include "TGo4AnalysisStep.h"
#include "TGo4Analysis.h"
#include "TGo4AnalysisClient.h"
#include "Go4EventServer.h"
#include "Go4EventServerTypes.h"
#include "TGo4FileStore.h"

#define PROCESSLOOPDELAY 20
// milliseconds
#define TERMCOUNTS 10000
// x PROCESSLOOPDELAY termination wait time

void showerror(const char* msg)
{
   std::cerr << "Error: " << msg << std::endl;
   std::cerr << "Call  'go4analysis -help'  to see list of available arguments" << std::endl;
   exit(1);
}

void printsources()
{
   std::cout << "  -file filename       :  use file filename (lmd or lml) as MBS event source (short: -f)" << std::endl;
   std::cout << "  -transport server    :  connect to MBS transport server (short: -tr)" << std::endl;
   std::cout << "  -stream server       :  connect to MBS stream server (short: -st)" << std::endl;
   std::cout << "  -evserv server       :  connect to MBS event server (short: -ev)" << std::endl;
   std::cout << "  -revserv server [port] :  connect to remote event server (short: -rev)" << std::endl;
   std::cout << "  -random              :  use random generator as source (short: -rnd)" << std::endl;
   std::cout << "  -user name           :  create user-defined event source" << std::endl;
   std::cout << "  -timeout tm          :  specify timeout parameter for event source" << std::endl;
   std::cout << "  -skip num            :  skip num first events in mbs event source" << std::endl;
   std::cout << "  -mbs-select first last step : select events interval from mbs source " << std::endl;
   std::cout << "         first: sequence number of the first event (starts from 0)" << std::endl;
   std::cout << "         last: sequence number of the last event" << std::endl;
   std::cout << "         step: step over several events, 1 means all events are used" << std::endl;

}

void usage(const char* subtopic = 0)
{
   std::cout << std::endl;
   std::cout << "GO4 analysis runnable " << __GO4RELEASE__ << std::endl;
   std::cout << "S. Linev, GSI, Darmstadt" << std::endl;

   if ((subtopic!=0) && (strlen(subtopic)>0)) {
      const char* sub = subtopic;
      if (*sub=='-') sub++;

      if ((strcmp(subtopic, "sources")==0) || (strcmp(subtopic, "src")==0) || (strcmp(subtopic, "src")==0)) {
         std::cout << "These are arguments of go4analysis which allows to specify event source" << std::endl;
         printsources();
         exit(0);
      } else
      if ((strcmp(sub, "print")==0) || (strcmp(sub, "pr")==0) ||
          (strcmp(sub, "type")==0) || (strcmp(sub, "ty")==0)) {
         std::cout << "Usage of go4analysis -print command." << std::endl;
         std::cout << std::endl;
         std::cout << "   go4analysis -print|-pr|-type|-ty [PROPT] [SOURCE] [MISC]" << std::endl;
         std::cout << std::endl;
         std::cout << "PROPT: print options, right after -print command" << std::endl;
         std::cout << "   hex          : print data in hexadecimal format" << std::endl;
         std::cout << "   dec          : print data in decimal format" << std::endl;
         std::cout << "   long         : print data in long (4 bytes) form (default)" << std::endl;
         std::cout << "   short        : print data in short (2 bytes) form" << std::endl;
         std::cout << "   sub=N        : select subevent id N (default all subevents are shown)" << std::endl;
         std::cout << "   fhead        : print current lmd file header" << std::endl;
         std::cout << "   bhead        : print current buffer header" << std::endl;
         std::cout << "SOURCE: event source print options" << std::endl;
         printsources();
         std::cout << "MISC: other options, which may be relevant for \"print\" command" << std::endl;
         std::cout << "   -number M    : print M events, default is 1 (short: -num)" << std::endl;
         std::cout << "   -lib name    : load library, may be required for user-defined sources" << std::endl;
         std::cout << "   -v           : enable verbose mode to see some debug output (default: -v2)" << std::endl;
         std::cout << std::endl;
         std::cout << "  Print event header from MBS stream server" << std::endl;
         std::cout << "     go4analysis -stream r4-4 -print " << std::endl;
         std::cout << std::endl;
         std::cout << "  Print event data in hexadecimal form from MBS event server" << std::endl;
         std::cout << "     go4analysis -ev r2-1 -pr hex" << std::endl;
         std::cout << std::endl;
         std::cout << "  Print 5 events with subevent id=1 in decimal form from MBS transport server" << std::endl;
         std::cout << "     go4analysis -tr r2-2 -num 5 -ty dec sub=1 " << std::endl;
         std::cout << std::endl;
         exit(0);
      } else {
         std::cout << "No help for topic: \""<< subtopic << "\"" << std::endl;
         std::cout << "Available: print, sources" << std::endl;
         exit(0);
      }
   }

   std::cout << "calling:                " << std::endl;
   std::cout << "" << std::endl;
   std::cout << "   go4analysis [RUN] [ANALYSIS] [STEP1] [STEP2] ... [USER]" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "RUN: configuration, relevant for application run mode" << std::endl;
   std::cout << "  -lib name                   : user library to load (default: libGo4UserLibrary)" << std::endl;
   std::cout << "  -server [name]              : run analysis in server mode, name - optional analysis name" << std::endl;
   std::cout << "  -gui name guihost guiport   : run analysis in gui mode, used by GUI launch analysis" << std::endl;
#ifdef WITH_HTTP
   std::cout << "  -http [port]                : run analysis with web-server running, " << std::endl;
   std::cout << "                                optionally port can be specified, default 8080" << std::endl;
   std::cout << "  -auth [filename]            : use authentication file to restrict access to http server " << std::endl;
   std::cout << "                                file should contain 'controller' and 'admin' accounts for 'go4' domain" << std::endl;
   std::cout << "                                Could be generated with htdigets utility, by default '$GO4SYS/etc/.htdigest' filename is used" << std::endl;
   std::cout << "  -fastcgi port               : run analysis with fastcgi server running, "<< std::endl;
   std::cout << "                                which can deliver data to normal webserver (see mod_proxy_fcgi for Apache)" << std::endl;
   std::cout << "  -jsroot location            : use JSROOT from other location like https://root.cern.ch/js/latest/" << std::endl;
#ifdef WITH_DABC
   std::cout << "  -dabc master_host:port      : run analysis with optional connection to dabc application, "<< std::endl;
   std::cout << "                                which could receive objects from running analysis" << std::endl;
#endif
#endif
   std::cout << "  -run                        : run analysis in server mode (default only run if source specified)" << std::endl;
   std::cout << "  -norun                      : exclude automatical run" << std::endl;
   std::cout << "  -number NUMBER              : process NUMBER events in batch mode" << std::endl;
   std::cout << "  -hserver [name [passwd]]    : start histogram server with optional name and password" << std::endl;
   std::cout << "  -log [filename]             : enable log output into filename (default:go4logfile.txt)" << std::endl;
   std::cout << "  -v -v0 -v1 -v2 -v3          : change log output verbosity (0 - maximum, 1 - info, 2 - warn, 3 - errors)" << std::endl;
   std::cout << "  -gdebug [lvl]               : set ROOT gDebug value, default 1" << std::endl;
   std::cout << "  -rate                       : display rate information during run" << std::endl;
   std::cout << "  -print [sub=N] [hex|dec]    : print events, see -help print for more info" << std::endl;
   std::cout << "  -help [topic]               : show this help or for selected topic" << std::endl;
   std::cout << "  -graphics                   : enable graphics in the analysis" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "ANALYSIS: common analysis configurations" << std::endl;
   std::cout << "  -name name             :  specify analysis instance name" << std::endl;
   std::cout << "  -asf [filename]        :  enable store autosave file and set autosave filename (default <Name>ASF.root)" << std::endl;
   std::cout << "  -enable-asf [interval] :  enable store of autosave file, optionally interval in seconds" << std::endl;
   std::cout << "  -disable-asf           :  disable usage of asf" << std::endl;
   std::cout << "  -prefs [filename]      :  load preferences (analysis configuration) from specified file (default Go4AnalysisPrefs.root)" << std::endl;
   std::cout << "  -no-prefs              :  disable preferences loading" << std::endl;
   std::cout << "  -maxtreesize value     :  define maximum tree size, value can be: 2g, 1900m, 1900000000" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "STEP: individual step configurations" << std::endl;
   std::cout << "  -step name           :  select step by it's name, if not found, first step will be used" << std::endl;
   std::cout << "  -step number         :  select step by it's number (first step has number 0)" << std::endl;
   std::cout << "  -enable-step         :  enable step processing" << std::endl;
   std::cout << "  -disable-step        :  disable step processing" << std::endl;
   std::cout << "  -file filename       :  use file filename (lmd or lml) as MBS event source" << std::endl;
   std::cout << "  -transport server    :  connect to MBS transport server" << std::endl;
   std::cout << "  -stream server       :  connect to MBS stream server" << std::endl;
   std::cout << "  -evserv server       :  connect to MBS event server" << std::endl;
   std::cout << "  -revserv server [port] :  connect to remote event server" << std::endl;
   std::cout << "  -port number         :  select custom port number for event source" << std::endl;
   std::cout << "  -retry number        :  select number of retries when connection to source was lost" << std::endl;
   std::cout << "  -random              :  use random generator as source" << std::endl;
   std::cout << "  -user name           :  create user-defined event source" << std::endl;
   std::cout << "  -source filename     :  read step input from the root file" << std::endl;
   std::cout << "  -skip num            :  skip num first events in mbs event source" << std::endl;
   std::cout << "  -mbs-select first last step : select events interval from mbs source" << std::endl;
   std::cout << "  -timeout tm          :  specify timeout parameter for event source" << std::endl;
   std::cout << "  -enable-source       :  enable step source" << std::endl;
   std::cout << "  -disable-source      :  disable step source" << std::endl;
   std::cout << "  -store filename [split buffersize compression] :  write step output into the root file" << std::endl;
   std::cout << "  -overwrite-store     :  overwrite file, when store output" << std::endl;
   std::cout << "  -append-store        :  append to file, when store output" << std::endl;
   std::cout << "  -backstore name      :  create backstore for online tree draw" << std::endl;
   std::cout << "  -enable-store        :  enable step store" << std::endl;
   std::cout << "  -disable-store       :  disable step store" << std::endl;
   std::cout << "  -enable-errstop      :  enable stop-on-error mode" << std::endl;
   std::cout << "  -disable-errstop     :  disable stop-on-error mode" << std::endl;
   std::cout << "  -inpevt-class name   :  (re)define class name of input event" << std::endl;
   std::cout << "  -outevt-class name   :  (re)define class name of output event" << std::endl;
   std::cout << std::endl;
   std::cout << "USER: user-defined arguments" << std::endl;
   std::cout << "  -args [userargs]     :  create user analysis with constructor (int argc, char** argv) signature" << std::endl;
   std::cout << "                          all following arguments will be provided as array of strings, first argument - analysis name" << std::endl;
   std::cout << std::endl;

   exit(0);
}

class TGo4PrintProcessor : public TGo4EventProcessor {
   public:
      TGo4PrintProcessor(const char* name) : TGo4EventProcessor(name) {}

      static Int_t  fSubId;
      static Bool_t fHex;
      static Bool_t fLong;
      static Bool_t fData;
      static Bool_t fFileHead;
      static Bool_t fBufHead;

      virtual Bool_t BuildEvent(TGo4EventElement* dest)
      {
         TGo4EventElement* evnt = GetInputEvent();

         TGo4MbsEvent* mbs = dynamic_cast<TGo4MbsEvent*> (evnt);
         if (mbs) mbs->PrintMbsEvent(fSubId, fLong, fHex, fData, fBufHead, fFileHead);
             else evnt->PrintEvent();

         return kTRUE;
      }
};

Int_t TGo4PrintProcessor::fSubId = -1;
Bool_t TGo4PrintProcessor::fHex = kTRUE;
Bool_t TGo4PrintProcessor::fLong = kTRUE;
Bool_t TGo4PrintProcessor::fData = kFALSE;
Bool_t TGo4PrintProcessor::fFileHead = kFALSE;
Bool_t TGo4PrintProcessor::fBufHead = kFALSE;

class TGo4PrintFactory : public TGo4StepFactory {
   public:
      TGo4PrintFactory(const char* name) : TGo4StepFactory(name) {}
      TGo4EventProcessor* CreateEventProcessor(TGo4EventProcessorParameter* par)
      {
         return new TGo4PrintProcessor("PrintProc");
      }
};


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

TGo4Analysis* CreateDefaultAnalysis(TList* lst, const char* name, int user_argc, char** user_argv, bool doprint)
{
   TIter iter(lst);
   TObject* obj(0);

   TObjArray evnt_classes; // list of found event classes

   TClass *proc_cl(0), *an_cl(0), *evsrc_cl(0);

   while ((obj = iter()) != 0) {
      TClass* cl = TClass::GetClass(obj->GetName());

      if (cl==0) continue;

      if (cl->InheritsFrom(TGo4EventProcessor::Class())) {
         // if several processor classes are existing, take higher in hierarchy
         if ((cl!=TGo4EventProcessor::Class()) && ((proc_cl==0) || cl->InheritsFrom(proc_cl))) proc_cl = cl;
      } else
      if (cl->InheritsFrom(TGo4EventSource::Class())) {
         if ((cl!=TGo4EventSource::Class()) && (evsrc_cl==0)) evsrc_cl = cl;
      } else
      if (cl->InheritsFrom(TGo4EventElement::Class())) {
         if (cl!=TGo4EventElement::Class()) evnt_classes.Add(cl);
      } else
      if (cl->InheritsFrom(TGo4Analysis::Class())) {
         if ((cl!=TGo4Analysis::Class()) && (an_cl==0)) an_cl = cl;
      }
   }

   if (doprint) {
      an_cl = 0;
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

	 #ifdef WIN32
         TString cmd = TString::Format("new %s(%d, (char**)0x%x)", an_cl->GetName(), user_argc, user_argv);
    #else
         TString cmd = TString::Format("new %s(%d, (char**)%p)", an_cl->GetName(), user_argc, user_argv);
	 #endif
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

         TString cmd = TString::Format("new %s(\"%s\")", an_cl->GetName(), name);
         Int_t err = 0;

         TGo4Log::Info("Process: %s", cmd.Data());

         TGo4Analysis* analysis = (TGo4Analysis*) gROOT->ProcessLineFast(cmd.Data(), &err);

         if ((analysis!=0) && (err==0)) return analysis;

         TGo4Log::Error("Cannot create analysis class %s instance with (const char*) prototype", an_cl->GetName());
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

      TString cmd = TString::Format("new %s(", an_cl->GetName());

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
               cmd += TString::Format("%d", GO4EV_MBS_FILE);
            } else
            if (basictype=="bool") {
               cmd += "false";
            } else
               cmd += "0";
         }
      }

      cmd += ")";

      Int_t err = 0;
      TGo4Log::Info("Process: %s", cmd.Data());
      TGo4Analysis* analysis = (TGo4Analysis*) gROOT->ProcessLineFast(cmd.Data(), &err);
      if ((analysis!=0) && (err==0)) return analysis;

      TGo4Log::Error("Cannot create analysis class %s instance", an_cl->GetName());
      TGo4Log::Error("Add CreateUserAnalysis(const char*) function in user library");

      exit(1);
   }

   TClass *outev_cl(0), *inpev_cl(0);

   const char* inp_evnt_classname = GetArgValue(user_argc, user_argv, "-inpevt-class");
   const char* out_evnt_classname = GetArgValue(user_argc, user_argv, "-outevt-class");

   if (inp_evnt_classname!=0) {
      inpev_cl = gROOT->GetClass(inp_evnt_classname);
      if (inpev_cl==0) {
         TGo4Log::Error("Class %s not exists", inp_evnt_classname);
         exit(1);
      }

      if (!inpev_cl->InheritsFrom(TGo4EventElement::Class())) {
         TGo4Log::Error("Class %s cannot be used as input event", inp_evnt_classname);
         exit(1);
      }

      evnt_classes.Remove(inpev_cl);
      evnt_classes.Compress();
   }

   if (out_evnt_classname!=0) {
      outev_cl = gROOT->GetClass(out_evnt_classname);
      if (outev_cl==0) {
         TGo4Log::Error("Class %s not exists", out_evnt_classname);
         exit(1);
      }

      if (!outev_cl->InheritsFrom(TGo4EventElement::Class())) {
         TGo4Log::Error("Class %s cannot be used as output event", out_evnt_classname);
         exit(1);
      }

      evnt_classes.Remove(outev_cl);
      evnt_classes.Compress();
   }

   // check if user event source requires special event class
   // create instance only if we have something to check
   if ((evsrc_cl!=0) && (inpev_cl==0) && (evnt_classes.GetLast() >= 0)) {
      TGo4EventSource* src = (TGo4EventSource*) evsrc_cl->New();

      // if special input event is required, try to detect it
      if ((src!=0) && !src->CheckEventClass(TGo4MbsEvent::Class())) {
         for (int n=0; n<=evnt_classes.GetLast(); n++) {
            TClass* cl = (TClass*) evnt_classes.At(n);
            if (!src->CheckEventClass(cl)) continue;

            // if more than two classes are suited - ignore any of them
            if (inpev_cl!=0) { inpev_cl = 0; break; }

            inpev_cl = cl;
         }
      }

      delete src;

      if (inpev_cl!=0) {
         evnt_classes.Remove(inpev_cl);
         evnt_classes.Compress();
      }
   }

   // as very last, try to define best-suitable output event
   if (outev_cl==0)
      for (int n=0; n<=evnt_classes.GetLast(); n++) {
         TClass* cl = (TClass*) evnt_classes.At(n);
         if ((outev_cl==0) || cl->InheritsFrom(outev_cl)) outev_cl = cl;
      }

   if (doprint) {
      TGo4Log::Info("Create default analysis with print-processor class");
      outev_cl = 0;
   } else {
      if (proc_cl==0) return 0;
      TGo4Log::Info("Create default analysis with processor class %s", proc_cl->GetName());
      if (outev_cl!=0)
         TGo4Log::Info("Use class %s as output event", outev_cl->GetName());
   }


   if (inpev_cl!=0)
      TGo4Log::Info("Use class %s as input event", inpev_cl->GetName());

   TGo4Analysis* analysis = TGo4Analysis::Instance();
   analysis->SetAnalysisName(name);

   TGo4StepFactory* factory = 0;

   if (doprint) {
      factory = new TGo4PrintFactory("Factory");
   } else {
      factory = new TGo4StepFactory("Factory");
      factory->DefEventProcessor("Processor", proc_cl->GetName());// object name, class name
   }

   if (inpev_cl!=0)
      factory->DefInputEvent("InputEvent", inpev_cl->GetName()); // object name, class name

   if (outev_cl!=0)
      factory->DefOutputEvent("OutputEvent", outev_cl->GetName()); // object name, class name
   else
      factory->DefOutputEvent("OutputEvent", "TGo4EventElement"); // object name, class name

   if (evsrc_cl!=0)
      factory->DefUserEventSource(evsrc_cl->GetName());

   TGo4MbsFileParameter* sourcepar = new TGo4MbsFileParameter(analysis->GetDefaultTestFileName());

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
      std::cerr << "Please check your system configuration and restart analysis again" << std::endl;
      return -1;
   }

   if (argc==1) usage();

   int phelp = FindArg(argc, argv, "-help");
   if (phelp < 0) phelp = FindArg(argc, argv, "-h") > 0;
   if (phelp > 0) {
      usage(phelp < argc-1 ? argv[phelp+1] : 0);
   }

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

   bool doprint = (FindArg(argc, argv, "-print") > 0) || (FindArg(argc, argv, "-type") > 0) ||
                  (FindArg(argc, argv, "-pr") > 0) || (FindArg(argc, argv, "-ty") > 0);
   if (doprint) TGo4Log::SetIgnoreLevel(2);

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

   TGo4Analysis::SetRunningMode(0);
   if (FindArg(argc, argv, "-gui")>0) TGo4Analysis::SetRunningMode(1); else
   if (FindArg(argc, argv, "-server")>0) TGo4Analysis::SetRunningMode(2);

   // disable cint locking when called via process line
   // makes problem in multi-threaded environment, where CINT lock
   // also used for object streaming
   gInterpreter->SetProcessLineLock(kFALSE);

   // add main go4 include path for the case if scripts are called from inside analysis, required for ROOT6
   TString go4inc = TGo4Log::GO4INCPATH();
   if (go4inc.Length()==0) go4inc = TGo4Log::subGO4SYS("include");
   if (go4inc.Length()>0)
      gInterpreter->AddIncludePath(go4inc.Data());

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

   // we should try to load library if for printing -user argument specified - means user input
   if (!isanylib && (!doprint || GetArgValue(argc, argv, "-user"))) {
      libname = "libGo4UserAnalysis";
      TGo4Log::Info("Reading library: %s", libname);
      if (gSystem->Load(libname)<0) return -1;
   }

   TList* lst1 = GetClassesList(lst0);

   TGo4Analysis* analysis = 0;

   UserCreateFunc* crfunc = (UserCreateFunc*) gSystem->DynFindSymbol("*", "CreateUserAnalysis");
   if (crfunc) analysis = crfunc(analysis_name);
          else analysis = CreateDefaultAnalysis(lst1, analysis_name, user_argc, user_argv, doprint);

   if (analysis==0) {
      std::cerr << "!!! Analysis instance cannot be created" << std::endl;
      std::cerr << "!!! PLEASE check your analysis library " << libname << std::endl;
      std::cerr << "!!! One requires user subclass for TGo4Analysis class in user library" << std::endl;
      std::cerr << "!!! Alternatively, CreateUserAnalysis(const char*) function can be implemented" << std::endl;
      std::cerr << "!!! See Go4ExampleSimple, Go4Example1Step or Go4Example2Step for details" << std::endl;
      return -1;
   }

   delete lst0; lst0 = 0;
   delete lst1; lst1 = 0;

   TGo4AnalysisStep* step = analysis->GetAnalysisStep(0);
   if (step==0) {
      std::cerr << "No active step in analysis found" << std::endl;
      return -1;
   }

   int app_argc = 1;
   // char* app_argv[] = { argv[0], (char*) "-l" };
   // TApplication theApp("Go4App", &app_argc, argv);

   gROOT->SetBatch(kTRUE);

   TObjArray http_args;                  // all arguments for http server
   http_args.SetOwner(kTRUE);
   TString auth_file;                    // authentication file for http server
   const char* auth_domain = "go4";      // default authentication domain http server

   Bool_t batchMode(kTRUE);              // GUI or Batch
   Bool_t servermode(kFALSE);            // run analysis as server task
   Bool_t httpmode(kFALSE);              // run analysis with web server
   Bool_t hserver(kFALSE);               // enable histogram server
   Bool_t loadprefs(kTRUE);              // loading preferences by client
   Bool_t showrate(kFALSE);              // display event rate
   Double_t process_interv(-1.);         // interval how often system events will be processed by analysis
   const char* hname  = "";              // namebase for histogram server
   const char* hpasswd  = "";            // password for histogram server
   const char* hostname = "localhost";   // gui host name
   Int_t iport(5000);                    // port number used by GUI

   bool autorun(false);    // immediately run analysis on start
   long  maxevents(-1);    // number of events (batch mode)
   Int_t canrun(0);        // -1 cannot run, 0 - only if source specify, 1 - always

   if (doprint) {
      maxevents = 1;
      analysis->SetAutoSave(kFALSE);
   }

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
#ifdef WITH_HTTP
      if (strcmp(argv[narg], "-http")==0) {
         narg++;
         httpmode = kTRUE;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            http_args.Add(new TObjString(Form("http:%s?top=Go4", argv[narg++])));
         else
            http_args.Add(new TObjString("http:8080?top=Go4"));
      } else
      if (strcmp(argv[narg], "-fastcgi")==0) {
         narg++;
         httpmode = kTRUE;
         if (narg >= argc) showerror("fastcgi options not specified");
         http_args.Add(new TObjString(Form("fastcgi:%s?top=Go4", argv[narg++])));
      } else
      if (strcmp(argv[narg], "-jsroot")==0) {
         narg++;
         if (narg >= argc) showerror("jsroot location not specified");
         http_args.Add(new TObjString(Form("jsroot:%s", argv[narg++])));
      } else
      if (strcmp(argv[narg], "-auth")==0) {
         narg++;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            auth_file = argv[narg++];
         else
            auth_file = TGo4Log::subGO4SYS("etc/htdigest.txt");
      } else
      if (strcmp(argv[narg], "-domain")==0) {
         narg++;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            auth_domain = argv[narg++];
         else
            auth_domain = "go4";
      } else
#ifdef WITH_DABC
      if (strcmp(argv[narg], "-dabc")==0) {
         narg++;
         if (narg >= argc) showerror("Master dabc node not specified");
         const char* hostname = gSystem->HostName();
         if ((hostname==0) || (*hostname==0)) hostname = "localhost";
         http_args.Add(new TObjString(Form("dabc:%s?top=Go4/%s_pid%d", argv[narg++],hostname,gSystem->GetPid())));
      } else
#endif
#endif
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
      if ((strcmp(argv[narg],"-file")==0) || (strcmp(argv[narg],"-f")==0)) {
         if (++narg < argc) {
            TGo4MbsFileParameter sourcepar(argv[narg++]);
            // this is case when many files are specified at once
            while ((narg<argc) && (argv[narg][0]!='-'))
               sourcepar.AddMoreFile(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("LMD/LML file name not specified");
      } else
      if (strcmp(argv[narg],"-source")==0) {
         if (++narg < argc) {
            TGo4FileSourceParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("Input file name not specified");
      } else
      if ((strcmp(argv[narg],"-transport")==0) || (strcmp(argv[narg],"-tr")==0)) {
         if (++narg < argc) {
            TGo4MbsTransportParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Transport server name not specified");
      } else
      if ((strcmp(argv[narg],"-stream")==0) || (strcmp(argv[narg],"-st")==0)) {
         if (++narg < argc) {
            TGo4MbsStreamParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Stream server name not specified");
      } else
      if ((strcmp(argv[narg],"-evserv")==0) || (strcmp(argv[narg],"-ev")==0))  {
         if (++narg < argc) {
            TGo4MbsEventServerParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Event server name not specified");
      } else
      if ((strcmp(argv[narg],"-random")==0) || (strcmp(argv[narg],"-rnd")==0)) {
         narg++;
         TGo4MbsRandomParameter sourcepar("Random");
         step->SetEventSource(&sourcepar);
         step->SetSourceEnabled(kTRUE);
         autorun = true;
      } else
      if (strcmp(argv[narg],"-user")==0) {
         if (++narg < argc) {
            TGo4UserSourceParameter sourcepar(argv[narg++]);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("MBS Event server name not specified");
      } else
      if ((strcmp(argv[narg],"-revserv")==0) || (strcmp(argv[narg],"-rev")==0)) {
         if (++narg < argc) {
            const char* serv_name = argv[narg++];
            int serv_port = 0;
            if ((narg < argc) && (argv[narg][0] != '-'))
               serv_port = atoi(argv[narg++]);
            TGo4RevServParameter sourcepar(serv_name, serv_port);
            step->SetEventSource(&sourcepar);
            step->SetSourceEnabled(kTRUE);
            autorun = true;
         } else
            showerror("Remote event server name or port are not specified");
      } else
      if (strcmp(argv[narg],"-skip")==0) {
         narg++;
         TGo4MbsSourceParameter* param = dynamic_cast<TGo4MbsSourceParameter*> (step->GetEventSource());
         if (param==0) showerror("only in MBS source events can be skipped");
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) {
            unsigned value=0;
            if (sscanf(argv[narg],"%u",&value)!=1)
               showerror(Form("Value error %s", argv[narg]));
            param->SetStartEvent(value);
            narg++;
         }
      } else
      if (strcmp(argv[narg],"-port")==0) {
         narg++;
         int port(0);
         if ((narg < argc) && (argv[narg][0]!='-')) {
            if (sscanf(argv[narg],"%d",&port)!=1)
               showerror(Form("Value error %s", argv[narg]));
            narg++;
         }
         TGo4MbsSourceParameter* param = dynamic_cast<TGo4MbsSourceParameter*> (step->GetEventSource());
         if (param) param->SetPort(port);

         TGo4UserSourceParameter* user = dynamic_cast<TGo4UserSourceParameter*> (step->GetEventSource());
         if (user) user->SetPort(port);
      } else
      if (strcmp(argv[narg],"-retry")==0) {
         narg++;
         int nretry(0);
         if ((narg < argc) && (argv[narg][0]!='-')) {
            if (sscanf(argv[narg],"%d",&nretry)!=1)
               showerror(Form("Value error %s", argv[narg]));
            narg++;
         }
         TGo4MbsSourceParameter* param = dynamic_cast<TGo4MbsSourceParameter*> (step->GetEventSource());
         if (param) param->SetRetryCnt(nretry);
      } else
      if (strcmp(argv[narg],"-mbs-select")==0) {
         narg++;
         TGo4MbsSourceParameter* param = dynamic_cast<TGo4MbsSourceParameter*> (step->GetEventSource());
         if (param==0) showerror("only in MBS source events can be selected");
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
      if (strcmp(argv[narg], "-timeout")==0) {
         if (++narg >= argc) showerror("Timeout value not specified");
         if (step->GetEventSource()==0) showerror("No source parameter configured");
         int value(0);
         if (sscanf(argv[narg],"%d",&value)!=1)
           showerror(Form("Timeout value error %s", argv[narg]));
         narg++;
         step->GetEventSource()->SetTimeout(value);
      } else
      if (strcmp(argv[narg],"-overwrite-store")==0) {
         narg++;
         TGo4FileStoreParameter* par = dynamic_cast<TGo4FileStoreParameter*> (step->GetEventStore());
         if (par) par->SetOverwriteMode(kTRUE);
            else showerror("No file-store parameter available");
      } else
      if (strcmp(argv[narg],"-append-store")==0) {
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
      if ((strcmp(argv[narg],"-events")==0) || (strcmp(argv[narg],"-number")==0) || (strcmp(argv[narg],"-num")==0)) {
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
      if(strcmp(argv[narg],"-gdebug")==0) {
         narg++;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-'))
            gDebug = TString(argv[narg++]).Atoi();
         else
            gDebug = 1;
      } else
      if((strcmp(argv[narg],"-v")==0) || (strcmp(argv[narg],"-v0")==0) || (strcmp(argv[narg],"-v1")==0) || (strcmp(argv[narg],"-v2")==0) || (strcmp(argv[narg],"-v3")==0)) {
         narg++;
      } else
      if((strcmp(argv[narg],"-graphics")==0) || (strcmp(argv[narg],"-gr")==0)) {
         narg++;
         gROOT->SetBatch(kFALSE);
         process_interv = 0.1; // allow system events processing
         new TApplication("Go4App", &app_argc, argv);
         TStyle::BuildStyles();
         gROOT->SetStyle();
      } else
      if(strcmp(argv[narg],"-run")==0) {
         narg++;
         autorun = true;
      } else
      if(strcmp(argv[narg],"-norun")==0) {
         narg++;
         canrun = -1;
      } else
      if(strcmp(argv[narg],"-maxtreesize")==0) {
         narg++;
         long long maxtreesize = 1900000000;
         if ((narg < argc) && (strlen(argv[narg]) > 0) && (argv[narg][0]!='-')) {
            char sbuf[1000];
            strncpy(sbuf, argv[narg], sizeof(sbuf));
            long long mult(1), val(1);
            int len = strlen(sbuf);
            if ((sbuf[len-1]=='g') || (sbuf[len-1]=='G')) { mult = 1000000000; sbuf[len-1] = 0; } else
            if ((sbuf[len-1]=='m') || (sbuf[len-1]=='M')) { mult = 1000000; sbuf[len-1] = 0; } else
            if ((sbuf[len-1]=='k') || (sbuf[len-1]=='K')) { mult = 1000; sbuf[len-1] = 0; }
            if (sscanf(sbuf, "%lld", &val)==1) maxtreesize = val*mult;
            narg++;
         }
         TGo4Log::Info("Set tree file size limit to %lld bytes", maxtreesize);
         TGo4FileStore::SetMaxTreeSize(maxtreesize);
      } else
      if ((strcmp(argv[narg],"-print")==0) || (strcmp(argv[narg],"-type")==0) || (strcmp(argv[narg],"-ty")==0) || (strcmp(argv[narg],"-pr")==0)) {
         narg++;
         while ((narg<argc) && (argv[narg][0]!='-')) {
            if (strncmp(argv[narg],"sub=",4)==0) {
               TGo4PrintProcessor::fSubId = atoi(argv[narg] + 4);
            } else
            if (strcmp(argv[narg],"hex")==0) {
               TGo4PrintProcessor::fHex = kTRUE;
               TGo4PrintProcessor::fData = kTRUE;
            } else
            if (strcmp(argv[narg],"dec")==0) {
               TGo4PrintProcessor::fHex = kFALSE;
               TGo4PrintProcessor::fData = kTRUE;
            } else
            if (strcmp(argv[narg],"long")==0) {
               TGo4PrintProcessor::fLong = kTRUE;
               TGo4PrintProcessor::fData = kTRUE;
            } else
            if (strcmp(argv[narg],"short")==0) {
               TGo4PrintProcessor::fLong = kFALSE;
               TGo4PrintProcessor::fData = kTRUE;
            } else
            if (strcmp(argv[narg],"data")==0) {
               TGo4PrintProcessor::fData = kTRUE;
            } else
            if (strcmp(argv[narg],"nodata")==0) {
               TGo4PrintProcessor::fData = kFALSE;
            } else
            if (strcmp(argv[narg],"fhead")==0) {
               TGo4PrintProcessor::fFileHead = kTRUE;
            } else
            if (strcmp(argv[narg],"bhead")==0) {
                TGo4PrintProcessor::fBufHead = kTRUE;
            }
            narg++;
         }
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
      if(strcmp(argv[narg],"-rate")==0) {
         narg++;
         showrate = kTRUE;
      } else
      if(strcmp(argv[narg],"-norate")==0) {
         narg++;
         showrate = kFALSE;
      } else
      if(strcmp(argv[narg],"-inpevt-class")==0) {
         // these arguments used only in simple analysis with single step and
         // processed when analysis created
         narg++;
         if (narg < argc) narg++;
      } else
      if(strcmp(argv[narg],"-outevt-class")==0) {
         // these arguments used only in simple analysis with single step and
         // processed when analysis created
         narg++;
         if (narg < argc) narg++;
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

   #ifdef WITH_HTTP
   if (http_args.GetLast()>=0) {
      if (gSystem->Load("libGo4Http")!=0)
         showerror("Fail to load libGo4Http.so library");

      TGo4Log::EnableRedirection(); // one sniff complete std out

      TString cmd;
      Long_t res(0);
      Int_t err(0);
      for (Int_t n=0;n<=http_args.GetLast();n++) {
         TString engine = http_args[n]->GetName();
         if ((engine.Index("http:")==0) && (auth_file.Length()>0))
            engine.Append(TString::Format("&auth_file=%s&auth_domain=%s", auth_file.Data(), auth_domain));

         cmd.Form("TGo4Sniffer::CreateEngine(\"%s\");", engine.Data());
         res = gROOT->ProcessLineFast(cmd.Data(), &err);
         if ((res<=0) || (err!=0)) {
            printf("Fail to start %s", engine.Data());
            return 1;
         }
      }

      process_interv = 0.1;
   }
   #endif

   //------ start the analysis -------------------------
   if(batchMode) {
      TGo4Log::Info("Main: starting analysis in batch mode ...  ");

      Bool_t enter_loop = kTRUE;
      if ((canrun>=0) || autorun) {
         // initialize event classes also with -norun option if any source was specified
         if (!analysis->InitEventClasses()) {
            TGo4Log::Error("Main: Init event classes failed, aborting!");
            enter_loop = kFALSE;
         } else {
            if (canrun>=0) analysis->StartAnalysis();
         }
      }

      if (enter_loop) {
         analysis->RunImplicitLoop(maxevents, showrate, process_interv, httpmode);
         delete analysis;
         TGo4Log::Info("Main: analysis batch done");
      }
   } else {
      if (hostname==0) hostname = "localhost";

      if(servermode)  TGo4Log::Info("Main: starting analysis in server mode ...");
      else            TGo4Log::Info("Main: starting analysis in slave mode ...");

      if (canrun<0) autorun = false;

      TGo4AnalysisClient* client = new TGo4AnalysisClient("UserClient", analysis, hostname, iport, hserver, hname, hpasswd, servermode, autorun, kFALSE, loadprefs, showrate);

      TGo4Log::Info("Main: created AnalysisClient instance: %s", client->GetName());
      TGo4Log::Info("Main: Run application loop");

#ifndef WIN32
#if ROOT_VERSION_CODE <= ROOT_VERSION(5,25,2)
      // workaround TUnixSystem::DispatchOneEvent problem
      gSystem->AddFileHandler(new TFileHandler(0, TFileHandler::kRead));
#endif
#endif

      int termcounter=0;

      while (TGo4Analysis::Exists()) {
         // add this check while at some moments ROOT could reset this pointer
         if (gSystem==0) {
            // printf("ROOT feature - gSystem==0, break execution\n");

            // exit call exit handler and produces even more problems
            // we trying to close application as soon as possible - anyway many
            // items already cleaned up
            abort();
            //exit(5);
         }

         gSystem->ProcessEvents();
         gSystem->Sleep(PROCESSLOOPDELAY);
         if(client->IsBeingQuit()) {
            if(termcounter == 0) {
               termcounter = TERMCOUNTS;
               TGo4Log::Info("Found Quit state: starting termination counter with %d s", PROCESSLOOPDELAY * TERMCOUNTS / 1000);
            } else
            if (termcounter>0) {
               termcounter--;
               if (termcounter == 0) {
                  TGo4Log::Info("Reached end of termination counter after %d s, terminating.", PROCESSLOOPDELAY * TERMCOUNTS / 1000);
                  break;
               }
               if ((termcounter % (10000 / PROCESSLOOPDELAY)) == 0)
                  TGo4Log::Info("Counting termination counter down, remains %d s", PROCESSLOOPDELAY * termcounter / 1000);
            }
         }
      }

   }

   TGo4Log::Info("Main: THE END");
   TGo4Log::CloseLogfile();

   return 0;
}
