#include "TMeshDummyEvent.h"

#include "Riostream.h"

#include "TMeshProviderProc.h"

//***********************************************************
TMeshDummyEvent::TMeshDummyEvent()
  :TGo4EventElement("DummyEvent"),fxProviderProc(0)
{

}
//***********************************************************
TMeshDummyEvent::TMeshDummyEvent(const char* name)
  :TGo4EventElement(name),fxProviderProc(0)
{
}
//***********************************************************
TMeshDummyEvent::~TMeshDummyEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TMeshDummyEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
   Clear();
  if(CheckEventSource("TMeshProviderProc"))
   {
      fxProviderProc = dynamic_cast<TMeshProviderProc*>(GetEventSource());
      //cout << "**** TMeshDummyEvent init"<< endl;
   }
  else
    rev=1;
  return rev;

}
//-----------------------------------------------------------
Int_t TMeshDummyEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   if(fxProviderProc)
      fxProviderProc->SetRealInput();
   else
      rev=1;
   return rev;

}
//-----------------------------------------------------------
void TMeshDummyEvent::Clear(Option_t*)
{

}
