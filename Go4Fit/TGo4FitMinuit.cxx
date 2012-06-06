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

#include "TGo4FitMinuit.h"

#include "go4iostream.h"

#include "TMinuit.h"
#include "TArrayD.h"
#include "TMatrixD.h"
#include "TObjString.h"

#include "TGo4FitterAbstract.h"
#include "TGo4FitMinuitResult.h"

class TMinuitEx : public TMinuit {
  public:

    TMinuitEx(Int_t NumPars, TGo4FitterAbstract* fitter);
    virtual ~TMinuitEx();

    virtual Int_t Eval(Int_t npar, Double_t *grad, Double_t &fval, Double_t *pars, Int_t iflag);

  protected:

    TGo4FitterAbstract* fxFitter;    //!
};

TMinuitEx::TMinuitEx(Int_t NumPars, TGo4FitterAbstract* fitter) :
  TMinuit(NumPars), fxFitter(fitter) {
}

TMinuitEx::~TMinuitEx() {
}

Int_t TMinuitEx::Eval(Int_t npar, Double_t *grad, Double_t &fval, Double_t *pars, Int_t iflag) {
   if (fxFitter) fval = fxFitter->CalculateFitFunction(pars);
            else fval = 0.;
   return 0;
}

// *******************************************************************************

TGo4FitMinuit::TGo4FitMinuit() : TGo4FitterAction(), fxCommands(), fxResults() {
}

TGo4FitMinuit::TGo4FitMinuit(const char* Name) :
    TGo4FitterAction(Name,"Fitter minimization using TMinuit object"), fxCommands(), fxResults() {
   fxCommands.SetOwner(kTRUE);
   fxResults.SetOwner(kTRUE);
}

TGo4FitMinuit::~TGo4FitMinuit() {
}

void TGo4FitMinuit::AddCommand(const char* iCommand)
{
   fxCommands.Add( new TObjString(iCommand) );
}

const char* TGo4FitMinuit::GetCommand(Int_t n)
{
   return ((TObjString*) fxCommands[n])->GetString().Data();
}

void TGo4FitMinuit::DoAction(TGo4FitterAbstract* Fitter) {

   TMinuitEx fMinuit(Fitter->NumPars(), Fitter);
   fMinuit.SetPrintLevel(-1);

   for(Int_t n=0;n<Fitter->NumPars();n++) {
      const char* FullName = Fitter->GetParFullName(n);
      Int_t ierflg = 0;
      Double_t epsilon = 0, RangeMin = 0, RangeMax = 0;
      if (!Fitter->GetParEpsilon(FullName,epsilon)) epsilon = 1.;

      if (!Fitter->GetParRange(FullName,RangeMin,RangeMax)) { RangeMin = 0; RangeMax = 0; }

      fMinuit.mnparm(n, FullName, Fitter->GetParValue(FullName),
                        epsilon, RangeMin, RangeMax, ierflg);

      if (Fitter->GetParFixed(FullName)) fMinuit.FixParameter(n);
   }

   if (fxCommands.GetLast()<0) fMinuit.Command("MIGRAD 500 1");
   else
     for(Int_t n=0;n<=fxCommands.GetLast();n++) {
       TString cmd ( ((TObjString*) fxCommands[n])->GetString() );
       if (cmd[0] == 'r') {
           if (cmd.Index("result",6,0,TString::kIgnoreCase) == 0) {
              cmd.Remove(0,6);
              while ((cmd.Length()>0) && (cmd[0]==' ')) cmd.Remove(0,1);
              if (cmd.Length()==0) cmd = "1000";
              if (cmd.Length()<4) { cout << "invalid result command syntax" << endl; break; }
              Bool_t getpar  = (cmd[0]=='1');
              Bool_t geterr  = (cmd[1]=='1');
              Bool_t getmatr = (cmd[2]=='1');
              Bool_t getcontr = (cmd[3]=='1');
              cmd.Remove(0,4);
              while ((cmd.Length()>0) && (cmd[0]==' ')) cmd.Remove(0,1);
              if (cmd.Length()==0) cmd="Result";

              TGo4FitMinuitResult *res = new TGo4FitMinuitResult(cmd,"TMinuit result object");
              fxResults.Add(res);
              res->CallMNSTAT(&fMinuit);
              if (getpar)  res->CallMNPOUT(&fMinuit,Fitter->NumPars());
              if (geterr)  res->CallMNERRS(&fMinuit,Fitter->NumPars());
              if (getmatr) res->CallMNEMAT(&fMinuit,Fitter->NumPars(), kTRUE);
              if (getcontr) res->GetContourPlot(&fMinuit);
          }

       } else fMinuit.Command(cmd);
     }

   for(Int_t n=0;n<Fitter->NumPars();n++) {
      Double_t value,error;
      fMinuit.GetParameter(n, value, error);
      Fitter->SetParValue(Fitter->GetParFullName(n),value);
      Fitter->SetParError(Fitter->GetParFullName(n),error);
   }
}

TGo4FitMinuitResult* TGo4FitMinuit::GetResult(Int_t indx)
{
   return (indx>=0) && (indx<=fxResults.GetLast()) ? (TGo4FitMinuitResult*) fxResults.At(indx) : 0;
}

TGo4FitMinuitResult* TGo4FitMinuit::FindResult(const char* ResName)
{
   return (TGo4FitMinuitResult*) fxResults.FindObject(ResName);
}

void TGo4FitMinuit::AddResult(TGo4FitMinuitResult* res)
{
   fxResults.Add(res);
}

void TGo4FitMinuit::RemoveResult(TGo4FitMinuitResult* res)
{
   fxResults.Remove(res);
   fxResults.Compress();
}

void TGo4FitMinuit::Print(Option_t* option) const {
   TGo4FitterAction::Print(option);
   if (fxCommands.GetLast()>=0)
     cout << "List of commands:" << endl;
   for(Int_t n=0;n<=fxCommands.GetLast();n++)
     cout << "   " << ((TObjString*) fxCommands[n])->String().Data() << endl;
   if (fxResults.GetLast()>=0) {
     cout << "List of stored results:" << endl;
     fxResults.Print(option);
   }
}
