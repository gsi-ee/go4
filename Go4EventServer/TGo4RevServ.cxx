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

#include "TGo4RevServ.h"

#include "TGo4Log.h"
#include "TGo4RevServParameter.h"

TGo4RevServ::TGo4RevServ(const char* name) :
   TGo4MbsSource(name, GETEVT__REVSERV)
{
   GO4TRACE((15,"TGo4RevServ::TGo4RevServ(const char*)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source RevServ %s:  ",name);
   Open();
}

TGo4RevServ::TGo4RevServ(TGo4RevServParameter* par) :
   TGo4MbsSource(par, GETEVT__REVSERV)
{
   GO4TRACE((15,"TGo4RevServ::TGo4RevServ(TGo4RevServParameter*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source RevServ %s:  ",GetName());

   Open();
}

TGo4RevServ::TGo4RevServ()
//: TGo4MbsSource("Default RevServ" , GETEVT__REVSERV)
{
   GO4TRACE((15,"TGo4RevServ::TGo4RevServ()",__LINE__, __FILE__));
}

TGo4RevServ::~TGo4RevServ()
{
  GO4TRACE((15,"TGo4RevServ::~TGo4RevServ()",__LINE__, __FILE__));
  Close();
}
