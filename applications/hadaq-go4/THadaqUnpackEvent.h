
#ifndef THadaqEVENT_H
#define THadaqEVENT_H


#include "TGo4EventElement.h"

//#define HAD_USE_TDC 1
#define HAD_USE_CAHIT 1
#define HAD_USE_TDCV3 1

#define HAD_USE_MULTITRB 1
// this will enable trb format with multiple trb board indices


#define HAD_TDC_CHANNELS 32
#define HAD_TDC_NUMTDC 16

#define HAD_TDC_BINS 0x40000

#define HAD_TDC_REFCHANNEL 31


#define HAD_TIME_CHANNELS 32
#define HAD_TIME_NUMTDC 4
#define HAD_TIME_NUMBOARDS 10
// trb id:                 0  1  2  3  4  5  6  7  8  9 
//                         |  |  |  |  |  |  |  |  |  |
#define SET_TRBS           {0, 0, 0, 1, 0, 1, 0, 0, 0, 0}

#define HAD_TIME_COARSEBINS 0x10000
#define HAD_TIME_COARSEUNIT 5000
#define HAD_TIME_COARSEHISTBINS 0x100
#define HAD_TIME_FINEBINS 0x258
#define HAD_TIME_DELTAHISTBINS 200 * HAD_TIME_FINEBINS

#define HAD_TIME_NUMMCP  4
#define HAD_TIME_MCPBINS 9
#define HAD_TIME_MCPRANGE 9

// this number defines how often calibration is performed:
#define HAD_TIME_CALIBFREQ 10000





class THadaqUnpackEvent : public TGo4EventElement {
   public:
      THadaqUnpackEvent() ;
      THadaqUnpackEvent(const char* name) ;
      virtual ~THadaqUnpackEvent() ;

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *t="");

      /* The raw data from first subevent*/
#ifdef HAD_USE_TDC
         /* Leading edge times*/
         std::vector<Int_t> fTDC_Leading[HAD_TDC_NUMTDC][HAD_TDC_CHANNELS];

         /* Trailing edge times*/
         std::vector<Int_t> fTDC_Trailing[HAD_TDC_NUMTDC][HAD_TDC_CHANNELS];
#endif

#ifdef HAD_USE_CAHIT
         std::vector<UShort_t> fLeadingCoarseTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
         std::vector<UShort_t> fLeadingFineTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
         std::vector<UShort_t> fTrailingCoarseTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];
         std::vector<UShort_t> fTrailingFineTime[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS];

#endif
         static Bool_t AssertTRB(unsigned trbid)
             {
                if(trbid >= HAD_TIME_NUMBOARDS) return kFALSE;
                if(THadaqUnpackEvent::ConfigTRBS[trbid]==0) return kFALSE;
                return kTRUE;
             }

         /* array for trb id  configuration*/
            static UInt_t    ConfigTRBS[HAD_TIME_NUMBOARDS];


   ClassDef(THadaqUnpackEvent,1)
};
#endif //THadaqEVENT_H



