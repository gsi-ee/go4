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

#include "TGo4BrowserProxy.h"

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TList.h"
#include "TFolder.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TTimer.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TDatime.h"
#include "TLatex.h"
#include "TVirtualTreePlayer.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4ObjectProxy.h"
#include "TGo4DirProxy.h"
#include "TGo4LinkProxy.h"
#include "TGo4Iter.h"
#include "TGo4ObjectManager.h"

#include "TGo4Fitter.h"
#include "TGo4Parameter.h"
#include "TGo4Picture.h"
#include "TGo4PolyCond.h"
#include "TGo4WinCond.h"
#include "TGo4ShapedCond.h"
#include "TGo4ListCond.h"
#include "TGo4CondArray.h"
#include "TGo4HistogramEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4ExportManager.h"

#include "TGo4ServerProxy.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4HServProxy.h"
#include "TGo4DabcProxy.h"

// required for ROOT browser
// #include "TGo4BrowserItem.h"
// #include "TGClient.h"
// #include "TGMimeTypes.h"


class TGo4BrowserObjProxy : public TGo4ObjectProxy {
   public:
      TGo4BrowserObjProxy() : TGo4ObjectProxy() {}
      TGo4BrowserObjProxy(TGo4Slot* slot, TObject* obj, Bool_t owner) : TGo4ObjectProxy(obj, owner)
      {
        if (slot && obj)
          TGo4BrowserProxy::SetItemTimeDate(slot);
      }

      virtual ~TGo4BrowserObjProxy() {}

      Bool_t Use() const override { return kFALSE; }

      Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner) override
      {
        if (fObject == obj) return kTRUE;

        Bool_t updatedone = false;

        if (fObject && obj)
           if ((fObject->IsA() == obj->IsA()) &&
              (strcmp(fObject->GetName(), obj->GetName()) == 0))
                updatedone = TGo4BrowserProxy::UpdateObjectContent(fObject, obj);

        if (obj)
          TGo4BrowserProxy::SetItemTimeDate(slot);

        if (updatedone) {
           slot->ForwardEvent(slot, TGo4Slot::evObjUpdated);
           if (owner) delete obj;
           return kTRUE;
        }

        return TGo4ObjectProxy::AssignObject(slot, obj, owner);
      }

      void Update(TGo4Slot* slot, Bool_t strong) override
      {
         // can not update object which is not there
         if (!fObject && !strong) return;

         TGo4Slot* parent = slot->GetParent();
         TGo4BrowserProxy* browser = nullptr;

         while(parent)  {
            browser = dynamic_cast<TGo4BrowserProxy*> (parent->GetProxy());
            if (browser) break;
            parent = parent->GetParent();
         }
         if (browser)
           browser->RequestBrowserObject(slot);
      }
};

// ******************************************************************

TGo4BrowserProxy::TGo4BrowserProxy(const char* datapath,
                                   const char* viewpath,
                                   Bool_t withRootBrowser)
{
   fxDataPath = datapath;
   fxViewPath = viewpath;
   fbWithRootBrowser = withRootBrowser;
   fxMemoryPath = "Workspace";
}

TGo4BrowserProxy::~TGo4BrowserProxy()
{
   if (gDebug > 1)
      Info("~TGo4BrowserProxy","Called");

   if (fDummyTreePlayer) {
      delete fDummyTreePlayer;
      fDummyTreePlayer = nullptr;
   }

   if (fxWaitingList) {
      fxWaitingList->Delete();
      delete fxWaitingList;
      fxWaitingList = nullptr;
   }

   ClearClipboard();

   if (fxSyncTimer) {
      delete fxSyncTimer;
      fxSyncTimer = nullptr;
   }

   if (fxMonitorTimer) {
      delete fxMonitorTimer;
      fxMonitorTimer = nullptr;
   }

   if (gDebug > 1)
      Info("~TGo4BrowserProxy","Finished");
}

void TGo4BrowserProxy::Initialize(TGo4Slot* slot)
{
   fxOM = slot->GetOM();
   fxBrowserSlot = slot;
   fxBrowserPath = "";
   fxBrowserSlot->ProduceFullName(fxBrowserPath);

   gROOT->GetClass("TH1");
   gROOT->GetClass("TH1C");
   gROOT->GetClass("TH1D");
   gROOT->GetClass("TH1F");
   gROOT->GetClass("TH1I");
   gROOT->GetClass("TH1S");
   gROOT->GetClass("TH2");
   gROOT->GetClass("TH2C");
   gROOT->GetClass("TH2D");
   gROOT->GetClass("TH2F");
   gROOT->GetClass("TH2I");
   gROOT->GetClass("TH2S");
   gROOT->GetClass("TH3");
   gROOT->GetClass("TH3C");
   gROOT->GetClass("TH3D");
   gROOT->GetClass("TH3F");
   gROOT->GetClass("TH3I");
   gROOT->GetClass("TH3S");

   gROOT->GetClass("TGo4WinCond");
   gROOT->GetClass("TGo4PolyCond");
   gROOT->GetClass("TGo4ShapedCond");
   gROOT->GetClass("TGo4CondArray");

   gROOT->GetClass("TGo4Fitter");
   gROOT->GetClass("TGo4Parameter");
   gROOT->GetClass("TGo4Picture");

   gROOT->GetClass("TGo4DynamicEntry");
   gROOT->GetClass("TGo4HistogramEntry");
   gROOT->GetClass("TGo4TreeHistogramEntry");
   gROOT->GetClass("TLatex");
   gROOT->GetClass("TF1"); // JAM test
   gROOT->GetClass("TGo4AnalysisStatus");

   if (fxOM)
      fxOM->RegisterLink(fxOM->GetSlot(fxDataPath.Data()), slot, kTRUE);

   fiMonitoringPeriod = 0;
}

void TGo4BrowserProxy::Finalize(TGo4Slot* slot)
{
   if (fxOM)
      fxOM->UnregisterLink(slot);
   fxOM = nullptr;
   fxBrowserSlot = nullptr;
   fxBrowserPath = "";
}

Bool_t TGo4BrowserProxy::ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param)
{
   bool ischildevent = source->IsParent(slot);

//   Info("ProcessEvent","Source %x %s event %d", source, source->GetName(), id);

   if ((source!=slot) && !ischildevent) {
      if (source->GetParent()) {
         TGo4ServerProxy* serv = dynamic_cast<TGo4ServerProxy*>(source->GetParent()->GetProxy());
         if (serv && serv->IsGo4Analysis()) return kFALSE; // suppress message from analysis subitems
      }

      if (!fbBlockSync) {
         if (!fxSyncTimer) fxSyncTimer = new TTimer(this, 10, kTRUE);
         fbBlockSync = kTRUE;
         fxSyncTimer->Start(10, kTRUE);
      }

      return kFALSE; // do not process further
   } else if ((source != slot) && ischildevent) {
      if ((id == TGo4Slot::evObjUpdated) || (id == TGo4Slot::evObjAssigned)) {
         CheckWaitingList(source);
         slot->ForwardEvent(slot, TGo4Slot::evSubslotUpdated, (void*) source);
         CheckPictureMonitor(source);
      }
   }

   return kTRUE;
}

void TGo4BrowserProxy::DataSlotName(const char* item, TString& res)
{
   res = fxDataPath;
   res += "/";
   res += item;
}

void TGo4BrowserProxy::BrowserSlotName(const char* item, TString& res)
{
   res = fxBrowserPath;
   res += "/";
   res += item;
}

TGo4Slot* TGo4BrowserProxy::BrowserSlot(const char* item)
{
   TString slotname;
   BrowserSlotName(item, slotname);
   return fxOM->GetSlot(slotname.Data());
}

TGo4Slot* TGo4BrowserProxy::DataSlot(const char* item)
{
   TString slotname;
   DataSlotName(item, slotname);
   return fxOM->GetSlot(slotname.Data());
}

TGo4Slot* TGo4BrowserProxy::BrowserMemorySlot()
{
   return BrowserSlot(fxMemoryPath.Data());
}

Bool_t TGo4BrowserProxy::BrowserItemName(TGo4Slot* itemslot, TString& res)
{
   if (!itemslot || !fxBrowserSlot || !itemslot->IsParent(fxBrowserSlot)) return kFALSE;
   itemslot->ProduceFullName(res, fxBrowserSlot);
   return kTRUE;
}

void TGo4BrowserProxy::UpdateBrowserContent()
{
   fxBrowserSlot->Update(kFALSE);

   TGo4Iter iter(fxOM->GetSlot(fxDataPath), kTRUE);
   while(iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      TGo4ServerProxy* ancont = dynamic_cast<TGo4ServerProxy*>(subslot->GetProxy());
      if (ancont && ancont->IsGo4Analysis()) subslot->Update(kTRUE);
   }
}

Int_t TGo4BrowserProxy::RequestBrowserObject(const char* name, Int_t wait_time)
{
   if (!name || !fxBrowserSlot || !fxOM) return 0;

   Int_t kind = ItemKind(name);
   if (kind==TGo4Access::kndObject) {
      TClass* cl = ItemClass(name);
      if (!cl || !cl->IsLoaded()) return 0;
   }

   TString src, tgt;

   DataSlotName(name, src);
   BrowserSlotName(name, tgt);

   return fxOM->RequestObject(src.Data(), tgt.Data(), wait_time);
}

Int_t TGo4BrowserProxy::RequestBrowserObject(TGo4Slot* slot, Int_t wait_time)
{
   if (!slot || !slot->IsParent(fxBrowserSlot)) return 0;
   TString name;
   slot->ProduceFullName(name, fxBrowserSlot);
   return RequestBrowserObject(name.Data(), wait_time);
}

Bool_t TGo4BrowserProxy::ProduceExplicitCopy(const char* itemname, const char* tgtpath, Bool_t forcerequest)
{
   return ProduceExplicitCopy(BrowserSlot(itemname), tgtpath, forcerequest);
}

