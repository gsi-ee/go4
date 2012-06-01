
#include "THadaqParam.h"
#include "Riostream.h"

THadaqParam::THadaqParam(const char* name) :
   TGo4Parameter(name)
{
   fillRaw = kTRUE;
   printEvent=kFALSE;
#ifdef HAD_USE_CAHIT
   // default for time difference: use neighbouring channels
  for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b)
      {
      if(!THadaqUnpackEvent::AssertTRB(b)) continue;
   for(int t=0; t<HAD_TIME_NUMTDC; ++t)
      {
         for(int i=0; i<HAD_TIME_CHANNELS; ++i)
         {
		deltaTDC[b][t][i]=t;// by default, channels refer to self tdc
		deltaTRB[b][t][i]=b;// by default, channels refer to self trb

//		imageMCP[b][t][i]=t+1;
//		imageRow[b][t][i]=i+1;
//		imageCol[b][t][i]=i+1;

                 if((i%2)==0)
                         deltaChannels[b][t][i]=i+1;
                 else
                         deltaChannels[b][t][i]=-1;
         }
     }
   }
   deltaChannels[0][0][0]=7; // first test setup
   deltaTDC[0][0][0]=1;  // refer to tdc1, channel 7
   deltaChannels[0][1][7]=0;
   deltaTDC[0][1][7]=0; // back reference
   generalCalibration=kTRUE;
   continuousCalibration=kFALSE;
   calibrationPeriod=HAD_TIME_CALIBFREQ;
#endif
}

Bool_t THadaqParam::UpdateFrom(TGo4Parameter *pp)
{
  THadaqParam* from = dynamic_cast<THadaqParam*> (pp);
   if (from==0) {
      cout << "Wrong parameter object: " << pp->ClassName() << endl;
      return kFALSE;
   }
   cout << "**** THadaqParam:UpdateFrom ... " << endl;
    fillRaw=from->fillRaw;
    printEvent=from->printEvent;
#ifdef HAD_USE_CAHIT
    generalCalibration=from->generalCalibration;
    continuousCalibration=from->continuousCalibration;
    calibrationPeriod=from->calibrationPeriod;

// exclude this for the moment, is not handled correctly by go4 gui JAM
// we therefore need to implement UpdateFrom and suppress changing of these arrays from gui
//    for (int trb = 0; trb < HAD_TIME_NUMBOARDS; ++trb)
//    { if(!THadaqUnpackEvent::AssertTRB(b)) continue;
//      for (int tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc)
//        {
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch)
//            {
//
//              deltaChannels[trb][tdc][ch] = from->deltaChannels[trb][tdc][ch];
//              deltaTDC[trb][tdc][ch] = from->deltaTDC[trb][tdc][ch];
//              deltaTRB[trb][tdc][ch] = from->deltaTRB[trb][tdc][ch];
//              imageMCP[trb][tdc][ch] = from->imageMCP[trb][tdc][ch];
//              imageRow[trb][tdc][ch] = from->imageRow[trb][tdc][ch];
//              imageCol[trb][tdc][ch] = from->imageCol[trb][tdc][ch];
//
//            }
//        }
//    }


#endif


return kTRUE;

     }

