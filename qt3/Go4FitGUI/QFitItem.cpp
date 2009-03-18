#include "QFitItem.h"

#include "Riostream.h"

#include "qstring.h"

#include "TObject.h"
#include "TGo4FitPanel.h"

QFitItem::QFitItem(TGo4FitPanel* panel, QFitItem* parent, TObject* iObj, int iObjectType, int iWidgetType, int iPopupMenuType, int iGraphType, int iTag ) :
   QListViewItem (parent, QString::null, QString::null)
{
   fxPanel = panel;
   fxObj = iObj;
   fiObjectType = iObjectType;
   fiWidgetType = iWidgetType;
   fiPopupMenuType = iPopupMenuType;
   fiGraphType = iGraphType;
   fiTag = iTag;
   if (fxPanel) fxPanel->UpdateItem(this, FALSE);
}

QFitItem::QFitItem(TGo4FitPanel* panel, QListView * parent, TObject* iObj, int iObjectType, int iWidgetType, int iPopupMenuType, int iGraphType, int iTag) :
   QListViewItem (parent, QString::null, QString::null)
{
   fxPanel = panel;
   fxObj = iObj;
   fiObjectType = iObjectType;
   fiWidgetType = iWidgetType;
   fiPopupMenuType = iPopupMenuType;
   fiGraphType = iGraphType;
   fiTag = iTag;
   if (fxPanel) fxPanel->UpdateItem(this, FALSE);
}

QFitItem::~QFitItem()
{
   if (fxPanel) fxPanel->FitItemDeleted(this);
}

TObject* QFitItem::Object()
{
  return fxObj;
}

void QFitItem::SetObject(TObject* obj)
{
  fxObj = obj;
}

int QFitItem::ObjectType() {
  return fiObjectType;
}

int QFitItem::WidgetType() {
  return fiWidgetType;
}

int QFitItem::PopupMenuType() {
  return fiPopupMenuType;
}

int QFitItem::GraphType() {
  return fiGraphType;
}

int QFitItem::Tag() {
  return fiTag;
}

QFitItem* QFitItem::Parent() {
  return dynamic_cast<QFitItem*> (parent());
}

QFitItem* QFitItem::DefineWidgetItem() {
  QFitItem* item = this;

  while (item!=0) {
    if (item->WidgetType()!=-1) return item;
    item = item->Parent();
  }
  return 0;
}

QFitItem* QFitItem::DefineGraphItem() {
  QFitItem* item = this;

  while (item!=0) {
    if (item->GraphType()!=-1) return item;
    item = item->Parent();
  }
  return 0;
}

bool QFitItem::FindInParents(QFitItem* item) {
   QFitItem* i = this;
   i = i->Parent();

   while (i!=0) {
     if (i==item) return TRUE;
     i = i->Parent();
   }

  return FALSE;
}
