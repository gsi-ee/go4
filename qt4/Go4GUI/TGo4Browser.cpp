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

#include "TGo4Browser.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QApplication>
#include <QToolTip>
#include <QMenu>
#include <QSignalMapper>
#include <QHeaderView>

#include "Riostream.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TGo4LockGuard.h"
#include "TGo4Proxy.h"
#include "TGo4Slot.h"
#include "TGo4Iter.h"
#include "TGo4BrowserProxy.h"
#include "QGo4BrowserTreeWidget.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4HServProxy.h"
#include "TGo4QSettings.h"
#include "TGo4ViewPanel.h"


const int NColumns = 7;
const int ColumnWidths[NColumns] = { 150, 40, 120, 90, 60, 100, 50 };
const char* ColumnNames[NColumns] = { "Name", "Flags", "Info", "Date", "Time", "Class", "Size" };
const int ColumnAllign[NColumns] = { Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignRight };

QTreeWidgetItem* nextSibling(QTreeWidgetItem* item)
{
   if (item==0) return 0;

   QTreeWidgetItem* prnt = item->parent();
   if (prnt==0) prnt = item->treeWidget()->invisibleRootItem();
   if (prnt==0) return 0;

   int indx = prnt->indexOfChild(item) + 1;
   if (indx >= prnt->childCount()) return 0;
   return prnt->child(indx);
}


TGo4Browser::TGo4Browser(QWidget *parent, const char* name)
         : QGo4Widget(parent,name)
{
   setupUi(this);

   setAcceptDrops(false);
   setCanDestroyWidget(FALSE);

   fbUpdateTimerActive = false;

   ListView->setSortingEnabled(false);
   ListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   ListView->setAcceptDrops(false);
   ListView->viewport()->setAcceptDrops(true);

   ListView->setRootIsDecorated(false);

   // ListView->setDragEnabled(true);

   for(int indx=0;indx<NColumns;indx++) {
      int width = -1;
      if ((indx==0) || (indx==2)) width = ColumnWidths[indx];
      width = go4sett->getBrowserColumn(ColumnNames[indx], width);
      fVisibleColumns[indx] = width>0;

      ListView->headerItem()->setText(indx, ColumnNames[indx]);

      ListView->header()->setSectionHidden(indx, ! fVisibleColumns[indx]);
      ListView->headerItem()->setTextAlignment(indx, ColumnAllign[indx]);

      ListView->header()->resizeSection(indx, width>0 ? width : ColumnWidths[indx]);
   }

   // not in .ui file while designer brakes this connection
   connect(ListView, SIGNAL(RequestDragObject(QDrag**)),
           this, SLOT(RequestDragObjectSlot(QDrag**)));

//   connect(ListView, SIGNAL(ItemDropAccept(void*, void*, bool*)),
//           this, SLOT(ItemDropAcceptSlot(void*, void*, bool*)));

   connect(ListView, SIGNAL(ItemDropProcess(void*, void*)),
           this, SLOT(ItemDropProcessSlot(void*, void*)));

   connect(ListView->header(), SIGNAL(sectionResized(int, int, int)),
           this, SLOT(HeaderSectionResizedSlot(int, int, int)));

   connect(ListView->header(), SIGNAL(customContextMenuRequested(const QPoint &)),
           this, SLOT(Header_customContextMenuRequested(const QPoint &)));

   ListView->header()->setToolTip(
     QString("You can change selected browser columns\n") +
             "by activating RMB. Flags has following meaning\n" +
             "m - monitored,\ns - static,\n" +
             "d - can be deleted,\np - protected against delete\n" +
             "r - can not be reset (read only),\nw - can be reset");

   ListView->setContextMenuPolicy(Qt::CustomContextMenu);
   ListView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
}


void TGo4Browser::StartWorking()
{
   AddLink("","Browser");
   ShootUpdateTimer();
}

void TGo4Browser::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname,"Browser")==0) {
      ShootUpdateTimer();
   } else

   if (strcmp(linkname,"TGo4Slot::evSubslotUpdated")==0) {
      TGo4Slot* itemslot = dynamic_cast<TGo4Slot*> (obj);
      if (itemslot!=0)
         SetViewItemProperties(itemslot, FindItemFor(itemslot));
   }
}


