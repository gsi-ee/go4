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

#include "TGo4MbsTransportParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsTransportParameter::TGo4MbsTransportParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_TRANSPORT)
{
   GO4TRACE((14,"TGo4MbsTransportParameter::TGo4MbsTransportParameter(const char*)", __LINE__, __FILE__));
}

TGo4MbsTransportParameter::TGo4MbsTransportParameter() :
   TGo4MbsSourceParameter("default mbs transport", GO4EV_MBS_TRANSPORT)
{
   GO4TRACE((14,"TGo4MbsTransportParameter::TGo4MbsTransportParameter()", __LINE__, __FILE__));
}

TGo4MbsTransportParameter::~TGo4MbsTransportParameter()
{
   GO4TRACE((14,"TGo4MbsTransportParameter::~TGo4MbsTransportParameter()", __LINE__, __FILE__));
}
