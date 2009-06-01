#include "TGo4MainWindow.h"

#include <dlfcn.h>
#include "Riostream.h"

#include "qcolor.h"
#include "qcolordialog.h"
#include <QtCore/qnamespace.h>
#include "qstring.h"
#include "qtabwidget.h"
#include "qvariant.h"
#include "qmime.h"
#include "qlayout.h"
#include "qtooltip.h"
#include "qaction.h"
#include "qmenubar.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qfontdialog.h"
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "TGo4QSettings.h"
#include "qapplication.h"
#include "qclipboard.h"
#include <QStatusBar>
#include "qtoolbutton.h"
#include "qsignalmapper.h"
#include "qstylefactory.h"
#include "qlayout.h"
#include "qmessagebox.h"
#include "qradiobutton.h"
#include "qcombobox.h"
#include "qinputdialog.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qevent.h"
#include <QtCore/qtimer.h>
#include "qspinbox.h"
#include "qstyle.h"
#include <QDateTime>

#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>

#include <QFileDialog>
#include <QtGui/QAction>
#include <QtCore/QProcess>
#include <QtCore/QHash>


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
#include "QRootApplication.h"
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
#include "TGo4MBSViewer.h"
#include "TGo4SetScaleValues.h"
#include "TGo4CommandLine.h"
#include "TGo4MarkerSettings.h"
#include "TGo4OptStatsSettings.h"

#ifdef __GO4DIM__
#include "TGo4DabcMonitor.h"
#endif


#include "TGo4Script.h"

#include "TGo4HServerConnection.h"

using namespace Qt;

TGo4MainWindow::TGo4MainWindow(QApplication* app, bool server) :
   QMainWindow(),
   fApp(app),
   fbServerMode(server)
{
   // setDockWindowsMovable(TRUE);
   // statusBar();
   resize( 1152, 864 );

   setWindowIcon(QIcon(":/icons/go4logo2_big.png"));
   // setRightJustification( TRUE );
   // setUsesTextLabel(TRUE);
   // setOpaqueMoving(FALSE);

   fxOM = new TGo4ObjectManager("GUI_OM","Gui object manager");
   fOMDataPath    = "data";
   fOMBrowserPath = "gui";
   fOMEditorsPath = "editors";

   fKillCommand = "";
   fLastPassword = "";
   fLastFileDir = QDir::currentPath();

   fCloseCounter = 0;
   fConnectingCounter = 0;

   fbPanelTimerActive = false;
   winMapper = 0;

    // create mount point for all data sources
   fxOM->MakeFolder(fOMDataPath.toAscii());

   // create entry for browser
   TGo4BrowserProxy* br = new TGo4BrowserProxy(fOMDataPath.toAscii(), fOMEditorsPath.toAscii(), kFALSE);
   fxOM->AddProxy("", br, fOMBrowserPath.toAscii(), "Place for gui slots");
   br->CreateMemoryFolder();

   TH1I* h1 = new TH1I("histo1","histo title", 100, -10., 10.);
   h1->FillRandom("gaus",10000);
   h1->SetDirectory(0);
   br->SaveToMemory("", h1, kTRUE);

   // br->OpenFile("asf.root");

   new TGo4Script(this);

   gStyle->SetPalette(1);
   //gStyle->SetOptStat(11111111);
   gStyle->SetOptStat(go4sett->getOptStat());

   fxWorkSpace = new TGo4WorkSpace(this);
   fxWorkSpace->setScrollBarsEnabled(TRUE);
   setCentralWidget(fxWorkSpace);
   fxWorkSpace->setFocus();

   AddFileMenu();
   AddFileToolBar();

   AddToolsMenu();
   AddToolsBar();

   AddAnalysisMenu();
   AddAnalysisBar();

   UpdateCaptionButtons();

   QDockWidget* MBSDockWin = new QDockWidget("MBS monitor", this, Qt::Widget);
   MBSDockWin->setObjectName("MbsViewerDock");
   TGo4MBSViewer* mbs = new TGo4MBSViewer(MBSDockWin, "MBSViewer");
   mbs->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(mbs);
   MBSDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   MBSDockWin->setWidget(mbs);
   MBSDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, MBSDockWin);

   QDockWidget* BrowserDockWin = new QDockWidget("Browser", this, Qt::Widget);
   BrowserDockWin->setObjectName("BrowserDock");
   TGo4Browser* browser = new TGo4Browser(BrowserDockWin,"Browser");
   browser->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(browser);
   browser->StartWorking();
   BrowserDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   BrowserDockWin->setWidget(browser);
   BrowserDockWin->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
   addDockWidget(Qt::LeftDockWidgetArea, BrowserDockWin);


#ifdef __GO4DIM__
   QDockWidget* DABCDockWin = new QDockWidget("DABC monitor", this, Qt::Widget);
   DABCDockWin->setObjectName("DABCMonitorDock");
   TGo4DabcMonitor* dabc = new TGo4DabcMonitor(DABCDockWin, "DabcMonitor");
   dabc->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(dabc);
   DABCDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   DABCDockWin->setWidget(dabc);
   DABCDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, DABCDockWin);
