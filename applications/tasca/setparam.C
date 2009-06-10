//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------
#include <Riostream.h>

void setparam()
{
	Float_t EvrHmin=7000,EvrHmax=20000;
	Float_t Alpha0Lmin=9800,Alpha0Lmax=10200;
	Float_t Alpha1Lmin=9700,Alpha1Lmax=10100;
	Float_t Alpha2Lmin=8970,Alpha2Lmax=9370;
	Float_t Fission1Hmin=60000, Fission1Hmax=220000;
	Float_t Fission2Hmin=60000, Fission2Hmax=220000;
	Float_t BackHmin=10000, BackHmax=80000;
	TGo4WinCond * wc;
	TascaParameter *fp;
// because the name Parameters is not unique we must fully qualify
 fp = (TascaParameter *) go4->GetObject("Parameters/Parameters","Go4");
 if(fp!=0){
	 fp->shift=5;
	 fp->EventStackSize=10000; // prefix for coefficient files
     fp->AlphaMaxL=16000.; // take low value up to this limit
     fp->AlphaMaxH=30000.;// take high value up to this limit as low
     fp->AlphaTmin=0;
     fp->AlphaTmax=10000000;
     fp->Alpha1Tmin=0;
     fp->Alpha1Tmax=20000000;
     fp->Alpha2Tmin=0;
     fp->Alpha2Tmax=180000000;
     fp->Fission1Tmin=0;
     fp->Fission1Tmax=1000000;
     fp->Fission2Tmin=0;
     fp->Fission2Tmax=60000000;
     fp->PrintParameter();
}
 else cout<<"Parameters not found"<<endl;
 // set conditions
 wc=(TGo4WinCond *) go4->GetObject("Check/EvrH","Go4");
 if(wc){wc->SetValues(EvrHmin,EvrHmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/AlphaL","Go4");
 if(wc){wc->SetValues(Alpha0Lmin,Alpha0Lmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/Alpha1L","Go4");
 if(wc){wc->SetValues(Alpha1Lmin,Alpha1Lmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/Alpha2L","Go4");
 if(wc){wc->SetValues(Alpha2Lmin,Alpha2Lmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/Fission1H","Go4");
 if(wc){wc->SetValues(Fission1Hmin,Fission1Hmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/Fission2H","Go4");
 if(wc){wc->SetValues(Fission2Hmin,Fission2Hmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
 wc=(TGo4WinCond *) go4->GetObject("Check/BackH","Go4");
 if(wc){wc->SetValues(BackHmin,BackHmax);wc->Enable();
  printf("      %-12s: %8.0f %8.0f\n",wc->GetName(),wc->GetXLow(),wc->GetXUp());}
}
