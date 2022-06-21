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

#include "QFitWidget.h"

#include <QContextMenuEvent>
#include <QtCore/QSignalMapper>
#include <QMenu>

#include "QFitItem.h"

#include "TGo4FitPanel.h"

QFitWidget::QFitWidget( QWidget* parent, const char* name, Qt::WindowFlags fl ) :
   QWidget( parent, fl )
{
   setObjectName(name ? name : "QFitWidget");
   resize( QSize(533, 405).expandedTo(minimumSizeHint()) );

   fxPanel = nullptr;
   fxItem = nullptr;
   fbFillWidget = false;
}

QFitWidget::~QFitWidget()
{
}

void QFitWidget::SetItem(TGo4FitPanel* panel, QFitItem * item)
{
   fxPanel = panel;
   fxItem = item;
   FillWidget();
}

QFitItem* QFitWidget::GetItem()
{
   return fxItem;
}

TObject * QFitWidget::GetObject()
{
   if(GetItem())
      return GetItem()->Object();
   return nullptr;
}

TGo4Fitter* QFitWidget::GetFitter()
{
   return !fxPanel ? nullptr : fxPanel->GetFitter();
}

void QFitWidget::FillWidget()
{
   fbFillWidget = true;
   FillSpecificData();
   fbFillWidget = false;
}

void QFitWidget::FillSpecificData()
{
}

void QFitWidget::contextMenuEvent(QContextMenuEvent* ev)
{
  if (!GetItem() || !fxPanel) return;

  QSignalMapper map(this);
  connect(&map, SIGNAL(mapped(int)), fxPanel, SLOT(ItemMenuItemSelected(int)));

  QMenu menu(this);

  if (fxPanel->FillPopupForItem(GetItem(),&menu, &map)) {
     fxPanel->CurrFitItem = GetItem();
     menu.exec(ev->globalPos());
     fxPanel->CurrFitItem = nullptr;
  }
}

void QFitWidget::UpdateWidgetItem(bool trace)
{
   if (fxPanel)
     fxPanel->UpdateItem(GetItem(), trace);
}

void QFitWidget::SetWidgetItemText(bool trace)
{
   if (fxPanel)
      fxPanel->SetItemText(GetItem(), trace);
}

void QFitWidget::UpdateItemsOfType(int typ, bool allitems)
{
   if (fxPanel)
      fxPanel->UpdateItemsOfType(typ, allitems ? nullptr : GetItem());
}
