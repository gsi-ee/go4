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

#ifndef TYYYUnpackProc_H
#define TYYYUnpackProc_H

#include "TGo4EventProcessor.h"

class TYYYUnpackEvent;
class TYYYParameter;

class TYYYUnpackProc : public TGo4EventProcessor {
   public:
      TYYYUnpackProc();
      TYYYUnpackProc(const char *name);
      virtual ~TYYYUnpackProc();

      /** This method checks if event class is suited for the source */
      Bool_t CheckEventClass(TClass *cl) override;

      Bool_t BuildEvent(TGo4EventElement *dest) override;
   private:
      TH1           *fX{nullptr};
      TH1           *fY{nullptr};
      TH1           *fZ{nullptr};
      TH1           *fVX{nullptr};
      TH1           *fVY{nullptr};
      TH1           *fVZ{nullptr};
      TH1           *fThetaX{nullptr};
      TH1           *fThetaY{nullptr};
      TH1           *fThetaR{nullptr};
      TH1           *fNumScatt{nullptr};
      TH2           *fXY{nullptr};
      TH2           *fVXVY{nullptr};
      TH2           *fXYCond{nullptr};
      TH2           *fVXVYCond{nullptr};
      TH2           *fEmitX{nullptr};
      TH2           *fEmitY{nullptr};
      TH1           *fEmitDist{nullptr};
      TGo4WinCond   *fWinConR{nullptr};
      TGo4WinCond   *fWinConV{nullptr};
      TGo4PolyCond  *fPolyConEmit{nullptr};
      TYYYParameter *fParam1{nullptr};

   ClassDefOverride(TYYYUnpackProc,1)
};

#endif

