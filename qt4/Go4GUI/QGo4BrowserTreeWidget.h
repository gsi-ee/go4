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

#ifndef QGOBROWSERLISTVIEW_H
#define QGOBROWSERLISTVIEW_H

#include <QTreeWidget>

#ifdef __GO4DESIGNER__
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4BrowserTreeWidget : public QTreeWidget {
#else
class QGo4BrowserTreeWidget : public QTreeWidget {
#endif
   Q_OBJECT

   public:
      QGo4BrowserTreeWidget(QWidget* parent = nullptr);
      virtual ~QGo4BrowserTreeWidget();

      bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) override;
      QStringList mimeTypes() const override;
      Qt::DropActions supportedDropActions() const override;

      void mouseMoveEvent(QMouseEvent *event) override;

   signals:

      void RequestDragObject(QDrag**);
      void ItemDropProcess(void*, void*);

};

#endif

