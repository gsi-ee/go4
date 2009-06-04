//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------
#include <Riostream.h>

void setparam()
{
// because the name Parameters is not unique we must fully qualify
TascaParameter *fParameter = (TascaParameter *) go4->GetObject("Parameters/Parameters","Go4");
 if(fParameter!=0){
	 fParameter->shift=5;
	 fParameter->EventStackSize=10000; // prefix for coefficient files
     fParameter->AlphaMaxL=16000.; // take low value up to this limit
     fParameter->AlphaMaxH=30000.;// take high value up to this limit as low
 }
 else cout<<"Parameters not found"<<endl;
}
