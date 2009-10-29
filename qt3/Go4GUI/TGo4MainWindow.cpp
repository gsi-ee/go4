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

#include "TGo4MainWindow.h"

#include <dlfcn.h>
#include "Riostream.h"

#include "qdockarea.h"
#include "qcolor.h"
#include "qcolordialog.h"
#include "qnamespace.h"
#include "qstring.h"
#include "qvaluevector.h"
#include "qtabwidget.h"
#include "qvariant.h"
#include "qmime.h"
#include "qdragobject.h"
#include "qlayout.h"
#include "qtooltip.h"
#include "qwhatsthis.h"
#include "qaction.h"
#include "qmenubar.h"
#include "qpopupmenu.h"
#include "qtoolbar.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qtextbrowser.h"
#include "qfontdialog.h"
#include "TGo4QSettings.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qstatusbar.h"
#include "qtoolbutton.h"
#include "qsignalmapper.h"
#include "qstylefactory.h"
#include "qfiledialog.h"
#include "qlayout.h"
#include "qmessagebox.h"
#include "qradiobutton.h"
#include "qcombobox.h"
#include "qinputdialog.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qevent.h"
#include "qprocess.h"
#include "qtimer.h"
#include "qspinbox.h"
#include "qstyle.h"

//////// root includes;
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TStyle.h"
#include "TH1.h"
#include "TTree.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TList.h"

/////// Go4 includes, other packages:
#include "qrootapplication.h"
#include "TGo4Version.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4Marker.h"
#include "TGo4WorkSpace.h"
#include "TGo4Fitter.h"
#include "TGo4FitPanel.h"
#include "TGo4DynamicEntry.h"
#include "TGo4AnalysisStatus.h"

#include "TGo4ObjectManager.h"
#include "TGo4ObjectProxy.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4WidgetProxy.h"

//////// Go4 includes from Go4GUI package:
#include "QGo4Widget.h"
#include "TGo4TreeViewer.h"
#include "TGo4Browser.h"
#include "TGo4BrowserOptions.h"
#include "TGo4Style.h"
#include "TGo4ViewPanel.h"
#include "TGo4AnalysisConfiguration.h"
#include "TGo4AnalysisWindow.h"
#include "TGo4StartClient.h"
#include "TGo4HisDrawOptions.h"
#include "TGo4HisDrawOptionsShort.h"
#include "TGo4ScaleOptions.h"
#include "TGo4DividePad.h"
#include "TGo4CreateNewHistogram.h"
#include "TGo4CreateNewCondition.h"
#include "TGo4CreateNewDynEntry.h"
#include "TGo4ConditionInfo.h"
#include "TGo4HistogramInfo.h"
#include "TGo4EventInfo.h"
#include "TGo4LoadedLibraries.h"
#include "TGo4EditDynEntry.h"
#include "TGo4ParaEdit.h"
#include "TGo4AnalysisStatusMonitor.h"
#include "TGo4ConditionEditor.h"
#include "TGo4LogSettings.h"
#include "TGo4LogInfo.h"
#include "TGo4OpenRemoteDialog.h"
#include "TGo4ConnectServer.h"
#include "TGo4HServerConnection.h"
#include "TGo4MBSViewer.h"

#ifdef __GO4DIM__
#include "TGo4DabcMonitor.h"
#endif

#include "TGo4SetScaleValues.h"
#include "TGo4CommandLine.h"
#include "TGo4MarkerSettings.h"
#include "TGo4OptStatsSettings.h"

#include "TGo4Script.h"

enum { fiFetchWhenDrawId = 111,
       fiFetchWhenCopyId = 112,
       fiFetchWhenSaveId = 113,
       fiCrosshairId = 123,
       fiEventstatusId = 124,
       fiCloneId = 125,
       fiDrawTimeId = 126,
       fiDrawDateId = 127,
       fiDrawItemId = 128,
       fiLaunchAnal = 201,
       fiConnectAnal = 202,
       fiPrepareAnal = 203,
       fiDisconnectAnal = 204,
       fiShutdownAnal = 205,
       fiSumbStartAnal = 206,
       fiStartAnal = 207,
       fiStopAnal = 208,
       fiAnalConfig = 209,
       fiAnalTermin = 210 };

TGo4MainWindow::TGo4MainWindow(QApplication* app, bool server) :
   QMainWindow(0, "MainWindow"),
   fApp(app),
   fbServerMode(server)
{
   setDockWindowsMovable(TRUE);
   statusBar();
   resize( 1152, 864 );

   setIcon(QPixmap::fromMimeSource("go4logo2_big.png"));
   setRightJustification( TRUE );
   setUsesTextLabel(TRUE);
   setOpaqueMoving(FALSE);

   fxOM = new TGo4ObjectManager("GUI_OM","Gui object manager");
   fOMDataPath    = "data";
   fOMBrowserPath = "gui";
   fOMEditorsPath = "editors";

   fKillCommand = "";
   fLastPassword = "";
   fLastFileDir = QDir::currentDirPath();

   fCloseCounter = 0;
   fConnectingCounter = 0;

   fbPanelTimerActive = false;

    // create mount point for all data sources
   fxOM->MakeFolder(fOMDataPath.latin1());

   // create entry for browser
   TGo4BrowserProxy* br = new TGo4BrowserProxy(fOMDataPath.latin1(), fOMEditorsPath.latin1(), kFALSE);
   fxOM->AddProxy("", br, fOMBrowserPath.latin1(), "Place for gui slots");
   br->CreateMemoryFolder();

   TH1I* h1 = new TH1I("histo1","histo title", 100, -10., 10.);
   h1->FillRandom("gaus",10000);
   h1->SetDirectory(0);
   br->SaveToMemory("", h1, kTRUE);

   // br->OpenFile("asf.root");


   new TGo4Script(this);


   gStyle->SetPalette(1);

   gStyle->SetCanvasColor(10); // no effect, can be set in settings
   gStyle->SetPadColor(10); // no effect
   gStyle->SetFrameFillColor(10); // works if not 0
   gStyle->SetStatColor(0); // works

   //gStyle->SetOptStat(11111111);
   gStyle->SetOptStat(go4sett->getOptStat());

   fxTGo4WorkSpace = new TGo4WorkSpace(this);
   fxTGo4WorkSpace->setScrollBarsEnabled(TRUE);
   setCentralWidget(fxTGo4WorkSpace);
   fxTGo4WorkSpace->setFocus();

   AddFileMenu();
   AddFileToolBar();

   AddToolsMenu();
   AddToolsBar();

   AddAnalysisMenu();
   AddAnalysisBar();

   AddSettingMenu();

   UpdateCaptionButtons();

   windowsMenu = new QPopupMenu( this );
   connect( windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow() ) );
   menuBar()->insertItem( "&Windows", windowsMenu );

   statusBar()->message("Ready");
   statusBar()->setSizeGripEnabled(TRUE);

   QDockWindow* BrowserDockWin = new QDockWindow();
   BrowserDockWin->setResizeEnabled(TRUE);
   BrowserDockWin->setCaption("Browser");
   setAppropriate(BrowserDockWin, true);
   TGo4Browser* browser = new TGo4Browser(BrowserDockWin,"Browser");
   ConnectGo4Widget(browser);
   browser->StartWorking();
   BrowserDockWin->setWidget(browser);
   addDockWindow(BrowserDockWin, Qt::DockRight);
   BrowserDockWin->show();

   QDockWindow* MBSDockWin = new QDockWindow();
   MBSDockWin->setResizeEnabled(TRUE);
   MBSDockWin->setCaption("MBS monitor");
   setAppropriate(MBSDockWin, true);
   TGo4MBSViewer* mbs = new TGo4MBSViewer(MBSDockWin, "MBSViewer");
   ConnectGo4Widget(mbs);
   MBSDockWin->setWidget(mbs);
   addDockWindow(MBSDockWin, Qt::DockBottom);
   MBSDockWin->show();
#ifdef __GO4DIM__
   QDockWindow* DABCDockWin = new QDockWindow();
   DABCDockWin->setResizeEnabled(TRUE);
   DABCDockWin->setCaption("DABC monitor");
   setAppropriate(DABCDockWin, true);
   TGo4DabcMonitor* dabc = new TGo4DabcMonitor(DABCDockWin, "DABCMonitor");
   ConnectGo4Widget(dabc);
   DABCDockWin->setWidget(dabc);
   addDockWindow(DABCDockWin, Qt::DockBottom);
   DABCDockWin->show();
#endif

   QToolBar* DividePanelBar = new QToolBar (this, "Canvas Tools");
   addDockWindow(DividePanelBar, "Canvas Tools", Qt::DockTop, TRUE );
   setAppropriate (DividePanelBar, true);
   DividePanelBar->setHorizontallyStretchable(FALSE);
   TGo4DividePad* dividepanel = new TGo4DividePad(DividePanelBar, "DividePad");
   dividepanel->polish();
   dividepanel->show();

   QToolBar* stylebar = new QToolBar(this, "Color Tools");
   addDockWindow(stylebar, "Color Tools", Qt::DockTop, TRUE );
   setAppropriate (stylebar, true);
   TGo4Style* style = new TGo4Style(stylebar, "StyleToolBar");
   style->polish();
   style->show();

   QToolBar* BrowserOptionsPanel = new QToolBar(this,"Browser Options Panel");
   addDockWindow(BrowserOptionsPanel, "Browser Options", Qt::DockTop, TRUE);
   TGo4BrowserOptions* bropt = new TGo4BrowserOptions(BrowserOptionsPanel,"BrowserOptions");
   ConnectGo4Widget(bropt);
   bropt->StartWorking();
   bropt->polish();
   bropt->show();

   QToolBar* DrawOptionPanel = new QToolBar(this,"Histogram Draw Options");
   addDockWindow(DrawOptionPanel, "Draw Options", Qt::DockTop, TRUE);
   TGo4HisDrawOptions* drawopt = new TGo4HisDrawOptions(DrawOptionPanel,"HisDrawOptions");
   drawopt->polish();
   drawopt->show();

   QToolBar* DrawOptionShortPanel = new QToolBar(this,"Histogram Short Draw Options");
   addDockWindow(DrawOptionShortPanel, "Draw Options Short", Qt::DockTop, TRUE);
   TGo4HisDrawOptionsShort* drawopts = new TGo4HisDrawOptionsShort(DrawOptionShortPanel,"HisDrawOptionsShort");
   drawopts->polish();
   drawopts->show();

   QToolBar* ScaleOptionBar = new QToolBar(this, "Zoom Tools");
   addDockWindow(ScaleOptionBar, "Zoom Tools", Qt::DockTop, TRUE );
   TGo4ScaleOptions* scopt = new TGo4ScaleOptions(ScaleOptionBar,"ScaleOptions");
   ConnectGo4Widget(scopt);
   scopt->polish();
   scopt->show();
   setAppropriate(ScaleOptionBar, TRUE);

   QDockWindow* commandlinebar = new QDockWindow();
   commandlinebar->setResizeEnabled(TRUE);
   commandlinebar->setCaption("Go4 Command Line");
   setAppropriate (commandlinebar, true);
   TGo4CommandLine* cli = new TGo4CommandLine(commandlinebar, "CommandlineToolBar");
   cli->setMainWindow(this);
   ConnectGo4Widget(cli);
   commandlinebar->setWidget(cli);
   cli->polish();
   cli->show();
   addDockWindow(commandlinebar, Qt::DockBottom);
   commandlinebar->show();


   QDockWindow* tviewerdock = new QDockWindow();
   tviewerdock->setResizeEnabled(TRUE);
   tviewerdock->setCaption("Tree viewer");
   setAppropriate(tviewerdock, true);
   TGo4TreeViewer* tviewer = new TGo4TreeViewer(tviewerdock, "TreeViewer");
   ConnectGo4Widget(tviewer);
   tviewerdock->setWidget(tviewer);
   addDockWindow(tviewerdock, Qt::DockBottom);
   tviewerdock->show();

   QDockWindow* lidock = new QDockWindow();
   lidock->setResizeEnabled( TRUE );
   lidock->setCaption("Log Window");
   setAppropriate(lidock, true);
   TGo4LogInfo* loginfo = new TGo4LogInfo(lidock, "LogInfo");
   ConnectGo4Widget(loginfo);
   lidock->setWidget(loginfo);
   addDockWindow(lidock, Qt::DockBottom);
   lidock->show();

   menuBar()->insertSeparator();
   QPopupMenu *HelpMenu = new QPopupMenu(this);
   menuBar()->insertItem( "&Help",HelpMenu);
   HelpMenu->insertItem("&Introduction (user manual)", this, SLOT(IntroHelpSlot()));
   HelpMenu->insertItem("&Reference manual", this, SLOT(RefHelpSlot()));
   HelpMenu->insertItem("&Fit Tutorial", this, SLOT(FitHelpSlot()));
   HelpMenu->insertSeparator();

   HelpMenu->insertItem( "About &Qt", this, SLOT(aboutQt()), Key_F2 );
   HelpMenu->insertItem( "About R&OOT", this, SLOT(aboutROOT()),Key_F3);
   HelpMenu->insertItem( "About &Go4", this, SLOT(about()), Key_F4);

   QStringList LibList = QStringList::split(":", getenv("GO4USERLIBRARY"));

   for (QStringList::Iterator it=LibList.begin(); it!=LibList.end(); ++it)
       gSystem->Load((*it).latin1());

   go4sett->RestoreSettings(this);

   go4sett->restoreGeometry(this);

   MakeStyleSlot(go4sett->getAppStyle());

   menuBar()->setItemChecked(fiCrosshairId, go4sett->getPadCrosshair());
   menuBar()->setItemChecked(fiEventstatusId, go4sett->getPadEventStatus());

   go4sett->getBasicSettings();

   // start mbs monitoring only after browser etc. is fully there:
   if(go4sett->getMbsMonitorMonitorActive()) mbs->TimerStart();

   go4sett->DumpSettingsLocation();
}

