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

#include "TGo4HisConnectorRunnable.h"

#include "Riostream.h"

#include "TH1.h"
#include "TGraph.h"

#include "TGo4LockGuard.h"

#include "TGo4Task.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4HistogramServer.h"
#include "TGo4MbsHist.h"

extern "C"
{
#include "f_his_hist.h"
}

TGo4HisConnectorRunnable::TGo4HisConnectorRunnable(const char* name, TGo4HistogramServer* hserv)
: TGo4Runnable(name, hserv->GetAnalysisClient()->GetTask())
{
fxHistogramServer=hserv;
}

TGo4HisConnectorRunnable::TGo4HisConnectorRunnable(): TGo4Runnable(0,0)
{
}

TGo4HisConnectorRunnable::~TGo4HisConnectorRunnable()
{
}

Int_t TGo4HisConnectorRunnable::Run(void*)
{
   // wait for client request on server
   // note: we use the __gsi histogram api__ here
   // instead of the first approach that intended to launch
   // one thread per client which is up permanently until client disconnects.
   Bool_t histofromgraph=kFALSE;
   Int_t action=0;
   char histo[128];
   TH1* his=0;
   TGo4MbsHist* mbshisto=0;
   TGo4Analysis* ana= fxHistogramServer->GetAnalysis();
   Int_t result=f_his_wait(&action,histo);
   //if(! GetThread()->IsRunning()) return 1; // fast stop in case of shutdown
   if(result==COMM__SUCCESS)
   {
      switch(action)
      {
         case COMM__GETHIST:
            if(!strcmp(histo,TGo4HistogramServer::fgcSHUTDOWNNAME))
            {
               // this is the last connect for shutdown, we do not acquire lockguard!
               mbshisto= 0;
            }
            else
            {
               // normal case: lockguard to protect creation of histogram structure
               TGo4LockGuard mainlock;
               his= ana->GetHistogram(histo);
               if(his==0)
               {
                  TObject* ob=ana->GetObject(histo);
                  if(ob && ob->InheritsFrom(TH1::Class()))
                  {
                     his=dynamic_cast<TH1*>(ob);
                  }
                  else if (ob && ob->InheritsFrom(TGraph::Class()))
                  {
                     TGraph* gr=dynamic_cast<TGraph*>(ob);
                     his=gr->GetHistogram();
                     if(his)
                     {
                        // this histogram has correct scales,
                        // but does not have graph points. we fill it:
                        Int_t maxpoints=gr->GetN();
                        for(Int_t point=0; point<maxpoints; ++point)
                        {
                           Double_t xg=0;
                           Double_t yg=0;
                           gr->GetPoint(point,xg,yg);
                           Int_t xbin=his->FindBin(xg);
                           his->SetBinContent(xbin,yg);
                           histofromgraph=kTRUE;
                           // note that this will fail if we have graph that
                           // is not injective (i.e. 1d-function). a histogram is not a graph!
                        }
                     }
                  }

               }

               mbshisto= new TGo4MbsHist(his); // this will create structures
               if(his && histofromgraph) his->Reset();
            }// if(!strcmp(histo,TGo4HistogramServer::fgcSHUTDOWNNAME))
            const char* hisname;
            if(his)
            {
               hisname=his->GetName();
               ana->Message(0,
                     "Histogram server is sending histogram %s",
                     hisname);
               //cout <<"Histogram Server is sending histogram "<< hisname <<endl;
            }
            else
            {
               hisname="No such histogram";
            }
            if(mbshisto)
            {
               // the normal case: send created histogram structure
               result=f_his_sendhis(mbshisto->GetHead(),
                     mbshisto->GetHisNum(),
                     (CHARS*) hisname,
                     (INTS4*) mbshisto->GetBuffer());
               delete mbshisto;
            }
            else
            {
               // only in case of server shutdown: send dummy to release socket wait
               s_his_head header;
               INTS4 buffer[128];
               result=f_his_sendhis(&header, 1, (CHARS*) hisname, (INTS4*) &buffer);
            }
            break;

         case COMM__GETDIR:
         {
            TGo4LockGuard mainlock; // this should lock also dirmutex
            mbshisto=new TGo4MbsHist(ana->GetObjectFolder(), histo);
            result=f_his_senddir((s_his_head*) (mbshisto->GetBuffer()), mbshisto->GetHisNum());
            //               cout <<"sending dir buffer :" << endl;
            //               cout <<"\thisnum="<<mbshisto->GetHisNum()<<", buflen="<<mbshisto->GetBufLen() << endl;
            delete mbshisto;
         }
         break;
         default:
            ana->Message(0,"Histogram server: got unknown command");
            //cout <<"Histogram Server: got unknown command" << endl;
            break;
      } // switch
   }
   else
   {
      cout <<"Histogram Server: Error on connect request: "<< result << endl;
   } // if(result==COMM__SUCCESS)

return result;
}
