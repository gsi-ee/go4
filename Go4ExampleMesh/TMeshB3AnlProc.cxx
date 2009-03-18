#include "TMeshB3AnlProc.h"

#include "Riostream.h"

#include "TGo4UserException.h"

#include "TMeshRawEvent.h"
#include "TMeshB3OutputEvent.h"
#include "TMeshB3InputEvent.h"

//***********************************************************
// this one is used in TXXXAnlFact
TMeshB3AnlProc::TMeshB3AnlProc(const char* name)
  :TGo4EventProcessor(name),fxInput(0)
{
  cout << "**** TMeshB3AnlProc: Create" << endl;
}
//***********************************************************
TMeshB3AnlProc::TMeshB3AnlProc()
  : TGo4EventProcessor("Processor3"),fxInput(0)
{
}
//***********************************************************
TMeshB3AnlProc::~TMeshB3AnlProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshB3AnlProc::Analysis(TMeshB3OutputEvent* poutevt)
{
   if(fxInput==0)
      {
         // lazy init for input event from framework
         TGo4EventElement* providerinput=GetInputEvent("Input3Provider");
         TMeshRawEvent* raw=dynamic_cast<TMeshRawEvent*>(providerinput);
         if(raw)
            fxInput=&(raw->fxSub3);
               // provider delivers full raw event, we use only our component
         else
            fxInput=dynamic_cast<TMeshB3InputEvent*>(providerinput);
               // provider with partial io delivers just our component
      }
   if(fxInput)
      {
         // do the processing here:
         for(Int_t i=0;i<4;i++)
            {
               poutevt->frData[i]=(Float_t)fxInput->fiCrate3[i];
            }


      }
   else
      {
         throw TGo4UserException(3,"Error: no input event for processor %s",GetName());
      }

} // BuildCalEvent
