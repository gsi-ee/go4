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

#include "QGo4BrowserItem.h"

#include "QGo4BrowserListView.h"

QGo4BrowserItem::QGo4BrowserItem(QListView* parent, QListViewItem* prev, const char* name) :
   QListViewItem(parent, prev, name)
{
}

QGo4BrowserItem::QGo4BrowserItem(QListViewItem* parent, QListViewItem* prev, const char* name) :
   QListViewItem(parent, prev, name)
{
}

bool QGo4BrowserItem::acceptDrop ( const QMimeSource * mime ) const
{
   if (!dropEnabled()) return false;

   QGo4BrowserListView* br = dynamic_cast<QGo4BrowserListView*> (listView());

   return (br==0) ? false : br->IsAcceptItemDrop(this, mime);
}

void QGo4BrowserItem::dropped ( QDropEvent * e )
{
   QGo4BrowserListView* br = dynamic_cast<QGo4BrowserListView*> (listView());
   if (br!=0) br->ProcessItemDrop(this, e);
}