TGo4MainWindow::~TGo4MainWindow()
{
    TGo4Script::DeleteInstance();

    delete fxOM;
}

const char* TGo4MainWindow::LastTypedPassword() const
{
   return fLastPassword.latin1();
}


void TGo4MainWindow::about()
{
   QString GO4STR("Go4  ");
   GO4STR.append ( __GO4RELEASE__);
   QString GO4STR1 = GO4STR;
   GO4STR1.append("\n");
   GO4STR1.append("The Go4 (GSI Object Oriented On-line Off-line system)  is based on ROOT\n with the specific requirements of the low and medium energy nuclear and atomic\n physics experiments implemented as extensions.\n See  http://go4.gsi.de  for more information") ;

   QMessageBox AboutGo4(GO4STR, GO4STR1, QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutGo4.setIconPixmap(QPixmap::fromMimeSource( "go4logo2_big.png"));
   AboutGo4.exec();
}

void TGo4MainWindow::aboutQt()
{
   QMessageBox::aboutQt(this, "Qt");
}

void TGo4MainWindow::aboutROOT()
{
   QString mestring("This Go4 uses ROOT Version ");
   mestring.append(gROOT->GetVersion());
   mestring.append("\n ROOT is an object-oriented framework for large\n scale scientific data analysis and data mining.\n It has been developed at CERN\n See http://root.cern.ch for more information");
   QMessageBox AboutRoot("ROOT", mestring,
                         QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutRoot.setIconPixmap(QPixmap::fromMimeSource( "root.png"));
   AboutRoot.exec();
}

void TGo4MainWindow::AddSettingMenu()
{
   QPopupMenu* SettingMenu = new QPopupMenu( this );
   menuBar()->insertItem( "&Settings",SettingMenu);

   SettingMenu->insertItem("Sh&ow/hide", createDockWindowMenu());

   SettingMenu->insertItem("&Fonts...",this, SLOT(ChangeFontSlot()));
   QPopupMenu *style = new QPopupMenu( this );
   style->setCheckable( TRUE );
   SettingMenu->insertItem("St&yle", style);
//   menuBar()->insertItem( "St&yle",style);


   QPopupMenu* PrefMenu = new QPopupMenu( this );
   SettingMenu->insertItem( "&Preferences", PrefMenu);

   PrefMenu->insertItem("Fetch when drawing", this, SLOT(ChangeFetchWhenDrawSlot()), 0, fiFetchWhenDrawId);
   PrefMenu->setItemChecked(fiFetchWhenDrawId, go4sett->getFetchDataWhenDraw());
   PrefMenu->insertItem("Fetch when copying", this, SLOT(ChangeFetchWhenCopySlot()), 0, fiFetchWhenCopyId);
   PrefMenu->setItemChecked(fiFetchWhenCopyId, go4sett->getFetchDataWhenCopy());
   PrefMenu->insertItem("Fetch when saving", this, SLOT(ChangeFetchWhenSaveSlot()), 0, fiFetchWhenSaveId);
   PrefMenu->setItemChecked(fiFetchWhenSaveId, go4sett->getFetchDataWhenSave());

   QPopupMenu* PanelMenu = new QPopupMenu( this );
   SettingMenu->insertItem( "&Panel defaults", PanelMenu);

   PanelMenu->insertItem("&Canvas color...",this, SLOT(CanvasColorSlot()));
   PanelMenu->insertItem("Marker labels...", this, SLOT(MarkerSettingsSlot()));
   PanelMenu->insertItem("Statistics box...", this, SLOT(OptStatsSlot()));
   PanelMenu->insertItem("Cross(&X)hair mode", this, SLOT(CrosshairSlot()), 0, fiCrosshairId);
   PanelMenu->setItemChecked(fiCrosshairId, go4sett->getPadCrosshair());
   PanelMenu->insertItem("Show Event Status", this, SLOT(EventStatusSlot()), 0, fiEventstatusId);
   PanelMenu->setItemChecked(fiEventstatusId, go4sett->getPadEventStatus());

   PanelMenu->insertItem("Objects cloning", this, SLOT(ChangeCloneFlagSlot()), 0, fiCloneId);
   PanelMenu->setItemChecked(fiCloneId, go4sett->getCloneFlag());
   PanelMenu->insertItem("Draw time", this, SLOT(ChangeDrawTimeFlagSlot()), 0, fiDrawTimeId);
   PanelMenu->setItemChecked(fiDrawTimeId, go4sett->getDrawTimeFlag());
   PanelMenu->setItemEnabled(fiDrawTimeId, go4sett->getCloneFlag());
   PanelMenu->insertItem("Draw date", this, SLOT(ChangeDrawDateFlagSlot()), 0, fiDrawDateId);
   PanelMenu->setItemChecked(fiDrawDateId, go4sett->getDrawDateFlag());
   PanelMenu->setItemEnabled(fiDrawDateId, go4sett->getCloneFlag());
   PanelMenu->insertItem("Draw item name", this, SLOT(ChangeDrawItemFlagSlot()), 0, fiDrawItemId);
   PanelMenu->setItemChecked(fiDrawItemId, go4sett->getDrawItemFlag());
   PanelMenu->setItemEnabled(fiDrawItemId, go4sett->getCloneFlag());

   SettingMenu->insertItem("&Log actions...",this, SLOT(LogSettingsSlot()));

   SettingMenu->insertItem("Generate &hotstart", this, SLOT(CreateGUIScriptSlot()));
   SettingMenu->insertItem("&Break hotstart execution", this, SLOT(StopGUIScriptSlot()));

   SettingMenu->insertItem("&Terminal history", this, SLOT(InputTerminalParametersSlot()));

   SettingMenu->insertItem("&Save Settings", this, SLOT(SaveSettingsSlot()));

   QActionGroup *ag = new QActionGroup( this, 0 );
   ag->setExclusive( TRUE );
   QSignalMapper *styleMapper = new QSignalMapper( this );
   connect( styleMapper, SIGNAL( mapped( const QString& ) ), this, SLOT( MakeStyleSlot(const QString& ) ) );
   QStringList list = QStyleFactory::keys();
   list.sort();
   QDict<int> stylesDict( 117, FALSE );
   for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      QString styleStr = *it;
      if (styleStr == "Check") continue;

      QString styleAccel = styleStr;
      if ( stylesDict[styleAccel.left(1)] ) {
         for ( uint i = 0; i < styleAccel.length(); i++ ) {
            if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
               stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
               styleAccel = styleAccel.insert( i, '&' );
               break;
             }
          }
       } else {
          stylesDict.insert(styleAccel.left(1), (const int *)1);
          styleAccel = "&"+styleAccel;
       }
       QAction *a = new QAction( styleStr, QIconSet(), styleAccel, 0, ag, 0, ag->isExclusive() );
       connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );

       a->setToggleAction(true);
       if (go4sett->getAppStyle() == styleStr)
         a->setOn(true);

       styleMapper->setMapping( a, a->text() );
   }
   ag->addTo(style);
   QApplication::setColorSpec( QApplication::ManyColor );
}

void TGo4MainWindow::AddFileMenu()
{
    QPopupMenu *FileMenu = new QPopupMenu( this );
     menuBar()->insertItem( "&File",FileMenu);

     FileMenu->insertItem(QPixmap::fromMimeSource( "open.png" ), "&Open...",
             this, SLOT(OpenFileSlot()), CTRL+Key_O );
     FileMenu->insertItem(QPixmap::fromMimeSource( "network.png" ), "Open &Remote...",
             this, SLOT(OpenRemoteFileSlot()), CTRL+Key_R );
     FileMenu->insertItem(QPixmap::fromMimeSource( "histserv.png" ), "Open &HServer...",
             this, SLOT(ConnectHServerSlot()), CTRL+Key_H );
     FileMenu->insertItem(QPixmap::fromMimeSource( "filesave.png" ), "Save memor&y...",
             this, SLOT(SaveFileSlot()), CTRL+Key_Y );
     FileMenu->insertItem(QPixmap::fromMimeSource( "close.png" ),"Close all files",
             this, SLOT(CloseAllFilesSlot()), CTRL+Key_Q);
     FileMenu->insertItem(QPixmap::fromMimeSource( "exit.png" ), "E&xit",
             this, SLOT(close()), CTRL+Key_X );
}

void TGo4MainWindow::AddFileToolBar()
{
   QToolBar *FileBar = new QToolBar( this, "File Toolbar" );
   addDockWindow(FileBar, "File Toolbar", Qt::DockTop, TRUE );
   FileBar->setNewLine ( FALSE );

   QToolButton *FileOpenB  = new QToolButton( QPixmap::fromMimeSource( "open.png" ), QString::null, QString::null,
                    this, SLOT(OpenFileSlot()), FileBar, "Open Local File" );
   FileOpenB->setUsesTextLabel(FALSE);
   QToolTip::add(FileOpenB, "Open a local file from disk");

   QToolButton *FileRemoteOpenB = new QToolButton( QPixmap::fromMimeSource( "network.png" ), QString::null, QString::null,
                    this, SLOT(OpenRemoteFileSlot()), FileBar, "Open Remote File" );
   FileRemoteOpenB->setUsesTextLabel(FALSE);
   QToolTip::add(FileRemoteOpenB, "Open a remote file from server");

   QToolButton *HServB = new QToolButton( QPixmap::fromMimeSource( "histserv.png" ), QString::null, QString::null,
                    this, SLOT(ConnectHServerSlot()), FileBar, "Open Remote Histogram Server" );
   HServB->setUsesTextLabel(FALSE);
   QToolTip::add(HServB, "Connect to running HServer");

   QToolButton *FileSaveB  = new QToolButton( QPixmap::fromMimeSource( "filesave.png" ), QString::null, QString::null,
                    this, SLOT(SaveFileSlot()), FileBar, "Save File" );
   FileSaveB->setUsesTextLabel(FALSE);
   QToolTip::add(FileSaveB, "Save the content of the browser to a root file");

   QToolButton *FileAllCloseB  = new QToolButton( QPixmap::fromMimeSource( "close.png" ), QString::null, QString::null,
                    this, SLOT(CloseAllFilesSlot() ), FileBar, "Close All Files" );
   FileAllCloseB->setUsesTextLabel(FALSE);
   QToolTip::add(FileAllCloseB, "Close all root files in file browser");

   QToolButton *FileExitB  = new QToolButton( QPixmap::fromMimeSource( "exit.png" ), QString::null, QString::null,
                   this, SLOT(close()), FileBar, "Exit" );
   FileExitB->setUsesTextLabel(FALSE);
   QToolTip::add(FileExitB, "Exit the Go4");
}

void TGo4MainWindow::AddToolsMenu()
{
   QPopupMenu * Go4ToolMenu = new QPopupMenu(this);
   menuBar()->insertItem( "&Tools", Go4ToolMenu);

   Go4ToolMenu->insertItem(QPixmap::fromMimeSource( "chart.png" ), "&View Panel",
                this, SLOT(MakeNewPanel()), CTRL+Key_V );
   Go4ToolMenu->insertItem(QPixmap::fromMimeSource( "fitpanel.png" ), "&Fit Panel...",
                this, SLOT(StartFitPanel()), CTRL+Key_F );
   Go4ToolMenu->insertItem( QPixmap::fromMimeSource( "hislist.png" ),"&Histogram properties...",
               this, SLOT(StartHistogramInfo()));
   Go4ToolMenu->insertItem(QPixmap::fromMimeSource( "hiscre.png" ), "Create New H&is...",
               this, SLOT(CreateNewHistSlot()), CTRL+Key_I );
   Go4ToolMenu->insertItem( QPixmap::fromMimeSource( "condlist.png" ),"C&ondition properties...",
               this, SLOT(StartConditionInfo()));
   Go4ToolMenu->insertItem( QPixmap::fromMimeSource( "condcre.png" ),"&Condition Editor...",
               this, SLOT(CreateNewConditionSlot()), CTRL+Key_C);
   Go4ToolMenu->insertItem( QPixmap::fromMimeSource( "zoom.png" ),"&Event Printout...",
               this, SLOT(StartEventInfo()));
   Go4ToolMenu->insertItem( QPixmap::fromMimeSource( "dynlist.png" ),"Create &Dyn. List entry ...",
               this, SLOT(CreateNewDynEntrySlot()), CTRL+Key_D);
   Go4ToolMenu->insertItem(QPixmap::fromMimeSource( "dllicon.png" ), "Load Li&braries ...",
               this, SLOT(LoadLibrarySlot()), CTRL+Key_B);
   Go4ToolMenu->insertItem(QPixmap::fromMimeSource( "user.png" ), "&User GUI ...",
               this, SLOT(UserPanelSlot()), CTRL+Key_U);
}