#endif

   QToolBar* DividePanelBar = addToolBar("Canvas Tools");
   DividePanelBar->setObjectName("DividePadDock");
   TGo4DividePad* dividepanel = new TGo4DividePad(this, "DividePad");
   DividePanelBar->addWidget(dividepanel);

   QToolBar* stylebar = addToolBar("Color Tools");
   stylebar->setObjectName("Go4StyleDock");
   TGo4Style* style = new TGo4Style(this, "Go4Style");
   stylebar->addWidget(style);

   QToolBar* BrowserOptionsPanel = addToolBar("Browser Options");
   BrowserOptionsPanel->setObjectName("BrowserOptionsDock");
   TGo4BrowserOptions* bropt = new TGo4BrowserOptions(this, "BrowserOptions");
   ConnectGo4Widget(bropt);
   BrowserOptionsPanel->addWidget(bropt);
   bropt->StartWorking();

   QToolBar* DrawOptionPanel = addToolBar("Draw Options");
   DrawOptionPanel->setObjectName("DrawOptionsDock");
   TGo4HisDrawOptions* drawopt = new TGo4HisDrawOptions(this, "DrawOptions");
   DrawOptionPanel->addWidget(drawopt);

   QToolBar* DrawOptionShortPanel = addToolBar("Hist Draw Options");
   DrawOptionShortPanel->setObjectName("HisDrawOptionsDock");
   TGo4HisDrawOptionsShort* drawopts = new TGo4HisDrawOptionsShort(this, "HisDrawOptions");
   DrawOptionShortPanel->addWidget(drawopts);
   DrawOptionShortPanel->adjustSize();

   QToolBar* ScaleOptionBar = addToolBar("Zoom Tools");
   ScaleOptionBar->setObjectName("ScaleOptionsDock");
   TGo4ScaleOptions* scopt = new TGo4ScaleOptions(this, "ScaleOptions");
   ConnectGo4Widget(scopt);
   ScaleOptionBar->addWidget(scopt);
   ScaleOptionBar->adjustSize();

   QToolBar* commandlinebar = addToolBar("Go4 Command Line");
   commandlinebar->setObjectName("CommandLineDock");
   TGo4CommandLine* cli = new TGo4CommandLine(this, "CommandLine");
   cli->setMainWindow(this);
   ConnectGo4Widget(cli);
   commandlinebar->addWidget(cli);

   QToolBar* tviewerdock = addToolBar("Tree viewer");
   tviewerdock->setObjectName("TreeViewerDock");
   TGo4TreeViewer* tviewer = new TGo4TreeViewer(this, "TreeViewer");
   ConnectGo4Widget(tviewer);
   tviewerdock->addWidget(tviewer);

   QDockWidget* LogDockWin = new QDockWidget("Log window", this);
   LogDockWin->setObjectName("LogInfoDock");
   TGo4LogInfo* loginfo = new TGo4LogInfo(this, "LogInfo");
   ConnectGo4Widget(loginfo);
   LogDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   LogDockWin->setWidget(loginfo);
   LogDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, LogDockWin);

   // make it here while only here exists all toolbars and dock widgets
   AddSettingMenu();

   windowsMenu = menuBar()->addMenu("&Windows");
   connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(windowsMenuAboutToShow()));

   menuBar()->addSeparator();

   QMenu* helpMenu = menuBar()->addMenu("&Help");
   helpMenu->addAction("&Introduction (user manual)", this, SLOT(IntroHelpSlot()));
   helpMenu->addAction("&Reference manual", this, SLOT(RefHelpSlot()));
   helpMenu->addAction("&Fit Tutorial", this, SLOT(FitHelpSlot()));
   helpMenu->addSeparator();
   helpMenu->addAction("About &Qt", this, SLOT(aboutQt()), Key_F2 );
   helpMenu->addAction("About R&OOT", this, SLOT(aboutROOT()), Key_F3);
   helpMenu->addAction("About &Go4", this, SLOT(about()), Key_F4);

   const char* libs = getenv("GO4USERLIBRARY");

   if (libs!=0) {
      QStringList LibList = QString(libs).split(":",QString::SkipEmptyParts);

      for (QStringList::Iterator it=LibList.begin(); it!=LibList.end(); ++it)
          gSystem->Load((*it).toAscii());
   }

   go4sett->RestoreSettings(this);

   go4sett->restoreGeometry(this);

   QApplication::setStyle(go4sett->getAppStyle());

   faCrosshair->setChecked(go4sett->getPadCrosshair());
   faEventstatus->setChecked(go4sett->getPadEventStatus());

   go4sett->getBasicSettings();
   go4sett->getAppFont();

   // start mbs monitoring only after browser etc. is fully there:
   if(go4sett->getMbsMonitorMonitorActive()) mbs->TimerStart();

   cout <<"Using Qt Settings at "<< TGo4QSettings::GetSettLoaction().toAscii().constData() << endl;

   statusBar()->showMessage("Ready");
   statusBar()->setSizeGripEnabled(TRUE);
}

TGo4MainWindow::~TGo4MainWindow()
{
    TGo4Script::DeleteInstance();

    delete fxOM;
}

const char* TGo4MainWindow::LastTypedPassword() const
{
   return fLastPassword.toAscii();
}


void TGo4MainWindow::about()
{
   QString GO4STR("Go4  ");
   GO4STR.append ( __GO4RELEASE__);
   QString GO4STR1 = GO4STR;
   GO4STR1.append("\n");
   GO4STR1.append("The Go4 (GSI Object Oriented On-line Off-line system)  is based on ROOT\n with the specific requirements of the low and medium energy nuclear and atomic\n physics experiments implemented as extensions.\n See  http://go4.gsi.de  for more information") ;

   QMessageBox AboutGo4(GO4STR, GO4STR1, QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutGo4.setIconPixmap(QPixmap( ":/icons/go4logo2_big.png"));
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
   AboutRoot.setIconPixmap(QPixmap( ":/icons/root.png"));
   AboutRoot.exec();
}

void TGo4MainWindow::AddSettingMenu()
{
   QMenu* settMenu = menuBar()->addMenu("&Settings");

   QMenu* sub = createPopupMenu();
   sub->setTitle("Sh&ow/hide");
   settMenu->addMenu(sub);

   settMenu->addAction("&Font...", this, SLOT(ChangeFontSlot()));

   QMenu *style = settMenu->addMenu("St&yle");

   QMenu* prefMenu = settMenu->addMenu("&Preferences");

   faFetchWhenDraw = AddChkAction(prefMenu, "Fetch when drawing",
		                go4sett->getFetchDataWhenDraw(), this, SLOT(ChangeFetchWhenDrawSlot()));

   faFetchWhenCopy = AddChkAction(prefMenu, "Fetch when copying",
		              go4sett->getFetchDataWhenCopy(), this, SLOT(ChangeFetchWhenCopySlot()));

   faFetchWhenSave = AddChkAction(prefMenu, "Fetch when saving",
		              go4sett->getFetchDataWhenSave(), this, SLOT(ChangeFetchWhenSaveSlot()));

   QMenu* panelMenu = settMenu->addMenu("&Panel defaults");

   panelMenu->addAction("&Canvas color...", this, SLOT(CanvasColorSlot()));
   panelMenu->addAction("Marker labels...", this, SLOT(MarkerSettingsSlot()));
   panelMenu->addAction("Statistics box...", this, SLOT(OptStatsSlot()));

   faCrosshair = AddChkAction(panelMenu, "Cross(&X)hair mode",
		         go4sett->getPadCrosshair(), this, SLOT(CrosshairSlot()));

   faEventstatus = AddChkAction(panelMenu, "Show Event Status",
		             go4sett->getPadEventStatus(), this, SLOT(EventStatusSlot()));

   faClone = AddChkAction(panelMenu, "Objects cloning",
		      go4sett->getCloneFlag(), this, SLOT(ChangeCloneFlagSlot()));

   faDrawTime = AddChkAction(panelMenu, "Draw time",
		     go4sett->getDrawTimeFlag(), this, SLOT(ChangeDrawTimeFlagSlot()));
   faDrawTime->setEnabled(go4sett->getCloneFlag());

   faDrawDate = AddChkAction(panelMenu, "Draw date",
		           go4sett->getDrawDateFlag(), this, SLOT(ChangeDrawDateFlagSlot()));
   faDrawDate->setEnabled(go4sett->getCloneFlag());

   faDrawItem = AddChkAction(panelMenu, "Draw item name",
		      go4sett->getDrawItemFlag(), this, SLOT(ChangeDrawItemFlagSlot()));
   panelMenu->addAction(faDrawItem);
   faDrawItem->setEnabled(go4sett->getCloneFlag());

   settMenu->addAction("&Log actions...", this, SLOT(LogSettingsSlot()));

   settMenu->addAction("Generate &hotstart", this, SLOT(CreateGUIScriptSlot()));
   settMenu->addAction("&Break hotstart execution", this, SLOT(StopGUIScriptSlot()));

   settMenu->addAction("&Terminal history", this, SLOT(InputTerminalParametersSlot()));

   settMenu->addAction("&Save Settings", this, SLOT(SaveSettingsSlot()));

   QActionGroup *ag = new QActionGroup(this);
   ag->setExclusive( TRUE );
   QSignalMapper *styleMapper = new QSignalMapper( this );
   connect(styleMapper, SIGNAL(mapped(const QString&)), this, SLOT(SetStyleSlot(const QString&)));

   QStringList list = QStyleFactory::keys();
   list.sort();
   QHash<QString, int> stylesDict;
   for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      QString styleStr = *it;
      QString styleAccel = styleStr;
      if ( stylesDict[styleAccel.left(1)] ) {
         for ( uint i = 0; i < styleAccel.length(); i++ ) {
            if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
               stylesDict.insert(styleAccel.mid( i, 1 ), 1);
               styleAccel = styleAccel.insert( i, '&' );
               break;
             }
          }
       } else {
          stylesDict.insert(styleAccel.left(1), 1);
          styleAccel = "&"+styleAccel;
       }
       QAction *act = new QAction(styleAccel, this);
       act->setCheckable(true);
       if (go4sett->getAppStyle() == styleStr)
    	   act->setChecked(true);

       connect( act, SIGNAL(triggered()), styleMapper, SLOT(map()) );
       styleMapper->setMapping(act, styleStr);

       ag->addAction(act);
       style->addAction(act);


   }
   QApplication::setColorSpec( QApplication::ManyColor );
}

