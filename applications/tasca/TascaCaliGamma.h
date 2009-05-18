#ifndef TascaCaliGamma_H
#define TascaCaliGamma_H

#include "TGo4Parameter.h"
#include "TMath.h"
#define GAMMA__LINES 10
#define __TEXTMAX__ 256
#define __POLORDER__ 3
#define GAMMA__DATA "Gamma"
#define GAMMA__GRAPH "GammaGraph"

class TGraph;
class TH1;
class TGo4Fitter;

class TascaCaliGamma : public TGo4Parameter {
   public:
      TascaCaliGamma();
      TascaCaliGamma(const char* name);
      virtual ~TascaCaliGamma();

      void Setup(Text_t *file,TH1* spectrum, TGraph* curve);
      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);
      /* Read calibration lines from database (ascii file)  */
      void ReadDatabase();

      /* calculate energy from channel number using the current polynom  */
      inline Double_t Energy(Int_t channel, Int_t value){
        Double_t result=fdGammaE_a0[channel] +
			value*fdGammaE_a1[channel] +
			value*value*fdGammaE_a2[channel];
        return result;
      }
      /** set filenames for input and output **/
      void SetFiles(Text_t *input,Text_t *output)
		  {if(input)LineFile=input;if(output)CoeffFile=output;}
      /** Fit lines of one histogram, calculate coefficients **/
      void FitLines(TH1* histo, Double_t& a0, Double_t& a1, Double_t& a2);

      /** Save calibration file **/
      Bool_t SaveToFile; // Save calibration file
      /** Filename for ascii file with linesname - energy */
      TString CoeffFile;     // Filename for ascii file with coefficients
      /** Set to kTRUE to make calibration fit in update from */
      Bool_t Recalibrate;       // Set to kTRUE to make calibration fit in update from
      /** Set to kTRUE to re-read energies from external ascii file */
      Bool_t ReadLineFile;   // Set to kTRUE to re-read energies from external ascii file
      /** Filename for ascii file with linesname - energy */
      TString LineFile;     // Filename for ascii file with linesname - energy
      /** Fitter to search lines*/
      TGo4Fitter* LineFitter; // Fitter to search lines

      /** Fitter for calibration of channel/energies with polynom */
      TGo4Fitter* Calibrator; // Fitter for calibration of channel/energies with polynom

      /** Centroid channel numbers for fitted lines */
      Int_t fiLineChannel[GAMMA__LINES]; // Centroid channel numbers for fitted lines
      /** Database energies of calibration lines */
      Float_t ffLineEnergy[GAMMA__LINES]; // Database energies of calibration lines
      /** Database names of calibration lines. */
      TString fxLineName[GAMMA__LINES]; // Database names of calibration lines.

      /** Reference to graph containing the calibration points */
      TGraph* fxCalibCurve; //! Reference to graph containing the calibration points
      /** Name of the graph to contain the calibration points */
      TString fxGraphName; // Name of the graph to contain the calibration points

     /** Reference to histogram containing the calibration spectrum */
      TH1* fxCalibSpectrum; //! Reference to histogram containing the calibration spectrum

      /** Name of the calibration spectrum histogram */
      TString fxSpectrumName; //Name of the calibration spectrum histogram

      /** Calibration polynom coeff */
      Double_t fdGammaE_a0[8];
      Double_t fdGammaE_a1[8];
      Double_t fdGammaE_a2[8];
      Double_t fdGammaT_a0[8];
      Double_t fdGammaT_a1[8];
      Double_t fdGammaT_a2[8];

      Double_t fdA[__POLORDER__]; // Calibration polynom coeff

   ClassDef(TascaCaliGamma,1)
};

#endif
