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

#ifndef TMESHUNPACKPROCESSOR_H
#define TMESHUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshRawEvent;
class TMeshB1InputEvent;
class TMeshB2InputEvent;
class TMeshB3InputEvent;
class TGo4MbsEvent;

class TMeshUnpackProc : public TGo4EventProcessor {
   public:
      TMeshUnpackProc() ;
      TMeshUnpackProc(const char* name);
      virtual ~TMeshUnpackProc();

      void Unpack(TMeshRawEvent* target);
      void UnpackBranch1(TMeshB1InputEvent* target);
      void UnpackBranch2(TMeshB2InputEvent* target);
      void UnpackBranch3(TMeshB3InputEvent* target);

   private:
      TGo4MbsEvent  *fInput;

   ClassDef(TMeshUnpackProc,1)
};

#endif //TXXXUNPACKPROCESSOR_H
