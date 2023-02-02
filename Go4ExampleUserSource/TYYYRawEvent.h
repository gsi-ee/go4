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

#ifndef TYYYRawEvent_H
#define TYYYRawEvent_H

#include "TGo4EventElement.h"

/**
 * Example for user defined raw event class.
 * This event structure corresponds to the user event source.
 */
class TYYYRawEvent : public TGo4EventElement {

   public:

      TYYYRawEvent();

      TYYYRawEvent(const char *name);

      virtual ~TYYYRawEvent();

      /**
        * Method called by the event owner (analysis step) to clear the
        * event element.
        */
      void Clear(Option_t *opt = "") override;

      /** Expand data field if necessary */
      void ReAllocate(Int_t newsize);

      void PrintEvent() override;

      Int_t GetColumns() const { return fiColumns; }

      /* Number of columns read from the file (used values for this event)  */
      Int_t fiColumns{0};

      Double_t *fdData{nullptr}; //[fiColumns]

      Int_t fiAllocated{0}; //! allocated array size

   ClassDefOverride(TYYYRawEvent,1)
};

#endif