void TGo4MainWindow::AddFileMenu()
{
   QMenu* fileMenu = menuBar()->addMenu("&File");

   fileMenu->addAction(QIcon( ":/icons/open.png" ), "&Open...",
             this, SLOT(OpenFileSlot()), CTRL+Key_O );
   fileMenu->addAction(QIcon( ":/icons/network.png" ), "Open &Remote...",
             this, SLOT(OpenRemoteFileSlot()), CTRL+Key_R );
   fileMenu->addAction(QIcon( ":/icons/histserv.png" ), "Open &HServer...",
             this, SLOT(ConnectHServerSlot()), CTRL+Key_H );
   fileMenu->addAction(QIcon( ":/icons/filesave.png" ), "Save memor&y...",
             this, SLOT(SaveFileSlot()), CTRL+Key_Y );
   fileMenu->addAction(QIcon( ":/icons/close.png" ),"Close all files",
             this, SLOT(CloseAllFilesSlot()), CTRL+Key_Q);
   fileMenu->addAction(QIcon( ":/icons/exit.png" ), "E&xit",
             this, SLOT(close()), CTRL+Key_X );
}

void TGo4MainWindow::AddFileToolBar()
{
   QToolBar *FileBar = addToolBar( "File Toolbar" );
   FileBar->setObjectName("FileToolBar");

   FileBar->addAction( QIcon( ":/icons/open.png" ), "Open a local file from disk",
                       this, SLOT(OpenFileSlot()));

   FileBar->addAction( QIcon( ":/icons/network.png" ), "Open a remote file from server",
                        this, SLOT(OpenRemoteFileSlot()));

   FileBar->addAction( QIcon( ":/icons/histserv.png" ), "Connect to running histogram server",
                        this, SLOT(ConnectHServerSlot()));

   FileBar->addAction( QIcon( ":/icons/filesave.png" ), "Save the content of the browser to a root file",
                       this, SLOT(SaveFileSlot()));

   FileBar->addAction( QIcon( ":/icons/close.png" ), "Close all root files in file browser",
                       this, SLOT(CloseAllFilesSlot() ));

   FileBar->addAction( QIcon( ":/icons/exit.png" ), "Exit the Go4",
                       this, SLOT(close()));
}

void TGo4MainWindow::AddToolsMenu()
{
   QMenu* toolMenu = menuBar()->addMenu("&Tools");

   toolMenu->addAction(QIcon(":/icons/chart.png" ), "&View Panel",
                this, SLOT(MakeNewPanel()), CTRL+Key_V );
   toolMenu->addAction(QIcon(":/icons/fitpanel.png" ), "&Fit Panel...",
                this, SLOT(StartFitPanel()), CTRL+Key_F );
   toolMenu->addAction(QIcon(":/icons/hislist.png" ),"&Histogram properties...",
               this, SLOT(StartHistogramInfo()));
   toolMenu->addAction(QIcon(":/icons/hiscre.png" ), "Create New H&is...",
               this, SLOT(CreateNewHistSlot()), CTRL+Key_I );
   toolMenu->addAction(QIcon(":/icons/condlist.png" ),"C&ondition properties...",
               this, SLOT(StartConditionInfo()));
   toolMenu->addAction(QIcon(":/icons/condcre.png" ),"&Condition Editor...",
               this, SLOT(CreateNewConditionSlot()), CTRL+Key_C);
   toolMenu->addAction(QIcon(":/icons/zoom.png" ),"&Event Printout...",
               this, SLOT(StartEventInfo()));
   toolMenu->addAction(QIcon(":/icons/dynlist.png" ),"Create &Dyn. List entry ...",
               this, SLOT(CreateNewDynEntrySlot()), CTRL+Key_D);
   toolMenu->addAction(QIcon(":/icons/dllicon.png" ), "Load Li&braries ...",
               this, SLOT(LoadLibrarySlot()), CTRL+Key_B);
   toolMenu->addAction(QIcon(":/icons/user.png" ), "&User GUI ...",
               this, SLOT(UserPanelSlot()), CTRL+Key_U);
}

