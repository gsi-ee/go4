//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaCheckEvent.h"

#include "Riostream.h"

#include "TascaCheckProc.h"

//***********************************************************
TascaCheckEvent::TascaCheckEvent()
  :TGo4EventElement(),fxTascaCP(0)
{
}
//***********************************************************
TascaCheckEvent::TascaCheckEvent(const char * name)
  :TGo4EventElement(name),fxTascaCP(0)
{
    cout << "Tasca> TascaCheckEvent: Created"<< endl;
}
//***********************************************************
TascaCheckEvent::~TascaCheckEvent()
{
    cout << "Tasca> TascaCheckEvent: Deleted"<< endl;
}
//***********************************************************


//-----------------------------------------------------------
Int_t TascaCheckEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("TascaCheckProc")){
    fxTascaCP = (TascaCheckProc*)GetEventSource();
    cout << "Tasca> TascaCheckEvent init for Analysis step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TascaCheckEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxTascaCP)fxTascaCP->TascaEventCheck(this);
   return rev;

}
