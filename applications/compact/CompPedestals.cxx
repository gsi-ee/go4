//---------------------------------------------
// Go4 Comp analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompPedestals.h"

#include "Riostream.h"
#include "snprintf.h"

//***********************************************************
CompPedestals::CompPedestals() : TGo4Parameter() {}
//***********************************************************
CompPedestals::CompPedestals(const char* name) : TGo4Parameter(name){
    cout << "Comp> CompPedestals: " << name << " created" << endl;
    Restore=kFALSE;
    Save=kFALSE;
    Calibrate=kFALSE;
    ffOffset=0;
    File="ped.txt";
    for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
       ffPedestals[ix]=0;
}
//***********************************************************
CompPedestals::~CompPedestals(){
}
//***********************************************************

//-----------------------------------------------------------
void CompPedestals::SavePedestals(const char * file){
	File=file;
	std::ofstream database(File.Data());
	if(database==0)
	    cout << "Comp> CompPedestals: Error open " << File.Data()<< endl;
	else {
		database << "# header " << endl;
		database << "! header " << endl;
		for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
			database << ix << " "
			<< setprecision(5)
			<< setiosflags(ios::fixed)<<ffPedestals[ix] << endl;
		database.close();
		cout << "Comp> CompPedestals: Pedestals in " << File.Data()<< endl;
	}
	RestorePedestals(file);
	return;
}
//-----------------------------------------------------------
void CompPedestals::RestorePedestals(const char * file){
	Text_t line[64];
	Float_t v;
	UInt_t ii,i=0;
	File=file;
	std::ifstream database(File.Data());
	if(database==0)
	    cout << "Comp> CompPedestals: Error open " << File.Data() << endl;
	else {
		cout << "Comp> CompPedestals: Pedestals from " << File.Data()<< endl;
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
Int_t CompPedestals::PrintPedestals(){
  return 0;
  cout << "Pedestals " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t CompPedestals::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("CompPedestals")) {
    CompPedestals * from = (CompPedestals *) pp;
    File=from->File;
    if(from->Restore){
    	RestorePedestals(File.Data());
    }
    if(from->Save){
    	SavePedestals(File.Data());
    }
    Calibrate=from->Calibrate;
    ffOffset=from->ffOffset;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

