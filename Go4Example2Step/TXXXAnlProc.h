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

#ifndef TXXXANLPROCESSOR_H
#define TXXXANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXCalibPar;
class TXXXAnlEvent;
class TGo4Fitter;

class TXXXAnlProc : public TGo4EventProcessor {
   public:
      TXXXAnlProc() ;
      TXXXAnlProc(const char * name);
      virtual ~TXXXAnlProc();

      virtual Bool_t BuildEvent(TGo4EventElement* dest);

      TH1              *fSum1;
      TH1              *fSum2;
      TH1              *fSum3;
      TH1              *fCaliSum1;
      TGraph           *fCaligraph;

      TXXXParameter    *fParam1;
      TXXXParameter    *fParam2;
      TXXXCalibPar     *fCalipar;
      TGo4WinCond      *fWinCon;

      TGo4Fitter       *fFitter;
      Int_t             fFitCounter;
      TH1              *fFitSrc;
      TH1              *fFitRes;

   ClassDef(TXXXAnlProc, 1)
};
#endif //TXXXANLPROCESSOR_H
