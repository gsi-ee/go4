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

#include "TGo4HServProxy.h"

#include "TH1.h"
#include "TH2.h"

#include "TGo4Slot.h"

extern "C" {
   #include "s_his_head.h"
   #include "f_his_hist.h"
}

class TGo4HServIter : public TGo4LevelIter {
   public:
      TGo4HServIter() : TGo4LevelIter() {}

      TGo4HServIter(const TGo4Slot* slot) : TGo4LevelIter(), fSlot(slot) {}

      virtual ~TGo4HServIter() {}

      Bool_t next() override
         { return fSlot && (++fIndex < fSlot->NumChilds()); }

      Bool_t isfolder() override { return curSlot()->HasSlotsSubLevels(); }

      TGo4LevelIter* subiterator() override { return new TGo4HServIter(curSlot()); }

      const char *name() override { return curSlot()->GetName();  }

      const char *info() override { return curSlot()->GetTitle(); }

      Int_t getflag(const char *flagname) override
      {
         if (strcmp(flagname,"IsRemote") == 0) return 1;
         return -1;
      }

      Int_t GetKind() override { return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject; }

      const char *GetClassName() override { return curSlot()->GetPar("::HistoClass"); }

    protected:
       TGo4Slot* curSlot() const { return fSlot->GetChild(fIndex); }

       const TGo4Slot* fSlot{nullptr};  //!
       Int_t      fIndex{-1};           //!
};

// ******************************************************************

class TGo4HServObjectAccess : public TGo4Access {
   public:
      TGo4HServObjectAccess(TGo4HServProxy* hserv,
                     const char *name,
                     const char *fullname,
                     const char *classname) :
         TGo4Access(),
         fHServ(hserv),
         fObjName(name),
         fObjFullName(fullname),
         fObjClassName(classname)
      {
      }

      virtual ~TGo4HServObjectAccess() {}

      Bool_t CanGetObject() const override { return fHServ!=nullptr; }

      Bool_t GetObject(TObject* &obj, Bool_t &owner) const override
      {
         if (!fHServ) return kFALSE;
         obj = fHServ->GetHistogram(fObjName.Data());
         if (obj) owner = kTRUE;
         return obj!=nullptr;
      }

      const char *GetObjectName() const override { return fObjName.Data(); }

      const char *GetObjectClassName() const override { return fObjClassName.Data(); }

   private:
      TGo4HServProxy *fHServ{nullptr};//!
      TString         fObjName;       //!
      TString         fObjFullName;   //!
      TString         fObjClassName;  //!
};



// ********************************************************************

TGo4HServProxy::TGo4HServProxy() :
   TGo4ServerProxy(),
   fServerName(),
   fPortNumber(0),
   fBaseName(),
   fUserPass(),
   fFilter(),
   fxStructure(nullptr)
{
}

TGo4HServProxy::~TGo4HServProxy()
{
   delete fxStructure;
}

void TGo4HServProxy::SetHServConfig(const char *servername,
                                        Int_t portnumber,
                                        const char *basename,
                                        const char *userpass,
                                        const char *filter)
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
   return fxStructure != nullptr;
}

TGo4LevelIter* TGo4HServProxy::MakeIter()
{
   return fxStructure ? new TGo4HServIter(fxStructure) : nullptr;
}

TGo4Access* TGo4HServProxy::ProvideAccess(const char *name)
{
   TGo4Slot* itemslot = fxStructure ? fxStructure->GetSlot(name) : nullptr;
   if (!itemslot) return nullptr;
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
   return fxStructure ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char *TGo4HServProxy::GetContainedClassName()
{
   return fxStructure ? ClassName() : nullptr;
}

void TGo4HServProxy::Update(TGo4Slot* slot, Bool_t strong)
{
}

Bool_t TGo4HServProxy::RefreshNamesList()
{
   if (fxStructure) {
      delete fxStructure;
      fxStructure = nullptr;
   }

   INTS4* pl_all_h = nullptr;
   INTS4 l_histos = 0;
   INTS4 result = f_his_getdir((CHARS *) fServerName.Data(),
                               fPortNumber,
                               (CHARS *) fBaseName.Data(),
                               (CHARS *) fUserPass.Data(),
                               (CHARS *) fFilter.Data(),
                               &pl_all_h,
                               &l_histos);

   if(result != 0) return kFALSE;

   fxStructure = new TGo4Slot(nullptr, "HClient","Structure holding slot");

   s_his_head* ps_his_head = (s_his_head*) pl_all_h;
   for(int i_j=0;i_j<l_histos;i_j++) {
      TString HisType="TH";
      if (ps_his_head->l_bins_2>1) {
         if(strstr(ps_his_head->c_dtype,"r"))
           HisType += "2F";
         else
           HisType += "2I";
      } else {
         if(strstr(ps_his_head->c_dtype,"r"))
            HisType += "1F";
         else
            HisType += "1I";
      }
//      std::cout << "name = " << ps_his_head->c_name
//           << "  type = " << HisType
//           << "  date = " << ps_his_head->c_data_time_cre << std::endl;

      TGo4Slot* child = fxStructure->GetSlot(ps_his_head->c_name, kTRUE);
      if (child) {
         child->SetPar("::HistoClass", HisType.Data());
         child->SetPar("::Date", ps_his_head->c_data_time_cre);
      }

      ps_his_head++;
   }

   free(pl_all_h);

   return kTRUE;
}

TH1* TGo4HServProxy::GetHistogram(const char *remotehistoname)
{
   s_his_head* ps_his_head = nullptr;
   INTS4* pl_all = nullptr;
   INTS4 l_size = 0;

   INTS4 result = f_his_gethis((CHARS *) fServerName.Data(),
                               fPortNumber,
                               (CHARS *) fBaseName.Data(),
                               (CHARS *) fUserPass.Data(),
                               (CHARS *) remotehistoname,
                               &ps_his_head,
                               &pl_all,
                               &l_size);
   if(result != 0) return nullptr; // error this connection

   if(l_size == 0) return nullptr; // no data in histogram at all

   int i1 = ps_his_head->l_bins_1;
   int i2 = ps_his_head->l_bins_2;

   REAL4* pr_all = strstr(ps_his_head->c_dtype,"r") ? (REAL4 *) pl_all : nullptr;
   INTS4* pl_start = pl_all;

   TH1* h1 = nullptr;
   Double_t entries = 0;

   if(i2==1) {    // 1-Dimensional histogram
      if (pr_all) {
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
      if (pr_all) {
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

   h1->SetDirectory(nullptr);
   h1->SetEntries(entries);
   h1->Sumw2();

   h1->GetXaxis()->SetTitle(ps_his_head->c_lettering_1);
   h1->GetXaxis()->CenterTitle();
   h1->GetYaxis()->CenterTitle();

   return h1;
}
