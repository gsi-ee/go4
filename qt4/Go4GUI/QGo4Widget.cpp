#include "QGo4Widget.h"
#include <QtCore/qtimer.h>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtCore/QSignalMapper>

#include <QDragMoveEvent>
#include <QCloseEvent>
#include <QDropEvent>
#include <QDragEnterEvent>


QGo4Widget::QGo4Widget(QWidget * parent, const char * name, Qt::WFlags f) :
   QWidget(parent, f),
   fWaitsForObjectCreation(false)
{
   setObjectName(name);
   setAcceptDrops(TRUE);
   fCanDestroyWidget = true;
   fBrowserProxy = 0;
   fResetWidgetShooted = false;
   fBlockUpdate = false;
}

QGo4Widget::~QGo4Widget()
{
   RemoveLinksMasked("complete");
}

void QGo4Widget::ObjectCreatedByWidget(const char* itemname, TClass* cl)
// this method calls by maingui when object created by one of editor
{
   if (fWaitsForObjectCreation) {
      fWaitsForObjectCreation = false;
      requestedObjectCreated(itemname, cl);
   }
}

void QGo4Widget::ResetWidget()
{
   // we set fResetWidgetShooted to prevent in any situation of ShootResetWidget() again
   fResetWidgetShooted = true;
   RemoveAllLinks();
   fWaitsForObjectCreation = false;
   fResetWidgetShooted = false;
}

void QGo4Widget::ShootResetWidget()
{
   if (fResetWidgetShooted) return;

   fResetWidgetShooted = true;

   QTimer::singleShot(0, this, SLOT(ResetWidgetSlot()));
}

void QGo4Widget::ShootCloseWidget()
{
   if (fResetWidgetShooted) return;

   fResetWidgetShooted = true;

   QTimer::singleShot(0, this, SLOT(close()));
}


void QGo4Widget::ResetWidgetSlot()
{
   ResetWidget();
}


bool QGo4Widget::IsAcceptDrag(const char*, TClass*, int)
{
   return false;
}

void QGo4Widget::DropItem(const char*, TClass*, int)
{
}


void QGo4Widget::closeEvent(QCloseEvent* e)
{
   if (fCanDestroyWidget) {
      e->accept();
      delete this;
   } else
      QWidget::closeEvent(e);
}

void QGo4Widget::dragEnterEvent(QDragEnterEvent *e)
{
   emit widgetService(this, service_DragEnter, 0, e);
}

void QGo4Widget::dragMoveEvent(QDragMoveEvent*)
{
//   emit widgetService(this, service_DragMove, "", e);
}

#include "Riostream.h"

void QGo4Widget::dropEvent(QDropEvent* e)
{
	cout << "Drop event " << endl;

   emit widgetService(this, service_DropEvent, 0, e);
}

void QGo4Widget::AddLink(const char* itemname, const char* linkname)
{
   emit widgetService(this, service_AddEditorLink, linkname, (void*) itemname);
}

void QGo4Widget::AddLink(TGo4Slot* slot, const char* linkname)
{
   emit widgetService(this, service_AddDirectLink, linkname, (void*) slot);
}

TGo4Slot* QGo4Widget::AddLink(const char* itemname, TGo4Slot* parent)
{
   TGo4Slot* res = parent;
   emit widgetService(this, service_AddLinkInSlot, itemname, (void*) &res);
   return res;
}

TGo4Slot* QGo4Widget::GetTopSlot(bool force)
{
   TGo4Slot* res = 0;
   emit widgetService(this, service_GetTopSlot, force ? "force" : "normal", (void*) &res);
   return res;
}

TGo4Slot* QGo4Widget::AddSlot(const char* slotname)
{
   TGo4Slot* res =  0;
   emit widgetService(this, service_AddEditorSlot, slotname, (void*) &res);
   return res;
}

void QGo4Widget::SetLinkedName(TGo4Slot* slot, const char* itemname)
{
   emit widgetService(this, service_SetLinkedName, itemname, (void*) slot);
}

const char* QGo4Widget::GetLinkedName(const char* linkname)
{
   const char* res = 0;

   emit widgetService(this, service_GetLinkedName, linkname, (void*) &res);

   return res;
}

const char* QGo4Widget::GetLinkedName(TGo4Slot* slot)
{
   void* res = slot;

   emit widgetService(this, service_GetLinkedName2, "", (void*) &res);

   return (const char*) res;
}