void TGo4MainWindow::AddToolsBar()
{
   QToolBar *ToolBar = addToolBar("Go4 tools");
   ToolBar->setObjectName("Go4ToolsBar");

   ToolBar->addAction( QIcon( ":/icons/chart.png" ), "Create a new view panel",
                       this, SLOT(MakeNewPanel()));

   ToolBar->addAction( QIcon( ":/icons/fitpanel.png" ), "Show/hide the Go4 Fit Panel",
                    this, SLOT(StartFitPanel()));

   ToolBar->addAction( QIcon( ":/icons/hislist.png" ), "Show histogram properties window",
                    this, SLOT(StartHistogramInfo()));

   ToolBar->addAction( QIcon( ":/icons/hiscre.png" ), "Create new ROOT Histogram",
                    this, SLOT(CreateNewHistSlot()));

   ToolBar->addAction( QIcon( ":/icons/condlist.png" ), "Show condition properties window",
                    this, SLOT(StartConditionInfo()));

   ToolBar->addAction( QIcon( ":/icons/condcre.png" ), "Create new condition in analysis",
                    this, SLOT(CreateNewConditionSlot()));

   ToolBar->addAction( QIcon( ":/icons/zoom.png" ), "Event printout and examination window",
                    this, SLOT(StartEventInfo()));

   ToolBar->addAction( QIcon( ":/icons/dynlist.png" ), "Create new Dynamic list entry",
                    this, SLOT(CreateNewDynEntrySlot()));

   ToolBar->addAction( QIcon( ":/icons/dllicon.png" ), "Load Libraries",
                    this, SLOT(LoadLibrarySlot() ));

   ToolBar->addAction( QIcon( ":/icons/user.png" ), "Start user panel",
                    this, SLOT(UserPanelSlot()));
}

void TGo4MainWindow::AddAnalysisMenu()
{
   QMenu* anMenu = menuBar()->addMenu("&Analysis");

   faLaunchAnal =
      anMenu->addAction(QIcon(":/icons/launchanal.png" ), "Lau&nch analysis...",
           this, SLOT(LaunchClientSlot()), CTRL+Key_N);
   faConnectAnal =
      anMenu->addAction(QIcon(":/icons/connect.png"), "&Connect to running server...",
             this, SLOT(ConnectServerSlot()), CTRL+Key_C);

   faPrepareAnal =
      anMenu->addAction(QIcon(":/icons/connect.png" ), "&Prepare for client connection...",
             this, SLOT(PrepareForClientConnectionSlot()), CTRL+Key_C);

   faDisconnectAnal =
      anMenu->addAction(QIcon(":/icons/disconnect.png"), "Disconnect from analysis",
             this, SLOT(DisconnectAnalysisSlot()), CTRL+Key_M);

   faShutdownAnal =
      anMenu->addAction(QIcon(":/icons/shutanal.png"), "Shutdown analysis server",
             this, SLOT(ShutdownAnalysisSlot()), CTRL+Key_M);

   faSumbStartAnal =
      anMenu->addAction(QIcon(":/icons/restart.png"), "Submit+S&tart",
             this, SLOT(SubmitStartAnalysisSlot()), CTRL+Key_T);

   faStartAnal =
      anMenu->addAction(QIcon(":/icons/start.png"), "&Start",
             this, SLOT(StartAnalysisSlot()), CTRL+Key_S);

   faStopAnal =
      anMenu->addAction(QIcon(":/icons/Stop.png"), "Stop (&Halt)",
             this, SLOT(StopAnalysisSlot()), CTRL+Key_H);

   faAnalConfig =
      anMenu->addAction(QIcon(":/icons/control.png"), "Confi&guration...",
               this, SLOT(ToggleAnalysisConfiguration()), CTRL+Key_G);

   faAnalTermin =
      anMenu->addAction(QIcon(":/icons/analysiswin.png"), "Analysis &Window",
               this, SLOT(ToggleAnalysisWindow()), CTRL+Key_W);
}

void TGo4MainWindow::AddAnalysisBar()
{
   QToolBar *AnalBar = addToolBar("Analysis Toolbar");
   AnalBar->setObjectName("AnalysisToolBar");
   AnalBar->setToolTip("Analysis related tool buttons");

   AnalBar->addAction(faLaunchAnal);
   AnalBar->addAction(faConnectAnal);
   AnalBar->addAction(faDisconnectAnal);
   AnalBar->addAction(faShutdownAnal);

   AnalBar->addAction(faSumbStartAnal);

   AnalBar->addAction(faStartAnal);

   AnalBar->addAction(faStopAnal);
   AnalBar->addAction(faAnalConfig);
   AnalBar->addAction(faAnalTermin);
}

void TGo4MainWindow::windowsMenuAboutToShow()
{
    windowsMenu->clear();

    bool on = ! fxWorkSpace->windowList().isEmpty();

    windowsMenu->addAction("Ca&scade", centralWidget(), SLOT(cascade()))->setEnabled(on);
    windowsMenu->addAction("&Tile", centralWidget(), SLOT(tile()))->setEnabled(on);
    windowsMenu->addAction("&Close all", this, SLOT(CloseAllWindows()))->setEnabled(on);
    windowsMenu->addAction("&Minimize all", this, SLOT(MinAllWindows()))->setEnabled(on);

    windowsMenu->addSeparator();

    QGo4Widget* loginfo = FindGo4Widget("LogInfo", false);

    windowsMenu->addAction("Save L&ogwindow", loginfo, SLOT(SaveLogInfo()))->setEnabled(loginfo!=0);
    windowsMenu->addAction("Clear &Logwindow", loginfo, SLOT(ClearLogInfo()))->setEnabled(loginfo!=0);

    QGo4Widget* anw = FindGo4Widget("AnalysisWindow", false);

    windowsMenu->addAction("Save &Analysis window", anw, SLOT(SaveAnalysisOutput()))->setEnabled(anw!=0);
    windowsMenu->addAction("Clear Analysis &window", anw, SLOT(ClearAnalysisOutput()))->setEnabled(anw!=0);

    windowsMenu->addSeparator();

    delete winMapper;
    winMapper = new QSignalMapper(this);
    connect(winMapper, SIGNAL(mapped(int)), this, SLOT(windowsMenuActivated(int)));

    QWidgetList windows =  fxWorkSpace->windowList();
    for (int i=0; i<int(windows.count()); ++i ) {
       QAction* act = new QAction(windows.at(i)->windowTitle(), winMapper);
       act->setCheckable(true);
       act->setChecked(fxWorkSpace->activeWindow() == windows.at(i));

       windowsMenu->addAction(act);

       connect(act, SIGNAL(triggered()), winMapper, SLOT(map()) );
       winMapper->setMapping(act, i);
    }
}

void TGo4MainWindow::CloseAllWindows()
{
   QWidgetList windows =  fxWorkSpace->windowList();
    for ( int i = 0; i < int(windows.count()); ++i )
       windows.at(i)->close();
}

void TGo4MainWindow::MinAllWindows()
{
   QWidgetList windows =  fxWorkSpace->windowList();
   for ( int i = 0; i < int(windows.count()); ++i )
       windows.at(i)->showMinimized();
}

