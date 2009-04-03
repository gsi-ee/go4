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
}
//***********************************************************
TascaAnlEvent::~TascaAnlEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TascaAnlEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("TascaAnlProc")){
    fxTascaCP = (TascaAnlProc*)GetEventSource();
    cout << "**** TascaAnlEvent init for Analysis step"<< endl;
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
//-----------------------------------------------------------
void TascaAnlEvent::Clear(Option_t *t)
{
  void* destfield;
  destfield = (void*) &frData[0];
  memset(destfield,0, sizeof(frData));
}
