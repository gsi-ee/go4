//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaEvent_H
#define TascaEvent_H
#include "TascaCheckEvent.h"


class TascaEvent : public TObject {
   public:
      TascaEvent();
      TascaEvent(const char* name);
      virtual ~TascaEvent();

      Int_t PrintEvent();
      Bool_t Copy(TascaCheckEvent *);

      UInt_t fiEventNumber;
      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;

      UInt_t fiSystemMysec;
      UInt_t fiTimeStamp;
      UInt_t fiGammaMysec;
      UInt_t fiGammaChannelTime[7];
      UInt_t fiDeltaSystemTime;
      UInt_t fiDeltaGammaMysec;
      UInt_t fiDeltaTime;
      UInt_t fiGammaMulti;
     // index of maximum hit, if we had more than one hit
      UInt_t fiStopXLhitI;
      UInt_t fiStopXHhitI;
      UInt_t fiStopYLhitI;
      UInt_t fiStopYHhitI;
      UInt_t fiBackHhitI;
      UInt_t fiBackLhitI;
      UInt_t fiVetoHhitI;
      UInt_t fiVetoLhitI;
      // value of maximum hit, if we had more than one hit
      Float_t ffStopXLhitV;
      Float_t ffStopXHhitV;
      Float_t ffStopYLhitV;
      Float_t ffStopYHhitV;
      Float_t ffBackHhitV;
      Float_t ffBackLhitV;
      Float_t ffVetoHhitV;
      Float_t ffVetoLhitV;

      Float_t ffGammaKev[7];

   ClassDef(TascaEvent,1)
};

#endif