void TGo4Browser::RequestDragObjectSlot(QDrag** res)
{
   *res = 0;

   if (ListView->currentItem()==0) return;

   QString fullname = FullItemName(ListView->currentItem());

   *res = new QDrag(this);
   QMimeData *mimeData = new QMimeData;
   mimeData->setText(fullname);

   (*res)->setMimeData(mimeData);
}

void TGo4Browser::ItemDropAcceptSlot(void* item, void* d, bool* res)
{
   *res = false;
   QString tgtname = FullItemName((QTreeWidgetItem*) item);
   const QMimeData* mime = (const QMimeData*) d;
   if (!mime->hasText()) return;

   QString dropname = mime->text();

   TGo4Slot* tgtslot = Browser()->ItemSlot(tgtname.toAscii().constData());
   TGo4Slot* dropslot = Browser()->ItemSlot(dropname.toAscii().constData());

   if ((tgtslot==0) || (dropslot==0)) return;
   if (Browser()->ItemKind(tgtslot)!=TGo4Access::kndFolder) return;

   if (dropslot->GetParent()==tgtslot) return;

   *res = true;
}

void TGo4Browser::ItemDropProcessSlot(void* item, void* d)
{
   QString tgtname = FullItemName((QTreeWidgetItem*) item);
   const QMimeData* mime = (const QMimeData*) d;
   if (!mime->hasText()) return;
   Browser()->ProduceExplicitCopy(mime->text().toAscii().constData(), tgtname.toAscii().constData(), go4sett->getFetchDataWhenCopy());
}

void TGo4Browser::ResetWidget()
{
}

TGo4BrowserProxy* TGo4Browser::BrowserProxy()
{
   return (TGo4BrowserProxy*) GetLinked("Browser", 0);
}

QString TGo4Browser::FullItemName(QTreeWidgetItem* item)
{
   QString name;
   if (item!=0) {
      name = item->text(0);
      while (item->parent()!=0) {
         item = item->parent();
         name = item->text(0) + "/" + name;
      }
   }
   return name;
}

QTreeWidgetItem* TGo4Browser::FindItemFor(TGo4Slot* slot)
{
   TGo4BrowserProxy* br = BrowserProxy();
   if (br==0) return 0;

   TString itemname;
   if (!br->BrowserItemName(slot, itemname)) return 0;
   QString iname = itemname.Data();

   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it ) {
      QString fullname = FullItemName(*it);
      if (fullname == iname) return *it;
   }
   return 0;
}

void TGo4Browser::SetViewItemProperties(TGo4Slot* itemslot, QTreeWidgetItem* item)
{
   if ((itemslot==0) || (item==0)) return;

   TGo4BrowserProxy* br = BrowserProxy();

   bool mon = br->IsItemMonitored(itemslot);
   bool remote = br->IsItemRemote(itemslot);
   int kind = br->ItemKind(itemslot);
   int cando = br->ItemCanDo(itemslot);
   const char* iteminfo = TGo4BrowserProxy::ItemInfo(itemslot);
   const char* classname = TGo4BrowserProxy::ItemClassName(itemslot);

   bool visible = false;

   switch (br->GetItemFilter()) {
      case 1: visible = (itemslot->GetAssignedObject()!=0); break;
      case 2: visible = mon; break;
      default: visible = true;
   }

   if (TGo4BrowserProxy::CanDragItem(cando))
      item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
   else
      item->setFlags(item->flags() & ~Qt::ItemIsDragEnabled);

   item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);
   if (kind==TGo4Access::kndFolder) {
      TGo4Slot* memslot = br->BrowserMemorySlot();
      if (itemslot->IsParent(memslot) || (itemslot==memslot))
        item->setFlags(item->flags() | Qt::ItemIsDropEnabled);
   }

   if (visible)
     item->setText(NColumns,"visible");
   else
     item->setText(NColumns,"hidden");

   // make visible all folders, where item is located
   if (visible && (br->GetItemFilter()>0)) {
      QTreeWidgetItem* parent = item->parent();
      while (parent!=0) {
         parent->setText(NColumns, "visible");
         parent = parent->parent();
      }
   }

   QString flags;

