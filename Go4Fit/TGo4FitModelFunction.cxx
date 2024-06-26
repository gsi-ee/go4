// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4FitModelFunction.h"

#include <iostream>

#ifndef _WINDOWS
#include <dlfcn.h>
#endif

#include "TGo4FitParameter.h"

TGo4FitModelFunction::TGo4FitModelFunction()
   : TGo4FitModel(), fxLibraryName(), fxFunctionName(), fxUserFunction(nullptr), fxLibrary(nullptr)
{
}

TGo4FitModelFunction::TGo4FitModelFunction(const char *iName, TUserFunction iUserFunction, Int_t iNPars,
                                           Bool_t AddAmplitude)
   : TGo4FitModel(iName, "Model, using user function", AddAmplitude), fxLibraryName(), fxFunctionName(), fxPosIndex(),
     fxWidthIndex(), fxUserFunction(iUserFunction), fxLibrary(nullptr)
{
   for (Int_t n = 0; n < iNPars; n++)
      NewParameter(GetFuncParName(n), "user parameter", 0.);
}

TGo4FitModelFunction::TGo4FitModelFunction(const char *iName, const char *iLibraryName, const char *iFunctionName,
                                           Int_t iNPars, Bool_t AddAmplitude)
   : TGo4FitModel(iName, "Model, using user function", AddAmplitude), fxLibraryName(iLibraryName),
     fxFunctionName(iFunctionName), fxPosIndex(), fxWidthIndex(), fxUserFunction(nullptr), fxLibrary(nullptr)
{
   for (Int_t n = 0; n < iNPars; n++)
      NewParameter(GetFuncParName(n), "user parameter", 0.);
}

TGo4FitModelFunction::~TGo4FitModelFunction()
{
   CloseLibrary();
}

void TGo4FitModelFunction::SetUserFunction(TUserFunction iUserFunction)
{
   CloseLibrary();
   fxLibraryName = "";
   fxFunctionName = "";
   fxUserFunction = iUserFunction;
}

void TGo4FitModelFunction::SetUserFunction(const char *iLibraryName, const char *iFunctionName)
{
   CloseLibrary();
   fxLibraryName = iLibraryName;
   fxFunctionName = iFunctionName;
   fxUserFunction = nullptr;
}

Int_t TGo4FitModelFunction::GetNumberOfFuncPar()
{
   Int_t num = TGo4FitParsList::NumPars();
   if (GetAmplPar())
      num--;
   return num;
}

TGo4FitParameter *TGo4FitModelFunction::GetFuncPar(Int_t n)
{
   if ((n < 0) || (n >= GetNumberOfFuncPar()))
      return nullptr;
   if ((GetAmplIndex() >= 0) && (n >= GetAmplIndex()))
      n++;
   return GetPar(n);
}

Bool_t TGo4FitModelFunction::SetNumberOfFuncPar(Int_t num)
{
   Int_t oldnum = GetNumberOfFuncPar();
   if ((num < 0) || (oldnum == num))
      return kFALSE;
   if (num < oldnum) {
      for (Int_t n = oldnum - 1; n >= num; n--)
         RemovePar(GetFuncPar(n)->GetName());
   } else {
      for (Int_t n = oldnum; n < num; n++)
         NewParameter(GetFuncParName(n), "user parameter", 0.);
   }

   return kTRUE;
}

void TGo4FitModelFunction::SetPosParIndex(Int_t naxis, Int_t indx)
{
   if (naxis < 0)
      return;
   Int_t oldsize = fxPosIndex.GetSize();
   if (naxis >= oldsize) {
      fxPosIndex.Set(naxis + 1);
      for (Int_t n = oldsize; n < fxPosIndex.GetSize(); n++)
         fxPosIndex[n] = -1;
   }
   fxPosIndex[naxis] = indx;
}

void TGo4FitModelFunction::SetWidthParIndex(Int_t naxis, Int_t indx)
{
   if (naxis < 0)
      return;
   Int_t oldsize = fxWidthIndex.GetSize();
   if (naxis >= oldsize) {
      fxWidthIndex.Set(naxis + 1);
      for (Int_t n = oldsize; n < fxWidthIndex.GetSize(); n++)
         fxWidthIndex[n] = -1;
   }
   fxWidthIndex[naxis] = indx;
}

Int_t TGo4FitModelFunction::GetPosParIndex(Int_t naxis)
{
   if ((naxis < 0) || (naxis >= fxPosIndex.GetSize()))
      return -1;
   return GetParIndex(GetFuncPar(fxPosIndex[naxis]));
}

