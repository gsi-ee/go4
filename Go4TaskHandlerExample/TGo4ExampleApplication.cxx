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

#include "TGo4ExampleApplication.h"

#include "Riostream.h"
#include "TRandom.h"

#include "TGo4LockGuard.h"
#include "TGo4Log.h"
#include "TGo4CommandInvoker.h"

TGo4ExampleApplication::TGo4ExampleApplication( TGo4BufferQueue* datq)
: TNamed("My example application","Go4 Taskhandler example")
   {
   TRACE((15,"TGo4ExampleApplication::TGo4ExampleApplication() constructor",__LINE__, __FILE__));
   fxDataQ=datq;
   fxDemoHistogram = new TH1D("Test Histogram", "Client Data", 2048, 0, 2047);

   TGo4CommandInvoker::Instance(); // make sure we have an invoker instance!
   TGo4CommandInvoker::Register("ExampleApplication",this); // register as command receiver at the global invoker
}

TGo4ExampleApplication::~TGo4ExampleApplication()
{
   TRACE((15,"TGo4ExampleApplication::TGo4ExampleApplication() destructor",__LINE__, __FILE__));
   delete fxDemoHistogram;
   TGo4CommandInvoker::UnRegister(this);
}

void TGo4ExampleApplication::SendData(TNamed* data)
{
   TRACE((12,"TGo4ExampleApplication::SendData(TNamed*)",__LINE__, __FILE__));
   if(fxDataQ)
      {
         std::cout << "Example Application: Sending data into queue..."<<std::endl;
         fxDataQ->AddBufferFromObject(data);
      }
   else
      {
         std::cout << "Example Application: Sorry, no data queue present"<<std::endl;
         // no data queue present
      }

}
TH1* TGo4ExampleApplication::GetHistogram()
{
   TRACE((12,"TGo4ExampleApplication::GetHistogram",__LINE__, __FILE__));
   return fxDemoHistogram;
}
void TGo4ExampleApplication::FillHistograms()
{
   TRACE((12,"TGo4ExampleApplication::FillHistograms",__LINE__, __FILE__));
   TGo4LockGuard mainlock; // protect against socket streaming
   static Int_t counter=0;
   counter++;
   Double_t peak1= gRandom->Gaus(550,42);
   Double_t peak2= gRandom->Gaus(1200,230);
   //cout << "Example Application random numbers: "<< peak1<<" "<< peak2 <<endl;
   fxDemoHistogram->Fill((Axis_t) peak1);
   fxDemoHistogram->Fill((Axis_t) peak2);
   if(!(counter%100))
      std::cout << "Example Application filled histogram 100 times" <<std::endl;

}
void TGo4ExampleApplication::CommandAction2()
{
   TRACE((14,"TGo4ExampleApplication::CommandAction2",__LINE__, __FILE__));
   std::cout << "Example Application: executing Action 2"<<std::endl;

}
void TGo4ExampleApplication::CommandAction1()
{
   TRACE((14,"TGo4ExampleApplication::CommandAction1",__LINE__, __FILE__));
   std::cout << "Example Application: executing Action 1"<<std::endl;
   SendData(GetHistogram()); // later we get histogram by specified name
}
