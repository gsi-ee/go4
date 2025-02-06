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

#ifndef TGO4STEPFACTORY_H
#define TGO4STEPFACTORY_H

#include "TGo4EventServerFactory.h"

/** @brief Step factory
 * @ingroup go4_major_classes
 * @ingroup go4_event
 */
class TGo4StepFactory : public TGo4EventServerFactory {
  public:
      TGo4StepFactory();
      TGo4StepFactory(const char *name);
      virtual ~TGo4StepFactory();
      TGo4EventElement *CreateInputEvent() override;
      TGo4EventElement *CreateOutputEvent() override;
      TGo4EventProcessor *CreateEventProcessor(TGo4EventProcessorParameter *par) override;
      TGo4EventSource *CreateEventSource(TGo4EventSourceParameter *par) override;
      TGo4EventStore *CreateEventStore(TGo4EventStoreParameter *par) override;
      void     DefEventProcessor(const char *Pname, const char *Pclass);
      void     DefInputEvent(const char *Iname, const char *Iclass);
      void     DefOutputEvent(const char *Oname, const char *Oclass);
      void     DefUserEventSource(const char *Sclass);
      void     DefUserEventStore(const char *Sclass);
  private:
      TString fnewProcessor;
      TString fProcessorName;
      TString fnewOutputEvent;
      TString fOutputEventName;
      TString fnewInputEvent;
      TString fInputEventName;
      TString fnewEventSource;
      TString fnewEventStore;

  ClassDefOverride(TGo4StepFactory,3)
};

#endif // TGO4STEPFACTORY_H