Bool_t TGo4BrowserProxy::ProduceExplicitCopy(TGo4Slot* itemslot, const char* tgtpath, Bool_t forcerequest)
{
   TGo4LockGuard lock;

   if (!itemslot) return kFALSE;

   TGo4Slot* memslot = BrowserMemorySlot();
   if (itemslot->IsParent(memslot) && !tgtpath) return kFALSE;
   if (tgtpath) {
       TGo4Slot* tgtslot = BrowserSlot(tgtpath);
       if (!tgtslot) return kFALSE;
       if ((tgtslot == itemslot) || tgtslot->IsParent(itemslot)) return kFALSE;
   }

   if (itemslot->NumChilds() > 0) {
     Bool_t res = kTRUE;
     TObjArray childs;
     for (Int_t n=0;n<itemslot->NumChilds();n++)
       childs.Add(itemslot->GetChild(n));

     TString subpath;
     // create subpath for paste from clipboard folder
     if (tgtpath) {
        TGo4Slot* tgtslot = DataSlot(tgtpath);
        if (!tgtslot) return kFALSE;
        if (!tgtslot->GetSlot(itemslot->GetName(), kTRUE)) return kFALSE;

        // synchronize folder structures in browser branch
        SyncBrowserSlots();

        subpath = tgtpath;
        subpath += "/";
        subpath += itemslot->GetName();

        tgtpath = subpath.Data();
     }

     for (Int_t n=0;n<=childs.GetLast();n++) {
        Bool_t res1 = ProduceExplicitCopy((TGo4Slot*)childs.At(n), tgtpath, forcerequest);
        res = res || res1;
     }

     return res;
   }

   if (ItemKind(itemslot) != TGo4Access::kndObject) return kFALSE;

   TString pathname;
   if (tgtpath) {
      TGo4Slot* tgtslot = ItemSlot(tgtpath);
      if (!tgtslot) return kFALSE;
      if (!tgtslot->IsParent(memslot) && (tgtslot!=memslot)) return kFALSE;
      if (itemslot->GetParent() == tgtslot) return kFALSE;
      if (memslot!=tgtslot)
         tgtslot->ProduceFullName(pathname, memslot);
   } else {
     if (!BrowserItemName(itemslot->GetParent(), pathname)) return kFALSE;
   }

   TObject* obj = itemslot->GetAssignedObject();

   if (!obj || forcerequest) {
      Int_t res = RequestBrowserObject(itemslot);
      if (res == 0) return kFALSE;
      obj = nullptr;
      if ((res == 1) || !forcerequest)
        obj = itemslot->GetAssignedObject();
   }

   if (obj) {
      TString res = SaveToMemory(pathname, obj->Clone(), kTRUE);
      return res.Length() > 0;
   }

   AddWaitingList(itemslot, tgtpath);

   return kTRUE;
}

void TGo4BrowserProxy::ClearClipboard()
{
   if (fxClipboard) {
      fxClipboard->Delete();
      delete fxClipboard;
      fxClipboard = nullptr;
   }
}

void TGo4BrowserProxy::AddToClipboard(const char* itemname)
{
   if (!fxClipboard) fxClipboard = new TObjArray;

   if (fxClipboard->FindObject(itemname)) return;

   TGo4Slot* itemslot = BrowserSlot(itemname);
   if (!itemslot) return;

   // check if parent of that item already in clipboard
   for (Int_t n = 0; n <= fxClipboard->GetLast(); n++) {
      TNamed *nm = (TNamed *)fxClipboard->At(n);
      TGo4Slot *slot = ItemSlot(nm->GetName());
      if (!slot)
         continue;
      if (itemslot->IsParent(slot))
         return;
   }

   fxClipboard->Add(new TNamed(itemname, "clipboard item"));
}

Bool_t TGo4BrowserProxy::IsClipboard()
{
   return fxClipboard != nullptr;
}

void TGo4BrowserProxy::CopyClipboard(const char* tgtpath, Bool_t forcerequest)
{
   if (!fxClipboard) return;

   for (Int_t n = 0; n <= fxClipboard->GetLast(); n++)
      ProduceExplicitCopy(fxClipboard->At(n)->GetName(), tgtpath, forcerequest);
}

void TGo4BrowserProxy::OpenFile(const char* fname)
{
   if (!fname || (*fname == 0)) return;

   fxOM->AddFile(fxDataPath.Data(), fname);

   SyncBrowserSlots();
}

void TGo4BrowserProxy::AddServerProxy(TGo4ServerProxy* serv, const char* slotname, const char* info)
{
   fxOM->AddProxy(fxDataPath.Data(), serv, slotname, info);

   SyncBrowserSlots();
}

Bool_t TGo4BrowserProxy::ConnectDabc(const char* nodename)
{
   if (!nodename || (*nodename == 0)) return kFALSE;

   TGo4DabcProxy* proxy = new TGo4DabcProxy();
   if (!proxy->Connect(nodename)) {
      delete proxy;
      return kFALSE;
   }

   const char* slotname = nodename;
   if (strncmp(slotname,"dabc://",7) == 0) slotname+=7;

   AddServerProxy(proxy, slotname, "Connection to DABC server");

   return kTRUE;
}


Bool_t TGo4BrowserProxy::ConnectHServer(const char* servername,
                                        Int_t portnumber,
                                        const char* basename,
                                        const char* userpass,
                                        const char* filter)
{
   Bool_t res = kFALSE;

   TGo4HServProxy* hserv = new TGo4HServProxy;

   hserv->SetHServConfig(servername,
                         portnumber,
                         basename,
                         userpass,
                         filter);

   if (hserv->RefreshNamesList()) {
      TString capt = "HServ_";
      capt += basename;

      AddServerProxy(hserv, capt.Data(), "Connection to histogram server");
      res = kTRUE;
   } else {
      delete hserv;
   }
   return res;
}

void TGo4BrowserProxy::MakeFilesList(TObjArray* arr)
{
   if (!arr) return;
   arr->Clear();
   TGo4Slot* slot = fxOM->GetSlot(fxDataPath.Data());
   if (!slot) return;

   for(Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TGo4DirProxy* pr = dynamic_cast<TGo4DirProxy*> (subslot->GetProxy());
      if (pr && pr->IsFile())
        arr->Add(pr);
   }
}

void TGo4BrowserProxy::MakeHServerList(TObjArray* arr)
{
   if (!arr) return;
   arr->Clear();
   TGo4Slot* slot = fxOM->GetSlot(fxDataPath.Data());
   if (!slot) return;

   for(Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TGo4HServProxy* pr = dynamic_cast<TGo4HServProxy*> (subslot->GetProxy());
      if (pr) arr->Add(pr);
   }
}

void TGo4BrowserProxy::MakeDabcList(TObjArray* arr)
{
   if (!arr) return;
   arr->Clear();
   TGo4Slot* slot = fxOM->GetSlot(fxDataPath.Data());
   if (!slot) return;

   for(Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TGo4DabcProxy* pr = dynamic_cast<TGo4DabcProxy*> (subslot->GetProxy());
      if (pr) arr->Add(pr);
   }
}

void TGo4BrowserProxy::MakeHttpList(TObjArray* arr)
{
   if (!arr) return;
   arr->Clear();
   TGo4Slot* slot = fxOM->GetSlot(fxDataPath.Data());
   if (!slot) return;

   for(Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TGo4ServerProxy* pr = dynamic_cast<TGo4ServerProxy*> (subslot->GetProxy());
      if (!pr || strcmp(pr->GetContainedClassName(),"TGo4ServerProxy")) continue;

      if ((strncmp(pr->GetServerName(),"http://",7) == 0) ||
          (strncmp(pr->GetServerName(),"https://",8) == 0)) arr->Add(pr);
   }
}

void TGo4BrowserProxy::RequestObjectStatus(const char* name, TGo4Slot* tgtslot)
{
   TString objname;
   TGo4ServerProxy* an = DefineAnalysisObject(name, objname);
   if (an) an->RequestObjectStatus(objname.Data(), tgtslot);
}

void TGo4BrowserProxy::PerformTreeDraw(const char* treename,
                                       const char* Xexp,
                                       const char* Yexp,
                                       const char* Zexp,
                                       const char* cutcond,
                                       const char* hname,
                                       TString& createdhistoname)
{
   if (strlen(Xexp) == 0) return;

   TString varexp(Xexp);

   if(strlen(Yexp) > 0) {
      varexp = TString(Yexp) + TString(":") + varexp;
      if(strlen(Zexp) > 0)
         varexp = TString(Zexp) + TString(":") + varexp;
   }

   int drawdim = varexp.CountChar(':') + 1;
   if (drawdim > 3) drawdim = 3;


   if (IsItemRemote(treename)) {
      TString objname;
      TGo4ServerProxy* an = DefineAnalysisObject(treename, objname);
      if (an) {
         TString analhname(hname);

         Int_t cnt = 0;
         TString anitem = "abcdef";
         if (analhname.IsNull())
           while (anitem.Length() > 0) {
              analhname = "hTreeDraw";
              if (cnt > 0) { analhname+="_"; analhname+=cnt; }
              cnt++;
              anitem = FindItemInAnalysis(analhname.Data());
           }

         an->RemoteTreeDraw(objname.Data(), varexp.Data(), cutcond, analhname.Data());
         an->DelayedRefreshNamesList(3);
      }

      createdhistoname = "";

      return;
   }

   // take tree directly from data browser while
   // tree pointer is not stored in browser slots
   TString treeslotname;
   DataSlotName(treename, treeslotname);
   TTree* SelectedTree = dynamic_cast<TTree*> (fxOM->GetObject(treeslotname.Data()));
   if (!SelectedTree) return;

   TString histoname(hname), hslotname;
   BrowserSlotName(hname, hslotname);
   TGo4Slot* hslot = fxOM->GetSlot(hslotname.Data());

   TH1* histo = nullptr;

   if (hslot && !IsItemRemote(hslot)) {
       histo = dynamic_cast<TH1*> (GetBrowserObject(hname, 1));
       if (histo && (drawdim != histo->GetDimension())) {
          histo = nullptr;
          histoname = "";
       }
   }

   // find non used entry in memory subfolder
   int cnt = 0;
   if (!histo && histoname.IsNull())
     do {
       histoname = fxMemoryPath;
       histoname += "/hTreeDraw_";
       histoname += cnt++;
       BrowserSlotName(histoname, hslotname);
       hslot = fxOM->GetSlot(hslotname.Data());
     } while (hslot);

   TString hfoldername, hobjectname;
   TGo4Slot::ProduceFolderAndName(histoname.Data(), hfoldername, hobjectname);

   if (histo)
     hobjectname  = histo->GetName();

   varexp += ">>+";
   varexp += hobjectname;

   TDirectory* oldhisdir = histo ? histo->GetDirectory() : nullptr;
   TDirectory* savdir = gDirectory;
   gDirectory = nullptr;
   TDirectory dummydir("DummyTreeDraw","Dummy directory to call tree draw");
   dummydir.cd();
   if (histo)
      histo->SetDirectory(&dummydir);

   SelectedTree->Draw(varexp, cutcond, "goff", 10000000, 0);

   if (!histo) { // when new histogram created by Tree::Draw
      histo = dynamic_cast<TH1*> (dummydir.FindObject(hobjectname));
      if(histo) {
         histo->SetDirectory(nullptr);
         createdhistoname = SaveToMemory(0, histo, kTRUE);
         // do sync immediately to be able draw this item in viewpanel
         SyncBrowserSlots();
      }
   } else {
       histo->SetDirectory(oldhisdir);
       TGo4Slot* brslot = BrowserSlot(hname);
       if (brslot) brslot->ForwardEvent(brslot, TGo4Slot::evObjUpdated);
   }
   dummydir.Clear();
   savdir->cd();
}

