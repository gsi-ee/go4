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

#ifndef TYYYAnalysis_H
#define TYYYAnalysis_H

#include "TGo4Analysis.h"

class TH1D;
class TYYYRawEvent;
class TYYYUnpackEvent;
class TYYYParameter;

class TYYYAnalysis : public TGo4Analysis {
   public:
      TYYYAnalysis();
      TYYYAnalysis(int argc, char **argv);
      virtual ~TYYYAnalysis();
      Int_t UserPreLoop() override;
      Int_t UserEventFunc() override;
      Int_t UserPostLoop() override;
   private:
      TYYYRawEvent    *fRawEvent{nullptr};
      TYYYUnpackEvent *fUnpackEvent{nullptr};
      TH1D            *fSize{nullptr};
      TYYYParameter   *fPar{nullptr};
      Int_t            fEvents{0};
      Int_t            fLastEvent{0};

   ClassDefOverride(TYYYAnalysis,1)
};

#endif
