#include "helpwindow.h"

#include "TSystem.h"

#include "q3textbrowser.h"
#include "qdir.h"
#include "qstatusbar.h"
#include "qmenubar.h"
#include "qtoolbutton.h"
#include "qfile.h"
#include "qmessagebox.h"
#include "q3filedialog.h"
#include "qapplication.h"
#include "qcombobox.h"
#include "qprinter.h"
#include "q3simplerichtext.h"
#include "qpainter.h"
#include "q3paintdevicemetrics.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <Q3PopupMenu>

using namespace Qt;

TGo4HelpWindow::TGo4HelpWindow( const QString& home_, QWidget* parent, const QString& subdir)
    : Q3MainWindow( parent, home_.latin1(), Qt::WDestructiveClose ),
      pathCombo( 0 ),
      selectedURL()
{
    readHistory();
    readBookmarks();

    browser = new Q3TextBrowser( this );

    _path = gSystem->Getenv("GO4SYS");
    _path.append("/docs");
    if(!subdir.isEmpty())
      {
         _path.append("/");
         _path.append(subdir);
      }
    browser->mimeSourceFactory()->setFilePath( _path );
    browser->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    connect( browser, SIGNAL( textChanged() ),
        this, SLOT( textChanged() ) );

    setCentralWidget( browser );

    if ( !home_.isEmpty() )
    browser->setSource( home_ );

    connect( browser, SIGNAL( highlighted( const QString&) ),
        statusBar(), SLOT( message( const QString&)) );

    resize( 640,700 );


    Q3PopupMenu* file = new Q3PopupMenu( this );
    file->insertItem( tr("&Print"), this, SLOT( print() ), CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT( close() ), CTRL+Key_Q );
    file->insertItem( tr("E&xit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_X );

    // The same three icons are used twice each.
    QString picPath = gSystem->Getenv("GO4SYS");
    picPath.append("/images/");
    QIcon icon_back(QPixmap(picPath + "back.xpm"));
    QIcon icon_forward(QPixmap(picPath + "forward.xpm"));
    QIcon icon_home(QPixmap(picPath + "home.xpm"));

    Q3PopupMenu* go = new Q3PopupMenu( this );
    backwardId = go->insertItem( icon_back,
             tr("&Backward"), browser, SLOT( backward() ),
             CTRL+Key_Left );
    forwardId = go->insertItem( icon_forward,
            tr("&Forward"), browser, SLOT( forward() ),
            CTRL+Key_Right );
    go->insertItem( icon_home, tr("&Home"), browser, SLOT( home() ) );

    Q3PopupMenu* help = new Q3PopupMenu( this );
    help->insertItem( tr("&About ..."), this, SLOT( about() ) );
    help->insertItem( tr("About &Qt ..."), this, SLOT( aboutQt() ) );

    hist = new Q3PopupMenu( this );
    QStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it )
   mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, SIGNAL( activated( int ) ),
        this, SLOT( histChosen( int ) ) );

    bookm = new Q3PopupMenu( this );
    bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
    bookm->insertSeparator();

    QStringList::Iterator it2 = bookmarks.begin();
    for ( ; it2 != bookmarks.end(); ++it2 )
   mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
    connect( bookm, SIGNAL( activated( int ) ),
        this, SLOT( bookmChosen( int ) ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );
    menuBar()->insertItem( tr( "History" ), hist );
    menuBar()->insertItem( tr( "Bookmarks" ), bookm );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("&Help"), help );

    menuBar()->setItemEnabled( forwardId, FALSE);
    menuBar()->setItemEnabled( backwardId, FALSE);
    connect( browser, SIGNAL( backwardAvailable( bool ) ),
        this, SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ),
        this, SLOT( setForwardAvailable( bool ) ) );


    Q3ToolBar* toolbar = new Q3ToolBar( this );
    addToolBar( toolbar, "Toolbar");
    QToolButton* button;

    button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_home, tr("Home"), "", browser, SLOT(home()), toolbar );

    toolbar->addSeparator();

    pathCombo = new QComboBox( TRUE, toolbar );
    connect( pathCombo, SIGNAL( activated( const QString & ) ),
        this, SLOT( pathSelected( const QString & ) ) );
    toolbar->setStretchableWidget( pathCombo );
    setRightJustification( TRUE );
    setDockEnabled( DockLeft, FALSE );
    setDockEnabled( DockRight, FALSE );

    pathCombo->insertItem( home_ );
    browser->setFocus();
}

