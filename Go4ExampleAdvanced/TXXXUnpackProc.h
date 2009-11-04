// $Id: TXXXUnpackProc.h 478 2009-10-29 12:26:09Z linev $
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

#ifndef TXXXUNPACKPROCESSOR_H
#define TXXXUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXUnpackEvent;

class TXXXUnpackProc : public TGo4EventProcessor {
   public:
      TXXXUnpackProc();
      TXXXUnpackProc(const char* name);
      virtual ~TXXXUnpackProc();
      virtual Bool_t BuildEvent(TGo4EventElement* dest);
  private:
      TH1           *fCr1Ch[8];
      TH1           *fCr2Ch[8];
      TH2           *fCr1Ch1x2;
      TH1           *fHis1;
      TH1           *fHis1gate;
      TH1           *fHis2;
      TH1           *fHis2gate;
      TGo4WinCond   *fconHis1;
      TGo4WinCond   *fconHis2;
      TGo4WinCond   *fWinCon1;
      TGo4WinCond   *fWinCon2;
      TGo4PolyCond  *fPolyCon1;
      TGo4CondArray *fConArr1;
      TGo4CondArray *fConArr2;
      TXXXParameter *fParam1;
      TXXXParameter *fParam2;
      TGo4Picture   *fPicture1;
      TGo4Picture   *fcondSet;

   ClassDef(TXXXUnpackProc,1)
};

#endif //TXXXUNPACKPROCESSOR_H
