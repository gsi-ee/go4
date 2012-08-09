
#ifndef THadaqUNPACKPROCESSOR_H
#define THadaqUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "THadaqUnpackEvent.h"
#include "THadaqParam.h"
#include "THadaqEventSource.h"

#include "TH1.h"
#include "TH2.h"
#include "TGo4WinCond.h"


class THadaqUnpackProc : public TGo4EventProcessor {
   public:
      THadaqUnpackProc() ;
      THadaqUnpackProc(const char* name);
      virtual ~THadaqUnpackProc() ;

      /** This method checks if event class is suited for the source */
      virtual Bool_t CheckEventClass(TClass* cl);

      virtual Bool_t BuildEvent(TGo4EventElement* dest);

   protected:

      /* Called for each subevent in the current event*/
      void ProcessSubevent(Hadaq_Subevent* sub);

      /* Here implementation for TDC subevent*/
      void ProcessTDC(Hadaq_Subevent* sub);

      /* Unpacking of Cahits time test subevent with old data format*/
      void ProcessTimeTest(Hadaq_Subevent* sub);

      /* Unpacking of Cahits time test subevent with old data format*/
      void ProcessTimeTestV3(Hadaq_Subevent* sub, Bool_t printoutonly=kFALSE);

      /* process Cahit data independent of data format*/
      void EvaluateTDCData(UShort_t board=0, UShort_t tdc=0);


      /* default printout of data fields if switched on in parameter*/
      void DumpData(Hadaq_Subevent* hadsubevent);






   private:
#ifdef HAD_USE_TDC
      TH1* hLeadingChannelHits[HAD_TDC_NUMTDC];
      TH1* hTrailingChannelHits[HAD_TDC_NUMTDC];

      TH1* hLeadingDeltaT[HAD_TDC_NUMTDC][HAD_TDC_CHANNELS];
      TH1* hTrailingDeltaT[HAD_TDC_NUMTDC][HAD_TDC_CHANNELS];
      TH1* hLeadingDeltaTAll;
      TH1* hTrailingDeltaTAll;

      TGo4WinCond* cLeadingDtRange;
      TGo4WinCond* cTrailingDtRange;

      TH2* hLeadingDtAllChans;
      TH2* hTrailingDtAllChans;
      TH2* hDeltaDtAllChans;
#endif
#ifdef HAD_USE_CAHIT
      TH1* hTriggerCount[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hChannelCount[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];

      TH1* hLeadingCoarseAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hLeadingFineAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hLeadingDeltaCalAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hTrailingCoarseAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hTrailingFineAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH1* hTrailingDeltaCalAll[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];

      TH1* hLeadingCoarse[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hLeadingFine[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hLeadingFineBuffer[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hLeadingDeltaRaw[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hLeadingDeltaCal[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];

      TH2* hLeadingCorrelFine[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];

      TH1* hTrailingCoarse[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hTrailingFine[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hTrailingFineBuffer[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hTrailingDeltaRaw[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hTrailingDeltaCal[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];

      TH2* hTrailingCorrelFine[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];



      TH2* hLeadingCoarseAllChans[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH2* hLeadingFineAllChans[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH2* hTrailingCoarseAllChans[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TH2* hTrailingFineAllChans[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];

      TH2* hImagingMCP[HAD_TIME_NUMMCP];
      TH2* hImagingMCPGated[HAD_TIME_NUMMCP];

      /* conditions on the absolute times:*/
      TGo4WinCond* cLeadingFineTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TGo4WinCond* cLeadingCoarseTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TGo4WinCond* cTrailingFineTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TGo4WinCond* cTrailingCoarseTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];

      /* conditions on the calibrated delta times:*/
      TGo4WinCond* cLeadingDeltaCalTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TGo4WinCond* cTrailingDeltaCalTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];

      TGo4WinCond* cLeadingDeltaCalDebugTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];
      TGo4WinCond* cTrailingDeltaCalDebugTimeGate[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC];


      /* better keep correction arrays inside histograms, will be stored */
      TH1* hCalcBinWidth[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hCalBinTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hUnCalBinTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
      TH1* hCorrTimeBinsFine[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
/*       TH1* hCorrTimeBinsCoarse[[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC]HAD_TIME_CHANNELS]; */


//      Double_t GetCorrectedFineTime(UShort_t raw, UShort_t chan);
//      Double_t GetCorrectedCoarseTime(UShort_t raw, UShort_t chan);

      void DoCalibration(UShort_t board, UShort_t tdc, UShort_t ch, Bool_t generalcal, Bool_t contcal);

      /* flag to indicate that no calibration has been done so far*/
      Bool_t fbHasCalibration;

#endif


      Hadaq_Subevent* fInputSub;

      THadaqParam* fPar;
      THadaqUnpackEvent* fOutEvent;

      unsigned fEventCount;

   ClassDef(THadaqUnpackProc,1)
};

#endif //THadaqUNPACKPROCESSOR_H