Int_t TGo4FitModelFunction::GetWidthParIndex(Int_t naxis)
{
   if ((naxis < 0) || (naxis >= fxWidthIndex.GetSize()))
      return -1;
   return GetParIndex(GetFuncPar(fxWidthIndex[naxis]));
}

TString TGo4FitModelFunction::GetFuncParName(Int_t n)
{
   TString res;
   res.Form("Par%d", n);
   return res;
}

Bool_t TGo4FitModelFunction::Initialize(Int_t UseBuffers)
{
   if (!LoadLibrary(kTRUE))
      return kFALSE;
   return TGo4FitModel::Initialize(UseBuffers);
}

void TGo4FitModelFunction::Finalize()
{
   CloseLibrary();
   TGo4FitModel::Finalize();
}

Bool_t TGo4FitModelFunction::BeforeEval(Int_t ndim)
{
   if (!TGo4FitModel::BeforeEval(ndim))
      return kFALSE;
   return LoadLibrary(kFALSE);
}

void TGo4FitModelFunction::AfterEval()
{
   TGo4FitModel::AfterEval();
}

Double_t TGo4FitModelFunction::UserFunction(Double_t *Coordinates, Double_t *Parameters)
{
   if (fxUserFunction)
      return (*fxUserFunction)(Coordinates, Parameters);
   return 0.;
}

#ifndef _WINDOWS

Bool_t TGo4FitModelFunction::LoadLibrary(Bool_t CloseFirst)
{
   if (fxLibraryName.IsNull() || fxFunctionName.IsNull()) {
      if (!fxUserFunction)
         std::cout << "TGo4FitModelFunction: user function not set" << std::endl;
      return fxUserFunction != nullptr;
   }

   if (CloseFirst)
      CloseLibrary();
   if (fxLibrary && fxUserFunction)
      return kTRUE;

   fxLibrary = dlopen(fxLibraryName, RTLD_NOW | RTLD_GLOBAL);
   if (!fxLibrary) {
      std::cout << " TGo4FitModelFunction: failed to open " << fxLibraryName << ",  " << dlerror() << std::endl;
      return kFALSE;
   }
   fxUserFunction = (TUserFunction)dlsym(fxLibrary, fxFunctionName);
   if (!fxUserFunction)
      fxUserFunction = (TUserFunction)dlsym(fxLibrary, fxFunctionName + "__FPdT0");
   if (!fxUserFunction)
      fxUserFunction = (TUserFunction)dlsym(fxLibrary, fxFunctionName + "__");
   if (!fxUserFunction)
      fxUserFunction = (TUserFunction)dlsym(fxLibrary, fxFunctionName + "_");
   if (!fxUserFunction) {
      std::cout << " TGo4FitModelFunction: failed to find " << fxFunctionName << ",  " << dlerror() << std::endl;
      CloseLibrary();
      return kFALSE;
   }
   return kTRUE;
}

void TGo4FitModelFunction::CloseLibrary()
{
   if (fxLibrary) {
      dlclose(fxLibrary);
      fxLibrary = nullptr;
   }
}

#else

Bool_t TGo4FitModelFunction::LoadLibrary(Bool_t CloseFirst)
{
   // here must be win32 LoadLibrary() and GetProcAddress() calls
   // when required, must be implemented

   fxLibrary = nullptr;
   return kFALSE;
}

void TGo4FitModelFunction::CloseLibrary()
{
   // here must be FreeLibrary() calls
   fxLibrary = nullptr;
}

#endif

void TGo4FitModelFunction::Print(Option_t *option) const
{
   TGo4FitModel::Print(option);
   if ((fxLibraryName.Length() > 0) || (fxFunctionName.Length() > 0))
      std::cout << "Function " << fxFunctionName << "  in " << fxLibraryName << std::endl;
   else
      std::cout << " Pointer on function " << fxUserFunction << std::endl;
   for (Int_t naxis = 0; naxis < fxPosIndex.GetSize(); naxis++) {
      TGo4FitParameter *par = ((TGo4FitModelFunction *)this)->GetFuncPar(fxPosIndex[naxis]);
      if (par)
         std::cout << "  Position on " << naxis << " axis is " << par->GetName() << std::endl;
   }
   for (Int_t naxis = 0; naxis < fxWidthIndex.GetSize(); naxis++) {
      TGo4FitParameter *par = ((TGo4FitModelFunction *)this)->GetFuncPar(fxWidthIndex[naxis]);
      if (par)
         std::cout << "  Width on " << naxis << " axis is " << par->GetName() << std::endl;
   }
}
