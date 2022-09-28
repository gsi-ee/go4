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

#ifndef TGO4BROWSER_H
#define TGO4BROWSER_H

#include "QGo4Widget.h"
#include "QGo4BrowserTreeWidget.h"
#include "ui_TGo4Browser.h"

class TGo4BrowserProxy;

class TGo4Browser : public QGo4Widget, public Ui::TGo4Browser
{
   Q_OBJECT

   public:
      enum { NColumns = 7 };

      TGo4Browser(QWidget *parent = nullptr, const char *name = nullptr);

      void StartWorking();

      void linkedObjectUpdated(const char *linkname, TObject *obj) override;

      void ResetWidget() override;

      TGo4BrowserProxy* BrowserProxy();

      QString FullItemName(QTreeWidgetItem* item);

      QTreeWidgetItem* FindItemFor(TGo4Slot* slot);

      void SetViewItemProperties(TGo4Slot* itemslot, QTreeWidgetItem* item);

      void checkVisisbilityFlags(bool showall);

      void DisplaySelectedItems();

      void SuperImposeSelectedItems();

      bool canDrawItem(QTreeWidgetItem* item);

      void ExpandItem(const QString& itemname);

      void ExecuteItem(const QString& itemname);

      void ShootUpdateTimer();

      void SaveSelectedItems();

      void ExportSelectedItems(const char *filtername);

      void ExportSelectedItems(const char *filename, const char *filedir, const char *format, const char *description);

   public slots:

      void RequestDragObjectSlot(QDrag**);

      void ItemDropAcceptSlot(void* item, void* mime, bool* res);

      void ItemDropProcessSlot(void* item, void* e);

      void updateListViewItems();

      void ListView_doubleClicked(QTreeWidgetItem* item, int ncol);

      void ListView_customContextMenuRequested(const QPoint &);

      void Header_customContextMenuRequested(const QPoint &);

      void ColumnToggled(int indx);

      void HeaderSectionResizedSlot(int, int, int);

      void ContextMenuActivated(int id);

   protected:

      bool fbUpdateTimerActive{false};
      bool fVisibleColumns[NColumns] = { false, false, false, false, false, false, false };

};

#endif

