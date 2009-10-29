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

#ifndef QUSERPANEL_H
#define QUSERPANEL_H

#include "ui_QUserPanel.h"
#include <QGo4Widget.h>

class QUserPanel : public QGo4Widget, public Ui::QUserPanel
{
    Q_OBJECT

public:
    QUserPanel( QWidget* parent = 0, const char* name = 0 );
    virtual ~QUserPanel();
    virtual QString kindString( int kind );
    virtual bool IsAcceptDrag( const char * itemname, TClass * cl, int kind );
    virtual void DropItem( const char * itemname, TClass * cl, int kind );
    virtual void linkedObjectUpdated( const char * linkname, TObject * obj );
    virtual void linkedObjectRemoved( const char * linkname );
    virtual void PrintObject(TObject* obj);
    virtual void DrawObjectOnCanvas(const char* itemname);

public slots:
    virtual void InfoBtn_clicked();
    virtual void EditBtn_clicked();
    virtual void DrawBtn_clicked();
    virtual void CanvasDropEventSlot(QDropEvent*,TPad*);
};

#endif
