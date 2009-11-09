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

#include "Riostream.h"

#include "TList.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TBaseClass.h"
#include "TDatime.h"

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

   Int_t size = 0;
   TString localbuf = Form(" Parameter Class %s, name: %s \n", ClassName(), GetName());

   if(buffer==0)
      cout << localbuf << endl;
   else {
      size = localbuf.Length();
      if(size>buflen-1) size = buflen-1;
      strncpy(buffer,localbuf.Data(), size);
   }
   return size;
}

TGo4Parameter::~TGo4Parameter()
{
   TRACE((12,"TGo4Parameter ::~TGo4Parameter ()",__LINE__, __FILE__));
}

Bool_t TGo4Parameter::UpdateFrom(TGo4Parameter* rhs)
{
   if(rhs) return kFALSE;
   // this method should better be pure virtual.
   // however, we have to implement dummy for root
   // to let it clone and stream this with baseclass type
   cout << "GO4> !!! ERROR: TGo4Parameter::UpdateFrom() not implemented!!!" << endl;
   cout << "GO4> Please overwrite virtual method in your class: " << this->ClassName() << endl;
   return kFALSE;
}

void TGo4Parameter::Clear(Option_t* opt)
{
 // dummy clear, may be implemented by user
  cout << "GO4> !!! ERROR: TGo4Parameter::Clear() not implemented!!!" << endl;
  cout << "GO4> Please overwrite virtual method in your class: ";
  cout << this->ClassName() << endl;
}

void TGo4Parameter::GetMemberValues(TObjArray* fItems)
{
   GetMemberValues(fItems, IsA(), (char*) this, 0);
}

Bool_t TGo4Parameter::SetMemberValues(TObjArray* fItems)
{
   if (fItems==0) return kFALSE;

   TIterator* iter = fItems->MakeIterator();

   Bool_t res = SetMemberValues(iter, IsA(), (char*) this, 0);

   delete iter;

   return res;
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

      if (strcmp(memtypename,"TString")==0)
        memtypeid = TGo4ParameterMember::kTString_t;
      else
      if (strcmp(memtypename,"TGo4Fitter*")==0)
        memtypeid = TGo4ParameterMember::kTGo4Fitter_t;
      else {
        if (!member->IsBasic()) continue;
        memtypeid = member->GetDataType()->GetType();
      }

      Int_t unitsize = member->GetUnitSize();
      Int_t arraydim = member->GetArrayDim();

      Int_t maxindex1 = 1;
      Int_t maxindex2 = 1;

      switch(arraydim) {
        case 1:
          maxindex1 = member->GetMaxIndex(0);
          break;
        case 2:
          maxindex1 = member->GetMaxIndex(0);
          maxindex2 = member->GetMaxIndex(1);
          break;
      } // switch()

      lastmemberid++;

      for(Int_t ix1=0;ix1<maxindex1;ix1++)
        for(Int_t ix2=0;ix2<maxindex2;ix2++) {
          TGo4ParameterMember* info = new TGo4ParameterMember(member->GetName(), member->GetTitle());
          fItems->Add(info);

          info->SetMemberId(lastmemberid);

          info->SetType(memtypename, memtypeid);

          info->SetVisible((ix1==0) && (ix2==0));

          info->SetArrayIndexes(arraydim, ix1, ix2);

          char* addr = ptr + cloffset + member->GetOffset() + (ix1*maxindex2 + ix2) * unitsize;
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

Bool_t TGo4Parameter::SetMemberValues(TIterator* fItems, TClass* cl, char* ptr, unsigned long int cloffset)
{
   if ((fItems==0) || (cl==0) || (ptr==0)) return kFALSE;

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

      if (strcmp(memtypename,"TString")==0)
        memtypeid = TGo4ParameterMember::kTString_t;
      else
      if (strcmp(memtypename,"TGo4Fitter*")==0)
        memtypeid = TGo4ParameterMember::kTGo4Fitter_t;
      else {
        if (!member->IsBasic()) continue;
        memtypeid = member->GetDataType()->GetType();
      }

      Int_t unitsize = member->GetUnitSize();
      Int_t arraydim = member->GetArrayDim();

      Int_t maxindex1 = 1;
      Int_t maxindex2 = 1;

      switch(arraydim) {
        case 1:
          maxindex1 = member->GetMaxIndex(0);
          break;
        case 2:
          maxindex1 = member->GetMaxIndex(0);
          maxindex2 = member->GetMaxIndex(1);
          break;
      } // switch()

      for(Int_t ix1=0;ix1<maxindex1;ix1++)
        for(Int_t ix2=0;ix2<maxindex2;ix2++) {
          TGo4ParameterMember* info =
            dynamic_cast<TGo4ParameterMember*> (fItems->Next());
          if (info==0) return kFALSE;

          if (strcmp(info->GetName(), member->GetName())!=0) return kFALSE;
          if (strcmp(info->GetTitle(), member->GetTitle())!=0) return kFALSE;

          if (strcmp(info->GetTypeName(), memtypename)!=0) return kFALSE;
          if (info->GetTypeId() != memtypeid) return kFALSE;

          if (!info->CheckArrayIndexes(arraydim, ix1, ix2)) return kFALSE;

          char* addr = ptr + cloffset + member->GetOffset() + (ix1*maxindex2 + ix2) * unitsize;
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

      if (!SetMemberValues(fItems, bclass, ptr, cloffset + baseclass->GetDelta())) return kFALSE;
   }

   return kTRUE;
}

void TGo4Parameter::SavePrimitive(ostream& out, Option_t* opt)
{
   static int cnt = 0;
   TString varname = Form("param%d", cnt++);
   Bool_t savemacro = (opt!=0) && (strstr(opt,"savemacro")!=0);

   if (savemacro) {
      out << Form("   %s* %s = (%s*) go4->GetObject(\"%s\",\"Go4\");",
                 ClassName(), varname.Data(), ClassName(), GetName()) << endl << endl;
      out << Form("   if (%s==0) {", varname.Data()) << endl;
      out << Form("      cout << \"Could not find parameter %s\" << endl;", GetName()) << endl;
      out << Form("      return kFALSE;") << endl;
      out << Form("   }") << endl << endl;
      out << Form("   if (strcmp(%s->ClassName(), \"%s\") != 0) {", varname.Data(), ClassName()) << endl;
      out << Form("      cout << \"Parameter %s has wrong class \" << %s->ClassName() << endl;", GetName(), varname.Data()) << endl;
      out << Form("      return kFALSE;") << endl;
      out << Form("   }") << endl << endl;
      out << Form("   cout << \"Set parameter %s as saved at %s\" << endl;", GetName(), TDatime().AsString()) << endl << endl;
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

      if (info->GetTypeId()==TGo4ParameterMember::kTString_t)
         out << Form("   %s->%s = \"%s\";", varname.Data(), membername.Data(), info->GetStrValue()) << endl;
      else
      if (info->GetTypeId()==kBool_t)
         out << Form("   %s->%s = %s;", varname.Data(), membername.Data(), (atoi(info->GetStrValue()) ? "kTRUE" : "kFALSE")) << endl;
      else
         out << Form("   %s->%s = %s;", varname.Data(), membername.Data(), info->GetStrValue()) << endl;
   }

   delete fitems;
}

