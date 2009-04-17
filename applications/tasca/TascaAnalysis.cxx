#include "TascaAnalysis.h"

#include "Riostream.h"
#include <time.h>

#include "TH1.h"
#include "TFile.h"

#include "TascaParameter.h"


//***********************************************************
TascaAnalysis::TascaAnalysis()
{
  cout << "Tasca> TascaAnalysis: Create" << endl;


  //////////////// Parameter //////////////////////////
  // At this point, autosave file has not yet been read!
  // Therefore parameter values set here will be overwritten
  // if an autosave file is there.
  fPar = new TascaParameter("TascaPedestals");
  AddParameter(fPar);

}

//***********************************************************
TascaAnalysis::~TascaAnalysis()
{
  cout << "Tasca> TascaAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaAnalysis::UserPreLoop()
{
  cout << "Tasca> TascaAnalysis: PreLoop" << endl;

   return 0;
}
//-----------------------------------------------------------
Int_t TascaAnalysis::UserPostLoop()
{
  cout << "Tasca> TascaAnalysis: PostLoop" << endl;
   return 0;
}

//-----------------------------------------------------------
Int_t TascaAnalysis::UserEventFunc()
{
//// This function is called once for each event.
   return 0;
}
