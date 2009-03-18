#ifndef TGO4SIMPLESUBEVENT_H
#define TGO4SIMPLESUBEVENT_H

#include "TGo4EventElement.h"

/**
 * @since 07-feb-2001
 * @author J. Adamczewski
 */
class TGo4SimpleSubEvent : public TGo4EventElement {
public:
   static Int_t fgiZeroField[13]; //!

   /**
    * Processor ID [from setup]
    */
   Short_t fsProcid;

   /**
    * Length of the used data field (number of longwords). Corresponds to the
    * fiDlen Parameter of the MbsSubevent, which gives the fieldlength in shortwords.
    */
   Int_t fiFieldLen;

   /**
    * Data longword
    */
   Int_t fiD0;

   /**
    * Data longword
    */
   Int_t fiD1;

   /**
    * Data longword
    */
   Int_t fiD2;

   /**
    * Data longword
    */
   Int_t fiD3;

   /**
    * Data longword
    */
   Int_t fiD4;

   /**
    * Data longword
    */
   Int_t fiD5;

   /**
    * Data longword
    */
   Int_t fiD6;

   /**
    * Data longword
    */
   Int_t fiD7;

   /**
    * Data longword
    */
   Int_t fiD8;

   /**
    * Data longword
    */
   Int_t fiD9;

   /**
    * Data longword
    */
   Int_t fiD10;

   /**
    * Data longword
    */
   Int_t fiD11;

   /**
    * Data longword
    */
   Int_t fiD12;


//    TGo4MbsSubEvent fxTestSub;

//    TClonesArray* fxMbsSubEventArray;

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

   void PrintEvent();

   TGo4SimpleSubEvent() ;

   virtual ~TGo4SimpleSubEvent() ;

   ClassDef(TGo4SimpleSubEvent,1)

};
#endif //TGO4SIMPLESUBEVENT_H



