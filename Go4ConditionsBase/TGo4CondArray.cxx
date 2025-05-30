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

#include "TGo4CondArray.h"

#include "TROOT.h"
#include "TObjArray.h"

#include "TGo4Log.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArrayPainter.h"

// ----------------------------------------------------------
TGo4WinCond *TGo4CondArray::Win(Int_t i) const
{
   if(fiType != kGO4CONDWINDOW) return nullptr;
   return (TGo4WinCond*) At(i);
}
// ----------------------------------------------------------
TGo4PolyCond *TGo4CondArray::Poly(Int_t i) const
{
   if(fiType != kGO4CONDPOLYGON) return nullptr;
   return (TGo4PolyCond*) At(i);
}
// ----------------------------------------------------------
TGo4Condition *TGo4CondArray::operator[](Int_t i) const
{
   return (TGo4Condition *) condarr->At(i);
}
// ----------------------------------------------------------
TGo4Condition *TGo4CondArray::At(Int_t i) const
{
   // boundary check by TObjArray, returns [0] in case
   return (TGo4Condition *) condarr->At(i);
}
// ----------------------------------------------------------
TGo4CondArray::TGo4CondArray() :
   TGo4Condition(),
   condarr(nullptr),
   fiNumCond(0),
   fiType(0),
   fiSelectedCond(0)
{
}
// ----------------------------------------------------------
TGo4CondArray::TGo4CondArray(const char *name, Int_t elements, Int_t contype) :
   TGo4Condition(name,"TGo4CondArray"),
   condarr(nullptr),
   fiNumCond(0),
   fiType(0),
   fiSelectedCond(0)
{
   fiNumCond = elements;
   if(contype == kGO4CONDWINDOW) {
     condarr = new TObjArray(elements);
     fiType = contype;
     for(Int_t i = 0; i < elements; i++)
       condarr->AddLast(new TGo4WinCond(TString::Format("%s%06d",name,i).Data()));
   } else
   if(contype == kGO4CONDPOLYGON) {
      condarr = new TObjArray(elements);
      fiType = contype;
      for(Int_t i = 0; i < elements; i++)
         condarr->AddLast(new TGo4PolyCond(TString::Format("%s%06d",name,i).Data()));

      TGo4PolyCond::CleanupSpecials(); // JAM2016
   } else {
     fiNumCond = 0;
     fiType = 0;
   }
}
// ----------------------------------------------------------
TGo4CondArray::TGo4CondArray(const char *name, Int_t elements, const char *type)
: TGo4Condition(name,type),fiSelectedCond(0)
{
   fiNumCond = elements;
   if(strcmp(type,"TGo4WinCond") == 0) {
      condarr = new TObjArray(elements);
      fiType = kGO4CONDWINDOW;
      for(Int_t i = 0; i < elements; i++)
         condarr->AddLast(new TGo4WinCond(TString::Format("%s%06d",name,i).Data()));
   } else
   if(strcmp(type,"TGo4PolyCond") == 0) {
      condarr = new TObjArray(elements);
      fiType = kGO4CONDPOLYGON;
      for(Int_t i = 0; i < elements; i++)
         condarr->AddLast(new TGo4PolyCond(TString::Format("%s%06d",name,i).Data()));

      TGo4PolyCond::CleanupSpecials(); // JAM2016
   } else {
      fiNumCond = 0;
      fiType = 0;
   }
}
// ----------------------------------------------------------
TGo4CondArray::~TGo4CondArray()
{
   if(fxPainter) delete fxPainter; // delete painter before this subclass is gone
                                   // when called in TGo4Condition dtor only, painter
                                   // cannot cast correctly on TGo4CondArray!
   fxPainter = nullptr;
   if (condarr) {
       condarr->Delete();
       delete condarr;
       condarr = nullptr;
   }

}

// ----------------------------------------------------------

Bool_t TGo4CondArray::Test(Double_t x, Double_t y)
{
   Bool_t rev = kTRUE;
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         IncCounts();
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         Bool_t result = cond->Test(x, y);
         if (result)
            IncTrueCounts();
         rev &= result;
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->Test(x, y);
   }
   return rev;
}

// ----------------------------------------------------------
Bool_t TGo4CondArray::Test(Double_t x)
{
   Bool_t rev = kTRUE;
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         IncCounts();
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         Bool_t result = cond->Test(x);
         if (result)
            IncTrueCounts();
         rev &= result;
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->Test(x);
   }
   return rev;
}

