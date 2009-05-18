#ifndef TascaCaliAdc_H
#define TascaCaliAdc_H

#include "TGo4Parameter.h"
#include "TMath.h"
#define ADC__LINES 6
#define __TEXTMAX__ 256
#define __POLORDER__ 3
#define ADC__DATA "Adc"
#define ADC__GRAPH "AdcGraph"

class TGraph;
class TH1;
class TGo4Fitter;

class TascaCaliAdc : public TGo4Parameter {
   public:
      TascaCaliAdc();
      TascaCaliAdc(const char* name);
      virtual ~TascaCaliAdc();

      void Setup(Text_t *file,TH1* spectrum, TGraph* curve);
      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);
      /* Read calibration lines from database (ascii file)  */
      void ReadDatabase();

      /* calculate energy from channel number using the current polynom  */
      inline Double_t Energy(Int_t channel){
        Double_t result=0;
        for(Int_t ord=0;ord<__POLORDER__; ++ord)
               result+=fdA[ord]*TMath::Power(channel,ord);
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

      /** Calibration polynom coeff */
      Double_t fdStopXL_a0[144];
      Double_t fdStopXL_a1[144];
      Double_t fdStopXL_a2[144];
      Double_t fdStopXH_a0[144];
      Double_t fdStopXH_a1[144];
      Double_t fdStopXH_a2[144];

      Double_t fdStopYL_a0[96];
      Double_t fdStopYL_a1[96];
      Double_t fdStopYL_a2[96];
      Double_t fdStopYH_a0[96];
      Double_t fdStopYH_a1[96];
      Double_t fdStopYH_a2[96];

      Double_t fdBackL_a0[64];
      Double_t fdBackL_a1[64];
      Double_t fdBackL_a2[64];
      Double_t fdBackH_a0[64];
      Double_t fdBackH_a1[64];
      Double_t fdBackH_a2[64];

      Double_t fdVetoL_a0[16];
      Double_t fdVetoL_a1[16];
      Double_t fdVetoL_a2[16];
      Double_t fdVetoH_a0[16];
      Double_t fdVetoH_a1[16];
      Double_t fdVetoH_a2[16];

      Double_t fdA[__POLORDER__]; // Calibration polynom coeff

   ClassDef(TascaCaliAdc,1)
};

#endif