//   if (kind == TGo4Access::kndObject)
//     if (itemslot->GetAssignedObject()!=0)
//       flags += "c";
//     else
//       flags += "a";

   if (remote && (kind == TGo4Access::kndObject)) {
      if (mon) flags+="m";
          else flags+="s";
      Int_t delprot, clearprot;
      br->GetProtectionBits(itemslot, delprot, clearprot);

      switch (delprot) {
         case 0: flags+="d"; break;
         case 1: flags+="p"; break;
         default: flags+="-"; break;
      }

      switch (clearprot) {
         case 0: flags+="w"; break;
         case 1: flags+="r"; break;
         default: flags+="-"; break;
      }
   }
   item->setText(1, flags);
   item->setText(2, iteminfo);
   item->setText(3, br->ItemDate(itemslot));
   item->setText(4, br->ItemTime(itemslot));
   item->setText(5, classname);
   QString sizelbl = "";
   int sizeinfo = br->ItemSizeInfo(itemslot);
   int calcsize = br->GetCalcSize(itemslot);
   if (sizeinfo>0) sizelbl = QString::number(sizeinfo); else
   if (calcsize>0) sizelbl = QString("= ") + QString::number(calcsize);
   item->setText(6, sizelbl);
}

void TGo4Browser::updateListViewItems()
{
   TGo4LockGuard lock(0,true);

   fbUpdateTimerActive = false;

   // first update list of visible columns

   for(int indx=0;indx<NColumns;indx++)
      ListView->header()->setSectionHidden(indx, !fVisibleColumns[indx]);

   TGo4BrowserProxy* br = BrowserProxy();
   if (br==0) return;

   ServiceCall("UpdateGuiLayout");

   TGo4Slot* topslot = br->BrowserTopSlot();
   if (topslot==0) return;

   checkVisisbilityFlags(true);

//   slot->Print("");

   QTreeWidgetItem* curfold = 0;
   QTreeWidgetItem* curitem = ListView->topLevelItem(0);
   QTreeWidgetItem* previtem = 0;

   TGo4Iter iter(topslot, kTRUE);

   TObjArray testedClasses;

   while (true) {
       Bool_t res = iter.next();

      // go to top folders and remove rest items
      Int_t levelchange = iter.levelchange();
      while (levelchange++<0) {

          while (curitem!=0) {
            QTreeWidgetItem* next = nextSibling(curitem);
            delete curitem;
            curitem = next;
          }

          if (curfold==0) break;

          curitem = nextSibling(curfold);
          previtem = curfold;
          curfold = curfold->parent();
      }

      if (!res) break;

      // delete all slots in folder, which has another name
      while ((curitem!=0) && (strcmp(iter.getname(), curitem->text(0).toAscii().constData())!=0)) {
         QTreeWidgetItem* next = nextSibling(curitem);
         delete curitem;
         curitem = next;
      }

      TGo4Slot* curslot = iter.getslot();
      if (curslot==0) {
         std::cerr << "************* ERROR in gui slots ****************** " << std::endl;
         return;
      }

      const char* classname = br->ItemClassName(curslot);
      Int_t itemkind = br->ItemKind(curslot);
      TClass* itemclass = 0;

      if ((classname!=0) && (strlen(classname)>0) && (testedClasses.FindObject(classname)==0)) {

         itemclass = gROOT->GetClass(classname, kFALSE);

         // if dictionary existing (library is loaded) force creation of TClass object
         if ((itemclass==0) && TClassTable::GetDict(classname))
            itemclass = gROOT->LoadClass(classname);

         if (itemclass==0)
           testedClasses.Add(new TNamed(classname,""));
      }

      TString pixmap;
      int cando = TGo4BrowserProxy::DefineItemProperties(itemkind, itemclass, pixmap);
      if (pixmap.Length()>0) pixmap = TString(":/icons/") + pixmap;
      TGo4BrowserProxy::SetItemCanDo(curslot, cando);

      if (curitem==0) {
        if (curfold==0)
          curitem = new QTreeWidgetItem(ListView, previtem);
        else
          curitem = new QTreeWidgetItem(curfold, previtem);
      }

      curitem->setText(0, iter.getname());

      if ((pixmap.Length()>0) && curitem->icon(0).isNull())
        curitem->setIcon(0, QIcon(pixmap.Data()));

      if ((curitem->text(0).contains(".TGo4EventElement") && (itemkind==TGo4Access::kndTreeBranch)) ||
          ((curitem->text(0)=="TGo4EventElement") && (itemkind==TGo4Access::kndFolder))) {
         curitem->setHidden(go4sett->getHideTGo4EventElement());
      }

      SetViewItemProperties(curslot, curitem);

      if (iter.isfolder()) {
         curfold = curitem;
         curitem = curfold->child(0);
         previtem = 0;
      } else {
         // remove any substructures if any
         while (curitem->child(0)!=0)
           delete curitem->child(0);
         previtem = curitem;
         curitem = nextSibling(curitem);
      }
   }

   while (curitem!=0) {
      QTreeWidgetItem* next = nextSibling(curitem);
      delete curitem;
      curitem = next;
   }

   if (br->GetItemFilter()>0)
     checkVisisbilityFlags(false);

   ListView->update();
   ListView->viewport()->update();

   testedClasses.Delete();
}

