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

#ifndef TMESHANALYSIS_H
#define TMESHANALYSIS_H

#include "TGo4Analysis.h"

class TGo4MbsEvent;
class TMeshParameter;
class TH1D;

class TMeshAnalysis : public TGo4Analysis {
   public:
      TMeshAnalysis();
      TMeshAnalysis(int argc, char** argv);
      virtual ~TMeshAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:

      TMeshParameter     *fPar;
      TGo4MbsEvent       *fMbsEvent;//!
      TH1D               *fSize;
      Int_t               fEvents;
      Int_t               fLastEvent;

      ClassDef(TMeshAnalysis,1)
};
#endif //TMESHANALYSIS_H



