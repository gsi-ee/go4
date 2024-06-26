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

#include "TGo4FitterAbstract.h"

#include <iostream>

#include "TBuffer.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TVectorD.h"
#include "TFormula.h"
#include "TClass.h"
#include "TString.h"
#include "TObjString.h"

#include "TGo4FitterConfig.h"
#include "TGo4FitDependency.h"
#include "TGo4FitterOutput.h"
#include "TGo4FitMinuit.h"
#include "TGo4FitParameter.h"

TGo4FitterAbstract::TGo4FitterAbstract()
   : TGo4FitParsList(), TGo4FitSlotList(), fxActions(), fxResults(), fbParsChange(kTRUE),
     fbInitializationDone(kFALSE), fbNeedToFinalize(kFALSE), fbParsAsResults(kFALSE)
{
}

TGo4FitterAbstract::TGo4FitterAbstract(const char *iName, const char *iTitle)
   : TGo4FitParsList(iName, iTitle, kFALSE), TGo4FitSlotList(), fxActions(), fxResults(),
     fbParsChange(kTRUE), fbInitializationDone(kFALSE), fbNeedToFinalize(kFALSE), fbParsAsResults(kFALSE)
{
   fxActions.SetOwner(kTRUE);
}

TGo4FitterAbstract::~TGo4FitterAbstract()
{
   if (fbNeedToFinalize)
      Finalize();
}

void TGo4FitterAbstract::Clear(Option_t *option)
{
   TGo4FitParsList::Clear(option);
   DeleteActions();
}

void TGo4FitterAbstract::CollectAllPars()
{
   ClearPars();
   if (fxCurrentConfig)
      fxCurrentConfig->GetParsNew().CollectParsTo(*this);
   fbParsChange = kFALSE;
}

void TGo4FitterAbstract::CopyParsValuesFrom(TGo4FitterAbstract *fitter)
{
   if (!fitter)
      return;
   fitter->CollectAllPars();
   CollectAllPars();

   for (Int_t n = 0; n < NumPars(); n++) {
      TGo4FitParameter *dest = Get(n);
      const TGo4FitParameter *src = fitter->FindPar(dest->GetFullName());
      if (src)
         dest->SetValue(src->GetValue());
   }
}

Bool_t TGo4FitterAbstract::GetParFixed(const char *ParName)
{
   Bool_t fixed = TGo4FitParsList::GetParFixed(ParName);
   if (fxCurrentConfig && !fixed)
      fixed = fxCurrentConfig->GetParFixed(ParName);
   return fixed;
}

Bool_t TGo4FitterAbstract::GetParRange(const char *ParName, Double_t &RangeMin, Double_t &RangeMax)
{
   Bool_t get = kFALSE;
   if (fxCurrentConfig)
      get = fxCurrentConfig->GetParRange(ParName, RangeMin, RangeMax);
   if (!get)
      get = TGo4FitParsList::GetParRange(ParName, RangeMin, RangeMax);
   return get;
}

Bool_t TGo4FitterAbstract::GetParEpsilon(const char *ParName, Double_t &Epsilon)
{
   Bool_t get = kFALSE;
   if (fxCurrentConfig)
      get = fxCurrentConfig->GetParEpsilon(ParName, Epsilon);
   if (!get)
      get = TGo4FitParsList::GetParEpsilon(ParName, Epsilon);
   return get;
}