void TGo4MainWindow::windowsMenuActivated( int id )
{
   QWidget* w = fxWorkSpace->windowList().at(id);
   if (w) w->showNormal();
   w->setFocus();
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
           dirname = info.path();
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

   cout << "Try : " << (dirname+libname).toAscii().constData() << endl;
   void* lib1 = ::dlopen((dirname+libname).toAscii(), RTLD_LAZY | RTLD_GLOBAL);

   if (lib1==0) {
      const char* errmsg = ::dlerror();
      if (errmsg!=0) cout << errmsg << endl;
      return false;
   }

   TStartUserGuiFunc startfunc = (TStartUserGuiFunc) ::dlsym(lib1,"StartUserPanel");

   if (startfunc!=0) {
     QGo4Widget* userpanel = (QGo4Widget*) startfunc(fxWorkSpace);
      fxWorkSpace->addWindow(userpanel);
     if (userpanel!=0) {
        userpanel->setObjectName("UserPanel");
        ConnectGo4Widget(userpanel);
        userpanel->show();
        result = true;
     } else
       StatusMessage("StartUserPanel() function did not create user widget");
   }

   if (result) {
     cout << "Start user GUI from ";
     if (dirname.length()>0)
        cout << dirname.toStdString() << endl;
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
   QString usergui = getenv("GO4USERGUI");
   if (usergui.length()>0)
     if (startUserGUI(usergui.toAscii())) return;

   // check from standard LD_LIBRARY_PATHS
   if (startUserGUI(0)) return;

   usergui = getenv("GO4SYS");
   if (usergui.length()>0) {
     if (usergui[usergui.length()-1]!='/') usergui+="/";
     usergui+="qt4/Go4UserGUI";
     if (startUserGUI(usergui.toAscii())) return;
   }

   QMessageBox::critical(this,"Starting user GUI", "No suitable libraries found");
}


TGo4ViewPanel* TGo4MainWindow::MakeNewPanel(int ndiv)
{
   QString name;

   TGo4Slot* edslot = fxOM->GetSlot(fOMEditorsPath.toAscii());

   int n = 0;
   do {
     n++;
     name = QString("Panel") + QString::number(n);
   } while ((edslot!=0) && (edslot->FindChild(name.toAscii())!=0));

   TGo4ViewPanel* panel = new TGo4ViewPanel(fxWorkSpace, name.toAscii());
   fxWorkSpace->addWindow(panel); // warning: Qt may exchange the winId here!
   panel->GetQCanvas()->performResize(); // may register new winId for TCanvas here
   ConnectGo4Widget(panel);
   panel->CompleteInitialization();
   panel->update();
   panel->show();

   if(ndiv>1) {

      int DivX(1), DivY(1);

      TGo4DividePad* divpad = findChild<TGo4DividePad*>("DividePad");

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
   //cout <<"TGo4MainWindow::closeEvent...." << endl;
   if (fCloseCounter!=0) return;

// new for Qt4:
 if(QMessageBox::question( this, "Go4 GUI", "Really Exit Go4?",
         QMessageBox::Yes | QMessageBox::No ,
         QMessageBox::Yes) != QMessageBox::Yes )
            {
               //cout <<"QMessageBox does not return yes! "<< endl;
               ce->ignore();
               return;
            }

   CloseAllWindows();

   Browser()->ToggleMonitoring(0);

   CloseAllFilesSlot();
   //cout <<"TGo4MainWindow::closeEvent after CloseAllFilesSlot" << endl;
   if(!RemoveAnalysisProxy(30)) {
      fCloseCounter = 100; // gui waits about 10 second to close analysis
      statusBar()->showMessage("Exit....  please wait");
      QApplication::setOverrideCursor(Qt::WaitCursor);
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
       //cout <<"TGo4MainWindow::closeEvent after QTimer, ignore close event" << endl;
       ce->ignore();
   } else {
      statusBar()->showMessage("Closing GUI...");

      ce->accept();
      gSystem->Exit( 0 );
   }
      //cout <<"TGo4MainWindow::closeEvent is finished." << endl;

}

void TGo4MainWindow::ForseCloseSlot()
{
   if (!RemoveAnalysisProxy(30) && (fCloseCounter>0)) {
      fCloseCounter--;
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
      return;
   }
   gSystem->Exit( 0 );
}

void TGo4MainWindow::OpenFileSlot()
{
   QFileDialog fd( this,
                   "Select a ROOT file to open it in the Go4 Disk Browser",
                   fLastFileDir,
                   QString("Root files (*.root);;Root xml files (*.xml);;All files (*.*)"));

   fd.setFileMode( QFileDialog::ExistingFiles);

   if ( fd.exec() != QDialog::Accepted ) return;

   QStringList list = fd.selectedFiles();
   QStringList::Iterator it = list.begin();
   while( it != list.end() ) {
      QString fileName = *it;
      fLastFileDir = QFileInfo(fileName).absolutePath();
      Browser()->OpenFile(fileName.toAscii());
      ++it;
   }
}

void TGo4MainWindow::OpenRemoteFileSlot()
{
   TGo4OpenRemoteDialog fd( this);
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

   fxOM->AddFile(fOMDataPath.toAscii(), fileName.toAscii());
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

   if (!Browser()->ConnectHServer(dlg.ServerName->text().toAscii(),
                                  dlg.PortNumberSpin->value(),
                                  dlg.BaseName->text().toAscii(),
                                  dlg.UserPassEdt->text().toAscii(),
                                  dlg.FilterList->text().toAscii()))
     QMessageBox::warning(0, "HServer", "Cannot connect to histogram server");
}

void TGo4MainWindow::SaveFileSlot()
{
   QFileDialog fd(this,
        go4sett->getFetchDataWhenSave() ?
        "Select root file to fetch and export all browser objects" :
        "Select root file to export all local browser objects",
        fLastFileDir, "ROOT (*.root)");

   fd.setFileMode( QFileDialog::AnyFile);
   fd.selectFile("export.root");

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fname = flst[0];
   fLastFileDir = fd.directory().path();
   if (fname.indexOf(".root", 0, Qt::CaseInsensitive)<0) fname+=".root";

   if (!Browser()->SaveBrowserToFile(fname.toAscii(), go4sett->getFetchDataWhenSave()))
     QMessageBox::warning(this, "Save data to file", "Specified file can not be created");
}

void TGo4MainWindow::CloseAllFilesSlot()
{
   fxOM->CloseFiles(fOMDataPath.toAscii());
}

void TGo4MainWindow::InputTerminalParametersSlot()
{
   int size = go4sett->getTermHistorySize();

   bool ok;

   size = QInputDialog::getInteger(this,
                                   "Analysis terminal history size in bytes (minimum 10000 bytes)",
                                   "Input 0 if full analysis history should be preserved",
                                   size, 0, 100000000, 10000, &ok);

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
   TGo4LogSettings dlg;
   if (dlg.exec() != QDialog::Accepted) return;

   if(dlg.LogfileWrite->isChecked())
      TGo4Log::OpenLogfile(dlg.LogfileName->text().toAscii(), "Logfile for Go4 GUI", true);

   TGo4Log::LogfileEnable(dlg.LogfileWrite->isChecked());
   TGo4Log::OutputEnable(dlg.LogfilePrint->isChecked());
   TGo4Log::SetIgnoreLevel(dlg.IgnoreCombo->currentIndex());
   TGo4Log::AutoEnable(dlg.ModeCombo->currentIndex());
}

void TGo4MainWindow::HelpWindow(const char* filename, const char* msg)
{
   QApplication::setOverrideCursor( Qt::WaitCursor );

   QString go4sys = getenv("GO4SYS");
   if (go4sys.length()==0) go4sys = ".";
   if (go4sys[go4sys.length()-1] != '/') go4sys+='/';

   QProcess info;

   info.start(go4sys + "etc/Go4ShowPdf.ksh " + go4sys + filename);
   if (info.waitForFinished(10000) && (info.exitCode()==0))
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
	go4sett->setAppFont(QApplication::font());

   go4sett->setAppStyle(QApplication::style()->objectName());

   go4sett->StoreSettings(this);

   go4sett->Store();

   cout <<"Using Qt Settings at "<< TGo4QSettings::GetSettLoaction().toAscii().constData() << endl;
}

void TGo4MainWindow::ChangeFontSlot()
{
   bool ok = false;
   QFont font= QFontDialog::getFont(&ok, QApplication::font(), this);
   if (!ok) return;

   QString infostring="Style " + font.toString() + "  was selected.\n";
   infostring += "One can try to apply it immediately, but Qt4 crashed often at that place :(\n";
   infostring += "One can save that font in settings and it will be applied next time Go4 is started";

   int res = QMessageBox::question( this, "Go4 GUI", infostring,
         QMessageBox::Apply | QMessageBox::Save | QMessageBox::Cancel ,
         QMessageBox::Save);

   if (res == QMessageBox::Cancel) return;

   go4sett->setAppFont(font);
   go4sett->Store();

   if (res == QMessageBox::Apply)
   	QApplication::setFont(font);
}

void TGo4MainWindow::SetStyleSlot(const QString &style)
{
   QString infostring="Style "+style+" was selected.\n";
   infostring += "One can try to apply it immediately, but Qt crashed very often at that place :(\n";
   infostring += "One can save that style in settings and it will be applied next time Go4 is started";

   int res = QMessageBox::question( this, "Go4 GUI", infostring,
         QMessageBox::Apply | QMessageBox::Save | QMessageBox::Cancel ,
         QMessageBox::Save);

   if (res == QMessageBox::Cancel) return;

   // save settings anyway
   go4sett->setAppStyle(style);
   go4sett->Store();

   if (res == QMessageBox::Apply)
      QApplication::setStyle(style);
}

void TGo4MainWindow::StatusMessage(const QString& mess)
{
   QString output = QDateTime::currentDateTime().toString(Qt::ISODate);
   output.append(": ");
   output.append(mess);
   statusBar()->showMessage(output, 3000);
   if(TGo4Log::IsAutoEnabled())
      TGo4Log::Message(1,output.toAscii());

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
   setWindowTitle(capt);

   bool flag = (pr==0);
   faLaunchAnal->setEnabled(flag);

   if (pr==0) flag = (pr==0);
         else flag = (fConnectingCounter<=0) && pr->IsAnalysisServer() && !pr->IsConnected();
   faConnectAnal->setEnabled(flag);

   faPrepareAnal->setEnabled(flag);

   if (pr==0) flag = false;
         else flag = pr->IsAnalysisServer() &&
                     (pr->IsConnected() || (fConnectingCounter<=0));
   faDisconnectAnal->setEnabled(flag);

   if (pr==0) flag = false;
         else flag = !pr->IsAnalysisServer() ||
                     (pr->IsConnected() && pr->IsAdministrator());
   faShutdownAnal->setEnabled(flag);

   bool iscontrolling = false;
   if (pr!=0)
     iscontrolling = pr->IsConnected() && (pr->IsAdministrator() || pr->IsController());
   faSumbStartAnal->setEnabled(iscontrolling);

   faStartAnal->setEnabled(iscontrolling);

   faStopAnal->setEnabled(iscontrolling);

   faAnalConfig->setEnabled(iscontrolling);

   flag = (FindAnalysisWindow()!=0);
   faAnalTermin->setEnabled(flag);
}

void TGo4MainWindow::ChangeFetchWhenDrawSlot()
{
   go4sett->setFetchDataWhenDraw(faFetchWhenDraw->isChecked());
}

void TGo4MainWindow::ChangeFetchWhenCopySlot()
{
   go4sett->setFetchDataWhenCopy(faFetchWhenCopy->isChecked());
}

void TGo4MainWindow::ChangeFetchWhenSaveSlot()
{
   go4sett->setFetchDataWhenSave(faFetchWhenSave->isChecked());
}

void TGo4MainWindow::CanvasColorSlot()
{
   QColor c = QColorDialog::getColor();
   if (c.isValid())
      go4sett->setCanvasColor(c.red(), c.green(), c.blue());
}

void TGo4MainWindow::MarkerSettingsSlot()
{
   TGo4MarkerSettings mset;
   if (mset.exec() != QDialog::Accepted) return;
   // all work is done inside dialog.
}

void TGo4MainWindow::OptStatsSlot()
{
   TGo4OptStatsSettings stset;
   if (stset.exec() != QDialog::Accepted) return;
   // all work is done inside dialog.
}

void TGo4MainWindow::CrosshairSlot()
{
   go4sett->setPadCrosshair(faCrosshair->isChecked());
}

void TGo4MainWindow::EventStatusSlot()
{
   go4sett->setPadEventStatus(faEventstatus->isChecked());
}

void TGo4MainWindow::ChangeCloneFlagSlot()
{
   bool s = faClone->isChecked();
   faDrawTime->setEnabled(s);
   faDrawDate->setEnabled(s);
   faDrawItem->setEnabled(s);
   go4sett->setCloneFlag(s);
}

void TGo4MainWindow::ChangeDrawTimeFlagSlot()
{
   go4sett->setDrawTimeFlag(faDrawTime->isChecked());
}

void TGo4MainWindow::ChangeDrawItemFlagSlot()
{
   go4sett->setDrawItemFlag(faDrawItem->isChecked());
}

void TGo4MainWindow::ChangeDrawDateFlagSlot()
{
   go4sett->setDrawDateFlag(faDrawDate->isChecked());
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

   if (!isserver) {
      TGo4AnalysisProxy* anal = AddAnalysisProxy(false, (termmode==1));
      if (anal!=0)
        res = anal->LaunchAsClient(launchcmd, killcmd,
                      (shellmode==2),
                      termmode,
                      go4sett->getClientName().toAscii(),
                      go4sett->getClientNode().toAscii(),
                      go4sett->getClientDir().toAscii(),
                      go4sett->getClientExec().toAscii());
      TGo4AnalysisWindow* anw = FindAnalysisWindow();
      if (res && (anw!=0) && (termmode==1))
         anw->StartAnalysisShell(launchcmd.Data());
   } else
      res = TGo4AnalysisProxy::LaunchAsServer(launchcmd, killcmd,
                      (shellmode==2),
                      termmode,
                      go4sett->getClientName().toAscii(),
                      go4sett->getClientNode().toAscii(),
                      go4sett->getClientDir().toAscii(),
                      go4sett->getClientExec().toAscii());

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
      fxOM->AddProxy(fOMDataPath.toAscii(), anal, analisysitem, "Analysis proxy");
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
        anw = new TGo4AnalysisWindow(fxWorkSpace, "AnalysisWindow", true);
        fxWorkSpace->addWindow(anw);
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
     QToolBar* dock = addToolBar("Analysis control");
     dock->setObjectName("AnalysisControlBar");
     bool showkill = !anal->IsAnalysisServer() || fKillCommand.length()>0;
     anw = new TGo4AnalysisWindow(this, "AnalysisWindow", false, showkill);
     ConnectGo4Widget(anw);
     dock->addWidget(anw);
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
      (FindGo4Widget("AnalysisStatusMonitor", false));

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
       conf = new TGo4AnalysisConfiguration(fxWorkSpace, "AnalysisConfiguration");
       fxWorkSpace->addWindow(conf);
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
        dlg.setPassword(fLastPassword.toAscii());
      if (dlg.exec()!=QDialog::Accepted) return;
      pass = dlg.getInput();
   }

   if (anal==0) anal = AddAnalysisProxy(true, false);
   bool def = go4sett->getClientDefaultPass();
   if (!def) fLastPassword = pass;

   if (anal!=0)
     anal->ConnectToServer(go4sett->getClientNode().toAscii(),
                           go4sett->getClientPort(),
                           go4sett->getClientControllerMode(),
                           def ? 0 : pass.toAscii().constData());
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
      killprocess->start(fKillCommand);
      QTimer::singleShot(10000, killprocess, SLOT(deleteLater()));
   } else
      StatusMessage("Can not kill analysis. Do it by OS commands");

   EstablishAnalysisConfiguration(0);

   EstablishRatemeter(0);
}

