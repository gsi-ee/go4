#ifndef TGO4MBSEVENT_H
#define TGO4MBSEVENT_H

#include "Riostream.h"

#include "TGo4EventElement.h"

#include "TGo4EventHeader10.h"
#include "s_bufhe_swap.h"
#include "s_filhe_swap.h"
#include "TGo4MbsSubEvent.h"

class TIterator;
class TObjArray;
class TGo4MbsSource;
class TGo4MbsSubEvent;

/**
 * Wrapper for the standard gsi event structure as
 * deliverd from mbs.
 */
class TGo4MbsEvent : public TGo4EventElement {

   friend class TGo4MbsSource;

   public:

      // should only be used in streaming
      TGo4MbsEvent();

      /** Ctor expecting the number of subevents to initialize and
       * an array which specifies the subevent ids (procids) to
       * create. Subevent number subnum must not extend
       * the length of subids field. */
      TGo4MbsEvent(UInt_t subnum, Short_t* subids, UInt_t datasize=100);

      /** Ctor expecting the number of subevents to initialize and
       * arrays which specify the subevent ids (subcrate, ctrl, procid) to
       * create. Subevent number subnum must not extend
       * the length of subids field. Array datasizes contains preallocated size of
       * each subevent data field. */
      TGo4MbsEvent(UInt_t subnum, Char_t* subcrates, Char_t* controls, Short_t* procids, UInt_t* datasizes);

      virtual ~TGo4MbsEvent();

      /** Method called by the event owner (analysis step) to clear the event element. */
      virtual void Clear(Option_t *t="");

      void Set(Int_t dlen=0, Short_t type=10, Short_t subtype=1,
            Short_t dummy=0, Short_t trigger=0, Int_t count=0);

      void PrintEvent();

      /** Set the internal iterator of the subevent array to the beginning of
       * the array. */
      inline void ResetIterator() { fiSubEvIndex = 0; }

      /** Iterator for subevent array. Continue at the position after the last
       * call of this method. /param all specifies if all available
       * subevents are delivered, or only subevents which were filled
       * in the previous cycle. */
      TGo4MbsSubEvent* NextSubEvent(Bool_t all=kFALSE);

      /** Access to subevent in list by procid, subcrate and ctrl identifiers.
       * Returns zero if no subevent of these ids is found. If event has subevents with
       * identical ids, the first in order will be returned (use NextSubEvent() to access all
       * in this case). */
      TGo4MbsSubEvent * GetSubEvent(Char_t subcrate,
                                    Char_t ctrl,
                                    Short_t procid);

      /** Access to subevent in list by procid.
       * Returns zero if no subevent of this id is found. If event has subevents with
       * identical procid, the first in order will be returned (use NextSubEvent() to access all
       * in this case). Not recommended to use, left for compatibility! */
      TGo4MbsSubEvent * GetSubEvent(Short_t procid);

      /** Add new subevent to mbs event.
       * /param fullID - 32-bit combination of subcrate, ctrl, procid values
       * /param source - pointer on raw data
       * /param datalength - total length of subevent in DWORDS (2-bytes)
       * /param copydata if true data will be copied into internal allocated buffer,
       *        otherwise pointer on provided buffer will be used */
      TGo4MbsSubEvent* AddSubEvent(Int_t fullID,
            Short_t* source,
            Int_t datalength,
            Bool_t copydata = kFALSE);

      void SetDlen(Int_t dlen) { fxHeader.fxGSIHeader.fiDlen = dlen; }
      Int_t GetDlen() const { return fxHeader.fxGSIHeader.fiDlen; }
      Int_t GetIntLen() const { return (GetDlen()-4)*sizeof(Short_t)/sizeof(Int_t); }

      void SetType(Short_t type) { fxHeader.fxGSIHeader.fsType  = type; }
      Short_t GetType() const { return fxHeader.fxGSIHeader.fsType; }

      void SetSubtype(Short_t subtype) { fxHeader.fxGSIHeader.fsSubtype = subtype; }
      Short_t GetSubtype() const { return fxHeader.fxGSIHeader.fsSubtype; }

      void SetDummy(Short_t dummy) { fxHeader.fsDummy  = dummy; }
      Short_t GetDummy() const { return fxHeader.fsDummy; }

      void SetTrigger(Short_t trigger) { fxHeader.fsTrigger = trigger; }
      Short_t GetTrigger() const { return fxHeader.fsTrigger; }

      void SetCount(Int_t count) { fxHeader.fiCount = count; }
      Int_t GetCount() const { return fxHeader.fiCount; }

      /** Access to the info header of the current mbs source */
      s_filhe * GetMbsSourceHeader();

      /** Access to the buffer header of the current event */
      s_bufhe * GetMbsBufferHeader();

      /** Set the "printout event" mode for the current mbs source.
       * See TGo4MbsSource::SetPrintEvent for parameter description. */
      void SetPrintEvent(Int_t num=1, Int_t sid=-1, Int_t longw=1, Int_t hexw=1, Int_t dataw=0);

   private:

      /** @link aggregationByValue */
      TGo4EventHeader10 fxHeader;

      /** dynamic array of subevents. */
      TObjArray* fxSubEvArray;

      /** Subevent index in array, replacement for Iterator. */
      Int_t fiSubEvIndex;//!

   ClassDef(TGo4MbsEvent,1)
};

#endif //TGO4MBSEVENT_H
