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

#include "TGo4RootBrowserProxy.h"

#include "TROOT.h"
#include "TCanvas.h"
#include "TClass.h"
#include "TH1.h"
#include "TMath.h"
#include "TTimer.h"
#include "TBrowser.h"
#include "TGMimeTypes.h"

#include "TGo4Log.h"
#include "TGo4Slot.h"
#include "TGo4Iter.h"
#include "TGo4Picture.h"
#include "TGo4Condition.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisClientStatus.h"
#include "TGo4BrowserItem.h"

Int_t TGo4RootBrowserProxy::fCanvasCounter = 0;

TGo4RootBrowserProxy::TGo4RootBrowserProxy(TGo4BrowserProxy* br) :
   TGo4Proxy(),
   fBrowser(br),
   fLockMessage(kFALSE)
{
}

TGo4RootBrowserProxy::~TGo4RootBrowserProxy()
{
   TGo4BrowserItem* topfold = (TGo4BrowserItem*) gROOT->GetListOfBrowsables()->FindObject("go4");
   if (topfold) {
      gROOT->GetListOfBrowsables()->Remove(topfold);
      delete topfold;
   }
}

Bool_t TGo4RootBrowserProxy::ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param)
{
   if ((id == TGo4Slot::evObjAssigned) || (id == TGo4Slot::evObjUpdated)) {
      if (strcmp(source->GetName(), "Go4Browser") == 0)
         SyncRootBrowserSlots();
      if (strcmp(source->GetName(), "AnalLoginfo") == 0)
         UpdateLoginfo(source->GetAssignedObject());
      if (strcmp(source->GetName(), "AnalRateMeter") == 0)
         UpdateRatemeter(source->GetAssignedObject());
   } else if ((id == TGo4Slot::evDelete) || (id == TGo4Slot::evObjDeleted)) {
      // std::cout << "Slot deleted " << source->GetName() << std::endl;
   }

   return id == TGo4Slot::evDelete;
}

void TGo4RootBrowserProxy::Message(const char* str1, const char* str2, Int_t blockdelay)
{
   TIter iter(gROOT->GetListOfBrowsers());

   while (auto br = dynamic_cast<TBrowser*> (iter())) {
      br->SetStatusText(str1, 0);
      br->SetStatusText(str2, 1);
   }

   if (blockdelay > 0) {
       fLockMessage = kTRUE;
       TTimer::SingleShot(blockdelay, "TGo4RootBrowserProxy", this, "UnblockStatusOutput()");
   }
}

void TGo4RootBrowserProxy::UpdateRatemeter(TObject* obj)
{
   if (fLockMessage) return;

   TGo4AnalysisClientStatus* anal =
      dynamic_cast<TGo4AnalysisClientStatus*> (obj);
   if (!anal) return;

   const char* header = nullptr;

   if(anal->IsAnalysisRunning())
      header = "Analysis running";
   else
      header = "Analysis stopped";

   TString res;

   if (anal->GetRate() < 10)
      res.Form("Rate = %6.4f Events = %d Time = %d Date = %s",
              anal->GetRate(),
              TMath::Nint(TMath::Floor(anal->GetCurrentCount())),
              TMath::Nint(TMath::Floor(anal->GetTime())),
              anal->GetDateTime());
     else
       res.Form("Rate = %d   Events = %d Time = %d Date = %s",
              TMath::Nint(TMath::Floor(anal->GetRate())),
              TMath::Nint(TMath::Floor(anal->GetCurrentCount())),
              TMath::Nint(TMath::Floor(anal->GetTime())),
              anal->GetDateTime());

   Message(header, res.Data(), 0);
}


void TGo4RootBrowserProxy::UpdateLoginfo(TObject* obj)
{
   std::cout << "Loginfo = " << obj->GetName() << std::endl;

   Message("Message",obj->GetName(), 5000);
}

void TGo4RootBrowserProxy::UnblockStatusOutput()
{
   fLockMessage = kFALSE;
}

