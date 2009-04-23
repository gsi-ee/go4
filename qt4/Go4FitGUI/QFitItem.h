#ifndef QFITITEM_H
#define QFITITEM_H

#include <QtGui/QTreeWidgetItem>

class TObject;
class TGo4FitPanel;

class QFitItem : public QTreeWidgetItem {
   public:
     QFitItem(TGo4FitPanel* panel, QTreeWidgetItem* parent, TObject* iObj, int iObjectType, int iWidgetType = -1, int iPopupMenuType = -1, int iGraphType = -1, int iTag = 0);
     virtual ~QFitItem();

     TObject* Object() const { return fxObj; }
     void SetObject(TObject* obj) { fxObj = obj; }
     int ObjectType() const { return fiObjectType; }
     int WidgetType() const { return fiWidgetType; }
     int PopupMenuType() const { return fiPopupMenuType; }
     int GraphType() const { return fiGraphType; }
     int Tag() const { return fiTag; }

     QFitItem* Parent() const { return dynamic_cast<QFitItem*> (parent()); }


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
