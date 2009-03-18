#include "TMeshB3OutputEvent.h"

#include "Riostream.h"

#include "TMeshB3AnlProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB3OutputEvent::TMeshB3OutputEvent()
  :TGo4EventElement("OutputEvent3"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3OutputEvent::TMeshB3OutputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3OutputEvent::~TMeshB3OutputEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TMeshB3OutputEvent::Init()
{
   // check for different source types
   Int_t rev = 0;
   if(CheckEventSource("TMeshB3AnlProc"))
      {
         fxProcessor = dynamic_cast<TMeshB3AnlProc*>(GetEventSource());
         cout << "**** TMeshB3OutputEvent Init for analysis step"<< endl;
      }
   else if(CheckEventSource("TGo4FileSource"))
      {
         fxFile = dynamic_cast<TGo4FileSource*>(GetEventSource());
         cout << "**** TMeshB3OutputEvent Init for file input"<< endl;
      }
   else
      rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TMeshB3OutputEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxProcessor)
      fxProcessor->Analysis(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev = 1;
   return rev;

}
//-----------------------------------------------------------
void TMeshB3OutputEvent::Clear(Option_t *t)
{
//-----------------------------------------------------------
void* destfield;
   destfield = (void*) &frData[0];
   memset(destfield,0, sizeof(frData));
}

