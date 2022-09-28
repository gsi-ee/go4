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

#include "TGo4BrowserItem.h"

#include "TGo4BrowserProxy.h"
#include "TGo4RootBrowserProxy.h"
#include "TGo4Iter.h"
#include "TGo4Slot.h"
#include "TGo4ServerProxy.h"

#include "TGo4Interface.h"

TGo4BrowserItem::TGo4BrowserItem() :
   TFolder(),
   fParent(nullptr),
   fIter(nullptr),
   fItemClass(),
   fIsFolder(kFALSE),
   fBrowser(nullptr)
{
}

TGo4BrowserItem::TGo4BrowserItem(const char *name, const char *title) :
   TFolder(name, title),
   fParent(nullptr),
   fIter(nullptr),
   fItemClass(),
   fIsFolder(kTRUE),
   fBrowser(nullptr)
{
   SetOwner(kTRUE);
}

TGo4BrowserItem::TGo4BrowserItem(TGo4BrowserItem* parent, TGo4BrowserItem* previtem,
                               const char *name, const char *title) :
   TFolder(name, title),
   fParent(parent),
   fIter(nullptr),
   fItemClass(),
   fIsFolder(kTRUE),
   fBrowser(nullptr)
{
   SetOwner(kTRUE);
   if (parent) {
      TList* list = (TList*) parent->GetListOfFolders();
      if (!previtem) list->AddFirst(this);
                else list->AddAfter(previtem, this);
   }
}

TGo4BrowserItem::~TGo4BrowserItem()
{
   if (fIter) delete fIter;
}

void TGo4BrowserItem::SetBrowser(TGo4BrowserProxy* br, TGo4RootBrowserProxy* br2)
{
   fBrowser = br;
   fRootBrowser = br2;
}

TGo4BrowserItem* TGo4BrowserItem::firstChild()
{
    if (fIter) delete fIter;
    fIter = GetListOfFolders()->MakeIterator();
    return (TGo4BrowserItem*) fIter->Next();
}

TGo4BrowserItem* TGo4BrowserItem::nextChild()
{
    if (!fIter) return nullptr;
    TGo4BrowserItem* res = dynamic_cast<TGo4BrowserItem*> (fIter->Next());
    if (!res) { delete fIter; fIter = nullptr; }
    return res;
}

void TGo4BrowserItem::deleteChild(TGo4BrowserItem* item)
{
   if (!item) return;
   Remove(item);
   delete item;
}

void TGo4BrowserItem::deleteChilds()
{
   if (fIter) { delete fIter; fIter = nullptr; }
   GetListOfFolders()->Delete();
}

void TGo4BrowserItem::ProduceFullName(TString& fullname)
{
   if (GetParent()) {
     GetParent()->ProduceFullName(fullname);
     if (fullname.Length()>0) fullname+="/";
     fullname += GetName();
   } else
     fullname = "";
}

TString TGo4BrowserItem::GetFullName()
{
   TString res;
   ProduceFullName(res);
   return res;

}

void TGo4BrowserItem::Browse(TBrowser* b)
{
   if (IsFolder()) TFolder::Browse(b);

   DrawItem();
}

void TGo4BrowserItem::DrawItem()
{
   TString itemname;

   ProduceFullName(itemname);

   fRootBrowser->DrawItem(itemname.Data());
}

void TGo4BrowserItem::CopyToWorkspace()
{
   TString itemname;
   ProduceFullName(itemname);
   if (itemname.IsNull() || !fBrowser) return;

   fBrowser->ProduceExplicitCopy(itemname.Data(), nullptr, kTRUE);
}

void TGo4BrowserItem::DeleteItem()
{
   if (!GetParent()) {
      TGo4Interface::DeleteInstance();
      return;
   }


   TString itemname;
   ProduceFullName(itemname);
   if (itemname.IsNull() || !fBrowser) return;

   TGo4Slot *itemslot = fBrowser->ItemSlot(itemname.Data());
   if (!itemslot) return;

   TGo4Slot *memslot = fBrowser->BrowserMemorySlot();

   if (fBrowser->IsCanDelete(itemslot) || itemslot->IsParent(memslot))
     fBrowser->DeleteDataSource(itemslot);
}

void TGo4BrowserItem::SetMonitorOn()
{
   SetMonitorFlag(kTRUE);
}

void TGo4BrowserItem::SetMonitorOff()
{
   SetMonitorFlag(kFALSE);
}

void TGo4BrowserItem::SetMonitorFlag(Bool_t on)
{
   TString itemname;
   ProduceFullName(itemname);
   if (itemname.IsNull() || !fBrowser) return;

   TGo4Slot *itemslot = fBrowser->ItemSlot(itemname.Data());
   if (!itemslot) return;
   int kind = fBrowser->ItemKind(itemslot);

   if (kind==TGo4Access::kndFolder) {
      TGo4Iter iter(itemslot, kTRUE);
      while (iter.next()) {
         TGo4Slot *subslot = iter.getslot();
         if (fBrowser->ItemKind(subslot)==TGo4Access::kndObject)
            fBrowser->SetItemMonitored(subslot, on);
      }
  } else
      fBrowser->SetItemMonitored(itemslot, on);
}

void TGo4BrowserItem::ToggleMonitoring(Int_t sec)
{
   if (fBrowser)
      fBrowser->ToggleMonitoring(sec*1000);
}

void TGo4BrowserItem::StartAnalysis()
{
   TGo4ServerProxy* anal = fBrowser->FindServer();
   if (anal)
      anal->StartAnalysis();
}

void TGo4BrowserItem::StopAnalysis()
{
   TGo4ServerProxy* anal = fBrowser->FindServer();
   if (anal)
      anal->StopAnalysis();
}


void TGo4BrowserItem::Delete(Option_t* option)
{
    DeleteItem();
}

void TGo4BrowserItem::SetName(const char *name)
{
   TFolder::SetName(name);
}

void TGo4BrowserItem::SetTitle(const char *title)
{
   TFolder::SetTitle(title);
}

void TGo4BrowserItem::ls(Option_t* option) const
{
   TFolder::ls(option);
}

void TGo4BrowserItem::SaveAs(const char *filename, Option_t *option) const
{
   TFolder::SaveAs(filename, option);
}

void TGo4BrowserItem::DrawClass() const
{
   TFolder::DrawClass();
}

TObject* TGo4BrowserItem::DrawClone(Option_t* option) const
{
   return TFolder::DrawClone(option);
}

void TGo4BrowserItem::Dump() const
{
   TFolder::Dump();
}

void TGo4BrowserItem::Inspect() const
{
   TFolder::Inspect();
}

void TGo4BrowserItem::SetDrawOption(Option_t* option)
{
   TFolder::SetDrawOption(option);
}
