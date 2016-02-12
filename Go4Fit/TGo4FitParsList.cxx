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

#include "TGo4FitParsList.h"

#include "Riostream.h"
#include "TBuffer.h"

#include "TClass.h"
#include "TStyle.h"
#include "TString.h"

#include "TGo4FitParameter.h"

TGo4FitParsList::TGo4FitParsList() :
   TGo4FitNamed(), fxPars(), fbCanRollbackPars(kFALSE)
{

}

TGo4FitParsList::TGo4FitParsList(Bool_t IsParsOwned) :
      TGo4FitNamed(), fxPars(), fbCanRollbackPars(kFALSE)
{
   fxPars.SetOwner(IsParsOwned);
}

TGo4FitParsList::TGo4FitParsList(const char* Name, const char* Title, Bool_t IsParsOwned) :
         TGo4FitNamed(Name,Title), fxPars(), fbCanRollbackPars(kFALSE)
{
   fxPars.SetOwner(IsParsOwned);
}

TGo4FitParsList::~TGo4FitParsList()
{
}

Int_t TGo4FitParsList::GetParIndex(const TGo4FitParameter* par)
{
   for(Int_t i=0;i<NumPars();i++)
     if(GetPar(i)==par) return i;
   return -1;
}

Double_t TGo4FitParsList::GetParValue(const char* ParName)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) return par->GetValue();
      else return 0.;
}

Bool_t TGo4FitParsList::SetParValue(const char* ParName, Double_t iValue)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) { par->SetValue(iValue); return kTRUE; }
      else return kFALSE;
}

void TGo4FitParsList::GetParsValues(Double_t* pars)
{
   for(Int_t i=0;i<NumPars();i++)
     pars[i] = Get(i)->GetValue();
}

void TGo4FitParsList::SetParsValues(Double_t* pars)
{
   if (pars)
     for(Int_t i=0;i<NumPars();i++)
        Get(i)->SetValue(pars[i]);
}

Double_t TGo4FitParsList::GetParError(const char* ParName)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) return par->GetError();
      else return 0.;
}

Bool_t TGo4FitParsList::SetParError(const char* ParName, Double_t iError)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) { par->SetError(iError); return kTRUE; }
      else return kFALSE;
}

Bool_t TGo4FitParsList::SetParFixed(const char* ParName, Bool_t iFixed)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) { par->SetFixed(iFixed); return kTRUE; }
      else return kFALSE;
}

Bool_t TGo4FitParsList::GetParFixed(const char* ParName)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) return par->GetFixed();
      else return kFALSE;
}

Int_t TGo4FitParsList::NumFixedPars()
{
   Int_t res = 0;
   for (Int_t n=0; n<NumPars(); n++)
      if (Get(n)->GetFixed()) res++;
   return res;
}

Int_t TGo4FitParsList::NumFreePars()
{
   return NumPars()-NumFixedPars();
}


Bool_t TGo4FitParsList::SetParRange(const char* ParName, Double_t RangeMin, Double_t RangeMax)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) { par->SetRange(RangeMin,RangeMax); return kTRUE; }
      else return kFALSE;
}

Bool_t TGo4FitParsList::GetParRange(const char* ParName, Double_t& RangeMin, Double_t& RangeMax)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) return par->GetRange(RangeMin,RangeMax);
      else return kFALSE;
}

Bool_t TGo4FitParsList::SetParEpsilon(const char* ParName, Double_t Epsilon)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) { par->SetEpsilon(Epsilon); return kTRUE; }
      else return kFALSE;
}

Bool_t TGo4FitParsList::GetParEpsilon(const char* ParName, Double_t& Epsilon)
{
   TGo4FitParameter* par = Find(ParName);
   if(par) return par->GetEpsilon(Epsilon);
      else return kFALSE;
}

void TGo4FitParsList::SetParName(Int_t n, const char* name)
{
   if(Get(n)) Get(n)->SetName(name);
}

const char* TGo4FitParsList::GetParName(Int_t n)
{
   if(Get(n)) return Get(n)->GetName();
         else return 0;
}

const char* TGo4FitParsList::GetParFullName(Int_t n)
{
   if(Get(n)) return Get(n)->GetFullName();
         else return 0;
}

void TGo4FitParsList::SetParsNames(const char* name0, const char* name1, const char* name2, const char* name3, const char* name4,
                                   const char* name5, const char* name6, const char* name7, const char* name8, const char* name9) {
  SetParName(0,name0);
  SetParName(1,name1);
  SetParName(2,name2);
  SetParName(3,name3);
  SetParName(4,name4);
  SetParName(5,name5);
  SetParName(6,name6);
  SetParName(7,name7);
  SetParName(8,name8);
  SetParName(9,name9);
}

void TGo4FitParsList::SetParsValues(Double_t par0, Double_t par1, Double_t par2, Double_t par3, Double_t par4,
                                    Double_t par5, Double_t par6, Double_t par7, Double_t par8, Double_t par9) {
  if(Get(0)) Get(0)->SetValue(par0);
  if(Get(1)) Get(1)->SetValue(par1);
  if(Get(2)) Get(2)->SetValue(par2);
  if(Get(3)) Get(3)->SetValue(par3);
  if(Get(4)) Get(4)->SetValue(par4);
  if(Get(5)) Get(5)->SetValue(par5);
  if(Get(6)) Get(6)->SetValue(par6);
  if(Get(7)) Get(7)->SetValue(par7);
  if(Get(8)) Get(8)->SetValue(par8);
  if(Get(9)) Get(9)->SetValue(par9);
}

