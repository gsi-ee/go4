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

#ifndef TYYYUNPACKPROCESSOR_H
#define TYYYUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TYYYUnpackEvent;
class TYYYParameter;

class TYYYUnpackProc : public TGo4EventProcessor {
   public:
      TYYYUnpackProc() ;
      TYYYUnpackProc(const char* name);
      virtual ~TYYYUnpackProc() ;

      /** This method checks if event class is suited for the source */
      virtual Bool_t CheckEventClass(TClass* cl);

      virtual Bool_t BuildEvent(TGo4EventElement* dest);
   private:
      TH1           *fX;
      TH1           *fY;
      TH1           *fZ;
      TH1           *fVX;
      TH1           *fVY;
      TH1           *fVZ;
      TH1           *fThetaX;
      TH1           *fThetaY;
      TH1           *fThetaR;
      TH1           *fNumScatt;
      TH2           *fXY;
      TH2           *fVXVY;
      TH2           *fXYCond;
      TH2           *fVXVYCond;
      TH2           *fEmitX;
      TH2           *fEmitY;
      TH1           *fEmitDist;
      TGo4WinCond   *fWinConR;
      TGo4WinCond   *fWinConV;
      TGo4PolyCond  *fPolyConEmit;
      TYYYParameter *fParam1;

   ClassDef(TYYYUnpackProc,1)
};

#endif //TYYYUNPACKPROCESSOR_H