void TGo4RootBrowserProxy::SyncRootBrowserSlots()
{
   if (!fBrowser) return;

   TGo4BrowserItem* topfold = (TGo4BrowserItem*) gROOT->GetListOfBrowsables()->FindObject("go4");
   if (!topfold) {
      topfold = new TGo4BrowserItem(0, 0, "go4","Top go4 objects folder");
      topfold->SetTitle("Top Go4 folder");
      topfold->SetItemClass("TFolder");
      topfold->SetIconName("Go4Logo");
      topfold->SetBrowser(fBrowser, this);

      gROOT->GetListOfBrowsables()->Add(topfold);

      TString img = TGo4Log::subGO4SYS("icons/");

      TGMimeTypes* mt = gClient->GetMimeTypeList();
      mt->AddType("go4/logo", "Go4Logo", img + "go4logo2_big.png", img + "go4logo2.png", "->Draw()");
      mt->AddType("go4/graph", "TGraph", img + "tgraph.png", img + "tgraph.png", "->Draw()");
      mt->AddType("go4/multigraph", "TMultiGraph", img + "superimpose.png", img + "superimpose.png", "->Draw()");
      mt->AddType("go4/picture", "TGo4Picture", img + "picture.png", img + "picture.png", "->Draw()");
      mt->AddType("go4/param", "TGo4Parameter", img + "parameter.png", img + "parameter.png", "->Draw()");
      mt->AddType("go4/fitter", "TGo4Fitter", img + "fitter.png", img + "fitter.png", "->Draw()");
      mt->AddType("go4/wincond", "TGo4WinCond", img + "windcond.png", img + "windcond.png", "->Draw()");
      mt->AddType("go4/polycond", "TGo4PolyCond", img + "polycond.png", img + "polycond.png", "->Draw()");
      mt->AddType("go4/condarr", "TGo4CondArray", img + "windcondarray.png", img + "windcondarray.png", "->Draw()");
      mt->AddType("go4/tentry", "TGo4TreeHistogramEntry", img + "dynentryx.png", img + "dynentryx.png", "->Draw()");
      mt->AddType("go4/pentry", "TGo4HistogramEntry", img + "dynentryx.png", img + "dynentryx.png", "->Draw()");
      mt->AddType("go4/hserv", "TGo4HServProxy", img + "histserv.png", img + "histserv.png", "->Draw()");
      mt->AddType("go4/dabcserv", "TGo4DabcProxy", img + "dabc.png", img + "dabc.png", "->Draw()");
      mt->AddType("go4/analysis", "TGo4AnalysisProxy", img + "analysiswin.png", img + "analysiswin.png", "->Draw()");
      /** cannot add type while  TGo4HttpProxy has no dictionary */
      //mt->AddType("go4/httpserv", "TGo4HttpProxy", img + "http.png", img + "http.png", "->Draw()");
      mt->AddType("go4/canvas", "TCanvas", img + "canvas.png", img + "canvas.png", "->Draw()");
      mt->AddType("go4/tbranch", "Go4-TreeBranch", img + "branch_t.png", img + "branch_t.png", "->Draw()");
      mt->AddType("go4/tleaf", "Go4-TreeLeaf", img + "leaf_t.png", img + "leaf_t.png", "->Draw()");
      mt->AddType("go4/datamember", "Go4-EventDataMember", img + "eventitem.png", img + "eventitem.png", "->Draw()");
      mt->AddType("go4/event", "Go4-EventElement", img + "eventobj.png", img + "eventobj.png", "->Draw()");
   }

   TGo4BrowserItem* curfold = topfold;
   TGo4BrowserItem* curitem = curfold->firstChild();
   TGo4BrowserItem* previtem = nullptr;

   TObjArray testedClasses;

   TGo4Iter iter(fBrowser->BrowserTopSlot(), kTRUE);

   while (true) {
       Bool_t res = iter.next();

      // go to top folders and remove rest items
      Int_t levelchange = iter.levelchange();
      while (levelchange++ < 0) {

          while (curitem) {
            TGo4BrowserItem* next = curfold->nextChild();
            curfold->deleteChild(curitem);
            curitem = next;
          }

          if (!curfold) break;

          previtem = curfold;
          curfold = curfold->GetParent();
          curitem = !curfold ? nullptr : curfold->nextChild();
      }

      if (!res) break;

      // delete all slots in folder, which has another name
      while (curitem && (strcmp(iter.getname(), curitem->GetName()) != 0)) {
         TGo4BrowserItem* next = curfold->nextChild();
         curfold->deleteChild(curitem);
         curitem = next;
      }

      TGo4Slot* curslot = iter.getslot();
      if (!curslot) {
         std::cerr << "************* ERROR in gui slots ****************** " << std::endl;
         return;
      }

      const char* classname = TGo4BrowserProxy::ItemClassName(curslot);
      const char* iteminfo = TGo4BrowserProxy::ItemInfo(curslot);
      Int_t itemkind = TGo4BrowserProxy::ItemKind(curslot);
      TClass* itemclass = nullptr;

      if (classname && !testedClasses.FindObject(classname)) {
         itemclass = gROOT->GetClass(classname);
         if (!itemclass)
           testedClasses.Add(new TNamed(classname,""));
      }

      TString pixmap;
      int cando = TGo4BrowserProxy::DefineItemProperties(itemkind, itemclass, pixmap);
      TGo4BrowserProxy::SetItemCanDo(curslot, cando);

      if (!curitem)
          curitem = new TGo4BrowserItem(curfold, previtem, iter.getname(), classname);

      curitem->SetBrowser(fBrowser, this);
      curitem->SetTitle(iteminfo);
      curitem->SetItemClass(classname);

      TString iconname = classname;
      if (itemkind == TGo4Access::kndTreeBranch)
         iconname = "Go4-TreeBranch";
      else if (itemkind == TGo4Access::kndTreeLeaf)
         iconname = "Go4-TreeLeaf";
      else if (itemkind == TGo4Access::kndGo4Param)
         iconname = "TGo4Parameter";
      else if (itemkind == TGo4Access::kndDataMember)
         iconname = "Go4-EventDataMember";
      else if (itemkind == TGo4Access::kndEventElement)
         iconname = "Go4-EventElement";

      curitem->SetIconName(iconname.Data());

      if (iter.isfolder()) {
         curitem->SetIsFolder(kTRUE);
         curfold = curitem;
         curitem = curfold->firstChild();
         previtem = nullptr;
      } else {
         // remove any substructures if any
         curitem->SetIsFolder(kFALSE);
         curitem->deleteChilds();
         previtem = curitem;
         curitem = curfold->nextChild();
      }
   }

   while (curitem) {
      TGo4BrowserItem* next = curfold->nextChild();
      curfold->deleteChild(curitem);
      curitem = next;
   }

   testedClasses.Delete();
}

