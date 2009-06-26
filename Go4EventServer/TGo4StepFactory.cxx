#include "TGo4StepFactory.h"

#include "Riostream.h"

#include "TROOT.h"
#include "TString.h"

#include "TGo4EventProcessor.h"
#include "TGo4EventElement.h"

//***********************************************************
TGo4StepFactory::TGo4StepFactory() :
   TGo4EventServerFactory(),
   fnewProcessor(),
   fProcessorName(),
   fnewOutputEvent(),
   fOutputEventName(),
   fnewInputEvent(),
   fInputEventName()
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
  if(fnewProcessor.Length() == 0)cout << "No event processor was specified!" << endl;
  else {
    // create event processor by macro
   proc=(TGo4EventProcessor *)gROOT->ProcessLine(fnewProcessor.Data());
    // get pointer to event processor
    //proc = (TGo4EventProcessor *)gROOT->FindObject(fProcessorName.Data());
    // remove event processor from global ROOT (otherwise delete would crash)
    //gROOT->RecursiveRemove(proc);
  }
  if(proc == 0) cout << "Cannot find event processor: " << fProcessorName << endl;
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
  if(fnewOutputEvent.Length() == 0) cout << "No output event was specified!" << endl;
  else {
   Oevent=(TGo4EventElement *)gROOT->ProcessLine(fnewOutputEvent.Data());
   //Oevent = (TGo4EventElement *)gROOT->FindObject(fOutputEventName.Data());
   //gROOT->RecursiveRemove(Oevent);
  }
   if(Oevent == 0) cout << "Cannot find output event: " << fOutputEventName << endl;
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
  if(fnewInputEvent.Length() == 0) return (TGo4EventElement *)TGo4EventServerFactory::CreateInputEvent();
  else {
   Ievent=(TGo4EventElement *)gROOT->ProcessLine(fnewInputEvent.Data());
   //Ievent = (TGo4EventElement *)gROOT->FindObject(fInputEventName.Data());
   //gROOT->RecursiveRemove(Ievent);
  }
   if(Ievent == 0) cout << "Cannot find input event: " << fInputEventName << endl;
   return Ievent;
}
