// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef QFITWIDGET_H
#define QFITWIDGET_H

#include <QWidget>

class TObject;
class QFitItem;
class TGo4Fitter;
class TGo4FitPanel;

#ifdef WIN32
class QFitWidget : public QWidget {
#else
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitWidget : public QWidget {
#endif


   Q_OBJECT

   public:
      QFitWidget( QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
      ~QFitWidget();

      virtual QFitItem* GetItem();
      virtual TObject* GetObject();
      virtual TGo4Fitter* GetFitter();
      virtual void SetItem(TGo4FitPanel* panel, QFitItem* iItem);
      virtual void FillWidget();
      virtual void FillSpecificData();
      virtual void UpdateWidgetItem(bool trace);
      virtual void SetWidgetItemText(bool trace);
      virtual void UpdateItemsOfType(int typ, bool allitems);

   protected:
      TGo4FitPanel* fxPanel;
      QFitItem* fxItem;
      bool fbFillWidget;

      virtual void closeEvent(QCloseEvent* ce);
      virtual void contextMenuEvent(QContextMenuEvent* e);
};

#endif

