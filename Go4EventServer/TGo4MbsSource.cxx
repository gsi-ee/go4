#include "TGo4MbsSource.h"

#include "Riostream.h"
#include <stdlib.h>
#include "TObjArray.h"

#include "TGo4MbsEvent.h"
#include "TGo4MbsSubEvent.h"
#include "TGo4MbsSourceParameter.h"

#include "TGo4Log.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4EventEndException.h"

const UInt_t TGo4MbsSource::fguLONGBYCHAR  = sizeof(Int_t) / sizeof(Char_t);
const UInt_t TGo4MbsSource::fguSHORTBYCHAR = sizeof(Short_t) / sizeof(Char_t);
const UInt_t TGo4MbsSource::fguLONGBYSHORT = sizeof(Int_t) / sizeof(Short_t);
const UInt_t TGo4MbsSource::fguEVHEBYCHAR  = sizeof(s_evhe) /  sizeof(Char_t);

TGo4MbsSource::TGo4MbsSource(TGo4MbsSourceParameter* par, Int_t mode) :
   TGo4EventSource(par->GetName()),
   fiMode(mode),
   fxEvent(0), fxBuffer(0), fxInfoHeader(0),
   fbIsOpen(kFALSE), fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(par->GetStartEvent()) ,
   fuStopEvent(par->GetStopEvent()),
   fuEventInterval(par->GetEventInterval()),
   fiTimeout(par->GetTimeout())
   {
   fxInputChannel=f_evt_control();
   TRACE((15,"TGo4MbsSource::TGo4MbsSource(Text_t*, Int_t)",__LINE__, __FILE__));
   // Open() call will be done by subclasses ctors, so we can overwrite Open() method
   //cout <<"TGo4MbsSource with data copy mode="<<fbDataCopyMode << endl;
   }


TGo4MbsSource::TGo4MbsSource(const char* name, Int_t mode) :
   TGo4EventSource(name),
   fiMode(mode),
   fxEvent(0), fxBuffer(0), fxInfoHeader(0),
   fbIsOpen(kFALSE), fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(0) ,fuStopEvent(0), fuEventInterval(0),
   fiTimeout(-1)
   {
   fxInputChannel=f_evt_control();
   TRACE((15,"TGo4MbsSource::TGo4MbsSource(Text_t*, Int_t)",__LINE__, __FILE__));
   // Open() call will be done by subclasses ctors, so we can overwrite Open() method
   //cout <<"TGo4MbsSource with data copy mode="<<fbDataCopyMode << endl;
   }


TGo4MbsSource::TGo4MbsSource() :
   TGo4EventSource("default mbs source"),
   fiMode(0),
   fxEvent(0), fxBuffer(0), fxInfoHeader(0),
   fbIsOpen(kFALSE),fbDataCopyMode(kFALSE),
   fuEventCounter(0), fbFirstEvent(kTRUE),
   fuStartEvent(0) ,fuStopEvent(0), fuEventInterval(0), fiTimeout(-1)
   {
   fxInputChannel=f_evt_control();
   TRACE((15,"TGo4MbsSource::TGo4MbsSource()",__LINE__, __FILE__));
   }

TGo4MbsSource::~TGo4MbsSource()
{
   TRACE((15,"TGo4MbsSource::~TGo4MbsSource()",__LINE__, __FILE__));
   Close();
   if(fxInputChannel)free(fxInputChannel);
}

void TGo4MbsSource::SetPrintEvent(Int_t num, Int_t sid, Int_t longw,
      Int_t hexw, Int_t dataw)
{
   fxPrEventPar.fiNum=num;
   fxPrEventPar.fiSid=sid;
   fxPrEventPar.fiLong=longw;
   fxPrEventPar.fiHex=hexw;
   fxPrEventPar.fiData=dataw;
}


