#include "TGo4Browser.h"

#include <Q3ListView>
#include <Q3PopupMenu>
#include <Q3DragObject>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QApplication>
#include <QToolTip>

#include "Riostream.h"
#include "TClass.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TGo4LockGuard.h"
#include "TGo4Proxy.h"
#include "TGo4Slot.h"
#include "TGo4Iter.h"
#include "TGo4BrowserProxy.h"
#include "QGo4BrowserListView.h"
#include "QGo4BrowserItem.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4HServProxy.h"
#include "TGo4QSettings.h"
#include "TGo4ViewPanel.h"


const int NColumns = 7;
const int ColumnWidths[NColumns] = { 150, 40, 120, 90, 60, 100, 50 };
const char* ColumnNames[NColumns] = { "Name", "Flags", "Info", "Date", "Time", "Class", "Size" };
const int ColumnAllign[NColumns] = { Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignLeft, Qt::AlignRight };

TGo4Browser::TGo4Browser(QWidget *parent, const char* name)
         : QGo4Widget(parent,name)
{
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI

   setAcceptDrops(false);
   setCanDestroyWidget(FALSE);

   fbUpdateTimerActive = false;

   ListView->setSorting(-1);
   ListView->setSelectionMode(Q3ListView::Extended);
   ListView->setAcceptDrops(false);
   ListView->viewport()->setAcceptDrops(true);

   for(int indx=0;indx<NColumns;indx++) {
      fIndexes[indx] = indx;

      int width = -1;
      if ((indx==0) || (indx==2))
        width = ColumnWidths[indx];
      width = go4sett->getBrowserColumn(ColumnNames[indx], width);
      fVisibleColumns[indx] = width>0;
      if (fVisibleColumns[indx])
         ListView->addColumn(ColumnNames[indx], width);
   }

   // not in .ui file while designer brakes this connection
   connect(ListView, SIGNAL(RequestDragObject(Q3DragObject**)),
           this, SLOT(RequestDragObjectSlot(Q3DragObject**)));

   connect(ListView, SIGNAL(ItemDropAccept(void*, void*, bool*)),
           this, SLOT(ItemDropAcceptSlot(void*, void*, bool*)));

   connect(ListView, SIGNAL(ItemDropProcess(void*, void*)),
           this, SLOT(ItemDropProcessSlot(void*, void*)));

   connect(ListView->header(), SIGNAL(sizeChange(int, int, int)),
           this, SLOT(HeaderSizeChanged(int, int, int)));

   QToolTip::add(ListView->header(),
     QString("You can change selected browser columns\n") +
             "by activating RMB. Flags has following meaning\n" +
             "m - monitored,\ns - static,\n" +
             "d - can be deleted,\np - protected against delete\n" +
             "r - can not be reset (read only),\nw - can be reset");
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

void TGo4Browser::RequestDragObjectSlot(Q3DragObject** res)
{
   *res = 0;

   if (ListView->currentItem()==0) return;

   QString fullname = FullItemName(ListView->currentItem());

   *res = new Q3TextDrag(fullname, this);
}

void TGo4Browser::ItemDropAcceptSlot(void* item, void* mime, bool* res)
{
   *res = false;
   QString tgtname = FullItemName((Q3ListViewItem*) item);

   QString dropname;
   if (!Q3TextDrag::decode((const QMimeSource*)mime, dropname)) return;

   TGo4Slot* tgtslot = Browser()->ItemSlot(tgtname);
   TGo4Slot* dropslot = Browser()->ItemSlot(dropname);

   if ((tgtslot==0) || (dropslot==0)) return;
   if (Browser()->ItemKind(tgtslot)!=TGo4Access::kndFolder) return;

   if (dropslot->GetParent()==tgtslot) return;

   *res = true;
}

void TGo4Browser::ItemDropProcessSlot(void* item, void* e)
{
   QString tgtname = FullItemName((Q3ListViewItem*) item);
   QDropEvent* event = (QDropEvent*) e;
   if (!event->mimeData()->hasText()) return;
   Browser()->ProduceExplicitCopy(event->mimeData()->text(), tgtname, go4sett->getFetchDataWhenCopy());
}

void TGo4Browser::ResetWidget()
{
}

TGo4BrowserProxy* TGo4Browser::BrowserProxy()
{
   return (TGo4BrowserProxy*) GetLinked("Browser", 0);
}

QString TGo4Browser::FullItemName(Q3ListViewItem* item)
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

Q3ListViewItem* TGo4Browser::FindItemFor(TGo4Slot* slot)
{
   TGo4BrowserProxy* br = BrowserProxy();
   if (br==0) return 0;

   TString itemname;
   if (!br->BrowserItemName(slot, itemname)) return 0;
   const char* iname = itemname.Data();

   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it ) {
     QString fullname = FullItemName(it.current());
     if (strcmp(fullname.latin1(), iname)==0) return it.current();
   }
   return 0;
}

