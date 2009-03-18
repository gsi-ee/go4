#include "TGo4FitterAbstract.h"

#include "Riostream.h"

#include "TArrayD.h"
#include "TArrayI.h"
#include "TVectorD.h"
#include "TFormula.h"
#include "TClass.h"
#include "TString.h"
#include "TObjString.h"
#include "TVirtualPad.h"

#include "TGo4FitterConfig.h"
#include "TGo4FitDependency.h"
#include "TGo4FitterAction.h"
#include "TGo4FitterOutput.h"
#include "TGo4FitMinuit.h"
#include "TGo4FitParameter.h"


TGo4FitterAbstract::TGo4FitterAbstract() :
   TGo4FitParsList(), TGo4FitSlotList(),
   fxActions(), fxResults(),
   fxCurrentConfig(0), fbParsChange(kTRUE),
   fbInitializationDone(kFALSE), fbNeedToFinalize(kFALSE), fbParsAsResults(kFALSE)
{
}

TGo4FitterAbstract::TGo4FitterAbstract(const char * iName, const char * iTitle) :
   TGo4FitParsList(iName,iTitle,kFALSE), TGo4FitSlotList(),
   fxActions(), fxResults(),
   fxCurrentConfig(0), fbParsChange(kTRUE),
   fbInitializationDone(kFALSE), fbNeedToFinalize(kFALSE), fbParsAsResults(kFALSE)
{
   fxActions.SetOwner(kTRUE);
}

TGo4FitterAbstract::~TGo4FitterAbstract()
{
   if (fbNeedToFinalize) Finalize();
}

void TGo4FitterAbstract::Clear(Option_t* option)
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

void TGo4FitterAbstract::CopyParsValuesFrom(TGo4FitterAbstract* fitter)
{
   if (fitter==0) return;
   fitter->CollectAllPars();
   CollectAllPars();

   for(Int_t n=0;n<NumPars();n++) {
      TGo4FitParameter* dest = Get(n);
      const TGo4FitParameter* src = fitter->FindPar(dest->GetFullName());
      if (src!=0) dest->SetValue(src->GetValue());
    }
}

Bool_t TGo4FitterAbstract::GetParFixed(const char* ParName)
{
   Bool_t fixed = TGo4FitParsList::GetParFixed(ParName);
   if (fxCurrentConfig && !fixed)
      fixed = fxCurrentConfig->GetParFixed(ParName);
   return fixed;
}

Bool_t TGo4FitterAbstract::GetParRange(const char* ParName, Double_t& RangeMin, Double_t& RangeMax)
{
   Bool_t get = kFALSE;
   if (fxCurrentConfig)
      get = fxCurrentConfig->GetParRange(ParName,RangeMin,RangeMax);
   if (!get) get = TGo4FitParsList::GetParRange(ParName,RangeMin,RangeMax);
   return get;
}

Bool_t TGo4FitterAbstract::GetParEpsilon(const char* ParName, Double_t& Epsilon)
{
   Bool_t get = kFALSE;
   if (fxCurrentConfig)
      get = fxCurrentConfig->GetParEpsilon(ParName,Epsilon);
   if (!get) get = TGo4FitParsList::GetParEpsilon(ParName,Epsilon);
   return get;
}


Bool_t TGo4FitterAbstract::InitializeDependencies(TObjArray& Dependencies, Bool_t BlockPars, Bool_t DoInit)
{
   for(Int_t i=0;i<=Dependencies.GetLast();i++) {
      TGo4FitDependency *dep = (TGo4FitDependency*) Dependencies[i];

     Int_t parindx = 0;
     TGo4FitParameter* deppar = 0;
     if (dep->GetParameter().Length()==0) parindx=-1; else {
       deppar = Find(dep->GetParameter().Data());
       if (deppar==0) { cout << "Error dependence parameter: " << dep->GetParameter().Data() << endl; return kFALSE; }
       parindx = GetParIndex(deppar);
     }

     if (dep->GetExpression().Length()>0) {
        TString formula(dep->GetExpression().Data());
        for(Int_t n=0;n<NumPars();n++) {
            if (n == parindx) continue;
            TString repl("[");
            repl+=n;
            repl+="]";
            formula.ReplaceAll(GetParFullName(n),repl);
        }
        TFormula* fx = new TFormula("test",formula.Data());
        Int_t err = fx->Compile(formula.Data());
        delete fx;
        if (err!=0) {
           cout << "Error in dependence: " << formula.Data() << "   code " << err << endl;
           return kFALSE;
        }
        if (DoInit) dep->Initialize(parindx, formula.Data());

     } else if (DoInit) dep->Initialize(parindx,0);

     if (BlockPars) deppar->SetBlocked();
  }
  return kTRUE;
}

