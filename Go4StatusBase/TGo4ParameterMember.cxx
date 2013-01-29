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

#include "TGo4ParameterMember.h"

#include <stdlib.h>

#include "RVersion.h"
#include "Riostream.h"
#include "TROOT.h"
#include "TDataType.h"

TGo4ParameterMember::TGo4ParameterMember() :
   TNamed(),
   fTypeName(),
   fTypeId(0),
   fMemberId(0),
   fValue(),
   fIndex1(-1),
   fIndex2(-1),
   fObject(0),
   fObjectOwner(kTRUE),
   fVisible(kTRUE)
{
}

TGo4ParameterMember::TGo4ParameterMember(const char* name, const char* title) :
   TNamed(name, title),
   fTypeName(),
   fTypeId(0),
   fMemberId(0),
   fValue(),
   fIndex1(-1),
   fIndex2(-1),
   fObject(0),
   fObjectOwner(kTRUE),
   fVisible(kTRUE)
{
}

TGo4ParameterMember::~TGo4ParameterMember()
{
   if ((fObject!=0) && fObjectOwner) {
      delete fObject;
      fObject = 0;
   }
}

void TGo4ParameterMember::SetArrayIndexes(Int_t ndim, Int_t indx1, Int_t indx2)
{
  switch(ndim) {
     case 1:
       fIndex1 = indx1;
       fIndex2 = -1;
       break;
     case 2:
       fIndex1 = indx1;
       fIndex2 = indx2;
       break;
     default:
       fIndex1 = -1;
       fIndex2 = -1;
       break;
  }
}

Bool_t TGo4ParameterMember::CheckArrayIndexes(Int_t ndim, Int_t indx1, Int_t indx2)
{
  switch(ndim) {
     case 1: return (fIndex1==indx1) && (fIndex2<0);
     case 2: return (fIndex1==indx1) && (fIndex2==indx2);
     default: return (fIndex1<0) && (fIndex2<0);
  }

  return (fIndex1<0) && (fIndex2<0);
}


const char* TGo4ParameterMember::GetFullName(TString& buf)
{
   buf = "";
   if ((fIndex1<0) && (fIndex2<0))
      buf = GetName();
   else
   if ((fIndex1>=0) && (fIndex2<0))
      buf.Form("%s[%d]",GetName(),fIndex1);
   else
   if ((fIndex1>=0) && (fIndex2>=0))
      buf.Form("%s[%d][%d]",GetName(),fIndex1,fIndex2);
   return buf.Data();
}

void TGo4ParameterMember::SetObject(TObject* obj, Bool_t owner)
{
   if ((fObject!=0) && fObjectOwner) delete fObject;

   fObject = obj;
   fObjectOwner = owner;
}

void TGo4ParameterMember::SetToZero()
{
   if ((fObject!=0) && fObjectOwner) {
      delete fObject;
      fObject = 0;
   }

   fValue = "";
   fObject = 0;
   fObjectOwner = kFALSE;

   switch (fTypeId) {
      case kUInt_t:
      case kInt_t:
      case kULong_t:
      case kLong_t:
      case kULong64_t:
      case kLong64_t:
      case kUShort_t:
      case kShort_t:
      case kUChar_t:
      case kChar_t:
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
      case kBool_t:
#endif
      case kFloat_t:
      case kDouble_t:
      case kDouble32_t: fValue = "0"; break;
      case kTString_t:  break;
      case kTGo4Fitter_t: break;
   }
}

