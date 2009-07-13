//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------
#include <Riostream.h>

void setparam()
{
// Energy windows MeV
	Float_t EvrHmin     = 4.000,  EvrHmax     = 15.000;
	Float_t Alpha0Lmin  = 9.800,  Alpha0Lmax  = 10.200;
	Float_t Alpha1Lmin  = 9.500,  Alpha1Lmax  = 10.100;
	Float_t Alpha2Lmin  = 8.970,  Alpha2Lmax  =  9.501;
	Float_t Fission1Hmin=60.000,  Fission1Hmax=220.0000;
	Float_t Fission2Hmin=60.000,  Fission2Hmax=220.0000;
	Float_t BackHmin   =-100000,  BackHmax    = 80000;
// Time windows sec
	Float_t fAlphaTmin   =0.,     fAlphaTmax   = 10.;
	Float_t fAlpha1Tmin  =0.,     fAlpha1Tmax  = 20.;
	Float_t fAlpha2Tmin  =0.,     fAlpha2Tmax  =180.; //180
	Float_t fFission1Tmin=0.,     fFission1Tmax=  1.;
	Float_t fFission2Tmin=0.,     fFission2Tmax= 70.;

TGo4WinCond * wc;
TascaParameter *fp;
// because the name Parameters is not unique we must fully qualify
fp = (TascaParameter *) go4->GetObject("Parameters/Parameters","Go4");
if(fp==0) return

	fp->shift=5;               // Unpacker gamma decoder for energies
	fp->Adc80TofMin=300;       // signals Tof (instead of TOF register)
	fp->AdcThreshold=100;      // Unpacker uses this as minimum raw value
	fp->AdcThresholdStop=100;  // Unpacker uses this as minimum raw value
	fp->AdcMax=3500;           // Unpacker uses this as maximum raw value
	fp->EventStackSize=100000; // used in Analysis
	fp->AlphaMaxL=16000.;      // Calibrator take low value up to this limit. Above
	fp->AlphaMaxH=30000.;      // take high value up to this limit as low
	fp->AlphaMinL=1000.;       // Unpacker uncalibrated minimum value for alpha
	fp->AlphaMinH=1000.;       // Unpacker uncalibrated minimum value for alpha

// -----------------Do not change below ---------------------
fp->AlphaTmin   =(UInt_t)fAlphaTmin*1000000;
fp->AlphaTmax   =(UInt_t)fAlphaTmax*1000000;
fp->Alpha1Tmin  =(UInt_t)fAlpha1Tmin*1000000;
fp->Alpha1Tmax  =(UInt_t)fAlpha1Tmax*1000000;
fp->Alpha2Tmin  =(UInt_t)fAlpha2Tmin*1000000;
fp->Alpha2Tmax  =(UInt_t)fAlpha2Tmax*1000000;
fp->Fission1Tmin=(UInt_t)fFission1Tmin*1000000;
fp->Fission1Tmax=(UInt_t)fFission1Tmax*1000000;
fp->Fission2Tmin=(UInt_t)fFission2Tmin*1000000;
fp->Fission2Tmax=(UInt_t)fFission2Tmax*1000000;
fp->PrintParameter();
// set conditions
wc=(TGo4WinCond *) go4->GetObject("Check/EvrH","Go4");
if(wc){wc->SetValues(EvrHmin*1000,EvrHmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/AlphaL","Go4");
if(wc){wc->SetValues(Alpha0Lmin*1000,Alpha0Lmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/Alpha1L","Go4");
if(wc){wc->SetValues(Alpha1Lmin*1000,Alpha1Lmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/Alpha2L","Go4");
if(wc){wc->SetValues(Alpha2Lmin*1000,Alpha2Lmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/Fission1H","Go4");
if(wc){wc->SetValues(Fission1Hmin*1000,Fission1Hmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/Fission2H","Go4");
if(wc){wc->SetValues(Fission2Hmin*1000,Fission2Hmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
wc=(TGo4WinCond *) go4->GetObject("Check/BackH","Go4");
if(wc){wc->SetValues(BackHmin*1000,BackHmax*1000);wc->Enable();
printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
}