void TGo4FitParsList::MemorizePars()
{
   for(Int_t i=0;i<NumPars();i++)
      Get(i)->MemorizeValue();
   fbCanRollbackPars = kTRUE;
}

void TGo4FitParsList::RememberPars()
{
   for(Int_t i=0;i<NumPars();i++)
      Get(i)->RememberValue();
}

Bool_t TGo4FitParsList::IsAllParsFixed()
{
   for(Int_t i=0;i<NumPars();i++)
      if (!Get(i)->GetFixed()) return kFALSE;
   return kTRUE;
}

void TGo4FitParsList::Print(Option_t* option) const
{
   TGo4FitParsList* lst = (TGo4FitParsList*) this;
   if ((lst->NumPars()>0) && fxPars.IsOwner()) {
      std::cout << "  List of parameters:" << std::endl;
      for(Int_t n=0;n<lst->NumPars();n++)
         lst->GetPar(n)->Print(option);
   }
}

void TGo4FitParsList::PrintPars() const
{
   std::cout << std::endl << "*** LIST OF PARAMETERS VALUE ***" << std::endl;

   TGo4FitParsList* l = (TGo4FitParsList*) this;

   int maxparlen = 14;
   for(Int_t np=0;np<l->NumPars();np++) {
      int len = strlen(l->GetParFullName(np));
      if (len>maxparlen) maxparlen = len;
   }

   TString tt = TString::Format("%s%ds %s%s %s%s","%",maxparlen,"%",gStyle->GetStatFormat(),"%",gStyle->GetStatFormat());

   for(Int_t n=0;n<l->NumPars();n++)
      std::cout << Form(tt.Data(), l->GetParFullName(n), l->GetPar(n)->GetValue(), l->GetPar(n)->GetError()) << std::endl;
}

void TGo4FitParsList::SetParsOwner(TGo4FitNamed* iOwner)
{
   for(Int_t i=0;i<NumPars();i++)
      Get(i)->SetOwner(iOwner);
}

TGo4FitParameter* TGo4FitParsList::CreatePar(const char* ParName, const char* Title, Double_t iValue)
{
  if (Find(ParName)==0) return AddPar(new TGo4FitParameter(ParName,Title,iValue));
                   else return Find(ParName);
}

TGo4FitParameter* TGo4FitParsList::Find(const char* ParName)
{
   for(Int_t i=0;i<NumPars();i++)
      if (strcmp(Get(i)->GetName(),ParName)==0) return Get(i);

   for(Int_t i=0;i<NumPars();i++)
      if (strcmp(Get(i)->GetFullName(),ParName)==0) return Get(i);

   return 0;
}

TGo4FitParameter* TGo4FitParsList::Find(const char* OwnerFullName, const char* ParName)
{
   for(Int_t i=0;i<NumPars();i++) {
      TGo4FitParameter* par = Get(i);
      if (par->GetOwner()==0) continue;
      if ( (strcmp(par->GetOwnerFullName(),OwnerFullName)==0) &&
           (strcmp(par->GetName(),ParName)==0) ) return par;
   }
   return 0;
}

TGo4FitParameter* TGo4FitParsList::Find(TGo4FitParameter* par)
{
   for(Int_t i=0;i<NumPars();i++)
     if (Get(i)==par) return par;
   return 0;
}

Bool_t TGo4FitParsList::RemoveParByIndex(Int_t indx)
{
  if((indx>=0) && (indx<NumPars())) return RemovePar(Get(indx));
                               else return kFALSE;
}

Bool_t TGo4FitParsList::RemovePar(const char* name)
{
   return RemovePar(FindPar(name));
}

Bool_t TGo4FitParsList::RemovePar(TGo4FitParameter* par)
{
   if (fxPars.FindObject(par)) {
      fbCanRollbackPars = kFALSE;
      fxPars.Remove(par);
      if (fxPars.IsOwner()) delete par;
      return kTRUE;
   } else return kFALSE;
}

void TGo4FitParsList::CollectParsTo(TGo4FitParsList & list)
{
   for(Int_t i=0;i<NumPars();i++) {
      TGo4FitParameter* par = Get(i);
      if (list.GetParIndex(par)<0) list.AddPar(par);
   }
}

void TGo4FitParsList::ClearParsBlocking()
{
   for(Int_t i=0;i<NumPars();i++)
     Get(i)->ClearBlocked();
}

void TGo4FitParsList::ClearPars()
{
   fxPars.Clear();
   fbCanRollbackPars = kFALSE;
}


TGo4FitParameter* TGo4FitParsList::AddPar(TGo4FitParameter* par)
{
  fxPars.AddLast(par);
  if ((par!=0) && fxPars.IsOwner()) par->SetOwner(this);
  fbCanRollbackPars = kFALSE;
  return par;
}

TGo4FitParameter* TGo4FitParsList::InsertPar(TGo4FitParameter* par, Int_t indx)
{
   if (indx<0) fxPars.AddLast(par);
   else fxPars.AddAt(par,indx);
   if ((par!=0) && fxPars.IsOwner()) par->SetOwner(this);
   fbCanRollbackPars = kFALSE;
   return par;
}

void TGo4FitParsList::Streamer(TBuffer& b)
{
   if (b.IsReading()) {
      TGo4FitParsList::Class()->ReadBuffer(b, this);
      Bool_t IsOwner;
      b >> IsOwner;
      if (IsOwner) {
         fxPars.Streamer(b);
         SetParsOwner(this);
      }
   } else {
      TGo4FitParsList::Class()->WriteBuffer(b, this);
      b << fxPars.IsOwner();
      if (fxPars.IsOwner())
         fxPars.Streamer(b);
   }
}
