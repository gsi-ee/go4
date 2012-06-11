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

#include "TGo4Parameter.h"

#include <string.h>
#include <stdlib.h>

//#include "go4iostream.h"

#include "TList.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TROOT.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TBaseClass.h"
#include "TDatime.h"
#include "TArrayI.h"
#include "TArrayD.h"

#include "TGo4Log.h"
#include "TGo4ParameterMember.h"

TGo4Parameter::TGo4Parameter() :
   TNamed()
{
}

TGo4Parameter::TGo4Parameter(const char* name, const char* title) :
   TNamed(name ? name : "Parameter", title)
{
   TRACE((12,"TGo4Parameter ::TGo4Parameter (const char*, const char*)",__LINE__, __FILE__));

}

void TGo4Parameter::Print(Option_t* dummy) const
{
   // this trick is needed since root defines Print as const function...
   TGo4Parameter* const localthis= const_cast<TGo4Parameter* const>(this);
   localthis->PrintParameter();
}

Int_t TGo4Parameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4Parameter ::PrintParameter()",__LINE__, __FILE__));
   //
   if(buflen<=0 && buffer!=0) return 0;

   TObjArray items;
   GetMemberValues(&items);

   Int_t size = 0;
   TString localbuf = TString::Format("Parameter name: %s class %s\n", GetName(), ClassName());

   if(buffer==0) {
      cout << localbuf;
   } else {
      size = localbuf.Length();
      if(size>buflen) size = buflen;
      strncpy(buffer, localbuf.Data(), size);
      buflen -= size;
      buffer += size;
   }

   TIter iter(&items);
   TGo4ParameterMember* info = 0;

   TROOT::IncreaseDirLevel();

   while ((info = (TGo4ParameterMember*) iter()) !=0 ) {
      Int_t size1 = info->PrintMember(buffer, buflen);

      size += size1;
      buflen -= size1;
      buffer += size1;
   }

   TROOT::DecreaseDirLevel();

   return size;
}

TGo4Parameter::~TGo4Parameter()
{
   TRACE((12,"TGo4Parameter ::~TGo4Parameter ()",__LINE__, __FILE__));
}

Bool_t TGo4Parameter::UpdateFrom(TGo4Parameter* rhs)
{
   if (rhs==0) return kFALSE;

   if (rhs->IsA() != IsA()) {
      cout << "GO4> !!! ERROR: Wrong parameter class is used in TGo4Parameter::UpdateFrom() method!!!" << endl;
      cout << "GO4> !!! ERROR: One cannot update " << IsA()->GetName() << " class from " << rhs->IsA()->GetName() << endl;
      cout << "GO4> !!! ERROR: Implement your custom UpdateFrom() method" << endl;
      return kFALSE;
   }

   TObjArray items;

   rhs->GetMemberValues(&items);

   return SetMemberValues(&items);
}

void TGo4Parameter::Clear(Option_t* opt)
{
 // dummy clear, may be implemented by user
  cout << "GO4> !!! Default TGo4Parameter::Clear() method is used." <<  endl;
  cout << "GO4> !!! You probably need to overwrite Clear() method for your class" << IsA()->GetName() << endl;

  TObjArray items;

  GetMemberValues(&items);

  TIter iter(&items);
  TGo4ParameterMember* info = 0;

  while ((info = (TGo4ParameterMember*) iter()) !=0 ) info->SetToZero();

  SetMemberValues(&items);
}

void TGo4Parameter::GetMemberValues(TObjArray* fItems)
{
   GetMemberValues(fItems, IsA(), (char*) this, 0);
}

Bool_t TGo4Parameter::SetMemberValues(TObjArray* items)
{
   if (items==0) return kFALSE;

   Int_t indx(0);

   return SetMemberValues(items, indx, IsA(), (char*) this, 0);
}


