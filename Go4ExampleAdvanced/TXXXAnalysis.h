// $Id: TXXXAnalysis.h 478 2009-10-29 12:26:09Z linev $
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
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TGo4MbsEvent       *fMbsEvent;
      TXXXUnpackEvent    *fRawEvent;
      TXXXAnlEvent       *fCalEvent;
      TXXXParameter      *fPar;
      TGo4FitterEnvelope *fFitEnvSize;
      TGo4FitterEnvelope *fFitEnvSpectrum;
      TH1D               *fSize;
      Int_t               fEvents;
      Int_t               fLastEvent;

   ClassDef(TXXXAnalysis,1)
};

#endif //TXXXANALYSIS_H



