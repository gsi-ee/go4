// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TYYYRawEvent.h"

#include <iostream>

#include "TGo4Log.h"

TYYYRawEvent::TYYYRawEvent() :
   TGo4EventElement()
{
   // default ctor is for streamer only, avoid heap objects here!
}

TYYYRawEvent::TYYYRawEvent(const char *name) :
   TGo4EventElement(name)
{
}

TYYYRawEvent::~TYYYRawEvent()
{
   if (fdData) {
      delete [] fdData;
      fdData = nullptr;
   }
}

void TYYYRawEvent::Clear(Option_t *)
{
   for (Int_t t = 0; t < fiColumns; ++t)
      fdData[t] = 0.;
}

void TYYYRawEvent::ReAllocate(Int_t newsize)
{
   if (newsize > fiAllocated) {
      fiAllocated = newsize < 16 ? 16 : (newsize + 8);
      TGo4Log::Info("YYYRawEvent reallocating from %d to %d", fiColumns, fiAllocated);
      auto narray = new Double_t[fiAllocated];
      for (Int_t i = 0; i < fiColumns; ++i)
         narray[i] = fdData[i];
      delete [] fdData;
      fdData = narray;
      fiColumns = newsize;
   } else {
      if (newsize == 0) {
         delete [] fdData;
         fdData = nullptr;
         fiAllocated = 0;
      }
      fiColumns = newsize;
   }
}

void TYYYRawEvent::PrintEvent()
{
   TGo4EventElement::PrintEvent();
   std::cout << " YYY Event printout: " << std::endl;
   for (Int_t t = 0; t < fiColumns; ++t)
      std::cout << "\t dat(" << t << ")=" << fdData[t] << std::endl;
}