TGo4Slot* TGo4BrowserProxy::FindServerSlot(Bool_t databranch, Int_t kind)
{
   // search for slot with TGo4ServerProxy
   // kind = 0 - should be TGo4AnalysisProxy (default)
   // kind = 1 - should be TGo4ServerProxy and IsGo4Analysis() == kTRUE
   // kind = 2 - should be TGo4ServerProxy

   TGo4Slot* dataslot = fxOM->GetSlot(fxDataPath.Data());

   TGo4Iter iter(dataslot, kTRUE);
   TGo4Slot* res = nullptr;

   while (iter.next()) {

      TGo4Slot* slot = iter.getslot();
      if (!slot) continue;

      if (kind > 0) {
         TGo4ServerProxy* serv = dynamic_cast<TGo4ServerProxy*> (slot->GetProxy());
         if (!serv || ((kind == 1) && !serv->IsGo4Analysis())) continue;
      } else {
         if (!dynamic_cast<TGo4AnalysisProxy*>(slot->GetProxy())) continue;
      }

      res = slot;
      break;
   }

   if (res && !databranch) {
      TString itemname;
      res->ProduceFullName(itemname, dataslot);
      res = BrowserSlot(itemname.Data());
   }

   return res;
}

TGo4AnalysisProxy* TGo4BrowserProxy::FindAnalysis(const char* itemname)
{
   TString slotname;
   DataSlotName(itemname, slotname);

   TGo4Slot* slot = fxOM->FindSlot(slotname.Data());
   if (!slot || !itemname)
     slot = FindServerSlot(kTRUE);

   return slot ? dynamic_cast<TGo4AnalysisProxy*>(slot->GetProxy()) : nullptr;
}

TGo4ServerProxy* TGo4BrowserProxy::FindServer(const char* itemname, Bool_t asanalysis)
{
   // method should be used when analysis can be used via TGo4ServerProxy interface

   TString slotname;
   DataSlotName(itemname, slotname);

   TGo4Slot* slot = fxOM->FindSlot(slotname.Data());
   if (!slot || !itemname)
     slot = FindServerSlot(kTRUE, asanalysis ? 1 : 2);

   return !slot ? nullptr : dynamic_cast<TGo4ServerProxy*>(slot->GetProxy());
}

TString TGo4BrowserProxy::FindItemInAnalysis(const char* objname)
{
   TGo4Slot *analslot = FindServerSlot(kTRUE, 1); // any kind of go4 analysis is ok
   if (!analslot || !objname) return TString("");

   TGo4Iter iter(analslot);
   while (iter.next()) {
      if (strcmp(iter.getname(), objname) != 0) continue;
      TString res;
      analslot->ProduceFullName(res, DataSlot(""));
      res += "/";
      res += iter.getfullname();
      return res;
   }

   return TString("");
}

TString TGo4BrowserProxy::FindItem(const char* objname)
{
   TGo4Slot* topslot = BrowserTopSlot();
   if (!topslot || !objname) return TString("");

   TGo4Iter iter(topslot);
   while (iter.next())
      if (strcmp(iter.getname(), objname) == 0)
         return iter.getfullname();

   return TString("");
}

TGo4ServerProxy* TGo4BrowserProxy::DefineServerProxy(const char* itemname)
{
   TGo4Slot* slot = DataSlot(itemname);
   if (!slot) return nullptr;

   while (slot) {
      TGo4ServerProxy* pr = dynamic_cast<TGo4ServerProxy*> (slot->GetProxy());
      if (pr) return pr;
      slot = slot->GetParent();
   }

   return nullptr;
}

TGo4ServerProxy* TGo4BrowserProxy::DefineServerObject(const char* itemname, TString* objname, Bool_t onlyanalysis)
{
   TString slotname;
   DataSlotName(itemname, slotname);
   const char* objectname = nullptr;

   TGo4Slot* servslot = fxOM->FindSlot(slotname.Data(), &objectname);

   TGo4ServerProxy* serv = !servslot ? nullptr :
      dynamic_cast<TGo4ServerProxy*>(servslot->GetProxy());
   if (!serv) return nullptr;
   if (onlyanalysis && !serv->IsGo4Analysis()) return nullptr;
   if (objname) *objname = objectname;
   return serv;
}

TGo4ServerProxy* TGo4BrowserProxy::DefineAnalysisObject(const char* itemname, TString& analysisname)
{
   return DefineServerObject(itemname, &analysisname, kTRUE);
}

Bool_t TGo4BrowserProxy::UpdateAnalysisItem(const char* itemname, TObject* obj)
{
   TGo4Slot* slot = BrowserSlot(itemname);
   if (!slot) return kFALSE;

   if (!obj) obj = GetBrowserObject(itemname, 0);
   if (!obj) return kFALSE;

   const char* analysisname = nullptr;
   TGo4Slot* anslot = nullptr;
   TString slotname;

   if (IsItemRemote(itemname)) {
      DataSlotName(itemname, slotname);
      anslot = fxOM->FindSlot(slotname.Data(), &analysisname);
   }

   if (!anslot) {
      analysisname = nullptr;
      anslot = FindServerSlot(kTRUE, 1);
   }

   TGo4ServerProxy* serv = anslot ? dynamic_cast<TGo4ServerProxy*>(anslot->GetProxy()) : 0;
   return serv ? serv->UpdateAnalysisObject(analysisname, obj) : kFALSE;
}

void TGo4BrowserProxy::FetchItem(const char* itemname, Int_t wait_time)
{
   TGo4Slot* itemslot = ItemSlot(itemname);
   if (!itemslot) return;

   if (ItemKind(itemslot) == TGo4Access::kndObject)
      RequestBrowserObject(itemslot, wait_time);

   TGo4Iter iter(itemslot, kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      if (ItemKind(subslot)==TGo4Access::kndObject)
         RequestBrowserObject(subslot, wait_time);
   }

   if (wait_time == 0) return;
}

void TGo4BrowserProxy::RedrawItem(const char* itemname)
{
   TGo4Slot* slot = BrowserSlot(itemname);
   if (slot) {
      slot->ForwardEvent(slot, TGo4Slot::evObjUpdated);
      SetItemTimeDate(slot);
   }
}

TObject* TGo4BrowserProxy::GetBrowserObject(const char* name, int update)
// update=0 - without update,
//        1 - request only if obj == nullptr,
//        2 - update of object in any case
//    >=100 - update object in any case and wait for specified time in millisec
{
   if (!name || !fxBrowserSlot || !fxOM) return nullptr;

   TString src, tgt;
   BrowserSlotName(name, tgt);
   TGo4Slot* guislot = fxOM->GetSlot(tgt.Data());

   if (!guislot) return nullptr;

   TObject* obj = guislot->GetAssignedObject();

   if ((update == 0) || ((update == 1) && obj)) return obj;

   if (guislot->IsParent(BrowserMemorySlot())) return obj;

   DataSlotName(name, src);

   fxOM->RequestObject(src.Data(), tgt.Data(), (update < 10) ? 0 : update);

   return guislot->GetAssignedObject();
}

void TGo4BrowserProxy::SetItemsFilter(Int_t filter)
{
   fiFilterIndex = filter;
   InformBrowserUpdate();
}

void TGo4BrowserProxy::InformBrowserUpdate()
{
   if (fxBrowserSlot)
     fxBrowserSlot->ForwardEvent(fxBrowserSlot, TGo4Slot::evObjUpdated);
}

Bool_t TGo4BrowserProxy::DeleteDataSource(TGo4Slot* itemslot)
{
   if (!itemslot || !fxBrowserSlot || !fxOM) return kFALSE;

   Int_t cando = ItemCanDo(itemslot);

   Bool_t ismemoryitem = itemslot->IsParent(BrowserMemorySlot());

   if (!ismemoryitem && !CanCloseItem(cando) && !IsCanDelete(itemslot)) return kFALSE;

   if (itemslot->GetPar("::CopyObject") != nullptr) {
      delete itemslot;
      InformBrowserUpdate();
   } else {
      TString name = itemslot->GetFullName(fxBrowserSlot);
      TString src;
      DataSlotName(name.Data(), src);
      fxOM->DeleteSlot(src.Data());
   }
   return kTRUE;
}

void TGo4BrowserProxy::DoItemMonitor(TGo4Slot* slot)
{
   if (!slot || !slot->IsParent(fxBrowserSlot)) return;

   slot->Update(kFALSE);
}

TGo4Slot* TGo4BrowserProxy::BrowserTopSlot()
{
   return fxBrowserSlot;
}

TGo4Slot* TGo4BrowserProxy::ItemSlot(const char* itemname)
{
   TString slotname;
   BrowserSlotName(itemname, slotname);
   return fxOM->GetSlot(slotname.Data());
}

