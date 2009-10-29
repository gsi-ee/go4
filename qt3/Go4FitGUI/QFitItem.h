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

#ifndef QFITITEM_H
#define QFITITEM_H

#include <qlistview.h>

class TObject;
class TGo4FitPanel;

class QFitItem : public QListViewItem {
   public:
     QFitItem(TGo4FitPanel* panel, QFitItem* parent, TObject* iObj, int iObjectType, int iWidgetType = -1, int iPopupMenuType = -1, int iGraphType = -1, int iTag = 0);
     QFitItem(TGo4FitPanel* panel, QListView * parent, TObject* iObj, int iObjectType, int iWidgetType = -1, int iPopupMenuType = -1, int iGraphType = -1, int iTag = 0);
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
