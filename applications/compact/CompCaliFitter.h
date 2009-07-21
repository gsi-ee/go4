//---------------------------------------------
// Go4 Comp analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCaliFitter_H
#define CompCaliFitter_H

#include "TGo4Parameter.h"
#include "TMath.h"
#define ADC__LINES 6
#define __TEXTMAX__ 256
#define __POLORDER__ 3
#define ADC__DATA "Cali"
#define ADC__GRAPH "CaliGraph"

class TGraph;
class TH1;
class TGo4Fitter;

class CompCaliFitter : public TGo4Parameter {
   public:
      CompCaliFitter();
      CompCaliFitter(const char* name);
      virtual ~CompCaliFitter();

      void Setup(TGraph* curve);
      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);
      /* Read calibration lines from database (ascii file)  */
      void ReadDatabase();

      TString LineFile;     // Filename for ascii file with linesname - energy
      /** to do the fit in UpdateFrom must be true, will be set false  */
      Bool_t DoFit; //to do the fit in UpdateFrom must be true, will be set false
      /** Fitter to search lines*/
      TGo4Fitter* LineFitter; // Fitter to search lines

      /** Fitter for calibration of channel/energies with polynom */
      TGo4Fitter* Calibrator; // Fitter for calibration of channel/energies with polynom

      /** Centroid channel numbers for fitted lines */
      Int_t fiLineChannel[ADC__LINES]; // Centroid channel numbers for fitted lines
      /** Database energies of calibration lines */
      Float_t ffLineEnergy[ADC__LINES]; // Database energies of calibration lines
      /** Database names of calibration lines. */
      TString fxLineName[ADC__LINES]; // Database names of calibration lines.

      /** Reference to graph containing the calibration points */
      TGraph* fxCalibCurve; //! Reference to graph containing the calibration points
      /** Name of the graph to contain the calibration points */
      TString fxGraphName; // Name of the graph to contain the calibration points

     /** Reference to histogram containing the calibration spectrum */
      TH1* fxCalibSpectrum; //! Reference to histogram containing the calibration spectrum

      /** Name of the calibration spectrum histogram */
      TString fxSpectrumName; //Name of the calibration spectrum histogram

      Double_t fdA[__POLORDER__]; // Calibration polynom coeff

   ClassDef(CompCaliFitter,1)
};

#endif
