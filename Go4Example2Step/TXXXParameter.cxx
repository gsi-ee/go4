// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXParameter.h"

#include "Riostream.h"

//***********************************************************
TXXXParameter::TXXXParameter() :
   TGo4Parameter()
{
   frP1=0;
   frP2=0;
   fiNum=0;
   fbEnab=kFALSE;
   fdTest=0;
   fsTest=0;
   fcTest=0;
   flTest=0;
   fuNum=0;
   fuShort=0;
   fuChar=0;
   fuLong=0;
   fxInputfile="No file specified";
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
      fiDataArray[ix]=0;
   for(Int_t u=0;u<__MEVX__;++u)
      for(Int_t v=0;v<__MEVY__;++v)
         fdMeV[u][v]=0;
   for(Int_t x=0;x<__MEVX__;++x)
      fdTestarr[x]=0;
}
//***********************************************************
TXXXParameter::TXXXParameter(const char* name) :
   TGo4Parameter(name)
{
   frP1=10;
   frP2=20;
   fiNum=-3;
   fbEnab=kFALSE;
   fdTest=3.141592665;
   fsTest=6;
   fcTest=47;
   flTest=20;
   fuNum=42;
   fuShort=8;
   fuChar=9;
   fuLong=10;
   fxInputfile="testdata.lmd";
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
      fiDataArray[ix]=ix+30;
   for(Int_t u=0;u<__MEVX__;++u)
      for(Int_t v=0;v<__MEVY__;++v)
         fdMeV[u][v]=(u+v)*1.537;
   for(Int_t x=0;x<__MEVX__;++x)
      fdTestarr[x]=x*3.1415;
}

//***********************************************************
TXXXParameter::~TXXXParameter()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXParameter::PrintParameter(Text_t * n, Int_t)
{
   cout << "Parameter " << GetName()<<":" <<endl;
   cout << "  P1="<<frP1<<endl;
   cout << "  P2="<<frP2<< endl;
   cout << "  fiNum="<<fiNum<<endl;
   cout << "  fbEnab="<<fbEnab<<endl;
   cout << "  fdTest="<<fdTest<<endl;
   cout << "  fsTest="<<fsTest<<endl;
   cout << "  fcTest="<<fcTest<<endl;
   cout << "  flTest="<<flTest<<endl;
   cout << "  fuNum="<<fuNum<<endl;
   cout << "  fuShort="<<fuShort<<endl;
   cout << "  fuChar="<<fuChar<<endl;
   cout << "  fuLong="<<fuLong<<endl;
//   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
//      cout << "  fiDataArray["<<ix<<"]="<<fiDataArray[ix]<<endl;
//   for(Int_t u=0;u<__MEVX__;++u)
//      for(Int_t v=0;v<__MEVY__;++v)
//         cout << "  fdMeV["<<u<<"]["<<v<<"]="<<fdMeV[u][v]<<endl;
//   for(Int_t x=0;x<__MEVX__;++x)
//      cout << "  fdTestarr["<<x<<"]="<<fdTestarr[x]<<endl;
//   cout <<"  fxInputfile="<<fxInputfile.Data()  << endl;
   return 0;
}
//-----------------------------------------------------------
Bool_t TXXXParameter::UpdateFrom(TGo4Parameter *pp)
{
   TXXXParameter* from = dynamic_cast<TXXXParameter*> (pp);
   if (from==0) {
      cout << "Wrong parameter object: " << pp->ClassName() << endl;
      return kFALSE;
   }

   cout << "Parameter " << GetName() << ": P1=" << frP1 << " P2="<< frP2 << endl;
   frP1=from->frP1;
   frP2=from->frP2;
   fiNum=from->fiNum;
   fbEnab=from->fbEnab;
   fdTest=from->fdTest;
   fsTest=from->fsTest;
   fcTest=from->fcTest;
   flTest=from->flTest;
   fuNum=from->fuNum;
   fuShort=from->fuShort;
   fuChar=from->fuChar;
   fuLong=from->fuLong;
   fxInputfile=from->fxInputfile;
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
      fiDataArray[ix]=from->fiDataArray[ix];
   for(Int_t u=0;u<__MEVX__;++u)
      for(Int_t v=0;v<__MEVY__;++v)
         fdMeV[u][v]=from->fdMeV[u][v];
   for(Int_t x=0;x<__MEVX__;++x)
      fdTestarr[x] = from->fdTestarr[x];
   // cout <<"Updated Parameter:" << endl;
   // PrintParameter(0,0);

  return kTRUE;
}

