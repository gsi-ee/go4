// $Id: TXXXUnpackProc.h 478 2009-10-29 12:26:09Z linev $
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

class TXXXUnpackProc : public TGo4EventProcessor {
   private:
       TH1           *fCr1Ch[8] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
       TH1           *fCr2Ch[8]= {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
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
       TXXXParameter *fParam1{nullptr};
       TXXXParameter *fParam2{nullptr};
       TGo4Picture   *fPicture1{nullptr};
       TGo4Picture   *fcondSet{nullptr};
       Long_t         fEvCount{0};

       void CreateRawHistograms(int nbins, double xmin, double xmax);

   public:
      TXXXUnpackProc();
      TXXXUnpackProc(const char *name);
      virtual ~TXXXUnpackProc();
      Bool_t BuildEvent(TGo4EventElement* dest) override;

   ClassDefOverride(TXXXUnpackProc,1)
};

#endif
