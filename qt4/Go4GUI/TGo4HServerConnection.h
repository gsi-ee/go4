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

#ifndef TGO4HSERVERCPNNECTION_H
#define TGO4HSERVERCPNNECTION_H

#include <QDialog>
#include "ui_TGo4HServerConnection.h"

class TGo4HServerConnection : public QDialog, public Ui::TGo4HServerConnection
 {
     Q_OBJECT

 public:
     TGo4HServerConnection( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 )
                        : QDialog( parent, fl )
        {
           setObjectName(name ? name : "Go4HServConnection");
           setModal(modal);

          setupUi(this);
        }
 };

#endif