Bool_t TGo4FitterAbstract::InitializeDependencies(TObjArray &Dependencies, Bool_t BlockPars, Bool_t DoInit)
{
   for (Int_t i = 0; i <= Dependencies.GetLast(); i++) {
      TGo4FitDependency *dep = (TGo4FitDependency *)Dependencies[i];

      Int_t parindx = 0;
      TGo4FitParameter *deppar = nullptr;
      if (dep->GetParameter().IsNull())
         parindx = -1;
      else {
         deppar = Find(dep->GetParameter().Data());
         if (!deppar) {
            std::cout << "Error dependence parameter: " << dep->GetParameter().Data() << std::endl;
            return kFALSE;
         }
         parindx = GetParIndex(deppar);
      }

      if (dep->GetExpression().Length() > 0) {
         TString formula(dep->GetExpression().Data());
         for (Int_t n = 0; n < NumPars(); n++) {
            if (n == parindx)
               continue;
            TString repl("[");
            repl += n;
            repl += "]";
            formula.ReplaceAll(GetParFullName(n), repl);
         }
         TFormula *fx = new TFormula("test", formula.Data());
         Int_t err = fx->Compile(formula.Data());
         delete fx;
         if (err != 0) {
            std::cout << "Error in dependence: " << formula.Data() << "   code " << err << std::endl;
            return kFALSE;
         }
         if (DoInit)
            dep->Initialize(parindx, formula.Data());

      } else if (DoInit)
         dep->Initialize(parindx, nullptr);

      if (BlockPars)
         deppar->SetBlocked();
   }
   return kTRUE;
}

void TGo4FitterAbstract::RunDependenciesList(TObjArray &Dependencies, Double_t *pars)
{
   for (Int_t i = 0; i <= Dependencies.GetLast(); i++)
      ((TGo4FitDependency *)Dependencies[i])->ExecuteDependency(pars);
}

void TGo4FitterAbstract::FinalizeDependencies(TObjArray &Dependencies)
{
   for (Int_t i = 0; i <= Dependencies.GetLast(); i++) {
      TGo4FitDependency *dep = (TGo4FitDependency *)Dependencies[i];
      dep->Finalize();
   }
}

void TGo4FitterAbstract::ExecuteDependencies(Double_t *pars)
{
   if (pars && fxCurrentConfig)
      RunDependenciesList(fxCurrentConfig->GetParsDepend(), pars);
}

Bool_t TGo4FitterAbstract::IsSuitableConfig(TGo4FitterConfig *Config)
{
   if (!Config)
      return kFALSE;
   Bool_t res = kTRUE;
   try {
      fxCurrentConfig = Config;
      CollectAllPars();
      res = InitializeDependencies(Config->GetParsInit(), kFALSE, kFALSE);
      res = res && InitializeDependencies(Config->GetParsDepend(), kFALSE, kFALSE);
      res = res && InitializeDependencies(Config->GetResults(), kFALSE, kFALSE);

      fxCurrentConfig = nullptr;
      CollectAllPars();

   } catch (...) {
      fxCurrentConfig = nullptr;
      CollectAllPars();
      throw;
   }

   return res;
}

Bool_t TGo4FitterAbstract::ApplyConfig(TGo4FitterConfig *Config)
{
   if (fxCurrentConfig) {
      FinalizeDependencies(fxCurrentConfig->GetParsInit());
      FinalizeDependencies(fxCurrentConfig->GetParsDepend());
      FinalizeDependencies(fxCurrentConfig->GetResults());
   }

   fxCurrentConfig = Config;
   CollectAllPars();

   Bool_t res = kTRUE;

   if (fxCurrentConfig) {
      res = res && InitializeDependencies(fxCurrentConfig->GetParsInit(), kFALSE, kTRUE);
      res = res && InitializeDependencies(fxCurrentConfig->GetParsDepend(), kTRUE, kTRUE);
      res = res && InitializeDependencies(fxCurrentConfig->GetResults(), kFALSE, kTRUE);

      if (res) {
         TArrayD pars(NumPars());
         GetParsValues(pars.GetArray());
         RunDependenciesList(fxCurrentConfig->GetParsInit(), pars.GetArray());
         SetParsValues(pars.GetArray());
      }
   }

   if (!res) {
      FinalizeDependencies(fxCurrentConfig->GetParsInit());
      FinalizeDependencies(fxCurrentConfig->GetParsDepend());
      FinalizeDependencies(fxCurrentConfig->GetResults());
      fxCurrentConfig = nullptr;
   }

   return res;
}

