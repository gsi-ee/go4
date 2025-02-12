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

/** @brief Ready to use step factory
 * @details One can configure input/output events and event processors per class names
 * Used in most go4 examples
 * @ingroup go4_major_classes
 * @ingroup go4_event
 */
class TGo4StepFactory : public TGo4EventServerFactory {
public:
   /** @brief Default constructor */
   TGo4StepFactory();
   /** @brief Normal constructor */
   TGo4StepFactory(const char *name);
   /** @brief destructor */
   virtual ~TGo4StepFactory();
   /** @brief Create input event */
   TGo4EventElement *CreateInputEvent() override;
   /** @brief Create output event */
   TGo4EventElement *CreateOutputEvent() override;
   /** @brief Create event processor event */
   TGo4EventProcessor *CreateEventProcessor(TGo4EventProcessorParameter *par) override;
   /** @brief Create event source */
   TGo4EventSource *CreateEventSource(TGo4EventSourceParameter *par) override;
   /** @brief Create event store */
   TGo4EventStore *CreateEventStore(TGo4EventStoreParameter *par) override;
   /** @brief Configure name and class name for event processor */
   void DefEventProcessor(const char *Pname, const char *Pclass);
   /** @brief Configure name and class name for input event */
   void DefInputEvent(const char *Iname, const char *Iclass);
   /** @brief Configure name and class name for output event */
   void DefOutputEvent(const char *Oname, const char *Oclass);
   /** @brief Configure class name for event source */
   void DefUserEventSource(const char *Sclass);
   /** @brief Configure class name for event store */
   void DefUserEventStore(const char *Sclass);

private:
   TString fnewProcessor;    ///< command to create new processor
   TString fProcessorName;   ///< name for new processor
   TString fnewOutputEvent;  ///< command to create new output event
   TString fOutputEventName; ///< name for new output event
   TString fnewInputEvent;   ///< command to create new input event
   TString fInputEventName;  ///< name for new input event
   TString fnewEventSource;  ///< command to create new event source
   TString fnewEventStore;   ///< command to create new event store

   ClassDefOverride(TGo4StepFactory, 3)
};

#endif // TGO4STEPFACTORY_H