TObject* QGo4Widget::GetLinked(const char* linkname, int updatelevel)
{
   TObject* res = 0;
   int func = 0;
   switch (updatelevel) {
      case 0: func = service_GetLinked0; break;
      case 1: func = service_GetLinked1; break;
      case 2: func = service_GetLinked2; break;
      default: func = service_GetLinked1; break;
   }

   emit widgetService(this, func, linkname, (void*) &res);

   return res;
}

void QGo4Widget::RemoveLink(const char* linkname, bool blockreset)
{
   bool oldvalue = fBlockUpdate;
   if (blockreset) fBlockUpdate = true;
   emit widgetService(this, service_RemoveLink, linkname, 0);
   fBlockUpdate = oldvalue;
}

void QGo4Widget::RemoveAllLinks(bool blockreset)
{
   bool oldvalue = fBlockUpdate;
   if (blockreset) fBlockUpdate = true;
   emit widgetService(this, service_RemoveAllLinks, 0, 0);
   fBlockUpdate = oldvalue;
}

void QGo4Widget::RemoveLinksMasked(const char* startedwith, bool blockreset)
{
   bool oldvalue = fBlockUpdate;
   if (blockreset) fBlockUpdate = true;
   emit widgetService(this, service_RemoveAllLinks, startedwith, 0);
   fBlockUpdate = oldvalue;
}

bool QGo4Widget::BrowserItemRemote(const char* itemname)
{
   bool isremote = false;
   emit widgetService(this, service_BrowserItemRemote, itemname, (void*) &isremote);
   return isremote;
}

TGo4BrowserProxy* QGo4Widget::Browser()
{
   if (fBrowserProxy!=0) return fBrowserProxy;

   TGo4BrowserProxy* br = 0;
   emit widgetService(this, service_Browser, "", (void*) &br);
   fBrowserProxy = br;
   return br;
}

void QGo4Widget::ConnectPad(TPad* pad)
{
   emit widgetService(this, service_ConnectPad, "", (void*) pad);
}

void QGo4Widget::CallPanelFunc(int func, TPad* pad)
{
   emit widgetService(this, func, "", (void*) pad);
}


void QGo4Widget::StatusMessage(const char* message)
{
   emit widgetService(this, service_StatusMessage, message, 0);
}


void QGo4Widget::ProcessSignal(const char* linkname, bool assigned, TObject* obj, TGo4Slot* slot)
{
   if (assigned) {
      linkedUpdated(slot, obj);
      linkedObjectUpdated(linkname, obj);
   } else
   // inform widget only if deletion caused not by widget itself by RemoveAllLinks() call
   if (!IsUpdateBlocked()) {
      linkedRemoved(slot, obj);
      linkedObjectRemoved(linkname);
   }
}

void QGo4Widget::linkedObjectUpdated(const char* /* linkname */, TObject* /* obj */)
{
}

void QGo4Widget::linkedObjectRemoved(const char* /* linkname */)
{
   // reset editor if any watched object is removed
   ShootResetWidget();
}

void QGo4Widget::linkedUpdated(TGo4Slot* /* slot */, TObject* /* obj */ )
{
}

void QGo4Widget::linkedRemoved(TGo4Slot* /* slot */, TObject* /* obj */)
{
}

void QGo4Widget::AskToCreateObject(TClass* cl, int isremote)
{
   fWaitsForObjectCreation = (isremote>=0);
   QString str = QString::number(isremote);
   emit widgetService(this, service_CreateItem, str.latin1(), (void*) cl);
}

void QGo4Widget::InformThatObjectCreated(const char* itemname, TClass* cl)
{
   emit widgetService(this, service_ObjectCreated, itemname, cl);
}

void QGo4Widget::requestedObjectCreated(const char* /* itemname */, TClass* /* cl */)
// this function should be reimplemented in editor that asks to create object
{
}

void QGo4Widget::ShowItemInfo(const char* itemname)
{
   emit widgetService(this, service_ShowInfo, itemname, 0);
}

TGo4ViewPanel* QGo4Widget::CreateViewPanel(int ndiv)
{
   QString str = QString::number(ndiv);
   TGo4ViewPanel* res = 0;
   emit widgetService(this, service_CreateViewPanel, str.latin1(), (void*)&res);
   return res;
}

TGo4ViewPanel* QGo4Widget::DrawItem(const char* itemname, TGo4ViewPanel* panel, TPad* pad, bool activate, int updatelevel)
{
   void* res[4];
   res[0] = panel;
   res[1] = pad;
   res[2] = &activate;
   res[3] = &updatelevel;
   emit widgetService(this, service_DrawItem, itemname, res);
   return (TGo4ViewPanel*) res[0];
}

