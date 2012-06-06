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

#include "TGo4TestThreadManager.h"

#include "go4iostream.h"

#include "TGo4Log.h"
#include "TGo4ThreadHandler.h"
#include "TGo4TestRunnable.h"

TGo4TestThreadManager::TGo4TestThreadManager(const TGo4TestThreadManager &right)
   :TGo4ThreadManager(right)
{
  TRACE((15,"TGo4TestThreadManager::TGo4TestThreadManager copy ctor",__LINE__, __FILE__));
}

TGo4TestThreadManager::TGo4TestThreadManager (const char* name)
   :TGo4ThreadManager(name,kFALSE)
{
   TRACE((15,"TGo4TestThreadManager::TGo4TestThreadManager (const char* name) constructor",__LINE__, __FILE__));
   fxControlRunnable=new TGo4TestRunnable("ControlRunnable",this,1);
   TGo4TestRunnable* th1run= new TGo4TestRunnable("HistogramMaker1",this,4);
   TGo4TestRunnable* th2run= new TGo4TestRunnable("HistogramMaker2",this,4);
   //TGo4TestRunnable* th3run= new TGo4TestRunnable("dummy action 3",this,0);
   TGo4TestRunnable* th4run= new TGo4TestRunnable("Exceptiontester",this,6);
   TGo4TestRunnable* th5run= new TGo4TestRunnable("Service thread",this,0);

   fxWorkHandler->NewThread("First Thread",th1run);
   fxWorkHandler->NewThread("Second Thread",th2run);
   fxWorkHandler->NewThread("Third Thread",th5run);
   fxWorkHandler->NewThread("Fourth Thread",th4run);
   fxWorkHandler->NewThread("Control",fxControlRunnable);
   cout << "thread manager: all threads added to handler"<<endl;

   fxCanvas=new TCanvas(GetName(),"TestRunnable",1200,400);
   fxPad1= new TPad("Pad1","TestRunnablePad",0.02,0.02,0.48,0.83,33);
   fxPad2= new TPad("Pad2","TestRunnablePad",0.52,0.02,0.98,0.83,33);
   fxCanvas->cd();
   cout <<"canvas drawn"<<endl;
   fxPad1->Draw();
   fxCanvas->cd();
   fxPad2->Draw();
   cout <<"pads drawn"<<endl;
   fxCanvas->Modified();
   fxCanvas->Update();

   Launch();
}

TGo4TestThreadManager::~TGo4TestThreadManager()
{
   TRACE((15,"TGo4TestThreadManager::~TGo4TestThreadManager destructor",__LINE__, __FILE__));
   delete fxPad1;
   delete fxPad2;
   delete fxCanvas;
}

Int_t TGo4TestThreadManager::TestAction ()
{
  TRACE((14,"TGo4TestThreadManager::TestAction",__LINE__, __FILE__));
  // no test action so far...
  return 0;
}

TCanvas* TGo4TestThreadManager::GetCanvas ()
{
   TRACE((12,"TGo4TestThreadManager::GetCanvas",__LINE__, __FILE__));
   return fxCanvas;
}

TPad* TGo4TestThreadManager::GetPad1 ()
{
   TRACE((12,"TGo4TestThreadManager::GetPad1",__LINE__, __FILE__));
   return fxPad1;
}

TPad* TGo4TestThreadManager::GetPad2 ()
{
   TRACE((12,"TGo4TestThreadManager::GetPad2",__LINE__, __FILE__));
   return fxPad2;
}
