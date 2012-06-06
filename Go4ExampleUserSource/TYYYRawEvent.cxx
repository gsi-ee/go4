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

#include "TYYYRawEvent.h"

#include "go4iostream.h"

#include "TGo4Log.h"

TYYYRawEvent::TYYYRawEvent() :
   TGo4EventElement(),
   fiColumns(0),
   fdData(0),
   fiAllocated(0)
{
   // default ctor is for streamer only, avoid heap objects here!
}

TYYYRawEvent::TYYYRawEvent(const char* name) :
   TGo4EventElement(name),
   fiColumns(0),
   fdData(0),
   fiAllocated(0)
{
}

TYYYRawEvent::~TYYYRawEvent()
{
   if (fdData) {
      delete [] fdData;
      fdData = 0;
   }
}

void TYYYRawEvent::Clear(Option_t *t)
{
   for(Int_t t=0; t<fiColumns;++t)
      fdData[t] = 0.;
}

void TYYYRawEvent::ReAllocate(Int_t newsize)
{
   if (newsize > fiAllocated) {
      fiAllocated = newsize < 16 ? 16 : (newsize + 8);
      cout <<"*** YYYRawEvent reallocating from "<<fiColumns<<" to "<< fiAllocated << endl;
      Double_t* narray=new Double_t[fiAllocated];
      for(Int_t i=0;i<fiColumns;++i)
         narray[i] = fdData[i];
      delete [] fdData;
      fdData = narray;
      fiColumns = newsize;
   } else {
      if (newsize == 0) {
         delete [] fdData;
         fdData = 0;
         fiAllocated = 0;
      }
      fiColumns = newsize;
   }
}

void TYYYRawEvent::PrintEvent()
{
   TGo4EventElement::PrintEvent();
   cout<<" YYY Event printout: "<<endl;
   for(Int_t t=0; t<fiColumns;++t)
      cout <<"\t dat("<<t<<")="<<fdData[t]<<endl;
}
