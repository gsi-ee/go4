//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompAnlEvent.h"

#include "Riostream.h"

#include "CompAnlProc.h"
#include "TGo4FileSource.h"

//***********************************************************
CompAnlEvent::CompAnlEvent()
  :TGo4EventElement(),fxCompCP(0)
{
}
//***********************************************************
CompAnlEvent::CompAnlEvent(CompAnlEvent *s)
  :TGo4EventElement(),fxCompCP(0)
{
	if(s){
		for(UInt_t i=0;i<10;i++)fiTest[i]=s->fiTest[i];
	}
}
//***********************************************************
CompAnlEvent::CompAnlEvent(const char * name)
  :TGo4EventElement(name),fxCompCP(0),fxCompFS(0)
{
    cout << "Comp> CompAnlEvent "<<name<<" created"<< endl;
}
//***********************************************************
CompAnlEvent::~CompAnlEvent()
{
    //cout << "Comp> CompAnlEvent: Deleted"<< endl;
}
//***********************************************************


//-----------------------------------------------------------
Int_t CompAnlEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("CompAnlProc")){
    fxCompCP = (CompAnlProc*)GetEventSource();
    cout << "Comp> CompAnlEvent init for Analysis step"<< endl;
  }
  // or is it used from Check step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxCompFS = (TGo4FileSource*)GetEventSource();
    cout << "Comp> CompCaliEvent init for Next step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t CompAnlEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxCompCP)fxCompCP->CompEventAnalysis(this);
   if(fxCompFS)fxCompFS->BuildEvent(this); // method from framework to restore event from file
   return rev;

}
