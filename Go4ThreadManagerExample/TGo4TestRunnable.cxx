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

#include "TApplication.h"
#include "TH1.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4TestRunnable.h"

#include "TGo4TerminateException.h"
#include "TGo4RestartException.h"
#include "TGo4LogicException.h"

TGo4TestRunnable::TGo4TestRunnable()
:TGo4Runnable("dummy",0),fxPad(0),fxCanvas(0),fxHisto(0)
{
  GO4TRACE((14,"TGo4TestRunnable::TGo4Runnable() constructor",__LINE__, __FILE__));
}

TGo4TestRunnable::TGo4TestRunnable(const TGo4TestRunnable &right)
   :TGo4Runnable(right),fxPad(0),fxCanvas(0),fxHisto(0)
{
   GO4TRACE((14,"TGo4TestRunnable::TGo4TestRunnable() copy ctor",__LINE__, __FILE__));
   fiMode=right.fiMode;
}

TGo4TestRunnable::TGo4TestRunnable(const char* name, TGo4ThreadManager* man, Int_t mode)
  :TGo4Runnable(name,man),fxPad(0),fxCanvas(0),fxHisto(0)
{
  GO4TRACE((14,"TGo4TestRunnable::TGo4TestRunnable(const char*,TGo4ThreadManager*,Int_t) constructor",__LINE__, __FILE__));
  fiMode=mode;
}


TGo4TestRunnable::~TGo4TestRunnable()
{
   GO4TRACE((14,"TGo4TestRunnable::~TGo4TestRunnable() destructor",__LINE__, __FILE__));
}

Int_t TGo4TestRunnable::Run (void* ptr)
{
   GO4TRACE((12,"TGo4TestRunnable::Run()",__LINE__, __FILE__));

   Int_t i=0;
   static Int_t t=0;
   static Int_t loops=0;
   Axis_t x=0;
   TGo4ThreadHandler* han=0;
   TGo4TestRunnable* th1run=0;
   TGo4Thread* go4thr=0;
   TCanvas* can=((TGo4TestThreadManager*)fxManager)->GetCanvas();
   TPad* pad1=((TGo4TestThreadManager*)fxManager)->GetPad1();
   TPad* pad2=((TGo4TestThreadManager*)fxManager)->GetPad2();

  switch(fiMode)
   {
      case 0:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 0",__LINE__, __FILE__));
         TGo4Log::Debug(" TestRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(5000);
         }
         break;
      case 1:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 1",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(30000);
         TGo4Log::Debug("\t ''%s'' killing all other Threads",
                  GetName());
         han= (TGo4ThreadHandler*) fxManager->GetWorkHandler();
         han->Cancel("First Thread");
         han->Cancel("Second Thread");
         han->Cancel("Third Thread");
         han->Cancel("Fourth Thread");
         fxGo4Thread->Sleep(5000);
         TGo4Log::Debug("\t ''%s'' re-starting all other Threads", GetName());
         han->Create("First Thread");
         han->Create("Second Thread");
         han->Create("Third Thread");
         han->Create("Fourth Thread");
         }
         break;
      case 2:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 2",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(10000);
         TGo4Log::Debug("\t ''%s'' stopping all other Workfunctions:", GetName());
         han=(TGo4ThreadHandler*) fxManager->GetWorkHandler();
         han->Stop("First Thread");
         han->Stop("Second Thread");
         han->Stop("Third Thread");
         han->Stop("Fourth Thread");
         fxGo4Thread->Sleep(10000);
         TGo4Log::Debug("\t ''%s'' re-starting all other Workfunctions:", GetName());
         han->Start("First Thread");
         han->Start("Second Thread");
         han->Start("Third Thread");
         han->Start("Fourth Thread");
         }
         break;
      case 3:
         // can we call methods of manager? aborting the whole thing?
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 3",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(30000);
         TGo4Log::Debug("\t ''%s'' terminating ThreadManager", GetName());
         //fxManager->Terminate(); // the direct approach
         throw TGo4TerminateException(this); //same, using a control exception
         }
         break;
      case 4:
         // memory alloc and histogram drawing test
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 4",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(2000);
         TGo4Log::Debug("\t ''%s'' creating histogram", GetName());
         if(fxHisto==0)
            {
               fxHisto = new TH1D(GetName(),"histogram",2048,0,100);
            }
         else
            {
               // keep old histo
            }
         while(i++<1000)
            {
               x=gRandom->Gaus(50,7);
               fxHisto->Fill(x);
            }
         fxManager->UnBlockApp();
         if(strstr(GetName(),"Maker1"))
            {
               pad1->cd();
            }
         else
            {
               pad2->cd();
            }

         fxHisto->Draw();
         can->Modified();
         can->Update();
         TThread::CancelPoint();
         fxManager->BlockApp();
         TGo4Log::Debug("\t ''%s'' histogram filled, waiting", GetName());
         fxGo4Thread->Sleep(2000);
         TThread::CancelPoint(); // necessary?
         if(strstr(GetName(),"Maker1"))
            {
               TGo4Log::Debug("\t ''%s'' deleting histogram", GetName());
               delete fxHisto;
               fxHisto=0;
            }
         else
            {
               if((++t%5)==0)
                  {
                  //throw TGo4RemoveException(this);

//                  throw TGo4ReplaceException(
//                        this,
//                        new TGo4TestRunnable("New HistogramMaker2",fxManager,4)
//                        );

                  //throw TGo4ReplaceException(this);

//                     std::cout << GetName()<<" ReCreating my thread:"<< std::endl;
//                     //fxGo4Thread->ReCreate();
//                  std::cout << GetName()<<" throwing exception now:"<< std::endl;
//                  throw TGo4RestartException(this);

//                  t=0;
                  }
            }
         }
         break;
      case 5:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 5",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(15000);
         TGo4Log::Debug("\t ''%s'' removing first thread", GetName());

         go4thr=((TGo4ThreadHandler*) fxManager->GetWorkHandler())->GetThread("First Thread");
         ((TGo4ThreadHandler*) fxManager->GetWorkHandler())->RemoveThread("First Thread");

         TString rname = TString::Format("NewHistogramMaker1:%d",loops++);
         //th1run= new TGo4TestRunnable(buf,fxManager,4);
         //sprintf(buf,"New First Thread:%d",loops++);
         if(loops==1)
            {
               // at first thread is internal
               TGo4Log::Debug("\t ''%s'' creating new first thread", GetName());
               th1run= new TGo4TestRunnable(rname.Data(),fxManager,4);
               go4thr= new TGo4Thread("First Thread",th1run,kFALSE); // first replacement: new thread
            }
         else
            {
               fxManager->UnBlockApp();
               go4thr->Cancel(); // need to cancel if not in internal mode
               fxManager->BlockApp();
            }
         TGo4Log::Debug("\t ''%s'' adding new first thread", GetName());
         ((TGo4ThreadHandler*) fxManager->GetWorkHandler())->AddThread(go4thr);
         ((TGo4ThreadHandler*) fxManager->GetWorkHandler())->Start("First Thread");