void TGo4Parameter::GetMemberValues(TObjArray* fItems, TClass* cl, char* ptr, unsigned long int cloffset)
{
   if ((fItems==0) || (cl==0) || (ptr==0)) return;

   TIter iter(cl->GetListOfDataMembers());
   Int_t lastmemberid = -1;
   if (fItems->GetLast()>=0)
      lastmemberid = ((TGo4ParameterMember*) fItems->Last())->GetMemberId();

   TObject* obj = 0;
   while ((obj=iter()) != 0) {
      TDataMember* member = dynamic_cast<TDataMember*>(obj);
      if (member==0) continue;
      const char* memtypename = member->GetFullTypeName();
      Int_t memtypeid = 0;

      // do not edit IsA info
      if(strcmp(memtypename,"TClass*")==0) continue;
      // skip for a moment all types which are not basic types

      Int_t arraydim = member->GetArrayDim();
      if (arraydim>2) continue;

      Int_t maxindex1(1), maxindex2(1);

      switch(arraydim) {
        case 1:
          maxindex1 = member->GetMaxIndex(0);
          break;
        case 2:
          maxindex1 = member->GetMaxIndex(0);
          maxindex2 = member->GetMaxIndex(1);
          break;
      } // switch()

      TArrayI* arri(0);
      TArrayD* arrd(0);

      if (strcmp(memtypename,"TString")==0) {
         memtypeid = TGo4ParameterMember::kTString_t;
      } else
      if (strcmp(memtypename,"TGo4Fitter*")==0) {
         memtypeid = TGo4ParameterMember::kTGo4Fitter_t;
      } else
      if (strcmp(memtypename,"TArrayI")==0) {
         memtypeid = kInt_t;
         memtypename = "Int_t";
         if ((maxindex1>1) || (maxindex2>1)) continue;
         arri = (TArrayI*) (ptr + cloffset + member->GetOffset());
         arraydim = 1;
         maxindex1 = arri->GetSize();
         maxindex2 = 1;
      } else
      if (strcmp(memtypename,"TArrayD")==0) {
         memtypeid = kDouble_t;
         memtypename = "Double_t";
         if ((maxindex1>1) || (maxindex2>1)) continue;
         arrd = (TArrayD*) (ptr + cloffset + member->GetOffset());
         arraydim = 1;
         maxindex1 = arrd->GetSize();
         maxindex2 = 1;
      } else {
         if (!member->IsBasic()) continue;
         memtypeid = member->GetDataType()->GetType();
      }

      // add extra filed with array size
      if (arri || arrd) {
         TGo4ParameterMember* info = new TGo4ParameterMember(member->GetName(), member->GetTitle());

         info->SetMemberId(lastmemberid++);

         info->SetType("TArray", TGo4ParameterMember::kTArray_t);
         info->SetVisible(kFALSE);
         //info->SetArrayIndexes(0, 0, 0);
         info->SetIntValue(maxindex1);

         fItems->Add(info);
      }

      lastmemberid++;

      for(Int_t ix1=0;ix1<maxindex1;ix1++)
        for(Int_t ix2=0;ix2<maxindex2;ix2++) {
          TGo4ParameterMember* info = new TGo4ParameterMember(member->GetName(), member->GetTitle());
          fItems->Add(info);

          info->SetMemberId(lastmemberid);

          info->SetType(memtypename, memtypeid);

          info->SetVisible((ix1==0) && (ix2==0));

          info->SetArrayIndexes(arraydim, ix1, ix2);

          char* addr = ptr + cloffset + member->GetOffset() + (ix1*maxindex2 + ix2) * member->GetUnitSize();
          if (arri) addr = (char*) (arri->GetArray() + ix1);
          if (arrd) addr = (char*) (arrd->GetArray() + ix1);

          info->SetValue(addr);
        }
   }

   // expand base classes
   TIter cliter(cl->GetListOfBases());
   while((obj=cliter()) !=0) {
      TBaseClass* baseclass = dynamic_cast<TBaseClass*>(obj);
      if (baseclass==0) continue;
      TClass* bclass = baseclass->GetClassPointer();
      if(bclass==0) continue;
      if(strcmp(bclass->GetName(), "TGo4Parameter")==0) continue;
      if(strcmp(bclass->GetName(), "TNamed")==0) continue;

      GetMemberValues(fItems, bclass, ptr, cloffset + baseclass->GetDelta());
   }
}

