// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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
   //std::cout << "Dummy, should be reimplemented " << std::endl; // but must not if this is the top beamtime event JAM
}


Bool_t TGo4CompositeProcessor::BuildEvent(TGo4EventElement* outevnt)
{
   // if there is no container event as output, subclass may run in standalone mode
   InitEvent(outevnt); // for plain subclass mode

   // first need to call SetEvent of all subprocessors to set the input/output structures:

   for (Int_t n=0;n<=fSubProcessors.GetLast();n++) {
      TGo4EventProcessor* proc = (TGo4EventProcessor*) fSubProcessors[n];
      proc->SetInputEvent(GetInputEvent()); //forward input to subprocessors
      proc->InitEvent(outevnt); // subprocessors may set own eventpointers here
   } // while

   // treat mbs input if we are in the first step:
   fMbsInput = dynamic_cast<TGo4MbsEvent*>(GetInputEvent());

//   printf("TGo4CompositeProcessor::BuildEvent this = %p name = %s evnt = %p \n", this, GetName(), fMbsInput);

   if(fMbsInput)
   {
//      if(fMbsInput->GetTrigger() > 11) {
//         std::cout << "**** TGo4CompositeProcessor: Skip trigger event"<< std::endl;
//         return kFALSE;
////}

      fMbsTriggerNumber = fMbsInput->GetTrigger();
      fMbsEventNumber = fMbsInput->GetCount();

      TGo4MbsSubEvent* psubevt = 0;
      fMbsInput->ResetIterator();
      while((psubevt = fMbsInput->NextSubEvent()) != 0)
      { // loop over subevents
         ProcessSubevent(psubevt); // process in our own subclass, if implemented

         for (Int_t n=0;n<=fSubProcessors.GetLast();n++) {

            TGo4CompositeProcessor* subcomp = dynamic_cast<TGo4CompositeProcessor*> (fSubProcessors[n]);

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

   for (Int_t n=0;n<=fSubProcessors.GetLast();n++) {
      TGo4EventProcessor* proc = (TGo4EventProcessor*) fSubProcessors[n];
      proc->FinalizeEvent(); // actions implemented in component subclass
   } // while proc

   return kTRUE;
}

