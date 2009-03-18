#include "TXXXAnlEvent.h"

#include "Riostream.h"

#include "TXXXAnlProc.h"

//***********************************************************
TXXXAnlEvent::TXXXAnlEvent()
  :TGo4EventElement(),fxXXXCP(0)
{
}
//***********************************************************
TXXXAnlEvent::TXXXAnlEvent(const char * name)
  :TGo4EventElement(name),fxXXXCP(0)
{
}
//***********************************************************
TXXXAnlEvent::~TXXXAnlEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TXXXAnlEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("TXXXAnlProc")){
    fxXXXCP = (TXXXAnlProc*)GetEventSource();
    cout << "**** XXXAnlEvent init for Analysis step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TXXXAnlEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxXXXCP)fxXXXCP->XXXEventAnalysis(this);
   return rev;

}
//-----------------------------------------------------------
void TXXXAnlEvent::Clear(Option_t *t)
{
  void* destfield;
  destfield = (void*) &frData[0];
  memset(destfield,0, sizeof(frData));
}