Int_t TGo4Parameter::FindArrayLength(TObjArray* items, Int_t& itemsindx, TDataMember* member)
{
   TGo4ParameterMember* info = dynamic_cast<TGo4ParameterMember*> (items->At(itemsindx++));
   if (info==0) return -1;
   if (strcmp(info->GetName(), member->GetName())!=0) return -1;
   if (strcmp(info->GetTitle(), member->GetTitle())!=0) return -1;
   if (info->GetTypeId() != TGo4ParameterMember::kTArray_t) return -1;
   return info->GetIntValue();
}


Bool_t TGo4Parameter::SetMemberValues(TObjArray* items, Int_t& itemsindx, TClass* cl, char* ptr, unsigned long int cloffset)
{
   if ((items==0) || (cl==0) || (ptr==0)) return kFALSE;

   TIter iter(cl->GetListOfDataMembers());

   TObject* obj = 0;
   while ((obj=iter()) != 0) {
      TDataMember* member = dynamic_cast<TDataMember*>(obj);
      if (member==0) continue;
      const char* memtypename = member->GetFullTypeName();
      Int_t memtypeid = 0;

      // do not edit IsA info
      if(strcmp(memtypename,"TClass*")==0) continue;
      // skip for a moment all types which are not basic types

      Int_t arraydim = member->GetArrayDim();
      if (arraydim>2) continue;

      Int_t maxindex1(1), maxindex2(1);

      switch(arraydim) {
        case 1:
          maxindex1 = member->GetMaxIndex(0);
          break;
        case 2:
          maxindex1 = member->GetMaxIndex(0);
          maxindex2 = member->GetMaxIndex(1);
          break;
      } // switch()

      TArrayI* arri(0);
      TArrayD* arrd(0);

      if (strcmp(memtypename,"TString")==0) {
        memtypeid = TGo4ParameterMember::kTString_t;
      } else
      if (strcmp(memtypename,"TGo4Fitter*")==0) {
        memtypeid = TGo4ParameterMember::kTGo4Fitter_t;
      } else
      if (strcmp(memtypename,"TArrayI")==0) {
         memtypeid = kInt_t;
         memtypename = "Int_t";
         if ((maxindex1>1) || (maxindex2>1)) continue;
         arri = (TArrayI*) (ptr + cloffset + member->GetOffset());
         arraydim = 1;
         maxindex1 = FindArrayLength(items, itemsindx, member); // here we need to define index ourself
         maxindex2 = 1;
         if (maxindex1<0) return kFALSE;
         if (arri->GetSize()!=maxindex1) arri->Set(maxindex1);
      } else
      if (strcmp(memtypename,"TArrayD")==0) {
         memtypeid = kDouble_t;
         memtypename = "Double_t";
         if ((maxindex1>1) || (maxindex2>1)) continue;
         arrd = (TArrayD*) (ptr + cloffset + member->GetOffset());
         arraydim = 1;
         maxindex1 = FindArrayLength(items, itemsindx, member); // here we need to define index ourself
         maxindex2 = 1;
         if (maxindex1<0) return kFALSE;
         if (arrd->GetSize()!=maxindex1) arrd->Set(maxindex1);
      } else {
        if (!member->IsBasic()) continue;
        memtypeid = member->GetDataType()->GetType();
      }

      for(Int_t ix1=0;ix1<maxindex1;ix1++)
        for(Int_t ix2=0;ix2<maxindex2;ix2++) {
           if (itemsindx>items->GetLast()) return kFALSE;
           TGo4ParameterMember* info =
              dynamic_cast<TGo4ParameterMember*> (items->At(itemsindx++));
           if (info==0) return kFALSE;

          if (strcmp(info->GetName(), member->GetName())!=0) return kFALSE;
          if (strcmp(info->GetTitle(), member->GetTitle())!=0) return kFALSE;

          if (strcmp(info->GetTypeName(), memtypename)!=0) return kFALSE;
          if (info->GetTypeId() != memtypeid) return kFALSE;

          if (!info->CheckArrayIndexes(arraydim, ix1, ix2)) return kFALSE;

          char* addr = ptr + cloffset + member->GetOffset() + (ix1*maxindex2 + ix2) * member->GetUnitSize();
          if (arri) addr = (char*) (arri->GetArray() + ix1);
          if (arrd) addr = (char*) (arrd->GetArray() + ix1);

          info->GetValue(addr);
        }
   }

   // expand base classes
   TIter cliter(cl->GetListOfBases());
   while((obj=cliter()) !=0) {
      TBaseClass* baseclass = dynamic_cast<TBaseClass*>(obj);
      if (baseclass==0) continue;
      TClass* bclass = baseclass->GetClassPointer();
      if(bclass==0) continue;
      if(strcmp(bclass->GetName(), "TGo4Parameter")==0) continue;
      if(strcmp(bclass->GetName(), "TNamed")==0) continue;

      if (!SetMemberValues(items, itemsindx, bclass, ptr, cloffset + baseclass->GetDelta())) return kFALSE;
   }

   return kTRUE;
}