Bool_t TGo4BrowserProxy::DefineTreeName(const char* itemname, TString& treename)
{
   TString slotname;
   BrowserSlotName(itemname, slotname);
   TGo4Slot* slot = fxOM->GetSlot(slotname.Data());
   if (!slot) return kFALSE;

   TGo4Slot* treeslot = slot;
   while (treeslot) {
      TClass* cl = ItemClass(treeslot);
      if (cl && cl->InheritsFrom(TTree::Class())) break;
      treeslot = treeslot->GetParent();
   }
   if (!treeslot) return kFALSE;

   treeslot->ProduceFullName(treename, fxBrowserSlot);

   return kTRUE;
}

Bool_t TGo4BrowserProxy::DefineLeafName(const char* itemname, const char* treename, TString& leafname)
{
   if (!itemname || !treename) return kFALSE;

   TString slotname;
   BrowserSlotName(itemname, slotname);
   TGo4Slot* slot = fxOM->GetSlot(slotname.Data());
   if (!slot) return kFALSE;

   if (ItemKind(slot)!=TGo4Access::kndTreeLeaf) return kFALSE;

   int ilen = strlen(itemname);
   int tlen = strlen(treename);
   if ((ilen < tlen) || (strncmp(itemname, treename, tlen) != 0))
      return kFALSE;

   leafname = slot->GetName();
   return kTRUE;
}

Bool_t TGo4BrowserProxy::DefineRelatedObject(const char* itemname, const char* objname, TString& objectitem, Int_t mask)
{
   if (!objname || (*objname == 0)) return kFALSE;

   if (BrowserSlot(objname)) {
      objectitem = objname;
      return kTRUE;
   }

   TGo4Slot* picslot = BrowserSlot(itemname);

   if (picslot) {
      TGo4Slot* searchslot = picslot->GetParent();

      while (searchslot && (searchslot != fxBrowserSlot)) {
         TString searchname;
         searchslot->ProduceFullName(searchname, fxBrowserSlot);
         if (*objname!='/') searchname+="/";
         searchname+=objname;

         TString fullname;
         BrowserSlotName(searchname.Data(), fullname);

         TGo4Slot* slot = fxOM->GetSlot(searchname.Data());
         if (!slot) {
            searchname.Append(";1");
            slot = fxOM->GetSlot(searchname.Data());
         }

         if (slot) {
            objectitem = searchname;
            return kTRUE;
         }

         searchslot = searchslot->GetParent();
      }
   }

   if (strchr(objname,'/')) {
      // to enable explore missing subfolders, mask should be 2
      if ((mask & 2) == 0) return kFALSE;

      // it could happen that folder in the file or remote server is not yet read
      // we could try to exploit it

      // first slash should be always there - any source should provide first level by default
      const char *slash = strchr(objname, '/');

      while (slash) {
         slash = strchr(slash+1, '/');
         if (!slash) break;

         TString diritem;
         if (!DefineRelatedObject(itemname, TString(objname, slash-objname).Data(), diritem, 0)) break;

         int kind = ItemKind(diritem.Data());

         if ((kind==TGo4Access::kndFolder) || (kind==TGo4Access::kndMoreFolder)) {
            GetBrowserObject(diritem.Data(), 1000);
            SyncBrowserSlots();
         }
      }

      return DefineRelatedObject(itemname, objname, objectitem, 0);
   }

   // one should specify mask==1 to enable recursive search
   if ((mask & 1) == 0) return kFALSE;

   TGo4Slot* searchslot = picslot ? picslot->GetParent() : fxBrowserSlot;

   do {
      TGo4Iter iter(searchslot, kTRUE);
      size_t len = strlen(objname);
      while (iter.next()) {
         const char* name = iter.getname();

         if (strncmp(objname, name, len) != 0) continue;

         if ((strlen(name) == len) ||
             ((strlen(name)==(len+2)) && (strcmp(name+len,";1") == 0))) {
            TGo4Slot* subslot = iter.getslot();
            subslot->ProduceFullName(objectitem, fxBrowserSlot);
            return kTRUE;
         }
      }
      if (searchslot == fxBrowserSlot) break;
      searchslot = searchslot->GetParent();
   } while (searchslot);

   return kFALSE;
}

Bool_t TGo4BrowserProxy::DefineFileObject(const char* itemname, TString& fitemname, const char** filepath)
{
   TGo4Slot* slot = BrowserSlot(itemname);
   if (!slot) return kFALSE;

   if (!slot->GetAssignedObject()) return kFALSE;

   while (slot!=fxBrowserSlot) {
      TClass* cl = ItemClass(slot);
      if (cl && cl->InheritsFrom(TFile::Class())) break;
      slot = slot->GetParent();
   }
   if (slot==fxBrowserSlot) return kFALSE;

   fitemname = "";
   slot->ProduceFullName(fitemname, fxBrowserSlot);

   *filepath = itemname + fitemname.Length();
   if (**filepath=='/') (*filepath)++;

   //filedataslot = fitemname;
   //DataSlotName(fitemname, filedataslot);

   return kTRUE;
}

Bool_t TGo4BrowserProxy::UpdateObjectInFile(const char* itemname, const char* fileitemname, const char* filepath)
{
   TGo4Slot* slot = BrowserSlot(itemname);
   if (!slot) return kFALSE;

   TObject* obj = slot->GetAssignedObject();
   if (!obj) return kFALSE;

   TGo4Slot* fileslot = DataSlot(fileitemname);
   if (!fileslot) return kFALSE;

   TGo4DirProxy* dircont = dynamic_cast<TGo4DirProxy*> (fileslot->GetProxy());
   if (!dircont) return kFALSE;

   Bool_t res = dircont->UpdateObjectInFile(filepath, obj);

   if(res)
     fileslot->ForwardEvent(fileslot, TGo4Slot::evObjUpdated);

   return res;
}

Bool_t TGo4BrowserProxy::SaveItemToFile(const char* itemname, const char* filename, const char* subfolder)
{
   TGo4Slot* slot = BrowserSlot(itemname);
   if (!slot) return kFALSE;

   TObject* obj = slot->GetAssignedObject();
   if (!obj) return kFALSE;

   TFile* f = TFile::Open(filename,"UPDATE");
   if (!f) return kFALSE;

   TDirectory* dir = f;

   if (subfolder && *subfolder) {
      dir = dynamic_cast<TDirectory*> (f->Get(subfolder));
      if (!dir) dir = f;
   }

   dir->cd();
   bool res = dir->WriteTObject(obj, obj->GetName(), "Overwrite") > 0;

   if (obj->InheritsFrom(TH1::Class()))
     ((TH1*) obj)->SetDirectory(nullptr);

   delete f;

   return res;
}

void TGo4BrowserProxy::ExportItemsTo(TObjArray* items,  // array of TObjString
                                     Bool_t fetchitems,
                                     const char* filename,
                                     const char* filedir,
                                     const char* format,
                                     const char* description)
{
   if (!items || (items->GetSize() == 0)) return;

   TString convert;
   if(!format)
      convert="ROOT";
   else
      convert=format;

   Go4Export_t filter;
   if(convert.Contains("ASCII")) {
      TGo4Log::Message(0,"Export filter is ASCII");
      filter=GO4EX_ASCII;
   } else
   if (convert.Contains("Radware")) {
      TGo4Log::Message(0,"Export filter is Radware");
      filter=GO4EX_RADWARE;
   } else
   if (convert.Contains("ROOT XML")) {
      TGo4Log::Message(0,"Export filter is ROOT XML");
      filter=GO4EX_XML;
   } else
   if (convert.Contains("ROOT")) {
      TGo4Log::Message(0,"Export filter is ROOT");
      filter=GO4EX_ROOT;
   } else {
      TGo4Log::Message(0,"Export filter is unknown, using ROOT");
      filter=GO4EX_ROOT;
   }

   // if root export filter and 1 subfolder, export with complete file structures
   if (((filter==GO4EX_ROOT) || (filter==GO4EX_XML)) && (items->GetLast() == 0)) {
      TObjString* str = dynamic_cast<TObjString*> (items->At(0));

      TGo4Slot* itemslot = !str ? nullptr : ItemSlot(str->GetName());

      TString fname = filename;

      if (filter==GO4EX_XML) {
         if(!fname.Contains(".xml")) fname.Append(".xml");
      } else {
         if(!fname.Contains(".root")) fname.Append(".root");
      }

      if (itemslot && (itemslot->NumChilds() > 0)) {
         SaveBrowserToFile(fname.Data(), fetchitems, str->GetName(), description);
         return;
      }
   }

   TGo4ExportManager exman("BrowserExport");
   exman.SetOutFile(filename);
   exman.SetOutFileComment(description);
   exman.SetStartDir();
   exman.SetCurrentDir(filedir);

   TGo4Log::Message(1,"Exporting selected browser objects to %s",convert.Data());

   TObjArray objs;
   for (int n = 0; n <= items->GetLast(); n++) {
      TObjString *str = dynamic_cast<TObjString *>(items->At(n));
      if (!str)
         continue;
      TObject *obj = GetBrowserObject(str->GetName(), 0);
      if (!obj)
         obj = GetBrowserObject(str->GetName(), 2000);

      if (obj)
         objs.Add(obj);
   }

   exman.Export(&objs, filter);
}

Bool_t TGo4BrowserProxy::SaveBrowserToFile(const char* filename,
                                           Bool_t prefetch,
                                           const char* selectedpath,
                                           const char* description)
{
   TGo4Slot* toppath = BrowserSlot(selectedpath);

   if (!toppath) return kFALSE;

   if (prefetch)
      FetchItem(selectedpath, 2000);

   TFile* f = TFile::Open(filename, "recreate", description);
   if (!f) return kFALSE;

   fxOM->SaveDataToFile(f, kTRUE, toppath);

   delete f;

   return kTRUE;
}

Bool_t TGo4BrowserProxy::IsItemRemote(const char* name)
{
   return IsItemRemote(ItemSlot(name));
}

Bool_t TGo4BrowserProxy::IsItemRemote(TGo4Slot* slot)
{
   if (!slot) return kFALSE;
   Int_t remote;
   if (!slot->GetIntPar("GUI::Remote", remote)) return kFALSE;
   return remote != 0;
}

