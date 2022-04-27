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

#ifndef TSIMPLEPROCESSOR_H
#define TSIMPLEPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParam;

class TXXXProc : public TGo4EventProcessor {
   public:
      TXXXProc() ;
      TXXXProc(const char* name);
      virtual ~TXXXProc() ;

      Bool_t BuildEvent(TGo4EventElement*); // event processing function

   private:
      TH1           *fCr1Ch[8];
      TH1           *fCr2Ch[8];
      TH2           *fCr1Ch1x2;
      TH1           *fHis1{nullptr};
      TH1           *fHis1gate{nullptr};
      TH1           *fHis2{nullptr};
      TH1           *fHis2gate{nullptr};
      TH1           *fHis3{nullptr};
      Long_t         fHis3Counter{0};
      TGo4WinCond   *fconHis1{nullptr};
      TGo4WinCond   *fconHis2{nullptr};
      TGo4PolyCond  *fPolyCon{nullptr};
      TXXXParam     *fParam{nullptr};
      TGo4Picture   *fcondSet{nullptr};

   ClassDef(TXXXProc,1)
};

#endif //TSIMPLEPROCESSOR_H