void TGo4ParameterMember::SetValue(char* addr)
{
   fValue = "";
   switch (fTypeId) {
      case kUInt_t:     fValue.Form("%u", *((UInt_t*)addr)); break;
      case kInt_t:      fValue.Form("%d", *((Int_t*)addr)); break;
      case kULong_t:    fValue.Form("%lu", *((ULong_t*)addr)); break;
      case kLong_t:     fValue.Form("%ld", *((Long_t*)addr)); break;
      case kULong64_t:  fValue.Form("%llu", *((ULong64_t*)addr)); break;
      case kLong64_t:   fValue.Form("%lld", *((Long64_t*)addr)); break;
      case kUShort_t:   fValue.Form("%hu", *((UShort_t*)addr)); break;
      case kShort_t:    fValue.Form("%hd", *((Short_t*)addr)); break;
      case kUChar_t:    fValue.Form("%u", *((UChar_t*)addr)); break;
      case kChar_t:     fValue.Form("%d", *((Char_t*)addr)); break;
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
      case kBool_t:     fValue.Form("%d", *((Bool_t*)addr)); break;
#endif
      case kFloat_t:    fValue.Form("%f", *((Float_t*)addr)); break;
      case kDouble_t:   fValue.Form("%f", *((Double_t*)addr)); break;
      case kDouble32_t: fValue.Form("%f", *((Double32_t*)addr)); break;
      case kTString_t:  fValue = *((TString*)addr); break;
      case kTGo4Fitter_t: {
         TObject** f = (TObject**) addr;
         fObject = *f;
         fObjectOwner = kFALSE;
         break;
      }
   }
}

void TGo4ParameterMember::GetValue(char* addr)
{
   const char* value = fValue.Data();
   switch (fTypeId) {
     case kUInt_t:     *((UInt_t*)addr) = atoi(value); break;
     case kInt_t:      *((Int_t*)addr) = atoi(value); break;
     case kULong_t:    *((ULong_t*)addr) = atoi(value); break;
     case kLong_t:     *((Long_t*)addr) = atoi(value); break;
     case kULong64_t:  *((ULong64_t*)addr) = atoi(value); break;
     case kLong64_t:   *((Long64_t*)addr) = atoi(value); break;
     case kUShort_t:   *((UShort_t*)addr) = atoi(value); break;
     case kShort_t:    *((Short_t*)addr) = atoi(value); break;
     case kUChar_t:    *((UChar_t*)addr) = atoi(value); break;
     case kChar_t:     *((Char_t*)addr) = atoi(value); break;
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
     case kBool_t:     *((Bool_t*)addr) = atoi(value); break;
#endif
     case kFloat_t:    *((Float_t*)addr) = atof(value); break;
     case kDouble_t:   *((Double_t*)addr) = atof(value); break;
     case kDouble32_t: *((Double32_t*)addr) = atof(value); break;
     case kTString_t:  *((TString*)addr) = value; break;
     case kTGo4Fitter_t: {
       TObject** f = (TObject**)addr;
       *f = fObject;
       fObjectOwner = kFALSE;
       break;
     }
   }
}

Int_t TGo4ParameterMember::PrintMember(Text_t* buffer, Int_t buflen) const
{
   if ((buffer!=0) && (buflen<=0)) return 0;

   Int_t size = 0;

   TString name;
   if ((fIndex1<0) && (fIndex2<0)) name += GetName(); else
   if (fIndex2<0) name += TString::Format("%s[%d]", GetName(), fIndex1); else
      name += TString::Format("%s[%d][%d]", GetName(), fIndex1, fIndex2);

   name += " = ";

   if (fObject) name += TString::Format("Obj:%p Class:%s", fObject, fObject->ClassName()); else
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
      if (fTypeId==kBool_t) name += ((fValue=="0") ? "kFALSE" : "kTRUE"); else
#endif
         name += fValue;

   if ((GetTitle() != 0) && (strlen(GetTitle())>0)) { name += " // "; name += GetTitle(); }

   name += "\n";

   if(buffer==0) {
      TROOT::IndentLevel();
      std::cout << name;
   } else {
      size = name.Length();
      if(size>buflen) size = buflen;
      strncpy(buffer, name.Data(), size);
   }

   return size;

}


void TGo4ParameterMember::Clear(Option_t* opt)
{
   SetToZero();
}

void TGo4ParameterMember::Print(Option_t* dummy) const
{
   PrintMember();
}
