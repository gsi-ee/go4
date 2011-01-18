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
   fnewProcessor.Form("new %s(\"%s\");", Pclass, Pname);
   fProcessorName = Pname;
}

//-----------------------------------------------------------
TGo4EventProcessor * TGo4StepFactory::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TGo4EventProcessor * proc = 0;

   // par is the object specified as last argument creating the step in TAnalysis
   // only info we can get is an ID
   cout << "GO4-*> " << GetName() << ": Create event processor " << fProcessorName.Data() << endl;
   if(fnewProcessor.Length() == 0)
      cout << "No event processor was specified!" << endl;
   else
      // create event processor by macro
      proc=(TGo4EventProcessor *)gROOT->ProcessLineFast(fnewProcessor.Data());
   if(proc == 0)
      cout << "Cannot create event processor: " << fProcessorName << endl;
   return proc;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefOutputEvent(const char* Oname, const char* Oclass)
{
   // need not to register object, because it is done by Go4 framework
   fnewOutputEvent.Form("new %s(\"%s\");",Oclass,Oname);
   fOutputEventName = Oname;
}

//-----------------------------------------------------------
TGo4EventElement * TGo4StepFactory::CreateOutputEvent()
{
   TGo4EventElement * Oevent = 0;

   cout << "GO4-*> " << GetName() << ": Create output event " << fOutputEventName.Data() << endl;

   if(fnewOutputEvent.Length() == 0)
      cout << "No output event was specified!" << endl;
   else
      Oevent = (TGo4EventElement*) gROOT->ProcessLineSync(fnewOutputEvent.Data());
   if(Oevent == 0)
      cout << "Cannot create output event: " << fOutputEventName.Data() << endl;
   return Oevent;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefInputEvent(const char* Iname, const char* Iclass)
{
   fnewInputEvent.Form("new %s(\"%s\");", Iclass, Iname);
   fInputEventName = Iname;
}

//-----------------------------------------------------------
TGo4EventElement* TGo4StepFactory::CreateInputEvent()
{
   TGo4EventElement * Ievent = 0;

   cout << "GO4-*> " << GetName() << ": Create input event " << fInputEventName.Data() << endl;


   if(fnewInputEvent.Length() == 0)
      return TGo4EventServerFactory::CreateInputEvent();

   Ievent = (TGo4EventElement*) gROOT->ProcessLineSync(fnewInputEvent.Data());
   if(Ievent == 0) cout << "Cannot create input event: " << fInputEventName.Data() << endl;
   return Ievent;
}

//-----------------------------------------------------------
void TGo4StepFactory::DefUserEventSource(const char* Sclass)
{
   #ifdef WIN32
   const char* ptr_arg = "0x%x";
   #else
   const char* ptr_arg = "%p";
   #endif

   fnewEventSource.Form("new %s((%s*)%s);", Sclass, TGo4UserSourceParameter::Class()->GetName(), ptr_arg);
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
