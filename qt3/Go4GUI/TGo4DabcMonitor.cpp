/****************************************************************************
** Form implementation generated from reading ui file 'TGo4DabcMonitor.ui'
**
** Created: Mo Aug 18 13:42:21 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "TGo4DabcMonitor.h"

#include <qvariant.h>
#include <iostream>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <QGo4LineEdit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "TGo4LockGuard.h"
#include "qdatetime.h"
#include "qregexp.h"
#include "TH1.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"
#include "qpixmap.h"
#include "TGo4QSettings.h"
#include "TGo4DabcMonitor.ui.h"
/*
 *  Constructs a TGo4DabcMonitor which is a child of 'parent', with the
 *  name 'name'.' 
 */
TGo4DabcMonitor::TGo4DabcMonitor( QWidget* parent,  const char* name )
    : QGo4Widget( parent, name )
{
    if ( !name )
	setName( "TGo4DabcMonitor" );
    TGo4DabcMonitorLayout = new QGridLayout( this, 1, 1, 11, 6, "TGo4DabcMonitorLayout"); 

    ratemeterBox = new QGroupBox( this, "ratemeterBox" );
    ratemeterBox->setColumnLayout(0, Qt::Vertical );
    ratemeterBox->layout()->setSpacing( 6 );
    ratemeterBox->layout()->setMargin( 11 );
    ratemeterBoxLayout = new QGridLayout( ratemeterBox->layout() );
    ratemeterBoxLayout->setAlignment( Qt::AlignTop );

    RateTable = new QGo4Table( ratemeterBox, "RateTable" );
    RateTable->setNumRows( 3 );
    RateTable->setNumCols( 5 );
    RateTable->setSorting( TRUE );

    ratemeterBoxLayout->addWidget( RateTable, 0, 0 );

    TGo4DabcMonitorLayout->addMultiCellWidget( ratemeterBox, 0, 1, 2, 2 );

    dabcNodeBox = new QGroupBox( this, "dabcNodeBox" );

    NodeTable = new QGo4Table( dabcNodeBox, "NodeTable" );
    NodeTable->setGeometry( QRect( 11, 21, 300, 237 ) );
    NodeTable->setNumRows( 3 );
    NodeTable->setNumCols( 5 );
    NodeTable->setRowMovingEnabled( FALSE );
    NodeTable->setReadOnly( FALSE );
    NodeTable->setSorting( TRUE );

    TGo4DabcMonitorLayout->addMultiCellWidget( dabcNodeBox, 0, 1, 1, 1 );

    controlBox = new QGroupBox( this, "controlBox" );
    controlBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, controlBox->sizePolicy().hasHeightForWidth() ) );
    controlBox->setMinimumSize( QSize( 200, 0 ) );
    controlBox->setMaximumSize( QSize( 200, 32767 ) );
    controlBox->setFrameShape( QGroupBox::GroupBoxPanel );
    controlBox->setFrameShadow( QGroupBox::Sunken );
    controlBox->setColumnLayout(0, Qt::Vertical );
    controlBox->layout()->setSpacing( 6 );
    controlBox->layout()->setMargin( 11 );
    controlBoxLayout = new QGridLayout( controlBox->layout() );
    controlBoxLayout->setAlignment( Qt::AlignTop );

    serviceFilterlayout = new QHBoxLayout( 0, 0, 6, "serviceFilterlayout"); 

    dimServiceFilterEdit = new QGo4LineEdit( controlBox, "dimServiceFilterEdit" );
    serviceFilterlayout->addWidget( dimServiceFilterEdit );

    FullPrintDIMButton = new QPushButton( controlBox, "FullPrintDIMButton" );
    FullPrintDIMButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FullPrintDIMButton->sizePolicy().hasHeightForWidth() ) );
    FullPrintDIMButton->setMaximumSize( QSize( 30, 30 ) );
    FullPrintDIMButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "log.png" ) ) );
    serviceFilterlayout->addWidget( FullPrintDIMButton );

    controlBoxLayout->addLayout( serviceFilterlayout, 0, 0 );

    HistogramCheck = new QCheckBox( controlBox, "HistogramCheck" );
    QFont HistogramCheck_font(  HistogramCheck->font() );
    HistogramCheck_font.setBold( TRUE );
    HistogramCheck->setFont( HistogramCheck_font ); 

    controlBoxLayout->addWidget( HistogramCheck, 1, 0 );

    layout64 = new QHBoxLayout( 0, 0, 6, "layout64"); 

    TrendAverageCheck = new QCheckBox( controlBox, "TrendAverageCheck" );
    layout64->addWidget( TrendAverageCheck );

    FrequencyBox = new QSpinBox( controlBox, "FrequencyBox" );
    FrequencyBox->setMaxValue( 3600 );
    FrequencyBox->setMinValue( 1 );
    FrequencyBox->setValue( 2 );
    layout64->addWidget( FrequencyBox );

    controlBoxLayout->addLayout( layout64, 4, 0 );

    layout67 = new QHBoxLayout( 0, 0, 6, "layout67"); 

    TrendBinsBox = new QSpinBox( controlBox, "TrendBinsBox" );
    TrendBinsBox->setMaxValue( 100000 );
    TrendBinsBox->setMinValue( 10 );
    TrendBinsBox->setLineStep( 100 );
    TrendBinsBox->setValue( 1000 );
    layout67->addWidget( TrendBinsBox );
    spacer12 = new QSpacerItem( 91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout67->addItem( spacer12 );

    controlBoxLayout->addLayout( layout67, 2, 0 );

    TrendSampleCheck = new QCheckBox( controlBox, "TrendSampleCheck" );

    controlBoxLayout->addWidget( TrendSampleCheck, 3, 0 );

    TGo4DabcMonitorLayout->addWidget( controlBox, 1, 0 );

    dnsBox = new QGroupBox( this, "dnsBox" );
    dnsBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, dnsBox->sizePolicy().hasHeightForWidth() ) );
    dnsBox->setMinimumSize( QSize( 200, 0 ) );
    dnsBox->setMaximumSize( QSize( 200, 32767 ) );
    dnsBox->setMargin( 2 );
    dnsBox->setColumnLayout(0, Qt::Vertical );
    dnsBox->layout()->setSpacing( 6 );
    dnsBox->layout()->setMargin( 11 );
    dnsBoxLayout = new QVBoxLayout( dnsBox->layout() );
    dnsBoxLayout->setAlignment( Qt::AlignTop );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    dimDnsNodeEdit = new QGo4LineEdit( dnsBox, "dimDnsNodeEdit" );
    layout2->addWidget( dimDnsNodeEdit );

    RefreshDIMButton = new QPushButton( dnsBox, "RefreshDIMButton" );
    RefreshDIMButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, RefreshDIMButton->sizePolicy().hasHeightForWidth() ) );
    RefreshDIMButton->setMaximumSize( QSize( 30, 30 ) );
    RefreshDIMButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "refresh.png" ) ) );
    layout2->addWidget( RefreshDIMButton );
    dnsBoxLayout->addLayout( layout2 );

    DateLabel = new QLabel( dnsBox, "DateLabel" );
    DateLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, DateLabel->sizePolicy().hasHeightForWidth() ) );
    DateLabel->setMinimumSize( QSize( 100, 12 ) );
    DateLabel->setFrameShape( QLabel::Box );
    DateLabel->setFrameShadow( QLabel::Raised );
    DateLabel->setMargin( 0 );
    dnsBoxLayout->addWidget( DateLabel );

    TGo4DabcMonitorLayout->addWidget( dnsBox, 0, 0 );
    languageChange();
    resize( QSize(879, 286).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( RefreshDIMButton, SIGNAL( clicked() ), this, SLOT( refreshDIMSlot() ) );
    connect( FullPrintDIMButton, SIGNAL( clicked() ), this, SLOT( logDIMSlot() ) );
    connect( NodeTable, SIGNAL( valueChanged(int,int) ), this, SLOT( nodeTableChangedSlot(int,int) ) );
    connect( RateTable, SIGNAL( valueChanged(int,int) ), this, SLOT( rateTableChangedSlot(int,int) ) );
    connect( HistogramCheck, SIGNAL( toggled(bool) ), this, SLOT( histogramCheckToggled(bool) ) );
    connect( TrendAverageCheck, SIGNAL( toggled(bool) ), this, SLOT( averageCheckToggled(bool) ) );
    connect( TrendBinsBox, SIGNAL( valueChanged(int) ), this, SLOT( binsizeChanged(int) ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
TGo4DabcMonitor::~TGo4DabcMonitor()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TGo4DabcMonitor::languageChange()
{
    setCaption( tr( "DABC Monitor" ) );
    ratemeterBox->setTitle( tr( "Ratemeter View" ) );
    dabcNodeBox->setTitle( tr( "DABC Nodes" ) );
    controlBox->setTitle( tr( "Inspector" ) );
    QToolTip::add( dimServiceFilterEdit, tr( "Filter for service names in log output" ) );
    FullPrintDIMButton->setText( QString::null );
    QToolTip::add( FullPrintDIMButton, tr( "Print DIM services of the nodes checked as 'Dump' to terminal" ) );
    HistogramCheck->setText( tr( "histogramming active" ) );
    QToolTip::add( HistogramCheck, tr( "Check to update monitoring histograms as selected in ratemeter table" ) );
    TrendAverageCheck->setText( tr( "average" ) );
    QToolTip::add( TrendAverageCheck, tr( "Check to update average histograms over refresh time" ) );
    FrequencyBox->setPrefix( QString::null );
    FrequencyBox->setSuffix( tr( " s" ) );
    FrequencyBox->setSpecialValueText( QString::null );
    QToolTip::add( FrequencyBox, tr( "Refresh period" ) );
    TrendBinsBox->setPrefix( QString::null );
    TrendBinsBox->setSuffix( tr( " bins" ) );
    TrendBinsBox->setSpecialValueText( QString::null );
    QToolTip::add( TrendBinsBox, tr( "Binsize for trending histograms" ) );
    TrendSampleCheck->setText( tr( "each sample" ) );
    QToolTip::add( TrendSampleCheck, tr( "Check to update direct sample histograms" ) );
    dnsBox->setTitle( tr( "DIM dns server" ) );
    QToolTip::add( dimDnsNodeEdit, tr( "Set DIM_DNS_NODE" ) );
    RefreshDIMButton->setText( QString::null );
    QToolTip::add( RefreshDIMButton, tr( "Refresh list from DIM server" ) );
    DateLabel->setText( tr( "Date" ) );
    QToolTip::add( DateLabel, tr( "Date and time of last refresh." ) );
}