void TGo4Browser::SetViewItemProperties(TGo4Slot* itemslot, Q3ListViewItem* item)
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

   item->setDragEnabled(TGo4BrowserProxy::CanDragItem(cando));

   item->setDropEnabled(false);
   if (kind==TGo4Access::kndFolder) {
      TGo4Slot* memslot = br->BrowserMemorySlot();
      if (itemslot->IsParent(memslot) || (itemslot==memslot))
        item->setDropEnabled(true);
   }

   if (visible)
     item->setText(NColumns,"visible");
   else
     item->setText(NColumns,"hidden");

   // make visible all folders, where item is located
   if (visible && (br->GetItemFilter()>0)) {
      Q3ListViewItem* parent = item->parent();
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
   if (fIndexes[1]>0)
     item->setText(fIndexes[1], flags);
   if (fIndexes[2]>0)
     item->setText(fIndexes[2], iteminfo);
   if (fIndexes[3]>0)
     item->setText(fIndexes[3], br->ItemDate(itemslot));
   if (fIndexes[4]>0)
     item->setText(fIndexes[4], br->ItemTime(itemslot));
   if (fIndexes[5]>0)
     item->setText(fIndexes[5], classname);
   if (fIndexes[6]>0) {
     QString sizelbl = "";
     int sizeinfo = br->ItemSizeInfo(itemslot);
     int calcsize = br->GetCalcSize(itemslot);
     if (sizeinfo>0) sizelbl = QString::number(sizeinfo); else
     if (calcsize>0) sizelbl = QString("= ") + QString::number(calcsize);
     item->setText(fIndexes[6], sizelbl);
   }
}

