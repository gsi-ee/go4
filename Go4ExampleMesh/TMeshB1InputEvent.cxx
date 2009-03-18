#include "TMeshB1InputEvent.h"

#include "Riostream.h"

#include "TMeshUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB1InputEvent::TMeshB1InputEvent()
  :TGo4EventElement("InputEvent1"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB1InputEvent::TMeshB1InputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB1InputEvent::~TMeshB1InputEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshB1InputEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TMeshUnpackProc")){
    fxProcessor = dynamic_cast<TMeshUnpackProc*>(GetEventSource());
    cout << "**** TMeshB1InputEvent init for Unpack step"<< endl;
  }
  // or is it used from provider step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxFile = dynamic_cast<TGo4FileSource*> (GetEventSource());
    cout << "**** TMeshB1InputEvent init for file input"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TMeshB1InputEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxProcessor)
      fxProcessor->UnpackBranch1(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev=1;
   return rev;
}

//-----------------------------------------------------------
void  TMeshB1InputEvent::Clear(Option_t *t)
{
void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fiCrate1[0];
   memset(destfield,0, sizeof(fiCrate1));
   //cout << "+++ event clear" << endl;
}
