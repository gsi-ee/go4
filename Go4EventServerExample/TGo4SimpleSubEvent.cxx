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

#include "TGo4SimpleSubEvent.h"

#include "TGo4Log.h"

#include "Go4EventServer.h"

Int_t TGo4SimpleSubEvent::fgiZeroField[]={0};


TGo4SimpleSubEvent::TGo4SimpleSubEvent()
:  fsProcid(0), fiFieldLen(0)
{
GO4TRACE((12,"TGo4SimpleSubEvent::TGo4SimpleSubEvent() ",__LINE__, __FILE__));
   //std::cout << "default ctor of simple subevent"<< std::endl;
   //Clear();
   fiD0=0;
   fiD1=0;
   fiD2=0;
   fiD3=0;
   fiD4=0;
   fiD5=0;
   fiD6=0;
   fiD7=0;
   fiD8=0;
   fiD9=0;
   fiD10=0;
   fiD11=0;
   fiD12=0;

// note: clonesarray within clonesarray won't stream! SEGV
//   Int_t subevtnum=7;
//   fxMbsSubEventArray=new TClonesArray("TGo4MbsSubEvent", subevtnum);
//   fxMbsSubEventArray->BypassStreamer(kFALSE);
//
//   TGo4MbsSubEvent* subeve;
//   for (Int_t t=0;t<subevtnum;++t)
//      {
//         //subeve= new( (*fxMbsSubEventArray) [t] ) TGo4MbsSubEvent();
//         fxMbsSubEventArray->New(t);
//      }

//fxTestSub= new TGo4MbsSubEvent(20);
//fxTestSub->Clear();
}



TGo4SimpleSubEvent::~TGo4SimpleSubEvent()
{
GO4TRACE((12,"TGo4SimpleSubEvent::~TGo4SimpleSubEvent() ",__LINE__, __FILE__));

//   delete fxMbsSubEventArray;

}

void TGo4SimpleSubEvent::PrintEvent()
{
   GO4TRACE((11,"TGo4SimpleSubEvent::PrintEvent()",__LINE__, __FILE__));

   TGo4Log::Debug( " SimpleSubEvent Header printout: ");
   TGo4Log::Debug( "\tProcid    %d", fsProcid);
   TGo4Log::Debug( "\tFieldLen    %d ", fiFieldLen);


}
void TGo4SimpleSubEvent::Clear(Option_t *t)
{
   GO4TRACE((11,"TGo4SimpleSubEvent::Clear()",__LINE__, __FILE__));
   void* destfield = (void*) &fiD0;
   void* sourcefield= (void*) TGo4SimpleSubEvent::fgiZeroField;
   memcpy(destfield,
            sourcefield,
               12*sizeof(Int_t));     // does this work?

   fiFieldLen=0;
   //std::cout << "cleared simple subevent"<< std::endl;
}

Int_t TGo4SimpleSubEvent::Fill()
{
   GO4TRACE((11,"TGo4SimpleSubEvent::Fill()",__LINE__, __FILE__));
   MayNotUse("TGo4SimpleSubEvent::Fill()");
   return -1;
}