QGo4Widget* TGo4MainWindow::FindGo4Widget(const char* name, bool activate)
{
   if (fxOM==0) return 0;

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toAscii());

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
      fitpanel = new TGo4FitPanel(fxWorkSpace,"FitPanel");
      fxWorkSpace->addWindow(fitpanel);
      ConnectGo4Widget(fitpanel);
      fitpanel->ensurePolished();
      fitpanel->show();
   }
   return fitpanel;
}

TGo4HistogramInfo* TGo4MainWindow::StartHistogramInfo()
{
   TGo4HistogramInfo* hinfo = (TGo4HistogramInfo*) FindGo4Widget("HistogramInfo", true);
   if (hinfo==0) {
      hinfo = new TGo4HistogramInfo(fxWorkSpace, "HistogramInfo");
      fxWorkSpace->addWindow(hinfo);
      ConnectGo4Widget(hinfo);
      hinfo->ensurePolished();
      hinfo->show();
      hinfo->adjustSize();
   }
   return hinfo;
}

TGo4ConditionInfo* TGo4MainWindow::StartConditionInfo()
{
   TGo4ConditionInfo* cinfo = (TGo4ConditionInfo*) FindGo4Widget("ConditionInfo", true);
   if (cinfo==0) {
      cinfo = new TGo4ConditionInfo(fxWorkSpace, "ConditionInfo");
      fxWorkSpace->addWindow(cinfo);
      ConnectGo4Widget(cinfo);
      cinfo->ensurePolished();
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
      pedit = new TGo4ParaEdit(fxWorkSpace, "ParaEdit");
      fxWorkSpace->addWindow(pedit);
      ConnectGo4Widget(pedit);
      pedit->ensurePolished();
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
      dedit = new TGo4EditDynEntry(fxWorkSpace, "EditDynEntry");
      fxWorkSpace->addWindow(dedit);
      ConnectGo4Widget(dedit);
      dedit->ensurePolished();
      dedit->show();
   }

   return dedit;
}

