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

#include "QFitNamedWidget.h"

#include <QLabel>

#include "QGo4LineEdit.h"
#include "TGo4FitPanel.h"

QFitNamedWidget::QFitNamedWidget( QWidget* parent,  const char* name )
    : QFitWidget(parent, name )
{
    if (!name) setObjectName( "QFitNamedWidget" );

    NameEdt = new QGo4LineEdit(this, "NameEdt");
    NameEdt->setGeometry( QRect( 61, 36, 100, 27 ) );
    NameEdt->setMinimumSize( QSize( 100, 0 ) );
    NameEdt->setMaximumSize( QSize( 100, 32767 ) );

    ClassNameLbl = new QLabel( "ClassNameLbl", this );
    ClassNameLbl->setGeometry( QRect( 12, 12, 155, 18 ) );

    TextLabel1 = new QLabel( "Name", this );
    TextLabel1->setGeometry( QRect( 12, 36, 49, 27 ) );

    TextLabel2 = new QLabel( "Title", this );
    TextLabel2->setGeometry( QRect( 12, 69, 49, 27 ) );

    FullNameLbl = new QLabel( "FullNameLbl", this );
    FullNameLbl->setGeometry( QRect( 173, 36, 95, 27 ) );

    TitleEdt = new QGo4LineEdit( this, "TitleEdt" );
    TitleEdt->setGeometry( QRect( 61, 69, 410, 27 ) );
    TitleEdt->setMinimumSize( QSize( 200, 0 ) );

    resize( QSize(550, 404).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );

    // signals and slots connections
    QObject::connect(NameEdt, &QGo4LineEdit::textChanged, this, &QFitNamedWidget::NameEdt_textChanged);
    QObject::connect(TitleEdt, &QGo4LineEdit::textChanged, this, &QFitNamedWidget::TitleEdt_textChanged);
}

QFitNamedWidget::~QFitNamedWidget()
{
}

void QFitNamedWidget::FillSpecificData()
{
   if (fxPanel)
     fxPanel->FillNamedWidget(this);
}

void QFitNamedWidget::ChangeName(const QString & name)
{
  if (name.length() > 0)
    NameEdt->setText(name);
}

void QFitNamedWidget::NameEdt_textChanged(const QString & name)
{
  if (!fbFillWidget && fxPanel)
    fxPanel->ChangeObjectName(this, name.toLatin1().constData());
}

void QFitNamedWidget::TitleEdt_textChanged(const QString & title)
{
   if(!fbFillWidget && fxPanel)
      fxPanel->ChangeObjectTitle(this, title.toLatin1().constData());
}