void TGo4MainWindow::AddToolsBar()
{
   QToolBar *Go4ToolBar = new QToolBar( this, "File operations" );
   addDockWindow(Go4ToolBar, "Tools Toolbar", Qt::DockTop, TRUE );
   Go4ToolBar->setNewLine ( FALSE );

   QToolButton *ViewB  = new QToolButton( QPixmap::fromMimeSource( "chart.png" ), QString::null, QString::null,
                 this, SLOT(MakeNewPanel()), Go4ToolBar, "View Panel" );
   ViewB->setUsesTextLabel(FALSE);
   QToolTip::add(ViewB , "Create a new view panel");

   QToolButton *FitPanelB  = new QToolButton( QPixmap::fromMimeSource( "fitpanel.png" ), QString::null, QString::null,
                    this, SLOT(StartFitPanel()), Go4ToolBar, "Fit Panel" );
   FitPanelB->setUsesTextLabel(FALSE);
   QToolTip::add(FitPanelB , "Show/hide the Go4 Fit Panel");

   QToolButton *HisInfoB  = new QToolButton( QPixmap::fromMimeSource( "hislist.png" ), QString::null, QString::null,
                    this, SLOT(StartHistogramInfo()), Go4ToolBar, "Histogram properties");
   HisInfoB->setUsesTextLabel(FALSE);
   QToolTip::add(HisInfoB , "Show histogram properties window");

   QToolButton *CreatHisB  = new QToolButton( QPixmap::fromMimeSource( "hiscre.png" ), QString::null, QString::null,
                    this, SLOT(CreateNewHistSlot()), Go4ToolBar, "Create New Histogram" );
   CreatHisB->setUsesTextLabel(FALSE);
   QToolTip::add(CreatHisB , "Create new ROOT Histogram");

   QToolButton *ConInfoB  = new QToolButton( QPixmap::fromMimeSource( "condlist.png" ), QString::null, QString::null,
                    this, SLOT(StartConditionInfo()), Go4ToolBar, "Condition properties");
   ConInfoB->setUsesTextLabel(FALSE);
   QToolTip::add(ConInfoB , "Show condition properties window");

   QToolButton *ConEditB  = new QToolButton( QPixmap::fromMimeSource( "condcre.png" ), QString::null, QString::null,
                    this, SLOT(CreateNewConditionSlot()), Go4ToolBar, "Condition editor");
   ConEditB->setUsesTextLabel(FALSE);
   QToolTip::add(ConEditB , "Create new condition in analysis");

   QToolButton *EveInfoB  = new QToolButton( QPixmap::fromMimeSource( "zoom.png" ), QString::null, QString::null,
                    this, SLOT(StartEventInfo()), Go4ToolBar, "Event printout");
   EveInfoB->setUsesTextLabel(FALSE);
   QToolTip::add(EveInfoB , "Event printout and examination window");

   QToolButton *DynEditorB  = new QToolButton( QPixmap::fromMimeSource( "dynlist.png" ), QString::null, QString::null,
                    this, SLOT(CreateNewDynEntrySlot()), Go4ToolBar, "Create Dyn. List");
   DynEditorB->setUsesTextLabel(FALSE);
   QToolTip::add(DynEditorB , "Create new Dynamic list entry");

   QToolButton *LibB  = new QToolButton( QPixmap::fromMimeSource( "dllicon.png" ), QString::null, QString::null,
                    this, SLOT(LoadLibrarySlot() ), Go4ToolBar, "Load Libraries" );
   LibB->setUsesTextLabel(FALSE);
   QToolTip::add(LibB , "Load Libraries");

   QToolButton *UsrB  = new QToolButton( QPixmap::fromMimeSource( "user.png" ), QString::null, QString::null,
                    this, SLOT(UserPanelSlot()), Go4ToolBar, "User GUI" );
   UsrB->setUsesTextLabel(FALSE);
   QToolTip::add(UsrB , "Start user panel");
}

void TGo4MainWindow::AddAnalysisMenu()
{
   QPopupMenu * AnalysisMenu  = new QPopupMenu( this );
   menuBar()->insertItem( "&Analysis", AnalysisMenu);

   AnalysisMenu->insertItem(QPixmap::fromMimeSource( "launchanal.png" ), "Lau&nch analysis...",
           this, SLOT(LaunchClientSlot()), CTRL+Key_N, fiLaunchAnal);

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("connect.png"), "&Connect to running server...",
             this, SLOT(ConnectServerSlot()), CTRL+Key_C, fiConnectAnal);

   AnalysisMenu->insertItem(QPixmap::fromMimeSource( "connect.png" ), "&Prepare for client connection...",
             this, SLOT(PrepareForClientConnectionSlot()), CTRL+Key_C, fiPrepareAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("disconnect.png"), "Disconnect from analysis",
             this, SLOT(DisconnectAnalysisSlot()), CTRL+Key_M, fiDisconnectAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("shutanal.png"), "Shutdown analysis server",
             this, SLOT(ShutdownAnalysisSlot()), CTRL+Key_M, fiShutdownAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("restart.png"), "Submit+S&tart",
             this, SLOT(SubmitStartAnalysisSlot()), CTRL+Key_T, fiSumbStartAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("start.png"), "&Start",
             this, SLOT(StartAnalysisSlot()), CTRL+Key_S, fiStartAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("Stop.png"), "Stop (&Halt)",
             this, SLOT(StopAnalysisSlot()), CTRL+Key_H, fiStopAnal );

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("control.png"), "Confi&guration...",
               this, SLOT(ToggleAnalysisConfiguration()), CTRL+Key_G, fiAnalConfig);

   AnalysisMenu->insertItem(QPixmap::fromMimeSource("analysiswin.png"), "Analysis &Window",
               this, SLOT(ToggleAnalysisWindow()), CTRL+Key_W, fiAnalTermin);
}

void TGo4MainWindow::AddAnalysisBar()
{
   QToolBar *AnalysisToolBar = new QToolBar( this, "AnalysisToolBar" );
   addDockWindow(AnalysisToolBar, "Analysis Toolbar", Qt::DockTop, TRUE );
   AnalysisToolBar->setNewLine ( FALSE );
   QToolTip::add(AnalysisToolBar, "Analysis related tool buttons");

   QToolButton *LaunchB  = new QToolButton( QPixmap::fromMimeSource( "launchanal.png" ), QString::null, QString::null,
                   this, SLOT(LaunchClientSlot()), AnalysisToolBar, "Launch Analysis Button" );
   LaunchB->setUsesTextLabel(FALSE);
   QToolTip::add(LaunchB, "Launch an analysis");

   QToolButton *ConnectB  = new QToolButton( QPixmap::fromMimeSource( "connect.png" ), QString::null, QString::null,
                   this, SLOT(ConnectServerSlot()), AnalysisToolBar, "Connect Analysis Button" );
   ConnectB->setUsesTextLabel(FALSE);
   QToolTip::add(ConnectB, "Connect running analysis");

   QToolButton *DisconnectB  = new QToolButton( QPixmap::fromMimeSource( "disconnect.png" ), QString::null, QString::null,
                   this, SLOT(DisconnectAnalysisSlot()), AnalysisToolBar, "Disconnect Analysis Button" );
   DisconnectB->setUsesTextLabel(FALSE);
   QToolTip::add(DisconnectB, "Disconnect from running analysis");

   QToolButton *ShutdownB  = new QToolButton( QPixmap::fromMimeSource( "shutanal.png" ), QString::null, QString::null,
                  this, SLOT(ShutdownAnalysisSlot()), AnalysisToolBar, "Shutdown Analysis Button" );
   ShutdownB->setUsesTextLabel(FALSE);
   QToolTip::add(ShutdownB, "Shutdown running analysis");

   QToolButton *SetStartB  = new QToolButton( QPixmap::fromMimeSource( "restart.png" ), QString::null, QString::null,
                    this, SLOT(SubmitStartAnalysisSlot()), AnalysisToolBar, "Submit and Start Analysis" );
   SetStartB->setUsesTextLabel(FALSE);
   QToolTip::add(SetStartB, "Submit Settings and start analysis");

   QToolButton *StartB  = new QToolButton( QPixmap::fromMimeSource( "start.png" ), QString::null, QString::null,
                   this, SLOT(StartAnalysisSlot()), AnalysisToolBar, "Start Analysis" );
   StartB->setUsesTextLabel(FALSE);
   QToolTip::add(StartB, "Start analysis");

   QToolButton *StopB  = new QToolButton( QPixmap::fromMimeSource( "Stop.png" ), QString::null, QString::null,
                  this, SLOT(StopAnalysisSlot()), AnalysisToolBar, "Stop  Analysis" );
   StopB->setUsesTextLabel(FALSE);
   QToolTip::add(StopB, "Stop analysis");

   QToolButton *ConfigB  = new QToolButton( QPixmap::fromMimeSource( "control.png" ), QString::null, QString::null,
                  this, SLOT(ToggleAnalysisConfiguration()), AnalysisToolBar, "Analysis Configuration" );
   ConfigB->setUsesTextLabel(FALSE);
   QToolTip::add(ConfigB, "Show the analysis configuration");

   QToolButton *AnalysisWinB  = new QToolButton( QPixmap::fromMimeSource( "analysiswin.png" ), QString::null, QString::null,
                   this, SLOT(ToggleAnalysisWindow()), AnalysisToolBar, "Analysis Window" );
   AnalysisWinB->setUsesTextLabel(FALSE);
   QToolTip::add(AnalysisWinB, "Show the analysis window");
}

void TGo4MainWindow::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    int cascadeId = windowsMenu->insertItem("Ca&scade", centralWidget(), SLOT(cascade()));
    int tileId    = windowsMenu->insertItem("&Tile", centralWidget(), SLOT(tile()));
    int closallId = windowsMenu->insertItem("&Close all", this, SLOT(CloseAllWindows()));
    int minallId  = windowsMenu->insertItem("&Minimize all", this, SLOT(MinAllWindows()));
    if (fxTGo4WorkSpace->windowList().isEmpty()) {
        windowsMenu->setItemEnabled(cascadeId, FALSE);
        windowsMenu->setItemEnabled(tileId, FALSE);
        windowsMenu->setItemEnabled(closallId, FALSE);
        windowsMenu->setItemEnabled(minallId, FALSE);
    }

    QGo4Widget* loginfo = FindGo4Widget("LogInfo", false);
    QGo4Widget* anw = FindGo4Widget("AnalysisWindow", false);

    windowsMenu->insertSeparator();
    int savelogId  = windowsMenu->insertItem("Save L&ogwindow", loginfo, SLOT(SaveLogInfo()));
    int clearlogId = windowsMenu->insertItem("Clear &Logwindow", loginfo, SLOT(ClearLogInfo()));
    int saveAnaId  = windowsMenu->insertItem("Save &Analysis window", anw, SLOT(SaveAnalysisOutput()));
    int clearAnaId = windowsMenu->insertItem("Clear Analysis &window", anw, SLOT(ClearAnalysisOutput()));
    windowsMenu->setItemEnabled(clearlogId, loginfo!=0);
    windowsMenu->setItemEnabled(savelogId, loginfo!=0);
    windowsMenu->setItemEnabled(saveAnaId, anw!=0);
    windowsMenu->setItemEnabled(clearAnaId, anw!=0);

    windowsMenu->insertSeparator();
    QWidgetList windows =  fxTGo4WorkSpace->windowList();
    for (int i=0; i<int(windows.count()); ++i ) {
        int id = windowsMenu->insertItem(windows.at(i)->caption(),
                                         this, SLOT( windowsMenuActivated(int)));
        windowsMenu->setItemParameter(id, i);
        windowsMenu->setItemChecked(id, fxTGo4WorkSpace->activeWindow() == windows.at(i));
    }
}

void TGo4MainWindow::CloseAllWindows()
{
   QWidgetList windows =  fxTGo4WorkSpace->windowList();
    for ( int i = 0; i < int(windows.count()); ++i )
       windows.at(i)->close();
}

void TGo4MainWindow::MinAllWindows()
{
   QWidgetList windows =  fxTGo4WorkSpace->windowList();
   for ( int i = 0; i < int(windows.count()); ++i )
       windows.at(i)->showMinimized();
}

void TGo4MainWindow::windowsMenuActivated( int id )
{
   QWidget* w = fxTGo4WorkSpace->windowList().at(id);
   if (w) {
      w->showNormal();
      w->setFocus();
   }

}

typedef void* (*TStartUserGuiFunc)(QWidget* parent);

bool TGo4MainWindow::startUserGUI(const char* usergui)
{
   QString dirname, libname;

   bool result = false;

   if ((usergui!=0) && (strlen(usergui)>0)) {
      QFileInfo info(usergui);
      if (info.exists())
        if (info.isFile()) {
           dirname = info.dirPath();
           libname = info.fileName();

        } else
        if (info.isDir()) {
           dirname = usergui;
        }
     if ((dirname.length()>0) && (dirname[dirname.length()-1]!='/')) dirname+="/";
   }

   bool defaultnames = libname.length()==0;
   if (defaultnames)
     libname = "libGo4UserGui.so";

   cout << "Try : " << dirname+libname << endl;
   void* lib1 = ::dlopen(dirname+libname, RTLD_LAZY | RTLD_GLOBAL);

   if (lib1==0) {
      const char* errmsg = ::dlerror();
      if (errmsg!=0) cout << errmsg << endl;
      return false;
   }

   TStartUserGuiFunc startfunc = (TStartUserGuiFunc) ::dlsym(lib1,"StartUserPanel");

   if (startfunc!=0) {
     QGo4Widget* userpanel = (QGo4Widget*) startfunc(centralWidget());
     if (userpanel!=0) {
        userpanel->setName("UserPanel");
        ConnectGo4Widget(userpanel);
        userpanel->show();
        result = true;
     } else
       StatusMessage("StartUserPanel() function didnot create user widget");
   }

   if (result) {
     cout << "Start user GUI from ";
     if (dirname.length()>0)
        cout << dirname << endl;
     else
        cout << "$LD_LIBRARY_PATH=" << ::getenv("LD_LIBRARY_PATH") << endl;
   } else {
      if (lib1!=0) dlclose(lib1);
   }

   return result;
}