void TGo4Browser::checkVisisbilityFlags(bool showall)
{
   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it ) {
      QTreeWidgetItem* item = *it;
      if (showall || (item->parent()==0))
        item->setHidden(false);
      else
        item->setHidden(item->text(NColumns)!="visible");
   }
}

void TGo4Browser::DisplaySelectedItems()
{

   TGo4BrowserProxy* br = BrowserProxy();

   int npads = 0;

   {
      QTreeWidgetItemIterator it(ListView);
      for ( ; *it; ++it )
       if ((*it)->isSelected() &&
           canDrawItem(*it)) npads++;
   }

   if (npads==0) return;

   TGo4ViewPanel* newpanel = CreateViewPanel(npads);
   TPad* subpad = 0;

   int cnt = 0;
   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it )
     if ( (*it)->isSelected() && canDrawItem(*it)) {
        QString itemname = FullItemName(*it);

        subpad = newpanel->GetSubPad(newpanel->GetCanvas(), cnt++, true);

        DrawItem(itemname, newpanel, subpad, false);
     }
   newpanel->ShootRepaintTimer();
//   newpanel->SetActivePad(subpad);
}


void TGo4Browser::SuperImposeSelectedItems()
{
   TGo4ViewPanel* newpanel = 0;

   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it )
     if ( (*it)->isSelected() && canDrawItem(*it)) {
        if (newpanel==0) {
           newpanel = CreateViewPanel();
           newpanel->SetPadSuperImpose(newpanel->GetCanvas(), true);
        }

        QString itemname = FullItemName(*it);

        DrawItem(itemname, newpanel, newpanel->GetCanvas(), false);
     }
   if (newpanel!=0)
     newpanel->ShootRepaintTimer();
}

void TGo4Browser::ListView_doubleClicked(QTreeWidgetItem* item, int ncol)
{
   if (item==0) return;

   QString fullname = FullItemName(item);

   TGo4BrowserProxy* br = BrowserProxy();

   int cando = br->ItemCanDo(fullname.toAscii().constData());
   TGo4Slot* itemslot = br->ItemSlot(fullname.toAscii().constData());

   if (go4sett->getDrawOnceFlag()) {
      TGo4ViewPanel* panel = WhereItemDrawn(fullname.toAscii().constData());
      if (panel!=0)  {
         if (panel->isMinimized()) panel->showNormal();
         panel->activateWindow();
         panel->raise();
         panel->show();
         panel->setFocus();
         return;
      }
   }

   if (TGo4BrowserProxy::CanDrawItem(cando))
     DrawItem(fullname, 0, 0, true);
   else
   if (TGo4BrowserProxy::CanEditItem(cando))
      EditItem(fullname);
//   else
//      ShowItemInfo(fullname);

//   SetViewItemProperties(itemslot, item);
}

void TGo4Browser::Header_customContextMenuRequested(const QPoint & pos)
{
   QMenu menu;
   QSignalMapper map;

   for(int indx=1;indx<NColumns;indx++)
       AddIdAction(&menu, &map,
                  ColumnNames[indx], indx, true, fVisibleColumns[indx]);
   connect(&map, SIGNAL(mapped(int)), this, SLOT(ColumnToggled(int)));

   menu.exec(ListView->header()->mapToGlobal(pos));
}


