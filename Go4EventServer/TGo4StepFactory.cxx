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

#include "TClass.h"
#include "TROOT.h"
#include "TString.h"

#include "TGo4Log.h"
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
   TGo4Log::Debug("Create factory");
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
   TGo4Log::Debug("Create factory %s", name);
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
   TGo4Log::Debug("Delete factory %s", GetName());
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
   TGo4Log::Info("%s: Create event processor %s", GetName(), fProcessorName.Data());
   if(fnewProcessor.Length() == 0)
      TGo4Log::Error("No event processor was specified!");
   else
      // create event processor by macro
      proc=(TGo4EventProcessor *)gROOT->ProcessLineFast(fnewProcessor.Data());
   if(proc == 0)
      TGo4Log::Error("Cannot create event processor: %s", fProcessorName.Data());
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

   TGo4Log::Info("%s: Create output event %s", GetName(), fOutputEventName.Data());

   if(fnewOutputEvent.Length() == 0)
      TGo4Log::Error("No output event was specified!");
   else
      Oevent = (TGo4EventElement*) gROOT->ProcessLineSync(fnewOutputEvent.Data());
   if(Oevent == 0)
      TGo4Log::Error("Cannot create output event: %s", fOutputEventName.Data());
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

   TGo4Log::Info("%s: Create input event %s", GetName(), fInputEventName.Data());

   if(fnewInputEvent.Length() == 0)
      return TGo4EventServerFactory::CreateInputEvent();

   Ievent = (TGo4EventElement*) gROOT->ProcessLineSync(fnewInputEvent.Data());
   if(Ievent == 0)
      TGo4Log::Error("Cannot create input event: %s", fInputEventName.Data());
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

      TGo4Log::Info("%s: Create input source %s", GetName(), fnewEventSource.Data());

      TString arg = TString::Format(fnewEventSource.Data(), par);

      TGo4EventSource* source = (TGo4EventSource*) gROOT->ProcessLineSync(arg.Data());

      if (source) return source;

      TGo4Log::Error("Cannot create event source with cmd: %s", fnewEventSource.Data());
   }

   return TGo4EventServerFactory::CreateEventSource(par);
}
