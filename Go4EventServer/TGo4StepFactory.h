// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TUNPACKFACTORY_H
#define TUNPACKFACTORY_H

#include "TGo4EventServerFactory.h"

class TGo4StepFactory : public TGo4EventServerFactory {
  public:
      TGo4StepFactory();
      TGo4StepFactory(const char* name);
      virtual ~TGo4StepFactory();
      virtual  TGo4EventElement* CreateInputEvent();
      virtual  TGo4EventElement* CreateOutputEvent();
      virtual  TGo4EventProcessor* CreateEventProcessor(TGo4EventProcessorParameter* par);
      virtual  TGo4EventSource* CreateEventSource(TGo4EventSourceParameter * par);
      void     DefEventProcessor(const char* Pname, const char* Pclass);
      void     DefInputEvent(const char* Iname, const char* Iclass);
      void     DefOutputEvent(const char* Oname, const char* Oclass);
      void     DefUserEventSource(const char* Sclass);
  private:
      TString fnewProcessor;
      TString fProcessorName;
      TString fnewOutputEvent;
      TString fOutputEventName;
      TString fnewInputEvent;
      TString fInputEventName;
      TString fnewEventSource;

  ClassDef(TGo4StepFactory,2)
};

#endif //TUNPACKFACTORY_H
