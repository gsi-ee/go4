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

#include "TGo4MbsSource.h"

#include <cstdlib>
#include <iostream>

#include "TClass.h"
#include "TSystem.h"

#include "TGo4MbsEvent.h"
#include "TGo4MbsSourceParameter.h"
#include "TGo4AnalysisImp.h"

#include "TGo4Log.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4EventEndException.h"

const UInt_t TGo4MbsSource::fguLONGBYCHAR  = sizeof(Int_t) / sizeof(Char_t);
const UInt_t TGo4MbsSource::fguSHORTBYCHAR = sizeof(Short_t) / sizeof(Char_t);
const UInt_t TGo4MbsSource::fguLONGBYSHORT = sizeof(Int_t) / sizeof(Short_t);
const UInt_t TGo4MbsSource::fguEVHEBYCHAR  = sizeof(s_evhe) /  sizeof(Char_t);

Bool_t TGo4MbsSource::gbPollingMode = kFALSE;

TGo4MbsSource::TGo4MbsSource(TGo4MbsSourceParameter* par, Int_t mode) :
   TGo4EventSource(par->GetName()),
   fiMode(mode),
   fiRetryCnt(par->GetRetryCnt()),
   fxEvent(nullptr), fxBuffer(nullptr), fxInfoHeader(nullptr),
   fbIsOpen(kFALSE), fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(par->GetStartEvent()) ,
   fuStopEvent(par->GetStopEvent()),
   fuEventInterval(par->GetEventInterval()),
   fiTimeout(par->GetTimeout()),
   fbPollingMode(kFALSE),
   fiPort(par->GetPort())
{
   fxInputChannel=f_evt_control();
   GO4TRACE((15,"TGo4MbsSource::TGo4MbsSource(const char *, Int_t)",__LINE__, __FILE__));
   // Open() call will be done by subclasses ctors, so we can overwrite Open() method
}


TGo4MbsSource::TGo4MbsSource(const char *name, Int_t mode) :
   TGo4EventSource(name),
   fiMode(mode),
   fiRetryCnt(0),
   fxEvent(nullptr), fxBuffer(nullptr), fxInfoHeader(nullptr),
   fbIsOpen(kFALSE), fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(0), fuStopEvent(0), fuEventInterval(0),
   fiTimeout(-1), fbPollingMode(kFALSE), fiPort(0)
{
   fxInputChannel = f_evt_control();
   GO4TRACE((15,"TGo4MbsSource::TGo4MbsSource(const char *, Int_t)",__LINE__, __FILE__));

   SetTimeout(fgiTIMEOUTDEFAULT);

   // Open() call will be done by subclasses ctors, so we can overwrite Open() method
}


TGo4MbsSource::TGo4MbsSource() :
   TGo4EventSource("default mbs source"),
   fiMode(0),
   fiRetryCnt(0),
   fxEvent(nullptr), fxBuffer(nullptr), fxInfoHeader(nullptr),
   fbIsOpen(kFALSE), fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(0), fuStopEvent(0), fuEventInterval(0),
   fiTimeout(-1), fbPollingMode(kFALSE), fiPort(0)
{
   fxInputChannel = f_evt_control();
   GO4TRACE((15,"TGo4MbsSource::TGo4MbsSource()",__LINE__, __FILE__));
}

TGo4MbsSource::~TGo4MbsSource()
{
   GO4TRACE((15,"TGo4MbsSource::~TGo4MbsSource()",__LINE__, __FILE__));
   Close();
   if(fxInputChannel)free(fxInputChannel);
}

void TGo4MbsSource::SetPollingMode(Bool_t on)
{
   gbPollingMode = on;
}

void TGo4MbsSource::SetPrintEvent(Int_t num, Int_t sid, Int_t longw, Int_t hexw, Int_t dataw)
{
   fxPrEventPar.fiNum = num;
   fxPrEventPar.fiSid = sid;
   fxPrEventPar.fiLong = longw;
   fxPrEventPar.fiHex = hexw;
   fxPrEventPar.fiData = dataw;
}

Bool_t TGo4MbsSource::CheckEventClass(TClass *cl)
{
   return cl->InheritsFrom(TGo4MbsEvent::Class());
}