void TGo4FitterAbstract::RunDependenciesList(TObjArray& Dependencies, Double_t* pars)
{
   for(Int_t i=0;i<=Dependencies.GetLast();i++) 
     ((TGo4FitDependency*) Dependencies[i]) -> ExecuteDependency(pars);
}

void TGo4FitterAbstract::FinalizeDependencies(TObjArray& Dependencies)
{
   for(Int_t i=0;i<=Dependencies.GetLast();i++) {
     TGo4FitDependency *dep = (TGo4FitDependency*) Dependencies[i];
     dep->Finalize();
  }
}

void TGo4FitterAbstract::ExecuteDependencies(Double_t* pars)
{
   if ((pars!=0) && (fxCurrentConfig!=0))
       RunDependenciesList(fxCurrentConfig->GetParsDepend(), pars );
}

Bool_t TGo4FitterAbstract::IsSuitableConfig(TGo4FitterConfig* Config)
{
   if (Config==0) return kFALSE;
   Bool_t res = kTRUE;
   try {
     fxCurrentConfig=Config;
     CollectAllPars();
     res = InitializeDependencies(Config->GetParsInit(), kFALSE,kFALSE);
     res = res && InitializeDependencies(Config->GetParsDepend(), kFALSE,kFALSE);
     res = res && InitializeDependencies(Config->GetResults(), kFALSE,kFALSE);

     fxCurrentConfig=0;
     CollectAllPars();

   } catch(...) {
      fxCurrentConfig=0;
      CollectAllPars();
      throw;
   }

   return res;
}

Bool_t TGo4FitterAbstract::ApplyConfig(TGo4FitterConfig* Config)
{
   if (fxCurrentConfig!=0) {
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
         RunDependenciesList(fxCurrentConfig->GetParsInit(), pars.GetArray() );
         SetParsValues(pars.GetArray());
      }
   }

   if (!res) {
      FinalizeDependencies(fxCurrentConfig->GetParsInit());
      FinalizeDependencies(fxCurrentConfig->GetParsDepend());
      FinalizeDependencies(fxCurrentConfig->GetResults());
      fxCurrentConfig = 0;
   }

   return res;
}

Bool_t TGo4FitterAbstract::Initialize()
{
   if (fbNeedToFinalize) Finalize();
   fbInitializationDone=kFALSE;

   if (!CheckObjects()) return kFALSE;

   fbNeedToFinalize = kTRUE;

   CollectAllPars();

   ClearParsBlocking();

   if (!InitFitterData()) return kFALSE;

   fbInitializationDone=kTRUE;

   return kTRUE;
}

