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

#include "TGo4HServProxy.h"

#include <stdlib.h>
#include "TH1.h"
#include "TH2.h"

#include "s_his_head.h"

#include "TGo4Slot.h"

extern "C"
{
   INTS4 f_his_getdir(const char*, int, const char*, const char*, const char*, INTS4**, INTS4*);
   INTS4 f_his_gethis(const char*, int, const char*, const char*, const char*, s_his_head**, INTS4**, INTS4*);
}


class TGo4HServIter : public TGo4LevelIter {
   public:
      TGo4HServIter() : TGo4LevelIter(), fSlot(0), fIndex(-1) {}

      TGo4HServIter(const TGo4Slot* slot) : TGo4LevelIter(), fSlot(slot), fIndex(-1) {}

      virtual ~TGo4HServIter() {}

      virtual Bool_t next()
         { return (fSlot!=0) && (++fIndex<fSlot->NumChilds()); }

      virtual Bool_t isfolder() { return curSlot()->HasSlotsSubLevels(); }

      virtual TGo4LevelIter* subiterator() { return new TGo4HServIter(curSlot()); }

      virtual const char* name() { return curSlot()->GetName();  }

      virtual const char* info() { return curSlot()->GetTitle(); }

      virtual Int_t getflag(const char* flagname)
      {
         if (strcmp(flagname,"IsRemote")==0) return 1;
         return -1;
      }

      virtual Int_t GetKind() { return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject; }

      virtual const char* GetClassName() { return curSlot()->GetPar("::HistoClass"); }

    protected:
       TGo4Slot* curSlot() const { return fSlot->GetChild(fIndex); }

       const TGo4Slot* fSlot;  //!
       Int_t      fIndex;      //!
};

// ******************************************************************

class TGo4HServObjectAccess : public TGo4Access {
   public:
      TGo4HServObjectAccess(TGo4HServProxy* hserv,
                     const char* name,
                     const char* fullname,
                     const char* classname) :
         TGo4Access(),
         fHServ(hserv),
         fObjName(name),
         fObjFullName(fullname),
         fObjClassName(classname)
      {
      }

      virtual ~TGo4HServObjectAccess() {}

      virtual Bool_t CanGetObject() const { return fHServ!=0; }

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const
      {
         if (fHServ==0) return kFALSE;
         obj = fHServ->GetHistogram(fObjName.Data());
         if (obj!=0) owner = kTRUE;
         return (obj!=0);
      }

      virtual const char* GetObjectName() const { return fObjName.Data(); }

      virtual const char* GetObjectClassName() const { return fObjClassName.Data(); }

   private:
      TGo4HServProxy* fHServ;         //!
      TString             fObjName;       //!
      TString             fObjFullName;   //!
      TString             fObjClassName;  //!
};



// ********************************************************************

TGo4HServProxy::TGo4HServProxy() :
   TGo4Proxy(),
   fServerName(),
   fPortNumber(0),
   fBaseName(),
   fUserPass(),
   fFilter(),
   fxStructure(0)
{
}

TGo4HServProxy::~TGo4HServProxy()
{
   delete fxStructure;
}

void TGo4HServProxy::SetHServConfig(const char* servername,
                                        Int_t portnumber,
                                        const char* basename,
                                        const char* userpass,
                                        const char* filter)
{
   fServerName = servername;
   fPortNumber = portnumber;
   fBaseName = basename;
   fUserPass = userpass;
   fFilter = filter;
}

void TGo4HServProxy::Initialize(TGo4Slot* slot)
{
}

void TGo4HServProxy::Finalize(TGo4Slot* slot)
{
}


Bool_t TGo4HServProxy::HasSublevels() const
{
   return fxStructure!=0;
}

TGo4LevelIter* TGo4HServProxy::MakeIter()
{
   return fxStructure!=0 ? new TGo4HServIter(fxStructure) : 0;
}

TGo4Access* TGo4HServProxy::MakeProxy(const char* name)
{
   if (fxStructure==0) return 0;

   TGo4Slot* itemslot = fxStructure->GetSlot(name);
   if (itemslot==0) return 0;
   return new TGo4HServObjectAccess(this, itemslot->GetName(), name, itemslot->GetPar("::HistoClass"));
}

void TGo4HServProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
}

void TGo4HServProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
}