Bool_t TGo4MbsSource::BuildEvent(TGo4EventElement *dest)
{
   Int_t rev = NextEvent();
   if(rev != GETEVT__SUCCESS) {
      TGo4Log::Debug(" Mbs Event --  !!! NextEvent() error: %s !!! ", GetErrMess());
      return kFALSE;
   }

   BuildMbsEvent((TGo4MbsEvent *) dest);
   return kTRUE;
}


void TGo4MbsSource::BuildMbsEvent(TGo4MbsEvent *target)
{
   GO4TRACE((12,"TGo4MbsSource::BuildMbsEvent(TGo4MbsEvent *)",__LINE__, __FILE__));
   TGo4MbsSubEvent *subtarget = nullptr;
   if(fxEvent && (GetEventStatus() == GETEVT__SUCCESS)) {
      // check for printevent mode here:
      if(fxPrEventPar.fiNum>0) {
         f_evt_type(fxBuffer,
               (s_evhe *) fxEvent,
               fxPrEventPar.fiSid,
               fxPrEventPar.fiLong,
               fxPrEventPar.fiHex,
               fxPrEventPar.fiData);
         std::cout << std::endl; // flush std::cout buffer
         fxPrEventPar.fiNum--;
      }
      // we have a valid event, proceed
      Char_t *endofevent = (Char_t *) (fxEvent) +
            (fxEvent->l_dlen) * fguSHORTBYCHAR + fguEVHEBYCHAR ;
      target->SetValid(kTRUE); // reset target if previously was set to false
      target->SetDlen(fxEvent->l_dlen);
      target->SetType(fxEvent->i_type);
      target->SetSubtype(fxEvent->i_subtype);


      //**************************************************************************
      //**************************************************************************
      // Event Type 10:
      if(fxEvent->i_type==10) {
         s_ves10_1* subevent; // pointer to subevent
         Char_t *subevtpointer; // dito, in bytes
         target->SetTrigger(fxEvent->i_trigger);
         target->SetCount(fxEvent->l_count);
         target->SetDummy(fxEvent->i_dummy);
         Int_t totalsubdatalength = 0; // check counter for total datalength of subevents
         if(fxEvent->l_dlen > 4) {
            // we have subevent data after the event header, proceed:
            subevent = (s_ves10_1*) (fxEvent + 1);
            // first subevent header starts after event header
            // loop over subevents:
            Int_t datalength = 0; // direct dlen from subevent header (in Short_t!)
            // Int_t fieldlength = 0; // actual size of the target Int_t data field
            while((datalength = subevent->l_dlen) > 0) {
               totalsubdatalength+=datalength-2+sizeof(s_ves10_1)/sizeof(Short_t);
               if(datalength>fxEvent->l_dlen) {
                  TGo4Log::Debug(" !!! MbsSource --  SUBEVENTS LENGTH mismatch!!! skipping event #%d",fxEvent->l_count);
                  TGo4Log::Debug("\t sub dlen:%d, event dlen:%d ",datalength-2, fxEvent->l_dlen-4);
                  target->SetValid(kFALSE);
                  break;
               }
               Int_t *subeventid= (Int_t *) (subevent) + 2; // full id starts 2 ints after subevent head anyway
               //Int_t *subeventid= (Int_t *) &(subevent->i_procid); // full id is lw from control, subcrate, procid fields - some compilers complain here!
               Short_t *data = (Short_t *) (subevent+1); // data starts after subevent header
               subtarget = target->AddSubEvent(*subeventid, data, datalength, fbDataCopyMode); // find subevent that matches id and fill it
               subtarget->SetType(subevent->i_type); // need to set ids manually afterwards
               subtarget->SetSubtype(subevent->i_subtype);
               subevtpointer = (Char_t*) (subevent) +
                     datalength * fguSHORTBYCHAR + fguEVHEBYCHAR;
               subevent = (s_ves10_1*) subevtpointer;
               if ((Char_t*) subevent >= endofevent) {
                  // found end of event, breaking
                  break;
               }
            } // while((datalength=subevent->l_dlen) >0)
            if(totalsubdatalength!=fxEvent->l_dlen-4) {
               TGo4Log::Debug(" !!! MbsSource --  SUBEVENTS TOTAL LENGTH mismatch!!! disabling event #%d",fxEvent->l_count);
               TGo4Log::Debug("\t subdlen sum:%d, event dlen:%d ",totalsubdatalength, fxEvent->l_dlen-4);
               target->SetValid(kFALSE);
            }
         } else { // if(fxEvent->dlen>4)
            // sorry, no subevents after event header
            TGo4Log::Debug(" !!! MbsSource --  NO SUBEVENTS!!! ");
            SetErrMess("!!! BuildMbsEvent: --  NO SUBEVENTS!!!");
            throw TGo4EventTimeoutException(this); // no subevts=timeout
         } // end if (fxEvent->dlen>0)
      } else
         //**************************************************************************
         //**************************************************************************
         // Event Type 4:
         if(fxEvent->i_type==4) {
            //      std::cout <<"found event type 4" << std::endl;
            s_evhe* eventfourone= (s_evhe*) fxEvent; // points to event 4 1 start
            // copy pseudo event header information to our target:
            target->SetTrigger(4);
            target->SetCount(1);
            target->SetDummy(0);
            if(fxEvent->l_dlen > 0) {
               Int_t subeventid = 4; // arbitrarily defined here for type 4,1
               Short_t *data = (Short_t *) (eventfourone+1); // data starts after subevent header
               Int_t datalength = eventfourone->l_dlen+2; // length of later subevent header  (in Short_t!)
               // add 2 to direct dlen from 4,1 event header to account subevent header
               subtarget = target->AddSubEvent(subeventid, data, datalength, fbDataCopyMode); // find subevent that matches id and fill it
               subtarget->SetType(4);
               subtarget->SetSubtype(1);
            } else { // if(fxEvent->dlen>0)
               // sorry, no subevents after event header
               TGo4Log::Debug(" !!! MbsSource --  NO Data in event 4,1!!! ");
               SetErrMess("!!! BuildMbsEvent: --  NO Data in event 4,1!!!");
               throw TGo4EventTimeoutException(this); // no data=timeout
            } // end if (fxEvent->dlen>0)
         } else {
            TGo4Log::Debug(" !!! Mbs Source --  ERROR: Unknown event type %d !!! ",fxEvent->i_type);
            throw TGo4EventErrorException(this);
         } //if(fxEvent->i_type==...)

   } else {
      // somethings wrong, display error message from f_evt_error()
      TGo4Log::Debug(" !!! Mbs Source --  ERROR: %s !!! ",GetErrMess());
      throw TGo4EventErrorException(this);
   } //if(fxEvent && GetEventStatus()==GETEVT__SUCCESS)
}