TCanvas* TGo4RootBrowserProxy::MakeCanvas(const char* title)
{
   TString cname = "Canvas_";
   cname += fCanvasCounter++;
   TCanvas* c1 = nullptr;

   if (!title) c1 = new TCanvas(cname, cname+" title");
          else c1 = new TCanvas(cname, title);
   c1->cd();

   return c1;
}

void TGo4RootBrowserProxy::DrawPicture(const char* picitemname, TGo4Picture* pic, TPad* pad)
{
   if (!pad || !pic) return;

   pad->cd();

   if (pic->IsDivided()) {
      pad->Divide(pic->GetDivX(), pic->GetDivY());

      for(Int_t posy=0; posy<pic->GetDivY(); posy++)
         for(Int_t posx=0; posx<pic->GetDivX(); posx++) {
           TGo4Picture* sub = pic->FindPic(posy,posx);
           if (sub)
             DrawPicture(picitemname, sub, (TPad*) pad->GetPad(posy*pic->GetDivX() + posx + 1));
       }
      return;
   }

   pic->GetDrawAttributes(pad, TGo4Picture::PictureIndex);

   TH1* h1 = nullptr;

   for (Int_t indx=0; indx<pic->GetNumObjNames(); indx++) {
      Option_t* drawopt = pic->GetDrawOption(indx);
      const char* objname = pic->GetObjName(indx);

      TString drawname;

      TObject* obj = nullptr;

      if (fBrowser->DefineRelatedObject(picitemname, objname, drawname))
         obj = fBrowser->GetBrowserObject(drawname.Data(), 5000);
      if (!obj) continue;

      if (!h1)
        h1 = dynamic_cast<TH1*>(obj);

      pic->GetDrawAttributes(obj, indx);

      if (obj->InheritsFrom(TGo4Condition::Class())) {
         TGo4Condition* con = (TGo4Condition*) obj;
         con->SetWorkHistogram(h1);
         con->SetLineColor(2);
         con->SetFillColor(2);
         con->SetFillStyle(3444);
      }

      obj->Draw(drawopt);
   }

   pad->Modified();
}

void TGo4RootBrowserProxy::DrawCondition(const char* itemname, TGo4Condition* con)
{
   if (!con || !fBrowser) return;

   const char* hname = con->GetLinkedHistogram();
   TString hitemname;

   TH1* h1 = nullptr;

   if (hname && fBrowser->DefineRelatedObject(itemname, hname, hitemname))
      h1 = dynamic_cast<TH1 *>(fBrowser->GetBrowserObject(hitemname.Data(), 5000));

   if (!h1) return;

   TCanvas* c1 = MakeCanvas(TString("Condition ") + con->GetName() + " on histo " + h1->GetName());

   h1->Draw();
   con->SetWorkHistogram(h1);
   con->SetLineColor(2);
   con->SetFillColor(2);
   con->SetFillStyle(3444);
   con->Draw();

   c1->Update();
}

void TGo4RootBrowserProxy::DrawItem(const char* itemname)
{
   if (!itemname || !fBrowser) return;

   Int_t cando = fBrowser->ItemCanDo(itemname);

   if (!TGo4BrowserProxy::CanDrawItem(cando)) {
      TClass* cl = fBrowser->ItemClass(itemname);
      if (!cl || !cl->InheritsFrom(TGo4Condition::Class())) return;
   }

   TObject* obj = fBrowser->GetBrowserObject(itemname, 5000);
   if (!obj) return;

   if (obj->InheritsFrom(TGo4Condition::Class())) {
      DrawCondition(itemname, (TGo4Condition*) obj);
      return;
   }

   TPad* pad = (TPad*) gPad;
   if (!pad) pad = MakeCanvas(TString("Drawing of ") + obj->GetName());
        else pad->Clear();

   if (obj->InheritsFrom(TGo4Picture::Class()))
      DrawPicture(itemname, (TGo4Picture*) obj, pad);
   else
     obj->Draw();

   pad->Update();
}