Bool_t TGo4FitterAbstract::Initialize()
{
   if (fbNeedToFinalize)
      Finalize();
   fbInitializationDone = kFALSE;

   if (!CheckObjects())
      return kFALSE;

   fbNeedToFinalize = kTRUE;

   CollectAllPars();

   ClearParsBlocking();

   if (!InitFitterData())
      return kFALSE;

   fbInitializationDone = kTRUE;

   return kTRUE;
}

Double_t TGo4FitterAbstract::CalculateFitFunction(Double_t *pars)
{
   if (pars) {
      ExecuteDependencies(pars);
      SetParsValues(pars);
   }

   return DoCalculation();
}

void TGo4FitterAbstract::Finalize()
{
   Double_t FF = DoCalculation();
   Int_t NDF = DoNDFCalculation();

   FinalizeFitterData();

   if (fxCurrentConfig && (fxCurrentConfig->GetResults().GetLast() >= 0)) {
      TArrayD pars(NumPars());
      GetParsValues(pars.GetArray());
      fxResults.Set(fxCurrentConfig->GetResults().GetLast() + 3);

      for (Int_t i = 0; i <= fxCurrentConfig->GetResults().GetLast(); i++) {
         TGo4FitDependency *res = (TGo4FitDependency *)(fxCurrentConfig->GetResults())[i];
         fxResults[i] = res->ExecuteDependency(pars.GetArray());
      }
   } else {
      fxResults.Set(NumPars() + 2);
      GetParsValues(fxResults.GetArray());
   }

   fxResults.SetAt(FF, fxResults.GetSize() - 2);
   fxResults.SetAt(NDF, fxResults.GetSize() - 1);

   if (fxCurrentConfig)
      ApplyConfig(nullptr);

   fbNeedToFinalize = kFALSE;
   fbInitializationDone = kFALSE;
}

void TGo4FitterAbstract::AddAction(TGo4FitterAction *Action)
{
   fxActions.Add(Action);
}

void TGo4FitterAbstract::AddActionAt(TGo4FitterAction *Action, Int_t indx)
{
   fxActions.AddAt(Action, indx);
}

TGo4FitterAction *TGo4FitterAbstract::GetAction(Int_t num)
{
   if ((num < 0) || (num > fxActions.GetLast()))
      return nullptr;
   return dynamic_cast<TGo4FitterAction *>(fxActions[num]);
}

void TGo4FitterAbstract::DeleteAction(TGo4FitterAction *action)
{
   if (!action)
      return;
   if (fxActions.IndexOf(action) >= 0) {
      fxActions.Remove(action);
      fxActions.Compress();
   }
   delete action;
}

void TGo4FitterAbstract::ReplaceAction(TGo4FitterAction *action, Int_t dir)
{
   Int_t indx = fxActions.IndexOf(action);
   if (!action || (indx < 0))
      return;
   Int_t newindx = indx + dir;
   if ((newindx >= 0) && (newindx <= fxActions.GetLast()) && (newindx != indx)) {
      fxActions[indx] = fxActions[newindx];
      fxActions[newindx] = action;
   }
}

void TGo4FitterAbstract::DoActions(Bool_t AllowFitterChange, TObjArray *Actions)
{
   if (!Actions)
      Actions = &fxActions;

   Bool_t need = kFALSE;
   for (Int_t n = 0; n <= Actions->GetLast(); n++) {
      TGo4FitterAction *action = dynamic_cast<TGo4FitterAction *>(Actions->At(n));
      if (action)
         need = need || action->NeedBuffers();
   }

   if (need)
      if (!Initialize())
         return;

   for (Int_t n = 0; n <= Actions->GetLast(); n++) {
      TGo4FitterAction *action = dynamic_cast<TGo4FitterAction *>(Actions->At(n));
      if (!action)
         continue;
      if (!AllowFitterChange && action->CanChangeFitter())
         continue;

      action->DoAction(this);
      CheckParsListChanging();
   }

   if (need)
      Finalize();
}

