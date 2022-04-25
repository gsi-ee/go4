// $Id: TXXXAnalysis.h 478 2009-10-29 12:26:09Z linev $
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

#ifndef TXXXANALYSIS_H
#define TXXXANALYSIS_H

#include "TGo4Analysis.h"

class TH1D;
class TGo4MbsEvent;
class TXXXUnpackEvent;
class TXXXAnlEvent;
class TXXXParameter;
class TGo4FitterEnvelope;

class TXXXAnalysis : public TGo4Analysis  {
   public:
      TXXXAnalysis();
      TXXXAnalysis(int argc, char** argv);
      virtual ~TXXXAnalysis() ;
      Int_t UserPreLoop() override;
      Int_t UserEventFunc() override;
      Int_t UserPostLoop() override;
   private:
      TGo4MbsEvent       *fMbsEvent{nullptr};
      TXXXUnpackEvent    *fRawEvent{nullptr};
      TXXXAnlEvent       *fCalEvent{nullptr};
      TXXXParameter      *fPar{nullptr};
      TGo4FitterEnvelope *fFitEnvSize{nullptr};
      TGo4FitterEnvelope *fFitEnvSpectrum{nullptr};
      TH1D               *fSize{nullptr};
      Int_t               fEvents{0};
      Int_t               fLastEvent{0};

   ClassDefOverride(TXXXAnalysis,1)
};

#endif //TXXXANALYSIS_H
