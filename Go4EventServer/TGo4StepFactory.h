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
      void     DefEventProcessor(const char* Pname, const char* Pclass);
      void     DefInputEvent(const char* Iname, const char* Iclass);
      void     DefOutputEvent(const char* Oname, const char* Oclass);
  private:
      TString fnewProcessor;
      TString fProcessorName;
//      TString fProcessorClass;
      TString fnewOutputEvent;
      TString fOutputEventName;
//      TString fOutputEventClass;
      TString fnewInputEvent;
      TString fInputEventName;
//      TString fInputEventClass;

  ClassDef(TGo4StepFactory,1)
};

#endif //TUNPACKFACTORY_H