TGo4MbsSubEvent *TGo4MbsSource::BuildMbsSubEvent(TGo4MbsEvent *target, Int_t fullID, Short_t *source, Int_t datalength)
{
   // code moved into TGo4MbsEvent class, method kept for compatibility

   return target->AddSubEvent(fullID, source, datalength, fbDataCopyMode);
}

void HandleAnlysisEvents()
{
   if (TGo4Analysis::Instance())
      TGo4Analysis::Instance()->ProcessEvents();
}

Int_t TGo4MbsSource::NextEvent()
{

frombegin:

   GO4TRACE((12,"TGo4MbsSource::NextEvent()",__LINE__, __FILE__));
   // skip and sample mode introduced without changed gsievt functions for first tests
   ULong_t eventstep;

   if (fbFirstEvent) {
      fbFirstEvent = kFALSE;
      eventstep = fuStartEvent + 1;
      TGo4Log::Info("MBS source:: First event = %lu step = %lu\n", fuStartEvent, fuEventInterval);

   } else
      if(fuEventInterval > 0)
         eventstep = fuEventInterval;
      else
         eventstep = 1;

   // test if we had reached the last event:
   if(fuStopEvent != 0 && fuEventCounter >= fuStopEvent)
      SetEventStatus(GETEVT__NOMORE);
   else {
      // check possible overflow of our counter:
      if(fuEventCounter+eventstep<fuEventCounter)
      {
         TGo4Log::Warn("TGo4MbsSource::NextEvent(): Overflow of eventcounter at %lu, reset to 0",fuEventCounter),
         fuEventCounter = 0;
      }

      while (eventstep > 0) {
         // retrieve the event, skip all events until end of the step
         Int_t status = f_evt_get_event(fxInputChannel,
                                         (Int_t **) (void *) &fxEvent,
                                         (Int_t **) (void *) &fxBuffer);

         if (fbPollingMode && (status == GETEVT__TIMEOUT))
            HandleAnlysisEvents();

         SetEventStatus(status);
         if(status != 0) break;
         eventstep--;
         fuEventCounter++;
      }
   }

   if (GetEventStatus() == 0)
      return 0;

   // provide text message for later output
   char buffer[TGo4EventSource::fguTXTLEN];
   f_evt_error(GetEventStatus(),buffer,1);
   SetErrMess(TString::Format("%s name:%s", buffer, GetName()).Data());

   if (GetEventStatus() == GETEVT__TIMEOUT)
      throw TGo4EventTimeoutException(this);

   if (GetEventStatus() == GETEVT__NOMORE)
      throw TGo4EventEndException(this);

   if (((fiMode==GETEVT__STREAM) || (fiMode==GETEVT__TRANS) || (fiMode==GETEVT__REVSERV) || (fiMode==GETEVT__EVENT)) && (fiRetryCnt > 0)) {

      printf("Error code %d mess %s\n", GetEventStatus(), GetErrMess());

      Close();
      Int_t cnt = fiRetryCnt;

      while (cnt-->0) {
         gSystem->Sleep(1000);

         //if (TGo4Analysis::Instance())
         //   if (TGo4Analysis::Instance()->IsStopWorking()) return GetEventStatus();
         try {
            Open();
            if (fbIsOpen) {
               printf("Retry %d successful\n", cnt);
               fflush(stdout);
               goto frombegin;
            }
         } catch (TGo4EventErrorException &) {
         }
         printf("Retry %d failed\n", cnt);
         fflush(stdout);
      }
   }

   throw TGo4EventErrorException(this);
}


