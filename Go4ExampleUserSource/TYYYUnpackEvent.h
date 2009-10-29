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

#ifndef TYYYEVENT_H
#define TYYYEVENT_H

#include "TGo4EventElement.h"

class TYYYUnpackEvent : public TGo4EventElement {
   public:
      TYYYUnpackEvent() ;
      TYYYUnpackEvent(const char* name) ;
      virtual ~TYYYUnpackEvent() ;

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *t="");

      Double_t fdR[3]; // final particle radius vector
      Double_t fdV[3]; // final velocity vector
      Double_t fdGam[2]; // projected divergence angle
      Int_t fiNumScatt; // number of scatterings
      Double_t fdEmittX; // pi*x*x'
      Double_t fdEmittY; // pi*y*y'
      Double_t fdEmitt4d; //EmittX*Y

   ClassDef(TYYYUnpackEvent,1)
};
#endif //TYYYEVENT_H