Bool_t TGo4BrowserProxy::IsAnalysisItem(const char* name)
{
   TString analysisname;
   return DefineAnalysisObject(name, analysisname) != nullptr;
}

void TGo4BrowserProxy::SetItemTimeDate(TGo4Slot* slot, const char* stime, const char* sdate)
{
   if (stime && sdate) {
     slot->SetPar("GUI::Time", stime);
     slot->SetPar("GUI::Date", sdate);
     return;
   }

   TDatime t;
   const char* datetime = t.AsSQLString();
   char* time = (char*) strchr(datetime, ' ');
   if (!time) {
     slot->SetPar("GUI::Time",datetime);
     slot->SetPar("GUI::Date",datetime);
   } else {
     slot->SetPar("GUI::Time", time+1);
     *time = 0;
     slot->SetPar("GUI::Date", datetime);
   }
}

const char* TGo4BrowserProxy::ItemTime(TGo4Slot* slot)
{
   return !slot ? nullptr : slot->GetPar("GUI::Time");
}

const char* TGo4BrowserProxy::ItemDate(TGo4Slot* slot)
{
   return !slot ? nullptr : slot->GetPar("GUI::Date");
}

void TGo4BrowserProxy::SetLinkedName(TGo4Slot* slot, const char* itemname)
{
   if (slot)
      slot->SetPar("::LinkedItem", itemname);
}

const char* TGo4BrowserProxy::GetLinkedName(TGo4Slot* slot)
{
   return !slot ? nullptr : slot->GetPar("::LinkedItem");
}

Int_t TGo4BrowserProxy::ItemKind(const char* name)
{
   return ItemKind(ItemSlot(name));
}

Int_t TGo4BrowserProxy::ItemKind(TGo4Slot* slot)
{
   if (!slot) return TGo4Access::kndNone;
   Int_t kind;
   if (!slot->GetIntPar("GUI::Kind", kind)) return TGo4Access::kndNone;
   return kind;
}

void TGo4BrowserProxy::SetItemKind(TGo4Slot* slot, Int_t kind, const char* classname, const char* info, Int_t sizeinfo)
{
   if (!slot) return;
   if (kind>=0) slot->SetIntPar("GUI::Kind", kind);
           else slot->RemovePar("GUI::Kind");
   slot->SetPar("GUI::Class", classname);
   slot->SetPar("GUI::Info", info);
   slot->SetIntPar("GUI::SizeInfo", sizeinfo);
}

const char* TGo4BrowserProxy::ItemInfo(TGo4Slot* slot)
{
   return !slot ? nullptr : slot->GetPar("GUI::Info");
}

Int_t TGo4BrowserProxy::ItemSizeInfo(TGo4Slot* slot)
{
   if (!slot) return -1;
   Int_t sizeinfo;
   if (!slot->GetIntPar("GUI::SizeInfo", sizeinfo)) return -1;
   return sizeinfo;
}

void TGo4BrowserProxy::SetCalcSize(TGo4Slot* slot, Int_t size)
{
   if (slot)
     slot->SetIntPar("GUI::CalcSize", size);
}

Int_t TGo4BrowserProxy::GetCalcSize(TGo4Slot* slot)
{
   if (!slot) return 0;
   Int_t size;
   if (!slot->GetIntPar("GUI::CalcSize", size)) return 0;
   return size;
}

TClass* TGo4BrowserProxy::ItemClass(TGo4Slot* slot)
{
   return TGo4Proxy::GetClass(ItemClassName(slot));
}

TClass* TGo4BrowserProxy::ItemClass(const char* name)
{
   return ItemClass(ItemSlot(name));
}

const char* TGo4BrowserProxy::ItemClassName(TGo4Slot* slot)
{
   return !slot ? nullptr : slot->GetPar("GUI::Class");
}

const char* TGo4BrowserProxy::ItemClassName(const char* name)
{
   return ItemClassName(ItemSlot(name));
}

Int_t TGo4BrowserProxy::ItemCanDo(const char* name)
{
   return ItemCanDo(ItemSlot(name));
}

Int_t TGo4BrowserProxy::ItemCanDo(TGo4Slot* slot)
{
   if (!slot) return 0;

   Int_t cando;
   if (!slot->GetIntPar("GUI::CanDo", cando)) return 0;
   return cando;
}

void TGo4BrowserProxy::SetItemCanDo(TGo4Slot* slot, Int_t cando)
{
   if (slot) slot->SetIntPar("GUI::CanDo", cando);
}

Bool_t TGo4BrowserProxy::IsItemMonitored(TGo4Slot* slot)
{
   return !slot ? kTRUE : !slot->GetPar("GUI::NotMonitored");
}

void TGo4BrowserProxy::SetItemMonitored(TGo4Slot* slot, Bool_t on)
{
   if (!slot) return;

   if (ItemKind(slot)==TGo4Access::kndFolder) {
      TGo4Iter iter(slot, kTRUE);
      while (iter.next()) {
         TGo4Slot* subslot = iter.getslot();
         if (ItemKind(subslot)==TGo4Access::kndObject)
             SetItemMonitored(subslot, on);
      }
   } else {
      if (on) {
         slot->RemovePar("GUI::NotMonitored");
         CheckPictureMonitor(slot);
      } else {
         slot->SetPar("GUI::NotMonitored","1");
      }
   }
}

bool TGo4BrowserProxy::CanExecuteItem(int cando)
{
   return (cando % 1000000000) / 100000000 > 0;
}

bool TGo4BrowserProxy::CanExpandItem(int cando)
{
   return (cando % 100000000) / 10000000 > 0;
}

bool TGo4BrowserProxy::CanExportItem(int cando)
{
   return (cando % 10000000) / 1000000 > 0;
}

bool TGo4BrowserProxy::CanInfoItem(int cando)
{
   return (cando % 1000000) / 100000 > 0;
}

bool TGo4BrowserProxy::CanCloseItem(int cando)
{
   return (cando % 100000) / 10000 > 0;
}

bool TGo4BrowserProxy::CanClearItem(int cando)
{
   return (cando % 10000) / 1000 > 0;
}

bool TGo4BrowserProxy::CanDrawItem(int cando)
{
   return (cando % 1000) / 100 > 0;
}

bool TGo4BrowserProxy::CanDragItem(int cando)
{
   return (cando % 100) / 10 > 0;
}

bool TGo4BrowserProxy::CanEditItem(int cando)
{
   return (cando % 10 > 0);
}

void TGo4BrowserProxy::CreateMemoryFolder(const char* foldername)
{
   if (foldername && *foldername)
     fxMemoryPath = foldername;

   TString fullpathname;
   DataSlotName(fxMemoryPath, fullpathname);

   fxOM->MakeFolder(fullpathname);
}

void TGo4BrowserProxy::CreateMemorySubfolder(const char* itemname, const char* newfoldername)
{
    TGo4Slot* itemslot = BrowserSlot(itemname);

    if (!itemslot || !newfoldername || (strlen(newfoldername) == 0)) return;

    TGo4Slot* memslot = BrowserMemorySlot();
    if (!itemslot->IsParent(memslot) && (memslot!=itemslot)) return;

    TGo4Slot* slot = DataSlot(itemname);
    if (!slot) return;

    TGo4Slot* newslot = slot->GetSlot(newfoldername, kTRUE);

    if (newslot)
       InformBrowserUpdate();
}

void TGo4BrowserProxy::RenameMemoryItem(const char* itemname, const char* newname)
{
   TGo4Slot* itemslot = BrowserSlot(itemname);
   if (!itemslot || !newname || (strlen(newname) == 0)) return;

   if (strchr(newname,'/') || strchr(newname,'\\')) return;

   TGo4Slot* memslot = BrowserMemorySlot();
   if (!itemslot->IsParent(memslot)) return;

   TGo4Slot* slot = DataSlot(itemname);
   if (!slot) return;

   // check if item of that name is already existing
   if (slot->GetParent()->FindChild(newname)) return;

   slot->SetName(newname);
   TNamed* n = dynamic_cast<TNamed*> (slot->GetAssignedObject());
   if (n) n->SetName(newname);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);
}



void TGo4BrowserProxy::ClearMemoryItem(const char* itemname)
{
   TGo4Slot* itemslot = BrowserSlot(itemname);
   if (!itemslot) return;

   TGo4Slot* memslot = BrowserMemorySlot();
   if (!itemslot->IsParent(memslot)) return;

   TGo4Slot* slot = DataSlot(itemname);
   if (!slot) return;

   TObject* ob = slot->GetAssignedObject();
   if (!ob) return;

   if(ob->InheritsFrom(TH1::Class())) {
      TH1* his = dynamic_cast<TH1*>(ob);
      if (his) his->Reset(); // histogram has no Clear implementation!
   } else
   if(ob->InheritsFrom(TGo4DynamicEntry::Class())) {
      TGo4DynamicEntry* entry = dynamic_cast<TGo4DynamicEntry*>(ob);
      if (entry) entry->Reset(); // dynamic entry has no Clear implementation!
   } else
   if(ob->InheritsFrom(TGo4Picture::Class())) {
      TGo4Picture* pic = dynamic_cast<TGo4Picture*>(ob);
      if (pic) pic->Reset(); // picture has no Clear implementation!
   } else
   if(ob->InheritsFrom(TGraph::Class())) {
      TGraph* gr = dynamic_cast<TGraph*>(ob);
      if (gr) {
         Int_t pn = gr->GetN();
         gr->Set(0); // clear array of points
         gr->Set(pn); // this should set all to 0
      }
   } else
   if(ob->InheritsFrom(TMultiGraph::Class())) {
     TMultiGraph* mg = dynamic_cast<TMultiGraph*>(ob);
     if (mg) {
        TIter liter(mg->GetListOfGraphs());
        while(auto gr = (TGraph*) liter()) {
           Int_t pn = gr->GetN();
           gr->Set(0); // clear array of points
           gr->Set(pn); // this should set all to 0
        }
     }
   } else {
      // use virtual Clear of all objects
      // make sure that TNamed::Clear is overwritten in subclasses
      ob->Clear();
   }

   itemslot->ForwardEvent(itemslot, TGo4Slot::evObjAssigned);
}

