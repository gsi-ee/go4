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

#include "TGo4AnalysisStepStatus.h"

#include "Riostream.h"

#include "TROOT.h"

#include "TGo4Log.h"

TGo4AnalysisStepStatus::TGo4AnalysisStepStatus() :
   TGo4Status(),
   fxSourceType(0),
   fxStoreType(0),
   fxProcessorType(0),
   fbSourceEnabled(kFALSE),
   fbStoreEnabled(kFALSE),
   fbProcessEnabled(kFALSE),
   fbErrorStopEnabled(kFALSE),
   fbErrorStopped(kFALSE),
   fiProcessStatus(0)
{
}

TGo4AnalysisStepStatus::TGo4AnalysisStepStatus(const char* name) :
   TGo4Status(name, "Go4 AnalysisStep Status Object"),
   fxSourceType(0),
   fxStoreType(0),
   fxProcessorType(0),
   fbSourceEnabled(kFALSE),
   fbStoreEnabled(kFALSE),
   fbProcessEnabled(kFALSE),
   fbErrorStopEnabled(kFALSE),
   fbErrorStopped(kFALSE),
   fiProcessStatus(0)
{
   TRACE((15,"TGo4AnalysisStepStatus::TGo4AnalysisStepStatus(const char*)",__LINE__, __FILE__));

}

TGo4AnalysisStepStatus::~TGo4AnalysisStepStatus()
{
   TRACE((15,"TGo4AnalysisStepStatus::~TGo4AnalysisStepStatus()",__LINE__, __FILE__));
   delete fxSourceType;
   delete fxStoreType;
   delete fxProcessorType;
}

Int_t TGo4AnalysisStepStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4AnalysisStepStatus::PrintStatus()",__LINE__, __FILE__));
   //

   Int_t locallen=64000;
   Text_t localbuf[64000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;

   Text_t* current=localbuf;
   Int_t restlen=locallen;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "++++ Analysis Step %s ++++\t \n",GetName());
   TROOT::IncreaseDirLevel();
   if(IsProcessEnabled())
      {
      if(IsSourceEnabled())
         {
         TGo4EventSourceParameter* spar=GetSourcePar();
         if( spar!=0 )
            {
               Int_t delta=spar->PrintParameter(current,restlen);
               restlen-=delta;
               current+=delta;
            }
         else
            {
               current=PrintIndent(current,restlen);
               current=PrintBuffer(current,restlen, "EventSource Type: undefined \n");
            } // if( spar!=0 )
         }
      else
         {
            current=PrintIndent(current,restlen);
            current=PrintBuffer(current,restlen, "EventSource is disabled. \n");
         }

      TGo4EventProcessorParameter* ppar=GetProcessorPar();
      if(ppar !=0)
         {
            Int_t delta=ppar->PrintParameter(current,restlen);
            restlen-=delta;
            current+=delta;
         }
      else
         {
            // for the moment, suppress unnecessary confusing output!
            //  current=PrintIndent(current,restlen);
            //  current=PrintBuffer(current,restlen, "EventProcessor Type: undefined \n");
         } // if( ppar!=0 )


      if(IsStoreEnabled())
         {
            TGo4EventStoreParameter* tpar=GetStorePar();
            if(tpar!=0)
               {
                  Int_t delta=tpar->PrintParameter(current,restlen);
                  restlen-=delta;
                  current+=delta;
               }
            else
               {
                  current=PrintIndent(current,restlen);
                  current=PrintBuffer(current,restlen, "EventStore Type: undefined \n");
               } // if( tpar!=0 )
         }
      else
         {
            current=PrintIndent(current,restlen);
            current=PrintBuffer(current,restlen, "EventStore is disabled.\n");
         }
     //   current=PrintIndent(current,restlen);
     //   current=PrintBuffer(current,restlen, "Error stop enabled: %d\n",IsErrorStopEnabled());
     //   current=PrintIndent(current,restlen);
     //   current=PrintBuffer(current,restlen, "Error stopped: %d\n",IsErrorStopped());
      current=PrintIndent(current,restlen);
      current=PrintBuffer(current,restlen, "Process Status: %d\n",GetProcessStatus());
      }
   else
      {
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, "Step is disabled.\n");
      }
   TROOT::DecreaseDirLevel();
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "----------------------------------------------  \n");
   if(buffer==0)
      {
          cout << localbuf << endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

void TGo4AnalysisStepStatus::SetSourcePar(TGo4EventSourceParameter* kind)
{
    if(fxSourceType) delete fxSourceType;
    if(kind)
        fxSourceType=dynamic_cast<TGo4EventSourceParameter*>(kind->Clone());
    else
        fxSourceType=0;
}

void TGo4AnalysisStepStatus::SetStorePar(TGo4EventStoreParameter* kind)
{
    if(fxStoreType) delete fxStoreType;
    if(kind)
        fxStoreType=dynamic_cast<TGo4EventStoreParameter*>(kind->Clone());
    else
        fxStoreType=0;
}

void TGo4AnalysisStepStatus::SetProcessorPar(TGo4EventProcessorParameter* kind)
{
    if(fxProcessorType) delete fxProcessorType;
    if(kind)
        fxProcessorType=dynamic_cast<TGo4EventProcessorParameter*>(kind->Clone());
    else
        fxProcessorType=0;
}
