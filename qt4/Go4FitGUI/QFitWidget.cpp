#include "QFitWidget.h"

#include <QContextMenuEvent>
#include <QCloseEvent>
#include <QtCore/QSignalMapper>
#include <QtGui/QMenu>

#include "QFitItem.h"

#include "TObject.h"
#include "TGo4FitPanel.h"

QFitWidget::QFitWidget( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{
    if (name==0) setName( "QFitWidget" );
    resize( QSize(533, 405).expandedTo(minimumSizeHint()) );
    //clearWState( Qt::WState_Polished );

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

  QSignalMapper map(this);
  connect(&map, SIGNAL(mapped(int)), fxPanel, SLOT(ItemMenuItemSelected(int)));

  QMenu menu(this);

  if (fxPanel->FillPopupForItem(GetItem(),&menu, &map)) {
	 fxPanel->CurrFitItem = GetItem();
     menu.exec(ev->globalPos());
     fxPanel->CurrFitItem = 0;
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
