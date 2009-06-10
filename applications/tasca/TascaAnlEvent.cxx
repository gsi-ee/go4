//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaAnlEvent.h"

#include "Riostream.h"

#include "TascaAnlProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TascaAnlEvent::TascaAnlEvent()
  :TGo4EventElement(),fxTascaCP(0)
{
}
//***********************************************************
TascaAnlEvent::TascaAnlEvent(TascaAnlEvent *s)
  :TGo4EventElement(),fxTascaCP(0)
{
	if(s){
		for(UInt_t i=0;i<10;i++)fiTest[i]=s->fiTest[i];
	}
}
//***********************************************************
TascaAnlEvent::TascaAnlEvent(const char * name)
  :TGo4EventElement(name),fxTascaCP(0),fxTascaFS(0)
{
    cout << "Tasca> TascaAnlEvent "<<name<<" created"<< endl;
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
  // or is it used from Check step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaCaliEvent init for Next step"<< endl;
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
   if(fxTascaFS)fxTascaFS->BuildEvent(this); // method from framework to restore event from file
   return rev;

}
