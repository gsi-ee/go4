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

#include "TGo4StepFactory.h"

#include "Riostream.h"

#include "TClass.h"
#include "TROOT.h"
#include "TString.h"

#include "TGo4EventProcessor.h"
#include "TGo4EventElement.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4UserSourceParameter.h"

//***********************************************************
TGo4StepFactory::TGo4StepFactory() :
   TGo4EventServerFactory(),
   fnewProcessor(),
   fProcessorName(),
   fnewOutputEvent(),
   fOutputEventName(),
   fnewInputEvent(),
   fInputEventName(),
   fnewEventSource()
{
   cout << "**** Create factory " << endl;
}


//***********************************************************
TGo4StepFactory::TGo4StepFactory(const char* name) :
   TGo4EventServerFactory(name),
   fnewProcessor(),
   fProcessorName(),
   fnewOutputEvent(),
   fOutputEventName(),
   fnewInputEvent(),
   fInputEventName()
{
   cout << "GO4-*> Create factory " << name << endl;
   fnewInputEvent = "";
   fnewOutputEvent = "";
   fnewProcessor = "";
   fInputEventName = "for MBS";
   fOutputEventName = "";
   fProcessorName = "";
}

//***********************************************************
TGo4StepFactory::~TGo4StepFactory()
{
   cout << "GO4-*> Delete factory " << GetName() << endl;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefEventProcessor(const char* Pname, const char* Pclass)
{
   TString ptrname=Pname;   // if processorname(eventname) matches tree subbranchname, we
   ptrname.ReplaceAll(".",1,"x",1); //  have to exchange dots in variable name.
   //cout <<"DefEventProcessor has pointername: "<<ptrname.Data() << endl;
   //fnewProcessor.Form("%s * %s = new %s(\"%s\");gROOT->Add(%s);",Pclass,ptrname.Data(),Pclass,Pname,ptrname.Data());
   // need not to register object, because it is done by Go4 framework
   fnewProcessor.Form("new %s(\"%s\");",Pclass,ptrname.Data(),Pclass,Pname);
   fProcessorName = Pname;
   //    fProcessorClass = Pclass;
}

//-----------------------------------------------------------
TGo4EventProcessor * TGo4StepFactory::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TGo4EventProcessor * proc = 0;

   // par is the object specified as last argument creating the step in TAnalysis
   // only info we can get is an ID
   cout << "GO4-*> " << GetName() << ": Create event processor " << fProcessorName << endl;
   if(fnewProcessor.Length() == 0)
      cout << "No event processor was specified!" << endl;
   else
      // create event processor by macro
      proc=(TGo4EventProcessor *)gROOT->ProcessLineSync(fnewProcessor.Data());
   if(proc == 0)
      cout << "Cannot find event processor: " << fProcessorName << endl;
   return proc;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefOutputEvent(const char* Oname, const char* Oclass)
{
   // need not to register object, because it is done by Go4 framework
   fnewOutputEvent.Form("new %s(\"%s\");",Oclass,Oname,Oclass,Oname);
   fOutputEventName = Oname;
   // fOutputEventClass = Oclass;
}

//-----------------------------------------------------------
TGo4EventElement * TGo4StepFactory::CreateOutputEvent()
{
   TGo4EventElement * Oevent = 0;

   cout << "GO4-*> " << GetName() << ": Create output event " << fOutputEventName << endl;

   if(fnewOutputEvent.Length() == 0)
      cout << "No output event was specified!" << endl;
   else
      Oevent=(TGo4EventElement *)gROOT->ProcessLineSync(fnewOutputEvent.Data());
   if(Oevent == 0)
      cout << "Cannot find output event: " << fOutputEventName << endl;
   return Oevent;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefInputEvent(const char* Iname, const char* Iclass)
{
   TString ptrname=Iname;    // if eventname matches tree subbranchname, we
   ptrname.ReplaceAll(".",1,"x",1); //  have to exchange dots in variable name.
   //cout <<"DefInputEvent has pointername: "<<ptrname.Data() << endl;
   // need not to register object, because it is done by Go4 framework
   fnewInputEvent.Form("new %s(\"%s\");",Iclass,ptrname.Data(),Iclass,Iname);
   fInputEventName = Iname;
   //    fInputEventClass = Iclass;
}

//-----------------------------------------------------------
TGo4EventElement * TGo4StepFactory::CreateInputEvent()
{
   TGo4EventElement * Ievent = 0;

   cout << "GO4-*> " << GetName() << ": Create input event " << fInputEventName << endl;
   if(fnewInputEvent.Length() == 0)
      return (TGo4EventElement*) TGo4EventServerFactory::CreateInputEvent();

   Ievent=(TGo4EventElement *)gROOT->ProcessLineSync(fnewInputEvent.Data());
   if(Ievent == 0) cout << "Cannot find input event: " << fInputEventName << endl;
   return Ievent;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefUserEventSource(const char* Sclass)
{
   fnewEventSource.Form("new %s((%s*)%s);", Sclass, TGo4UserSourceParameter::Class()->GetName(), "%p");
}


//-----------------------------------------------------------
TGo4EventSource* TGo4StepFactory::CreateEventSource(TGo4EventSourceParameter* par)
{
   if ((fnewEventSource.Length()>0) && par->InheritsFrom(TGo4UserSourceParameter::Class())) {

      cout << "GO4-*> " << GetName() << ": Create input source " << fnewEventSource << endl;

      TString arg = TString::Format(fnewEventSource.Data(), par);

      TGo4EventSource* source = (TGo4EventSource*) gROOT->ProcessLineSync(arg.Data());

      if (source) return source;

      cout << "Cannot create event source with cmd: " << fnewEventSource << endl;
   }

   return TGo4EventServerFactory::CreateEventSource(par);

}
