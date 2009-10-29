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

#ifndef QGO4TABLE_H
#define QGO4TABLE_H

#include "qtable.h"
#include "qwidgetplugin.h"


class QT_WIDGET_PLUGIN_EXPORT QGo4Table : public QTable {
   Q_OBJECT

   public:
    friend class QTableHeader;
    friend class QComboTableItem;
    friend class QCheckTableItem;
    friend class QTableItem;



      QGo4Table(QWidget *parent=0, const char *name=0);
      virtual ~QGo4Table();


   protected:
     virtual void sortColumn ( int col, bool ascending = TRUE, bool wholeRows = TRUE );
};

#endif