void TGo4Browser::ListView_customContextMenuRequested(const QPoint& pos)
{
   QTreeWidgetItem* item = ListView->itemAt(pos);

   int col = ListView->header()->logicalIndexAt(pos);

   QMenu menu;
   QSignalMapper map;

   if (col!=0) {
      for(int indx=1;indx<NColumns;indx++)
        AddIdAction(&menu, &map,
                  ColumnNames[indx], indx, true, fVisibleColumns[indx]);
      connect(&map, SIGNAL(mapped(int)), this, SLOT(ColumnToggled(int)));

      menu.exec(ListView->viewport()->mapToGlobal(pos));
      return;
   }

   TGo4BrowserProxy* br = BrowserProxy();
   TGo4Slot* memslot = br->BrowserMemorySlot();
   TGo4Slot* analslot = br->FindAnalysisSlot(false);

   int nitems = 0;
   int nmemory = 0;
   bool istopmemory = false;
   int nclose = 0;
   int ndraw = 0;
   int nsuperimpose = 0;
   int si_kind = -1;
   int nremote = 0;
   int nanalysis = 0;
   int nmonitor = 0;
   int nclear = 0;
   int nclearlocal = 0;
   int nclearproton = 0;
   int nclearprotoff = 0;
   int ndelprotoff = 0;
   int nobjects = 0;
   int nfolders = 0;
   int nedits = 0;
   int ninfo = 0;
   int nexport = 0;
   int ndelete = 0;
   int nassigned = 0;

   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it )
      if ((*it)->isSelected()) {
         QString fullname = FullItemName(*it);
         TGo4Slot* itemslot = br->ItemSlot(fullname.toAscii().constData());
         if (itemslot==0) continue;
         nitems++;

         int cando = br->ItemCanDo(itemslot);
         int kind = br->ItemKind(itemslot);
         const char* itemclassname = br->ItemClassName(itemslot);

         bool ismemitem = itemslot->IsParent(memslot);

         if (br->IsCanDelete(itemslot) || ismemitem)
           ndelete++;

         if (kind==TGo4Access::kndObject)
            nobjects++;

         if (kind==TGo4Access::kndFolder)
           nfolders++;

         if (TGo4BrowserProxy::CanDrawItem(cando)) {
           ndraw++;
           TClass* cl = gROOT->GetClass(br->ItemClassName(itemslot));
           if (cl!=0)
              if (cl->InheritsFrom("TH1")) {
                 if (!cl->InheritsFrom("TH2") && !cl->InheritsFrom("TH3")) {
                    if ((si_kind<0) || (si_kind==1)) {
                       si_kind=1;
                       nsuperimpose++;
                    }
                 }
              } else
              if (cl->InheritsFrom("TGraph")) {
                if ((si_kind<0) || (si_kind==2)) {
                   si_kind=2;
                   nsuperimpose++;
                }
              }
         }

         if (TGo4BrowserProxy::CanEditItem(cando))
           nedits++;

         if (TGo4BrowserProxy::CanCloseItem(cando))
           nclose++;

         if (TGo4BrowserProxy::CanInfoItem(cando))
           ninfo++;

         istopmemory = (itemslot==memslot);

         if (ismemitem || istopmemory)
           nmemory++;

         if (ismemitem && TGo4BrowserProxy::CanClearItem(cando))
            nclearlocal++;

         if ((kind==TGo4Access::kndObject) && (itemslot->GetAssignedObject()!=0)) {
            nassigned++;
            if (TGo4BrowserProxy::CanExportItem(cando))
              nexport++;
         }

         bool isanalysisitem = (itemslot==analslot) || itemslot->IsParent(analslot);

         if (isanalysisitem)
           nanalysis++;

         if (br->IsItemRemote(itemslot)) {
            nremote++;

            if (isanalysisitem) {

               if (kind==TGo4Access::kndObject) {
                  Int_t delprot, clearprot;
                  br->GetProtectionBits(itemslot, delprot, clearprot);
                  if (clearprot==1) nclearproton++; else
                  if (clearprot==0) nclearprotoff++;
                  if (delprot==0) ndelprotoff++;
               }

               if (TGo4BrowserProxy::CanClearItem(cando) ||
                   (kind==TGo4Access::kndFolder))
                 nclear++;
            }
         }

         if (br->IsItemMonitored(itemslot)) nmonitor++;
      }

   AddIdAction(&menu, &map, QIcon(":/icons/chart.png"),
                 "Plot",  11, (ndraw>0));

   AddIdAction(&menu, &map, QIcon(":/icons/superimpose.png"),
                 "Superimpose",  12, (ndraw>1) && (nsuperimpose==ndraw));

   AddIdAction(&menu, &map, QIcon(":/icons/right.png"),
                  "Fetch item(s)",  18, (nfolders>0) || (nobjects>0));

   AddIdAction(&menu, &map, QIcon(":/icons/saveall.png"),
                   "Save selected...",  13, (nobjects>0) || ((nfolders==1) && (nitems==1)));

   // organize export submenu
   if (nexport>0) {

      QMenu* expmenu = menu.addMenu(QIcon(":/icons/export.png"), "Export to");

      AddIdAction(expmenu, &map, "ASCII", 141);
      AddIdAction(expmenu, &map, "Radware", 142);

   } else {
      AddIdAction(&menu, &map, QIcon(":/icons/export.png"),
                     "Export to",  14, false);
   }

   AddIdAction(&menu, &map, QIcon(":/icons/info.png"),
                 "Info...",  15, (ninfo>0));

   AddIdAction(&menu, &map, QIcon(":/icons/control.png"),
                 "Edit..", 16, (nedits>0));

   QString dellabel = "Delete item";
   QString delbutton = ":/icons/delete.png";
   if ((nclose>0) && (ndelete==0)) {
     dellabel = "Close item";
     if (nclose>1) dellabel+="s";
     delbutton=":/icons/close.png";
   } else
   if ((nclose==0) && (ndelete>0)) {
     dellabel = "Delete item";
     if (ndelete>1) dellabel+="s";
   } else
   if ((nclose>0) && (ndelete>0)) {
     dellabel = "Close/delete items";
   }

   AddIdAction(&menu, &map, QIcon(delbutton),
                 dellabel, 17, (nclose>0) || (ndelete>0));

   AddIdAction(&menu, &map, QIcon(":/icons/copyws.png"),
                 "Copy to Workspace",  19, (nobjects>0) || ((nfolders==1) && (nitems==1)));

   AddIdAction(&menu, &map, QIcon(":/icons/editcopy.png"),
                  "Copy to clipboard",  20, (nobjects>0) || (nfolders>0));

   if ((nremote>0) || (nanalysis>0)) {

       menu.addSeparator();

       AddIdAction(&menu, &map, QIcon(":/icons/monitor.png"),
                     "Monitor item(s)",  21, ((nobjects>0) && (nremote>0) && (nmonitor<nremote)) || ((nfolders==1) && (nitems==1)));

       AddIdAction(&menu, &map, QIcon(":/icons/Stop.png"),
                     "Stop item(s) monitoring", 22, ((nobjects>0) && (nremote>0) && (nmonitor>0)) || ((nfolders==1) && (nitems==1)));

       AddIdAction(&menu, &map, QIcon( ":/icons/clear.png" ),
                     "Clear (Reset to 0)", 23, (nclear>0));

       AddIdAction(&menu, &map, QIcon( ":/icons/clear_nok.png" ),
                     "Set Clear protection", 24, (nclearprotoff>0));

       AddIdAction(&menu, &map, QIcon( ":/icons/clear_ok.png" ),
                     "Unset Clear protection", 25, (nclearproton>0));

       AddIdAction(&menu, &map, QIcon( ":/icons/delete.png" ),
                     "Delete from analysis", 26, (ndelprotoff>0));

       AddIdAction(&menu, &map, QIcon( ":/icons/refresh.png" ),
                     "Refresh namelist", 27, true);
   }

   if ((nmemory>0) && (nmemory==nitems)) {
       menu.addSeparator();

       AddIdAction(&menu, &map, QIcon(":/icons/crefolder.png"),
                    "Create folder",  41, (nmemory==1) && (nfolders==1));

       AddIdAction(&menu, &map, QIcon(":/icons/rename.png"),
                    "Rename object",  42, (nmemory==1) && !istopmemory);

       AddIdAction(&menu, &map, QIcon(":/icons/clear.png"),
                    "Clear object(s)", 44, (nclearlocal>0));

       AddIdAction(&menu, &map, QIcon(":/icons/editpaste.png"),
                    "Paste from clipboard",  43, br->IsClipboard() && (nmemory==1) && (nfolders==1));
   }

   connect(&map, SIGNAL(mapped(int)), this, SLOT(ContextMenuActivated(int)));

   menu.exec(ListView->viewport()->mapToGlobal(pos));
}

