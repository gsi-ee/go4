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

#include "TGo4MbsTransport.h"

#include "TGo4Log.h"
#include "TGo4MbsTransportParameter.h"


TGo4MbsTransport::TGo4MbsTransport(const char* name)
: TGo4MbsSource(name , GETEVT__TRANS)
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport(const char*)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source MbsTransport %s:  ",name);
   Open();
}

TGo4MbsTransport::TGo4MbsTransport(TGo4MbsTransportParameter* par)
: TGo4MbsSource(par, GETEVT__TRANS)
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport(TGo4MbsTransportParameter**)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source MbsTransport %s:  ",GetName());

   Open();
}


TGo4MbsTransport::TGo4MbsTransport()
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport()",__LINE__, __FILE__));
}


TGo4MbsTransport::~TGo4MbsTransport()
{
   TRACE((15,"TGo4MbsTransport::~TGo4MbsTransport()",__LINE__, __FILE__));
   Close();
}
