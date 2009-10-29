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

#ifndef TGO4EVENTFACTORY_H
#define TGO4EVENTFACTORY_H

#include "TNamed.h"

class TGo4EventElement;
class TGo4EventProcessor;
class TGo4EventStore;
class TGo4EventSource;
class TGo4EventProcessorParameter;
class TGo4EventStoreParameter;
class TGo4EventSourceParameter;

/**
 * Abstract factory for the event related classes. A concrete subclass of this
 * base class is used by the actual analysis to build the experiment specialized
 * classes and structures in the initialization phase. For example, raw event,
 * raw event storage class, raw event source,....
 * Parameter objects of the factory methods may be used to switch between different
 * plug-ins on the fly; definition is left to the user.
 * @stereotype abstractfactory
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4EventFactory : public TNamed {
   public:

      TGo4EventFactory(const char* name);

      TGo4EventFactory();

      virtual ~TGo4EventFactory();

      virtual TGo4EventElement * CreateInputEvent() = 0;

      virtual TGo4EventSource * CreateEventSource(TGo4EventSourceParameter* par) = 0;

      virtual TGo4EventElement * CreateOutputEvent() = 0;

      virtual TGo4EventStore * CreateEventStore(TGo4EventStoreParameter* par) = 0;

      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par) = 0;

   ClassDef(TGo4EventFactory,1)
};

#endif //TGO4EVENTFACTORY_H
