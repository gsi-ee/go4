
#include "THadaqUnpackEvent.h"

UInt_t    THadaqUnpackEvent::ConfigTRBS[HAD_TIME_NUMBOARDS] = SET_TRBS;


//***********************************************************
THadaqUnpackEvent::THadaqUnpackEvent() :
   TGo4EventElement()
{
}
//***********************************************************
THadaqUnpackEvent::THadaqUnpackEvent(const char* name) :
   TGo4EventElement(name)
{
}
//***********************************************************
THadaqUnpackEvent::~THadaqUnpackEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
void  THadaqUnpackEvent::Clear(Option_t *t)
{
#ifdef HAD_USE_TDC
	for(int tdc=0;tdc<HAD_TDC_NUMTDC;++tdc)
	  {
		for(int ch=0;ch<HAD_TDC_CHANNELS;++ch)
			  {
				fTDC_Leading[tdc][ch].clear();
				fTDC_Trailing[tdc][ch].clear();
			  }
	  }
#endif

#ifdef HAD_USE_CAHIT
  for (int trb = 0; trb < HAD_TIME_NUMBOARDS; ++trb)
    {
      if(!AssertTRB(trb)) continue;
      for (int tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc)
        {
          for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch)
            {
              fLeadingCoarseTime[trb][tdc][ch].clear();
              fLeadingFineTime[trb][tdc][ch].clear();
              fTrailingCoarseTime[trb][tdc][ch].clear();
              fTrailingFineTime[trb][tdc][ch].clear();
            }
        }

    }
#endif

}
