#include "TascaPedestals.h"

#include "Riostream.h"
#include "snprintf.h"

//***********************************************************
TascaPedestals::TascaPedestals() : TGo4Parameter() {}
//***********************************************************
TascaPedestals::TascaPedestals(const char* name) : TGo4Parameter(name){
    cout << "Tasca> TascaPedestals: " << name << " created" << endl;
    fbRestore=kFALSE;
    fbSave=kFALSE;
    fxFile="ped.txt";
    for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
       ffPedestals[ix]=0;
}
//***********************************************************
TascaPedestals::~TascaPedestals(){
}
//***********************************************************

//-----------------------------------------------------------
void TascaPedestals::SavePedestals(const char * file){
	fxFile=file;
	std::ofstream database(fxFile.Data());
	if(database==0)
	    cout << "Tasca> TascaPedestals: Error open " << fxFile.Data()<< endl;
	else {
		database << "# header " << endl;
		database << "! header " << endl;
		for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
			database << ix << " "
			<< setprecision(5)
			<< setiosflags(ios::fixed)<<ffPedestals[ix] << endl;
		database.close();
		cout << "Tasca> TascaPedestals: Pedestals in " << fxFile.Data()<< endl;
	}
	RestorePedestals(file);
	return;
}
//-----------------------------------------------------------
void TascaPedestals::RestorePedestals(const char * file){
	Text_t line[64];
	Float_t v;
	UInt_t ii,i=0;
	fxFile=file;
	std::ifstream database(fxFile.Data());
	if(database==0)
	    cout << "Tasca> TascaPedestals: Error open " << fxFile.Data() << endl;
	else {
		cout << "Tasca> TascaPedestals: Pedestals from " << fxFile.Data()<< endl;
		while(i<sizeof(ffPedestals)/sizeof(Float_t)){
			database.getline(line,63,'\n' ); // read whole line
            if(database.eof() || !database.good()) break;
            if(strstr(line,"#") || strstr(line,"!")){
            	cout << line << endl;
            	continue;// skip any comments
            }
			sscanf(line,"%d %f",&ii,&v);
			if(i != ii) cout << "Index error: " << ii << " should be " << i << endl;
			ffPedestals[i]=v;
			i++;
		}
		database.close();
	}
	return;
}
//-----------------------------------------------------------
Int_t TascaPedestals::PrintPedestals(){
  return 0;
  cout << "Pedestals " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaPedestals::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaPedestals")) {
    TascaPedestals * from = (TascaPedestals *) pp;
    fxFile=from->fxFile;
    if(from->fbRestore){
    	RestorePedestals(fxFile.Data());
    }
    if(from->fbSave){
    	SavePedestals(fxFile.Data());
    }
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

