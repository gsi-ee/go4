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

#ifndef TGO4SIMPLEEVENT_H
#define TGO4SIMPLEEVENT_H

#include "TGo4EventElement.h"

#include "TClonesArray.h"

class TIterator;
class TGo4MbsSubEvent;
class TGo4SimpleSubEvent;

/**
 * Simple Event structure containing a fixed size TClonesArray of subevents.
 * Each subevent has integer data members corresponding to the first
 * n longwords of the TMbsSubEvent dynamic data field. May be used to work with
 * quasi raw event data within the root TTreeViewer.
 * @author J. Adamczewski
 * @since 07-feb-2001
 */
class TGo4SimpleEvent : public TGo4EventElement {

      friend class TGo4SimpleEventProcessor;

   public:
      TGo4SimpleEvent();

      TGo4SimpleEvent(Int_t subevtnum);

      virtual ~TGo4SimpleEvent() ;

      /**
       * Method called by the event owner (analysis step) to fill the
       * event element from the set event source. Event source can
       * be the source of the analysis step (if this is a raw event) or the event processor (if this is a reduced event).
       * Fill method has to specify how the event source writes to the
       * members of this event structure. Either by calling methods of
       * the source (like myevent.a=GetEventSource()->GetA(); etc ), or by
       * direct copy to the member (like GetEventSource()->FillMemoryAt(&myevent.a);)
       */
      virtual Int_t Fill();

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      virtual void Clear(Option_t *t="");
      /**
       * Set the internal iterator of the subevent array to the beginning of
       * the array.
       */
      void ResetIterator();

      /**
       * Iterator for subevent array. If reset is kTRUE, the iteration starts at the
       * first TObjArray slot, otherwise we continue at the position after the last
       * call of this method.
       */
      TGo4SimpleSubEvent* NextSubEvent();

      void PrintEvent();

      /**
       * Access to subevent in list by procid. Returns zero if no
       * subevent of this procid is found
       */
      TGo4SimpleSubEvent * GetSubEvent(Short_t procid);

      /**
       * Add subevent ito clonesarray with new procid. The pointer to the new
       * subevent is returned. If entry for procid already existed, this entry is
       * returned and the clonesarray is left as is.
       */
      TGo4SimpleSubEvent * AddSubEvent(Short_t procid);

      Int_t  GetCount() const;

   private:
      Int_t fiCount;

      /**
       * Index of last used slot in the clonesarray.
       */
      Int_t fiLastSlot;

      /**
       * Index of last existing slot in the clonesarray.
       */
      Int_t fiMaxSlot;

      /** @link aggregation
       * @clientCardinality 1
       * @supplierCardinality 1..*
       * @associationAsClass TClonesArray*/
      /*#  TGo4SimpleSubEvent fxSubEventArray1; */
      TClonesArray* fxSubEventArray;

      Int_t fiArrLen;

      Int_t fiTestArray[10]; //[fiArrLen]

      char fcTestCharArr[10]; //[fiArrLen]
      /**
       * Iterator for array.
       */
      TIterator * fxIterator; //!

      //   TGo4SimpleSubEvent fxFirstsubevent;
      //   TGo4SimpleSubEvent fxSecondsubevent;
      //   TGo4MbsSubEvent fxTestSub;

   ClassDef(TGo4SimpleEvent,1)
};

#endif //TGO4SIMPLEEVENT_H
