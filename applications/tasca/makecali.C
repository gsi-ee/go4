// Main function caligam at the end
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
Double_t sollmin=122.;
Double_t sollmax=1408.;
Double_t offset,factor;
Double_t cal,cal0,cal1,cal2;
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
Bool_t MakeCalibration(TH1 *his, Int_t width){
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
fitter->AddH1("data1", his, kFALSE, 0., 1500.);
fitter->DoActions(kTRUE);
lines=0;
memset(linepos,0,sizeof(linepos));
memset(lineposc,0,sizeof(lineposc));
// count lines
for(ii=0;ii<MAXLINES;ii++){
	model=fitter->GetModel(ii);
	if(model) if(model->GetParValue("Pos")>0)lines++;
}
cout <<" "<< setw(2)<<lines << ":";
lines+=2; // two back ground models
peaks=0;
// get position of lines
for(ii=0;ii<lines;ii++){
	model=fitter->GetModel(ii);
	if(model){
		linepos[peaks]=model->GetParValue("Pos");
		if(linepos[peaks]>0){
			cout << " "<< setw(7)<<linepos[peaks];
			//			 << " Ampl " << mod->GetParValue("Ampl")
			//			 << " Width " << mod->GetParValue("Width")<<endl;
			peaks++;
		}}
}
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
ii=0;
for(i=0;i<energies;i++){
	while(abs(lineposc[ii]-energy[i]) > width){
		lineposc[ii++]=0;
		if(ii==MAXLINES)break;
	}
	if(ii==MAXLINES)break;
//	    	   cout <<i<<"-"<< ii
//				   << " ch "<< setw(6)<<linepos[ii]
//				   <<" "<< setw(6)<<lineposc[ii]
//				   << " E "<< setw(6)<<energy[i]<< endl;
	graph->SetPoint(i,linepos[ii],energy[i]);
	ii++;
}
cali->SetObject("CaliGraph", graph, kFALSE);
cali->DoActions(kTRUE);
//cali->PrintLines();
model=cali->GetModel(0);
cal0=model->GetParValue("Ampl");
model=cali->GetModel(1);
cal1=model->GetParValue("Ampl");
model=cali->GetModel(2);
cal2=model->GetParValue("Ampl");
strcpy(num,his->GetName());
// need index for file:
p=strrchr(num,'_')+1;
Int_t i=atoi(p);
//cout << num<<" "<<i<<" "
//	<< cal0<<" "<<cal1<<" "<<cal2<<endl;
fLine.Form("%s %d %lf %le %le\n",num,i,cal0,cal1,cal2);
cout<<coeffline;
ii=0;
for(i=0;i<energies;i++){
	while((ii<(MAXLINES-1))&(lineposc[ii]==0))ii++;
	cal=cal0+cal1*linepos[ii]+cal2*linepos[ii]*linepos[ii];
//	cout <<i<< " ch "<< setw(7)<<linepos[ii]
//		<<" cal "<< setw(7)<<cal
//		<<" E "<< setw(7)<<energy[i]
//		<<" "<<cal*100./energy[i]<<"%"<< endl;
	ii++;
	if(ii==MAXLINES)break;
}
return kTRUE;
}
//-------------------------------------------------------
// Main entry function
// Text_t is since 5.15-04 no longer supported!
Bool_t makecali(const char *calif, const char *file)
{
	TString x=calif;
	TString califile = x+".txt";
	x=file;
	TString rootfile = x+".root";
cout << "# Calib: "<<califile << " File: "<<rootfile << endl;
cout << "# Factor "<<noisefactor<<" noise "<<noise<<" sum "<<sum<<endl;
fitter=new TGo4Fitter("Fitter", TGo4Fitter::ff_least_squares, kTRUE);
cali=new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
graph=new TGraph();
cali->AddGraph("CaliGraph", graph, kFALSE);
cali->AddPolynomX("CaliGraph","A",2);

Bool_t caligam=kTRUE;
Bool_t caliadc=kTRUE;
//-----------------------------------
if(caligam)
{
noisefactor=7.7;
noise=20.;
sum=1.;
// Read calibration file with energies
ReadEnergies(calif);

// open root file with histograms
TFile* rfile = new TFile(rootfile.Data(),"READ");

// get list of histograms from directory
strcpy(dir,"Histograms/Unpack/GammaE");
cout<<rootfile.Data()<<": "<<dir<<" * "<<endl;
TListIter *iter=GetHistograms(rfile, dir, "GammaE*");
ofstream ofile("gammacoeff.txt");
while(namo = iter->Next())
{
	his= (TH1*) namo;
	if(his->GetMean()>0){
		MakeCalibration(his,10);
		ofile.write(fLine.Data(),fLine.Length());
	} // histogram with data
	else cout << his->GetName()<<" empty"<<endl;
} // histogram loop
rfile->Close();
ofile.close();
}
//-----------------------------------
if(caliadc)
{
noisefactor=3.;
noise=5.;
sum=1.;

// Read calibration file with energies
ReadEnergies("alpha.txt");

//fitter=new TGo4Fitter("Fitter", TGo4Fitter::ff_least_squares, kTRUE);
// open root file with histograms
TFile* rfile = new TFile(rootfile.Data(),"READ");

// get list of histograms from directory
strcpy(dir,"Histograms/Cali/StopL");
cout<<rootfile.Data()<<": "<<dir<<" * "<<endl;
TListIter *iter2=GetHistograms(rfile, dir, "StopXL_*");
ofstream ofile("stopXLcoeff.txt");
while(namo = iter2->Next())
{
	his= (TH1*) namo;
	if(his->GetMean()>0){
		MakeCalibration(his,2500);
		if(peaks != 4)cout << his->GetName()<<" "<<peaks<<" peaks"<<endl;
		ofile.write(fLine.Data(),fLine.Length());
	} // histogram with data
	else cout << his->GetName()<<" empty"<<endl;
} // histogram loop
rfile->Close();
ofile.close();
}
//-----------------------------------
return kTRUE;
}