void TGo4HelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void TGo4HelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}

void TGo4HelpWindow::textChanged()
{
    browser->mimeSourceFactory()->setFilePath( _path );
    if ( browser->documentTitle().isNull() )
   setCaption( "Go4 - Helpviewer - " + browser->context() );
    else
   setCaption( "Go4 - Helpviewer - " + browser->documentTitle() ) ;

    selectedURL = browser->context();

    if ( !selectedURL.isEmpty() && pathCombo ) {
   bool exists = FALSE;
   int i;
   for ( i = 0; i < pathCombo->count(); ++i ) {
       if ( pathCombo->text( i ) == selectedURL ) {
      exists = TRUE;
      break;
       }
   }
   if ( !exists ) {
       pathCombo->insertItem( selectedURL, 0 );
       pathCombo->setCurrentItem( 0 );
       mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
   } else
       pathCombo->setCurrentItem( i );
   selectedURL = QString::null;
    }
}

TGo4HelpWindow::~TGo4HelpWindow()
{
    history.clear();
    QMap<int, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
   history.append( *it );

    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( QIODevice::WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
   bookmarks.append( *it2 );

    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    f2.open( QIODevice::WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
}

void TGo4HelpWindow::about()
{
    QMessageBox::about( this, "Go4 Online Help",
         "Go4 Online Help"
         );
}

void TGo4HelpWindow::aboutQt()
{
  QMessageBox::aboutQt( this, "QBrowser" );
}

void TGo4HelpWindow::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
   QPainter p( &printer );
   Q3PaintDeviceMetrics metrics(p.device());
   int dpix = metrics.logicalDpiX();
   int dpiy = metrics.logicalDpiY();
   const int margin = 72; // pt
   QRect body(margin*dpix/72, margin*dpiy/72,
         metrics.width()-margin*dpix/72*2,
         metrics.height()-margin*dpiy/72*2 );
   Q3SimpleRichText richText( browser->text(), QFont(), browser->context(), browser->styleSheet(),
              browser->mimeSourceFactory(), body.height() );
   richText.setWidth( &p, body.width() );
   QRect view( body );
   int page = 1;
   do {
       richText.draw( &p, body.left(), body.top(), view, colorGroup() );
       view.moveBy( 0, body.height() );
       p.translate( 0 , -body.height() );
       p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
         view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
       if ( view.top()  >= richText.height() )
      break;
       printer.newPage();
       page++;
   } while (TRUE);
    }
#endif
}

void TGo4HelpWindow::pathSelected( const QString &_path )
{
    browser->setSource( _path );
    QMap<int, QString>::Iterator it = mHistory.begin();
    bool exists = FALSE;
    for ( ; it != mHistory.end(); ++it ) {
   if ( *it == _path ) {
       exists = TRUE;
       break;
   }
    }
    if ( !exists )
   mHistory[ hist->insertItem( _path ) ] = _path;
}

void TGo4HelpWindow::readHistory()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
   QFile f( QDir::currentDirPath() + "/.history" );
   f.open( QIODevice::ReadOnly );
   QDataStream s( &f );
   s >> history;
   f.close();
   while ( history.count() > 20 )
       history.remove( history.begin() );
    }
}

void TGo4HelpWindow::histChosen( int i )
{
   if ( mHistory.contains( i ) )
     browser->setSource( mHistory[ i ] );
}

void TGo4HelpWindow::bookmChosen( int i )
{
   if ( mBookmarks.contains( i ) )
     browser->setSource( mBookmarks[ i ] );
}

void TGo4HelpWindow::addBookmark()
{
    mBookmarks[ bookm->insertItem( caption() ) ] = browser->context();
}

void TGo4HelpWindow::readBookmarks()
{
   if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
      QFile f( QDir::currentDirPath() + "/.bookmarks" );
      f.open( QIODevice::ReadOnly );
      QDataStream s( &f );
      s >> bookmarks;
      f.close();
   }
}


