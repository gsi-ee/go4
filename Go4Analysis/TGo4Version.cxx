#include "TGo4Version.h"

#include "Riostream.h"

const Int_t TGo4Version::fgiGO4VERSION= __GO4BUILDVERSION__;

TGo4Version* TGo4Version::fxInstance = 0;

TGo4Version* TGo4Version::Instance()
{
   if (fxInstance == 0)
     fxInstance = new TGo4Version();
   return fxInstance;
}

Int_t TGo4Version::GetBuildVersion()
{
   return fgiGO4VERSION;
}

TGo4Version::TGo4Version()
{
}

TGo4Version::~TGo4Version()
{
}

Bool_t TGo4Version::CheckVersion(Int_t version)
{
//
   if(fgiGO4VERSION==version) return kTRUE;

   cerr <<" !!!! VERSION MISMATCH: \n\tUser Analysis Build \tVersion ";
   cerr << fgiGO4VERSION << endl;
   cerr <<"\tdoes not match current \tVersion ";
   cerr << version <<" " << endl;
   return kFALSE;
}