TString TGo4BrowserProxy::SaveToMemory(const char* pathname, TObject* obj, Bool_t ownership, Bool_t overwrite)
{
   if (!obj) return TString("");

   TString path = fxMemoryPath;
   if (pathname && (*pathname != 0)) {
      path += "/";
      path += pathname;
   }

   // if overwrite, check if slot of that name exists
   if (overwrite) {
       TString itemname = path;
       itemname+= "/";
       itemname+=obj->GetName();
       TGo4Slot* slot = DataSlot(itemname);
       if (slot) {
          slot->AssignObject(obj, ownership);
          return itemname;
       }
   }

   TString fullpathname;
   DataSlotName(path.Data(), fullpathname);

   TGo4Slot* slot = fxOM->Add(fullpathname.Data(), obj, ownership, kTRUE);

   return !slot ? TString("") : slot->GetFullName(fxOM->GetSlot(fxDataPath));
}

void TGo4BrowserProxy::CheckPictureMonitor(TGo4Slot* slot)
{
   if (!slot) return;

   TObject* obj = slot->GetAssignedObject();
   if (!obj) return;

   TGo4Picture* pic = dynamic_cast<TGo4Picture*> (obj);
   if (!pic) return;

   if (!IsItemMonitored(slot)) return;

   TString picitemname;
   if (!BrowserItemName(slot, picitemname)) return;

   CheckPictureMonitor(pic, picitemname.Data());
}

void TGo4BrowserProxy::CheckPictureMonitor(TGo4Picture* pic, const char* picitemname)
{
   if (!pic) return;

   if (pic->IsDivided())
      for(Int_t posy=0; posy<pic->GetDivY(); posy++)
         for(Int_t posx=0; posx<pic->GetDivX(); posx++)
            CheckPictureMonitor(pic->FindPic(posy,posx), picitemname);

   for (Int_t n=0; n<pic->GetNumObjNames(); n++) {

      TString drawname;

      if (DefineRelatedObject(picitemname, pic->GetObjName(n), drawname))
         SetItemMonitored(BrowserSlot(drawname), kTRUE);
   }
}

void TGo4BrowserProxy::Scan_gROOT()
{
   TIter iter(gROOT->GetList());
   while (auto obj = iter()) {
      SaveToMemory("gROOT", obj, kTRUE);
   }

   gROOT->GetList()->Clear();
}

void TGo4BrowserProxy::SetCanDelete(TGo4Slot* slot, Bool_t on)
{
   if (slot) {
     if(on) slot->SetPar("GUI::CanDelete","1");
       else slot->RemovePar("GUI::CanDelete");
   }
}

Bool_t TGo4BrowserProxy::IsCanDelete(TGo4Slot* slot)
{
   return !slot ? kFALSE : slot->GetPar("GUI::CanDelete") != nullptr;
}

void TGo4BrowserProxy::ToggleMonitoring(Int_t rate)
{
   fiMonitoringPeriod = rate;

   fbBlockMonitoring = kFALSE;

   if (fiMonitoringPeriod > 0) {
      if (!fxMonitorTimer)
         fxMonitorTimer = new TTimer(this, 10, kTRUE);
      fxMonitorTimer->Start(10, kTRUE);
   } else {
      if (fxMonitorTimer) fxMonitorTimer->Reset();
      delete fxMonitorTimer;
      fxMonitorTimer = nullptr;
   }

   InformBrowserUpdate();
}

Int_t TGo4BrowserProxy::UpdateVisibleAnalysisObjects(bool checkmonitor)
{
   TGo4Slot* viewslot = fxOM->GetSlot(fxViewPath.Data());
   if (!viewslot) return 0;

   TGo4Slot* brslot = BrowserSlot();

   TObjArray UniqueItems;

   // first produce list of slots without duplication
   TGo4Iter iter(viewslot, kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();

      TGo4LinkProxy* link = dynamic_cast<TGo4LinkProxy*> (subslot->GetProxy());
      if (link) {
         TGo4Slot* srcslot = link->GetLink();
         if (srcslot && srcslot->IsParent(brslot) && !UniqueItems.FindObject(srcslot))
            UniqueItems.Add(srcslot);
      }
   }

   Int_t nrequests = 0;
   for (int n=0;n<=UniqueItems.GetLast();n++) {
      TGo4Slot* srcslot = (TGo4Slot*)UniqueItems.At(n);
      if (!checkmonitor || IsItemMonitored(srcslot)) {
         nrequests++;
         srcslot->Update(kFALSE);
      }
   }
   return nrequests;
}

Int_t TGo4BrowserProxy::UpdateAllMonitoredObjects()
{
   Int_t nrequests = 0;

   TGo4Iter iter(BrowserSlot(),kTRUE);

   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      if (IsItemMonitored(subslot)) {
         nrequests++;
         subslot->Update(kTRUE);
      }
   }

   return nrequests;
}

Bool_t TGo4BrowserProxy::HandleTimer(TTimer* timer)
{
   if (timer==fxSyncTimer) {
      SyncBrowserSlots();
      return kTRUE;
   } else if (timer == fxMonitorTimer) {

      if (fiMonitoringPeriod <= 0) return kTRUE;

      TGo4ServerProxy* an = FindServer();

      Bool_t anready = kTRUE;
      if (an) anready = an->IsConnected() && (an->NumberOfWaitingProxyes() < 3);

      // request new objects if total number of proxies is not too big
      if (anready && !fbBlockMonitoring) {
         if (fbWithRootBrowser) {
            UpdateAllMonitoredObjects();
            UpdateAllCanvases();
         } else {
            UpdateVisibleAnalysisObjects(true);
         }
      }

      Int_t period = fiMonitoringPeriod;

      // if monitoring is blocked (no request for objects update are called), check again quickly
      if (fbBlockMonitoring) period = 1000;

      fxMonitorTimer->Start(period, kTRUE);

      return kTRUE;
   }

   return kFALSE;
}

void TGo4BrowserProxy::SetProtectionBits(TGo4Slot* slot, Int_t delprot, Int_t clearprot)
{
   if (!slot) return;
   if (delprot >= 0)
      slot->SetIntPar("GUI::DeleteProtect", delprot);
   else
      slot->RemovePar("GUI::DeleteProtect");
   if (clearprot >= 0)
      slot->SetIntPar("GUI::ResetProtect", clearprot);
   else
      slot->RemovePar("GUI::ResetProtect");
}

void TGo4BrowserProxy::GetProtectionBits(TGo4Slot* slot, Int_t& delprot, Int_t& clearprot)
{
   delprot = -1; clearprot = -1;
   if (!slot) return;
   if (!slot->GetIntPar("GUI::DeleteProtect", delprot)) delprot = -1;
   if (!slot->GetIntPar("GUI::ResetProtect", clearprot)) clearprot = -1;
}

void TGo4BrowserProxy::SyncBrowserSlots()
{
   TGo4LockGuard lock;

   TGo4Slot* targetslot = BrowserTopSlot();

   TGo4Slot* source = fxOM->GetSlot(fxDataPath.Data());
   if (!source) return;

   TGo4Slot* curfold = targetslot;
   if (!curfold) return;

   TGo4Slot* curslot = curfold->GetChild(0);

   TGo4Iter iter(source);

   const char* EventsFolder = "EventObjects/Events/";

   while (curfold) {

      // shift to next item in iterator
      Bool_t res = iter.next();

      // go to top folders and remove rest items
      Int_t levelchange = iter.levelchange();

      while (levelchange++ < 0) {
          while (curslot) {
             TGo4Slot* next = curslot->GetNext();
             delete curslot;
             curslot = next;
          }

          curslot = curfold->GetNext();
          curfold = curfold->GetParent();
          if (!curfold) break;
      }

      if (!res) break;


      TGo4Slot* find = curslot;
      while (find && (strcmp(iter.getname(), find->GetName()) != 0))
        find = find->GetNext();

      if (!find)
         find = new TGo4Slot(curfold, iter.getname(), iter.getinfo());

      if (find!=curslot) {
         curfold->ShiftSlotBefore(find, curslot);
         curslot = find;
      }

      Int_t kind = iter.getkindofitem();
      const char* classname = iter.getclassname();
      const char* fullname = iter.getfullname();
      const char* ppp = strstr(fullname, EventsFolder);
      if (ppp) {
          ppp += strlen(EventsFolder);
          if ((strlen(ppp) != 0) && !strchr(ppp,'/')) {
             kind = TGo4Access::kndEventElement;
             classname = nullptr;
          }
      }

      SetItemKind(curslot, kind, classname, iter.getinfo(), iter.getsizeinfo());
      curslot->SetIntPar("GUI::Remote", iter.getflag("IsRemote")==1);
      SetProtectionBits(curslot, iter.getflag("IsDeleteProtect"), iter.getflag("IsResetProtect"));

      if (iter.getslot())
         SetCanDelete(curslot, IsCanDelete(iter.getslot()));

      TObject* assobj = nullptr;
      if ((kind==TGo4Access::kndObject) && iter.getslot()) {
         TObject* obj = iter.getslot()->GetAssignedObject();
         if (obj && !curslot->GetAssignedObject())
           assobj = obj;
      }

      if (!curslot->GetProxy())
         curslot->SetProxy(new TGo4BrowserObjProxy(curslot, assobj, kFALSE));
      else if (assobj)
         curslot->AssignObject(assobj, kFALSE);

      if (iter.isfolder()) {
         curfold = curslot;
         curslot = curfold->GetChild(0);
      } else {
         curslot->DeleteChilds();
         curslot = curslot->GetNext();
      }
   }

   if (curslot && curslot->IsParent(targetslot))
      while (curslot) {
         TGo4Slot* next = curslot->GetNext();
         delete curslot;
         curslot = next;
      }

    CalculateFolderSizes(targetslot);

    // inform other world that content was updated
    targetslot->ForwardEvent(targetslot, TGo4Slot::evObjUpdated);

    if (fxSyncTimer) fxSyncTimer->Reset();

    fbBlockSync = kFALSE;
}

