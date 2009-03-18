#include "TMeshRawEvent.h"

#include "Riostream.h"

#include "TMeshUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshRawEvent::TMeshRawEvent()
  :TGo4EventElement("RawEvent"),fxUnpacker(0),fxFile(0)
{
}
//***********************************************************
TMeshRawEvent::TMeshRawEvent(const char* name)
  :TGo4EventElement(name),fxUnpacker(0),fxFile(0)
{
}
//***********************************************************
TMeshRawEvent::~TMeshRawEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshRawEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  if(CheckEventSource("TMeshUnpackProc"))
   {
      fxUnpacker = dynamic_cast<TMeshUnpackProc*>(GetEventSource());
      // for the subcomponents, we have to set unpacker as source ?:
      // only if we would use the subcomponents' Fill() methodsin our Fill()
      //      fxSub1.SetEventSource(fxUnpacker);
      //      fxSub1.Init();
      //      fxSub2.SetEventSource(fxUnpacker);
      //      fxSub1.Init();
      //      fxSub3.SetEventSource(fxUnpacker);
      //      fxSub1.Init();
      cout << "**** TMeshRawEvent init for Unpack step"<< endl;
   }
  else if(CheckEventSource("TGo4FileSource"))
   {
      fxFile = dynamic_cast<TGo4FileSource*> (GetEventSource());
      cout << "**** TMeshRawEvent init for file input"<< endl;
   }
  else
   rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TMeshRawEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxUnpacker)
      {
         fxUnpacker->Unpack(this);  // user event processing method
         // alternatively, we could work with the Fill() of the subcomponents:
         //         fxSub1.Fill();
         //         fxSub2.Fill();
         //         fxSub3.Fill();
      }
   else if(fxFile)
      fxFile->BuildEvent(this); // method from framework to restore event from file
   else
      rev=1;
   return rev;
}

//-----------------------------------------------------------
void  TMeshRawEvent::Clear(Option_t *t)
{
   fxSub1.Clear(t);
   fxSub2.Clear(t);
   fxSub3.Clear(t);
}