TGo4ConditionEditor* TGo4MainWindow::StartConditionEditor()
{
   TGo4ConditionEditor* wedit = (TGo4ConditionEditor*) FindGo4Widget("ConditionEditor", true);
   if (wedit==0) {
      wedit = new TGo4ConditionEditor(fxWorkSpace, "ConditionEditor");
      fxWorkSpace->addWindow(wedit);
      ConnectGo4Widget(wedit);
      wedit->ensurePolished();
      wedit->show();
   }
   return wedit;
}

TGo4EventInfo* TGo4MainWindow::StartEventInfo()
{
   TGo4EventInfo* einfo = (TGo4EventInfo*) FindGo4Widget("EventInfo", true);

   if (einfo==0) {
      einfo = new TGo4EventInfo(fxWorkSpace, "EventInfo");
      fxWorkSpace->addWindow(einfo);
      ConnectGo4Widget(einfo);
      einfo->ensurePolished();
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
      QToolBar* dock = dynamic_cast<QToolBar*> (anw->parentWidget());
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
      QToolBar* bar = dynamic_cast<QToolBar*> (anw->parentWidget());
      removeToolBar(bar);
      delete bar;
   }
}


void TGo4MainWindow::ToggleMbsMonitor(const char* nodename)
{
   TGo4MBSViewer* mbs =
     dynamic_cast<TGo4MBSViewer*> (FindGo4Widget("MBSViewer", false));
   if (mbs==0) return;
   if (mbs->parentWidget()->isVisible())
       mbs->parentWidget()->hide();
   else {
     mbs->SetNode(nodename);
     mbs->parentWidget()->show();
   }
}


