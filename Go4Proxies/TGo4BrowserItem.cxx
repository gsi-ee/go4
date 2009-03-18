#include "TGo4BrowserItem.h"

#include "TCanvas.h"
#include "TH1.h"

#include "TGo4BrowserProxy.h"
#include "TGo4RootBrowserProxy.h"
#include "TGo4Iter.h"
#include "TGo4Slot.h"
#include "TGo4Proxy.h"
#include "TGo4AnalysisProxy.h"

#include "TGo4Condition.h"
#include "TGo4Picture.h"
#include "TGo4Interface.h"

TGo4BrowserItem::TGo4BrowserItem() :
   TFolder(),
   fParent(0),
   fIter(0),
   fItemClass(),
   fIsFolder(kFALSE),
   fBrowser(0)
{
}

TGo4BrowserItem::TGo4BrowserItem(const char* name, const char* title) :
   TFolder(name, title),
   fParent(0),
   fIter(0),
   fItemClass(),
   fIsFolder(kTRUE),
   fBrowser(0)
{
   SetOwner(kTRUE);
}

TGo4BrowserItem::TGo4BrowserItem(TGo4BrowserItem* parent, TGo4BrowserItem* previtem,
                               const char* name, const char* title) :
   TFolder(name, title),
   fParent(parent),
   fIter(0),
   fItemClass(),
   fIsFolder(kTRUE),
   fBrowser(0)
{
   SetOwner(kTRUE);
   if (parent!=0) {
      TList* list = (TList*) parent->GetListOfFolders();
      if (previtem==0) list->AddFirst(this);
                  else list->AddAfter(previtem, this);
   }
}

TGo4BrowserItem::~TGo4BrowserItem()
{
   if (fIter!=0) delete fIter;
}

void TGo4BrowserItem::SetBrowser(TGo4BrowserProxy* br, TGo4RootBrowserProxy* br2)
{
   fBrowser = br;
   fRootBrowser = br2;
}


TGo4BrowserItem* TGo4BrowserItem::firstChild()
{
    if (fIter!=0) delete fIter;
    fIter = GetListOfFolders()->MakeIterator();
    return (TGo4BrowserItem*) fIter->Next();
}

TGo4BrowserItem* TGo4BrowserItem::nextChild()
{
    if (fIter==0) return 0;
    TGo4BrowserItem* res = dynamic_cast<TGo4BrowserItem*> (fIter->Next());
    if (res==0) { delete fIter; fIter = 0; }
    return res;
}

void TGo4BrowserItem::deleteChild(TGo4BrowserItem* item)
{
   if (item==0) return;
   Remove(item);
   delete item;
}

void TGo4BrowserItem::deleteChilds()
{
   if (fIter!=0) { delete fIter; fIter = 0; }
   GetListOfFolders()->Delete();
}

void TGo4BrowserItem::ProduceFullName(TString& fullname)
{
   if (GetParent()!=0) {
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
   if ((itemname.Length()==0) || (fBrowser==0)) return;

   fBrowser->ProduceExplicitCopy(itemname.Data(), 0, kTRUE);
}

void TGo4BrowserItem::DeleteItem()
{
   if (GetParent()==0) {
      TGo4Interface::DeleteInstance();
      return;
   }


   TString itemname;
   ProduceFullName(itemname);
   if ((itemname.Length()==0) || (fBrowser==0)) return;

   TGo4Slot* itemslot = fBrowser->ItemSlot(itemname.Data());
   if (itemslot==0) return;

   TGo4Slot* memslot = fBrowser->BrowserMemorySlot();

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
   if ((itemname.Length()==0) || (fBrowser==0)) return;

   TGo4Slot* itemslot = fBrowser->ItemSlot(itemname.Data());
   if (itemslot==0) return;
   int kind = fBrowser->ItemKind(itemslot);

   if (kind==TGo4Access::kndFolder) {
      TGo4Iter iter(itemslot, kTRUE);
      while (iter.next()) {
         TGo4Slot* subslot = iter.getslot();
         if (fBrowser->ItemKind(subslot)==TGo4Access::kndObject)
            fBrowser->SetItemMonitored(subslot, on);
      }
  } else
      fBrowser->SetItemMonitored(itemslot, on);
}


void TGo4BrowserItem::ToggleMonitoring(Int_t sec)
{
   if (fBrowser!=0)
      fBrowser->ToggleMonitoring(sec*1000);
}

void TGo4BrowserItem::StartAnalysis()
{
   TGo4AnalysisProxy* anal = fBrowser->FindAnalysis();
   if (anal!=0)
      anal->StartAnalysis();
}

void TGo4BrowserItem::StopAnalysis()
{
   TGo4AnalysisProxy* anal = fBrowser->FindAnalysis();
   if (anal!=0)
      anal->StopAnalysis();
}



void TGo4BrowserItem::Delete(Option_t* option)
{
    DeleteItem();
}

void TGo4BrowserItem::SetName(const char* name)
{
   TFolder::SetName(name);
}

void TGo4BrowserItem::SetTitle(const char* title)
{
   TFolder::SetTitle(title);
}

void TGo4BrowserItem::ls(Option_t* option) const
{
   TFolder::ls(option);
}

#if ROOT_VERSION_CODE < ROOT_VERSION(5,13,6)
void TGo4BrowserItem::SaveAs(const char* filename)
{
   TFolder::SaveAs(filename);
}
#else
void TGo4BrowserItem::SaveAs(const char* filename, Option_t *option)
{
   TFolder::SaveAs(filename, option);
}
#endif

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

