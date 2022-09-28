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

#ifndef TXXXUnpackProc_H
#define TXXXUnpackProc_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXUnpackEvent;
class TLatex;

#include "TXXXUnpackEvent.h"

class TXXXUnpackProc : public TGo4EventProcessor {
   public:
      TXXXUnpackProc();
      TXXXUnpackProc(const char *name);
      virtual ~TXXXUnpackProc();

      Bool_t BuildEvent(TGo4EventElement *dest) override;

   protected:
      TH1           *fCr1Ch[XXX_NUM_CHAN];
      TH1           *fCr2Ch[XXX_NUM_CHAN];
      TH2           *fCr1Ch1x2{nullptr};
      TH1           *fHis1{nullptr};
      TH1           *fHis1gate{nullptr};
      TH1           *fHis2{nullptr};
      TH1           *fHis2gate{nullptr};
      TGo4WinCond   *fconHis1{nullptr};
      TGo4WinCond   *fconHis2{nullptr};
      TGo4WinCond   *fWinCon1{nullptr};
      TGo4WinCond   *fWinCon2{nullptr};
      TGo4PolyCond  *fPolyCon1{nullptr};
      TGo4CondArray *fConArr1{nullptr};
      TGo4CondArray *fConArr2{nullptr};
      TGo4ShapedCond  *fEllipseCond{nullptr};
      TGo4ShapedCond  *fCircleCond{nullptr};
      TGo4ShapedCond  *fBoxCond{nullptr};
      TGo4ShapedCond  *fFreestyleCond{nullptr};
      TGo4ListCond     *fWhitelistCon{nullptr};
      TGo4RollingGraph *fRollingGraph1{nullptr};
      TGo4RollingGraph *fRollingGraph2{nullptr};
      TGraph        *fGraph{nullptr};
      TXXXParameter *fParam{nullptr};
      TGo4Picture   *fPicture1{nullptr};
      TGo4Picture   *fcondSet{nullptr};
      TLatex        *fLaText{nullptr};

   ClassDefOverride(TXXXUnpackProc,1)
};

#endif