TGo4SetScaleValues* TGo4MainWindow::ToggleScaleValues()
{
   TGo4SetScaleValues* scl = 0;

   QWidgetList windows =  fxWorkSpace->windowList();
   for (int i=0; i<int(windows.count()); ++i ) {
      scl = dynamic_cast<TGo4SetScaleValues*> (windows.at(i));
      if (scl!=0) break;
   }

   if (scl==0) {
      scl = new TGo4SetScaleValues(fxWorkSpace, "ScaleValues", Qt::WindowStaysOnTopHint);
      fxWorkSpace->addWindow(scl);
      scl->ensurePolished();
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

//   cout << "Draw item " << itemname << " slot = " << guislot << endl;

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

   if (br->DefineFileObject(itemname, fileslotname, filepath)) {
      int answer = QMessageBox::question(this, "Writing object to file",
         QString("Overwrite ") + filepath + " in file " + fileslotname.Data(),
         "Overwrite", "Save to other file", "Cancel");
      if (answer==2) return false;
      if (answer==0)
        res = br->UpdateObjectInFile(itemname, fileslotname.Data(), filepath);
   }

   if (!res) {
      QFileDialog fd(this, QString("Save ") + itemname + " in root file",
                     fLastFileDir, "ROOT (*.root);;ROOT XML (*.xml)");
      fd.setFileMode( QFileDialog::AnyFile);
      if (fd.exec() == QDialog::Accepted) {
         QStringList flst = fd.selectedFiles();
         if (!flst.isEmpty()) {
            res = br->SaveItemToFile(itemname, flst[0].toAscii(), subfolder);
            fLastFileDir = fd.directory().path();
         }
      }
   }

   return res;
}

void TGo4MainWindow::SavePanelCanvas(TGo4ViewPanel* panel)
{
   if (panel==0) return;

   TCanvas* can = panel->GetCanvas();

   QFileDialog fd( this, QString("Save ") + panel->objectName() + " As", fLastFileDir);
   fd.setFileMode( QFileDialog::AnyFile );

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

   QStringList flt;

   flt << PS;
   flt << PS_Portrait;
   flt << PS_Landscape;
   flt << EPS;
   flt << EPS_Preview;
   flt << PDF;
   flt << SVG;
   flt << GIF;

   #if ROOT_VERSION_CODE >= ROOT_VERSION(4,4,2)
   flt << XPM;
   flt << PNG;
   flt << JPG;
   flt << TIFF;
   #endif

   flt << CXXM;
   flt << ROOTM;

   fd.setFilters(flt);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString filename = flst[0];
   QString filter = fd.selectedNameFilter();

   fLastFileDir = fd.directory().path();

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
   can->Print(filename.toAscii(), opt);
}

TGo4ObjectManager* TGo4MainWindow::OM()
{
   return fxOM;
}

TGo4BrowserProxy* TGo4MainWindow::Browser()
{
   return (TGo4BrowserProxy*) fxOM->GetProxy(fOMBrowserPath.toAscii());
}

TGo4Slot* TGo4MainWindow::GetWidgetTopSlot(QGo4Widget* widget, bool force)
{
   if (widget==0) return 0;

   QString EditorName = widget->objectName();

   QString editorslotname = fOMEditorsPath;
   editorslotname += "/";
   editorslotname += EditorName;
   TGo4Slot* edslot = fxOM->GetSlot(editorslotname.toAscii());
   if ((edslot==0) && force) {
      fxOM->AddProxy(fOMEditorsPath.toAscii(), new TGo4WidgetProxy(widget), EditorName.toAscii(), "Links for Go4 widget");
      edslot = fxOM->GetSlot(editorslotname.toAscii());
   }
   return edslot;
}

void TGo4MainWindow::checkPanelRepaintSlot()
{
   // block monitoring when start panel updates
   Browser()->SetMonitorBlockingFlag(kTRUE);

   fbPanelTimerActive = false;

   // first check if active viewpnael need update its content
   TGo4ViewPanel* actpanel = fxWorkSpace->GetActivePanel();
   if ((actpanel!=0) && actpanel->IsRepaintTimerActive())
      actpanel->checkRepaintSlot();

   if (fbPanelTimerActive) return;

   // now check if any other panel shold be repainted
   TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toAscii());
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
         if ((event==0) || !event->mimeData()->hasText()) return;

         QString eventstr = event->mimeData()->text();

         bool accept = false;
         if (event->source()==FindGo4Widget("Browser",false)) {
            int kind = Browser()->ItemKind(eventstr.toAscii());
            TClass* cl = Browser()->ItemClass(eventstr.toAscii());
            accept = editor->IsAcceptDrag(eventstr.toAscii(), cl, kind);
         }

         if (accept) event->acceptProposedAction();
         break;
      }

      case QGo4Widget::service_DropEvent: {
         QDropEvent* event = (QDropEvent*) par;

         if ((event==0) || !event->mimeData()->hasText()) return;

         QString eventstr = event->mimeData()->text();
         event->acceptProposedAction();

         if (event->source()==FindGo4Widget("Browser",false)) {
             int kind = Browser()->ItemKind(eventstr.toAscii());
             TClass* cl = Browser()->ItemClass(eventstr.toAscii());

             TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (editor);
             TGo4FitPanel* fitpanel = dynamic_cast<TGo4FitPanel*> (editor);
             if (panel!=0)
                panel->DropOnPad((TPad*)str, eventstr.toAscii(), cl, kind);
             else
             if (fitpanel!=0)
                fitpanel->DropOnPanel(event, eventstr.toAscii(), cl, kind);
             else
                editor->DropItem(eventstr.toAscii(), cl, kind);
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

         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toAscii());
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
         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toAscii());

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

         if (str==0) str = editor->objectName().toAscii().constData();

         TGo4Slot* link = fxOM->AddLink(brslot, edslot->GetFullName(),
                                        str, "link to browser item");
         if (link!=0)
            editor->SetLinkedName(link, (const char*)par);

         break;
      }

      case QGo4Widget::service_AddDirectLink: {
         //cout << " QGo4Widget::service_AddDirectLink " << endl;
         if (str==0) str = editor->objectName().toAscii().constData();
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
         TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toAscii());
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
         *res = fxWorkSpace->GetActivePanel();
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
         fxWorkSpace->ResponseOnPanelEvent(serviceid, panel, pad);
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
   QString fileName =
      QFileDialog::getSaveFileName(this,
                                   "Create GUI script dialog",
                                   fLastFileDir,
                                   QString("GUI hotstart script (*") + ext + ")");
   if (fileName.length()==0) return;

   int pos = fileName.indexOf(ext);

   if ((pos<0) || (pos!=fileName.length()-ext.length()))
     fileName+=ext;

   StatusMessage(QString("Generate script ")+fileName);

   TGo4Script::ProduceScript(fileName.toAscii(), this);
}

void TGo4MainWindow::ProcessQtEvents()
{
   if (fApp!=0) fApp->processEvents(QEventLoop::AllEvents, TGo4Script::DelayMillisec());
}

TGo4ViewPanel* TGo4MainWindow::FindViewPanel(const char* name)
{
   if ((name==0) || (*name==0)) return 0;

   TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (FindGo4Widget(name, false));

   if (panel!=0) return panel;

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toAscii());

   for (Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* widgslot = slot->GetChild(n);
      TGo4WidgetProxy* wproxy = widgslot==0 ? 0 : dynamic_cast<TGo4WidgetProxy*> (widgslot->GetProxy());
      panel = wproxy==0 ? 0 : dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

      if (panel!=0)
        if (strcmp(panel->GetPanelName(), name)==0) return panel;
   }

   return 0;
}
