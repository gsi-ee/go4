#ifndef TYYYRAWEVENT_H
#define TYYYRAWEVENT_H

#include "TGo4EventElement.h"

class TObjArray;
class TGo4FileSource;
class TGo4FileStore;
class TGo4TreeSource;
class TYYYEventSource;

/**
 * Example for user defined raw event class.
 * This event structure corresponds to the user event source.
 */
class TYYYRawEvent : public TGo4EventElement {

   friend class TYYYEventSource;
   friend class TYYYUnpackProc;

   public:

      TYYYRawEvent();

      TYYYRawEvent(const char* name);

      virtual ~TYYYRawEvent();

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

      virtual Int_t Init();

      /**
        * Method called by the event owner (analysis step) to clear the
        * event element.
        */
      virtual void Clear(Option_t *t="");

      /** Expand data field if necessary */
      void ReAllocate(Int_t newsize);

      void PrintEvent();

      Int_t GetColumns() const { return fiColumns; }

   private:

      /* Number of columns read from the file (used values for this event)  */
      Int_t fiColumns;

      Double_t * fdData; //[fiColumns]

      /** points to mbssource if exists */
      TYYYEventSource* fxUserSource; //!

      /** points to treesource if exists */
      TGo4TreeSource* fxTreeSource; //!

      /** points to filesource if exists */
      TGo4FileSource* fxFileSource; //!

   ClassDef(TYYYRawEvent,1)
};

#endif //TYYYRAWEVENT_H