void TGo4Browser::ColumnToggled(int indx)
{
   if ((indx<=0) || (indx>=NColumns)) return;
   fVisibleColumns[indx] = !fVisibleColumns[indx];

   HeaderSectionResizedSlot(0, 0, 0);

   ShootUpdateTimer();
}

void TGo4Browser::HeaderSectionResizedSlot(int, int, int)
{
   int ncolumn = 0;
   for(int indx=0;indx<NColumns;indx++) {
      int width = -1;
      if (fVisibleColumns[indx]) {
         width = ListView->columnWidth(ncolumn++);
         if (width==0) width = ColumnWidths[indx];
      }
      go4sett->setBrowserColumn(ColumnNames[indx], width);
   }
}

void TGo4Browser::ContextMenuActivated(int id)
{
   switch (id) {
      case 11: DisplaySelectedItems(); return;
      case 12: SuperImposeSelectedItems(); return;
      case 13: SaveSelectedItems(); return;
      case 141: ExportSelectedItems("ASCII format"); return;
      case 142: ExportSelectedItems("Radware format"); return;
   }

   TGo4BrowserProxy* br = BrowserProxy();

   TGo4AnalysisProxy* anrefresh = 0;
   TGo4HServProxy* hservrefresh = 0;

   if (id==20) br->ClearClipboard();

   if (id==19)
     QApplication::setOverrideCursor(Qt::WaitCursor);

   QTreeWidgetItemIterator it(ListView);
   for ( ; *it; ++it )
      if ((*it)->isSelected()) {
         QString itemname = FullItemName(*it);
         TGo4Slot* itemslot = br->ItemSlot(itemname.toAscii().constData());
         if (itemslot==0) continue;
         int cando = br->ItemCanDo(itemslot);
         int kind = br->ItemKind(itemslot);

         switch(id) {
            case 15: {
               if (TGo4BrowserProxy::CanInfoItem(cando)) {
                  ShowItemInfo(itemname);
                  return;
               }
               break;
            }

            case 16: {
               if (TGo4BrowserProxy::CanEditItem(cando)) {
                  EditItem(itemname);
                  return;
               }
               break;
            }

            case 17: {
               br->DeleteDataSource(itemslot);
               break;
            }

            case 18: { // fetch item from the data source
                br->FetchItem(itemname.toAscii().constData());
                break;
            }

            case 19: {
               br->ProduceExplicitCopy(itemname.toAscii().constData(), 0, go4sett->getFetchDataWhenCopy());
               break;
            }

            case 20: {
               br->AddToClipboard(itemname.toAscii().constData());
               break;
            }

            case 21:    // toggle monitor on
            case 22: {  // toggle monitor off
               br->SetItemMonitored(itemslot, id==21);
               ShootUpdateTimer();
               break;
            }

            case 23: {  // clear
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.toAscii().constData(), objname);
               if (an!=0) {
                  an->ClearAnalysisObject(objname.Data());
                  // if clear folder, request all objects which were requested before
                  if (kind==TGo4Access::kndFolder) {
                     TGo4Iter iter(itemslot, kTRUE);
                     while (iter.next()) {
                        TGo4Slot* subslot = iter.getslot();
                        if (subslot->GetAssignedObject()!=0)
                           subslot->Update(kFALSE);
                     }
                  } else
                    if (itemslot->GetAssignedObject()!=0)
                       itemslot->Update(kFALSE);
               }
               break;
            }

            case 24:     // set clear protect
            case 25: {   // unset clear protect
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.toAscii().constData(), objname);
               if (an!=0) {
                  an->ChageObjectProtection(objname.Data(), (id == 24 ? "+C" : "-C"));
                  anrefresh = an;
               }
               break;
            }

            case 26: {   // delete remote object
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.toAscii().constData(), objname);
               if (an!=0) {
                  an->RemoveObjectFromAnalysis(objname.Data(), br->ItemClass(itemslot));
                  anrefresh = an;
               }
               break;
            }

            case 27: {
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.toAscii().constData(), objname);
               TGo4HServProxy* hserv = br->DefineHServerProxy(itemname.toAscii().constData());
               if (an!=0) anrefresh = an;
               if (hserv!=0) hservrefresh = hserv;
               break;
            }

            case 41: { // create folder in memory
              bool ok = false;
              QString folder =
                QInputDialog::getText(this,
                                      "Create folder in workspace",
                                      "Input folder name",
                                      QLineEdit::Normal,
                                      QString::null,
                                      &ok);
               if (ok) br->CreateMemorySubfolder(itemname.toAscii().constData(), folder.toAscii().constData());
               break;
            }

            case 42: {  // rename memory item
              bool ok = false;
              QString newname =
                QInputDialog::getText(this,
                                      "Rename item in workspace",
                                      "Input new item name",
                                      QLineEdit::Normal,
                                      QString::null,
                                      &ok);
               if (ok) br->RenameMemoryItem(itemname.toAscii().constData(), newname.toAscii().constData());
               break;
            }

            case 43: {  // paste items from clipboard
               br->CopyClipboard(itemname.toAscii().constData(), go4sett->getFetchDataWhenCopy());
               br->ClearClipboard();
               break;
            }

            case 44: { // clear memory item
               br->ClearMemoryItem(itemname.toAscii().constData());
               break;
            }
         }
      }

   if (anrefresh!=0)
      anrefresh->RefreshNamesList();

   if (hservrefresh!=0)
      hservrefresh->RequestHistosList();

   if (id==19)
     QApplication::restoreOverrideCursor();
}