// ----------------------------------------------------------
void TGo4CondArray::PrintCondition(Bool_t points)
{
   // all entries are used!
   Int_t ii = condarr->GetLast()+1;
   TGo4Condition *cond = (TGo4Condition *) condarr->At(0);
   std::cout << GetName() << " has " << ii << " " << cond->ClassName() << " conditions" << std::endl;
   for(Int_t i = 0; i < ii; i++) {
      cond = dynamic_cast<TGo4Condition *> (condarr->UncheckedAt(i));
      if (cond) cond->PrintCondition(points);
   }
}

// ----------------------------------------------------------
void TGo4CondArray::PrintBar()
{
   Int_t ii = condarr->GetLast()+1;
   std::cout <<"-"<<ClassName()<<" "<<GetName()<<" Printout:" << std::endl;
   //TROOT::IncreaseDirLevel();
   for(Int_t i = 0; i < ii; i++)
   {
      TGo4Condition *cond = dynamic_cast<TGo4Condition *> (condarr->UncheckedAt(i));
      TROOT::IndentLevel();
      if (cond) cond->PrintBar();
   }
   //TROOT::DecreaseDirLevel();
}

// ----------------------------------------------------------
Int_t TGo4CondArray::GetNumberOfConditions() const
{
   return GetNumber();
}

// ----------------------------------------------------------
Int_t TGo4CondArray::GetNumber() const
{
  return (condarr->GetLast()+1);
}

// ----------------------------------------------------------
const char *TGo4CondArray::GetType() const
{
   TGo4Condition *cond = (TGo4Condition *) condarr->At(0);
   return cond->ClassName();
}

// -----------------------------------------------
void TGo4CondArray::GetType(Int_t &type) const
{
  type = fiType;
}
// -----------------------------------------------
void TGo4CondArray::Enable()
{
   TGo4Condition::Enable();
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->Enable();
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->Enable();
   }
}
// -----------------------------------------------
void TGo4CondArray::Disable(Bool_t result)
{
   TGo4Condition::Disable(result);
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->Disable(result);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->Disable(result);
   }
}
// -----------------------------------------------
void TGo4CondArray::Invert(Bool_t on)
{
   TGo4Condition::Invert(on);
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->Invert(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->Invert(on);
   }
}

void TGo4CondArray::MarkReset(Bool_t on)
{
   TGo4Condition::MarkReset(on);
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->MarkReset(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->MarkReset(on);
   }
}

void TGo4CondArray::GetFlags(Bool_t *enabled, Bool_t *lastresult, Bool_t *markreset, Bool_t *result, Bool_t *truevalue,
                             Bool_t *falsevalue)
{
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->GetFlags(enabled, lastresult, markreset, result, truevalue, falsevalue);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->GetFlags(enabled, lastresult, markreset, result, truevalue, falsevalue);
   }
}

// -----------------------------------------------
void TGo4CondArray::ResetCounts()
{
   TGo4Condition::ResetCounts();
   if (IsMultiEdit()) {
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->ResetCounts();
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->ResetCounts();
   }
}

Int_t TGo4CondArray::Counts()
{
   Int_t rev = 0;
   if (IsMultiEdit()) {
      rev = TGo4Condition::Counts();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->Counts();
   }
   return rev;
}
// ---------------------------------------------------------
Int_t TGo4CondArray::TrueCounts()
{
   Int_t rev = 0;
   if (IsMultiEdit()) {
      rev = TGo4Condition::Counts();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->Counts();
   }
   return rev;
}

Double_t TGo4CondArray::GetXLow() const
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -1;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetXLow();
   }
   return rev;
}

Double_t TGo4CondArray::GetXUp() const
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -1;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetXUp();
   }
   return rev;
}

Double_t TGo4CondArray::GetYLow() const
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -1;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetYLow();
   }
   return rev;
}

Double_t TGo4CondArray::GetYUp() const
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -1;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetYUp();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetValues(Double_t low1, Double_t up1)
{
   if (fiType == kGO4CONDWINDOW) {
      if (IsMultiEdit()) {
         Int_t ii = condarr->GetLast() + 1;
         for (Int_t i = 0; i < ii; i++) {
            TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
            cond->SetValues(low1, up1);
         }
      } else {
         TGo4Condition *conny = At(GetCurrentIndex());
         if (conny)
            conny->SetValues(low1, up1);
      }
   }
}
// -----------------------------------------------
void TGo4CondArray::SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2)
{
   if (fiType == kGO4CONDWINDOW) {
      if (IsMultiEdit()) {
         Int_t ii = condarr->GetLast() + 1;
         for (Int_t i = 0; i < ii; i++) {
            TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
            cond->SetValues(low1, up1, low2, up2);
         }
      } else {
         TGo4Condition *conny = At(GetCurrentIndex());
         if (conny)
            conny->SetValues(low1, up1, low2, up2);
      }
   }
}