void TGo4Browser::updateListViewItems()
{
   TGo4LockGuard lock;

   fbUpdateTimerActive = false;

   // first update list of visible columns
   int indx = 1;
   int ncolumn = 1;
   while (indx<NColumns) {
      if (fVisibleColumns[indx]) {
         while ((ncolumn<ListView->columns()) &&
                 ListView->header()->label(ncolumn) != QString(ColumnNames[indx]))
           ListView->removeColumn(ncolumn);
         if (ncolumn>=ListView->columns())
           ListView->addColumn(ColumnNames[indx], ColumnWidths[indx]);
         fIndexes[indx]=ncolumn;
         ListView->setColumnAlignment(ncolumn, ColumnAllign[indx]);
         ncolumn++;
      } else
         fIndexes[indx]=-1;

      indx++;
   }

   // remove unused columns
   while (ncolumn<ListView->columns())
      ListView->removeColumn(ncolumn);

   TGo4BrowserProxy* br = BrowserProxy();
   if (br==0) return;

   ServiceCall("UpdateGuiLayout");

   TGo4Slot* topslot = br->BrowserTopSlot();
   if (topslot==0) return;

   checkVisisbilityFlags(true);

//   slot->Print("");

   Q3ListViewItem* curfold = 0;
   Q3ListViewItem* curitem = ListView->firstChild();
   Q3ListViewItem* previtem = 0;

   TGo4Iter iter(topslot, kTRUE);

   TObjArray testedClasses;

   while (true) {
       Bool_t res = iter.next();

      // go to top folders and remove rest items
      Int_t levelchange = iter.levelchange();
      while (levelchange++<0) {

          while (curitem!=0) {
            Q3ListViewItem* next = curitem->nextSibling();
            delete curitem;
            curitem = next;
          }

          if (curfold==0) break;

          curitem = curfold->nextSibling();
          previtem = curfold;
          curfold = curfold->parent();
      }

      if (!res) break;

      // delete all slots in folder, which has another name
      while ((curitem!=0) && (strcmp(iter.getname(), curitem->text(0))!=0)) {
         Q3ListViewItem* next = curitem->nextSibling();
         delete curitem;
         curitem = next;
      }

      TGo4Slot* curslot = iter.getslot();
      if (curslot==0) {
         cerr << "************* ERROR in gui slots ****************** " << endl;
         return;
      }

      const char* classname = br->ItemClassName(curslot);
      Int_t itemkind = br->ItemKind(curslot);
      TClass* itemclass = 0;

      if ((classname!=0) && (testedClasses.FindObject(classname)==0)) {
         itemclass = gROOT->GetClass(classname);
         if (itemclass==0)
           testedClasses.Add(new TNamed(classname,""));
      }

      TString pixmap;
      int cando = TGo4BrowserProxy::DefineItemProperties(itemkind, itemclass, pixmap);
      TGo4BrowserProxy::SetItemCanDo(curslot, cando);

      if (curitem==0) {
        if (curfold==0)
          curitem = new QGo4BrowserItem(ListView, previtem, iter.getname());
        else
          curitem = new QGo4BrowserItem(curfold, previtem, iter.getname());
      }

      if ((pixmap.Length()>0) && (curitem->pixmap(0)==0))
        curitem->setPixmap(0, QPixmap(pixmap.Data()));

      SetViewItemProperties(curslot, curitem);

      if (iter.isfolder()) {
         curfold = curitem;
         curitem = curfold->firstChild();
         previtem = 0;
      } else {
         // remove any substructures if any
         while (curitem->firstChild()!=0)
           delete curitem->firstChild();
         previtem = curitem;
         curitem = curitem->nextSibling();
      }
   }

   while (curitem!=0) {
      Q3ListViewItem* next = curitem->nextSibling();
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
   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it ) {
      Q3ListViewItem* item = it.current();
     if (showall || (item->parent()==0))
        item->setVisible(true);
     else
        item->setVisible(item->text(NColumns)=="visible");
   }
}

void TGo4Browser::DisplaySelectedItems()
{

   TGo4BrowserProxy* br = BrowserProxy();

   int npads = 0;

   {
     Q3ListViewItemIterator it(ListView);
     for ( ; it.current(); ++it )
       if (it.current()->isSelected() &&
           canDrawItem(it.current())) npads++;
   }

   if (npads==0) return;

   TGo4ViewPanel* newpanel = CreateViewPanel(npads);
   TPad* subpad = 0;

   int cnt = 0;
   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it )
     if ( it.current()->isSelected() && canDrawItem(it.current())) {
        QString itemname = FullItemName(it.current());

        subpad = newpanel->GetSubPad(newpanel->GetCanvas(), cnt++, true);

        DrawItem(itemname.latin1(), newpanel, subpad, false);
     }
   newpanel->ShootRepaintTimer();
//   newpanel->SetActivePad(subpad);
}


void TGo4Browser::SuperImposeSelectedItems()
{
   TGo4ViewPanel* newpanel = 0;

   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it )
     if ( it.current()->isSelected() && canDrawItem(it.current())) {
        if (newpanel==0) {
           newpanel = CreateViewPanel();
           newpanel->SetPadSuperImpose(newpanel->GetCanvas(), true);
        }

        QString itemname = FullItemName(it.current());

        DrawItem(itemname.latin1(), newpanel, newpanel->GetCanvas(), false);
     }
   if (newpanel!=0)
     newpanel->ShootRepaintTimer();
}