Int_t TGo4MbsSource::Open()
{
   GO4TRACE((12,"TGo4MbsSource::Open()",__LINE__, __FILE__));

   if(fbIsOpen) return -1;
   // open connection/file

   char name[5000];
   strncpy(name, GetName(), sizeof(name) - 1);
   int nport = fiPort;

   char *separ = strrchr(name, ':');
   if ((nport <= 0) && separ && (GetMode() != GETEVT__FILE)) {
      if ((sscanf(separ+1,"%d",&nport)==1) && (nport>0)) {
         *separ = 0;
      } else {
         nport = fiPort;
      }
   }

   Int_t status  = f_evt_source_port(nport);
   SetCreateStatus(status);
   if(GetCreateStatus() != GETEVT__SUCCESS) {
       char buffer[TGo4EventSource::fguTXTLEN];
       f_evt_error(GetCreateStatus(), buffer, 1); // provide text message for later output
       SetErrMess(TString::Format("%s name:%s port:%d", buffer, name, nport).Data());
       throw TGo4EventErrorException(this);
    }

   fbPollingMode = gbPollingMode && ((fiMode==GETEVT__STREAM) || (fiMode==GETEVT__TRANS) || (fiMode==GETEVT__REVSERV) || (fiMode==GETEVT__EVENT));
   f_evt_timeout(fxInputChannel, fiTimeout); // have to set timeout before open now JAM

   status = f_evt_get_open(
         fiMode,
         name,
         fxInputChannel,
         (Char_t**) ((void *) &fxInfoHeader), // suppress type-punned pointer warning
         0,
         0);
   SetCreateStatus(status);
   if(GetCreateStatus() != GETEVT__SUCCESS) {
      char buffer[TGo4EventSource::fguTXTLEN];
      f_evt_error(GetCreateStatus(),buffer,1); // provide text message for later output
      SetErrMess(TString::Format("%s name:%s", buffer, GetName()).Data());
      fbIsOpen = kFALSE;
      throw TGo4EventErrorException(this);
   } else {
      TGo4Log::Debug(" Mbs Source --  opened input from type %d:  %s . Timeout=%d s",
            fiMode, GetName(), fiTimeout);
      fbIsOpen = kTRUE;

      // only remote event server implement internally polling, stream server makes return timeout itself
      if (fbPollingMode && (fiMode==GETEVT__REVSERV))
         fxInputChannel->cb_polling = &HandleAnlysisEvents;
   }
   return status;
}

Int_t TGo4MbsSource::Close()
{
   GO4TRACE((12,"TGo4MbsSource::Close()",__LINE__, __FILE__));
   if(!fbIsOpen) return -1;
   Int_t rev = GetCreateStatus();
   // close connection/file
   if(rev == GETEVT__SUCCESS) {
      f_evt_get_close(fxInputChannel);
      fbIsOpen=kFALSE;
   }

   return rev;
}

s_bufhe *TGo4MbsSource::GetBufferHeader()
{
   return (s_bufhe *) f_evt_get_buffer_ptr(fxInputChannel);
}
