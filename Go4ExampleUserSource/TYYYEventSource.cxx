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

#include "TYYYEventSource.h"

#include "TClass.h"

#include "TGo4Log.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventEndException.h"
#include "TGo4UserSourceParameter.h"
#include "TYYYRawEvent.h"

TYYYEventSource::TYYYEventSource(const char *name,
                                 const char *args,
                                 Int_t port) :
   TGo4EventSource(name),
   fbIsOpen(kFALSE),
   fxArgs(args),
   fiPort(port)
{
   Open();
}

TYYYEventSource::TYYYEventSource(TGo4UserSourceParameter* par) :
   TGo4EventSource(" "),
   fbIsOpen(kFALSE),
   fxArgs(),
   fiPort(0)
{
   if(par) {
      SetName(par->GetName());
      SetPort(par->GetPort());
      SetArgs(par->GetExpression());
      Open();
   } else {
      TGo4Log::Error("TYYYEventSource constructor with zero parameter!");
   }
}

TYYYEventSource::TYYYEventSource() :
   TGo4EventSource("default YYY source"),
   fbIsOpen(kFALSE),
   fxArgs(),
   fiPort(0)
{
}

TYYYEventSource::~TYYYEventSource()
{
   Close();
}

Bool_t TYYYEventSource::CheckEventClass(TClass *cl)
{
   return cl->InheritsFrom(TYYYRawEvent::Class());
}

Bool_t TYYYEventSource::BuildEvent(TGo4EventElement *dest)
{
   auto evnt = static_cast<TYYYRawEvent *>(dest);
   if (!evnt) return kFALSE;

   char sbuf[1024], buffer[1024];

   // read another event from open file into our buffer
   do {
      fxFile->getline(sbuf, sizeof(sbuf), '\n' ); // read whole line
      if(fxFile->eof() || !fxFile->good()) {
         // reached last line or read error?
         SetCreateStatus(1);
         TString errmsg = TString::Format("End of input file %s", GetName());
         SetErrMess(errmsg.Data());
         SetEventStatus(1);
         throw TGo4EventEndException(this);
      }
   } while(strstr(sbuf,"#") || strstr(sbuf,"!") ); // skip any comments

   Int_t status = 1;
   // process on event information in our buffer
   // scan the last input line for values:
   Int_t scanresult = 0;
   Int_t numval = 0;
   const char *cursor = sbuf;
   do {
      evnt->ReAllocate(numval+1); // check if realloc necessary
      scanresult = sscanf(cursor,"%s",buffer);
      if(scanresult != 0 && scanresult != -1) {
         evnt->fdData[numval] = atof(buffer);
         status = 0; // only ok if at least one value scanned
      }
      numval++;
      cursor += strlen(buffer)+1;
   } while( scanresult != 0 && scanresult != -1);

   // test here for error in input event

   if(status != 0) {
      evnt->SetValid(kFALSE);
      // somethings wrong, display error message from f_evt_error()
      SetErrMess("YYY Event Source --  ERROR !!!");
      throw TGo4EventErrorException(this);
   }

   return kTRUE;
}

Int_t TYYYEventSource::Open()
{
   if(fbIsOpen) return -1;
   TGo4Log::Info("Open of TYYYEventSource %s", GetName());
   // open connection/file
   fxFile = new std::ifstream(GetName());
   if(!fxFile || !fxFile->good()) {
      delete fxFile;
      fxFile = nullptr;
      TString errmsg = TString::Format("Eror opening user file:%s",GetName());
      SetCreateStatus(1);
      SetErrMess(errmsg.Data());
      throw TGo4EventErrorException(this);
   }
   fbIsOpen = kTRUE;
   return 0;
}

Int_t TYYYEventSource::Close()
{
   if(!fbIsOpen) return -1;
   TGo4Log::Info("Close of TYYYEventSource");
   Int_t status = 0; // closestatus of source
   if (fxFile) {
      delete fxFile;
      fxFile = nullptr;
   }
   fbIsOpen = kFALSE;
   return status;
}
