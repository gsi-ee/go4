#include "TMeshB1AnlProc.h"

#include "Riostream.h"

#include "TGo4UserException.h"

#include "TMeshRawEvent.h"
#include "TMeshB1OutputEvent.h"
#include "TMeshB1InputEvent.h"

//***********************************************************
// this one is used in TXXXAnlFact
TMeshB1AnlProc::TMeshB1AnlProc(const char* name)
  :TGo4EventProcessor(name),fxInput(0)
{
  cout << "**** TMeshB1AnlProc: Create" << endl;
}
//***********************************************************
TMeshB1AnlProc::TMeshB1AnlProc()
  : TGo4EventProcessor("Processor1"),fxInput(0)
{
}
//***********************************************************
TMeshB1AnlProc::~TMeshB1AnlProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshB1AnlProc::Analysis(TMeshB1OutputEvent* poutevt)
{
   if(poutevt==0) return;
   if(fxInput==0)
      {
         // lazy init for input event from framework
         TGo4EventElement* providerinput=GetInputEvent("Input1Provider");
         TMeshRawEvent* raw=dynamic_cast<TMeshRawEvent*>(providerinput);
         if(raw)
            fxInput=&(raw->fxSub1);
               // provider delivers full raw event, we use only our component
         else
            fxInput=dynamic_cast<TMeshB1InputEvent*>(providerinput);
               // provider with partial io delivers just our component
      }
   if(fxInput)
      {
         //cout <<"Dump of event "<<fxInput->GetName() << endl <<"   ";
         // do the processing here:
         for(Int_t i=0;i<4;i++)
            {
               poutevt->frData[i]=(Float_t)fxInput->fiCrate1[i];
               //cout << fxInput->fiCrate1[i]<<" ";
            }
         //cout << endl;
      }
   else
      {
         throw TGo4UserException(3,"Error: no input event for processor %s",GetName());
      }

} // BuildCalEvent

