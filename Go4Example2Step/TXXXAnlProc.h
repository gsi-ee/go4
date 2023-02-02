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

#ifndef TXXXAnlProc_H
#define TXXXAnlProc_H

#include "TGo4EventProcessor.h"

#include "TXXXUnpackEvent.h"

class TXXXParameter;
class TXXXAnlEvent;

class TXXXAnlProc : public TGo4EventProcessor {
   public:
      TXXXAnlProc();
      TXXXAnlProc(const char *name);
      virtual ~TXXXAnlProc();

      Bool_t BuildEvent(TGo4EventElement *dest) override;

      TH1              *fSum1{nullptr};
      TH1              *fSum2{nullptr};
      TH1              *fSum3{nullptr};

      TXXXParameter    *fParam{nullptr};
      TGo4WinCond      *fWinCon{nullptr};

   ClassDefOverride(TXXXAnlProc, 1)
};

#endif
