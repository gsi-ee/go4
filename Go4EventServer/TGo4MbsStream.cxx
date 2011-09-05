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

#include "TGo4MbsStream.h"

#include "TGo4Log.h"
#include "TGo4MbsStreamParameter.h"

TGo4MbsStream::TGo4MbsStream(const char* name) :
   TGo4MbsSource(name , GETEVT__STREAM)
{
   TRACE((15,"TGo4MbsStream::TGo4MbsStream(const char*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsStream %s:  ",name);
   Open();
}

TGo4MbsStream::TGo4MbsStream(TGo4MbsStreamParameter* par) :
   TGo4MbsSource(par , GETEVT__STREAM)
{
   TRACE((15,"TGo4MbsStream::TGo4MbsStream(TGo4MbsStreamParameter**)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsStream %s:  ",GetName());
   Open();
}

TGo4MbsStream::TGo4MbsStream()
{
   TRACE((15,"TGo4MbsStream::TGo4MbsStream()",__LINE__, __FILE__));
}

TGo4MbsStream::~TGo4MbsStream()
{
TRACE((15,"TGo4MbsStream::~TGo4MbsStream()",__LINE__, __FILE__));
   Close();
}