void TGo4MainWindow::UserPanelSlot()
{
   QGo4Widget* userpanel = FindGo4Widget("UserPanel", true);
   if (userpanel!=0) return;

   // check GO4USERGUI variable
   TString usergui = gSystem->Getenv("GO4USERGUI");
   if (usergui.Length()>0)
     if (startUserGUI(usergui.Data())) return;

   // check from standard LD_LIBRARY_PATHS
   if (startUserGUI(0)) return;

   usergui = TGo4Log::subGO4SYS("qt3/Go4UserGUI");
   if (startUserGUI(usergui.Data())) return;

   QMessageBox::critical(this,"Starting user GUI", "No suitable libraries found");
}


TGo4ViewPanel* TGo4MainWindow::MakeNewPanel(int ndiv)
{
   QString name;

   TGo4Slot* edslot = fxOM->GetSlot(fOMEditorsPath);

   int n = 0;
   do {
     n++;
     name = QString("Panel") + QString::number(n);
   } while ((edslot!=0) && (edslot->FindChild(name.latin1())!=0));

   TGo4ViewPanel* panel = new TGo4ViewPanel(centralWidget(), name);
   ConnectGo4Widget(panel);
   panel->CompleteInitialization();

   panel->polish();
   panel->show();

   panel->GetCanvas()->Resize();

   if(ndiv>1) {

      int DivX(1), DivY(1);

      TGo4DividePad* divpad = (TGo4DividePad*) child("DividePad");

      if (divpad!=0) {
         DivX = divpad->SpinBoxX->value();
         DivY = divpad->SpinBoxY->value();
      }
      if (DivX * DivY < ndiv) {
         DivX = (int) TMath::Sqrt(ndiv*1.);
         DivY = DivX;
         while (DivX*DivY<ndiv) DivY++;
      }
      panel->Divide(DivX, DivY);
   }

   return panel;
}

void TGo4MainWindow::closeEvent( QCloseEvent* ce)
{
   if (fCloseCounter!=0) return;

   if(QMessageBox::warning( this, "Go4 GUI", "Really Exit Go4?",
         QMessageBox::Yes ,
         QMessageBox::No | QMessageBox::Escape) != QMessageBox::Yes ) return;


   CloseAllWindows();

   Browser()->ToggleMonitoring(0);

   CloseAllFilesSlot();

   if(!RemoveAnalysisProxy(30)) {
      fCloseCounter = 100; // gui waits about 10 second to close analysis
      statusBar()->message("Exit....  please wait");
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
   } else {
      statusBar()->message("Closing GUI...");

      ce->accept();
   }
}

void TGo4MainWindow::ForseCloseSlot()
{
   if (!RemoveAnalysisProxy(30) && (fCloseCounter>0)) {
      fCloseCounter--;
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
      return;
   }
   delete this;
}

void TGo4MainWindow::OpenFileSlot()
{
   QFileDialog fd( this, "Read Root File", TRUE );
   fd.setCaption("Select a ROOT file to open it in the Go4 Disk Browser");
   fd.setMode( QFileDialog::ExistingFiles);
   fd.setName( "Open Root File ");
   fd.setFilters(QString("Root files (*.root);;Root xml files (*.xml);;All files (*.*)"));
   if (fLastFileDir.length()>0)
     fd.setDir(QDir(fLastFileDir));
   if ( fd.exec() != QDialog::Accepted ) return;

   QStringList list = fd.selectedFiles();
   QStringList::Iterator it = list.begin();
   while( it != list.end() ) {
      QString fileName = *it;
      fLastFileDir = QFileInfo(fileName).dirPath(true);
      Browser()->OpenFile(fileName.latin1());
      ++it;
   }
}

void TGo4MainWindow::OpenRemoteFileSlot()
{
   TGo4OpenRemoteDialog fd( this, "Open Remote root file", TRUE );
   if (fd.exec() != QDialog::Accepted) return;

   QString ftype = fd.GetFileType();
   QString host = fd.GetServer();

   QString urlpath;
   if(ftype.contains("root:"))
      urlpath = "root://" + host + "/";
   else
   if (ftype.contains("rfio:")) {
      if(!host.isEmpty())
        urlpath="rfio:" + host + ":/";
      else
        urlpath="rfio:/";
   } else
   if (ftype.contains("http:")) {
      if(!host.isEmpty())
         urlpath="http://" + host + "/";
      else
         urlpath="http://";
   }

   QString fileName = fd.GetFileName();

   if(!fileName.isEmpty() && !fileName.contains(".root"))
      fileName.append(".root");

   fileName = urlpath + fileName;

   fxOM->AddFile(fOMDataPath.latin1(), fileName.latin1());
}

void TGo4MainWindow::ConnectHServerSlot()
{
   TGo4HServerConnection dlg( this, "Connect to remote HServer", TRUE );

   dlg.ServerName->setText(go4sett->getHServName());
   dlg.BaseName->setText(go4sett->getHServBase());
   dlg.FilterList->setText(go4sett->getHServFilter());
   dlg.PortNumberSpin->setValue(go4sett->getHServPort());

   if (dlg.exec() != QDialog::Accepted) return;

   go4sett->setHServName(dlg.ServerName->text());
   go4sett->setHServBase(dlg.BaseName->text());
   go4sett->setHServFilter(dlg.FilterList->text());
   go4sett->setHServPort(dlg.PortNumberSpin->value());

   if (!Browser()->ConnectHServer(dlg.ServerName->text().latin1(),
                                  dlg.PortNumberSpin->value(),
                                  dlg.BaseName->text().latin1(),
                                  dlg.UserPassEdt->text().latin1(),
                                  dlg.FilterList->text().latin1()))
     QMessageBox::warning(0, "HServer", "Cannot connect to histogram server");
}

void TGo4MainWindow::SaveFileSlot()
{
   QFileDialog fd(this, "export", kTRUE );
   fd.setMode( QFileDialog::AnyFile);
   if (go4sett->getFetchDataWhenSave())
      fd.setCaption("Select root file to fetch and export all browser objects");
   else
      fd.setCaption("Select root file to export all local browser objects");

   fd.setFilters(QString("ROOT (*.root)"));
   fd.setSelection("export.root");
   if (fLastFileDir.length()>0)
      fd.setDir(QDir(fLastFileDir));

   if (fd.exec() != QDialog::Accepted) return;

   QString fname = fd.selectedFile();
   fLastFileDir = QFileInfo(fname).dirPath(true);
   if (fname.find(".root", 0, FALSE)<0) fname+=".root";

   if (!Browser()->SaveBrowserToFile(fname.latin1(), go4sett->getFetchDataWhenSave()))
     QMessageBox::warning(this, "Save data to file", "Specified file can not be created");
}

void TGo4MainWindow::CloseAllFilesSlot()
{
   fxOM->CloseFiles(fOMDataPath.latin1());
}

void TGo4MainWindow::InputTerminalParametersSlot()
{
   int size = go4sett->getTermHistorySize();

   bool ok;

   size = QInputDialog::getInteger("Analysis terminal history size in bytes (minimum 10000 bytes)",
                                   "Input 0 if full analysis history should be preserved",
                                   size, 0, 100000000, 10000, &ok, this);

   if ((size>0) && (size<10000)) size = 10000;

   if (ok) {
     go4sett->setTermHistorySize(size);
     TGo4AnalysisWindow* anw = FindAnalysisWindow();
     if (anw!=0)
       anw->SetHistorySize(size);
   }
}

void TGo4MainWindow::LogSettingsSlot()
{
   TGo4LogSettings dlg( this, "Logging setup for GUI:", TRUE );
   if (dlg.exec() != QDialog::Accepted) return;

   if(dlg.LogfileWrite->isChecked())
      TGo4Log::OpenLogfile(dlg.LogfileName->text(), "Logfile for Go4 GUI", true);

   TGo4Log::LogfileEnable(dlg.LogfileWrite->isChecked());
   TGo4Log::OutputEnable(dlg.LogfilePrint->isChecked());
   TGo4Log::SetIgnoreLevel(dlg.IgnoreCombo->currentItem());
   TGo4Log::AutoEnable(dlg.ModeCombo->currentItem());
}

void TGo4MainWindow::HelpWindow(const char* filename, const char* msg)
{
   QApplication::setOverrideCursor( Qt::WaitCursor );


   QString cmd = TGo4Log::subGO4SYS("etc/Go4ShowPdf.sh").Data();

   QProcess info(cmd);
   info.addArgument(TGo4Log::subGO4SYS(filename).Data());
   bool done = false;

   if (info.start()) {
      int cnt = 5;
      while (cnt-->0) {
         if (info.normalExit()) {
            done = info.exitStatus()==0;
            break;
         }
         sleep(1);
      }
   }

   if (done)
      StatusMessage(msg ? QString(msg) : QString("Show ") + filename);
   else
      StatusMessage(QString("Fail to display") + filename);

   QApplication::restoreOverrideCursor();
}

void  TGo4MainWindow::IntroHelpSlot()
{
   HelpWindow("docs/Go4Introduction.pdf", "Show Go4 Introduction manual...");
}

void  TGo4MainWindow::RefHelpSlot()
{
   HelpWindow("docs/Go4Reference.pdf", "Show Go4 Reference manual...");
}

void TGo4MainWindow::FitHelpSlot()
{
   HelpWindow("docs/Go4FitTutorial.pdf", "Show Go4 Fit tutorial...");
}

void TGo4MainWindow::SaveSettingsSlot()
{
   go4sett->storeGeometry(this);

   go4sett->setBasicSettings();

   go4sett->setAppStyle(QApplication::style().name());

   go4sett->StoreSettings(this);

   go4sett->DumpSettingsLocation();

   delete go4sett;

   go4sett = new TGo4QSettings;
}

void TGo4MainWindow::ChangeFontSlot()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, QApplication::font(), this);
    if (ok)
       QApplication::setFont( font, true );
}

void TGo4MainWindow::MakeStyleSlot(const QString &style)
{
   QApplication::setStyle(style);
   if(style == "Platinum") {
       QPalette p( QColor( 239, 239, 239 ) );
       QApplication::setPalette( p, TRUE );
       QApplication::setFont(QApplication::font(), TRUE);
   } else if(style == "Windows") {
       QApplication::setFont(QApplication::font(), TRUE);
   } else if(style == "CDE") {
       QPalette p( QColor( 75, 123, 130 ) );
       p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
       p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
       p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
       p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
       p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
       p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
       p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
       p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
       p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
       p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
       p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
       p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
       p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
       QApplication::setPalette( p, TRUE );
       QApplication::setFont( QFont( "times", QApplication::font().pointSize() ), TRUE );
   } else if(style == "Motif" || style == "MotifPlus") {
       QPalette p( QColor( 192, 192, 192 ) );
       QApplication::setPalette( p, TRUE );
       QApplication::setFont(QApplication::font(), TRUE );
   }
}

void TGo4MainWindow::StatusMessage(const QString& mess)
{
   QString output = QDateTime::currentDateTime().toString(Qt::ISODate);
   output.append(": ");
   output.append(mess);
   statusBar()->message(output, 3000);
   if(TGo4Log::IsAutoEnabled())
      TGo4Log::Message(1,output);

   UpdateCaptionButtons();
}

void TGo4MainWindow::UpdateCaptionButtons()
{
   TGo4AnalysisProxy* pr = Browser()->FindAnalysis();

   QString capt = "Go4 ";
   capt += __GO4RELEASE__;
   capt += " @";
   capt += gSystem->HostName();
   if ((pr!=0) && pr->IsConnected()) {
      capt += " <";
      capt += pr->GetContainedObjectInfo();
      capt += ">";
   }
   setCaption(capt);

   bool flag = (pr==0);
   QWidget* w1 = dynamic_cast<QWidget*> (child("Launch Analysis Button"));
   if (w1!=0) w1->setEnabled(flag);
   menuBar()->setItemEnabled(fiLaunchAnal, flag);

   if (pr==0) flag = (pr==0);
         else flag = (fConnectingCounter<=0) && pr->IsAnalysisServer() && !pr->IsConnected();
   QWidget* w2 = dynamic_cast<QWidget*> (child("Connect Analysis Button"));
   if (w2!=0) w2->setEnabled(flag);
   menuBar()->setItemEnabled(fiConnectAnal, flag);

   menuBar()->setItemEnabled(fiPrepareAnal, flag);

   if (pr==0) flag = false;
         else flag = pr->IsAnalysisServer() &&
                     (pr->IsConnected() || (fConnectingCounter<=0));
   QWidget* w3 = dynamic_cast<QWidget*> (child("Disconnect Analysis Button"));
   if (w3!=0) w3->setEnabled(flag);
   menuBar()->setItemEnabled(fiDisconnectAnal, flag);

   if (pr==0) flag = false;
         else flag = !pr->IsAnalysisServer() ||
                     (pr->IsConnected() && pr->IsAdministrator());
   QWidget* w4 = dynamic_cast<QWidget*> (child("Shutdown Analysis Button"));
   if (w4!=0) w4->setEnabled(flag);
   menuBar()->setItemEnabled(fiShutdownAnal, flag);


   bool iscontrolling = false;
   if (pr!=0)
     iscontrolling = pr->IsConnected() && (pr->IsAdministrator() || pr->IsController());

   QWidget* ww = dynamic_cast<QWidget*> (child("Submit and Start Analysis"));
   if (ww!=0) ww->setEnabled(iscontrolling);
   menuBar()->setItemEnabled(fiSumbStartAnal, iscontrolling);

   ww = dynamic_cast<QWidget*> (child("Start Analysis"));
   if (ww!=0) ww->setEnabled(iscontrolling);
   menuBar()->setItemEnabled(fiStartAnal, iscontrolling);

   ww = dynamic_cast<QWidget*> (child("Stop  Analysis"));
   if (ww!=0) ww->setEnabled(iscontrolling);
   menuBar()->setItemEnabled(fiStopAnal, iscontrolling);

   ww = dynamic_cast<QWidget*> (child("Analysis Configuration"));
   if (ww!=0) ww->setEnabled(iscontrolling);
   menuBar()->setItemEnabled(fiAnalConfig, iscontrolling);

   flag = (FindAnalysisWindow()!=0);
   ww = dynamic_cast<QWidget*> (child("Analysis Window"));
   if (ww!=0) ww->setEnabled(flag);
   menuBar()->setItemEnabled(fiAnalTermin, flag);
}

