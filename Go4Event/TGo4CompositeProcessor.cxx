#include "TGo4CompositeProcessor.h"

#include "TGo4MbsEvent.h"
#include "TGo4MbsSubEvent.h"


TGo4CompositeProcessor::TGo4CompositeProcessor():
   TGo4EventProcessor(),
   fMbsInput(0),
   fMbsTriggerNumber(0),
   fMbsEventNumber(0),
   fSubProcessors()
{
} // streamer dummy


TGo4CompositeProcessor::TGo4CompositeProcessor(const char* name) :
   TGo4EventProcessor(name),
   fMbsInput(0),
   fMbsTriggerNumber(0),
   fMbsEventNumber(0),
   fSubProcessors()
{
}

TGo4CompositeProcessor::~TGo4CompositeProcessor()
{
   fSubProcessors.Delete();
}

void TGo4CompositeProcessor::AddSubProcessor(TGo4EventProcessor* proc)
{
   if (proc!=0) fSubProcessors.AddLast(proc);
}

void TGo4CompositeProcessor::ProcessSubevent(TGo4MbsSubEvent* subevt)
{
   //cout << "Dummy, should be reimplemented " << endl; // but must not if this is the top beamtime event JAM
}


Bool_t TGo4CompositeProcessor::BuildEvent(TGo4EventElement* outevnt)
{
   // if there is no container event as output, subclass may run in standalone mode
   InitEvent(outevnt); // for plain subclass mode

   // first need to call SetEvent of all subprocessors to set the input/output structures:
   TGo4EventProcessor* proc=0;

   TIter iter(&fSubProcessors);
   while((proc= dynamic_cast<TGo4EventProcessor*> (iter())) !=0)
   {
      proc->SetInputEvent(GetInputEvent()); //forward input to subprocessors
      proc->InitEvent(outevnt); // subprocessors may set own eventpointers here
   } // while

   // treat mbs input if we are in the first step:
   fMbsInput = dynamic_cast<TGo4MbsEvent*>(GetInputEvent());

//   printf("TGo4CompositeProcessor::BuildEvent this = %p name = %s evnt = %p \n", this, GetName(), fMbsInput);

   if(fMbsInput)
   {
//      if(fMbsInput->GetTrigger() > 11) {
//         cout << "**** TGo4CompositeProcessor: Skip trigger event"<<endl;
//         return kFALSE;
////}

      fMbsTriggerNumber = fMbsInput->GetTrigger();
      fMbsEventNumber = fMbsInput->GetCount();

      TGo4MbsSubEvent* psubevt = 0;
      fMbsInput->ResetIterator();
      while((psubevt = fMbsInput->NextSubEvent()) != 0)
      { // loop over subevents
         ProcessSubevent(psubevt); // process in our own subclass, if implemented

         iter.Reset();

         while((proc= dynamic_cast<TGo4EventProcessor*> (iter())) !=0) {

            TGo4CompositeProcessor* subcomp = dynamic_cast<TGo4CompositeProcessor*> (proc);

            if (subcomp) {
               subcomp->fMbsTriggerNumber = fMbsTriggerNumber;
               subcomp->fMbsEventNumber = fMbsEventNumber;
               subcomp->ProcessSubevent(psubevt); // actions implemented in component subclass
            }
         } // while proc
      } // whilesubevents
   } // mbs input


   // in any case, call finalize processing in all subclasses/components:
   // this is the way to implement actions in second step processors
   // for first step processors, this can be used to do actions after all subevents are done
   FinalizeEvent(); // process in our own subclass, if implemented

   iter.Reset();
   while((proc= dynamic_cast<TGo4EventProcessor*> (iter())) !=0)
   {
      proc->FinalizeEvent(); // actions implemented in component subclass
   } // while proc

   return kTRUE;
}

