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

#ifndef QGOBROWSERLISTVIEW_H
#define QGOBROWSERLISTVIEW_H

#include <QtGui/QTreeWidget>

#ifdef WIN32
class QGo4BrowserTreeWidget : public QTreeWidget {

#else
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4BrowserTreeWidget : public QTreeWidget {
#endif
   Q_OBJECT

   public:
      QGo4BrowserTreeWidget(QWidget* parent = 0);
      virtual ~QGo4BrowserTreeWidget();

      virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
      virtual QStringList mimeTypes() const;
      virtual Qt::DropActions supportedDropActions () const;

      virtual void mouseMoveEvent(QMouseEvent *event);

   signals:

      void RequestDragObject(QDrag**);
      void ItemDropProcess(void*, void*);

};

#endif