void TGo4MainWindow::ChangeFetchWhenDrawSlot()
{
   bool s = !menuBar()->isItemChecked(fiFetchWhenDrawId);
   menuBar()->setItemChecked(fiFetchWhenDrawId, s);
   go4sett->setFetchDataWhenDraw(s);
}

void TGo4MainWindow::ChangeFetchWhenCopySlot()
{
   bool s = !menuBar()->isItemChecked(fiFetchWhenCopyId);
   menuBar()->setItemChecked(fiFetchWhenCopyId, s);
   go4sett->setFetchDataWhenCopy(s);
}

void TGo4MainWindow::ChangeFetchWhenSaveSlot()
{
   bool s = !menuBar()->isItemChecked(fiFetchWhenSaveId);
   menuBar()->setItemChecked(fiFetchWhenSaveId, s);
   go4sett->setFetchDataWhenSave(s);
}

void TGo4MainWindow::CanvasColorSlot()
{
   QColor c = QColorDialog::getColor();
   if (c.isValid())
      go4sett->setCanvasColor(c.red(), c.green(), c.blue());
}

void TGo4MainWindow::MarkerSettingsSlot()
{
   TGo4MarkerSettings mset(this, "Marker labels set up", true);
   if (mset.exec() != QDialog::Accepted) return;
   // all work is done inside dialog.
}

void TGo4MainWindow::OptStatsSlot()
{
   TGo4OptStatsSettings stset(this, "Histogram statistics set up", true);
   if (stset.exec() != QDialog::Accepted) return;
   // all work is done inside dialog.
}



void TGo4MainWindow::CrosshairSlot()
{
   bool s = !menuBar()->isItemChecked(fiCrosshairId);
   menuBar()->setItemChecked(fiCrosshairId, s);
   go4sett->setPadCrosshair(s);
}

void TGo4MainWindow::EventStatusSlot()
{
   bool s = !menuBar()->isItemChecked(fiEventstatusId);
   menuBar()->setItemChecked(fiEventstatusId, s);
   go4sett->setPadEventStatus(s);
}

void TGo4MainWindow::ChangeCloneFlagSlot()
{
   bool s = !menuBar()->isItemChecked(fiCloneId);
   menuBar()->setItemChecked(fiCloneId, s);
   menuBar()->setItemEnabled(fiDrawTimeId, s);
   menuBar()->setItemEnabled(fiDrawDateId, s);
   menuBar()->setItemEnabled(fiDrawItemId, s);
   go4sett->setCloneFlag(s);
}

void TGo4MainWindow::ChangeDrawTimeFlagSlot()
{
   bool s = !menuBar()->isItemChecked(fiDrawTimeId);
   menuBar()->setItemChecked(fiDrawTimeId, s);
   go4sett->setDrawTimeFlag(s);
}

void TGo4MainWindow::ChangeDrawItemFlagSlot()
{
   bool s = !menuBar()->isItemChecked(fiDrawItemId);
   menuBar()->setItemChecked(fiDrawItemId, s);
   go4sett->setDrawItemFlag(s);
}

void TGo4MainWindow::ChangeDrawDateFlagSlot()
{
   bool s = !menuBar()->isItemChecked(fiDrawDateId);
   menuBar()->setItemChecked(fiDrawDateId, s);
   go4sett->setDrawDateFlag(s);
}


void TGo4MainWindow::LaunchClientSlot(bool interactive)
{
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal!=0) {
      QMessageBox::warning(this, "Launch analysis", "Please disconnect analysis first");
      return;
   }

   if (interactive) {
      TGo4StartClient dlg;
      if (dlg.exec()!=QDialog::Accepted) return;
      dlg.getResults();
   }

   int shellmode = go4sett->getClientShellMode();
   int termmode = go4sett->getClientTermMode();
   bool isserver = go4sett->getClientIsServer();

   TString launchcmd, killcmd;
   Bool_t res = kFALSE;

   QString workdir = go4sett->getClientDir();

   if (!isserver) {
      TGo4AnalysisProxy* anal = AddAnalysisProxy(false, (termmode==1));
      if (anal!=0)
        res = anal->LaunchAsClient(launchcmd, killcmd,
                      shellmode,
                      termmode,
                      go4sett->getClientName().latin1(),
                      go4sett->getClientNode().latin1(),
                      workdir.latin1(),
                      go4sett->getClientExec().latin1(),
                      go4sett->getClientExeMode());
      TGo4AnalysisWindow* anw = FindAnalysisWindow();
      if (res && (anw!=0) && (termmode==1))
         anw->StartAnalysisShell(launchcmd.Data(), (shellmode==0) ? workdir.latin1() : 0);
   } else
      res = TGo4AnalysisProxy::LaunchAsServer(launchcmd, killcmd,
                      shellmode,
                      termmode,
                      go4sett->getClientName().latin1(),
                      go4sett->getClientNode().latin1(),
                      workdir.latin1(),
                      go4sett->getClientExec().latin1(),
                      go4sett->getClientExeMode());

   if (res) fKillCommand = killcmd.Data();
       else fKillCommand = "";

   StatusMessage("Starting Analysis....  Please wait");

   if (isserver && interactive)
     ConnectServerSlot(true, "");
}

void TGo4MainWindow::PrepareForClientConnectionSlot()
{
   TGo4AnalysisProxy* ana = AddAnalysisProxy(false, false);
   if (ana!=0)
     QMessageBox::information(this,"Prepare for client connection",
      "Now you can start client from other shell (node) and connect to port " +
       QString::number(ana->ConnectorPort()), "Ok");
}

TGo4AnalysisProxy* TGo4MainWindow::AddAnalysisProxy(bool isserver, bool needoutput)
{
   const char* analisysitem = "Analysis";

   TGo4Slot* analslot = Browser()->DataSlot(analisysitem);

   if (analslot==0) {
      TGo4AnalysisProxy* anal = new TGo4AnalysisProxy(isserver);
      fxOM->AddProxy(fOMDataPath.latin1(), anal, analisysitem, "Analysis proxy");
      analslot = Browser()->DataSlot(analisysitem);
   }

   TGo4AnalysisProxy* anal = (analslot==0) ? 0 :
      dynamic_cast<TGo4AnalysisProxy*> (analslot->GetProxy());

   if (anal==0) return 0;

   anal->SetAnalysisReady(kFALSE);

   TGo4LogInfo* loginfo = (TGo4LogInfo*) FindGo4Widget("LogInfo", false);
   if (loginfo!=0)
     loginfo->WorkWithInfo(anal->LoginfoSlot());

   if (anal->IsAnalysisServer()) {
      EstablishRatemeter(anal->IsConnected() ? 2 : 0);
   } else
      EstablishRatemeter(1);

   EstablishAnalysisConfiguration(2);

   FindAnalysisConfiguration();

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0)
    if (anw->HasOutput() && !needoutput) {
       delete anw;
       anw = 0;
    }

   if(anw==0)
     if (needoutput) {
        anw = new TGo4AnalysisWindow(centralWidget(), "AnalysisWindow", true);
        ConnectGo4Widget(anw);
        anw->show();
        anw->WorkWithUpdateObjectCmd(anal->UpdateObjectSlot());
     } else {
        UpdateDockAnalysisWindow();
     }

   return anal;
}

bool TGo4MainWindow::RemoveAnalysisProxy(int waittime, bool servershutdown)
{
   EstablishAnalysisConfiguration(0);

   EstablishRatemeter(0);

   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal!=0)
      anal->DisconnectAnalysis(waittime, servershutdown);

   return Browser()->FindAnalysis()==0;
}

void TGo4MainWindow::UpdateDockAnalysisWindow()
{
   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();

   if ((anw!=0) && anw->HasOutput()) return;

   bool shouldexists = false;
   if (anal!=0)
     if (anal->IsAnalysisServer()) {
       if (anal->IsConnected() && (anal->IsController() || anal->IsAdministrator()))
         shouldexists = true;
     } else
       shouldexists = true;

   if (shouldexists && (anw==0)) {
     QDockWindow* dock = new QDockWindow();
     dock->setResizeEnabled(FALSE);
     dock->setCaption("AnalysisControls");
     setAppropriate(dock, true);
     bool showkill = !anal->IsAnalysisServer() || fKillCommand.length()>0;
     anw = new TGo4AnalysisWindow(dock, "AnalysisWindow", false, showkill);
     ConnectGo4Widget(anw);
     dock->setWidget(anw);
     addDockWindow(dock, Qt::DockTop);
     dock->show();
     anw->WorkWithUpdateObjectCmd(anal->UpdateObjectSlot());
   }
}

TGo4AnalysisStatusMonitor* TGo4MainWindow::EstablishRatemeter(int level)
// level = 0 - delete
// level = 1 - as is
// level = 2 - create
{
   TGo4AnalysisStatusMonitor* status =
     dynamic_cast<TGo4AnalysisStatusMonitor*>
      (statusBar()->child("AnalysisStatusMonitor"));

   if (level==2) {
     if (status==0) {
        status = new TGo4AnalysisStatusMonitor(statusBar(), "AnalysisStatusMonitor");
        ConnectGo4Widget(status);
        statusBar()->addWidget(status);
        TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
        if (anal!=0)
          status->WorkWithRatemeter(anal->RatemeterSlot());
     }
   } else
   if (level==0) {
      if (status!=0) {
         statusBar()->removeWidget(status);
         delete status;
      }
      status = 0;
   }

   if (status!=0)
      status->show();

   return status;
}

TGo4AnalysisConfiguration* TGo4MainWindow::EstablishAnalysisConfiguration(int level)
{
   // level: 0 - delete, 1 - keep as is, 2 - create, 3- create + request status

   TGo4AnalysisConfiguration* conf = FindAnalysisConfiguration();

   if (level==0) {
      if (conf!=0) {
         delete conf;
         conf = 0;
      }
   } else
   if (level>=2) {
     if (conf==0) {
       conf = new TGo4AnalysisConfiguration(centralWidget(), "AnalysisConfiguration");
       ConnectGo4Widget(conf);
     }
     TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
     if (anal!=0) {
       conf->WorkWithAnalysis(anal);
       if (level>2) anal->RequestAnalysisSettings();
     }
   }

   return conf;
}


void TGo4MainWindow::ConnectServerSlot(bool interactive, const char* password)
{
   if (fConnectingCounter>0) return;

   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal!=0) {
      if (anal->IsConnected() || !anal->IsAnalysisServer()) {
         if (interactive)
           QMessageBox::warning(this, "Connect client", "Analysis task is already connected");
         return;
      }
   }

   QString pass = password;
   if (interactive) {
      TGo4ConnectServer dlg;
      if (fLastPassword.length()>0)
        dlg.setPassword(fLastPassword.latin1());
      if (dlg.exec()!=QDialog::Accepted) return;
      pass = dlg.getInput();
   }

   if (anal==0) anal = AddAnalysisProxy(true, false);
   bool def = go4sett->getClientDefaultPass();
   if (!def) fLastPassword = pass;

   if (anal!=0)
     anal->ConnectToServer(go4sett->getClientNode().latin1(),
                           go4sett->getClientPort(),
                           go4sett->getClientControllerMode(),
                           def ? 0 : pass.latin1());
   StatusMessage("Connecting running analysis....  Please wait");

   // wait about 4 sec that analysis is connected
   fConnectingCounter = 41;
   UpdateCaptionButtons();
   CheckConnectingCounterSlot();
}

void TGo4MainWindow::CheckConnectingCounterSlot()
{
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();

   if ((anal==0) || anal->IsConnected() || (--fConnectingCounter<=0)) {
      if (fConnectingCounter<=0)
        StatusMessage("Analysis refused connection. Try again");
      fConnectingCounter = 0;
      EstablishRatemeter((anal!=0) && anal->IsConnected() ? 2 : 0);
      EstablishAnalysisConfiguration((anal!=0) && anal->IsConnected() && (anal->IsController() || anal->IsAdministrator()) ? 3 : 0);
      UpdateDockAnalysisWindow();
      UpdateCaptionButtons();
      if ((anal!=0) && !anal->IsConnected()) RemoveAnalysisProxy(1);
      return;
   }
   QTimer::singleShot(100, this, SLOT(CheckConnectingCounterSlot()));
}

void TGo4MainWindow::DisconnectAnalysisSlot(bool interactive)
{
   if (interactive) {
      int res = QMessageBox::warning(this, "Disconnect analysis",
                QString("Really disconnect from analysis task?"),
                QString("Disconnect"),
                QString("Cancel"),
                QString::null, 0);
      if (res!=0) return;
   }
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal==0) return;

   Browser()->ToggleMonitoring(0);

   RemoveAnalysisProxy(30, false);

   StatusMessage("Disconnect analysis");
}

void TGo4MainWindow::ShutdownAnalysisSlot(bool interactive)
{
   if (interactive) {
      int res = QMessageBox::warning(this, "Shutdown analysis",
                QString("Really shutdown analysis task?"),
                QString("Shutdown"),
                QString("Cancel"),
                QString::null, 0);
      if (res!=0) return;
   }

   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal==0) return;

   Browser()->ToggleMonitoring(0);

   bool realshutdown = anal->IsAnalysisServer() &&
                       anal->IsConnected() &&
                       anal->IsAdministrator();

   RemoveAnalysisProxy(30, realshutdown);

   StatusMessage("Shutdown analysis");
}

