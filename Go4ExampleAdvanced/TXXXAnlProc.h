// $Id: TXXXAnlProc.h 478 2009-10-29 12:26:09Z linev $
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

#ifndef TXXXANLPROCESSOR_H
#define TXXXANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXCalibPar;
class TXXXAnlEvent;
class TGo4Fitter;

class TXXXAnlProc : public TGo4EventProcessor {
   public:
      TXXXAnlProc();
      TXXXAnlProc(const char *name);
      virtual ~TXXXAnlProc();

      Bool_t BuildEvent(TGo4EventElement* dest) override;

      TH1              *fSum1{nullptr};
      TH1              *fSum2{nullptr};
      TH1              *fSum3{nullptr};
      TH1              *fCaliSum1{nullptr};
      TGraph           *fCaligraph{nullptr};

      TXXXParameter    *fParam1{nullptr};
      TXXXParameter    *fParam2{nullptr};
      TXXXCalibPar     *fCalipar{nullptr};
      TGo4WinCond      *fWinCon{nullptr};

      TGo4Fitter       *fFitter{nullptr};
      Int_t             fFitCounter{0};
      TH1              *fFitSrc{nullptr};
      TH1              *fFitRes{nullptr};

   ClassDefOverride(TXXXAnlProc, 1)
};

#endif //TXXXANLPROCESSOR_H
