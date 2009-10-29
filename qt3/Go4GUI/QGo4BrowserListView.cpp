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

#include "QGo4BrowserListView.h"

QGo4BrowserListView::QGo4BrowserListView(QWidget* parent, const char* name, WFlags f) :
  QListView(parent, name, f)
{
}

QGo4BrowserListView::~QGo4BrowserListView()
{
}

QDragObject* QGo4BrowserListView::dragObject()
{
   QDragObject* res = 0;
   emit RequestDragObject(&res);
   return res;
}

bool QGo4BrowserListView::IsAcceptItemDrop(const QListViewItem* item, const QMimeSource * mime)
{
   bool res = false;
   emit ItemDropAccept((void*)item, (void*)mime, &res);
   return res;
}

void QGo4BrowserListView::ProcessItemDrop(QListViewItem* item, QDropEvent * e)
{
   emit ItemDropProcess((void*)item, (void*)e);
}
