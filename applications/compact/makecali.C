//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

// Main function makecali at the end
//
#include <Riostream.h>
#include <stdio.h>
#include <stdarg.h>
#define NUMHIS 7
#define MAXLINES 20
Double_t noisefactor=7.7;
Double_t noise=20.;
Double_t sum=1.;
Double_t linepos[MAXLINES];
Double_t lineposc[MAXLINES];
Double_t energy[MAXLINES];
Int_t i,ii,iii,lines,peaks,energies,hisind;
Int_t minpeaks,maxpeaks,peakdistance;
Double_t sollmin=122.;
Double_t sollmax=1408.;
Double_t offset,factor;
Double_t cal,cal0,cal1,cal2;
Bool_t verbose=kFALSE;
Float_t lolim,hilim;
char nextline[256];
char buf[256];
char full[128];
char dir[128];
char coeffline[128];
TGo4FitModel* model;
TH1* his;
TString hname;
TString fLine;
TObject *namo;
TGo4Fitter* fitter;
TGo4FitPeakFinder* finder;
TGo4Fitter* cali;
TGraph *graph;
TDatime *time;

//-------------------------------------------------------
// function to read calibration files
Bool_t ReadEnergies(char *file)
{
memset(energy,0,sizeof(energy));
// read calibration file
std::ifstream cfile(file);
if(cfile.bad())
   {
      cout <<"Open error of calibration energy file "<< file<<endl;
      return;
   }
cout <<endl<<"CaliGamma energy file "<<file<<endl;
  energies=0;
while(energies < MAXLINES){
	do{
		  cfile.getline(nextline,256,'\n' ); // read whole line
		  if(cfile.eof() || cfile.bad()) break;
	  }while(strstr(nextline,"#") || strstr(nextline,"!") ); // skip any comments
	if(cfile.eof() || cfile.bad()) break;
	sscanf(nextline,"%s %lf",buf, &energy[energies]);
	cout <<" "<<energy[energies];
	energies++;
}
cout<<endl;
cfile.close();
sollmax=energy[energies-1]; // highest energy
}
//-------------------------------------------------------
// function to get list of histograms from folder
TListIter *GetHistograms(TFile *f, char *dir, char *wildcard)
{
	TString fulldir;
	TString fullname;
	TString name;
	TH1I *h;
	TKey *key;
	TRegexp wild(wildcard,kTRUE);

	TList* found = new TList;
	fulldir.Form(dir);
	f->cd(fulldir.Data());
	//namiter(f,fulldir,"*",found);
	TIter next(gDirectory->GetListOfKeys());
	while((key=(TKey*)next()))
	{
		if(strcmp(key->GetClassName(),"TH1I")==0)
		{
			name.Form(key->GetName());
			fullname.Form("%s/%s",fulldir.Data(),key->GetName());
			// if name contains a / wildcard does not work!!!!
		    if(name.Index(wild) != kNPOS)
		    	found->Add((TObject *)f->Get(fullname.Data()));
		}
	}
	TListIter *iter = found->MakeIterator();
	f->cd();
	return iter;
}
//-------------------------------------------------------
// find peaks and fit coefficients for one histogram
Bool_t MakeCalibration(TH1 *his){
char num[128];
char *p;
sprintf(full,"%s/%s",dir,his->GetName());
cout<<full<<endl;
fitter->Clear();
finder = new TGo4FitPeakFinder("Finder", "data1", kFALSE, 1);
finder->SetupForThird(noisefactor, noise, sum);
fitter->AddAction(finder);
//fitter->AddStandardActions();
// with kFALSE fitter does not own histogram
fitter->AddH1("data1", his, kFALSE, lolim, hilim);
fitter->DoActions(kTRUE);
lines=0;
memset(linepos,0,sizeof(linepos));
memset(lineposc,0,sizeof(lineposc));
// count lines
for(ii=0;ii<MAXLINES;ii++){
	model=fitter->GetModel(ii);
	if(model) if(model->GetParValue("Pos")>0)lines++;
}
cout <<" "<< setw(2)<<lines << ": ";
peaks=0;
lines+=2; // two back ground models
// get position of lines
for(ii=0;ii<lines;ii++){
	model=fitter->GetModel(ii);
	if(model){
		linepos[peaks]=model->GetParValue("Pos");
		if(linepos[peaks]>0){ // excludes background models
			cout << " "<< setw(7)<<linepos[peaks];
			//			 << " Ampl " << mod->GetParValue("Ampl")
			//			 << " Width " << mod->GetParValue("Width")<<endl;
			peaks++;
		}}
}
cal0=0.;
cal1=1.;
cal2=0.;
if(peaks < minpeaks){
	cout<<full<<" empty"<<endl;
}else{
// Adjust raw
factor=sollmax/linepos[peaks-1];
offset=0.0;
cout << " f " <<factor<< endl;
//cout << "adj:";
for(ii=0;ii<peaks;ii++){
	lineposc[ii]=offset+factor*linepos[ii];
	iii=(Int_t)lineposc[ii];
	//cout << " "<< setw(7)<<lineposc[ii];
}
//cout << endl;
// now identify lines in histogram
// histogram may have more lines than energies.
graph->Set(0);// remove points
ii=peaks;
for(i=energies-1;i>=0;i--){
	while(abs(lineposc[ii]-energy[i]) > peakdistance){
		lineposc[ii--]=0;
		if(ii<0)break;
	}
	if(ii<0)break;
// 	    	   cout <<i<<"-"<< ii
// 				   << " ch "<< setw(6)<<linepos[ii]
// 				   <<" "<< setw(6)<<lineposc[ii]
// 				   << " E "<< setw(6)<<energy[i]<< endl;
	graph->SetPoint(i,linepos[ii],energy[i]);
	ii--;
}
cali->SetObject("CaliGraph", graph, kFALSE);
cali->DoActions(kTRUE);
//cali->PrintLines();
model=cali->GetModel(0);
cal0=model->GetParValue("Ampl");
model=cali->GetModel(1);
cal1=model->GetParValue("Ampl");
// model=cali->GetModel(2);
// cal2=model->GetParValue("Ampl");
} // peaks found, fitting coefficients
strcpy(num,his->GetName());
// need index for file:
p=strrchr(num,'_')+1;
Int_t i=atoi(p);
fLine.Form("%s %3d %10.1lf %13.5le %13.6le : %d %8.1le\n",
		num,i,cal0,cal1,cal2,cali->GetResultNDF(),cali->GetResultFF());
if(verbose)cout << fLine.Data();
ii=peaks;
for(i=energies-1;i>=0;i--){
	while((ii>0)&(lineposc[ii]==0))ii--;
	cal=cal0+cal1*linepos[ii]+cal2*linepos[ii]*linepos[ii];
	if(verbose)cout <<i<< " ch "<< setw(7)<<linepos[ii]
 		<<" cal "<< setw(7)<<cal
 		<<" E "<< setw(7)<<energy[i]
 		<<" "<<cal*100./energy[i]<<"%"<< endl;
	ii--;
	if(ii<0)break;
}
return kTRUE;
}
//-------------------------------------------------------
// Handle one directory and create one calibration file
void makecali1(TString rootfile, char *directory, TString califile)
{
cout << "# Calib: "<<califile << " File: "<<rootfile << endl;
cout << "# Factor "<<noisefactor<<" noise "<<noise<<" sum "<<sum<<endl;
strcpy(dir,directory);
// open root file with histograms
TFile* rfile = new TFile(rootfile.Data(),"READ");
// get list of histograms from directory
cout<<rootfile.Data()<<": "<<directory<<" * "<<endl;
time.Set();
TListIter *iter=GetHistograms(rfile, directory, "*");
ofstream ofile(califile.Data());
fLine.Form("# %s\n",rootfile.Data()); ofile.write(fLine.Data(),fLine.Length());
fLine.Form("# %s\n",califile.Data()); ofile.write(fLine.Data(),fLine.Length());
fLine.Form("# %s\n",time.AsString()); ofile.write(fLine.Data(),fLine.Length());
fLine.Form("# Finder: factor=%4.1f, min=%4.0f, sum=%3.0f\n",noisefactor,noise,sum);
ofile.write(fLine.Data(),fLine.Length());
fLine.Form("# Peaks: dist=%d, min=%d, max=%d\n",peakdistance,minpeaks,maxpeaks);
ofile.write(fLine.Data(),fLine.Length());
fLine.Form("# Histo: low=%5.0f, high=%5.0f\n",lolim,hilim);
ofile.write(fLine.Data(),fLine.Length());
while(namo = iter->Next())
{
	his= (TH1*) namo;
	if(his->GetMean()>0){
		MakeCalibration(his);
		ofile.write(fLine.Data(),fLine.Length());
	} // histogram with data
	else cout << directory<<"/"<<his->GetName()<<" empty"<<endl;
} // histogram loop
rfile->Close();
ofile.close();
}
//-------------------------------------------------------
// Main entry function
// Text_t is since 5.15-04 no longer supported!
Bool_t makecali(const char *prefix, const char *histofile)
{
time=new TDatime();
TString x=histofile;
TString rootfile = x+".root";
TString pref=prefix;
TString califile;
fitter=new TGo4Fitter("Fitter", TGo4Fitter::ff_least_squares, kTRUE);
cali=new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
graph=new TGraph();
cali->AddGraph("CaliGraph", graph, kFALSE);
 cali->AddPolynomX("CaliGraph","A",1); // only linear fit

//-----------------------------------
// For peak finder
noisefactor=3.;
noise=20.;
sum=2.;
peakdistance=1500;
minpeaks=3;
maxpeaks=4;
verbose=kTRUE;
// Read calibration file with energies
ReadEnergies("alpha.txt");

lolim=200.;
hilim=1500.;
califile=pref+"_StopXL.txt";
makecali1(rootfile,"Histograms/Cali/StopXL",califile);
lolim=200.;
hilim=1500.;
califile=pref+"_StopYL.txt";
makecali1(rootfile,"Histograms/Cali/StopYL",califile);
lolim=50.;
hilim=500.;
califile=pref+"_StopXH.txt";
makecali1(rootfile,"Histograms/Cali/StopXH",califile);
lolim=50.;
hilim=500.;
califile=pref+"_StopYH.txt";
makecali1(rootfile,"Histograms/Cali/StopYH",califile);


//-----------------------------------
cout<<"Done!"<<endl;
exit(0);
return kTRUE;
}
