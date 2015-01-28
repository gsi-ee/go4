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

#include "TGo4RootBrowserProxy.h"

#include "Riostream.h"
#include "RVersion.h"

#include "TROOT.h"
#include "TPad.h"
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

#if ROOT_VERSION_CODE <= ROOT_VERSION(5,17,4)
#include "TRootBrowser.h"
#include "TGStatusBar.h"
#endif

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
   if (topfold!=0) {
      gROOT->GetListOfBrowsables()->Remove(topfold);
      delete topfold;
   }
}

Bool_t TGo4RootBrowserProxy::ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param)
{
  if ((id==TGo4Slot::evObjAssigned) ||
      (id==TGo4Slot::evObjUpdated)) {
         if (strcmp(source->GetName(), "Go4Browser")==0)
            SyncRootBrowserSlots();
         if (strcmp(source->GetName(), "AnalLoginfo")==0)
            UpdateLoginfo(source->GetAssignedObject());
         if (strcmp(source->GetName(), "AnalRateMeter")==0)
            UpdateRatemeter(source->GetAssignedObject());
      }
   else
   if ((id==TGo4Slot::evDelete) ||
       (id==TGo4Slot::evObjDeleted)) {
          // std::cout << "Slot deleted " << source->GetName() << std::endl;
       }

   return (id==TGo4Slot::evDelete);
}

void TGo4RootBrowserProxy::Message(const char* str1, const char* str2, Int_t blockdelay)
{
   TIter iter(gROOT->GetListOfBrowsers());
   TBrowser* br = 0;

   while ((br = dynamic_cast<TBrowser*> (iter())) !=0 ) {

#if ROOT_VERSION_CODE > ROOT_VERSION(5,17,4)
      br->SetStatusText(str1, 0);
      br->SetStatusText(str2, 1);
#else
      TRootBrowser* imp = dynamic_cast<TRootBrowser*> (br->GetBrowserImp());
      if (imp==0) continue;

      imp->ShowStatusBar(kTRUE);

      TGStatusBar* status = imp->GetStatusBar();
      if (status!=0) {
         status->SetText(str1, 0);
         status->SetText(str2, 1);
      }
#endif
   }

   if (blockdelay>0) {
       fLockMessage = kTRUE;
       TTimer::SingleShot(blockdelay, "TGo4RootBrowserProxy", this, "UnblockStatusOutput()");
   }
}

