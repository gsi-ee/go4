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

#include "TGo4RevServParameter.h"

#include "Riostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"


TGo4RevServParameter::TGo4RevServParameter(const char* name, Int_t defport) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_REVSERV)
{
   TRACE((14,"TGo4MbsRevServParameter::TGo4MbsRevServParameter(const char*,...)", __LINE__, __FILE__));

   if (defport>0) SetPort(defport);
}

TGo4RevServParameter::TGo4RevServParameter() :
   TGo4MbsSourceParameter("default mbs remote event server", GO4EV_MBS_REVSERV)
{
TRACE((14,"TGo4MbsRevServParameter::TGo4MbsRevServParameter()", __LINE__, __FILE__));
}

TGo4RevServParameter::~TGo4RevServParameter()
{
   TRACE((14,"TGo4MbsRevServParameter::~TGo4MbsRevServParameter()", __LINE__, __FILE__));
}
