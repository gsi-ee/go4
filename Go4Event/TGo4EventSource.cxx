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

#include "TGo4EventSource.h"

#include "TClass.h"
#include "snprintf.h"

#include "TGo4EventElement.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventEndException.h"
#include "TGo4Log.h"

const Int_t TGo4EventSource::fgiTIMEOUTDEFAULT=1; // source timeout default in seconds

TGo4EventSource::TGo4EventSource(const char* name) :
   TNamed(name, "This is a Go4 Event Source"),
   fiCreateStatus(0),
   fiEventStatus(0)
{
   GO4TRACE((15,"TGo4EventSource::TGo4EventSource()",__LINE__, __FILE__));
}

TGo4EventSource::TGo4EventSource() :
   TNamed("default eventsource","This is a Go4 EventSource"),
   fiCreateStatus(0),
   fiEventStatus(0)
{
}

TGo4EventSource::~TGo4EventSource()
{
   GO4TRACE((15,"TGo4EventSource::~TGo4EventSource()",__LINE__, __FILE__));
}

void TGo4EventSource::ThrowError(Int_t crestat, Int_t errstat, const char* message,...)
{
   //
   UInt_t lbuflen = TGo4EventSource::fguTXTLEN;
   char* txtbuf = new char[lbuflen];
   va_list args;
   va_start(args, message);
   vsnprintf(txtbuf, lbuflen, message, args);
   va_end(args);
   if(crestat!=0)
      SetCreateStatus(crestat);
   if(errstat!=0)
      SetEventStatus(errstat);
   SetErrMess(txtbuf);
   delete[] txtbuf;
   throw TGo4EventErrorException(this);
}

void TGo4EventSource::ThrowEOF(Int_t crestat, Int_t errstat, const char* message,...)
{
   //
   UInt_t lbuflen = TGo4EventSource::fguTXTLEN;
   char* txtbuf = new char[lbuflen];
   va_list args;
   va_start(args, message);
   vsnprintf(txtbuf, lbuflen, message, args);
   va_end(args);
   if(crestat!=0)
      SetCreateStatus(crestat);
   if(errstat!=0)
      SetEventStatus(errstat);
   SetErrMess(txtbuf);
   delete[] txtbuf;
   throw TGo4EventEndException(this);
}

void TGo4EventSource::Clear(Option_t*)
{
   // dummy clear, may be implemented by user
   TGo4Log::Info("Default clear of eventsource %s", GetName());
}


const char* TGo4EventSource::GetActiveName()
{
   return GetName();
}

Bool_t TGo4EventSource::CheckEventClass(TClass* cl)
{
   return cl->InheritsFrom(TGo4EventElement::Class());
}