Bool_t TGo4MbsSource::BuildEvent(TGo4EventElement* dest)
{
   Int_t rev = NextEvent();
   if(rev != GETEVT__SUCCESS) {
      TGo4Log::Debug(" Mbs Event --  !!! NextEvent() error: %s !!! ", GetErrMess());
      return kFALSE;
   }

   BuildMbsEvent((TGo4MbsEvent*) dest);
   return kTRUE;
}


void TGo4MbsSource::BuildMbsEvent(TGo4MbsEvent* target)
{
   TRACE((12,"TGo4MbsSource::BuildMbsEvent(TGo4MbsEvent*)",__LINE__, __FILE__));
   TGo4MbsSubEvent* subtarget=0;
   if(fxEvent!=0 && GetEventStatus()==GETEVT__SUCCESS) {
      // check for printevent mode here:
      if(fxPrEventPar.fiNum>0) {
         f_evt_type(fxBuffer,
               (s_evhe *) fxEvent,
               fxPrEventPar.fiSid,
               fxPrEventPar.fiLong,
               fxPrEventPar.fiHex,
               fxPrEventPar.fiData);
         cout << endl; // flush cout buffer
         fxPrEventPar.fiNum--;
      }
      // we have a valid event, proceed
      Char_t* endofevent = (Char_t*) (fxEvent) +
            (fxEvent->l_dlen) * fguSHORTBYCHAR + fguEVHEBYCHAR ;
      //cout << "end of event "<< endofevent <<endl;
      target->SetValid(kTRUE); // reset target if previously was set to false
      target->SetDlen(fxEvent->l_dlen);
      target->SetType(fxEvent->i_type);
      target->SetSubtype(fxEvent->i_subtype);


      //**************************************************************************
      //**************************************************************************
      // Event Type 10:
      if(fxEvent->i_type==10) {
         s_ves10_1* subevent; // pointer to subevent
         Char_t* subevtpointer; // dito, in bytes
         target->SetTrigger(fxEvent->i_trigger);
         target->SetCount(fxEvent->l_count);
         target->SetDummy(fxEvent->i_dummy);
         Int_t totalsubdatalength=0; // check counter for total datalength of subevents
         if(fxEvent->l_dlen > 4) {
            // we have subevent data after the event header, proceed:
            subevent = (s_ves10_1*) (fxEvent + 1);
            // first subevent header starts after event header
            // loop over subevents:
            Int_t datalength = 0; // direct dlen from subevent header (in Short_t!)
            //            Int_t fieldlength=0; // actual size of the target Int_t data field
            while((datalength = subevent->l_dlen) >0 ) {
               totalsubdatalength+=datalength-2+sizeof(s_ves10_1)/sizeof(Short_t);
               if(datalength>fxEvent->l_dlen) {
                  TGo4Log::Debug(" !!! MbsSource --  SUBEVENTS LENGTH mismatch!!! skipping event #%d",fxEvent->l_count);
                  TGo4Log::Debug("\t sub dlen:%d, event dlen:%d ",datalength-2, fxEvent->l_dlen-4);
                  target->SetValid(kFALSE);
                  break;
               }
               Int_t * subeventid= (Int_t *) (subevent) + 2; // full id starts 2 ints after subevent head anyway
               //Int_t* subeventid= (Int_t*) &(subevent->i_procid); // full id is lw from control, subcrate, procid fields - some compilers complain here!
               Short_t* data = (Short_t*) (subevent+1); // data starts after subevent header
               subtarget = BuildMbsSubEvent(target, *subeventid, data, datalength); // find subevent that matches id and fill it
               subtarget->SetType(subevent->i_type); // need to set ids manually afterwards
               subtarget->SetSubtype(subevent->i_subtype);
               subevtpointer = (Char_t*) (subevent) +
                     datalength * fguSHORTBYCHAR + fguEVHEBYCHAR;
               subevent = (s_ves10_1*) subevtpointer;
               if ((Char_t*) subevent >= endofevent) {
                  //cout << "found end of event, breaking.."<< endl;
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
            //      cout <<"found event type 4" << endl;
            s_evhe* eventfourone= (s_evhe*) fxEvent; // points to event 4 1 start
            // copy pseudo event header information to our target:
            target->SetTrigger(4);
            target->SetCount(1);
            target->SetDummy(0);
            if(fxEvent->l_dlen > 0) {
               Int_t subeventid= 4; // arbitrarily defined here for type 4,1
               Short_t* data = (Short_t*) (eventfourone+1); // data starts after subevent header
               Int_t datalength=eventfourone->l_dlen+2; // length of later subevent header  (in Short_t!)
               // add 2 to direct dlen from 4,1 event header to account subevent header
               subtarget = BuildMbsSubEvent(target, subeventid, data, datalength); // find subevent that matches id and fill it
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
   } //if(fxEvent!=0 && GetEventStatus()==GETEVT__SUCCESS)
}


TGo4MbsSubEvent* TGo4MbsSource::BuildMbsSubEvent(TGo4MbsEvent * target, Int_t fullID, Short_t* source, Int_t datalength)
{
   Int_t fieldlength =0;
   TGo4MbsSubEvent* subtarget=0; // the subevent in use
   TGo4MbsSubEvent* subtargetindex=0; // target subevent iterator
   if(datalength>2)
      fieldlength = (datalength-2) / fguLONGBYSHORT ; // field is Int_t
   else {
      TGo4Log::Debug(" !!! MbsSource --  EMPTY subevent #%d ",fxEvent->l_count);
      fieldlength =0; // for empty subevents (<- W.M.)
   }

   target->ResetIterator();
   while ( ( subtargetindex = target->NextSubEvent(kTRUE) ) !=0 ) {
      // get pointer to complete id longword in structures:
      Int_t* subtargetid= &((subtargetindex->fxHeader).fiFullid);
      if(*subtargetid == fullID) {
         // subevent ids match:
         if(!subtargetindex->fbIsFilled) {
            // this has not been filled before, we fill this one
            subtarget=subtargetindex;
            break; // leave the loop, fill later
         } else {
            // was already filled in this cycle, continue for next sub
         }
      } else {
         // no match, try next subevent
      }
   } // while (subtargetindex...)

   if(!subtarget) {
      // we found no matching id, create new TObjArray entry
      subtarget = new TGo4MbsSubEvent(fieldlength);
      Int_t* newsubtargetid= &((subtarget->fxHeader).fiFullid);
      if(!fbDataCopyMode)
      {
         subtarget->fbIsDataOwner=kFALSE;
         delete [] (subtarget->fiData); // remove default field
         subtarget->fiAllocLen=0;
      }
      *newsubtargetid=fullID;
      TGo4Log::Debug(" Created new output subevent for event %d\n\tpid:%d subcrate:%d ctrl:%d",
            fxEvent->l_count ,subtarget->GetProcid(),subtarget->GetSubcrate(), subtarget->GetControl());
      target->fxSubEvArray->AddLast(subtarget);
   }
   ////     fill header:
   subtarget->SetDlen(datalength);
   void* data = (void*) source;
   if(fbDataCopyMode) {
      subtarget->fbIsDataOwner = kTRUE;
      subtarget->ReAllocate(fieldlength); // reallocate field if necessary
      //// fill data into target field:
      if(datalength>2)
         memcpy((void*) (subtarget->fiData),
               data, (datalength-2)*sizeof(Short_t));
   } else {
      // set reference to external data field in subevent
      subtarget->fbIsDataOwner=kFALSE;
      subtarget->fiAllocLen=fieldlength;
      if(datalength>2)
         subtarget->fiData= (Int_t*) data;
      else
         subtarget->fiData= 0; // reset for empty subevent
   }// if(fbDataCopyMode)
   subtarget->fbIsFilled=kTRUE; // remember we filled this one, never overwrite!
   return subtarget;
}

Int_t TGo4MbsSource::NextEvent()
{
   TRACE((12,"TGo4MbsSource::NextEvent()",__LINE__, __FILE__));
   // skip and sample mode introduced without changed gsievt functions for first tests
   ULong_t eventstep;

   if (fbFirstEvent) {
      fbFirstEvent = kFALSE;
      eventstep = fuStartEvent + 1;
   } else
      if(fuEventInterval > 0)
         eventstep = fuEventInterval;
      else
         eventstep = 1;

   // test if we had reached the last event:
   if(fuStopEvent!=0 && fuEventCounter>=fuStopEvent)
      SetEventStatus(GETEVT__NOMORE);
   else {
      // check possible overflow of our counter:
      if(fuEventCounter+eventstep<fuEventCounter)
      {
         TGo4Log::Warn("TGo4MbsSource::NextEvent(): Overflow of eventcounter at %d, reset to 0",fuEventCounter),
         fuEventCounter=0;
      }
      while (eventstep > 0) {
         // retrieve the event, skip all events until end of the step
         Int_t status=f_evt_get_event(fxInputChannel,
               (Int_t **) (void*) &fxEvent,
               (Int_t **) (void*) &fxBuffer);
         SetEventStatus(status);
         if(status!=0) break;
         eventstep--;
         fuEventCounter++;
      }
   }

   if(GetEventStatus()!=0) {
      Text_t buffer[TGo4EventSource::fguTXTLEN];
      f_evt_error(GetEventStatus(),buffer,1); // provide text message for later output
      SetErrMess(buffer);
   }

   switch(GetEventStatus()) {
      case 0:
         return 0;
         break;
      case GETEVT__TIMEOUT:
         throw TGo4EventTimeoutException(this);
         break;
      case GETEVT__NOMORE:
         throw TGo4EventEndException(this);
         break;
      default:
         throw TGo4EventErrorException(this);
         break;
   }

   cout << "MbsSource::NextEvent --  NEVER COME HERE" << endl;
   return GetEventStatus();
}

Int_t TGo4MbsSource::Open()
{
   TRACE((12,"TGo4MbsSource::Open()",__LINE__, __FILE__));

   if(fbIsOpen) return -1;
   //cout << "Open of TGo4MbsSource"<< endl;
   // open connection/file
   void* headptr= &fxInfoHeader; // suppress type-punned pointer warning
   f_evt_timeout(fxInputChannel, fiTimeout); // have to set timeout before open now JAM
   Int_t status = f_evt_get_open(
         fiMode,
         const_cast<Text_t*>( GetName() ),
         fxInputChannel,
         (Char_t**) headptr,
         0,
         0);
   SetCreateStatus(status);
   if(GetCreateStatus() !=GETEVT__SUCCESS) {
      //     TGo4Log::Debug(" Mbs Source --  !!! failed to open input from type %d:  %s!!! ",
      //        fiMode, GetName());
      Text_t buffer[TGo4EventSource::fguTXTLEN];
      f_evt_error(GetCreateStatus(),buffer,1); // provide text message for later output
      //
      //   snprintf(buffer,TGo4EventSource::fguTXTLEN-1," Mbs Source --  !!! failed to open input from type %d:  %s!!! ",
      //        fiMode, GetName());
      SetErrMess(buffer);
      fbIsOpen = kFALSE;
      throw TGo4EventErrorException(this);
   } else {
      TGo4Log::Debug(" Mbs Source --  opened input from type %d:  %s . Timeout=%d s",
            fiMode, GetName(), fiTimeout);

      fbIsOpen=kTRUE;
   }
   return status;
}

Int_t TGo4MbsSource::Close()
{
   TRACE((12,"TGo4MbsSource::Close()",__LINE__, __FILE__));
   if(!fbIsOpen) return -1;
   //cout << "Close of TGo4MbsSource"<< endl;
   Int_t rev = GetCreateStatus();
   // close connection/file
   if(rev == GETEVT__SUCCESS) {
      f_evt_get_close(fxInputChannel);
      fbIsOpen=kFALSE;
   }

   return rev;
}

s_bufhe * TGo4MbsSource::GetBufferHeader()
{
   return (s_bufhe*) f_evt_get_buffer_ptr(fxInputChannel);
}