void TGo4MainWindow::SubmitAnalysisSettings()
{
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   if (anal==0) return;

   if (anal!=0) {
      anal->SubmitAnalysisSettings();
      anal->RefreshNamesList();
   }
   StatusMessage("Press Ctrl+S or choose Analysis->Start from the Menu to start the analysis");
}

void TGo4MainWindow::SubmitStartAnalysisSlot()
{
   SubmitAnalysisSettings();
   StartAnalysisSlot();
}

void TGo4MainWindow::StartAnalysisSlot()
{
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();

   if (anal==0) return;

   anal->StartAnalysis();
   anal->RefreshNamesList();
   anal->DelayedRefreshNamesList(4);

   EstablishRatemeter(2);
}

void TGo4MainWindow::StopAnalysisSlot()
{
   TGo4AnalysisProxy* anal = Browser()->FindAnalysis();

   if (anal!=0)
      anal->StopAnalysis();
}

void TGo4MainWindow::TerminateAnalysis()
{
   int res = QMessageBox::warning(this, "Kill analysis process",
             QString("Kill analysis by shell command: ") +fKillCommand + " ?",
             QString("Kill"),
             QString("Cancel"),
             QString::null, 0);
   if (res!=0) return;

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0) anw->TerminateAnalysisProcess();

   // proxy will be deleted after 7 second
   //RemoveAnalysisProxy(7);

   anw = FindAnalysisWindow();
   if (anw!=0)
      anw->AppendOutputBuffer(QString("\nKilling analysis client: \n  ")+fKillCommand);
   else
      StatusMessage(QString("Killing analysis client with: ")+fKillCommand);

   if (fKillCommand.length()>0) {
      QProcess* killprocess = new QProcess;
      TGo4AnalysisWindow::SetProcessArgs(killprocess, fKillCommand);
//      killprocess->setArguments(QStringList::split(" ",fKillCommand));
      if (!killprocess->start())
        StatusMessage("Can not start kill command");
      QTimer::singleShot(10000, killprocess, SLOT(deleteLater()));
   } else
      StatusMessage("Can not kill analysis. Do it by OS commands");

   EstablishAnalysisConfiguration(0);

   EstablishRatemeter(0);
}

QGo4Widget* TGo4MainWindow::FindGo4Widget(const char* name, bool activate)
{
   if (fxOM==0) return 0;

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.latin1());

   TGo4Slot* widgslot = slot==0 ? 0 : slot->FindChild(name);
   if (widgslot==0) return 0;

   TGo4WidgetProxy* wproxy = (TGo4WidgetProxy*) widgslot->GetProxy();

   QGo4Widget* widg = wproxy->GetWidget();
   if ((widg!=0) && activate) {
     if(!widg->isVisible()){
       widg->raise();
       widg->show();
       widg->setFocus();
     }
   }

   return widg;
}


TGo4FitPanel* TGo4MainWindow::StartFitPanel()
{
   TGo4FitPanel* fitpanel = (TGo4FitPanel*) FindGo4Widget("FitPanel", true);

   if (fitpanel==0) {
      fitpanel = new TGo4FitPanel(centralWidget(),"FitPanel");
      ConnectGo4Widget(fitpanel);
      fitpanel->polish();
      fitpanel->show();
   } else {
      fitpanel->showNormal();
      fitpanel->setFocus();
   }
   return fitpanel;
}

TGo4HistogramInfo* TGo4MainWindow::StartHistogramInfo()
{
   TGo4HistogramInfo* hinfo = (TGo4HistogramInfo*) FindGo4Widget("HistogramInfo", true);
   if (hinfo==0) {
      hinfo = new TGo4HistogramInfo(centralWidget(), "HistogramInfo");
      ConnectGo4Widget(hinfo);
      hinfo->polish();
      hinfo->show();
      hinfo->adjustSize();
   }
   return hinfo;
}

TGo4ConditionInfo* TGo4MainWindow::StartConditionInfo()
{
   TGo4ConditionInfo* cinfo = (TGo4ConditionInfo*) FindGo4Widget("ConditionInfo", true);
   if (cinfo==0) {
      cinfo = new TGo4ConditionInfo(centralWidget(), "ConditionInfo");
      ConnectGo4Widget(cinfo);
      cinfo->polish();
      cinfo->show();
      cinfo->adjustSize();
   }
   return cinfo;
}

void TGo4MainWindow::LoadLibrarySlot()
{
   TGo4LoadedLibraries llib(this);
   llib.exec();
   TGo4Browser* br = (TGo4Browser*) FindGo4Widget("Browser", false);
   if (br!=0) br->ShootUpdateTimer();
}

TGo4ParaEdit* TGo4MainWindow::StartParaEdit(const char* itemname)
{
   TGo4ParaEdit* pedit = (TGo4ParaEdit*) FindGo4Widget("ParaEdit", true);

   if (itemname!=0) {
      TClass* cl = Browser()->ItemClass(itemname);
      if (cl!=0)
         if (!cl->IsLoaded()) {
            QMessageBox::warning(0, "Parameter editor", QString("Cannot start parameter editor for incomplete class ") + cl->GetName());
            return pedit;
         }
   }

   if (pedit==0) {
      pedit = new TGo4ParaEdit(centralWidget(), "ParaEdit");
      ConnectGo4Widget(pedit);
      pedit->polish();
      pedit->show();
   }

   if (itemname!=0)
      pedit->WorkWithParameter(itemname, false);

   return pedit;
}

TGo4EditDynEntry* TGo4MainWindow::StartEditDynEntry()
{
   TGo4EditDynEntry* dedit = (TGo4EditDynEntry*) FindGo4Widget("EditDynEntry", true);

   if (dedit==0) {
      dedit = new TGo4EditDynEntry(centralWidget(), "EditDynEntry");
      ConnectGo4Widget(dedit);
      dedit->polish();
      dedit->show();
   }

   return dedit;
}

TGo4ConditionEditor* TGo4MainWindow::StartConditionEditor()
{
   TGo4ConditionEditor* wedit = (TGo4ConditionEditor*) FindGo4Widget("ConditionEditor", true);
   if (wedit==0) {
      wedit = new TGo4ConditionEditor(centralWidget(), "ConditionEditor");
      ConnectGo4Widget(wedit);
      wedit->polish();
      wedit->show();
   }
   return wedit;
}

TGo4EventInfo* TGo4MainWindow::StartEventInfo()
{
   TGo4EventInfo* einfo = (TGo4EventInfo*) FindGo4Widget("EventInfo", true);

   if (einfo==0) {
      einfo = new TGo4EventInfo(centralWidget(), "EventInfo");
      ConnectGo4Widget(einfo);
      einfo->polish();
      einfo->show();
   }

   return einfo;
}

TGo4AnalysisConfiguration* TGo4MainWindow::FindAnalysisConfiguration()
{
   return (TGo4AnalysisConfiguration*) FindGo4Widget("AnalysisConfiguration", false);
}

void TGo4MainWindow::ToggleAnalysisConfiguration()
{
   TGo4AnalysisConfiguration* conf = FindAnalysisConfiguration();
   if (conf==0) return;

   if (conf->isVisible())
     conf->hide();
   else {
     conf->raise();
     conf->show();
     if (conf->isMinimized()) conf->showNormal();
   }
}

TGo4AnalysisWindow* TGo4MainWindow::FindAnalysisWindow()
{
   return (TGo4AnalysisWindow*) FindGo4Widget("AnalysisWindow", false);
}

void TGo4MainWindow::ToggleAnalysisWindow()
{
   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw==0) return;

   if (anw->HasOutput()) {
      if (anw->isVisible())
        anw->hide();
      else {
        anw->raise();
        anw->show();
        if (anw->isMinimized()) anw->showNormal();
      }
   } else {
      QDockWindow* dock = dynamic_cast<QDockWindow*> (anw->parentWidget());
      if (dock->isVisible())
         dock->hide();
      else
         dock->show();
   }
}

void TGo4MainWindow::CloseAnalysisWindow()
{
   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0) {
      QDockWindow* dock = dynamic_cast<QDockWindow*> (anw->parentWidget());
      removeDockWindow(dock);
      delete dock;
   }
}


void TGo4MainWindow::ToggleMbsMonitor(const char* nodename)
{
   TGo4MBSViewer* mbs =
     dynamic_cast<TGo4MBSViewer*> (FindGo4Widget("MBSViewer", false));
   if (mbs==0) return;
   if (mbs->parentWidget()->isShown())
       mbs->parentWidget()->hide();
   else {
     mbs->SetNode(nodename);
     mbs->parentWidget()->show();
   }
}


TGo4SetScaleValues* TGo4MainWindow::ToggleScaleValues()
{
   TGo4SetScaleValues* scl = 0;

   QWidgetList windows =  fxTGo4WorkSpace->windowList();
   for (int i=0; i<int(windows.count()); ++i ) {
      scl = dynamic_cast<TGo4SetScaleValues*> (windows.at(i));
      if (scl!=0) break;
   }

   if (scl==0) {
      scl = new TGo4SetScaleValues(centralWidget(), "ScaleValues", Qt::WStyle_StaysOnTop);
      scl->polish();
      scl->show();
   } else {
      delete scl;
      scl = 0;
   }
   return scl;
}

void TGo4MainWindow::CreateNewHistSlot(int isremote)
{
   TGo4CreateNewHistogram dlg(this);
   TGo4AnalysisProxy* an = Browser()->FindAnalysis();

   dlg.SetAnalysisAvaliable((isremote!=0) && (an!=0) && an->IsAnalysisSettingsReady());
   dlg.SetLocalAvaliable(isremote!=1);

   if (dlg.exec() != QDialog::Accepted) return;
   TH1* h1 = dlg.MakeHistogram();
   if (h1==0) return;

   TGo4AnalysisWindow* anw = FindAnalysisWindow();

   if (dlg.GetSelectedCmd()==1) {
      TString newitem = Browser()->SaveToMemory("", h1, kTRUE);

      if (newitem.Length()>0)
         editorServiceSlot(anw,
                           QGo4Widget::service_ObjectCreated,
                           newitem.Data(), h1->IsA());
      h1 = 0;
   } else
   if ((dlg.GetSelectedCmd()==2) && (an!=0)) {
      h1->SetBit(TGo4Status::kGo4CanDelete);
      an->UpdateAnalysisObject("", h1);
      if (anw!=0) anw->WaitForNewObject(isremote==1);
   }
   if (h1!=0) delete h1;
}

void TGo4MainWindow::CreateNewConditionSlot(bool forothereditor)
{
   TGo4AnalysisProxy* an = Browser()->FindAnalysis();
   if ((an==0) || !an->IsAnalysisSettingsReady()) {
      QMessageBox::information(this,"Create new condition","Cannot create new condition before analysis setup");
      return;
   }

   TGo4CreateNewCondition dlg(this);

   if (dlg.exec() != QDialog::Accepted) return;

   TGo4Condition* cond = dlg.MakeCondition();
   if (cond==0) return;

   cond->SetBit(TGo4Status::kGo4CanDelete);
   an->UpdateAnalysisObject("", cond);
   delete cond;

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if(anw!=0) anw->WaitForNewObject(forothereditor);
}

void TGo4MainWindow::CreateNewDynEntrySlot(bool forothereditor)
{
   TGo4AnalysisProxy* an = Browser()->FindAnalysis();
   if ((an==0) || !an->IsAnalysisSettingsReady()) {
      QMessageBox::information(this,"Create new dynamic entry","Cannot create new entry before analysis setup");
      return;
   }

   TGo4CreateNewDynEntry dlg(this);

   if (dlg.exec() != QDialog::Accepted) return;

   TGo4DynamicEntry* entry = dlg.MakeEntry();
   if (entry==0) return;

   entry->SetBit(TGo4Status::kGo4CanDelete);
   an->UpdateAnalysisObject("", entry);
   delete entry;

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0) anw->WaitForNewObject(forothereditor);
}

void TGo4MainWindow::ConnectGo4Widget(QGo4Widget* editor)
{
   if (editor==0) return;
   connect(editor, SIGNAL(widgetService(QGo4Widget*, int, const char*, void*)),
           this, SLOT(editorServiceSlot(QGo4Widget*, int, const char*, void*)));
   GetWidgetTopSlot(editor, true);
}

TGo4ViewPanel* TGo4MainWindow::DisplayBrowserItem(const char* itemname, TGo4ViewPanel* panel, TPad* pad, bool activate, int updatelevel, const char* drawopt)
{
   TGo4BrowserProxy* br = Browser();
   if ((br==0) || (itemname==0)) return 0;

   TGo4Slot* guislot = br->BrowserSlot(itemname);

   if (guislot==0) return 0;
   if (guislot->GetProxy()==0) {
      cerr << " Problem with gui slots" << endl;
      return 0;
   }

   if (panel==0) {
      panel = MakeNewPanel();
      pad = panel->GetCanvas();
   } else
   if (pad==0) pad = panel->GetActivePad();

   if (panel->AddDrawObject(pad, TGo4ViewPanel::kind_Link, itemname, 0, false, drawopt)==0) return 0;

   if (activate) {
      panel->SetActivePad(pad);
      panel->ShootRepaintTimer();
   }

   if (updatelevel<0)
      if (go4sett->getFetchDataWhenDraw())
         updatelevel = 2;
      else
         updatelevel = 1;

   br->GetBrowserObject(itemname, updatelevel);

   return panel;
}

