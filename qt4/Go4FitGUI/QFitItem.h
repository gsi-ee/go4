#ifndef QFITITEM_H
#define QFITITEM_H

#include <q3listview.h>

class TObject;
class TGo4FitPanel;

class QFitItem : public Q3ListViewItem {
   public:
     QFitItem(TGo4FitPanel* panel, QFitItem* parent, TObject* iObj, int iObjectType, int iWidgetType = -1, int iPopupMenuType = -1, int iGraphType = -1, int iTag = 0);
     QFitItem(TGo4FitPanel* panel, Q3ListView * parent, TObject* iObj, int iObjectType, int iWidgetType = -1, int iPopupMenuType = -1, int iGraphType = -1, int iTag = 0);
     virtual ~QFitItem();

     TObject* Object();
     void SetObject(TObject* obj);
     int ObjectType();
     int WidgetType();
     int PopupMenuType();
     int GraphType();
     int Tag();
     QFitItem* Parent();

     QFitItem* DefineWidgetItem();
     QFitItem* DefineGraphItem();
     bool FindInParents(QFitItem* item);

   protected:
     TGo4FitPanel* fxPanel;
     TObject*      fxObj;
     int           fiObjectType;
     int           fiWidgetType;
     int           fiPopupMenuType;
     int           fiGraphType;
     int           fiTag;
};

#endif // QFITITEM_H