Double_t TGo4FitterAbstract::CalculateFitFunction(Double_t* pars)
{
   if (pars!=0) {
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

   if ((fxCurrentConfig!=0) && (fxCurrentConfig->GetResults().GetLast()>=0)) {
      TArrayD pars(NumPars());
      GetParsValues(pars.GetArray());
      fxResults.Set(fxCurrentConfig->GetResults().GetLast()+3);

      for(Int_t i=0;i<=fxCurrentConfig->GetResults().GetLast();i++) {
        TGo4FitDependency *res = (TGo4FitDependency*) (fxCurrentConfig->GetResults())[i];
        fxResults[i] = res->ExecuteDependency(pars.GetArray());
      }
   } else {
     fxResults.Set(NumPars()+2);
     GetParsValues(fxResults.GetArray());
   }

   fxResults[fxResults.GetSize()-2] = FF;
   fxResults[fxResults.GetSize()-1] = NDF;

   if (fxCurrentConfig!=0)
     ApplyConfig(0);

   fbNeedToFinalize = kFALSE;
   fbInitializationDone=kFALSE;
}

void TGo4FitterAbstract::AddAction(TGo4FitterAction* Action)
{
   fxActions.Add(Action);
}

void TGo4FitterAbstract::AddActionAt(TGo4FitterAction* Action, Int_t indx)
{
   fxActions.AddAt(Action, indx);
}

TGo4FitterAction* TGo4FitterAbstract::GetAction(Int_t num)
{
  if ((num<0) || (num>fxActions.GetLast())) return 0;
  return dynamic_cast<TGo4FitterAction*> (fxActions[num]);
}

void TGo4FitterAbstract::DeleteAction(TGo4FitterAction* action)
{
  if (action==0) return;
  if (fxActions.IndexOf(action)>=0) {
    fxActions.Remove(action);
    fxActions.Compress();
  }
  delete action;
}

void TGo4FitterAbstract::ReplaceAction(TGo4FitterAction* action, Int_t dir)
{
   Int_t indx = fxActions.IndexOf(action);
   if ((action==0) || (indx<0)) return;
   Int_t newindx = indx+dir;
   if ((newindx>=0) && (newindx<=fxActions.GetLast()) && (newindx!=indx)) {
      fxActions[indx] = fxActions[newindx];
      fxActions[newindx] = action;
   }
}

void TGo4FitterAbstract::DoActions(Bool_t AllowFitterChange, TObjArray* Actions)
{
  if (Actions==0) Actions = &fxActions;

  Bool_t need = kFALSE;
  for(Int_t n=0;n<=Actions->GetLast();n++) {
     TGo4FitterAction* action = dynamic_cast<TGo4FitterAction*> (Actions->At(n));
     if (action)
       need = need || action->NeedBuffers();
  }
  if (need)
    if (!Initialize()) return;

  for(Int_t n=0;n<=Actions->GetLast();n++) {
     TGo4FitterAction* action = dynamic_cast<TGo4FitterAction*> (Actions->At(n));
     if (action==0) continue;
     if (!AllowFitterChange && action->CanChangeFitter()) continue;

     action->DoAction(this);
     CheckParsListChanging();
  }

  if (need) Finalize();
}

void TGo4FitterAbstract::DoAction(TGo4FitterAction* Action)
{
   if (Action==0) return;

   Bool_t need = Action->NeedBuffers();

   if (need)
     if (!Initialize()) return;

   Action->DoAction(this);

   if (need) Finalize();
}

void TGo4FitterAbstract::DoAction(Int_t indx)
{
   DoAction(GetAction(indx));
}

TObjArray* TGo4FitterAbstract::ProcessObjects(TObjArray* objs, Bool_t CloneFitter, Bool_t OnlyRequired, TObjArray* rownames, TObjArray* colnames)
{
  if ((objs==0) || (objs->GetLast()<0)) return 0;

  if (NumSlots()<=0) return 0;

  TArrayI use(NumSlots()); use.Reset(-1);

  Int_t numuse = 0;
  for (Int_t n=0;n<NumSlots();n++) {
     TGo4FitSlot* slot = GetSlot(n);
     if (slot==0) return 0;
     if (slot->GetObject()==0)
       if (slot->IsRequired() || !OnlyRequired) use[numuse++] = n;
  }

  if ((numuse==0) || ((objs->GetLast()+1) % numuse != 0)) return 0;

  Int_t nuse = 0;
  for (Int_t nobj=0;nobj<=objs->GetLast();nobj++) {
     TObject* obj = objs->At(nobj);
     if (obj==0) {
       cout << "Empty object in list" << endl;
       return 0;
     }
     TGo4FitSlot* slot = GetSlot(use[nuse++]);
     if (nuse==numuse) nuse=0;
     if (!slot->IsSuitable(obj)) {
        cout << "Object " << obj->GetName() << " of class " << obj->ClassName() <<
                " noncompatible with " << slot->GetClass()->GetName() << endl;
        return 0;
     }
  }

  TObjArray* res = new TObjArray((objs->GetLast()+1) / numuse);
  res->SetOwner(kTRUE);
  TGo4FitterAbstract* resf = 0;

  if (rownames!=0) { rownames->Clear(); rownames->SetOwner(kTRUE); }
  if (colnames!=0) { colnames->Clear(); colnames->SetOwner(kTRUE); }

  if (CloneFitter) {

    Int_t nobj = 0;

    do {
       TGo4FitterAbstract* newfitter = dynamic_cast<TGo4FitterAbstract*> (Clone());
       if (newfitter==0) break;
       res->Add(newfitter);

       if (newfitter->NumSlots()!=NumSlots()) break;

       nuse = 0;
       while(nuse<numuse) {
          TGo4FitSlot* slot = newfitter->GetSlot(use[nuse++]);
          slot->SetObject(objs->At(nobj), kFALSE);
          if (nuse==1) newfitter->SetName(objs->At(nobj)->GetName());
          if ((nuse==1) && (colnames!=0)) colnames->Add(new TObjString(objs->At(nobj)->GetName()));
          nobj++;
       }

       newfitter->DoActions();

       resf = newfitter;

    } while (nobj<=objs->GetLast());


  } else {

    MemorizePars();

    Int_t nobj = 0;

    do {

       nuse = 0;
       while(nuse<numuse) {
         TGo4FitSlot* slot = GetSlot(use[nuse++]);
         slot->SetObject(objs->At(nobj), kFALSE);
         if ((nuse==1) && (colnames!=0)) colnames->Add(new TObjString(objs->At(nobj)->GetName()));
         nobj++;
       }

       RememberPars();

       DoActions();

       res->Add(new TVectorD(0, GetNumResults()-1, GetResults()->GetArray()));

    } while (nobj<=objs->GetLast());

    RememberPars();

    resf = this;
  }

  if ((rownames!=0) && (resf!=0)){
    if (resf->IsParsAsResults())
        for(Int_t n=0;n<resf->NumPars();n++) rownames->Add(new TObjString(resf->GetParFullName(n)));
    else
      for(Int_t n=0;n<resf->GetNumResults();n++) {
        TString rname("Result");
        rname+=n;
        rownames->Add(new TObjString(rname));
      }
  }
  nuse = 0;
  while(nuse<numuse) {
    TGo4FitSlot* slot = GetSlot(use[nuse++]);
    slot->SetObject(0, kFALSE);
  }

  return res;
}

void TGo4FitterAbstract::AddSimpleMinuit()
{
   TGo4FitMinuit* fMinuit = new TGo4FitMinuit("Minuit");
   fMinuit->AddCommand("MIGRAD 500 1");
   AddAction(fMinuit);
}

TGo4FitterOutput*  TGo4FitterAbstract::AddOutputAction(const char* Action, const char* Option)
{
   TGo4FitterOutput* act = new  TGo4FitterOutput(Action,Option);
   AddAction(act);
   return act;
}

void TGo4FitterAbstract::DeleteOutputActions()
{
   for(Int_t n=0;n<=fxActions.GetLast();n++) {
     TGo4FitterOutput* action = dynamic_cast<TGo4FitterOutput*> (fxActions[n]);
     if (action) { fxActions.Remove(action); delete action; }
   }
   fxActions.Compress();
}

Int_t TGo4FitterAbstract::NeedPadsNumber()
{
   Int_t res = 0;
   for(Int_t n=0;n<GetNumActions(); n++) {
      TGo4FitterOutput* act = dynamic_cast<TGo4FitterOutput*>  (GetAction(n));
      if (act && act->NeedPad()) res++;
   }
   return res;
}

void TGo4FitterAbstract::SetPad(Int_t indx, TVirtualPad* pad)
{
   Int_t i = 1; // puds numbering starts from 1
   for(Int_t n=0;n<GetNumActions(); n++) {
      TGo4FitterOutput* act = dynamic_cast<TGo4FitterOutput*>  (GetAction(n));
      if (act && act->NeedPad())  {
         if (i==indx) { act->SetPad(pad); return; }
         i++;
      }
   }
}

Double_t TGo4FitterAbstract::GetResultValue(Int_t n) const
{
  if ((n>=0) && (n<GetNumResults())) return fxResults[n];
                                else return 0.;
}

Double_t TGo4FitterAbstract::GetResultFF() const
{
  if (fxResults.GetSize()>1) return fxResults[fxResults.GetSize()-2];
                        else return 0;
}

Int_t TGo4FitterAbstract::GetResultNDF() const
{
  if (fxResults.GetSize()>1) return Int_t(fxResults[fxResults.GetSize()-1]);
                        else return 0;
}

void TGo4FitterAbstract::PrintResults() const
{
   cout << endl << "*** LIST OF RESULT VALUE ***" << endl;
   cout << "    Fit function = " << GetResultFF() << endl;
   cout << "    NDF = " << GetResultNDF() << endl;
   for(Int_t n=0;n<GetNumResults();n++)
     cout << "    Res " << n << " =  " << GetResultValue(n) << endl;
}

void TGo4FitterAbstract::Print(Option_t* option) const
{
   cout << endl << "********** THIS IS PRINTOUT OF FITTER OBJECT **********" << endl;
   TGo4FitNamed::Print(option);
   TGo4FitParsList::Print(option);
   if (fxActions.GetLast()>=0) {
      cout << "Actions list: " << endl;
      fxActions.Print(option);
   }
}

void TGo4FitterAbstract::Streamer(TBuffer& b)
{
   if (b.IsReading()) {
     TGo4FitterAbstract::Class()->ReadBuffer(b, this);
   } else {
     PrepareSlotsForWriting();
     TGo4FitterAbstract::Class()->WriteBuffer(b, this);
   }
}
