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
