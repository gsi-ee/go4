// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/**
*   MainGo4EventServerExample
*      Example of usage for the Go4 Mbs Event interface
*--------------------------------------------------------------
* authors: Joern Adamczewski, Marc Hemberger, Hans G. Essel
*                                                      1999-2001
*
*/

#include <iostream>

#include "TApplication.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TBenchmark.h"

#include "TGo4Log.h"
#include "Go4EventServer.h"
#include "TGo4SimpleEvent.h"
#include "TGo4SimpleSubEvent.h"
#include "TGo4SimpleEventProcessor.h"

int main(int argc, char **argv)
{
     if(argc<2) {
        std::cout << "usage: MainGo4EventServerExample eventnumber"<<std::endl;
        return 0;
     }

     TApplication theApp("App", &argc, argv);

     // the following statements control go4 debug output:
     TGo4Log::Instance(); // init logger object
     TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                           // set this to 2 to get warnings and errors only
                           // set this to 3 to get errors only
     TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file

     TBenchmark mybench;
     const char *num = argv[1];
     Int_t maxevents = atoi(num);
     Int_t value = 0;

     TCanvas *can = new TCanvas("MyCanvas","MainGo4EventServer");
     can->Divide(1,2);
     TH1D* histo1 = new TH1D ("histogram 1", "mbsevent data",400,0,4000);
     TH1D* histo2 = new TH1D ("histogram 2", "simpleevent data",400,0,4000);

     TGo4MbsSubEvent *sub = nullptr;
     TGo4SimpleSubEvent* simpsub = nullptr;

     // create the event structure objects:
     // raw mbs event
     Short_t idfield[2] = {0,3}; // this array defines the procids for the subevents
                                // note: if other procids are found during read,
                                // subevents of theses procids will be added dynamically
     TGo4MbsEvent *event = new TGo4MbsEvent(2, idfield, 1);
                                // create initial mbs event with 2 subevents
                                // having the procids giving in the idfield (0 and 3)
                                // initial size for each subevent data field is 1 longword
                                // initial size is reallocated dynamically
     TGo4SimpleEvent* simpevent = new TGo4SimpleEvent(2);
                                // user event with 2 subevents
     TGo4SimpleEventProcessor* proc = new TGo4SimpleEventProcessor();
                                // the "unpack processor" converting mbs events to user events
     TGo4EventSource *input = nullptr;
     //  TGo4EventSource *simpinput = nullptr;
     TGo4EventStore *output = nullptr;
     TGo4EventStore *simpoutput = nullptr;

     // Create instance of TGo4EventSource which specifies
     // the input. Please change subclass of TGo4EventSource
     // to change input type.
/////////// MBS LISTMODE FILE /////////////////////////////////////////////
     TString testfilename = TString::Format("%s/data/test.lmd",getenv("GO4SYS")); // this file is part of go4 distribution
     input = new TGo4MbsFile(testfilename.Data());
//         input= new TGo4MbsFile("dat0.lmd");
        // for listmode file with given path and name
        // file will be opened in constructor and closed in destructor
/////////// MBS TRANSPORT ////////////////////////////////////////////////
     //         input= new TGo4MbsTransport("r2f-2");
        // connect to mbs transport channel, with given hostname
        // constructor connects to transport, dtor disconnects
/////////// MBS STREAM ////////////////////////////////////////////////////
//         input= new TGo4MbsStream("r2f-2");
        // connect to mbs stream channel, with given hostname
        // constructor connects to stream, dtor disconnects
/////////// MBS EVENTSERVER ////////////////////////////////////////////////
//         input= new TGo4MbsEventServer("r2f-2");
        // connect to mbs event server channel, with given hostname
        // constructor connects to event server, dtor disconnects
/////////// REMOTE EVENT SERVER /// ////////////////////////////////////////
//         input= new TGo4RevServ("r2f-2");
        // connect to mbs remote event server, with given hostname
        // constructor connects to revserv, dtor disconnects

     // optional: output of the mbs raw event classes into ROOT file
     output = new TGo4FileStore("MbsEvents",99,5);

     // optional: input of the mbs raw events from ROOT file
     //simpinput= new TGo4FileSource("MbsEvents");

     // output of user events into ROOT file
     simpoutput = new TGo4FileStore("SimpleEvents",99,5);

///////////////////// Printout initial values
     event->PrintEvent();
     simpevent->PrintEvent();

///////////////// Initialize the chain of analysis
     event->SetEventSource(input); // tell raw event to be filled from mbs source
//        event->SetEventSource(simpinput); // tell raw event to be read from root file

     proc->SetInputEvent(event); // tell "unpack processor" to take mbs event as input
     simpevent->SetEventSource(proc); // tell user event to be filled by unpack processor


     std::cout << "starting event loop:"<< std::endl;
     mybench.Start("Go4EventServer");
     for(Int_t t = 0; t < maxevents; ++t)
        {
//////// get raw event from mbs source
           event->Clear();
           Int_t errmess = event->Fill(); // fills raw event from mbs source
           if(errmess==GETEVT__NOMORE)
              {
                 std::cout << "no more events from MbsSource."<< std::endl;
                 break;
                 //gApplication->Terminate();
              }
           else if(errmess != 0)
              {
                 std::cout << "ERROR on event fill."<< std::endl;
                 break;
              }
//////// access to a certain mbs raw subevent data:
           sub=event->GetSubEvent(3); // subevent by procid
           if (sub) {
              value = sub->Data(1); // access data array at 1
              if (value) {
                 histo1->Fill(value);
              }
           }

           if(output)
             output->Store(event);   // write mbs raw event to ROOT file

//////// Fill user event from raw event
           simpevent->Clear();
           simpevent->Fill(); // this will invoke unpack processor methods
/////// access to certain user subevent data:
           simpsub=simpevent->GetSubEvent(3);
           if (simpsub)
              {
                 value= simpsub->fiD1;  // userevents have data as members,
                                        // may also be seen in ROOT TreeBrowser
                 if(value) {
                     histo2->Fill(value);
                 }
              }

           if(simpoutput)
             simpoutput->Store(simpevent); // write user event to ROOT file


           if(t%2000 == 0 && t != 0)  {
                 event->PrintEvent();
                 can->cd(1);
                 histo1->Draw();
                 can->cd(2);
                 histo2->Draw();
                 can->Modified();
                 can->Update();
                 std::cout << " 2000 events processed "<< std::endl;
              }

        } // for(.....)
     std::cout << "\t finished filling and saving events."<<std::endl;
     mybench.Show("Go4EventServer");
     delete output;
     delete event;
     delete input;
     delete simpoutput;
     delete simpevent;
     delete proc;
     //gApplication->Terminate();
     theApp.Run();
     return 0;
}