// -----------------------------------------------
void TGo4CondArray::SetValues(TCutG *newcut)
{
   if (fiType == kGO4CONDPOLYGON) {
      if (IsMultiEdit()) {
         Int_t ii = condarr->GetLast() + 1;
         for (Int_t i = 0; i < ii; i++) {
            TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
            cond->SetValues(newcut);
         }
      } else {
         TGo4Condition *conny = At(GetCurrentIndex());
         if (conny)
            conny->SetValues(newcut);
      }
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::UpdateFrom(TGo4Condition *cond, Bool_t counts)
{
   TGo4Condition::UpdateFrom(cond, counts); // make sure to copy general flags
   TGo4Condition *scond;                    // source member
   TGo4Condition *dcond;                    // destination member
   TGo4CondArray *carr;                     // source array
   if (cond->InheritsFrom(TGo4CondArray::Class())) {
      Int_t dii = condarr->GetLast() + 1;               // this is destination
      Int_t sii = ((TGo4CondArray *)cond)->GetNumber(); // elements in source array
      if (dii == sii) {
         Bool_t result = kTRUE;
         carr = (TGo4CondArray *)cond;
         //           std::cout << GetName() << ": Update " << dii << " from " << cond->GetName() << std::endl;
         for (Int_t i = 0; i < dii; i++) {
            dcond = (TGo4Condition *)condarr->UncheckedAt(i); // destination is this
            scond = (TGo4Condition *)(*carr)[i];              // source is from cond
            result = result && (dcond->UpdateFrom(scond, counts));
         }
         return result;
      } else {
         return kFALSE;
      }
   } else {
      return kFALSE;
   }
}

// -----------------------------------------------
void TGo4CondArray::SetVisible(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetVisible(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetVisible(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetVisible(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsVisible() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsVisible();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsVisible();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetLabelDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetLabelDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetLabelDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetLabelDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsLabelDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsLabelDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsLabelDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetLimitsDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetLimitsDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetLimitsDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetLimitsDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsLimitsDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsLimitsDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsLimitsDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetIntDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetIntDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetIntDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetIntDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsIntDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsIntDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsIntDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetXMeanDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetXMeanDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetXMeanDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetXMeanDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsXMeanDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsXMeanDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsXMeanDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetXRMSDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetXRMSDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetXRMSDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetXRMSDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsXRMSDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsXRMSDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsXRMSDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetYMeanDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetYMeanDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetYMeanDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetYMeanDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsYMeanDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsYMeanDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsYMeanDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetYRMSDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetYRMSDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetYRMSDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetYRMSDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsYRMSDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsYRMSDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsYRMSDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetXMaxDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetXMaxDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetXMaxDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetXMaxDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsXMaxDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsXMaxDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsXMaxDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetYMaxDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetYMaxDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetYMaxDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetYMaxDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsYMaxDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsYMaxDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsYMaxDraw();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetCMaxDraw(Bool_t on)
{
   if (IsMultiEdit()) {
      TGo4Condition::SetCMaxDraw(on);
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         cond->SetCMaxDraw(on);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         conny->SetCMaxDraw(on);
   }
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsCMaxDraw() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      rev = TGo4Condition::IsCMaxDraw();
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsCMaxDraw();
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetIntegral(TH1 *histo, Option_t *opt)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      // return sum of all integrals
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         rev += cond->GetIntegral(histo, opt);
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetIntegral(histo, opt);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetMean(TH1 *histo, Int_t axis)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      // give back mean value of means. does this make sense?
      Int_t ii = condarr->GetLast() + 1;
      Double_t sum = 0;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         sum += cond->GetMean(histo, axis);
         rev = sum / ii;
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetMean(histo, axis);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetRMS(TH1 *histo, Int_t axis)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -2; // what is the result RMS of all sub-conditions?
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetRMS(histo, axis);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetSkewness(TH1 *histo, Int_t axis)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -2; // what is the result Skewness of all subconditions?
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetSkewness(histo, axis);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetCurtosis(TH1 *histo, Int_t axis)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -2; // what is the result Curtosis of all subconditions?
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetCurtosis(histo, axis);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetXMax(TH1 *histo)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -2; // would need the xmax of the maximum content of all
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetXMax(histo);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetYMax(TH1 *histo)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      rev = -2;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetYMax(histo);
   }
   return rev;
}

// -----------------------------------------------
Double_t TGo4CondArray::GetCMax(TH1 *histo)
{
   Double_t rev = 0;
   if (IsMultiEdit()) {
      // return highest channel content of all subconditions
      Int_t ii = condarr->GetLast() + 1;
      for (Int_t i = 0; i < ii; i++) {
         TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
         Double_t lmax = cond->GetCMax(histo);
         if (lmax > rev)
            rev = lmax;
      }
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->GetCMax(histo);
   }
   return rev;
}

// -----------------------------------------------
const TGo4Condition *TGo4CondArray::GetActiveCondition() const
{
   if (IsMultiEdit())
      return this;
   else
      return At(GetCurrentIndex());
}

// -----------------------------------------------
void TGo4CondArray::SetChanged(Bool_t on)
{
   for (Int_t i = 0; i <= condarr->GetLast(); i++) {
      TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
      cond->SetChanged(on);
   }
}

// -----------------------------------------------
Int_t TGo4CondArray::IsChanged() const
{
   Int_t cnt = 0;
   for (Int_t i = 0; i <= condarr->GetLast(); i++) {
      TGo4Condition *cond = (TGo4Condition *)condarr->UncheckedAt(i);
      cnt += cond->IsChanged();
   }
   return cnt;
}

// -----------------------------------------------
Bool_t TGo4CondArray::IsPolygonType() const
{
   Bool_t rev = kFALSE;
   if (IsMultiEdit()) {
      if (fiType == kGO4CONDPOLYGON)
         rev = kTRUE;
      else
         rev = kFALSE;
   } else {
      TGo4Condition *conny = At(GetCurrentIndex());
      if (conny)
         rev = conny->IsPolygonType();
   }
   return rev;
}

// -----------------------------------------------
void TGo4CondArray::SetCurrentIndex(Int_t ix)
{
   fiSelectedCond = ix;
}

// -----------------------------------------------
Int_t TGo4CondArray::GetCurrentIndex() const
{
   return fiSelectedCond;
}

// -----------------------------------------------
void TGo4CondArray::Draw(Option_t *opt)
{
   TGo4Condition::Draw(opt);
   Int_t selid = -1;
   if (opt)
      if (strstr(opt, "sel=") == opt)
         selid = atoi(opt + 4);
   for (Int_t i = 0; i < GetNumber(); ++i) {
      TGo4Condition *con = At(i);
      bool selected = (selid < 0) || (selid == i);
      if (con)
         con->SetPainted(IsPainted() && selected);
   }
}

// -----------------------------------------------
void TGo4CondArray::SetPainter(TGo4ConditionPainter *painter)
{
   if (!painter)
      return;
   if (painter->InheritsFrom(TGo4CondArrayPainter::Class())) {
      if (fxPainter)
         delete fxPainter;
      fxPainter = painter;
   } else {
      TGo4Log::Warn("Could not set painter of class %s for TGo4CondArray %s", painter->ClassName(), GetName());
   }
}

// -----------------------------------------------
TGo4ConditionPainter *TGo4CondArray::CreatePainter()
{
   auto painter = new TGo4CondArrayPainter(GetName());
   painter->SetCondition(this);
   return painter;
}

// -----------------------------------------------
Int_t TGo4CondArray::GetMemorySize() const
{
   Int_t size = sizeof(*this);
   if (GetName())
      size += strlen(GetName());
   if (GetTitle())
      size += strlen(GetTitle());

   if (condarr)
      size += sizeof(*condarr) + condarr->GetEntriesFast() * sizeof(TObject *);

   for (Int_t n = 0; n < GetNumberOfConditions(); n++) {
      TGo4Condition *cond = At(n);
      if (cond)
         size += cond->GetMemorySize();
   }

   return size;
}

// -----------------------------------------------
void TGo4CondArray::SavePrimitive(std::ostream &out, Option_t *opt)
{
   static int cnt = 0;
   TString extraargs = TString::Format(", %d, \"%s\"", GetNumber(), GetType());
   TString varname = MakeScript(out, TString::Format("condarr%d", cnt++).Data(), opt, extraargs.Data());

   // exclude name: options
   TString options = opt;
   const char *subname = strstr(opt, "name:");
   if (subname)
      options.Resize(subname - opt);

   for (int n = 0; n < GetNumber(); n++) {
      TString subopt = options + TString::Format(" nocreate name:%s->At(%d)", varname.Data(), n);
      At(n)->SavePrimitive(out, subopt.Data());
      out << std::endl;
   }
}