void TGo4FitterAbstract::DoAction(TGo4FitterAction *Action)
{
   if (!Action)
      return;

   Bool_t need = Action->NeedBuffers();

   if (need)
      if (!Initialize())
         return;

   Action->DoAction(this);

   if (need)
      Finalize();
}

void TGo4FitterAbstract::DoAction(Int_t indx)
{
   DoAction(GetAction(indx));
}

TObjArray *TGo4FitterAbstract::ProcessObjects(TObjArray *objs, Bool_t CloneFitter, Bool_t OnlyRequired,
                                              TObjArray *rownames, TObjArray *colnames)
{
   if (!objs || (objs->GetLast() < 0))
      return nullptr;

   if (NumSlots() <= 0)
      return nullptr;

   TArrayI use(NumSlots());
   use.Reset(-1);

   Int_t numuse = 0;
   for (Int_t n = 0; n < NumSlots(); n++) {
      TGo4FitSlot *slot = GetSlot(n);
      if (!slot)
         return nullptr;
      if (!slot->GetObject())
         if (slot->IsRequired() || !OnlyRequired)
            use[numuse++] = n;
   }

   if ((numuse == 0) || ((objs->GetLast() + 1) % numuse != 0))
      return nullptr;

   Int_t nuse = 0;
   for (Int_t nobj = 0; nobj <= objs->GetLast(); nobj++) {
      TObject *obj = objs->At(nobj);
      if (!obj) {
         std::cout << "Empty object in list" << std::endl;
         return nullptr;
      }
      TGo4FitSlot *slot = GetSlot(use[nuse++]);
      if (nuse == numuse)
         nuse = 0;
      if (!slot->IsSuitable(obj)) {
         std::cout << "Object " << obj->GetName() << " of class " << obj->ClassName() << " noncompatible with "
                   << slot->GetClass()->GetName() << std::endl;
         return nullptr;
      }
   }

   TObjArray *res = new TObjArray((objs->GetLast() + 1) / numuse);
   res->SetOwner(kTRUE);
   TGo4FitterAbstract *resf = nullptr;

   if (rownames) {
      rownames->Clear();
      rownames->SetOwner(kTRUE);
   }
   if (colnames) {
      colnames->Clear();
      colnames->SetOwner(kTRUE);
   }

   if (CloneFitter) {

      Int_t nobj = 0;

      do {
         TGo4FitterAbstract *newfitter = dynamic_cast<TGo4FitterAbstract *>(Clone());
         if (!newfitter)
            break;
         res->Add(newfitter);

         if (newfitter->NumSlots() != NumSlots())
            break;

         nuse = 0;
         while (nuse < numuse) {
            TGo4FitSlot *slot = newfitter->GetSlot(use[nuse++]);
            slot->SetObject(objs->At(nobj), kFALSE);
            if (nuse == 1)
               newfitter->SetName(objs->At(nobj)->GetName());
            if ((nuse == 1) && colnames)
               colnames->Add(new TObjString(objs->At(nobj)->GetName()));
            nobj++;
         }

         newfitter->DoActions();

         resf = newfitter;

      } while (nobj <= objs->GetLast());

   } else {

      MemorizePars();

      Int_t nobj = 0;

      do {

         nuse = 0;
         while (nuse < numuse) {
            TGo4FitSlot *slot = GetSlot(use[nuse++]);
            slot->SetObject(objs->At(nobj), kFALSE);
            if ((nuse == 1) && colnames)
               colnames->Add(new TObjString(objs->At(nobj)->GetName()));
            nobj++;
         }

         RememberPars();

         DoActions();

         res->Add(new TVectorD(0, GetNumResults() - 1, GetResults()->GetArray()));

      } while (nobj <= objs->GetLast());

      RememberPars();

      resf = this;
   }

   if (rownames && resf) {
      if (resf->IsParsAsResults())
         for (Int_t n = 0; n < resf->NumPars(); n++)
            rownames->Add(new TObjString(resf->GetParFullName(n)));
      else
         for (Int_t n = 0; n < resf->GetNumResults(); n++) {
            TString rname("Result");
            rname += n;
            rownames->Add(new TObjString(rname));
         }
   }
   nuse = 0;
   while (nuse < numuse) {
      TGo4FitSlot *slot = GetSlot(use[nuse++]);
      slot->SetObject(nullptr, kFALSE);
   }

   return res;
}

