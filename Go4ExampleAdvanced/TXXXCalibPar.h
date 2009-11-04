// $Id: TXXXCalibPar.h 478 2009-10-29 12:26:09Z linev $
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

#ifndef TXXXCalibPar_H
#define TXXXCalibPar_H

#include "TGo4Parameter.h"

#define __LINESNUMBER__ 20
#define __TEXTMAX__ 256
#define __POLORDER__ 4
#define __DATANAME__ "data1"
#define __GRAPHNAME__ "graph1"

class TGraph;
class TH1;
class TGo4Fitter;

class TXXXCalibPar : public TGo4Parameter {
   public:
      TXXXCalibPar();
      TXXXCalibPar(const char* name, TH1* spectrum, TGraph* curve);
      virtual ~TXXXCalibPar();

      Int_t PrintParameter(Text_t * n, Int_t);
      Bool_t UpdateFrom(TGo4Parameter *);

      /* Read calibration lines from database (ascii file)  */
      void ReadDatabase();

      /* calculate energy from channel number using the current polynom  */
      Double_t Energy(Int_t channel);

      /** Calibration polynom coeff */
      Double_t fdA[__POLORDER__]; // Calibration polynom coeff
      /** Set to kTRUE to make calibration fit in update from */
      Bool_t fbRecalibrate;       // Set to kTRUE to make calibration fit in update from
      /** Set to kTRUE to re-read energies from external ascii file */
      Bool_t fbReadDatabase;   // Set to kTRUE to re-read energies from external ascii file
      /** Filename for ascii file with linesname - energy */
      TString fxDatabase;     // Filename for ascii file with linesname - energy
      /** Centroid channel numbers for fitted lines */
      Int_t fiLinesChannel[__LINESNUMBER__]; // Centroid channel numbers for fitted lines
      /** Database energies of calibration lines */
      Float_t ffLinesEnergy[__LINESNUMBER__]; // Database energies of calibration lines
      /** Database names of calibration lines. */
      TString fxLinesNames[__LINESNUMBER__]; // Database names of calibration lines.

      /** Fitter to search lines*/
      TGo4Fitter* fxLinesFinder; // Fitter to search lines

      /** Fitter for calibration of channel/energies with polynom */
      TGo4Fitter* fxCalibrator; // Fitter for calibration of channel/energies with polynom

      /** Reference to graph containing the calibration points */
      TGraph* fxCalibCurve; //! Reference to graph containing the calibration points
      /** Name of the graph to contain the calibration points */
      TString fxGraphName; // Name of the graph to contain the calibration points

     /** Reference to histogram containing the calibration spectrum */
      TH1* fxCalibSpectrum; //! Reference to histogram containing the calibration spectrum

      /** Name of the calibration spectrum histogram */
      TString fxSpectrumName; //Name of the calibration spectrum histogram

   ClassDef(TXXXCalibPar,1)
};

#endif
