#include "TMeshB3InputEvent.h"

#include "Riostream.h"

#include "TMeshUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB3InputEvent::TMeshB3InputEvent()
  :TGo4EventElement("InputEvent3"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3InputEvent::TMeshB3InputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3InputEvent::~TMeshB3InputEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshB3InputEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TMeshUnpackProc")){
    fxProcessor = dynamic_cast<TMeshUnpackProc*>(GetEventSource());
    cout << "**** TMeshB3InputEvent init for Unpack step"<< endl;
  }
  // or is it used from provider step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxFile = dynamic_cast<TGo4FileSource*> (GetEventSource());
    cout << "**** TMeshB3InputEvent init for file input"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TMeshB3InputEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxProcessor)
      fxProcessor->UnpackBranch3(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev=1;
   return rev;
}

//-----------------------------------------------------------
void  TMeshB3InputEvent::Clear(Option_t *t)
{
void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fiCrate3[0];
   memset(destfield,0, sizeof(fiCrate3));
   //cout << "+++ event clear" << endl;
}