void TGo4FitterAbstract::AddSimpleMinuit()
{
   TGo4FitMinuit *fMinuit = new TGo4FitMinuit("Minuit");
   fMinuit->AddCommand("MIGRAD 500 1");
   AddAction(fMinuit);
}

TGo4FitterOutput *TGo4FitterAbstract::AddOutputAction(const char *Action, const char *Option)
{
   TGo4FitterOutput *act = new TGo4FitterOutput(Action, Option);
   AddAction(act);
   return act;
}

void TGo4FitterAbstract::DeleteOutputActions()
{
   for (Int_t n = 0; n <= fxActions.GetLast(); n++) {
      TGo4FitterOutput *action = dynamic_cast<TGo4FitterOutput *>(fxActions[n]);
      if (action) {
         fxActions.Remove(action);
         delete action;
      }
   }
   fxActions.Compress();
}

Int_t TGo4FitterAbstract::NeedPadsNumber()
{
   Int_t res = 0;
   for (Int_t n = 0; n < GetNumActions(); n++) {
      TGo4FitterOutput *act = dynamic_cast<TGo4FitterOutput *>(GetAction(n));
      if (act && act->NeedPad())
         res++;
   }
   return res;
}

void TGo4FitterAbstract::SetPad(Int_t indx, TVirtualPad *pad)
{
   Int_t i = 1; // puds numbering starts from 1
   for (Int_t n = 0; n < GetNumActions(); n++) {
      TGo4FitterOutput *act = dynamic_cast<TGo4FitterOutput *>(GetAction(n));
      if (act && act->NeedPad()) {
         if (i == indx) {
            act->SetPad(pad);
            return;
         }
         i++;
      }
   }
}

Double_t TGo4FitterAbstract::GetResultValue(Int_t n) const
{
   if ((n >= 0) && (n < GetNumResults()))
      return fxResults.At(n);
   return 0.;
}

Int_t TGo4FitterAbstract::GetResultNDF() const
{
   if (fxResults.GetSize() < 2)
      return 0;
   return fxResults.GetArray()[fxResults.GetSize() - 1];
}

Double_t TGo4FitterAbstract::GetResultFF() const
{
   if (fxResults.GetSize() < 2)
      return 0.;
   return fxResults.GetArray()[fxResults.GetSize() - 2];
}

void TGo4FitterAbstract::PrintResults() const
{
   std::cout << std::endl << "*** LIST OF RESULT VALUE ***" << std::endl;
   std::cout << "    Fit function = " << GetResultFF() << std::endl;
   std::cout << "    NDF = " << GetResultNDF() << std::endl;
   for (Int_t n = 0; n < GetNumResults(); n++)
      std::cout << "    Res " << n << " =  " << GetResultValue(n) << std::endl;
}

void TGo4FitterAbstract::Print(Option_t *option) const
{
   std::cout << std::endl << "********** THIS IS PRINTOUT OF FITTER OBJECT **********" << std::endl;
   TGo4FitNamed::Print(option);
   TGo4FitParsList::Print(option);
   if (fxActions.GetLast() >= 0) {
      std::cout << "Actions list: " << std::endl;
      fxActions.Print(option);
   }
}

void TGo4FitterAbstract::Streamer(TBuffer &b)
{
   if (b.IsReading()) {
      TGo4FitterAbstract::Class()->ReadBuffer(b, this);
   } else {
      PrepareSlotsForWriting();
      TGo4FitterAbstract::Class()->WriteBuffer(b, this);
   }
}