bool TGo4MainWindow::SaveBrowserItemToFile(const char* itemname, const char* subfolder)
{
   TGo4BrowserProxy* br = Browser();
   if ((br==0) || (itemname==0)) return 0;

   TString fileslotname;
   const char* filepath = 0;

   bool res = false;

   if (br->DefineFileObject(itemname, fileslotname, &filepath)) {
      int answer = QMessageBox::question(this, "Writing object to file",
         QString("Overwrite ") + filepath + " in file " + fileslotname.Data(),
         "Overwrite", "Save to other file", "Cancel");
      if (answer==2) return false;
      if (answer==0)
        res = br->UpdateObjectInFile(itemname, fileslotname.Data(), filepath);
   }

   if (!res) {
      QFileDialog fd(this, "Save object", kTRUE );
      fd.setMode( QFileDialog::AnyFile);
      fd.setCaption(QString("Save ") + itemname + " in root file");
      fd.setFilters(QString("ROOT (*.root);;ROOT XML (*.xml)"));
      fd.setSelection("file.root");
      if (fLastFileDir.length()>0)
         fd.setDir(QDir(fLastFileDir));
      if (fd.exec() == QDialog::Accepted) {
         res = br->SaveItemToFile(itemname, fd.selectedFile().latin1(), subfolder);
         fLastFileDir = QFileInfo(fd.selectedFile()).dirPath(true);
      }
   }

   return res;
}

void TGo4MainWindow::SavePanelCanvas(TGo4ViewPanel* panel)
{
   if (panel==0) return;

   TCanvas* can = panel->GetCanvas();
   QFileDialog fd( this, "Save Canvas", TRUE);
   QString fdCaption = "Save ";
   fdCaption.append(name());
   fdCaption.append("  As");
   fd.setCaption(fdCaption);
   QString PS = "Post Script (*.ps)";
   QString PS_Portrait = "Post Script Portrait (*.ps)";
   QString PS_Landscape = "Post Script Landscape (*.ps)";
   QString EPS = "Encapsulated Post Script (*.eps)";
   QString EPS_Preview = "Encapsulated Post Script previw (*.eps)";
   QString GIF = "GIF format (*.gif)";
   QString PDF = "PDF format (*.pdf)";
   QString SVG = "SVG format (*.svg)";
   QString XPM = "XPM format (*.xpm)";
   QString PNG = "PNG format (*.png)";
   QString JPG = "JPG format (*.jpg)";
   QString TIFF = "TIFF format (*.tiff)";

   QString CXXM = "C++ Macro (*.C)";
   QString ROOTM = "root file (*.root)";

   fd.setMode( QFileDialog::AnyFile );
   fd.setName( "Save Canvas ");
   fd.setFilter(PS);
   fd.addFilter(PS_Portrait);
   fd.addFilter(PS_Landscape);
   fd.addFilter(EPS);
   fd.addFilter(EPS_Preview);
   fd.addFilter(PDF);
   fd.addFilter(SVG);
   fd.addFilter(GIF);

   #if ROOT_VERSION_CODE >= ROOT_VERSION(4,4,2)
   fd.addFilter(XPM);
   fd.addFilter(PNG);
   fd.addFilter(JPG);
   fd.addFilter(TIFF);
   #endif

   fd.addFilter(CXXM);
   fd.addFilter(ROOTM);

   if (fLastFileDir.length()>0)
      fd.setDir(QDir(fLastFileDir));

   if (fd.exec() != QDialog::Accepted) return;

   QString filename = fd.selectedFile();
   QString filter = fd.selectedFilter();

   fLastFileDir = QFileInfo(filename).dirPath(true);

   const char* opt = "ps";

   if(filter==EPS) {
      opt = "eps";
      if (!filename.endsWith(".eps")) filename.append(".eps");
   } else
   if(filter==EPS_Preview) {
      opt = "Preview";
      if (!filename.endsWith(".eps")) filename.append(".eps");
   } else
   if(filter==PS) {
      opt = "ps";
      if (!filename.endsWith(".ps")) filename.append(".ps");
   } else
   if(filter==PS_Portrait) {
      opt = "Portrait";
      if (!filename.endsWith(".ps")) filename.append(".ps");
   } else
   if(filter==PS_Landscape) {
      opt = "Landscape";
      if (!filename.endsWith(".ps")) filename.append(".ps");
   } else
   if(filter==GIF) {
      opt = "gif";
      if (!filename.endsWith(".gif")) filename.append(".gif");
   } else
   if(filter==PDF) {
      opt = "pdf";
      if (!filename.endsWith(".pdf")) filename.append(".pdf");
   } else
   if(filter==SVG) {
      opt = "svg";
      if (!filename.endsWith(".svg")) filename.append(".svg");
   } else
   if(filter==XPM) {
      opt = "xpm";
      if (!filename.endsWith(".xpm")) filename.append(".xpm");
   } else
   if(filter==PNG) {
      opt = "png";
      if (!filename.endsWith(".png")) filename.append(".png");
   } else
   if(filter==JPG) {
      opt = "jpg";
      if (!filename.endsWith(".jpg")) filename.append(".jpg");
   } else
   if(filter==TIFF) {
      opt = "tiff";
      if (!filename.endsWith(".tiff")) filename.append(".tiff");
   } else
   if(filter==CXXM) {
      opt = "cxx";
      if (!filename.endsWith(".C")) filename.append(".C");
   } else
   if (filter==ROOTM) {
      opt = "root";
      if (!filename.endsWith(".root")) filename.append(".root");

      // plain root browser might not know the go4 color numbers
      // canvas always saved with white background!
      panel->ResetPadFillColors(can,0);
   }

   //TString oldname = thiscanvas->GetName();
   //thiscanvas->SetName("Canvas");
   can->Print(filename.latin1(), opt);
}

TGo4ObjectManager* TGo4MainWindow::OM()
{
   return fxOM;
}

TGo4BrowserProxy* TGo4MainWindow::Browser()
{
   return (TGo4BrowserProxy*) fxOM->GetProxy(fOMBrowserPath.latin1());
}

TGo4Slot* TGo4MainWindow::GetWidgetTopSlot(QGo4Widget* widget, bool force)
{
   if (widget==0) return 0;

   const char* EditorName = widget->name();

   TString editorslotname = fOMEditorsPath.latin1();
   editorslotname += "/";
   editorslotname += EditorName;
   TGo4Slot* edslot = fxOM->GetSlot(editorslotname.Data());
   if ((edslot==0) && force) {
      fxOM->AddProxy(fOMEditorsPath.latin1(), new TGo4WidgetProxy(widget), EditorName, "Links for Go4 widget");
      edslot = fxOM->GetSlot(editorslotname.Data());
   }
   return edslot;
}

void TGo4MainWindow::checkPanelRepaintSlot()
{
   // block monitoring when start panel updates
   Browser()->SetMonitorBlockingFlag(kTRUE);

   fbPanelTimerActive = false;

   // first check if active viewpnael need update its content
   TGo4ViewPanel* actpanel = fxTGo4WorkSpace->GetActivePanel();
   if ((actpanel!=0) && actpanel->IsRepaintTimerActive())
      actpanel->checkRepaintSlot();

   if (fbPanelTimerActive) return;

   // now check if any other panel shold be repainted
   TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.latin1());
   for (int n=0;n<topslot->NumChilds();n++) {
      TGo4Slot* subslot = topslot->GetChild(n);

      TGo4WidgetProxy* wproxy =
          dynamic_cast<TGo4WidgetProxy*> (subslot->GetProxy());
      if (wproxy==0) continue;

      TGo4ViewPanel* panel =
        dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

      if ((panel!=0) && (panel!=actpanel))
        if (panel->IsRepaintTimerActive()) {
            panel->checkRepaintSlot();
            if (fbPanelTimerActive) return;
        }
   }

   // enable monitoring only when all drawing are finished
   Browser()->SetMonitorBlockingFlag(kFALSE);
}

//int temp_counter = 0;

