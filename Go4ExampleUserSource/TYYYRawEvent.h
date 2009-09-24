#ifndef TYYYRAWEVENT_H
#define TYYYRAWEVENT_H

#include "TGo4EventElement.h"

/**
 * Example for user defined raw event class.
 * This event structure corresponds to the user event source.
 */
class TYYYRawEvent : public TGo4EventElement {

   public:

      TYYYRawEvent();

      TYYYRawEvent(const char* name);

      virtual ~TYYYRawEvent();

      /**
        * Method called by the event owner (analysis step) to clear the
        * event element.
        */
      virtual void Clear(Option_t *t="");

      /** Expand data field if necessary */
      void ReAllocate(Int_t newsize);

      void PrintEvent();

      Int_t GetColumns() const { return fiColumns; }

      /* Number of columns read from the file (used values for this event)  */
      Int_t fiColumns;

      Double_t * fdData; //[fiColumns]

      Int_t fiAllocated; //! allocated array size

   ClassDef(TYYYRawEvent,1)
};

#endif //TYYYRAWEVENT_H
