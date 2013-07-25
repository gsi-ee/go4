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

#include "TGo4MbsEventServer.h"

#include "TGo4Log.h"
#include "TGo4MbsEventServerParameter.h"


TGo4MbsEventServer::TGo4MbsEventServer(const char* name) :
   TGo4MbsSource(name, GETEVT__EVENT)
{
   GO4TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer(const char*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsEventServer %s:  ",name);

   Open();
}

TGo4MbsEventServer::TGo4MbsEventServer(TGo4MbsEventServerParameter* par) :
   TGo4MbsSource(par, GETEVT__EVENT)
{
   GO4TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer(TGo4MbsEventServerParameter*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsEventServer %s:  ",GetName());

   Open();
}

TGo4MbsEventServer::TGo4MbsEventServer() :
   TGo4MbsSource()
{
   GO4TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer()",__LINE__, __FILE__));
}

TGo4MbsEventServer::~TGo4MbsEventServer()
{
   GO4TRACE((15,"TGo4MbsEventServer::~TGo4MbsEventServer()",__LINE__, __FILE__));
   Close();
}
