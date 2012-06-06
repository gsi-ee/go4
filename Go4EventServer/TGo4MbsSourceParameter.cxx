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

#include "TGo4MbsSourceParameter.h"

#include "go4iostream.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4MbsSourceParameter::TGo4MbsSourceParameter() :
    TGo4EventSourceParameter("MbsSource", 0),
    fuStartEvent(0),
    fuStopEvent(0),
    fuEventInterval(0),
    fiPort(0),
    fiRetryCnt(0)
{
   TRACE((14,"TGo4MbsSourceParameter::TGo4MbsSourceParameter()", __LINE__, __FILE__));
}

TGo4MbsSourceParameter::TGo4MbsSourceParameter(const char* name, Int_t id) :
   TGo4EventSourceParameter(name, id),
   fuStartEvent(0),
   fuStopEvent(0),
   fuEventInterval(0),
   fiPort(0),
   fiRetryCnt(0)
{
   TRACE((14,"TGo4MbsSourceParameter::TGo4MbsSourceParameter(const char*,...)", __LINE__, __FILE__));
}

TGo4MbsSourceParameter::~TGo4MbsSourceParameter()
{
   TRACE((14,"TGo4MbsSourceParameter::~TGo4MbsSourceParameter()", __LINE__, __FILE__));
}

Int_t TGo4MbsSourceParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4MbsSourceParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;

   Int_t delta=TGo4EventSourceParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+= delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Startevent: \t%d",GetStartEvent());
   current=TGo4Status::PrintBuffer(current,restlen, "\t Stopevent: \t%d \n",GetStopEvent());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Event interval: \t%d\n",GetEventInterval());
   if (GetPort()>0)
      current=TGo4Status::PrintBuffer(current,restlen, "  Server port: \t%d \n",GetPort());
   if (GetRetryCnt()>0)
      current=TGo4Status::PrintBuffer(current,restlen, "  Retry cnt: \t%d \n",GetRetryCnt());

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

Bool_t TGo4MbsSourceParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4MbsSourceParameter::UpdateFrom()",__LINE__, __FILE__));
   if((rhs!=0) && rhs->InheritsFrom(TGo4MbsSourceParameter::Class())) {
      TGo4MbsSourceParameter* mbspar=dynamic_cast<TGo4MbsSourceParameter*>(rhs);
      if(!mbspar) return kFALSE;
      if(!TGo4EventSourceParameter::UpdateFrom(rhs)) return kFALSE;
      SetStartEvent(mbspar->GetStartEvent());
      SetStopEvent(mbspar->GetStopEvent());
      SetEventInterval(mbspar->GetEventInterval());
      SetPort(mbspar->GetPort());
      SetRetryCnt(mbspar->GetRetryCnt());
      return kTRUE;
   }
   return kFALSE;
}
