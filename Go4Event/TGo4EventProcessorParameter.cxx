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

#include "TGo4EventProcessorParameter.h"

#include "TGo4Status.h"
#include "TGo4Log.h"


TGo4EventProcessorParameter::TGo4EventProcessorParameter(const char* name, Int_t id) :
   TGo4Parameter (name), fiID(id)
{
   GO4TRACE((12,"TGo4EventProcessorParameter::TGo4EventProcessorParameter(const char*)",__LINE__, __FILE__));
}

TGo4EventProcessorParameter::TGo4EventProcessorParameter() :
   TGo4Parameter ("Default Event Processor Parameter")
{
   GO4TRACE((12,"TGo4EventProcessorParameter::TGo4EventProcessorParameter()",__LINE__, __FILE__));
}

TGo4EventProcessorParameter::~TGo4EventProcessorParameter()
{
   GO4TRACE((12,"TGo4EventProcessorParameter::~TGo4EventProcessorParameter()",__LINE__, __FILE__));
}

void TGo4EventProcessorParameter::Print(Option_t*) const
{
   TGo4Status::PrintLine("EventProcessor: %s", ClassName());
   TGo4Status::PrintLine("  Name: \t%s", GetName());
   TGo4Status::PrintLine("  ID: \t%d", GetID());
}

Bool_t TGo4EventProcessorParameter::UpdateFrom(TGo4Parameter* rhs)
{
   GO4TRACE((12,"TGo4EventProcessorParameter::UpdateFrom()",__LINE__, __FILE__));
   auto procpar = dynamic_cast<TGo4EventProcessorParameter*>(rhs);
   if(!procpar) return kFALSE;
   SetName(procpar->GetName());
   fiID = procpar->GetID();
   return kTRUE;
}