Int_t TGo4BrowserProxy::CalculateFolderSizes(TGo4Slot* topslot)
{
   if (!topslot) return 0;
   Int_t sizeinfo = ItemSizeInfo(topslot);
   bool verytop = fxBrowserSlot==topslot;
   if (!verytop && ItemKind(topslot)!=TGo4Access::kndFolder) return sizeinfo;

   Int_t sum = 0;
   for (Int_t n=0;n<topslot->NumChilds();n++)
       sum += CalculateFolderSizes(topslot->GetChild(n));
   if (!verytop) SetCalcSize(topslot, sum);

   return sum;
}

Int_t TGo4BrowserProxy::DefineItemProperties(Int_t kind, TClass* cl, TString& pixmap)
{
   // 100000000 - execute,
   //  10000000 - expand,
   //   1000000 - export,
   //    100000 - info,
   //     10000 - close,
   //      1000 - clear,
   //       100 - draw,
   //        10 - drag,
   //         1 - edit

   Int_t cando = 0;

   // TClass * cl = TGo4Proxy::GetClass(clname);

   if (kind==TGo4Access::kndObject) {
      if (cl) {
        if (cl->InheritsFrom(TProfile::Class())) { cando = 1110; pixmap = "profile_t.png"; } else
        if (cl->InheritsFrom(TProfile2D::Class())) { cando = 1110; pixmap = "col.png"; } else
        if (cl->InheritsFrom(TGo4Picture::Class())) { cando = 110; pixmap = "picture.png"; } else
        if (cl->InheritsFrom(TGo4Parameter::Class())) { cando = 1011; pixmap = "parameter.png"; } else
        if (cl->InheritsFrom(TMultiGraph::Class())) { cando = 1110; pixmap = "superimpose.png"; } else
        if (cl->InheritsFrom(THStack::Class())) { cando = 1110; pixmap = "superimpose.png"; } else
        if (cl->InheritsFrom(TGraph::Class())) { cando = 1001110; pixmap = "tgraph.png"; } else
        if (cl->InheritsFrom(TGo4Fitter::Class())) { cando = 11; pixmap = "fitter.png"; } else
        if (cl->InheritsFrom(TCanvas::Class())) { cando = 110; pixmap = "canvas.png"; } else
        if (cl->InheritsFrom(TFile::Class())) { pixmap = "rootdb_t.png"; } else
        if (cl->InheritsFrom(TTree::Class())) { pixmap = "tree_t.png"; } else
        if (cl->InheritsFrom(TFolder::Class())) { pixmap = "folder_t.png"; } else
        if (cl->InheritsFrom(TDirectory::Class())) { pixmap = "folder_t.png"; } else
        if (cl->InheritsFrom(TH3::Class())) { cando = 1101110; pixmap = "h3_t.png"; } else
        if (cl->InheritsFrom(TH2::Class())) { cando = 1101110; pixmap = "h2_t.png"; } else
        if (cl->InheritsFrom(TH1::Class())) { cando = 1101110; pixmap = "h1_t.png"; } else
        if (cl->InheritsFrom(TGo4WinCond::Class())) { cando = 101011; pixmap = "windcond.png"; } else
        if (cl->InheritsFrom(TGo4PolyCond::Class())) { cando = 101011; pixmap = "polycond.png"; } else
        if (cl->InheritsFrom(TGo4ShapedCond::Class())) { cando = 101011; pixmap = "polycond.png"; } else
        if (cl->InheritsFrom(TGo4ListCond::Class())) { cando = 101011; pixmap = "windcond.png"; } else
        if (cl->InheritsFrom(TGo4CondArray::Class())) { cando = 101011; pixmap = "windcondarray.png"; } else
        if (cl->InheritsFrom(TGo4TreeHistogramEntry::Class())) { cando = 1011; pixmap = "dynentryx.png"; } else
        if (cl->InheritsFrom(TGo4HistogramEntry::Class())) { cando = 1011; pixmap = "dynentryx.png"; } else
        if (cl->InheritsFrom(TLatex::Class())) { cando = 110; pixmap = "canvas.png"; } else
        if (cl->InheritsFrom(TF1::Class())) { cando = 110; pixmap = "draw1dline.png"; } else
        if (cl->InheritsFrom(TLeaf::Class())) { cando = 11; pixmap = "leaf_t.png"; } else
        if (cl->InheritsFrom(TGo4AnalysisStatus::Class())) { cando = 1; pixmap = "control.png"; }
      }
   } else if ((kind==TGo4Access::kndFolder) || (kind==TGo4Access::kndMoreFolder)) {
      pixmap = "folder_t.png";
      if (cl && cl->InheritsFrom(TTree::Class())) { cando = 10; pixmap = "tree_t.png"; } else
      if (cl && cl->InheritsFrom(TCanvas::Class())) { cando = 110; pixmap = "canvas.png"; } else
      if (cl && cl->InheritsFrom(THStack::Class())) { cando = 110; pixmap = "superimpose.png"; } else
      if (cl && cl->InheritsFrom(TFile::Class())) { cando = 10000; pixmap = "rootdb_t.png"; } else
      if (cl && cl->InheritsFrom(TGo4HServProxy::Class())) { cando = 10000; pixmap = "histserv.png"; } else
      if (cl && cl->InheritsFrom(TGo4DabcProxy::Class())) { cando = 10000; pixmap = "dabc.png"; } else
      if (cl && cl->InheritsFrom(TGo4AnalysisProxy::Class())) { pixmap = "analysiswin.png"; } else
      if (cl && cl->InheritsFrom(TGo4ServerProxy::Class())) { cando = 10000; pixmap = "http.png"; }
      if (kind==TGo4Access::kndMoreFolder) cando += 10000000;
   } else if (kind==TGo4Access::kndTreeBranch)
      pixmap = "branch_t.png";
   else if (kind==TGo4Access::kndTreeLeaf) {
      cando = 11;  pixmap = "leaf_t.png";
   } else if (kind==TGo4Access::kndGo4Param) {
      cando = 1011; pixmap = "parameter.png";
   } else if (kind==TGo4Access::kndDataMember) {
      cando = 10; pixmap = "eventitem.png";
   } else if (kind==TGo4Access::kndEventElement) {
      cando = 100010; pixmap = "eventobj.png";
   } else if (kind==TGo4Access::kndRootCommand) {
      cando = 100000000; pixmap = "eventobj.png";
   }

   return cando;
}

Bool_t TGo4BrowserProxy::CompareAxisValues(Double_t v1, Double_t v2, Double_t scale)
{
   if (v1==v2) return kTRUE;

   return (fabs(v1-v2) <= scale);
}

Int_t TGo4BrowserProxy::CompareAxis(TAxis* ax1, TAxis* ax2)
{
   // return rebin factor
   // 0 - axis are different
   // 1 - both axis the same
   // >1 - rebin factor than ax2->Rebin(n) will produce ax1

   if (!ax1 || !ax2) return 0;

   Int_t num1 = ax1->GetNbins();
   Int_t num2 = ax2->GetNbins();

   Double_t scale = fabs(ax1->GetXmax() - ax1->GetXmin()) / (num1+1.) * 1e-3;

   if ((num1 != num2)) {
      // Minimum number for rebin is 2, therefore num2 should not more than num1/2
      if ((num1 > num2/2) || (num1 < 2)) return 0;

      Int_t rebin = num2/num1;
      if (rebin == 0) return 0;

      // support only uniform scale rebinning
      if ((ax1->GetXbins()->GetSize() != 0) || (ax2->GetXbins()->GetSize() != 0)) return 0;

      // minimum value on both axis should be the same
      if (!CompareAxisValues(ax1->GetXmin(), ax2->GetXmin(), scale)) return 0;

      Double_t xmax;

      if (num1*rebin!=num2) xmax = ax2->GetBinUpEdge(num1*rebin);
                       else xmax = ax2->GetXmax();

      // maximum value on both axis should be the same
       if (!CompareAxisValues(ax1->GetXmax(), xmax, scale)) return 0;

      return rebin;
   }

   if ((ax1->GetXbins()->GetSize() == 0) && (ax2->GetXbins()->GetSize() == 0)) {
       if (!CompareAxisValues(ax1->GetXmin(), ax2->GetXmin(), scale)) return 0;
       if (!CompareAxisValues(ax1->GetXmax(), ax2->GetXmax(), scale)) return 0;
   } else
      for (Int_t n=1;n<=num1;n++)
         if (!CompareAxisValues(ax1->GetBinCenter(n),
                                ax2->GetBinCenter(n), scale)) return 0;

   return 1;
}