TGo4ViewPanel* QGo4Widget::WhereItemDrawn(const char* itemname)
{
   TGo4ViewPanel* res = false;
   emit widgetService(this, service_WhereItemDrawn, itemname, &res);
   return res;
}

void QGo4Widget::UndrawItem(const char* itemname)
{
   emit widgetService(this, service_UndrawItem, itemname, 0);
}

void QGo4Widget::HelpWindow(const char* filename, const char* msg)
{
   emit widgetService(this, service_HelpWindow, filename, (void*)msg);
}

TGo4ViewPanel* QGo4Widget::LastActivePanel()
{
   TGo4ViewPanel* res = 0;
   emit widgetService(this, service_LastActivePanel, "", (void*)&res);
   return res;
}

void QGo4Widget::EditItem(const char* itemname)
{
   emit widgetService(this, service_EditItem, itemname, 0);
}

void QGo4Widget::EditObjectInSlot(TGo4Slot* slot)
{
   emit widgetService(this, service_EditInSlot, "", (void*) slot);
}

QString QGo4Widget::SaveObjectInMemory(const char* foldername, TObject* obj)
{
   void* par = obj;
   emit widgetService(this, service_SaveToMemory, foldername, (void*) &par);
   QString itemname = "";
   if ((par!=obj) && (par!=0)) {
      QString* res = (QString*) par;
      itemname = *res;
      delete res;
   }
   return itemname;
}

bool QGo4Widget::SaveItemToFile(const char* itemname, const char* subfolder)
{
   char buf[1000];
   if (subfolder==0) strcpy(buf, ""); else
     strcpy(buf, subfolder);
   emit widgetService(this, service_SaveItem, itemname, (void*) buf);
   return (buf[0]!=0);
}

bool QGo4Widget::UpdateItemInAnalysis(const char* itemname, TObject* obj)
{
   TObject* res = obj;
   emit widgetService(this, service_UpdateAnalysisItem, itemname, (void*) &res);
   return (res!=0);
}

TGo4AnalysisProxy* QGo4Widget::GetAnalysis(const char* itemname)
{
   TGo4AnalysisProxy* res = 0;
   emit widgetService(this, service_GetAnalysis, itemname, (void*) &res);
   return res;
}

void QGo4Widget::CallServiceFunc(int func, const char* str, void* par)
{
   emit widgetService(this, func, str, par);
}

void QGo4Widget::ServiceCall(const char* name, void* par)
{
   CallServiceFunc(service_General, name, par);
}

QAction* AddChkAction(QMenu* menu,
                      const QString& text, bool checked,
		              QObject* recv, const char* member)
{
   QAction* act = new QAction(text, menu);
   act->setCheckable(true);
   act->setChecked(checked);
   recv->connect (act, SIGNAL(triggered()), recv, member);
   menu->addAction(act);
   return act;
}

QAction* AddIdAction(QMenu* menu, QSignalMapper* map,
                	const QString& text, int id, int enabled, int checked)
{
   QAction* act = new QAction(text, menu);
   if (checked!=-1) {
      act->setCheckable(true);
      act->setChecked(checked > 0);
   }
   if (enabled!=-1)
      act->setEnabled(enabled > 0);
   map->connect (act, SIGNAL(triggered()), map, SLOT(map()));
   menu->addAction(act);
   map->setMapping(act, id);
   return act;
}

QAction* SetIdAction(QSignalMapper* map, int id, int enabled, int checked)
{
   QAction* act = (QAction*) map->mapping(id);
   if (act==0) return 0;
   if (checked!=-1) {
      act->setCheckable(true);
      act->setChecked(checked > 0);
   }
   if (enabled!=-1)
      act->setEnabled(enabled > 0);
   return act;
}

QAction* AddIdAction(QMenu* menu, QSignalMapper* map,
		const QPixmap& icon, const QString& text, int id, int enabled, int checked)
{
   QAction* act = new QAction(icon, text, menu);
   if (checked!=-1) {
	  act->setCheckable(true);
	  act->setChecked(checked > 0);
   }
   if (enabled!=-1)
	  act->setEnabled(enabled > 0);
   map->connect (act, SIGNAL(triggered()), map, SLOT(map()));
   menu->addAction(act);
   map->setMapping(act, id);
   return act;
}
