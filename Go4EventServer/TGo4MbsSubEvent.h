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

#ifndef TGO4MBSSUBEVENT_H
#define TGO4MBSSUBEVENT_H

#include "TGo4EventElement.h"

#include "TGo4SubEventHeader10.h"

class TGo4MbsEvent;

/** Subevent class for gsi mbs data. */
class TGo4MbsSubEvent : public TGo4EventElement {

   friend class TGo4MbsSource;
   friend class TGo4MbsEvent;

   public:

      // only for streaming
      TGo4MbsSubEvent();

      TGo4MbsSubEvent(Int_t datasize);

      virtual ~TGo4MbsSubEvent();

      virtual Int_t Fill() { return -1; } // method not used

      virtual void Clear(Option_t *t="");

      /** Print event with default arguments */
      virtual void PrintEvent();

      /** Print MBS subevent
       * \param longw - data in long format (4 bytes)
       * \param hexw  - print in hex format
       * \param dataw - print raw data */
      void PrintMbsSubevent(Bool_t longw = kTRUE, Bool_t hexw = kTRUE, Bool_t dataw = kTRUE);

      void Set(Int_t dlen, Short_t type=10, Short_t subtype=1,
            Short_t procid=0, Char_t subcrate=0, Char_t ctrl=0);

      void  SetDlen(Int_t dlen) { fxHeader.fxGSIHeader.fiDlen = dlen; }
      Int_t  GetDlen() const { return fxHeader.fxGSIHeader.fiDlen; }

      /** Return raw data length in bytes */
      Int_t  GetByteLen() const { return (GetDlen()-2) * 2; }

      /** Return raw data length in int (4-bytes) */
      Int_t  GetIntLen() const { return GetByteLen() / 4; }

      void  SetType(Short_t type) { fxHeader.fxGSIHeader.fsType  = type; }
      Short_t GetType() const { return fxHeader.fxGSIHeader.fsType; }

      void  SetSubtype(Short_t subtype) { fxHeader.fxGSIHeader.fsSubtype = subtype; }
      Short_t GetSubtype() const { return fxHeader.fxGSIHeader.fsSubtype; }

      void  SetProcid(Short_t procid) { fxHeader.fsProcid = procid; }
      Short_t GetProcid() const { return fxHeader.fsProcid; }

      void  SetSubcrate(Char_t subcrate) { fxHeader.fcSubcrate = subcrate; }
      Char_t GetSubcrate() const { return fxHeader.fcSubcrate; }

      void  SetControl(Char_t control) { fxHeader.fcControl = control; }
      Char_t GetControl() const { return fxHeader.fcControl; }

      void SetFullId(Int_t fullid) { fxHeader.fiFullid = fullid; }
      Int_t GetFullId() const { return fxHeader.fiFullid; }

      /** Return length of allocated memory for data field. */
      Int_t GetAllocatedLength() const { return fiAllocLen; }

      /** Direct access to the fiData field pointer. User has to care
       * about allocated range as specified in fiAllocLen. */
      Int_t* GetDataField() { return fiData; }

      /** Returns the value at position i in the fiData field.
       * If Index i is out of DLen range, zero is returned. */
      Int_t Data(Int_t i) const
         { return ((i<0) || (i>=GetAllocatedLength())) ? 0 : fiData[i]; }

      /** true if this subevent was filled since the last Clear() */
      Bool_t IsFilled() const { return fbIsFilled; }

   private:

      /** Allocate the data field with newsize. Free the previously
       * allocated field before, and set new fiAllocLen value. */
      void ReAllocate(Int_t newsize);

      /** True if this subevent has already been filled in this event cycle.
       * For subsequent subevents of identical header ids, prevents overwriting. */
      Bool_t fbIsFilled;

      /** True if subevent owns data field.
       * note that this should not be streamed to root file!*/
      Bool_t fbIsDataOwner; //!

      /** @link aggregationByValue */
      TGo4SubEventHeader10 fxHeader;

      /** Length of the allocated data field. Is _not_ equal to the
       * subevent data length which can be smaller. On filling the
       * subevent, it is tested if the source data fits into the
       * already allocated field; if the source data is larger,
       * we will re-allocate the field with the larger size
       * before filling it. If the source data is smaller, we
       * leave the data field as is and use only the first
       * DLen words. */
      Int_t fiAllocLen;

      /** Pointer to external integer field containing the
       * subevent data. Length of this field may be changed
       * dynamically depending on the maximum length
       * of the incoming subevent of this type. */
      Int_t* fiData; //[fiAllocLen]

   ClassDef(TGo4MbsSubEvent,2)
};

#endif //TGO4MBSSUBEVENT_H