void TGo4Browser::ListView_doubleClicked(Q3ListViewItem* item)
{
   if (item==0) return;

   QString fullname = FullItemName(item);

   TGo4BrowserProxy* br = BrowserProxy();

   int cando = br->ItemCanDo(fullname.latin1());
   TGo4Slot* itemslot = br->ItemSlot(fullname.latin1());

   if (TGo4BrowserProxy::CanDrawItem(cando))
     DrawItem(fullname.latin1(), 0, 0, true);
   else
   if (TGo4BrowserProxy::CanEditItem(cando))
      EditItem(fullname.latin1());
//   else
//      ShowItemInfo(fullname.latin1());

//   SetViewItemProperties(itemslot, item);
}

void TGo4Browser::ListView_contextMenuRequested(Q3ListViewItem* item, const QPoint& pos, int col)
{
   if (col!=0) {
      Q3PopupMenu contextMenu;
      for(int indx=1;indx<NColumns;indx++) {
         contextMenu.insertItem(ColumnNames[indx], indx);
         contextMenu.setItemChecked(indx, fVisibleColumns[indx]);
      }
      connect(&contextMenu, SIGNAL(activated(int)), this, SLOT(ColumnToggled(int)));

      contextMenu.exec(pos);
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

   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it )
      if (it.current()->isSelected()) {
         QString fullname = FullItemName(it.current());
         TGo4Slot* itemslot = br->ItemSlot(fullname.latin1());
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

   Q3PopupMenu contextMenu;
   contextMenu.insertItem(QPixmap(":/icons/chart.png"),
                          "Plot",  0, 0, 0, 11);
   contextMenu.setItemEnabled(11, (ndraw>0));

   contextMenu.insertItem(QPixmap(":/icons/superimpose.png"),
                          "Superimpose",  0, 0, 0, 12);
   contextMenu.setItemEnabled(12, (ndraw>1) && (nsuperimpose==ndraw));

   contextMenu.insertItem(QPixmap(":/icons/right.png"),
                          "Fetch item(s)",  0, 0, 0, 18);
   contextMenu.setItemEnabled(18, (nfolders>0) || (nobjects>0));

   contextMenu.insertItem(QPixmap(":/icons/saveall.png"),
                          "Save selected...",  0, 0, 0, 13);
   contextMenu.setItemEnabled(13, (nobjects>0) || ((nfolders==1) && (nitems==1)));

   // organize export submenu
   if (nexport>0) {

      Q3PopupMenu* expmenu = new Q3PopupMenu(&contextMenu, "Export menu");

      expmenu->insertItem("ASCII", 0, 0, 0, 141);
      expmenu->insertItem("Radware", 0, 0, 0, 142);

      contextMenu.insertItem(QPixmap(":/icons/export.png"),
                             "Export to", expmenu);

      connect(expmenu, SIGNAL(activated(int)), this, SLOT(ContextMenuActivated(int)));

   } else {
      contextMenu.insertItem(QPixmap(":/icons/export.png"),
                             "Export to",  0, 0, 0, 14);
      contextMenu.setItemEnabled(14, false);
   }

   contextMenu.insertItem(QPixmap(":/icons/info.png"),
                          "Info...",  0, 0, 0, 15);
   contextMenu.setItemEnabled(15, (ninfo>0));

   contextMenu.insertItem(QPixmap(":/icons/control.png"),
                           "Edit..", 0, 0, 0, 16);
   contextMenu.setItemEnabled(16, (nedits>0));

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

   contextMenu.insertItem(QPixmap(delbutton),
                          dellabel, 0, 0, 0, 17);
   contextMenu.setItemEnabled(17, (nclose>0) || (ndelete>0));

   contextMenu.insertItem(QPixmap(":/icons/copyws.png"),
                          "Copy to Workspace",  0, 0, 0, 19);
   contextMenu.setItemEnabled(19, (nobjects>0) || ((nfolders==1) && (nitems==1)));

   contextMenu.insertItem(QPixmap(":/icons/editcopy.png"),
                          "Copy to clipboard",  0, 0, 0, 20);
   contextMenu.setItemEnabled(20, (nobjects>0) || (nfolders>0));

   if ((nremote>0) || (nanalysis>0)) {

       contextMenu.insertSeparator();

       contextMenu.insertItem(QPixmap(":/icons/monitor.png"),
                              "Monitor item(s)",  0, 0, 0, 21);
       contextMenu.setItemEnabled(21, ((nobjects>0) && (nremote>0) && (nmonitor<nremote)) || ((nfolders==1) && (nitems==1)));

       contextMenu.insertItem(QPixmap(":/icons/Stop.png"),
                              "Stop item(s) monitoring",  0, 0, 0, 22);
       contextMenu.setItemEnabled(22, ((nobjects>0) && (nremote>0) && (nmonitor>0)) || ((nfolders==1) && (nitems==1)));

       contextMenu.insertItem(QPixmap( ":/icons/clear.png" ),
                              "Clear (Reset to 0)", 0, 0, 0, 23);
       contextMenu.setItemEnabled(23, (nclear>0));

       contextMenu.insertItem(QPixmap( ":/icons/clear_nok.png" ),
                              "Set Clear protection", 0, 0, 0, 24);
       contextMenu.setItemEnabled(24, (nclearprotoff>0));

       contextMenu.insertItem(QPixmap( ":/icons/clear_ok.png" ),
                              "Unset Clear protection", 0, 0, 0, 25);
       contextMenu.setItemEnabled(25, (nclearproton>0));

       contextMenu.insertItem(QPixmap( ":/icons/delete.png" ),
                              "Delete from analysis", 0, 0, 0, 26);
       contextMenu.setItemEnabled(26, (ndelprotoff>0));

       contextMenu.insertItem(QPixmap( ":/icons/refresh.png" ),
                              "Refresh namelist", 0, 0, 0, 27);
       contextMenu.setItemEnabled(27, true);
   }

   if ((nmemory>0) && (nmemory==nitems)) {
       contextMenu.insertSeparator();

       contextMenu.insertItem(QPixmap(":/icons/crefolder.png"),
                              "Create folder",  0, 0, 0, 41);
       contextMenu.setItemEnabled(41, (nmemory==1) && (nfolders==1));

       contextMenu.insertItem(QPixmap(":/icons/rename.png"),
                              "Rename object",  0, 0, 0, 42);
       contextMenu.setItemEnabled(42, (nmemory==1) && !istopmemory);

       contextMenu.insertItem(QPixmap(":/icons/editpaste.png"),
                              "Paste from clipboard",  0, 0, 0, 43);
       contextMenu.setItemEnabled(43, br->IsClipboard() && (nmemory==1) && (nfolders==1));
   }

   connect(&contextMenu, SIGNAL(activated(int)), this, SLOT(ContextMenuActivated(int)));

   contextMenu.exec(pos);
}

void TGo4Browser::ColumnToggled(int indx)
{
   if ((indx<=0) || (indx>=NColumns)) return;
   fVisibleColumns[indx] = !fVisibleColumns[indx];

   HeaderSizeChanged(0, 0, 0);

   ShootUpdateTimer();
}

void TGo4Browser::HeaderSizeChanged(int, int, int)
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

   Q3ListViewItemIterator it(ListView);
   for ( ; it.current(); ++it )
      if (it.current()->isSelected()) {
         QString itemname = FullItemName(it.current());
         TGo4Slot* itemslot = br->ItemSlot(itemname.latin1());
         if (itemslot==0) continue;
         int cando = br->ItemCanDo(itemslot);
         int kind = br->ItemKind(itemslot);

         switch(id) {
            case 15: {
               if (TGo4BrowserProxy::CanInfoItem(cando)) {
                  ShowItemInfo(itemname.latin1());
                  return;
               }
               break;
            }

            case 16: {
               if (TGo4BrowserProxy::CanEditItem(cando)) {
                  EditItem(itemname.latin1());
                  return;
               }
               break;
            }

            case 17: {
               br->DeleteDataSource(itemslot);
               break;
            }

            case 18: { // fetch item from the data source
                br->FetchItem(itemname.latin1());
                break;
            }

            case 19: {
               br->ProduceExplicitCopy(itemname.latin1(), 0, go4sett->getFetchDataWhenCopy());
               break;
            }

            case 20: {
               br->AddToClipboard(itemname.latin1());
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
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.latin1(), objname);
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
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.latin1(), objname);
               if (an!=0) {
                  an->ChageObjectProtection(objname.Data(), (id == 24 ? "+C" : "-C"));
                  anrefresh = an;
               }
               break;
            }

            case 26: {   // delete remote object
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.latin1(), objname);
               if (an!=0) {
                  an->RemoveObjectFromAnalysis(objname.Data(), br->ItemClass(itemslot));
                  anrefresh = an;
               }
               break;
            }

            case 27: {
               TString objname;
               TGo4AnalysisProxy* an = br->DefineAnalysisObject(itemname.latin1(), objname);
               TGo4HServProxy* hserv = br->DefineHServerProxy(itemname.latin1());
               if (an!=0) anrefresh = an;
               if (hserv!=0) hservrefresh = hserv;
               break;
            }

            case 41: { // create folder in memory
              bool ok = false;
              QString folder =
                QInputDialog::getText("Create folder in workspace",
                                      "Input folder name",
                                      QLineEdit::Normal,
                                      QString::null,
                                      &ok);
               if (ok) br->CreateMemorySubfolder(itemname.latin1(), folder.latin1());
               break;
            }

            case 42: {  // rename memory item
              bool ok = false;
              QString newname =
                QInputDialog::getText("Rename item in workspace",
                                      "Input new item name",
                                      QLineEdit::Normal,
                                      QString::null,
                                      &ok);
               if (ok) br->RenameMemoryItem(itemname.latin1(), newname.latin1());
               break;
            }

            case 43: {  // paste items from clipboard
               br->CopyClipboard(itemname.latin1(), go4sett->getFetchDataWhenCopy());
               br->ClearClipboard();
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

bool TGo4Browser::canDrawItem(Q3ListViewItem* item)
{
   if (item==0) return false;
   int cando = BrowserProxy()->ItemCanDo(FullItemName(item));
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
   fd.setMode( QFileDialog::AnyFile);

   if (fd.exec() != QDialog::Accepted) return;

   QString fname = fd.selectedFile();
   QString title;

   if (fd.selectedFilter() == "ROOT (*.root)") {
      bool ok = false;
      title = QInputDialog::getText(
         "Save slected objects to file", "Provide file title",
         QLineEdit::Normal, QString::null, &ok);
      if (!ok) return;
      if (fname.find(".root", 0, FALSE)<0) fname+=".root";
   } else {
      if (fname.find(".xml", 0, FALSE)<0) fname+=".xml";
   }

   ExportSelectedItems(fname.latin1(),
                       QFileInfo(fname).dirPath(true).latin1(),
                       fd.selectedFilter().latin1(),
                       title.latin1());
}

void TGo4Browser::ExportSelectedItems(const char* filtername)
{
   QFileDialog fd(this, QString("Select directory to export to ") + filtername);
   fd.setMode(QFileDialog::DirectoryOnly);

   if (fd.exec() != QDialog::Accepted) return;

   ExportSelectedItems("null",
                       fd.selectedFile().latin1(),
                       filtername,
                       "Export of selected items");
}

void TGo4Browser::ExportSelectedItems(const char* filename, const char* filedir, const char* format, const char* description)
{
    TObjArray items;
    Q3ListViewItemIterator it(ListView);
    for ( ; it.current(); ++it )
      if (it.current()->isSelected()) {
         QString fullname = FullItemName(it.current());
         items.Add(new TObjString(fullname.latin1()));
      }

    BrowserProxy()->ExportItemsTo(&items, go4sett->getFetchDataWhenSave(), filename, filedir, format, description);

    items.Delete();
}
