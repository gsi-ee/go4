// $Id: THadaqParam.h 557 2010-01-27 15:11:43Z linev $
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

#ifndef THadaqParam_H
#define THadaqParam_H

#include "TGo4Parameter.h"
#include "THadaqUnpackEvent.h"

#pragma pack(push, 1)

class THadaqParam : public TGo4Parameter {
   public:
      THadaqParam(const char* name);
      THadaqParam() {}

      Bool_t   fillRaw;  // control filling of raw histograms
      Bool_t printEvent; // control printout of event contents
#ifdef HAD_USE_CAHIT
      Bool_t printDebugGate; // switch data printout when inside dt debug gate

      Short_t deltaChannels[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // specifies for each channel the corresponding reference channel for delta time
      Short_t deltaTDC[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // reference tdc for each channel for delta time
      Short_t deltaTRB[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // reference trb board for each channel for delta time

      Int_t imageMCP[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // relative MCP for data imaging for each channel
      Int_t imageRow[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // relative row for data imaginf for each channel
      Int_t imageCol[HAD_TIME_NUMBOARDS][HAD_TIME_NUMTDC][HAD_TIME_CHANNELS]; // relative column for data imaginf for each channel
      Bool_t generalCalibration; // switch calibration of time bins on
      Bool_t continuousCalibration; // continuous calibration with doublebuffered table
      UInt_t calibrationPeriod; // number of events between 2 time calibrations
#endif
     Bool_t UpdateFrom(TGo4Parameter *pp);

   ClassDef(THadaqParam,2)
};

#pragma pack(pop)
#endif // THadaqParam_H