void TGo4MainWindow::editorServiceSlot(QGo4Widget* editor, int serviceid, const char* str, void* par)
{
   if (editor==0) return;

   TGo4LockGuard lock;

   TGo4Slot* edslot = GetWidgetTopSlot(editor, false);
   if (edslot==0) return;

   switch (serviceid) {
      case QGo4Widget::service_DragEnter: {
         QDragEnterEvent* event = (QDragEnterEvent*) par;
         if (event==0) return;
         QString eventstr;
         if (!QTextDrag::decode(event, eventstr)) {
            event->ignore();
            return;
         }

         bool accept = false;
         if (event->source()==FindGo4Widget("Browser",false)) {
            int kind = Browser()->ItemKind(eventstr.latin1());
            TClass* cl = Browser()->ItemClass(eventstr.latin1());
            accept = editor->IsAcceptDrag(eventstr.latin1(), cl, kind);
         }

         if (accept) event->accept();
                else event->ignore();
         break;
      }

      case QGo4Widget::service_DropEvent: {
         QDropEvent* event = (QDropEvent*) par;
         if (event==0) return;
         QString eventstr;
         if (!QTextDrag::decode(event, eventstr)) return;
         event->accept();

         if (event->source()==FindGo4Widget("Browser",false)) {
             int kind = Browser()->ItemKind(eventstr.latin1());
             TClass* cl = Browser()->ItemClass(eventstr.latin1());

             TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (editor);
             TGo4FitPanel* fitpanel = dynamic_cast<TGo4FitPanel*> (editor);
             if (panel!=0)
                panel->DropOnPad((TPad*)str, eventstr.latin1(), cl, kind);
             else
             if (fitpanel!=0)
                fitpanel->DropOnPanel(event, eventstr.latin1(), cl, kind);
             else
                editor->DropItem(eventstr.latin1(), cl, kind);
         }
         break;
      }

      case QGo4Widget::service_CreateItem: {
         TClass* cl = (TClass*) par;
         int id = str!=0 ? atoi(str) : 0;
         if (cl!=0)
           if (cl->InheritsFrom(TH1::Class()))
              CreateNewHistSlot(id);
           else
           if (cl->InheritsFrom(TGo4Condition::Class()) && (id!=0))
              CreateNewConditionSlot(id==1);
           else
           if (cl->InheritsFrom(TGo4DynamicEntry::Class()) && (id!=0))
              CreateNewDynEntrySlot(id==1);
         break;
      }
      case QGo4Widget::service_DrawItem: {
         void** res = (void**) par;
         res[0] = DisplayBrowserItem(str, (TGo4ViewPanel*) res[0], (TPad*) res[1], *((bool*)res[2]), *((int*) res[3]), 0);
         break;
      }

      case QGo4Widget::service_WhereItemDrawn: {
         TGo4ViewPanel** res = (TGo4ViewPanel**) par;
         *res = 0;

         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.latin1());
         for (int n=0;n<topslot->NumChilds();n++) {
            TGo4Slot* subslot = topslot->GetChild(n);

            TGo4WidgetProxy* wproxy =
               dynamic_cast<TGo4WidgetProxy*> (subslot->GetProxy());
            if (wproxy==0) continue;

            TGo4ViewPanel* panel =
               dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

            if (panel!=0)
              if (panel->FindPadWithItem(str)!=0) {
                *res = panel;
                break;
              }
         }
         break;
      }

      case QGo4Widget::service_UndrawItem: {
         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.latin1());

         for (int n=0;n<topslot->NumChilds();n++) {
            TGo4Slot* subslot = topslot->GetChild(n);

            TGo4WidgetProxy* wproxy =
               dynamic_cast<TGo4WidgetProxy*> (subslot->GetProxy());
            if (wproxy==0) continue;

            TGo4ViewPanel* panel =
               dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

            if (panel!=0)
              panel->UndrawItemOnPanel(str);
         }

         break;
      }

      case QGo4Widget::service_EditItem: {
         TClass* cl = Browser()->ItemClass(str);
         Int_t kind = Browser()->ItemKind(str);

         if (kind==TGo4Access::kndGo4Param) {
            TGo4ParaEdit* paredit = StartParaEdit(str);
            break;
         } else
         if (kind==TGo4Access::kndTreeLeaf) {
            TGo4TreeViewer* tviewer =
               dynamic_cast<TGo4TreeViewer*> (FindGo4Widget("TreeViewer", false));
            if (tviewer!=0) {
               tviewer->WorkWithLeaf(str);
               tviewer->parentWidget()->show();
            }
            break;
         }

         if (cl==0) return;

         if (cl->InheritsFrom(TGo4Fitter::Class())) {
            TGo4FitPanel* fitpanel = StartFitPanel();
            TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (editor);
            fitpanel->WorkWithFitter(str, panel, panel==0 ? 0 : panel->GetActivePad());
         } else
         if (cl->InheritsFrom(TGo4Parameter::Class())) {
            TGo4ParaEdit* paredit = StartParaEdit(str);
         } else
         if (cl->InheritsFrom(TH1::Class())) {
            TGo4HistogramInfo* hinfo = StartHistogramInfo();
            hinfo->WorkWithHistogram(str);
         } else
         if (cl->InheritsFrom(TGo4Condition::Class())) {
            TGo4ConditionEditor* wedit = StartConditionEditor();
            wedit->WorkWithCondition(str);
         } else
         if (cl->InheritsFrom(TGo4DynamicEntry::Class())) {
            TGo4EditDynEntry* dedit = StartEditDynEntry();
            dedit->WorkWithEntry(str);
         }
         break;
      }

      case QGo4Widget::service_ShowInfo: {
        TClass* cl = Browser()->ItemClass(str);
        int kind = Browser()->ItemKind(str);
        if (kind==TGo4Access::kndEventElement) {
           TGo4EventInfo* einfo = StartEventInfo();
           einfo->WorkWithEvent(str);
           break;
        }
        if (cl==0) return;
        if (cl->InheritsFrom(TH1::Class())) {
           TGo4HistogramInfo* hinfo = StartHistogramInfo();
           hinfo->WorkWithHistogram(str);
        } else
        if (cl->InheritsFrom(TGo4Condition::Class())) {
           TGo4ConditionInfo* cinfo = StartConditionInfo();
           cinfo->WorkWithCondition(str);
        }
        break;
      }

      case QGo4Widget::service_EditInSlot: {
         TGo4Slot* slot = (TGo4Slot*) par;
         if (slot==0) break;
         TObject* obj = slot->GetAssignedObject();
         if (obj==0) break;
         if (obj->InheritsFrom(TGo4Fitter::Class())) {
            TGo4FitPanel* fitpanel = StartFitPanel();
            fitpanel->WorkWithFitterInSlot(slot);
         }
         break;
      }

      case QGo4Widget::service_SaveItem: {
         const char* subfolder = (const char*) par;
         bool* replace = (bool*) par;
         bool res = SaveBrowserItemToFile(str, subfolder);
         * ((char*) par) = res ? 1 : 0;
         break;
      }

      case QGo4Widget::service_UpdateAnalysisItem: {
         TObject** obj = (TObject**) par;
         Bool_t res = Browser()->UpdateAnalysisItem(str, *obj);
         if (res) *obj = (TObject*) 1; else *obj = 0;
         break;
      }

      case QGo4Widget::service_StatusMessage: {
          StatusMessage(str);
          break;
      }

      case QGo4Widget::service_CreateViewPanel: {
         TGo4ViewPanel** res = (TGo4ViewPanel**) par;
         *res = MakeNewPanel(atoi(str));
         break;
      }

      case QGo4Widget::service_BrowserItemRemote: {
         bool* res = (bool*) par;
         *res = Browser()->IsItemRemote(str);
         break;
      }

      case QGo4Widget::service_Browser: {
         TGo4BrowserProxy** res = (TGo4BrowserProxy**) par;
         *res= Browser();
         break;
      }

      case QGo4Widget::service_SaveToMemory: {
          TObject** obj = (TObject**) par;
          TString itemname = Browser()->SaveToMemory(str, *obj, true);
          QString** res = (QString**) par;
          *res = 0;
          if (itemname.Length()>0)
            *res = new QString(itemname.Data());
          break;
      }

      case QGo4Widget::service_GetAnalysis: {
         TGo4AnalysisProxy** res = (TGo4AnalysisProxy**) par;
         *res = Browser()->FindAnalysis(str);
         break;
      }

      case QGo4Widget::service_AddEditorLink: {
         TGo4Slot* brslot = Browser()->BrowserSlot((const char*)par);

         if (str==0) str = editor->name();

         TGo4Slot* link = fxOM->AddLink(brslot, edslot->GetFullName(),
                                        str, "link to browser item");
         if (link!=0)
            editor->SetLinkedName(link, (const char*)par);

         break;
      }

      case QGo4Widget::service_AddDirectLink: {
         //cout << " QGo4Widget::service_AddDirectLink " << endl;
         if (str==0) str = editor->name();
         TGo4Slot* slot = (TGo4Slot*) par;
         if (slot!=0) {
            //cout << "add link for " << str << " in edslot = " << edslot->GetFullName() << endl;
            fxOM->AddLink(slot, edslot->GetFullName(),
                          str, "direct link to OM slot");
            //edslot->Print("*");
         }
         break;
      }

      case QGo4Widget::service_AddLinkInSlot: {
         //cout << " QGo4Widget::service_AddLinkInSlot " << endl;

         TGo4Slot** res = (TGo4Slot**) par;

         TGo4Slot* parent = *res;
         *res = 0;
         if (parent==0) parent = edslot;
         TGo4Slot* brslot = Browser()->BrowserSlot(str);

         if (brslot!=0) {
            *res = fxOM->AddLink(brslot, parent->GetFullName(),
                               brslot->GetName(), "link in specific subfolder");
            if (*res!=0)
              editor->SetLinkedName(*res, str);
         }
         break;
      }

      case QGo4Widget::service_GetTopSlot: {
         TGo4Slot** res = (TGo4Slot**) par;
         *res = edslot;
         break;
      }

      case QGo4Widget::service_AddEditorSlot: {
         TGo4Slot* objslot = edslot->FindChild(str);

         if (objslot==0)
           objslot = fxOM->MakeObjSlot(edslot->GetFullName(), str, "Place for editor object");
         if (objslot!=0)
            objslot->SetProxy(new TGo4ObjectProxy);

         TGo4Slot** res = (TGo4Slot**) par;
         *res = objslot;
         break;
      }

      case QGo4Widget::service_SetLinkedName: {
         TGo4BrowserProxy::SetLinkedName((TGo4Slot*) par, str);
         break;
      }

      case QGo4Widget::service_GetLinkedName: {
         if (edslot==0) return;
         const char** res = (const char**) par;
         *res = TGo4BrowserProxy::GetLinkedName(edslot->FindChild(str));
         break;
      }

      case QGo4Widget::service_GetLinkedName2: {
         void** res = (void**) par;
         TGo4Slot* link = (TGo4Slot*) *res;
         *res = (void*) TGo4BrowserProxy::GetLinkedName(link);
         break;
      }

      case QGo4Widget::service_GetLinked0:
      case QGo4Widget::service_GetLinked1:
      case QGo4Widget::service_GetLinked2: {
         if (edslot==0) return;
         TGo4Slot* link = edslot->FindChild(str);
         if (link==0) return;
         const char* itemname = TGo4BrowserProxy::GetLinkedName(link);
         TObject** res = (TObject**) par;
         int updatelevel = serviceid - QGo4Widget::service_GetLinked0;
         if (itemname!=0)
            *res = Browser()->GetBrowserObject(itemname, updatelevel);
         else
            *res = link->GetAssignedObject();
         break;
      }

      case QGo4Widget::service_RemoveLink: {
         if (edslot==0) return;
         TGo4Slot* link = edslot->FindChild(str);
         if (link!=0) link->Delete();
         break;
      }

      case QGo4Widget::service_RemoveAllLinks: {
         if ((str==0) || (strcmp(str,"complete")!=0))
            edslot->DeleteChilds(str);
         else
            edslot->Delete();
         break;
      }

      case QGo4Widget::service_ObjectCreated: {
         TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.latin1());
         for (int n=0;n<slot->NumChilds();n++) {
            TGo4Slot* subslot = slot->GetChild(n);
            if (edslot==subslot) continue;

            TGo4WidgetProxy* wproxy =
               dynamic_cast<TGo4WidgetProxy*>
                 (subslot->GetProxy());

            if (wproxy!=0)
              wproxy->GetWidget()->ObjectCreatedByWidget(str, (TClass*) par);
         }
         break;
      }

      case QGo4Widget::service_ConnectPad: {
         if (edslot==0) break;

         TGo4WidgetProxy* proxy =
           dynamic_cast<TGo4WidgetProxy*> (edslot->GetProxy());
         if (proxy!=0)
            proxy->ConnectPad((TPad*) par);
         break;
      }

      case QGo4Widget::service_LastActivePanel: {
         TGo4ViewPanel** res = (TGo4ViewPanel**) par;
         *res = fxTGo4WorkSpace->GetActivePanel();
         break;
      }

      case QGo4Widget::service_General: {
        if (strcmp(str,"CloseAnalysisWindow")==0) {
            // we should postpone window closing, while it is called from inside window itself
            // and it is create problem in sequence,
           QTimer::singleShot(100, this, SLOT(CloseAnalysisWindow()));
        } else

        if (strcmp(str,"PrintAnalysisHistograms")==0) {
           TGo4AnalysisWindow* anw = FindAnalysisWindow();
           if (anw!=0) anw->PrintHistograms();
        } else

        if (strcmp(str,"PrintAnalysisConditions")==0) {
           TGo4AnalysisWindow* anw = FindAnalysisWindow();
           if (anw!=0) anw->PrintConditions();
        } else

        if (strcmp(str,"DisplayMbsMonitor")==0) {
           ToggleMbsMonitor((const char*) par);
        } else

        if (strcmp(str,"SubmitAnalysisSettings")==0) {
           SubmitAnalysisSettings();
        } else

        if (strcmp(str,"SubmitStartAnalysis")==0) {
           SubmitStartAnalysisSlot();
        } else

        if (strcmp(str,"StartAnalysis")==0) {
          StartAnalysisSlot();
        } else

        if (strcmp(str,"StopAnalysis")==0) {
          StopAnalysisSlot();
        } else

        if (strcmp(str,"TerminateAnalysis")==0) {
           TerminateAnalysis();
        } else

        if (strcmp(str,"UpdateGuiLayout")==0) {
          UpdateCaptionButtons();
        } else

        if (strcmp(str, "StartEventInfo")==0) {
           StartEventInfo();
        } else

        if (strcmp(str, "ActivateConditionEditor")==0) {
           TGo4ConditionEditor* w =
             (TGo4ConditionEditor*) FindGo4Widget("ConditionEditor", true);
           if (w!=0) {
              w->setFocus();
              w->RefreshWidget(true);
           }
        } else

        if (strcmp(str, "SavePanelCanvas")==0) {
           SavePanelCanvas(dynamic_cast<TGo4ViewPanel*>(editor));
        } else

        if (strcmp(str, "ToggleScaleValues")==0) {
           ToggleScaleValues();
        } else

        if (strcmp(str, "GetFitterFromFitPanel")==0) {
           TGo4FitPanel* panel = (TGo4FitPanel*) FindGo4Widget("FitPanel", false);
           TGo4Fitter** res = (TGo4Fitter**) par;
           if ((panel!=0) && (res!=0))
              *res = panel->GetFitter();
        } else

        if (strcmp(str, "CloneFitterFromFitPanel")==0) {
           TGo4FitPanel* panel = (TGo4FitPanel*) FindGo4Widget("FitPanel", false);
           TGo4Fitter** res = (TGo4Fitter**) par;
           if ((panel!=0) && (res!=0))
              *res = panel->CloneFitter();
        }

        break;
      }

      case QGo4Widget::service_PanelTimer: {
         TGo4ViewPanel* panel = (TGo4ViewPanel*) editor;
//         int interval = (int) par;
         if (!fbPanelTimerActive) {
           fbPanelTimerActive = true;
           QTimer::singleShot(0, this, SLOT(checkPanelRepaintSlot()));
         }

         break;
      }

      case QGo4Widget::service_HelpWindow: {
         HelpWindow(str, (const char*) par);
         break;
      }

      case QGo4Widget::panel_Activated:
      case QGo4Widget::panel_Modified:
      case QGo4Widget::panel_Updated:
      case QGo4Widget::panel_ActiveUpdated:
      case QGo4Widget::panel_PadDeleted:
      case QGo4Widget::panel_Deleted: {
         TGo4ViewPanel* panel = (TGo4ViewPanel*) editor;
         TPad* pad = (TPad*) par;
         fxTGo4WorkSpace->ResponseOnPanelEvent(serviceid, panel, pad);
         break;
      }

   }
}

// ******************** hot start functions *********************

void TGo4MainWindow::HotStart(const char* fname)
{
   if (fname==0) return;

   TGo4Script* exec = TGo4Script::ScriptInstance();
   if(exec==0) return;

   if (!exec->StartScriptExecution(fname)) return;

   QTimer::singleShot(TGo4Script::DelayMillisec(), this, SLOT(ProcessHotStart()));
}

void TGo4MainWindow::ProcessHotStart()
{
   TGo4Script* exec = TGo4Script::ScriptInstance();
   if (exec==0) return;

   Bool_t res = kTRUE;
   do {
      res = exec->ContinueExecution();
   } while(res && !exec->IsWaitSomething());

   if (res) {
      QTimer::singleShot(TGo4Script::DelayMillisec(), this, SLOT(ProcessHotStart()));
      if (QApplication::overrideCursor()==0)
        QApplication::setOverrideCursor(Qt::WaitCursor);
   } else {
      exec->FinishExecution();
      StatusMessage("Script execution done");
      QApplication::restoreOverrideCursor();
   }
}

void TGo4MainWindow::StopGUIScriptSlot()
{
   TGo4Script* exec = TGo4Script::ScriptInstance();
   if (exec!=0) exec->FinishExecution();
}

void TGo4MainWindow::CreateGUIScriptSlot()
{
   QString ext = TGo4Script::FileExtension();
   QString fileName = QFileDialog::getSaveFileName(
        fLastFileDir, QString("GUI hotstart script (*") + ext + ")", this,
        "Create GUI script dialog", "Choose a file to be created");
   if (fileName.length()==0) return;

   int pos = fileName.find(ext);

   if ((pos<0) || (pos!=fileName.length()-strlen(ext)))
     fileName+=ext;

   StatusMessage(QString("Generate script ")+fileName);

   TGo4Script::ProduceScript(fileName.latin1(), this);
}

void TGo4MainWindow::ProcessQtEvents()
{
   if (fApp!=0) fApp->processEvents(TGo4Script::DelayMillisec());
}

TGo4ViewPanel* TGo4MainWindow::FindViewPanel(const char* name)
{
   if ((name==0) || (*name==0)) return 0;

   TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (FindGo4Widget(name, false));

   if (panel!=0) return panel;

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.latin1());

   for (Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* widgslot = slot->GetChild(n);
      TGo4WidgetProxy* wproxy = widgslot==0 ? 0 : dynamic_cast<TGo4WidgetProxy*> (widgslot->GetProxy());
      panel = wproxy==0 ? 0 : dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

      if (panel!=0)
        if (strcmp(panel->GetPanelName(), name)==0) return panel;
   }

   return 0;
}
