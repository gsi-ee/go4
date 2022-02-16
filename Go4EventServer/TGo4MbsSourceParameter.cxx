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

#include "TGo4MbsSourceParameter.h"

#include "TGo4Status.h"

TGo4MbsSourceParameter::TGo4MbsSourceParameter() :
    TGo4EventSourceParameter("MbsSource", 0)
{
}

TGo4MbsSourceParameter::TGo4MbsSourceParameter(const char* name, Int_t id) :
   TGo4EventSourceParameter(name, id)
{
}

TGo4MbsSourceParameter::~TGo4MbsSourceParameter()
{
}

void TGo4MbsSourceParameter::Print(Option_t*) const
{
   TGo4EventSourceParameter::Print();
   TGo4Status::PrintLine("  Startevent: \t%d",GetStartEvent());
   TGo4Status::PrintLine("  Stopevent: \t%d" ,GetStopEvent());
   TGo4Status::PrintLine("  Event interval: \t%d", GetEventInterval());
   if (GetPort()>0)
      TGo4Status::PrintLine("  Server port: \t%d",GetPort());
   if (GetRetryCnt()>0)
      TGo4Status::PrintLine("  Retry cnt: \t%d",GetRetryCnt());
}

Bool_t TGo4MbsSourceParameter::UpdateFrom(TGo4Parameter* rhs)
{
   if(rhs && rhs->InheritsFrom(TGo4MbsSourceParameter::Class())) {
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
