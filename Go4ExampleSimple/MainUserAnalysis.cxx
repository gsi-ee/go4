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
#include "Go4EventServerTypes.h"

void usage(); // print announcement

TROOT go4application("GO4","Go4 user analysis");

#define kGUI   2
#define kBatch 1

//==================  analysis main program ============================
int main(int argc, char **argv)
{
if(argc < 3) {
  usage(); // no argument: print usage
  exit(0);
}

//=================  init root application loop ==========================
TApplication theApp("Go4App", 0, 0);

//================= Begin analysis setup ==================================
// argv[0] program
// argv[1] "-gui" when started by GUI.
//         In this case the following args are (delivered by GUI):
// argv[2] analysis name as specified in GUI
// argv[3] hostname of GUI
// argv[4] connector port of GUI
////////////
// argv[1] "-server" when started as analysis server by GUI client
// argv[2] optional name
////////////
//         if not -gui or -server the args are for batch (designed by user):
// argv[1] -file|-trans|-stream|-evserv|-revserv|-random
// argv[2] input name (file or server)
// argv[3] optional -server // use batch arguments, but run immediately as server
// argv[4] optional -port portnumber
// argv[5] optional -output
// argv[6] optional number of events

Bool_t servermode=kFALSE; // run analysis slave as servertask
Bool_t autorun=kFALSE;    // immedeately run analysis on start
Int_t  runningMode;      // kGUI or kBatch
Int_t  maxevents = -1;   // number of events (batch mode)
Bool_t writeout=kFALSE;  // write output file
Text_t hostname[128];    // hostname used by GUI
UInt_t iport=5000;       // port number used by GUI
Int_t  iarg;              // argument index
UInt_t sport=6003;       // remote event server port
Text_t source[128];      // input source
Text_t serv[128];        // input name
Text_t asf[128];         // auto save file (batch)
Text_t filetype[8];      // file type .lmd or .lml
Text_t *pc,*tmpname,*outname;
TGo4AnalysisClient* client;

strcpy(hostname,"localhost");
//------ process arguments -------------------------

// strip any path information from input:
// outname is name without path and postfix
// postfix .lmd is default in all cases
// postfix .lml is a metafile, @ is prefixed
strcpy(filetype,".lmd");
tmpname=argv[2]+strlen(argv[2])-4;
if((pc=strstr(tmpname,".lmd")) != 0)*pc=0;
if((pc=strstr(tmpname,".lml")) != 0){*pc=0;strcpy(filetype,".lml");}
tmpname=argv[2];
if((pc=strstr(argv[2],"@")) != 0) tmpname++;
outname=tmpname;
if((tmpname=strrchr(outname,'/')) != 0) outname=tmpname+1;
strncpy(asf,outname,110);     // auto save file
strcat(asf,"_AS");
strcpy(source,".x file.C(\"/GSI/lea/gauss\")"); // will be processed below
strncpy(serv,argv[2],110);

if(strcmp(argv[1],"-gui") == 0)
{ // set up arguments for GUI mode
   runningMode = kGUI;
   strncpy(hostname,argv[3],110);
   iport = (argc>4) ? atoi(argv[4]) : 5000; // port of GUI server
   writeout=kFALSE;       // no output
 }
else if(strstr(argv[1],"-server"))
 {
// set up analysis server mode started from GUI -client
   runningMode = kGUI;
   servermode=kTRUE;
   autorun=kFALSE;
 }
else
 { // set up arguments for batch mode
   runningMode = kBatch;
   strcpy(source,".x ");      // for macro name
        if(strstr(argv[1],"-f")){strcat(source,"file.C");strcat(serv,filetype);}
   else if(strstr(argv[1],"-t")) strcat(source,"trans.C");
   else if(strstr(argv[1],"-s")) strcat(source,"stream.C");
   else if(strstr(argv[1],"-e")) strcat(source,"evserv.C");
   else if(strstr(argv[1],"-ra")) strcat(source,"random.C");
   else if(strstr(argv[1],"-r")) strcat(source,"revserv.C");
   else {
       cout << "invalid input source: " << argv[1] << endl;
       usage(); // print usage
       exit(0);
     }
   iarg=3;
   if(argc > iarg){
          if(strstr(argv[iarg],"-s"))servermode=kTRUE;
     else if(strstr(argv[iarg],"-p")){iarg++; sport=atoi(argv[iarg]);}
     else maxevents=atoi(argv[iarg]);
     iarg++;
   }
   if(argc > iarg){
          if(strstr(argv[iarg],"-p")){iarg++; sport=atoi(argv[iarg]);}
     else maxevents=atoi(argv[iarg]);
     iarg++;
   }
   if(argc > iarg) maxevents=atoi(argv[iarg]);
   if(maxevents == -1) maxevents = 99999999;
   cout << endl << "**** Input " << serv << " (" << argv[1] << ") ";
   if(strstr(argv[1],"-r")) cout << "port " << sport;
   cout << endl << "     process " << maxevents << " events" << endl;
   cout << "     auto save file: " << asf << ".root" << endl << endl;
   if(strstr(source,"revserv")) snprintf(source,127,".x revserv.C(\"%s\",%d)",serv,sport);
   else {strcat(source,"(\"");  strcat(source,serv); strcat(source,"\")");}
   if(servermode){
     autorun=kTRUE;
     runningMode = kGUI;
     cout << "     Analysis running in server mode. GUIs may connect!" << endl;
   }
 }

//------ setup the analysis -------------------------

// We will use only one analysis step (factory)
// we use only standard components
  TGo4Analysis*     analysis = TGo4Analysis::Instance();
  TGo4StepFactory*  factory  = new TGo4StepFactory("Factory");
  TGo4AnalysisStep* step     = new TGo4AnalysisStep("Analysis",factory,0,0,0);
  analysis->AddAnalysisStep(step);

// tell the factory the names of processor and output event
// both will be created by the framework later
// Input event is by default an MBS event
  factory->DefEventProcessor("XXXProc","TXXXProc");// object name, class name
  factory->DefOutputEvent("XXXEvent","TGo4EventElement"); // object name, class name

// use macros to set event input and setup
  gROOT->ProcessLine(source);
  snprintf(source,127,".x setup.C(\"%s\")",asf);
  gROOT->ProcessLine(source);

if(servermode)
{
   //==================== password settings for gui login (for analysis server only)
   analysis->SetAdministratorPassword("XXXadmin");
   analysis->SetControllerPassword("XXXctrl");
   analysis->SetObserverPassword("XXXview");
   // note: do not change go4 default passwords for analysis in client mode
   // autoconnect to gui server will not work then!!!
}
//------ start the analysis -------------------------

 if(runningMode == kBatch)
   {
     cout << "**** Main: starting analysis in batch mode ...  " << endl;
     analysis->SetAutoSave(kTRUE);   // optional enable auto-save
     if (analysis->InitEventClasses() )
       {
    analysis->RunImplicitLoop(maxevents);
    delete analysis;
    cout << "**** Main: Done!"<<endl;
    gApplication->Terminate();
       }
     else
       {
    cout << "**** Main: Init event classes failed, aborting!"<<endl;
    gApplication->Terminate();
       }
   }
 if(runningMode == kGUI)
   {
     if(servermode)  cout << "**** Main: starting analysis in server mode ..." << endl;
     else            cout << "**** Main: starting analysis in slave mode ..." << endl;
     // to start histogram server: kTRUE,"base","password"
     client = new TGo4AnalysisClient(serv, analysis, hostname, iport, kFALSE, "", "", servermode, autorun);
     cout << "**** Main: created AnalysisClient Instance: "<<client->GetName()<<endl;
   }
//=================  start root application loop ==========================
cout << "**** Main: Run application loop" << endl;
theApp.Run();
return 0;
}

void usage()
{
cout << endl;
cout << "* GO4  online analysis    " << endl;
cout << "* H. Essel, GSI, Darmstadt" << endl;
cout << "* calling:                " << endl;
cout << "* MainUserAnalysis -file|-trans|-stream|-evserv|-revserv|-random input [-server] [-port #] [events]" <<endl;
cout << endl;
}
