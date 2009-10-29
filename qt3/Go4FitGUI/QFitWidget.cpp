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

#include "QFitWidget.h"

#include "QFitItem.h"
#include "qpopupmenu.h"
#include "TObject.h"
#include "TGo4FitPanel.h"

QFitWidget::QFitWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if (name==0) setName( "QFitWidget" );
    resize( QSize(533, 405).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    fxPanel = 0;
    fxItem = 0;
    fbFillWidget = FALSE;
}

QFitWidget::~QFitWidget()
{
}

void QFitWidget::closeEvent( QCloseEvent *ce )
{
    ce->accept();
    delete this;
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
   if(GetItem()) return GetItem()->Object();
            else return 0;
}

TGo4Fitter* QFitWidget::GetFitter()
{
   return fxPanel==0 ? 0 : fxPanel->GetFitter();
}

void QFitWidget::FillWidget()
{
  fbFillWidget = TRUE;
  FillSpecificData();
  fbFillWidget = FALSE;
}

void QFitWidget::FillSpecificData()
{
}

void QFitWidget::contextMenuEvent(QContextMenuEvent* ev)
{
  if ((GetItem()==0) || (fxPanel==0)) return;

  QPopupMenu menu(0,"QFitWidgetPopup");

  if (fxPanel->FillPopupForItem(GetItem(),&menu)) {
     int id = menu.exec(ev->globalPos());
     fxPanel->ExecPopupForItem(GetItem(), id);
  }
}

void QFitWidget::UpdateWidgetItem(bool trace)
{
   if (fxPanel!=0)
     fxPanel->UpdateItem(GetItem(), trace);
}

void QFitWidget::SetWidgetItemText(bool trace)
{
   if (fxPanel!=0)
      fxPanel->SetItemText(GetItem(), trace);
}

void QFitWidget::UpdateItemsOfType(int typ, bool allitems)
{
   if (fxPanel!=0)
      fxPanel->UpdateItemsOfType(typ, allitems ? 0 : GetItem());
}