Bool_t TGo4BrowserProxy::UpdateObjectContent(TObject* obj, TObject* newobj, Int_t* hasrebinx, Int_t* hasrebiny)
{
   Bool_t tdisp = kFALSE;
   TString tform;

   if (obj->InheritsFrom(TProfile::Class())) {
      TProfile* profile = dynamic_cast<TProfile*> (obj);
      TProfile* profile2 = dynamic_cast<TProfile*> (newobj);
      if (!profile || !profile2) return kFALSE;
      if (profile->GetNbinsX() != profile2->GetNbinsX()) return kFALSE;
      profile2->Copy(*profile);
      return kTRUE;
   } else if (obj->InheritsFrom(TProfile2D::Class())) {
      TProfile2D* profile = dynamic_cast<TProfile2D*> (obj);
      TProfile2D* profile2 = dynamic_cast<TProfile2D*> (newobj);
      if (!profile || !profile2) return kFALSE;
      if ((profile->GetNbinsX() != profile2->GetNbinsX()) || (profile->GetNbinsY() != profile2->GetNbinsY())) return kFALSE;
      profile2->Copy(*profile);
      return kTRUE;
   } else if (obj->InheritsFrom(TH1::Class())) {
      TH1* histo = dynamic_cast<TH1*> (obj);
      TH1* histo2 = dynamic_cast<TH1*> (newobj);
      if (!histo || !histo2) return kFALSE;

      if (dynamic_cast<TProfile*>(obj) || dynamic_cast<TProfile2D*>(obj)) return kFALSE;

      if (histo->GetDimension()!=histo2->GetDimension()) return kFALSE;

      histo->SetTitle(histo2->GetTitle());

      Int_t rebinx = 1, rebiny = 1;

      rebinx = CompareAxis(histo->GetXaxis(), histo2->GetXaxis());
      if (histo->GetDimension()>1)
         rebiny = CompareAxis(histo->GetYaxis(), histo2->GetYaxis());
      if (histo->GetDimension()>2)
         if (CompareAxis(histo->GetZaxis(), histo2->GetZaxis())!=1) return kFALSE;

      if ((rebinx == 0) || (rebiny == 0)) return kFALSE;

      // in case when object was interactively rebin,
      // try to repeat same operation with new object to perform content update
      if ((rebinx>1) || (rebiny>1)) {
         if (!hasrebinx || !hasrebiny) return kFALSE;

         TH1* clon = (TH1*) histo2->Clone("____dummyhisto____");
         if (!clon) return kFALSE;
         clon->SetDirectory(nullptr);
         Bool_t rebinres = kFALSE;

         if (histo->GetDimension() == 1) {
            clon->Rebin(rebinx);
            rebinres = UpdateObjectContent(histo, clon);
         } else if (histo->GetDimension() == 2) {
            TH2 *h2 = dynamic_cast<TH2 *>(clon);
            if (h2 != 0)
               h2->Rebin2D(rebinx, rebiny);
            rebinres = UpdateObjectContent(histo, h2);
         }
         delete clon;

         if (rebinres) {
            *hasrebinx = rebinx;
            *hasrebiny = rebiny;
         }

         return rebinres;
      }

      Int_t sz = histo->GetNbinsX()+2;
      if (histo->GetDimension()>1)
        sz = sz*(histo->GetNbinsY()+2);
      if (histo->GetDimension()>2)
        sz = sz*(histo->GetNbinsZ()+2);
      Bool_t canrebin = histo->CanExtendAllAxes();
      histo->SetCanExtend(TH1::kNoAxis);
      Double_t sum = 0;
      for (int n=0;n<sz;n++) {
         Stat_t value = histo2->GetBinContent(n);
         sum += value;
         histo->SetBinContent(n, value);
      }

      if (canrebin) histo->SetCanExtend(TH1::kAllAxes);

      histo->SetEntries(sum);

      TArrayD *sumw_tgt = nullptr, *sumw_src = nullptr;

      if (histo2->GetSumw2N() > 0)
         sumw_src = histo2->GetSumw2();

      // if source has no sumw, target should also not has them
      if (!sumw_src) {
         histo->GetSumw2()->Set(0);
      } else {
        if(histo->GetSumw2N() == 0) // JAM2018 workaround to reduce warnings in ROOT 6 (?)
          histo->Sumw2();
        sumw_tgt = histo->GetSumw2();
      }

      if (sumw_src && sumw_tgt)
         sumw_tgt->Set(sz, sumw_src->GetArray());

      return kTRUE;
   } else if (obj->InheritsFrom(TGo4Condition::Class())) {
      TGo4Condition* cond = dynamic_cast<TGo4Condition*> (obj);
      TGo4Condition* newcond = dynamic_cast<TGo4Condition*> (newobj);
      if (!cond || !newcond) return kFALSE;

      cond->UpdateFrom(newcond, kTRUE);
      cond->SetChanged(kFALSE);

      return kTRUE;
   } else if (obj->InheritsFrom(TGraphAsymmErrors::Class())) {
      TGraphAsymmErrors* gr = dynamic_cast<TGraphAsymmErrors*> (obj);
      TGraph* newgr = dynamic_cast<TGraph*> (newobj);
      if (!gr || !newgr) return kFALSE;
      SaveAxisTimeProperties(gr,tdisp,tform);
      gr->SetTitle(newgr->GetTitle());

      Int_t npoints = newgr->GetN();
      gr->Set(npoints);
      Double_t xp, yp, exh, exl, eyh, eyl;
      for (Int_t n=0;n<npoints;n++) {
        newgr->GetPoint(n,xp,yp);
        gr->SetPoint(n,xp,yp);
        exh = newgr->GetErrorXhigh(n);
        exl = newgr->GetErrorXlow(n);
        eyh = newgr->GetErrorYhigh(n);
        eyl = newgr->GetErrorYlow(n);
        gr->SetPointError(n, exl, exh, eyl, eyh);
      }

      UpdateListOfFunctions(gr,newgr);
      RestoreAxisTimeProperties(gr,tdisp,tform);
      return kTRUE;
   } else if (obj->InheritsFrom(TGraphErrors::Class())) {
      TGraphErrors* gr = dynamic_cast<TGraphErrors*> (obj);
      TGraph* newgr = dynamic_cast<TGraph*> (newobj);
      if (!gr || !newgr) return kFALSE;
      SaveAxisTimeProperties(gr,tdisp,tform);
      gr->SetTitle(newgr->GetTitle());

      Int_t npoints = newgr->GetN();
      gr->Set(npoints);
      Double_t xp, yp, ex, ey;
      for (Int_t n=0;n<npoints;n++) {
        newgr->GetPoint(n,xp,yp);
        gr->SetPoint(n,xp,yp);
        ex = newgr->GetErrorX(n);
        ey = newgr->GetErrorY(n);
        gr->SetPointError(n, ex, ey);
      }

      UpdateListOfFunctions(gr,newgr);
      RestoreAxisTimeProperties(gr,tdisp,tform);

      return kTRUE;
   } else if (obj->InheritsFrom(TGraph::Class())) {
      TGraph* gr = dynamic_cast<TGraph*> (obj);
      TGraph* newgr = dynamic_cast<TGraph*> (newobj);
      if (!gr || !newgr) return kFALSE;
      // JAM: save axis time properties of currently displayed histo
      SaveAxisTimeProperties(gr,tdisp,tform);

      gr->SetTitle(newgr->GetTitle());

      Int_t npoints = newgr->GetN();
      gr->Set(npoints);
      Double_t xp, yp;
      for (Int_t n=0;n<npoints;n++) {
        newgr->GetPoint(n,xp,yp);
        gr->SetPoint(n,xp,yp);
      }
      UpdateListOfFunctions(gr,newgr);
      RestoreAxisTimeProperties(gr,tdisp,tform);

      return kTRUE;
   } else if (obj->InheritsFrom(TLatex::Class())) {
      TLatex* l0 = dynamic_cast<TLatex*> (obj);
      TLatex* l1 = dynamic_cast<TLatex*> (newobj);
      if (!l1 || !l0) return kFALSE;

      l0->SetTitle(l1->GetTitle());

      return kTRUE;
   } else if (obj->InheritsFrom(TF1::Class())) {
      //std::cout <<"Update object contents with TF1"<<obj->GetName() << std::endl;
      TF1* f0 = dynamic_cast<TF1*> (obj);
      TF1* f1 = dynamic_cast<TF1*> (newobj);
      if (!f1 || !f0) return kFALSE;
      f1->Copy(*f0);
      return kTRUE;
   }

   return kFALSE;
}

void TGo4BrowserProxy::SaveAxisTimeProperties(TGraph* gr, Bool_t& timedisplay, TString& format)
{
   if(!gr) return;
   TH1* h1 = gr->GetHistogram();
   TAxis* xax = h1->GetXaxis();
   timedisplay = xax->GetTimeDisplay();
   format = xax->GetTimeFormat();
}

void TGo4BrowserProxy::RestoreAxisTimeProperties(TGraph* gr, Bool_t& timedisplay, TString& format)
{
   if(!gr) return;
   TH1*h1=gr->GetHistogram();
   TAxis* xax=h1->GetXaxis();
   xax->SetTimeDisplay(timedisplay);
   xax->SetTimeFormat(format.Data());
}


void TGo4BrowserProxy::UpdateListOfFunctions(TGraph* oldgr, TGraph* newgr)
{
   if(!oldgr || !newgr) return;
   TList* theFunctions=oldgr->GetListOfFunctions();
   TObject *obj = nullptr;
   while ((obj  = theFunctions->First()) != nullptr) {
      while (theFunctions->Remove(obj)) { }
      delete obj;
   }

   TList* newFunctions = newgr->GetListOfFunctions();
   TListIter fiter(newFunctions);
   TF1* fun = nullptr;
   while((fun = dynamic_cast<TF1*>(fiter.Next())) != nullptr) {
     TF1* fclon = dynamic_cast<TF1*>(fun->Clone());
     theFunctions->Add(fclon);
     fclon->SetParent(oldgr);
   }

   // also restore graph axis titles here:
   TH1* oldhis = oldgr->GetHistogram();
   TH1* newhis = newgr->GetHistogram();
   if(oldhis && newhis) {
      oldhis->GetXaxis()->SetTitle(newhis->GetXaxis()->GetTitle());
      oldhis->GetYaxis()->SetTitle(newhis->GetYaxis()->GetTitle());
   }
}


void TGo4BrowserProxy::AddWaitingList(TGo4Slot* itemslot, const char* destination)
{
   if (!itemslot) return;
   if (!fxWaitingList) fxWaitingList = new TList;

   TString itemname;
   if (!BrowserItemName(itemslot, itemname)) return;

   TNamed* n = (TNamed*) fxWaitingList->FindObject(itemname.Data());
   if (n)
     n->SetTitle(destination);
   else
     fxWaitingList->Add(new TNamed(itemname.Data(), destination));
}

void TGo4BrowserProxy::CheckWaitingList(TGo4Slot* source)
{
   if (!fxWaitingList) return;

   TString itemname;
   if (!BrowserItemName(source, itemname)) return;

   TNamed* n = (TNamed*) fxWaitingList->FindObject(itemname.Data());
   if (n) {
      const char* dest = n->GetTitle();
      if (dest && (strlen(dest) == 0)) dest = nullptr;
      ProduceExplicitCopy(source, dest, kFALSE);
      fxWaitingList->Remove(n);
      delete n;
   }
}

// used by ROOT browser

void TGo4BrowserProxy::UpdateAllCanvases()
{
   TIter next(gROOT->GetListOfCanvases());
   while (auto pad = (TPad*) next()) {
      pad->Modified();

      TVirtualPad* subpad = nullptr;
      Int_t number = 0;
      while ((subpad = pad->GetPad(number++)) != nullptr)
        subpad->Modified();

      pad->Update();
   }
}
