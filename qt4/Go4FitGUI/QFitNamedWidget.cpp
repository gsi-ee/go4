#include "QFitNamedWidget.h"

#include "qlabel.h"

#include "QGo4LineEdit.h"
#include "QFitItem.h"
#include "TGo4FitPanel.h"

QFitNamedWidget::QFitNamedWidget( QWidget* parent,  const char* name )
    : QFitWidget( parent, name )
{
    if (name==0) setName( "QFitNamedWidget" );

    NameEdt = new QGo4LineEdit( this, "NameEdt" );
    NameEdt->setGeometry( QRect( 61, 36, 100, 27 ) );
    NameEdt->setMinimumSize( QSize( 100, 0 ) );
    NameEdt->setMaximumSize( QSize( 100, 32767 ) );

    ClassNameLbl = new QLabel( "ClassNameLbl", this, "ClassNameLbl" );
    ClassNameLbl->setGeometry( QRect( 12, 12, 155, 18 ) );

    TextLabel1 = new QLabel( "Name", this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 12, 36, 49, 27 ) );

    TextLabel2 = new QLabel( "Title", this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 12, 69, 49, 27 ) );

    FullNameLbl = new QLabel( "FullNameLbl", this, "FullNameLbl" );
    FullNameLbl->setGeometry( QRect( 173, 36, 95, 27 ) );

    TitleEdt = new QGo4LineEdit( this, "TitleEdt" );
    TitleEdt->setGeometry( QRect( 61, 69, 410, 27 ) );
    TitleEdt->setMinimumSize( QSize( 200, 0 ) );

    resize( QSize(550, 404).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );

    // signals and slots connections
    connect( NameEdt, SIGNAL( textChanged(const QString&) ), this, SLOT( NameEdt_textChanged(const QString&) ) );
    connect( TitleEdt, SIGNAL( textChanged(const QString&) ), this, SLOT( TitleEdt_textChanged(const QString&) ) );
}

QFitNamedWidget::~QFitNamedWidget()
{
}

void QFitNamedWidget::FillSpecificData()
{
   if (fxPanel!=0)
     fxPanel->FillNamedWidget(this);
}

void QFitNamedWidget::ChangeName(const QString & name)
{
  if (name.length()>0)
    NameEdt->setText(name);
}

void QFitNamedWidget::NameEdt_textChanged(const QString & name)
{
  if (!fbFillWidget && (fxPanel!=0))
    fxPanel->ChangeObjectName(this, name.latin1());
}

void QFitNamedWidget::TitleEdt_textChanged(const QString & title)
{
   if(!fbFillWidget && (fxPanel!=0))
      fxPanel->ChangeObjectTitle(this, title.latin1());
}