void TGo4RootBrowserProxy::UpdateRatemeter(TObject* obj)
{
   if (fLockMessage) return;

   TGo4AnalysisClientStatus* anal =
     dynamic_cast<TGo4AnalysisClientStatus*> (obj);
   if (anal==0) return;

   const char* header = 0;

   if(anal->IsAnalysisRunning())
      header = "Analysis running";
   else
      header = "Analysis stopped";

   TString res;

   if (anal->GetRate()<10)
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
   if (fBrowser==0) return;

   TGo4BrowserItem* topfold = (TGo4BrowserItem*) gROOT->GetListOfBrowsables()->FindObject("go4");
   if (topfold==0) {
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
      mt->AddType("go4/httpserv", "TGo4HttpProxy", img + "http.png", img + "http.png", "->Draw()");
      mt->AddType("go4/canvas", "TCanvas", img + "canvas.png", img + "canvas.png", "->Draw()");
      mt->AddType("go4/analysis", "TGo4AnalysisProxy", img + "analysiswin.png", img + "analysiswin.png", "->Draw()");
      mt->AddType("go4/tbranch", "Go4-TreeBranch", img + "branch_t.png", img + "branch_t.png", "->Draw()");
      mt->AddType("go4/tleaf", "Go4-TreeLeaf", img + "leaf_t.png", img + "leaf_t.png", "->Draw()");
      mt->AddType("go4/datamember", "Go4-EventDataMember", img + "eventitem.png", img + "eventitem.png", "->Draw()");
      mt->AddType("go4/event", "Go4-EventElement", img + "eventobj.png", img + "eventobj.png", "->Draw()");
   }

   TGo4BrowserItem* curfold = topfold;
   TGo4BrowserItem* curitem = curfold->firstChild();
   TGo4BrowserItem* previtem = 0;

   TObjArray testedClasses;

   TGo4Iter iter(fBrowser->BrowserTopSlot(), kTRUE);

   while (true) {
       Bool_t res = iter.next();

      // go to top folders and remove rest items
      Int_t levelchange = iter.levelchange();
      while (levelchange++<0) {

          while (curitem!=0) {
            TGo4BrowserItem* next = curfold->nextChild();
            curfold->deleteChild(curitem);
            curitem = next;
          }

          if (curfold==0) break;

          previtem = curfold;
          curfold = curfold->GetParent();
          curitem = curfold==0 ? 0 : curfold->nextChild();
      }

      if (!res) break;

      // delete all slots in folder, which has another name
      while ((curitem!=0) && (strcmp(iter.getname(), curitem->GetName())!=0)) {
         TGo4BrowserItem* next = curfold->nextChild();
         curfold->deleteChild(curitem);
         curitem = next;
      }

      TGo4Slot* curslot = iter.getslot();
      if (curslot==0) {
         std::cerr << "************* ERROR in gui slots ****************** " << std::endl;
         return;
      }

      const char* classname = TGo4BrowserProxy::ItemClassName(curslot);
      const char* iteminfo = TGo4BrowserProxy::ItemInfo(curslot);
      Int_t itemkind = TGo4BrowserProxy::ItemKind(curslot);
      TClass* itemclass = 0;

      if ((classname!=0) && (testedClasses.FindObject(classname)==0)) {
         itemclass = gROOT->GetClass(classname);
         if (itemclass==0)
           testedClasses.Add(new TNamed(classname,""));
      }

      TString pixmap;
      int cando = TGo4BrowserProxy::DefineItemProperties(itemkind, itemclass, pixmap);
      TGo4BrowserProxy::SetItemCanDo(curslot, cando);

      if (curitem==0)
          curitem = new TGo4BrowserItem(curfold, previtem, iter.getname(), classname);

      curitem->SetBrowser(fBrowser, this);
      curitem->SetTitle(iteminfo);
      curitem->SetItemClass(classname);

      TString iconname = classname;
      if (itemkind==TGo4Access::kndTreeBranch)
         iconname = "Go4-TreeBranch";
      else
      if (itemkind==TGo4Access::kndTreeLeaf)
         iconname = "Go4-TreeLeaf";
      else
      if (itemkind==TGo4Access::kndGo4Param)
         iconname = "TGo4Parameter";
      else
      if (itemkind==TGo4Access::kndDataMember)
         iconname = "Go4-EventDataMember";
      else
      if (itemkind==TGo4Access::kndEventElement)
         iconname = "Go4-EventElement";

      curitem->SetIconName(iconname.Data());

      if (iter.isfolder()) {
         curitem->SetIsFolder(kTRUE);
         curfold = curitem;
         curitem = curfold->firstChild();
         previtem = 0;
      } else {
         // remove any substructures if any
         curitem->SetIsFolder(kFALSE);
         curitem->deleteChilds();
         previtem = curitem;
         curitem = curfold->nextChild();
      }
   }

   while (curitem!=0) {
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
   TCanvas* c1 = 0;

   if (title==0) c1 = new TCanvas(cname, cname+" title");
            else c1 = new TCanvas(cname, title);
   c1->cd();

   return c1;
}

void TGo4RootBrowserProxy::DrawPicture(const char* picitemname, TGo4Picture* pic, TPad* pad)
{
   if ((pad==0) || (pic==0)) return;

   pad->cd();

   if (pic->IsDivided()) {
      pad->Divide(pic->GetDivX(), pic->GetDivY());

      for(Int_t posy=0; posy<pic->GetDivY(); posy++)
         for(Int_t posx=0; posx<pic->GetDivX(); posx++) {
           TGo4Picture* sub = pic->FindPic(posy,posx);
           if (sub!=0)
             DrawPicture(picitemname, sub, (TPad*) pad->GetPad(posy*pic->GetDivX() + posx + 1));
       }
      return;
   }

   pic->GetDrawAttributes(pad, TGo4Picture::PictureIndex);

   TH1* h1 = 0;

   for (Int_t indx=0; indx<pic->GetNumObjNames(); indx++) {
      Option_t* drawopt = pic->GetDrawOption(indx);
      const char* objname = pic->GetObjName(indx);

      TString drawname;

      TObject* obj = 0;

      if (fBrowser->DefineRelatedObject(picitemname, objname, drawname))
         obj = fBrowser->GetBrowserObject(drawname.Data(), 5000);
      if (obj==0) continue;

      if (h1==0)
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
   if ((con==0) || (fBrowser==0)) return;

   const char* hname = con->GetLinkedHistogram();
   TString hitemname;

   TH1* h1 = 0;

   if ((hname!=0) &&
      fBrowser->DefineRelatedObject(itemname, hname, hitemname))
      h1 = dynamic_cast<TH1*> (fBrowser->GetBrowserObject(hitemname.Data(), 5000));

   if (h1==0) return;

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
   if ((itemname==0) || (fBrowser==0)) return;

   Int_t cando = fBrowser->ItemCanDo(itemname);

   if (!TGo4BrowserProxy::CanDrawItem(cando)) {
      TClass* cl = fBrowser->ItemClass(itemname);
      if ((cl==0) || !cl->InheritsFrom(TGo4Condition::Class())) return;
   }


   TObject* obj = fBrowser->GetBrowserObject(itemname, 5000);
   if (obj==0) return;

   if (obj->InheritsFrom(TGo4Condition::Class())) {
      DrawCondition(itemname, (TGo4Condition*) obj);
      return;
   }

   TPad* pad = (TPad*) gPad;
   if (pad==0) pad = MakeCanvas(TString("Drawing of ") + obj->GetName());
          else pad->Clear();

//   TCanvas* c1 = MakeCanvas(TString("Drawing of ") + obj->GetName());

   if (obj->InheritsFrom(TGo4Picture::Class()))
      DrawPicture(itemname, (TGo4Picture*) obj, pad);
   else
     obj->Draw();

   pad->Update();
}