Int_t TGo4HServProxy::GetObjectKind()
{
   return fxStructure!=0 ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4HServProxy::GetContainedClassName()
{
   return fxStructure!=0 ? ClassName() : 0;
}

void TGo4HServProxy::Update(TGo4Slot* slot, Bool_t strong)
{
}

Bool_t TGo4HServProxy::RequestHistosList()
{
   delete fxStructure;
   fxStructure = 0;

   INTS4* pl_all_h = 0;
   INTS4 l_histos = 0;
   INTS4 result = f_his_getdir(fServerName.Data(),
                               fPortNumber,
                               fBaseName.Data(),
                               fUserPass.Data(),
                               fFilter.Data(),
                               &pl_all_h,
                               &l_histos);

   if(result!=0) return kFALSE;

   fxStructure = new TGo4Slot(0, "HClient","Structure holding slot");

   s_his_head* ps_his_head = (s_his_head*) pl_all_h;
   for(int i_j=0;i_j<l_histos;i_j++) {
      TString HisType="TH";
      if (ps_his_head->l_bins_2>1) {
         if(strstr(ps_his_head->c_dtype,"r")!=0)
           HisType += "2F";
         else
           HisType += "2I";
      } else {
         if(strstr(ps_his_head->c_dtype,"r")!=0)
            HisType += "1F";
         else
            HisType += "1I";
      }
//      std::cout << "name = " << ps_his_head->c_name
//           << "  type = " << HisType
//           << "  date = " << ps_his_head->c_data_time_cre << std::endl;

      TGo4Slot* child = fxStructure->GetSlot(ps_his_head->c_name, kTRUE);
      if (child!=0) {
         child->SetPar("::HistoClass", HisType.Data());
         child->SetPar("::Date", ps_his_head->c_data_time_cre);
      }

      ps_his_head++;
   }

   free(pl_all_h);

   return kTRUE;
}

TH1* TGo4HServProxy::GetHistogram(const char* remotehistoname)
{

   s_his_head* ps_his_head = 0;
   INTS4* pl_all = 0;
   INTS4 l_size = 0;

   INTS4 result = f_his_gethis(fServerName.Data(),
                               fPortNumber,
                               fBaseName.Data(),
                               fUserPass.Data(),
                               remotehistoname,
                               &ps_his_head,
                               &pl_all,
                               &l_size);
   if(result!=0) return 0; // error this connection

   if(l_size==0) return 0; // no data in histogram at all

   int i1 = ps_his_head->l_bins_1;
   int i2 = ps_his_head->l_bins_2;

   REAL4* pr_all = (strstr(ps_his_head->c_dtype,"r")!=0) ? (REAL4 *) pl_all : 0;
   INTS4* pl_start = pl_all;

   TH1* h1 = 0;
   Double_t entries = 0;

   if(i2==1) {    // 1-Dimensional histogram
      if (pr_all!=0) {
        h1 = new TH1F(ps_his_head->c_name, ps_his_head->c_name,
                      i1, ps_his_head->r_limits_low, ps_his_head->r_limits_up);
        for (int k=0; k<i1; k++) {
           REAL4 val = *(pr_all++);
           h1->SetBinContent(k, val);
           entries+=val;
        }
      } else {
        h1 = new TH1I(ps_his_head->c_name, ps_his_head->c_name,
                      i1, ps_his_head->r_limits_low, ps_his_head->r_limits_up);
        for (int k=0; k<i1; k++) {
           INTS4 val = *(pl_all++);
           h1->SetBinContent(k, val);
           entries+=val;
        }
      }

      h1->GetYaxis()->SetTitle(ps_his_head->c_lettering_res);

   } else {  // 2-dimensional histogram
      if (pr_all!=0) {
         h1 = new TH2F(ps_his_head->c_name,ps_his_head->c_name,
                       i1, ps_his_head->r_limits_low, ps_his_head->r_limits_up,
                       i2, ps_his_head->r_limits_low_2, ps_his_head->r_limits_up_2);
         for (int k=0; k<i2; k++)
            for (int k1=0; k1<i1; k1++) {
               REAL4 val= *(pr_all++);
               h1->SetBinContent(k1,k,val);
               entries+=val;
           }
      } else {
         h1 = new TH2I(ps_his_head->c_name,ps_his_head->c_name,
                       i1, ps_his_head->r_limits_low, ps_his_head->r_limits_up,
                       i2, ps_his_head->r_limits_low_2, ps_his_head->r_limits_up_2);
         for (int k=0; k<i2; k++)
            for (int k1=0; k1<i1; k1++) {
               INTS4 val = *(pl_all++);
               h1->SetBinContent(k1,k,val);
               entries+=val;
            }
      }

      h1->GetYaxis()->SetTitle(ps_his_head->c_lettering_2);
   }

   free(pl_start);
   free(ps_his_head);

   h1->SetDirectory(0);
   h1->SetEntries(entries);
   h1->Sumw2();

   h1->GetXaxis()->SetTitle(ps_his_head->c_lettering_1);
   h1->GetXaxis()->CenterTitle();
   h1->GetYaxis()->CenterTitle();

   return h1;
}