void TGo4Parameter::SavePrimitive(ostream& out, Option_t* opt)
{
   static int cnt = 0;
   TString varname = TString::Format("param%d", cnt++);
   Bool_t savemacro = (opt!=0) && (strstr(opt,"savemacro")!=0);

   if (savemacro) {
      out << Form("   %s* %s = (%s*) go4->GetParameter(\"%s\",\"%s\");",
                 ClassName(), varname.Data(), ClassName(), GetName(), ClassName()) << endl << endl;
      out << Form("   if (%s==0) {", varname.Data()) << endl;
      out << Form("      TGo4Log::Error(\"Could not find parameter %s of class %s\");", GetName(), ClassName()) << endl;
      out << Form("      return;") << endl;
      out << Form("   }") << endl << endl;
      out << Form("   TGo4Log::Info(\"Set parameter %s as saved at %s\");", GetName(), TDatime().AsString()) << endl << endl;
   } else {
      out << Form("   %s* %s = new %s;", ClassName(), varname.Data(), ClassName()) << endl;
      out << Form("   %s->SetName(\"%s\");", varname.Data(), GetName()) << endl;
      out << Form("   %s->SetTitle(\"%s\");", varname.Data(), GetTitle()) << endl;
   }

   TObjArray *fitems = new TObjArray();
   fitems->SetOwner(kTRUE);
   GetMemberValues(fitems);

   TIter iter(fitems);
   TGo4ParameterMember* info = 0;

   while ((info = (TGo4ParameterMember*) iter()) !=0 ) {
      if (info->GetTypeId()==TGo4ParameterMember::kTGo4Fitter_t) continue;

      TString membername;
      info->GetFullName(membername);

      switch (info->GetTypeId()) {
         case TGo4ParameterMember::kTString_t:
            out << Form("   %s->%s = \"%s\";", varname.Data(), membername.Data(), info->GetStrValue()) << endl;
            break;
         case TGo4ParameterMember::kTGo4Fitter_t:
            out << Form("   // fitter %s->%s ignored", varname.Data(), membername.Data()) << endl;
            break;
         case TGo4ParameterMember::kTArray_t:
            out << Form("   %s->%s.Set(%d);", varname.Data(), membername.Data(), info->GetIntValue()) << endl;
            break;
         case kBool_t:
            out << Form("   %s->%s = %s;", varname.Data(), membername.Data(), (info->GetIntValue() ? "kTRUE" : "kFALSE")) << endl;
            break;
         default:
            out << Form("   %s->%s = %s;", varname.Data(), membername.Data(), info->GetStrValue()) << endl;
            break;
      }
   }

   delete fitems;
}

