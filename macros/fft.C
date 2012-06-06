#include "Riostream.h"
using namespace std;

///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script fft.C
//          J.Adamczewski, gsi, 30 May 2012
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// perfroms fft on histogram of name1 using the option as explained in root TVirtualFFT:FFT
/////// Usage:
// The draw flag switches if the results are displayed each time this macro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t fft(const char* name1, Option_t*  opt = "R2C M", Bool_t draw=kTRUE)
{
if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance())
   {
      cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << endl;
      return kFALSE;
   }
TString newname;
TString fullname1 = go4->FindItem(name1);
TObject* ob1=go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms
if(ob1 && ob1->InheritsFrom("TH2")){  // 2d
   cout <<"fft does not support 2d histogram "<<fullname1 << endl;
   return kFALSE;

} else if(ob1 && ob1->InheritsFrom("TH1")){
	TH1* his1=(TH1*)ob1;
	TString n1=his1->GetName();
	TString t1=his1->GetTitle();
	newname.Form("_fft_%s",opt);
	TString finalname=n1+newname;
	TString finaltitle=t1+newname;


	// do fft here:
	 Int_t N = his1->GetNbinsX();
   TH1D* result= new TH1D(finalname, finaltitle,N,0,N);
	result->SetName(finalname);
	result->SetTitle(finaltitle);
	result->Reset("");
	 Double_t *in = new Double_t[N];
	 // since we do not know type of input histo, we copy contents to Double array:
	 for(Int_t ix=0; ix<N;++ix)
	 {
	    in[ix]=his1->GetBinContent(ix+1);
	 }
	 TVirtualFFT *thefft = TVirtualFFT::FFT(1, &N, opt);
	 thefft->SetPoints(in);
	 thefft->Transform();
	    Double_t re, im;
	    for (Int_t i=0; i<N; i++)
	    {
	       thefft->GetPointComplex(i, re, im);
	       result->SetBinContent(i+1,TMath::Sqrt(re*re + im*im));
	    }

} else {
  cout <<"fft could not get histogram "<<fullname1 << endl;
  return kFALSE;
}
result->SetDirectory(0);
rname = go4->SaveToMemory("FFT", result, kTRUE);
cout<< "Saved result histogram to " << rname.Data() <<endl;
if(draw){
	ViewPanelHandle vpanel = go4->StartViewPanel();
	go4->DrawItem(rname, vpanel);
}
return kTRUE;
}