bool TGo4Browser::canDrawItem(QTreeWidgetItem* item)
{
   if (item==0) return false;
   int cando = BrowserProxy()->ItemCanDo(FullItemName(item).toAscii().constData());
   return TGo4BrowserProxy::CanDrawItem(cando);
}

void TGo4Browser::ShootUpdateTimer()
{
   if (fbUpdateTimerActive) return;

   fbUpdateTimerActive = true;

   QTimer::singleShot(1, this, SLOT(updateListViewItems()));
}

void TGo4Browser::SaveSelectedItems()
{
   QFileDialog fd(this, "Save selected objects to file", QString(),
                  "ROOT (*.root);;ROOT XML (*.xml)");
   fd.setFileMode( QFileDialog::AnyFile);
   fd.setAcceptMode(QFileDialog::AcceptSave);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fname = flst[0];
   QString title;

   if (fd.selectedNameFilter() == "ROOT (*.root)") {
      bool ok = false;
      title = QInputDialog::getText(this,
         "Save slected objects to file", "Provide file title",
         QLineEdit::Normal, QString::null, &ok);
      if (!ok) return;
      if (fname.indexOf(".root", 0, Qt::CaseInsensitive)<0) fname+=".root";
   } else {
      if (fname.indexOf(".xml", 0, Qt::CaseInsensitive)<0) fname+=".xml";
   }

   ExportSelectedItems(fname.toAscii().constData(),
                       QFileInfo(fname).absolutePath().toAscii().constData(),
                       fd.selectedNameFilter().toAscii().constData(),
                       title.toAscii().constData());
}

void TGo4Browser::ExportSelectedItems(const char* filtername)
{
   QFileDialog fd(this, QString("Select directory to export to ") + filtername);
   fd.setFileMode(QFileDialog::DirectoryOnly);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   ExportSelectedItems("null",
                       flst[0].toAscii().constData(),
                       filtername,
                       "Export of selected items");
}

void TGo4Browser::ExportSelectedItems(const char* filename, const char* filedir, const char* format, const char* description)
{
   TObjArray items;
     QTreeWidgetItemIterator it(ListView);
     for ( ; *it; ++it )
        if ((*it)->isSelected()) {
           QString fullname = FullItemName(*it);
           items.Add(new TObjString(fullname.toAscii().constData()));
        }

     BrowserProxy()->ExportItemsTo(&items, go4sett->getFetchDataWhenSave(), filename, filedir, format, description);

     items.Delete();
}
