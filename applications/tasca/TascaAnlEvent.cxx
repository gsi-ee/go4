//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaAnlEvent.h"

#include "Riostream.h"

#include "TascaAnlProc.h"

//***********************************************************
TascaAnlEvent::TascaAnlEvent()
  :TGo4EventElement(),fxTascaCP(0)
{
}
//***********************************************************
TascaAnlEvent::TascaAnlEvent(const char * name)
  :TGo4EventElement(name),fxTascaCP(0)
{
    cout << "Tasca> TascaAnlEvent: Created"<< endl;
}
//***********************************************************
TascaAnlEvent::~TascaAnlEvent()
{
    cout << "Tasca> TascaAnlEvent: Deleted"<< endl;
}
//***********************************************************


//-----------------------------------------------------------
Int_t TascaAnlEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("TascaAnlProc")){
    fxTascaCP = (TascaAnlProc*)GetEventSource();
    cout << "Tasca> TascaAnlEvent init for Analysis step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TascaAnlEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxTascaCP)fxTascaCP->TascaEventAnalysis(this);
   return rev;

}
