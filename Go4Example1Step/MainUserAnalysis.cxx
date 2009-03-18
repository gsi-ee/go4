#include "Riostream.h"
#include <string.h>
#include <stdlib.h>

#include "TROOT.h"
#include "TApplication.h"

#include "Go4EventServerTypes.h"
#include "TXXXAnalysis.h"
#include "TGo4AnalysisClient.h"

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
//         if not GUI (batch mode) the args are (designed by user):
// argv[1] -file|-trans|-stream|-evserv|-revserv
// argv[2] input name (file or server)
// argv[3] optional -p portnumber
// argv[4] optional -output or number of events
// argv[5] optional number of events

Int_t  runningMode;      // kGUI or kBatch
Int_t  maxevents = -1;   // number of events (batch mode)
Int_t  intype=GO4EV_NULL;// type of source
Bool_t writeout=kFALSE;  // write output file
UInt_t iport;            // port number used by GUI
UInt_t sport=6003;       // remote event server port
Int_t  iarg;             // argument index
Text_t serv[128];        // input name
Text_t out[128];         // output root events
Text_t asf[128];         // auto save file (batch)
Text_t filetype[8];      // file type .lmd or .lml
Text_t *pc,*tmpname,*outname;

// strip any path information from input:
// outname is name without path
 strcpy(filetype,".lmd");
 tmpname=argv[2]+strlen(argv[2])-4;
 if((pc=strstr(tmpname,".lmd")) != 0)*pc=0;
 if((pc=strstr(tmpname,".lml")) != 0){*pc=0;strcpy(filetype,".lml");}
 tmpname=argv[2];
 if((pc=strstr(argv[2],"@")) != 0) tmpname++;
 outname=tmpname;
 if((tmpname=strrchr(outname,'/')) != 0) outname=tmpname+1;
 strncpy(out,outname,110);     // output root file
 strcat(out,"_XXXEvent"); // append name of output event object
 strncpy(asf,outname,110);     // auto save file
 strcat(asf,"_AS");

if(strcmp(argv[1],"-gui") == 0)
{
// set up arguments for GUI mode
   runningMode = kGUI;
   iport = (argc>4) ? atoi(argv[4]) : 5000; // port of GUI server
   intype=GO4EV_MBS_FILE;
   writeout=kFALSE;        // no output
   strcpy(serv,"/GSI/lea/gauss"); // default input file
   strcpy(out,"gauss_XXXEvent");
 }
 else
// set up arguments for batch mode
 {
   runningMode = kBatch;
   strncpy(serv,argv[2],110);     // input (file with full path)
        if(strstr(argv[1],"-f")){intype=GO4EV_MBS_FILE;strcat(serv,filetype);}
   else if(strstr(argv[1],"-t")) intype=GO4EV_MBS_TRANSPORT;
   else if(strstr(argv[1],"-s")) intype=GO4EV_MBS_STREAM;
   else if(strstr(argv[1],"-e")) intype=GO4EV_MBS_EVENTSERVER;
   else if(strstr(argv[1],"-r")) intype=GO4EV_MBS_REVSERV;
   else {
       cout << "invalid input type: " << argv[1] << endl;
       usage(); // print usage
       exit(0);
     }
   iarg=3;
   if(argc > iarg){
     if(strstr(argv[iarg],"-o"))writeout=kTRUE;
     else if(strstr(argv[iarg],"-p")){iarg++; sport=atoi(argv[iarg]);}
     else maxevents=atoi(argv[iarg]);
     iarg++;
   }
   if(argc > iarg){
     if(strstr(argv[iarg],"-o"))writeout=kTRUE;
     else maxevents=atoi(argv[iarg]);
     iarg++;
   }
   if(argc > iarg)maxevents=atoi(argv[iarg]);
   if(maxevents == -1) maxevents = 99999999;
   cout << endl << "**** Input " << serv << " (" << argv[1] << ") ";
   if(strstr(argv[1],"-r")) cout << "port " << sport;
   cout << endl << "     output " << out << ".root";
   if(writeout) cout << " enabled"; else cout << " disabled";
   cout << endl << "     process " << maxevents << " events" << endl;
   cout << "     auto save file: " << asf << ".root" << endl << endl;
 }
// Now setup the  analysis itself
// arguments could be adjusted for other needs

TXXXAnalysis* analysis = new TXXXAnalysis(serv,intype,sport,out,writeout);
analysis->SetAutoSaveFile(asf);   // optional
analysis->SetAutoSaveInterval(0); // after n seconds , 0 = at termination of event loop
analysis->SetAutoSave(kFALSE);    // optional

// ===================== End analysis setup ================================

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
     cout << "**** Main: starting analysis in client mode ..." << endl;
     // to start histogram server: kTRUE,"base","password"
     TGo4AnalysisClient* client = new TGo4AnalysisClient(argc,argv,analysis,kFALSE,"","");
     cout << "**** Main created AnalysisClient Instance: "<<client->GetName()<<endl;
   }
//=================  start root application loop ==========================
cout << "Run the application loop" << endl;
theApp.Run();
return 0;
}

void usage()
{
cout << endl;
cout << "* GO4  online analysis    " << endl;
cout << "* H. Essel, GSI, Darmstadt" << endl;
cout << "* calling:                " << endl;
cout << "* MainUserAnalysis -file|-trans|-stream|-evserv|-revserv input [-port #] [-output] [events]" <<endl;
cout << endl;
}
