// $Id: TXXXParameter.cxx 487 2009-11-03 17:06:10Z goofy $
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
