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

#ifndef QGO4BROWSERITEM_H
#define QGO4BROWSERITEM_H

#include "qlistview.h"

class QGo4BrowserItem : public QListViewItem {
   public:
      QGo4BrowserItem(QListView* parent, QListViewItem* prev, const char* name);
      QGo4BrowserItem(QListViewItem* parent, QListViewItem* prev, const char* name);

      virtual bool acceptDrop ( const QMimeSource * mime ) const;
      virtual void dropped ( QDropEvent * e );
};

#endif
