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

#ifndef QFITWIDGET_H
#define QFITWIDGET_H

#include <QWidget>

class TObject;
class QFitItem;
class TGo4Fitter;
class TGo4FitPanel;

#ifdef __GO4DESIGNER__
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitWidget : public QWidget {
#else
class QFitWidget : public QWidget {
#endif

   Q_OBJECT

   public:
      QFitWidget(QWidget *parent = nullptr, const char *name = nullptr, Qt::WindowFlags fl = Qt::Widget);
      ~QFitWidget();

      virtual QFitItem* GetItem();
      virtual TObject *GetObject();
      virtual TGo4Fitter *GetFitter();
      virtual void SetItem(TGo4FitPanel* panel, QFitItem* iItem);
      virtual void FillWidget();
      virtual void FillSpecificData();
      virtual void UpdateWidgetItem(bool trace);
      virtual void SetWidgetItemText(bool trace);
      virtual void UpdateItemsOfType(int typ, bool allitems = false);

   protected:
      TGo4FitPanel* fxPanel{nullptr};
      QFitItem* fxItem{nullptr};
      bool fbFillWidget{false};

      void contextMenuEvent(QContextMenuEvent* e) override;
};

#endif