//         ((TGo4ThreadHandler*) fxManager->GetWorkHandler())->NewThread(buf,th1run);
//         ((TGo4ThreadHandler*) fxManager->GetWorkHandler())->Start(buf);


//         std::cout << GetName()<<" end of work, stopping"<< std::endl;
//         fxGo4Thread->Stop();
//

//         std::cout << GetName()<<" end of work, canceling"<< std::endl;
//         fxGo4Thread->Cancel();

//         throw TGo4CancelException(this);
         }
         break;
      case 6:
         // exception test:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 6",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         fxGo4Thread->Sleep(61000);
         TGo4Log::Debug("\t ''%s'' throwing exception now:", GetName());
         // restart of myself:
         //throw TGo4RestartException(this);
         //throw TGo4CancelException(this);

         // restart of other thread by name:
//         throw TGo4ReplaceException(
//            this,
//            new TGo4TestRunnable("New HistogramMaker2",fxManager,4),
//            "Second Thread"
//            );
//
         throw TGo4RestartException(this,"Second Thread");
         //throw TGo4RemoveException(this,"First Thread");
         //fxGo4Thread->Stop();

         // unexpected test:
         //throw 1;
         }
         break;
         case 7:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode 7",__LINE__, __FILE__));
         TGo4Log::Debug(" ControlRunnable ''%s'' running, testcase %d -- ",
                  GetName(),fiMode);
         TGo4Log::Debug("\t ''%s'' creating histogram", GetName());
         if(fxHisto==0)
            {
               fxHisto = new TH1D(GetName(),"histogram",2048,0,100);
            }
         else
            {
               // keep old histo
            }
         while(i++<1000)
            {
               x=gRandom->Gaus(50,7);
               fxHisto->Fill(x);
            }
//         pad2->cd();
//         {
//         TThread::Lock();
//            fxHisto->Draw();
//         TThread::UnLock();
//         }
         TGo4Log::Debug("\t ''%s'' histogram %d filled, waiting", GetName(),loops);
         fxGo4Thread->Sleep(5000);
         TGo4Log::Debug("\t ''%s'' deleting histogram %d", GetName(),loops++);
         delete fxHisto;
         fxHisto=0;
         }
         break;
      default:
         {
         GO4TRACE((11,"TGo4TestRunnable::Run() mode default",__LINE__, __FILE__));
         TGo4Log::Debug(" TestRunnable ''%s'' --  unspecified Workfunc action! ",
                  GetName(),fiMode);
         throw TGo4LogicException();
         }
         break;
   }
   return 0;

}

void TGo4TestRunnable::ThreadCatch (TGo4Exception& ex)
{
GO4TRACE((12,"TGo4TestRunnable::ThreadCatch()",__LINE__, __FILE__));
TGo4Runnable::ThreadCatch(ex); // we use parent method

//   std::cout  << "TGo4TestRunnable "<< GetName() << ":ThreadCatch --"
//   << std::endl<< "\t"
//   << ex.What()
//   << " occured in thread "<< fxGo4Thread->GetSelfID()
//   << "\n\t\tcalling default exception handler..."
//   << std::endl;
//   ex.Handle();
}

void TGo4TestRunnable::UnexpectedCatch ()
{
GO4TRACE((12,"TGo4TestRunnable::UnexpectedCatch()",__LINE__, __FILE__));

TGo4Runnable::UnexpectedCatch();

//   std::cout  << "TGo4TestRunnable "<< GetName() << ":UnexpectedCatch --"
//         << std::endl<< "\t"
//         << "aborting Application"<< std::endl;
//   gApplication->Terminate(0);
//
}





