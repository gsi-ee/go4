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

#ifndef TGO4EVENTSERVERFACTORY_H
#define TGO4EVENTSERVERFACTORY_H

#include "TGo4EventFactory.h"

/**
 * @brief Abstract event factory providing all go4 event service classes.
 * @details User factory should be a subclass of this class. Parameter objects of
 * factory methods are checked by class name; the known event sources /event stores
 * are created, unknown parameter objects let return 0 and can be handled by
 * the user factory. See TGo4StepFactory for ready-to-use implementation of such factory class
 * @ingroup go4_event
 */
class TGo4EventServerFactory : public TGo4EventFactory {
   public:

      TGo4EventServerFactory();

      TGo4EventServerFactory(const char *name);

      virtual ~TGo4EventServerFactory();

      TGo4EventElement *CreateInputEvent() override;

      TGo4EventSource *CreateEventSource(TGo4EventSourceParameter *par) override;

      TGo4EventElement *CreateOutputEvent() override = 0;

      TGo4EventStore *CreateEventStore(TGo4EventStoreParameter *par) override;

      TGo4EventProcessor *CreateEventProcessor(TGo4EventProcessorParameter *par) override = 0;

      /** For usersubclass: set pointer to user output event. Required by create event store for
        * the TreeStore TTree. */
      void SetOutputEvent(TGo4EventElement *eve) { fxEvent = eve; }

   private:
      /** remember pointer to output event for tree store ctor */
      TGo4EventElement *fxEvent{nullptr}; //!

   ClassDefOverride(TGo4EventServerFactory,1)
};

#endif //TGO4EVENTSERVERFACTORY_H
