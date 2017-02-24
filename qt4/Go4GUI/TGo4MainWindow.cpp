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

#include "qaction.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qclipboard.h"
#include "qstylefactory.h"
#include "qfileinfo.h"

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QHash>
#include <QtCore/QSignalMapper>
#include <QtCore/QDir>

#include <QApplication>
#include <QMenuBar>
#include <QDockWidget>
#include <QToolBar>
#include <QComboBox>
#include <QAction>
#include <QStatusBar>
#include <QStyle>
#include <QColor>
#include <QToolTip>
#include <QToolButton>
#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QMimeData>
#include <QDesktopServices>
#include <QUrl>


//////// root includes;
#include "Riostream.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
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
#include "TGo4MdiArea.h"
#include "TGo4Fitter.h"
#include "TGo4FitPanel.h"
#include "TGo4DynamicEntry.h"
#include "TGo4AnalysisStatus.h"

#include "TGo4ObjectManager.h"
#include "TGo4ObjectProxy.h"
#include "TGo4BrowserProxy.h"
#include "TGo4DabcProxy.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4WidgetProxy.h"



//////// Go4 includes from Go4GUI package:
#include "QGo4Widget.h"
#include "TGo4QSettings.h"
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
#include "TGo4HttpProxy.h"

#include "TGo4UserCommands.h"


#ifdef __GO4DIM__
#include "TGo4DabcMonitor.h"
#endif


#include "TGo4Script.h"

#include "TGo4HServerConnection.h"

using namespace Qt;

TGo4MainWindow::TGo4MainWindow(QApplication* app) :
   QMainWindow(),
   fApp(app)
{
   // setDockWindowsMovable(true);
   // statusBar();
   resize( 1152, 864 );

   setWindowIcon(QIcon(":/icons/go4logo2_big.png"));
   // setRightJustification( true );
   // setUsesTextLabel(true);
   // setOpaqueMoving(false);



   fxOM = new TGo4ObjectManager("GUI_OM","Gui object manager");
   fOMDataPath    = "data";
   fOMBrowserPath = "gui";
   fOMEditorsPath = "editors";

   fNewWidgetX = 0;
   fNewWidgetY = 0;

   fKillCommand = "";
   fLastPassword = "";
   fLastFileDir = QDir::currentPath();

   fCloseCounter = 0;
   fConnectingCounter = 0;
   fConnectingHttp = "";

   fbGetAnalysisConfig=true;

   fbPanelTimerActive = false;
   winMapper = 0;

   fbFullScreen = false;


    // create mount point for all data sources
   fxOM->MakeFolder(fOMDataPath.toLatin1().constData());

   // create entry for browser
   TGo4BrowserProxy* br = new TGo4BrowserProxy(fOMDataPath.toLatin1().constData(), fOMEditorsPath.toLatin1().constData(), kFALSE);
   fxOM->AddProxy("", br, fOMBrowserPath.toLatin1().constData(), "Place for gui slots");
   br->CreateMemoryFolder();

   TH1I* h1 = new TH1I("histo1","histo title", 100, -10., 10.);
   h1->FillRandom("gaus",10000);
   h1->SetDirectory(0);
   br->SaveToMemory("", h1, kTRUE);

/*   TH2I* h2 = new TH2I("histo2","histo 2 title", 100, -10., 10., 100, -10., 10.);
   h2->FillRandom("gaus",100000);
   h2->SetDrawOption("hist");
   h2->SetDirectory(0);
   br->SaveToMemory("", h2, kTRUE);
*/
   // br->OpenFile("asf.root");

   new TGo4Script(this);

   gStyle->SetPalette(1);
   gStyle->SetCanvasColor(10); // no effect, can be set in settings
   gStyle->SetPadColor(10); // no effect
   gStyle->SetFrameFillColor(10); // works if not 0
   gStyle->SetStatColor(0); // works

   //gStyle->SetOptStat(11111111);
   gStyle->SetOptStat(go4sett->getOptStat());

   fxMdiArea = new TGo4MdiArea(this);
   fxMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   fxMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   setCentralWidget(fxMdiArea);
   fxMdiArea->setFocus();

   AddFileMenu();
   AddFileToolBar();

   AddToolsMenu();
   AddToolsBar();

   AddAnalysisMenu();
   AddAnalysisBar();

   UpdateCaptionButtons();

   QDockWidget* BrowserDockWin = new QDockWidget("Browser", this);
   BrowserDockWin->setObjectName("BrowserDock");
   TGo4Browser* browser = new TGo4Browser(BrowserDockWin,"Browser");
   browser->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(browser);
   browser->StartWorking();
   BrowserDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   BrowserDockWin->setWidget(browser);
   BrowserDockWin->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
   addDockWidget(Qt::LeftDockWidgetArea, BrowserDockWin);

   QAction* BrowserDockAction=BrowserDockWin->toggleViewAction();
   BrowserDockAction->setShortcut(QKeySequence("F6"));


#if (QT_VERSION >= 0x040700) && (QT_VERSION <= 0x040909)
   browser->setMinimumWidth(230);
#endif

   QDockWidget* LogDockWin = new QDockWidget("Log window", this);
   LogDockWin->setObjectName("LogInfoDock");
   TGo4LogInfo* loginfo = new TGo4LogInfo(this, "LogInfo");
   ConnectGo4Widget(loginfo);
   LogDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   LogDockWin->setWidget(loginfo);
   LogDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, LogDockWin);

   QAction* LogDockWinAction=LogDockWin->toggleViewAction();
   LogDockWinAction->setShortcut(QKeySequence("F7"));

   QDockWidget* MBSDockWin = new QDockWidget("MBS monitor", this);
   MBSDockWin->setObjectName("MbsViewerDock");
   TGo4MBSViewer* mbs = new TGo4MBSViewer(MBSDockWin, "MBSViewer");
   mbs->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(mbs);
   MBSDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   MBSDockWin->setWidget(mbs);
   MBSDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, MBSDockWin);

   QAction* MBSDockAction=MBSDockWin->toggleViewAction();
   MBSDockAction->setShortcut(QKeySequence("F8"));


#ifdef __GO4DIM__
   QDockWidget* DABCDockWin = new QDockWidget("DABC monitor", this);
   DABCDockWin->setObjectName("DABCMonitorDock");
   TGo4DabcMonitor* dabc = new TGo4DabcMonitor(DABCDockWin, "DabcMonitor");
   dabc->setWindowFlags(Qt::Widget);
   ConnectGo4Widget(dabc);
   DABCDockWin->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
   DABCDockWin->setWidget(dabc);
   DABCDockWin->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::BottomDockWidgetArea, DABCDockWin);
   QAction* DABCDockWinAction=DABCDockWin->toggleViewAction();
   DABCDockWinAction->setShortcut(QKeySequence("F9"));


#endif

   QToolBar* DividePanelBar = addToolBar("Canvas Tools");
   DividePanelBar->setObjectName("DividePadDock");
   TGo4DividePad* dividepanel = new TGo4DividePad(this, "DividePad");
   DividePanelBar->addWidget(dividepanel);

   QToolBar* stylebar = addToolBar("Color Tools");
   stylebar->setObjectName("Go4StyleDock");
   fxStyle = new TGo4Style(this, "Go4Style");
   stylebar->addWidget(fxStyle);

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
   DrawOptionPanel->adjustSize();

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
   ConnectGo4Widget(cli);
   commandlinebar->addWidget(cli);

   QToolBar* tviewerdock = addToolBar("Tree viewer");
   tviewerdock->setObjectName("TreeViewerDock");
   TGo4TreeViewer* tviewer = new TGo4TreeViewer(this, "TreeViewer");
   ConnectGo4Widget(tviewer);
   tviewerdock->addWidget(tviewer);

   AddAnalysisMacrosBar(); // new 2016


   // test: can we implement shortcuts to enable specific dock windows JAM?
//   QMenu* dockMenu = menuBar()->addMenu("&Docks");
//   dockMenu->addAction(BrowserDockAction);
//   dockMenu->addAction(LogDockWinAction);
//   dockMenu->addAction(MBSDockAction);
// do not need this, since shortcuts also work in default dock menu :)

   // make it here while only here exists all toolbars and dock widgets
   AddSettingMenu();

   windowsMenu = menuBar()->addMenu("&Windows");
   connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(windowsMenuAboutToShow()));
   // need to create menu item with F11
   windowsMenuAboutToShow();

   menuBar()->addSeparator();

   QMenu* helpMenu = menuBar()->addMenu("&Help");
   helpMenu->addAction("&Introduction (user manual)", this, SLOT(IntroHelpSlot()), Key_F1);
   helpMenu->addAction("&Reference manual", this, SLOT(RefHelpSlot()));
   helpMenu->addAction("&Fit Tutorial", this, SLOT(FitHelpSlot()));
   helpMenu->addSeparator();
   helpMenu->addAction("About &Qt", this, SLOT(aboutQt()), Key_F2 );
   helpMenu->addAction("About R&OOT", this, SLOT(aboutROOT()), Key_F3);
   if (TGo4DabcProxy::GetDabcVersion())
      helpMenu->addAction("About &DABC", this, SLOT(aboutDABC()));
   helpMenu->addAction("About &Go4", this, SLOT(about()), Key_F4);

   const char* libs = gSystem->Getenv("GO4USERLIBRARY");

   if (libs!=0) {
      QStringList LibList = QString(libs).split(":",QString::SkipEmptyParts);

      for (QStringList::Iterator it=LibList.begin(); it!=LibList.end(); ++it)
          gSystem->Load((*it).toLatin1().constData());
   }

   go4sett->restoreMainWindowState(this);

   QApplication::setStyle(go4sett->getAppStyle());

   faSuperimpose->setChecked(go4sett->getPadSuperimpose());
   faCrosshair->setChecked(go4sett->getPadCrosshair());
   faEventstatus->setChecked(go4sett->getPadEventStatus());

   go4sett->getBasicSettings();
   go4sett->getAppFont();

   // start mbs monitoring only after browser etc. is fully there:
   if(go4sett->getMbsMonitorMonitorActive()) mbs->TimerStart();

   std::cout <<"Using Qt settings at "<< go4sett->GetSettLoaction().toLatin1().constData() << std::endl;

   statusBar()->showMessage("Ready");
   statusBar()->setSizeGripEnabled(true);

   QString sfmt = go4sett->getGStyleStatFormat();
   if (!sfmt.isEmpty()) gStyle->SetStatFormat(sfmt.toLatin1().constData());
}

TGo4MainWindow::~TGo4MainWindow()
{
   TGo4Script::DeleteInstance();

   delete fxOM;
}

const char* TGo4MainWindow::LastTypedPassword() const
{
   return fLastPassword.toLatin1().constData();
}


void TGo4MainWindow::about()
{
   QString GO4STR("Go4  ");
   GO4STR.append ( __GO4RELEASE__);
   QString GO4STR1 = GO4STR;
   GO4STR1.append("<br>");
   GO4STR1.append("The Go4 (GSI Object Oriented On-line Off-line system) is based on ROOT<br> with the specific requirements of the low and medium energy nuclear and atomic<br> physics experiments implemented as extensions.<br> See <a href='http://go4.gsi.de'>http://go4.gsi.de</a> for more information") ;

   QMessageBox AboutGo4(GO4STR, GO4STR1, QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutGo4.setIconPixmap(QPixmap( ":/icons/go4logo2_big.png"));
   AboutGo4.setTextFormat(Qt::RichText);
   AboutGo4.exec();
}

void TGo4MainWindow::aboutQt()
{
   QMessageBox::aboutQt(this, "Qt");
}

void TGo4MainWindow::aboutROOT()
{
   QString mestring("This Go4 uses ROOT ");
   mestring.append(gROOT->GetVersion());
   mestring.append("<br> ROOT is an object-oriented framework <br>for large scale scientific <br>data analysis and data mining.<br> It has been developed at CERN<br> See <a href='http://root.cern.ch'>http://root.cern.ch</a> for more information");
   QMessageBox AboutRoot("ROOT", mestring,
                         QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutRoot.setIconPixmap(QPixmap( ":/icons/root.png"));
   AboutRoot.setTextFormat(Qt::RichText);
   AboutRoot.exec();
}

void TGo4MainWindow::aboutDABC()
{
   QString mestring("This Go4 uses DABC ");
   if (TGo4DabcProxy::GetDabcVersion())
      mestring.append(TGo4DabcProxy::GetDabcVersion());
   else
      mestring.append("<undef>");
   mestring.append("<br>DABC (Data Acquisition Backbone Core) is <br> framework to build multi-threaded multi-node DAQ applications.<br> It is developed in GSI.<br> See <a href='http://dabc.gsi.de'>http://dabc.gsi.de</a> for more information");
   QMessageBox AboutDabc("DABC", mestring,
                         QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton ,this);
   AboutDabc.setIconPixmap(QPixmap( ":/icons/dabc.png"));
   AboutDabc.setTextFormat(Qt::RichText);
   AboutDabc.exec();
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

   faHideEventElement = AddChkAction(prefMenu, "Hide TGo4EventElement",
                    go4sett->getHideTGo4EventElement(), this, SLOT(ChangeHideEventElement()));

   faDrawOnce = AddChkAction(prefMenu, "Draw item once",
                             go4sett->getDrawOnceFlag(), this, SLOT(ChangeDrawOnceSlot()));

   faWindowRubberBand = AddChkAction(prefMenu, "Rubberband when moving windows",
                                go4sett->getMoveSubwindowRubberBand(), this, SLOT(ChangeWindowRubberBandSlot()));

#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
   prefMenu->addAction("GUI HighDPI Scale factor...", this, SLOT(ScaleFactorSlot()));
#endif

   QMenu* panelMenu = settMenu->addMenu("&Panel defaults");

   panelMenu->addAction("&Canvas color...", this, SLOT(CanvasColorSlot()));

   faSaveWhite=AddChkAction(panelMenu, "White canvas for saved images",
          go4sett->getSavePadWhiteBackground(), this, SLOT(ChangeSaveWhiteBackgroundSlot()));

   panelMenu->addAction("Marker labels...", this, SLOT(MarkerSettingsSlot()));
   panelMenu->addAction("Statistics box...", this, SLOT(OptStatsSlot()));

   faSuperimpose = AddChkAction(panelMenu, "&Superimpose mode",
                   go4sett->getPadSuperimpose(), this, SLOT(SuperimposeSlot()));

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

   panelMenu->addAction("Draw line &width ...", this, SLOT(DrawLineWidthSlot()));
   panelMenu->addAction("Draw fill color ...", this, SLOT(DrawFillColorSlot()));
   panelMenu->addAction("Draw fill style ...", this, SLOT(DrawFillStyleSlot()));
   panelMenu->addAction("TH1 draw opt ...", this, SLOT(TH1DrawOptSlot()));
   panelMenu->addAction("TH2 draw opt ...", this, SLOT(TH2DrawOptSlot()));
   panelMenu->addAction("TH3 draw opt ...", this, SLOT(TH3DrawOptSlot()));
   panelMenu->addAction("TGraph draw opt ...", this, SLOT(TGraphDrawOptSlot()));
   panelMenu->addAction("Printf format ...", this, SLOT(GStyleStatFormatSlot()));
   panelMenu->addAction("Palette settings ...", this, SLOT(PaletteSettingsSlot()));




   settMenu->addAction("&Log actions...", this, SLOT(LogSettingsSlot()));
   QMenu* termMenu = settMenu->addMenu("&Terminal");
   faTermTimeStamp = AddChkAction(termMenu, "Print timestamps",
                         go4sett->getTermShowTimestamp(), this, SLOT(ChangeTerminalTimeStampSlot()));
   termMenu->addAction("&Timestamp Format...", this, SLOT(ChangeTerminalTimeStampFormatSlot()));
   termMenu->addAction("&History...", this, SLOT(InputTerminalParametersSlot()));
   termMenu->addAction("&Font...", this, SLOT(ChangeTerminalFontSlot()));


   settMenu->addAction("Generate &hotstart", this, SLOT(CreateGUIScriptSlot()));
   settMenu->addAction("&Break hotstart execution", this, SLOT(StopGUIScriptSlot()));


   settMenu->addAction("&Save settings", this, SLOT(SaveSettingsSlot()));

   QActionGroup *ag = new QActionGroup(this);
   ag->setExclusive( true );
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
}

void TGo4MainWindow::AddFileMenu()
{
   QMenu* fileMenu = menuBar()->addMenu("&File");

   fileMenu->addAction(QIcon( ":/icons/open.png" ), "&Open...",
             this, SLOT(OpenFileSlot()), CTRL+Key_O );
   fileMenu->addAction(QIcon( ":/icons/network.png" ), "Open &Remote...",
             this, SLOT(OpenRemoteFileSlot()), CTRL+Key_R );
   if (TGo4DabcProxy::GetDabcVersion())
      fileMenu->addAction(QIcon( ":/icons/dabc.png" ), "Connect &DABC...",
                this, SLOT(ConnectDabcSlot()) );
   fileMenu->addAction(QIcon( ":/icons/http.png" ), "Connect &HTTP...",
             this, SLOT(ConnectHttpSlot()) );
   fileMenu->addAction(QIcon( ":/icons/histserv.png" ), "Open HIST &Server...",
             this, SLOT(ConnectHServerSlot()) );
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

   if (TGo4DabcProxy::GetDabcVersion())
   FileBar->addAction( QIcon( ":/icons/dabc.png" ), "Connect to DABC server",
                        this, SLOT(ConnectDabcSlot()));

   FileBar->addAction( QIcon( ":/icons/http.png" ), "Connect to HTTP server",
                        this, SLOT(ConnectHttpSlot()));

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

   toolMenu->addAction(QIcon(":/icons/chart.png" ), "&View panel",
                this, SLOT(MakeNewPanel()), CTRL+Key_V );
   toolMenu->addAction(QIcon(":/icons/fitpanel.png" ), "&Fit panel...",
                this, SLOT(StartFitPanel()), CTRL+Key_F );
   toolMenu->addAction(QIcon(":/icons/hislist.png" ),"&Histogram properties...",
               this, SLOT(StartHistogramInfo()));
   toolMenu->addAction(QIcon(":/icons/hiscre.png" ), "Create New H&istogram...",
               this, SLOT(CreateNewHistSlot()), CTRL+Key_I );
   toolMenu->addAction(QIcon(":/icons/condlist.png" ),"C&ondition properties...",
               this, SLOT(StartConditionInfo()));
   toolMenu->addAction(QIcon(":/icons/condcre.png" ),"Create new &condition...",
               this, SLOT(CreateNewConditionSlot()));
   toolMenu->addAction(QIcon(":/icons/zoom.png" ),"&Event printout...",
               this, SLOT(StartEventInfo()));
   toolMenu->addAction(QIcon(":/icons/dynlist.png" ),"Create &dyn. list entry ...",
               this, SLOT(CreateNewDynEntrySlot()), CTRL+Key_D);
   toolMenu->addAction(QIcon(":/icons/dllicon.png" ), "Load li&braries ...",
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
             this, SLOT(PrepareForClientConnectionSlot()));

   faDisconnectAnal =
      anMenu->addAction(QIcon(":/icons/disconnect.png"), "Disconnect from analysis",
             this, SLOT(DisconnectAnalysisSlot()), CTRL+Key_M);

   faShutdownAnal =
      anMenu->addAction(QIcon(":/icons/shutanal.png"), "Shutdown analysis",
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

    bool on = ! fxMdiArea->subWindowList().isEmpty();

    //windowsMenu->addAction("Ca&scade", this, SLOT(CascadeSubWindows()))->setEnabled(on);
    windowsMenu->addAction("Ca&scade", fxMdiArea, SLOT(cascadeSubWindows()))->setEnabled(on);
    windowsMenu->addAction("&Tile", fxMdiArea, SLOT(tileSubWindows()))->setEnabled(on);
    windowsMenu->addAction("&Close all", fxMdiArea, SLOT(closeAllSubWindows()))->setEnabled(on);
    windowsMenu->addAction("&Minimize all", this, SLOT(MinAllWindows()))->setEnabled(on);

    windowsMenu->addAction((fbFullScreen ? "&Normal window" : "&Full screen"), this, SLOT(ToggleFullScreenSlot()), Key_F11);

    windowsMenu->addSeparator();

    QGo4Widget* loginfo = FindGo4Widget("LogInfo", false);
    if (loginfo!=0) {
       windowsMenu->addAction("Save L&ogwindow", loginfo, SLOT(SaveLogInfo()));
       windowsMenu->addAction("Clear &Logwindow", loginfo, SLOT(ClearLogInfo()));
    } else {
       windowsMenu->addAction("Save L&ogwindow")->setEnabled(false);
       windowsMenu->addAction("Clear &Logwindow")->setEnabled(false);
    }

    QGo4Widget* anw = FindGo4Widget("AnalysisWindow", false);
    if (anw!=0) {
       windowsMenu->addAction("Save &Analysis window", anw, SLOT(SaveAnalysisOutput()));
       windowsMenu->addAction("Clear Analysis &window", anw, SLOT(ClearAnalysisOutput()));
    } else {
       windowsMenu->addAction("Save &Analysis window")->setEnabled(false);
       windowsMenu->addAction("Clear Analysis &window")->setEnabled(false);
    }

    windowsMenu->addSeparator();

    delete winMapper;
    winMapper = new QSignalMapper(this);
    connect(winMapper, SIGNAL(mapped(int)), this, SLOT(windowsMenuActivated(int)));

    QList<QMdiSubWindow *> windows = fxMdiArea->subWindowList();
    for (int i=0; i< windows.count(); i++ ) {
       QAction* act = new QAction(windows.at(i)->widget()->windowTitle(), winMapper);
       act->setCheckable(true);
       act->setChecked(fxMdiArea->activeSubWindow() == windows.at(i));

       windowsMenu->addAction(act);

       connect(act, SIGNAL(triggered()), winMapper, SLOT(map()) );
       winMapper->setMapping(act, i);
    }
}

void TGo4MainWindow::MinAllWindows()
{
   QList<QMdiSubWindow *> windows = fxMdiArea->subWindowList();
   for ( int i = 0; i < windows.count(); i++ )
       windows.at(i)->widget()->showMinimized();
}

void TGo4MainWindow::CascadeSubWindows()
{
   QList<QMdiSubWindow *> windows = fxMdiArea->subWindowList();

   int x(15),y(15);

   for ( int i = 0; i < windows.count(); i++ ) {
      windows.at(i)->move(x, y);
      x+=25; y+=25;
   }
}


void TGo4MainWindow::ToggleFullScreenSlot()
{
   if (fbFullScreen) showNormal();
               else showFullScreen();
   fbFullScreen = !fbFullScreen;
}


void TGo4MainWindow::windowsMenuActivated( int id )
{
   QList<QMdiSubWindow *> windows = fxMdiArea->subWindowList();
   if ((id>=0) && (id<windows.count())) {
      windows.at(id)->widget()->showNormal();
      windows.at(id)->widget()->setFocus();
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
     libname = "libGo4UserGui";

   TStartUserGuiFunc startfunc = 0;

   libname = dirname + libname;

   std::cout << "Try usergui: " << libname.toLatin1().constData() << std::endl;

   bool loaded = false;

   if (gSystem->Load(libname.toLatin1().constData())>=0) {
      loaded = true;
      startfunc = (TStartUserGuiFunc) gSystem->DynFindSymbol(libname.toLatin1().constData(), "StartUserPanel");
   }

   if (startfunc!=0) {
      QGo4Widget* userpanel = (QGo4Widget*) startfunc(fxMdiArea);
      if (userpanel!=0) {
         userpanel->setObjectName("UserPanel");

         QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(userpanel);
         CascadeMdiPosition(sub);
         ConnectGo4Widget(userpanel);
         userpanel->ensurePolished();
         sub->show();
         result = true;
      } else
         StatusMessage("StartUserPanel() function did not create user widget");
   }

   if (result) {
      std::cout << "Start user GUI from ";
      if (dirname.length()>0)
         std::cout << dirname.toLatin1().constData() << std::endl;
      else
         std::cout << "$LD_LIBRARY_PATH=" << gSystem->Getenv("LD_LIBRARY_PATH") << std::endl;
   } else {
      if (loaded) gSystem->Unload(libname.toLatin1().constData());
   }

   return result;
}

void TGo4MainWindow::UserPanelSlot()
{
   QGo4Widget* userpanel = FindGo4Widget("UserPanel", true);
   if (userpanel!=0) return;

   // check GO4USERGUI variable
   if (gSystem->Getenv("GO4USERGUI")!=0)
     if (startUserGUI(gSystem->Getenv("GO4USERGUI"))) return;

   // check from standard LD_LIBRARY_PATHS
   if (startUserGUI(0)) return;

   if (startUserGUI(TGo4Log::subGO4SYS("qt4/Go4UserGUI").Data())) return;

   QMessageBox::critical(this,"Starting user GUI", "No suitable libraries found");
}


TGo4ViewPanel* TGo4MainWindow::MakeNewPanel(int ndiv)
{
   QString name;

   TGo4Slot* edslot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());

   int n = 0;
   do {
      n++;
      name = QString("Panel") + QString::number(n);
   } while ((edslot!=0) && (edslot->FindChild(name.toLatin1().constData())!=0));

   TGo4ViewPanel* panel = new TGo4ViewPanel(fxMdiArea, name.toLatin1().constData());
   QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(panel); // warning: Qt may exchange the winId here!
   // panel->GetQCanvas()->performResize(); // may register new winId for TCanvas here


   sub->resize(go4sett->lastPanelSize("ViewPanel"));

   CascadeMdiPosition(sub);

   ConnectGo4Widget(panel);
   panel->update();
   panel->show();
   panel->ensurePolished();
   panel->CompleteInitialization();

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

void TGo4MainWindow::CascadeMdiPosition(QWidget* sub)
{
   //if ((sub->x() + sub->width() < fxMdiArea->viewport()->width()) &&
       //(sub->y() + sub->height() < fxMdiArea->viewport()->height())) return;

   int newx = fNewWidgetX;
   int newy = fNewWidgetY;
   if (newx + sub->width() > fxMdiArea->width()) newx = 0;
   if (newy + sub->height() > fxMdiArea->height())  newy = 0;
   sub->move(newx, newy);
   fNewWidgetX = sub->x() + 30;
   fNewWidgetY = sub->y() + 30;
}


void TGo4MainWindow::closeEvent( QCloseEvent* ce)
{
   if (fCloseCounter!=0) return;

// new for Qt4:
   if(QMessageBox::question( this, "Go4 GUI", "Really Exit Go4?",
         QMessageBox::Yes | QMessageBox::No ,
         QMessageBox::Yes) != QMessageBox::Yes ) {
            //std::cout <<"QMessageBox does not return yes! "<< std::endl;
            ce->ignore();
            return;
      }

   //fxMdiArea->closeAllSubWindows();

   Browser()->ToggleMonitoring(0);
   fxMdiArea->closeAllSubWindows();
   CloseAllFilesSlot();

   bool realshutdown = false;

   // TODO: terminate HTTP server running in Qt window
   TGo4AnalysisWindow* an = FindAnalysisWindow();
   if (an && an->TerminateOnClose()) realshutdown = true;

   StopGUIScriptSlot();
   int waitsecs = 180;
   if(!RemoveAnalysisProxy(waitsecs, realshutdown)) {
      //std::cout <<"closeEvent - RemoveAnalysisProxy returned false, using close counter" << std::endl;
      fCloseCounter = (waitsecs+10) *10; // was 100 gui waits about 10 second to close analysis
      statusBar()->showMessage("Exit....  please wait");
      QApplication::setOverrideCursor(Qt::WaitCursor);
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
      //std::cout <<"TGo4MainWindow::closeEvent after QTimer, ignore close event" << std::endl;
      ce->ignore();
   } else {
      statusBar()->showMessage("Closing GUI...");
      // JAM: due to problems with Qt5, we just use delayed exit here
      // note that calling directly gSystem->Exit gives crash in ROOT object/pad cleanup...
      // probably because fxMdiArea->closeAllSubWindows() will have effect only after this event handler returns
      QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
      ce->ignore();
   }
}

void TGo4MainWindow::ForseCloseSlot()
{
   //   if (!RemoveAnalysisProxy(30) && (fCloseCounter>0)) {

   if (Browser()->FindServer()!=0) {

      if (fCloseCounter>0) {
         fCloseCounter--;
         QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
         return;
      }

      QMessageBox box(QMessageBox::Critical, "Exit GUI", "Analysis is not (yet) shutdown correctly");
      QPushButton *wait_btn = box.addButton("Wait 10 s more", QMessageBox::AcceptRole);
      QAbstractButton *kill_btn = box.addButton("Kill analysis", QMessageBox::DestructiveRole);
      QAbstractButton *exit_btn = box.addButton("Exit immediately", QMessageBox::DestructiveRole);
      QAbstractButton *cancel_btn = box.addButton("Cancel quit", QMessageBox::RejectRole);
      box.setDefaultButton(wait_btn);

      box.exec();

      if (box.clickedButton() == wait_btn) {
         fCloseCounter = 100;
         QTimer::singleShot(100, this, SLOT(ForseCloseSlot()));
         return;
      }

      if ((box.clickedButton() == cancel_btn) || (box.clickedButton() == 0)) {
         std::cout << "Keep GUI running, press exit once again" << std::endl;
         return;
      }

      if (box.clickedButton() == kill_btn) {
         std::cout << "Killing analysis" << std::endl;
         TerminateAnalysis(false);
      }

      if (box.clickedButton() == exit_btn) {
         std::cout << "GUI closed with analysis still running - may lead to analysis task running forever" << std::endl;
         std::cout << "Please check running processes with \"ps\" and probably, kill analysis with \"killall go4analysis\" command" << std::endl;
      }
   }
   std::cout << "----- Exiting Go4 GUI now -----" << std::endl;
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
      Browser()->OpenFile(fileName.toLatin1().constData());
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

   fxOM->AddFile(fOMDataPath.toLatin1().constData(), fileName.toLatin1().constData());
}

void TGo4MainWindow::ConnectDabcSlot()
{
   bool ok = false;
   QString dabcnode = QInputDialog::getText(
      this, "Establish connection with DABC", "Provide dabc server name",
      QLineEdit::Normal, QString::null, &ok);
   if (!ok) return;

   if (!Browser()->ConnectDabc(dabcnode.toLatin1().constData()))
      QMessageBox::warning(0, "DABC server", "Cannot connect to DABC server");
}

TGo4ServerProxy* TGo4MainWindow::ConnectHttpSlot(const char* addr, const char* user, const char* pass, bool with_qt_process, bool get_analysis_config)
{
   QString httpaddr;

   if (addr==0) {
      bool ok = false;
      QString portstring;
      QString fulladdress = go4sett->getClientNode().append(QString(":")).append(portstring.setNum(go4sett->getClientPort()));
      httpaddr = QInputDialog::getText(
      this, "Establish connection with HTTP", "Provide http server name",
      QLineEdit::Normal, fulladdress, &ok);
      if (!ok) return 0;
      QStringList nameportlist = httpaddr.split(":");
      int nameindex=0;
      int portindex=1;
      if(nameportlist.at(0).contains("http")) {
          nameindex=1;
          portindex=2;
        }
      // first check here if we have valid port number:
      if(nameportlist.size()<portindex+1 || nameportlist.at(portindex).isEmpty()){
          QMessageBox::warning(0, "HTTP server connection", "Could not connect. Please specify port number!");
        return 0;
      }
      QString host = nameportlist.at(nameindex);
      QStringList hostnamelist = nameportlist.at(nameindex).split("//");
      if(hostnamelist.size()>1) host = hostnamelist.at(1); // get rid of optional leading // of full http adress
      go4sett->setClientNode(host);
      go4sett->setClientPort(nameportlist.at(portindex).toInt());
      go4sett->setClientConnectMode(1);

   } else {
      httpaddr = addr;
   }

   TGo4Script* exec = TGo4Script::ScriptInstance();

   if (!exec) return 0;

   TGo4ServerProxy* serv = exec->ConnectHttp(httpaddr.toLatin1().constData(), user, pass);

   if (serv) StatusMessage(QString("Connect with http server %1 %2").arg(httpaddr).arg(serv->IsGo4Analysis() ? " as GO4 analysis" : ""));

   if (serv && serv->IsGo4Analysis()) {
      EstablishRatemeter(2);
      if (!serv->IsViewer()) {
             EstablishAnalysisConfiguration(get_analysis_config ? 3 : 2);
           }


      TGo4LogInfo* loginfo = (TGo4LogInfo*) FindGo4Widget("LogInfo", false);
      if (loginfo!=0) loginfo->WorkWithInfo(serv->LoginfoSlot());

      if (!with_qt_process) EstablishAnalysisWindowForHttp();
   }

   return serv;
}


void TGo4MainWindow::ConnectHServerSlot()
{
   TGo4HServerConnection dlg( this, "Connect to remote HServer", true );

   dlg.ServerName->setText(go4sett->getHServName());
   dlg.BaseName->setText(go4sett->getHServBase());
   dlg.FilterList->setText(go4sett->getHServFilter());
   dlg.PortNumberSpin->setValue(go4sett->getHServPort());

   if (dlg.exec() != QDialog::Accepted) return;

   go4sett->setHServName(dlg.ServerName->text());
   go4sett->setHServBase(dlg.BaseName->text());
   go4sett->setHServFilter(dlg.FilterList->text());
   go4sett->setHServPort(dlg.PortNumberSpin->value());

   if (!Browser()->ConnectHServer(dlg.ServerName->text().toLatin1().constData(),
                                  dlg.PortNumberSpin->value(),
                                  dlg.BaseName->text().toLatin1().constData(),
                                  dlg.UserPassEdt->text().toLatin1().constData(),
                                  dlg.FilterList->text().toLatin1().constData()))
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
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.selectFile("export.root");

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fname = flst[0];
   fLastFileDir = fd.directory().path();
   if (fname.indexOf(".root", 0, Qt::CaseInsensitive)<0) fname+=".root";

   if (!Browser()->SaveBrowserToFile(fname.toLatin1().constData(), go4sett->getFetchDataWhenSave()))
     QMessageBox::warning(this, "Save data to file", "Specified file can not be created");
}

void TGo4MainWindow::CloseAllFilesSlot()
{
   fxOM->CloseFiles(fOMDataPath.toLatin1().constData());
}

void TGo4MainWindow::InputTerminalParametersSlot()
{
   int size = go4sett->getTermHistorySize();

   bool ok;

#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   size = QInputDialog::getInt(this,
                               "Analysis terminal history size in bytes (minimum 10000 bytes)",
                                "Input 0 if full analysis history should be preserved",
                                size, 0, 100000000, 10000, &ok);
#else
  size = QInputDialog::getInteger(this,
                               "Analysis terminal history size in bytes (minimum 10000 bytes)",
                                "Input 0 if full analysis history should be preserved",
                                size, 0, 100000000, 10000, &ok);
#endif


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
      TGo4Log::OpenLogfile(dlg.LogfileName->text().toLatin1().constData(), "Logfile for Go4 GUI", true);

   TGo4Log::LogfileEnable(dlg.LogfileWrite->isChecked());
   TGo4Log::OutputEnable(dlg.LogfilePrint->isChecked());
   TGo4Log::SetIgnoreLevel(dlg.IgnoreCombo->currentIndex());
   TGo4Log::AutoEnable(dlg.ModeCombo->currentIndex());
}

void TGo4MainWindow::HelpWindow(const char* filename, const char* msg)
{
   QApplication::setOverrideCursor( Qt::WaitCursor );

   QString surl = TGo4Log::subGO4SYS(filename).Data();

   bool result = QDesktopServices::openUrl(QUrl::fromLocalFile(surl));

   if(result)
      StatusMessage(msg ? QString(msg) : QString("Show ") + filename);
   else
      StatusMessage(QString("Fail to display ") + filename);

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
   go4sett->setBasicSettings();
   go4sett->setAppFont(QApplication::font());

   go4sett->setAppStyle(QApplication::style()->objectName());

   go4sett->storeMainWindowState(this);

   go4sett->Store();

   std::cout <<"Save Qt settings to "<< go4sett->GetSettLoaction().toLatin1().constData() << std::endl;
}

void TGo4MainWindow::ChangeFontSlot()
{
   bool ok = false;
   QFont font= QFontDialog::getFont(&ok, QApplication::font(), this);
   if (!ok) return;

   go4sett->setAppFont(font);

   QApplication::setFont(font);
}

void TGo4MainWindow::ChangeTerminalFontSlot()
{

   bool ok = false;
   QFont font= QFontDialog::getFont(&ok, go4sett->getTermFont(), this);
   if (!ok) return;

   go4sett->setTermFont(font);

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw) anw->setFont(font);
}


void TGo4MainWindow::ChangeTerminalTimeStampSlot()
{
  go4sett->setTermShowTimestamp(faTermTimeStamp->isChecked());
  TGo4AnalysisWindow* anw = FindAnalysisWindow();
  if (anw!=0)
    anw->UpdateTimeStampFormat();
}

void TGo4MainWindow::ChangeTerminalTimeStampFormatSlot()
{

  bool ok = false;
  QString str = QInputDialog::getText(this, "Analysis terminal timestamp format",
      "Time format (empty resets to  yyyy-MM-dd·hh mm:ss.zzz)",
       QLineEdit::Normal, go4sett->getTermTimeFormat(), &ok);
  if (ok)
  {
    if (str.isEmpty())
      str = "yyyy-MM-dd hh:mm:ss.zzz";
    go4sett->setTermTimeFormat(str);
    TGo4AnalysisWindow* anw = FindAnalysisWindow();
    if (anw!=0)
          anw->UpdateTimeStampFormat();
  }
}



void TGo4MainWindow::SetStyleSlot(const QString &style)
{
/*
   QString infostring="Style "+style+" was selected.\n";
   infostring += "One can try to apply it immediately, but Qt crashed very often at that place :(\n";
   infostring += "One can save that style in settings and it will be applied next time Go4 is started";

   int res = QMessageBox::question( this, "Go4 GUI", infostring,
         QMessageBox::Apply | QMessageBox::Save | QMessageBox::Cancel ,
         QMessageBox::Save);

   if (res == QMessageBox::Cancel) return;
*/
   // save settings anyway
   go4sett->setAppStyle(style);
   go4sett->Store();

//   if (res == QMessageBox::Apply)
      QApplication::setStyle(style);
}

void TGo4MainWindow::StatusMessage(const QString& mess)
{
   QString output = QDateTime::currentDateTime().toString(Qt::ISODate);
   output.append(": ");
   output.append(mess);
   statusBar()->showMessage(output, 0); // JAM2016: do not clear message after 3s. ratemeter will clear message on each update.
   if(TGo4Log::IsAutoEnabled())
      TGo4Log::Message(1,output.toLatin1().constData());

   //UpdateCaptionButtons();
   // <- JAM2016: do not refresh ratemeter display immediately,
   // but give message a chance to appear a while!
}

void TGo4MainWindow::UpdateCaptionButtons()
{
   // JAM note this function is called by update timer from TGo4Browser each second
   TGo4ServerProxy* root_serv = Browser()->FindServer(0, kFALSE);
   TGo4ServerProxy* go4_serv = Browser()->FindServer();
   TGo4AnalysisProxy* pr = dynamic_cast<TGo4AnalysisProxy*>(go4_serv);
   TGo4HttpProxy* ht = dynamic_cast<TGo4HttpProxy*>(go4_serv);

   QString capt = "Go4 ";
   capt += __GO4RELEASE__;
   capt += " @";
   capt += gSystem->HostName();
   if ((go4_serv!=0) && go4_serv->IsConnected() ) {
      capt += " <";
      capt += go4_serv->GetContainedObjectInfo();
      capt += ">";
   }
   setWindowTitle(capt);

   bool flag = false;
   if (pr==0) flag = (ht==0 ?  true: false) ;
   faLaunchAnal->setEnabled(flag);
   // JAM here check again ratemeter connection, if http server was disconnected by browser popup close item:
   EstablishRatemeter(flag? 0: 1);
   // need to check if the controller role has switched to another HTTP server here:
   static TGo4HttpProxy* oldhttp=0;
    if (ht) {
      // check for server names may not be unique if connected twice by chance, better use proxy pointers!
      if(ht!= oldhttp)
      {
        //printf("UpdateCaptionButtons sees new http server (0x%x), old (0x%x) \n", ht, oldhttp);
        oldhttp=ht;
        EstablishRatemeter(0);
        EstablishRatemeter(2);
      }
    }

   if (pr==0) flag = (ht==0 ?  true: false) ;
         else flag = (fConnectingCounter<=0) && pr->IsAnalysisServer() && !pr->IsConnected();
   faConnectAnal->setEnabled(flag);

   faPrepareAnal->setEnabled(flag);

   if (pr==0) flag = (ht==0 ?  false: true);
         else flag = pr->IsAnalysisServer() &&
                     (pr->IsConnected() || (fConnectingCounter<=0));
   faDisconnectAnal->setEnabled(flag);

   flag = false;
   if (go4_serv)
      flag = !go4_serv->IsAnalysisServer() || (go4_serv->IsConnected() && go4_serv->IsAdministrator());
   faShutdownAnal->setEnabled(flag);

   bool iscontrolling(false), issubmit(false);
   if (go4_serv) {
      iscontrolling = go4_serv->IsConnected() && (go4_serv->IsAdministrator() || go4_serv->IsController());
      if (iscontrolling) issubmit = go4_serv->CanSubmitObjects();
   }

   faSumbStartAnal->setEnabled(issubmit);

   faStartAnal->setEnabled(iscontrolling);

   faStopAnal->setEnabled(iscontrolling);

   faAnalConfig->setEnabled(iscontrolling);

   faAnalTermin->setEnabled(FindAnalysisWindow()!=0);

   if ((go4_serv==0) && (root_serv!=0)) {
      faStartAnal->setEnabled(root_serv->FindCommand("Start").Length()>0);
      faStopAnal->setEnabled(root_serv->FindCommand("Stop").Length()>0);
   }
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

void TGo4MainWindow::ChangeHideEventElement()
{
   go4sett->setHideTGo4EventElement(faHideEventElement->isChecked());
   UpdateBrowser();
}

void TGo4MainWindow::ChangeDrawOnceSlot()
{
   go4sett->setDrawOnceFlag(faDrawOnce->isChecked());

}

void TGo4MainWindow::ChangeWindowRubberBandSlot()
{
  go4sett->setMoveSubwindowRubberBand(faWindowRubberBand->isChecked());
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

void TGo4MainWindow::SuperimposeSlot()
{
   go4sett->setPadSuperimpose(faSuperimpose->isChecked());
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


void TGo4MainWindow::ChangeSaveWhiteBackgroundSlot()
{
  go4sett->setSavePadWhiteBackground(faSaveWhite->isChecked());
}


void TGo4MainWindow::DrawLineWidthSlot()
{
   int w = go4sett->getDrawLineWidth();

   bool ok = false;

#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   w = QInputDialog::getInt(this,
                            "Default line width for object drawing",
                            "Input 1 to preserve default value",
                             w, 1, 10, 1, &ok);
#else
   w = QInputDialog::getInteger(this,
                               "Default line width for object drawing",
                                "Input 1 to preserve default value",
                                w, 1, 10, 1, &ok);
#endif

   if (ok) go4sett->setDrawLineWidth(w);
}


void TGo4MainWindow::DrawFillColorSlot()
{
   int w = go4sett->getDrawFillColor();

   bool ok = false;

#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   w = QInputDialog::getInt(this,
                            "Default fill color for 1D objects (histogram and graph) drawing",
                            "Input 0 for no fill",
                             w, 0, 50, 1, &ok);
#else
   w = QInputDialog::getInteger(this,
                               "Default fill color for 1D objects (histogram and graph) drawing",
                                "Input 0 for no fill",
                                w, 0, 50, 1, &ok);
#endif

   if (ok) go4sett->setDrawFillColor(w);
}

void TGo4MainWindow::DrawFillStyleSlot()
{
   int w = go4sett->getDrawFillStyle();

   bool ok = false;

#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   w = QInputDialog::getInt(this,
                            "Default fill style for 1D objects (histogram and graph) drawing",
                            "Input 1001 for solid fill",
                             w, 0, 4000, 1, &ok);
#else
   w = QInputDialog::getInteger(this,
                               "Default fill style for 1D objects (histogram and graph) drawing",
                                "Input 1001 for solid fill",
                                w, 0, 4000, 1, &ok);
#endif

   if (ok) go4sett->setDrawFillStyle(w);
}


void TGo4MainWindow::TH1DrawOptSlot()
{
   bool ok = false;
   QString str = QInputDialog::getText(this,
                     "Default draw options for TH1 class",
                     "Input draw options",
                     QLineEdit::Normal, go4sett->getTH1DrawOpt(), &ok);
   if (ok) go4sett->setTH1DrawOpt(str);
}

void TGo4MainWindow::TH2DrawOptSlot()
{
   bool ok = false;
   QString str = QInputDialog::getText(this,
                     "Default draw options for TH2 class",
                     "Input draw options",
                     QLineEdit::Normal, go4sett->getTH2DrawOpt(), &ok);
   if (ok) go4sett->setTH2DrawOpt(str);
}

void TGo4MainWindow::TH3DrawOptSlot()
{
   bool ok = false;
   QString str = QInputDialog::getText(this,
                     "Default draw options for TH3 class",
                     "Input draw options",
                     QLineEdit::Normal, go4sett->getTH3DrawOpt(), &ok);
   if (ok) go4sett->setTH3DrawOpt(str);
}

void TGo4MainWindow::TGraphDrawOptSlot()
{
   bool ok = false;
   QString str = QInputDialog::getText(this,
                     "Default draw options for TGraph class",
                     "Input draw options",
                     QLineEdit::Normal, go4sett->getTGraphDrawOpt(), &ok);
   if (ok)
      go4sett->setTGraphDrawOpt(str);
}


void TGo4MainWindow::GStyleStatFormatSlot()
{
   bool ok = false;
   QString s0 = go4sett->getGStyleStatFormat();
   if (s0.isEmpty()) s0 = gStyle->GetStatFormat();

   QString str = QInputDialog::getText(this,
                     "Printf argument for float values - gStyle->GetStatFormat()",
                     "Input format string like 6.4g (empty - ROOT default)",
                     QLineEdit::Normal, s0, &ok);
   if (ok) {
      go4sett->setGStyleStatFormat(str);
      if (str.isEmpty())
         gStyle->SetStatFormat();
      else
         gStyle->SetStatFormat(str.toLatin1().constData());
   }
}


void TGo4MainWindow::PaletteSettingsSlot()
{
  // JAM: later we might put some elaborate input dialog here. for the moment keep on with parsing a string-
  bool ok = false;
  int min=0, def=0, max =0;
  QString palvals;
  go4sett->getPaletteOpt(min,def,max);
  palvals.sprintf("%d:%d:%d",min,def,max);
     QString str = QInputDialog::getText(this,
                       "Default Palette options",
                       "Input - MinIndex:DefaultIndex:MaxIndex",
                       QLineEdit::Normal, palvals, &ok);
  if (ok) {
       QStringList parts=str.split(":");
       min=parts[0].toInt();
       def=parts[1].toInt();
       max=parts[2].toInt();
//       std::cout <<"PaletteSettingsSlot has "<<min<<":"<<def<<":"<<max  <<std::endl;
       go4sett->setPaletteOpt(min,def,max);
       // activate settings immediately, do not need to restart go4
       fxStyle->SetPaletteRange(min,def,max);
     }
}


void TGo4MainWindow::ScaleFactorSlot()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
  double w = go4sett->getScreenScaleFactor();

 bool ok = false;

 w = QInputDialog::getDouble(this,
                              "HighDPI scaling",
                              "Please set scale factor. Restart GUI to apply!",
                               w, 0, 50, 1, &ok);
 if (ok) go4sett->setScreenScaleFactor(w);
#endif
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
   int isserver = go4sett->getClientIsServer();

   TString launchcmd, killcmd;
   Bool_t res = kFALSE;

   QString workdir = go4sett->getClientDir();

   if (isserver==2) {
      // first handle http connection

      QString addr = QString("%1:%2").arg(go4sett->getClientNode()).arg(go4sett->getClientPort());

      // first verify that http server already running with such address
      // need to request analysis status anyway
      if (ConnectHttpSlot(addr.toLatin1().constData(),0,0,false, true )!=0) {
        StatusMessage("Connected to exisiting analysis webserver!"); // JAM tell user that this is no analysis restart!
        std::cout<< "!!! Connected to exisiting analysis webserver "<<addr.toLatin1().constData()<< "!!! "<<std::endl; // status message is shadowed by ratemeters....
        return;
      }

      res = TGo4ServerProxy::GetLaunchString(launchcmd, killcmd,
                           2, shellmode, termmode,
                           go4sett->getClientName().toLatin1().constData(),
                           go4sett->getClientNode().toLatin1().constData(),
                           workdir.toLatin1().constData(),
                           go4sett->getClientExec().toLatin1().constData(),
                           go4sett->getClientPort(),
                           go4sett->getClientExeMode(),
                           go4sett->getClientArgs().toLatin1().constData());

      if (!res) return;

      fKillCommand = killcmd.Data();

      if ((termmode==2) || (termmode==3)) {
          gSystem->Exec(launchcmd.Data());
      } else {
         TGo4AnalysisWindow* anw = EstablishAnalysisWindow(true, true, true);
         anw->StartAnalysisShell(launchcmd.Data(), (shellmode==0) ? workdir.toLatin1().constData() : 0, true);
      }

      fConnectingCounter = 100; // try next 10 seconds connect with the server
      fConnectingHttp = addr;
      fbGetAnalysisConfig=true; // pass to timer that we want to have analysis config window when ready JAM
      QTimer::singleShot(100, this, SLOT(CheckConnectingCounterSlot()));

      return;
   }

   if (isserver==0) {
      TGo4AnalysisProxy* anal = AddAnalysisProxy(false, (termmode==1));
      if (anal!=0)
        res = anal->LaunchAsClient(launchcmd, killcmd,
                      shellmode,
                      termmode,
                      go4sett->getClientName().toLatin1().constData(),
                      go4sett->getClientNode().toLatin1().constData(),
                      workdir.toLatin1().constData(),
                      go4sett->getClientExec().toLatin1().constData(),
                      go4sett->getClientExeMode(),
                      go4sett->getClientArgs().toLatin1().constData());
      TGo4AnalysisWindow* anw = FindAnalysisWindow();
      if (res && (anw!=0) && (termmode==1)) {
         anw->StartAnalysisShell(launchcmd.Data(), (shellmode==0) ? workdir.toLatin1().constData() : 0, false);
      }
      if (res && (anal!=0)) anal->SetAnalysisLaunched(termmode==1 ? 2 : 1);
   } else {
      res = TGo4AnalysisProxy::LaunchAsServer(launchcmd, killcmd,
                     shellmode,
                     termmode,
                     go4sett->getClientName().toLatin1().constData(),
                     go4sett->getClientNode().toLatin1().constData(),
                     go4sett->getClientPort(),
                     workdir.toLatin1().constData(),
                     go4sett->getClientExec().toLatin1().constData(),
                     go4sett->getClientExeMode(),
                     go4sett->getClientArgs().toLatin1().constData());
   }

   if (res)
     fKillCommand = killcmd.Data();
   else
     fKillCommand = "";

   StatusMessage("Starting Analysis....  Please wait");

   if ((isserver==1) && interactive) {
      fbGetAnalysisConfig=true; // pass to timer that we want to have analysis config window when ready JAM
      ConnectServerSlot(true, "");
   }
}

void TGo4MainWindow::PrepareForClientConnectionSlot(bool interactive)
{
   TGo4AnalysisProxy* ana = AddAnalysisProxy(false, false);
   if ((ana!=0) && interactive)
     QMessageBox::information(this,"Prepare for client connection",
      "Now you can start client from other shell (node) and connect to port " +
       QString::number(ana->ConnectorPort()), "Ok");
}

TGo4AnalysisWindow* TGo4MainWindow::EstablishAnalysisWindow(bool needoutput, bool withkillbnt, bool force_recreate)
{
   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0)
      if (force_recreate || (anw->HasOutput() && !needoutput)) {
        delete anw;
        anw = 0;
     }

   if (anw==0)
      if (needoutput) {
         anw = new TGo4AnalysisWindow(fxMdiArea, "AnalysisWindow", true);
         QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(anw);
         sub->resize(go4sett->lastPanelSize("AnalysisWindow", 700, 500));
         CascadeMdiPosition(sub);
         ConnectGo4Widget(anw);
         anw->ensurePolished();
         sub->show();
     } else {
        QToolBar* dock = addToolBar("Analysis control");
        dock->setObjectName("AnalysisControlBar");
        anw = new TGo4AnalysisWindow(this, "AnalysisWindow", false, withkillbnt || fKillCommand.length()>0);
        ConnectGo4Widget(anw);
        dock->addWidget(anw);
     }

   return anw;
}

void TGo4MainWindow::EstablishAnalysisWindowForHttp()
{
   // used together with http server connection
   // analysis window only created when not exists and http proxy is available

   TGo4ServerProxy* serv = Browser()->FindServer();
   if ((serv==0) || (dynamic_cast<TGo4AnalysisProxy*>(serv)!=0) || (serv->DebugOutputSlot()==0)) return;

   TGo4AnalysisWindow* anw = FindAnalysisWindow();

   if (anw) {
      if (anw->HasLink()) return;
      delete anw;
      anw = 0;
   }

   anw = EstablishAnalysisWindow(true, false);
   if (anw) anw->WorkWithDebugOutput(serv->DebugOutputSlot());
   serv->ResetDebugOutputRequests();
}


TGo4AnalysisProxy* TGo4MainWindow::AddAnalysisProxy(bool isserver, bool needoutput)
{
   const char* analisysitem = "Analysis";

   TGo4Slot* analslot = Browser()->DataSlot(analisysitem);

   if (analslot==0) {
      TGo4AnalysisProxy* anal = new TGo4AnalysisProxy(isserver);
      fxOM->AddProxy(fOMDataPath.toLatin1().constData(), anal, analisysitem, "Analysis proxy");
      analslot = Browser()->DataSlot(analisysitem);
      anal->SetDefaultReceiver(fxOM, TString("gui/") + analisysitem + "/");
   }

   TGo4AnalysisProxy* anal = (analslot==0) ? 0 :
      dynamic_cast<TGo4AnalysisProxy*> (analslot->GetProxy());

   if (anal==0) return 0;

   anal->SetAnalysisReady(kFALSE);

   TGo4LogInfo* loginfo = (TGo4LogInfo*) FindGo4Widget("LogInfo", false);
   if (loginfo!=0)
     loginfo->WorkWithInfo(anal->LoginfoSlot());

   if (anal->IsAnalysisServer())
      EstablishRatemeter(anal->IsConnected() ? 2 : 0);
   else
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
        anw = EstablishAnalysisWindow(true);
        if (anw) anw->WorkWithUpdateObjectCmd(anal->UpdateObjectSlot());
     } else {
        UpdateDockAnalysisWindow();
     }

   return anal;
}

bool TGo4MainWindow::RemoveAnalysisProxy(int waittime, bool servershutdown)
{
   // Browser()->ToggleMonitoring(0);
   EstablishAnalysisConfiguration(0);
   EstablishRatemeter(0);

   // TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
   TGo4ServerProxy* srv = Browser()->FindServer();
   if (srv!=0) {
      srv->DisconnectAnalysis(waittime, servershutdown);
   }
   // TGo4Slot* slot = Browser()->FindServerSlot(kTRUE, 1);
   // it is allowed to delete slot directly
   //if (slot) delete slot;

   return Browser()->FindServer()==0;
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
     } else {
       shouldexists = true;
     }

   if (shouldexists && (anw==0)) {
      anw = EstablishAnalysisWindow(false, !anal->IsAnalysisServer());
      if (anw) anw->WorkWithUpdateObjectCmd(anal->UpdateObjectSlot());
   }
}

TGo4AnalysisStatusMonitor* TGo4MainWindow::EstablishRatemeter(int level)
// level = 0 - delete
// level = 1 - as is
// level = 2 - create
{
  //std::cout<<"EstablishRatemeter with level "<<level << std::endl;



   TGo4AnalysisStatusMonitor* status =
     dynamic_cast<TGo4AnalysisStatusMonitor*>
       (FindGo4Widget("AnalysisStatusMonitor", false));

   if (level==2) {
     if (status==0) {
        status = new TGo4AnalysisStatusMonitor(statusBar(), "AnalysisStatusMonitor");
        ConnectGo4Widget(status);
        statusBar()->addWidget(status);
        TGo4ServerProxy* serv = Browser()->FindServer();
        if (serv!=0)
          status->WorkWithRatemeter(serv->RatemeterSlot());
     }
   } else
   if (level==0) {
      if (status!=0) {
         status->RemoveLink("Ratemeter",true); // JAM: need to remove the update link before deleting!
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
   // level: 0 - delete, 1 - keep as is, 2 - create, 3 - create + request status

   TGo4AnalysisConfiguration* conf = FindAnalysisConfiguration();

   if (level==0) {
      if (conf!=0) {
         delete conf->parentWidget();
         conf = 0;
      }
   } else
   if (level>=2) {
      if (conf==0) {
         conf = new TGo4AnalysisConfiguration(fxMdiArea, "AnalysisConfiguration");
         QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(conf);
         CascadeMdiPosition(sub);
         ConnectGo4Widget(conf);
         // ! do not show configuration window before analysis settings requested
         // conf->ensurePolished();
         // sub->show();
      }
      TGo4ServerProxy* anal = Browser()->FindServer();
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
        dlg.setPassword(fLastPassword.toLatin1().constData());
      if (dlg.exec()!=QDialog::Accepted) return;
      pass = dlg.getInput();
   }
   // here check if we want web server or regular connection:

   if(go4sett->getClientConnectMode()==0)
   {
     if (anal==0) anal = AddAnalysisProxy(true, false);
     bool def = go4sett->getClientDefaultPass();
     if (!def) fLastPassword = pass;

     if (anal!=0)
       anal->ConnectToServer(go4sett->getClientNode().toLatin1().constData(),
                             go4sett->getClientPort(),
                             go4sett->getClientControllerMode(),
                             def ? 0 : pass.toLatin1().constData());
     StatusMessage("Connecting running analysis....  Please wait");

     // wait about 4 sec that analysis is connected
     fConnectingCounter = 41;
     UpdateCaptionButtons();
     CheckConnectingCounterSlot();
   }
   else
   {
     QString portstring;
     QString fulladdress = go4sett->getClientNode().append(QString(":")).append(portstring.setNum(go4sett->getClientPort()));
     //std::cout << " try to connect to server: "<<fulladdress.toLatin1().constData() << std::endl;
     QString msg("Connecting analysis http server at ");
     msg.append(fulladdress).append(QString(", Please wait"));
     StatusMessage(msg);
     ConnectHttpSlot(fulladdress.toLatin1().constData(),
             go4sett->getClientAccountName().toLatin1().constData(),
             pass.toLatin1().constData(), false, false);
     UpdateCaptionButtons();
   }
}

void TGo4MainWindow::CheckConnectingCounterSlot()
{
   if (fConnectingHttp.length() > 0) {
      TGo4ServerProxy *serv =
          ConnectHttpSlot(fConnectingHttp.toLatin1().constData(), 0, 0, go4sett->getClientTermMode()==1,fbGetAnalysisConfig);
      if (serv != 0) {
         serv->SetAnalysisLaunched(go4sett->getClientTermMode()==1 ? 2 : 1);
         fConnectingHttp = "";
         fConnectingCounter = 0;
         fbGetAnalysisConfig=false;
         return;
      }
      if (--fConnectingCounter<=0) {
         StatusMessage(fConnectingHttp + " refused connection. Try again");
         fConnectingHttp = "";
         fConnectingCounter = 0;
         fbGetAnalysisConfig=false;
         return;
      }
   } else {
      TGo4AnalysisProxy* anal = Browser()->FindAnalysis();
      if ((anal==0) || anal->IsConnected() || (--fConnectingCounter<=0)) {
         if (fConnectingCounter<=0)
            StatusMessage("Analysis refused connection. Try again");
         fConnectingCounter = 0;
         EstablishRatemeter((anal!=0) && anal->IsConnected() ? 2 : 0);
         int level=0;
         if((anal!=0) && anal->IsConnected() && (anal->IsController() || anal->IsAdministrator()))
           {
             if(fbGetAnalysisConfig)
               {
                 level=3;
                 fbGetAnalysisConfig=false;
               }
             else
             {
               level=2;
             }
           }
         EstablishAnalysisConfiguration(level);
         UpdateDockAnalysisWindow();
         UpdateCaptionButtons();
         if ((anal!=0) && !anal->IsConnected()) RemoveAnalysisProxy(1);
         return;
     }
   }
   QTimer::singleShot(100, this, SLOT(CheckConnectingCounterSlot()));
}

void TGo4MainWindow::DisconnectAnalysisSlot(bool interactive)
{
   TGo4ServerProxy* serv = Browser()->FindServer();
   if (serv==0) return;

   bool shutdown = serv->IsAnalysisLaunched() > 1;

   if (interactive) {

      if (shutdown) {

         QMessageBox msgBox(
                     QMessageBox::Warning,
                     "Disconnect from analysis",
                     "Analysis runs inside go4 widget.\n"
                     "If one only disconnects from the analysis,\n"
                     "it remains invisible and difficult to stop.\n"
                     "To shutdown it later, one need to reconnect with go4 gui again.\""
                     "It is recommended to shutdown analysis now",
                     QMessageBox::Ok | QMessageBox::Close | QMessageBox::Abort);

         msgBox.setButtonText(QMessageBox::Ok, "Shutdown");
         msgBox.setButtonText(QMessageBox::Close, "Disconnect");
         msgBox.setButtonText(QMessageBox::Abort, "Cancel");

         switch (msgBox.exec()) {
            case QMessageBox::Ok: shutdown = true; break;
            case QMessageBox::Close: shutdown = false; break;
            default: return;
         }
      } else {
         if (QMessageBox::warning(this, "Disconnect analysis",
                    QString("Really disconnect from analysis task?"),
                    QString("Disconnect"),
                    QString("Cancel"),
                    QString::null, 0)!=0) return;
      }
   }
   RemoveAnalysisProxy(30, shutdown);
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
   TGo4ServerProxy* srv = Browser()->FindServer();
   if (srv==0) return;
   bool realshutdown = false;
   TGo4AnalysisProxy* anal = dynamic_cast<TGo4AnalysisProxy*> (srv);
   TGo4HttpProxy* http = dynamic_cast<TGo4HttpProxy*>(srv);
   if (anal)
       realshutdown = anal->IsAnalysisServer() &&
                      anal->IsConnected() &&
                       anal->IsAdministrator();
   else if (http)
      realshutdown = http->IsConnected() && http->IsAdministrator();

   RemoveAnalysisProxy(30, realshutdown);
   StatusMessage("Shutdown analysis");
}

bool TGo4MainWindow::SubmitAnalysisSettings()
{
   TGo4ServerProxy* serv = Browser()->FindServer();
   if (serv==0) return false;

   if (serv->IsConnected() && serv->CanSubmitObjects() &&
       (serv->IsAdministrator() || serv->IsController())) {
      serv->SubmitAnalysisSettings();
      serv->RefreshNamesList();
      StatusMessage("Press Ctrl+S or choose Analysis->Start from the Menu to start the analysis");
      return true;
   }

   return false;
}

void TGo4MainWindow::SubmitStartAnalysisSlot()
{
   if (SubmitAnalysisSettings())
      StartAnalysisSlot();
}

void TGo4MainWindow::StartAnalysisSlot()
{
   TGo4ServerProxy* go4_serv = Browser()->FindServer();
   if (go4_serv!=0) {
      go4_serv->StartAnalysis();
      go4_serv->RefreshNamesList();
      go4_serv->DelayedRefreshNamesList(4);
      EstablishRatemeter(2);
   } else {
      TGo4ServerProxy* root_serv = Browser()->FindServer(0, kFALSE);
      if (root_serv) {
         TString cmd = root_serv->FindCommand("Start");
         if (cmd.Length()>0) {
            root_serv->SubmitCommand(cmd);
            StatusMessage(TString::Format("Submit %s command to the server", cmd.Data()).Data());
         }
      }
   }
}

void TGo4MainWindow::StopAnalysisSlot()
{
   TGo4ServerProxy* go4_serv = Browser()->FindServer();
   if (go4_serv) {
      go4_serv->StopAnalysis();
   } else {
      TGo4ServerProxy* root_serv = Browser()->FindServer(0, kFALSE);
      if (root_serv) {
         TString cmd = root_serv->FindCommand("Stop");
         if (cmd.Length()>0) {
            root_serv->SubmitCommand(cmd);
            StatusMessage(TString::Format("Submit %s command to the server", cmd.Data()).Data());
         }
      }
   }
}

void TGo4MainWindow::TerminateAnalysis(bool interactive)
{
   if (interactive) {
      int res = QMessageBox::warning(this, "Kill analysis process",
                QString("Kill analysis by shell command: ") +fKillCommand + " ?",
                QString("Kill"),
                QString("Cancel"),
                QString::null, 0);
      if (res!=0) return;
   }

   TGo4AnalysisWindow* anw = FindAnalysisWindow();
   if (anw!=0) anw->TerminateAnalysisProcess();

   // proxy will be deleted after 7 second
   //RemoveAnalysisProxy(7);

   anw = FindAnalysisWindow();
   if (anw!=0)
      anw->AppendOutputBuffer(QString("\nKilling analysis client: \n  ")+fKillCommand, 2);
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

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());

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
      fitpanel = new TGo4FitPanel(fxMdiArea,"FitPanel");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(fitpanel);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(fitpanel);
      fitpanel->ensurePolished();
      sub->show();
   } else {
      fitpanel->parentWidget()->showNormal();
      fitpanel->setFocus();
   }
   return fitpanel;
}

TGo4HistogramInfo* TGo4MainWindow::StartHistogramInfo()
{
   TGo4HistogramInfo* hinfo = (TGo4HistogramInfo*) FindGo4Widget("HistogramInfo", true);
   if (hinfo==0) {
      hinfo = new TGo4HistogramInfo(fxMdiArea, "HistogramInfo");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(hinfo);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(hinfo);
      hinfo->ensurePolished();
      hinfo->adjustSize();
      sub->show();
   }
   return hinfo;
}

TGo4ConditionInfo* TGo4MainWindow::StartConditionInfo()
{
   TGo4ConditionInfo* cinfo = (TGo4ConditionInfo*) FindGo4Widget("ConditionInfo", true);
   if (cinfo==0) {
      cinfo = new TGo4ConditionInfo(fxMdiArea, "ConditionInfo");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(cinfo);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(cinfo);
      cinfo->ensurePolished();
      cinfo->adjustSize();
      sub->show();
   }
   return cinfo;
}

void TGo4MainWindow::LoadLibrarySlot()
{
   TGo4LoadedLibraries llib(this);
   llib.exec();

   UpdateBrowser();
}

void TGo4MainWindow::UpdateBrowser()
{
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
      pedit = new TGo4ParaEdit(fxMdiArea, "ParaEdit");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(pedit);
      ConnectGo4Widget(pedit);
      pedit->ensurePolished();
      sub->show();
      CascadeMdiPosition(sub);
   }

   if (itemname!=0)
      pedit->WorkWithParameter(itemname, false);

   return pedit;
}

TGo4EditDynEntry* TGo4MainWindow::StartEditDynEntry()
{
   TGo4EditDynEntry* dedit = (TGo4EditDynEntry*) FindGo4Widget("EditDynEntry", true);

   if (dedit==0) {
      dedit = new TGo4EditDynEntry(fxMdiArea, "EditDynEntry");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(dedit);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(dedit);
      dedit->ensurePolished();
      sub->show();
   }

   return dedit;
}

TGo4ConditionEditor* TGo4MainWindow::StartConditionEditor()
{
   TGo4ConditionEditor* wedit = (TGo4ConditionEditor*) FindGo4Widget("ConditionEditor", true);
   if (wedit==0) {
      wedit = new TGo4ConditionEditor(fxMdiArea, "ConditionEditor");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(wedit);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(wedit);
      wedit->ensurePolished();
      sub->show();
   }
   return wedit;
}

TGo4EventInfo* TGo4MainWindow::StartEventInfo()
{
   TGo4EventInfo* einfo = (TGo4EventInfo*) FindGo4Widget("EventInfo", true);

   if (einfo==0) {
      einfo = new TGo4EventInfo(fxMdiArea, "EventInfo");
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(einfo);
      CascadeMdiPosition(sub);
      ConnectGo4Widget(einfo);
      einfo->ensurePolished();
      sub->show();
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
  QWidget* mdi = conf ? conf->parentWidget() : 0;

  if (mdi == 0)
    return;

  if (mdi->isVisible())
  {
    mdi->hide();
  }
  else
  {

    if (conf->GetNumSteps() == 0)
    {
      // this can happen when gui connected to server without requesting the setup. Do it when user wants to see config:
      TGo4ServerProxy* anal = Browser()->FindServer();
      if (anal != 0)
        anal->RequestAnalysisSettings();
    }
    mdi->raise();
    mdi->show();
    if (mdi->isMinimized())
      mdi->showNormal();
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
      QWidget* mdi = anw->parentWidget();

      if (mdi->isVisible()) {
         mdi->hide();
      } else {
         mdi->raise();
         mdi->show();
         if (mdi->isMinimized()) mdi->showNormal();
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

   if (anw)
     if (anw->HasOutput()) {
        anw->parentWidget()->close();
      } else {
         QToolBar* bar = dynamic_cast<QToolBar*> (anw->parentWidget());
         removeToolBar(bar);
         delete bar;
      }

   // try to reestablish window for other server
   QTimer::singleShot(250, this, SLOT(EstablishAnalysisWindowForHttp()));
}


void TGo4MainWindow::ToggleMbsMonitor(const char* nodename)
{
   TGo4MBSViewer* mbs =
     dynamic_cast<TGo4MBSViewer*> (FindGo4Widget("MBSViewer", false));
   if (mbs==0) return;
   if (mbs->parentWidget()->isVisible()) {
       mbs->parentWidget()->hide();
   } else {
     mbs->SetNode(nodename);
     mbs->parentWidget()->show();
   }
}


TGo4SetScaleValues* TGo4MainWindow::ToggleScaleValues()
{
   TGo4SetScaleValues* scl = 0;

   QList<QMdiSubWindow *> windows = fxMdiArea->subWindowList();
   for (int i=0; i < windows.count(); ++i ) {
      scl = dynamic_cast<TGo4SetScaleValues*> (windows.at(i)->widget());
      if (scl!=0) break;
   }

   if (scl==0) {
      scl = new TGo4SetScaleValues(fxMdiArea, "ScaleValues", Qt::WindowStaysOnTopHint);
      QMdiSubWindow* sub = fxMdiArea->AddGo4SubWindow(scl);
      CascadeMdiPosition(sub);
      scl->ensurePolished();
      sub->show();
   } else {
      scl->parentWidget()->close();
      scl = 0;
   }
   return scl;
}

void TGo4MainWindow::CreateNewHistSlot(int isremote)
{
   TGo4CreateNewHistogram dlg(this);
   TGo4ServerProxy* an = Browser()->FindServer();

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
             else an->RefreshNamesList();
   }
   if (h1!=0) delete h1;
}

void TGo4MainWindow::CreateNewConditionSlot(bool forothereditor)
{
   TGo4ServerProxy* an = Browser()->FindServer();
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
         else an->RefreshNamesList();
}

void TGo4MainWindow::CreateNewDynEntrySlot(bool forothereditor)
{
   TGo4ServerProxy* an = Browser()->FindServer();
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
          else an->RefreshNamesList();
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
    if ((br==0) || (itemname==0) || strlen(itemname)==0 ) return 0;

   TGo4Slot* guislot = br->BrowserSlot(itemname);

   if (guislot==0) return 0;
   if (guislot->GetProxy()==0) {
      std::cerr << " Problem with gui slots" << std::endl;
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
      QString root_fmt = "ROOT (*.root)";
      QString xml_fmt = "ROOT XML (*.xml)";

      QFileDialog fd(this, QString("Save ") + itemname + " in root file", fLastFileDir);
      fd.setFileMode(QFileDialog::AnyFile);
      fd.setAcceptMode(QFileDialog::AcceptSave);
      fd.setNameFilters(QStringList() << root_fmt << xml_fmt);

      if (fd.exec() == QDialog::Accepted) {
         QStringList flst = fd.selectedFiles();
         if (!flst.isEmpty()) {
            QString filename = flst[0];
            if (fd.selectedNameFilter()==xml_fmt) {
               if (!filename.endsWith(".xml")) filename.append(".xml");
            } else {
               if (!filename.endsWith(".root")) filename.append(".root");
            }
            res = br->SaveItemToFile(itemname, filename.toLatin1().constData(), subfolder);
            fLastFileDir = fd.directory().path();
         }
      }
   }

   return res;
}

void TGo4MainWindow::SavePanelCanvas(TGo4ViewPanel* panel)
{
   if (panel==0) return;

   TCanvas* can= panel->GetCanvas();
   TCanvas* cansav;


   QFileDialog fd( this, QString("Save ") + panel->objectName() + " As", fLastFileDir);
   fd.setFileMode( QFileDialog::AnyFile );
   fd.setAcceptMode(QFileDialog::AcceptSave);

   QString PS = "Post Script (*.ps)";
   QString PS_Portrait = "Post Script Portrait (*.ps)";
   QString PS_Landscape = "Post Script Landscape (*.ps)";
   QString EPS = "Encapsulated Post Script (*.eps)";
   QString EPS_Preview = "Encapsulated Post Script preview (*.eps)";
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

   fd.setNameFilters(flt);

   if (fd.exec() != QDialog::Accepted) return;


   bool blankbg=go4sett->getSavePadWhiteBackground();
   if(blankbg)
   {
     can = (TCanvas*) panel->GetCanvas()->Clone();
     can->SetName("PrintoutPad");
     //std::cout << "SavePanelCanvas has cloned panel canvas! "<< can->GetName()<< std::endl;
   }
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
   }

   if(blankbg)
   {
     panel->ResetPadFillColors(can,0);
     // std::cout <<"Reset pad fill colors for blank bg option with canvas "<<can->GetName() <<std::endl;
     gROOT->SetBatch(kTRUE); // force ROOT to make memory print of canvas instead of using actual window display for image formats.
   }
   can->Print(filename.toLatin1().constData(), opt);

    // std::cout<< "printed canvas "<<can->GetName() << std::endl;
   if(blankbg)
   {
        gROOT->SetBatch(kFALSE);
       delete can;
   }
}

TGo4ObjectManager* TGo4MainWindow::OM()
{
   return fxOM;
}

TGo4BrowserProxy* TGo4MainWindow::Browser()
{
   return (TGo4BrowserProxy*) fxOM->GetProxy(fOMBrowserPath.toLatin1().constData());
}

TGo4Slot* TGo4MainWindow::GetWidgetTopSlot(QGo4Widget* widget, bool force)
{
   if (widget==0) return 0;

   QString EditorName = widget->objectName();

   QString editorslotname = fOMEditorsPath;
   editorslotname += "/";
   editorslotname += EditorName;
   TGo4Slot* edslot = fxOM->GetSlot(editorslotname.toLatin1().constData());
   if ((edslot==0) && force) {
      fxOM->AddProxy(fOMEditorsPath.toLatin1().constData(), new TGo4WidgetProxy(widget), EditorName.toLatin1().constData(), "Links for Go4 widget");
      edslot = fxOM->GetSlot(editorslotname.toLatin1().constData());
   }
   return edslot;
}

void TGo4MainWindow::checkPanelRepaintSlot()
{
   // block monitoring when start panel updates
   Browser()->SetMonitorBlockingFlag(kTRUE);

   fbPanelTimerActive = false;

   // first check if active viewpnael need update its content
   TGo4ViewPanel* actpanel = fxMdiArea->GetActivePanel();
   if ((actpanel!=0) && actpanel->IsRepaintTimerActive())
      actpanel->checkRepaintSlot();

   if (fbPanelTimerActive) return;

   // now check if any other panel shold be repainted
   TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());
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
            int kind = Browser()->ItemKind(eventstr.toLatin1().constData());
            TClass* cl = Browser()->ItemClass(eventstr.toLatin1().constData());
            accept = editor->IsAcceptDrag(eventstr.toLatin1().constData(), cl, kind);
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
             int kind = Browser()->ItemKind(eventstr.toLatin1().constData());
             TClass* cl = Browser()->ItemClass(eventstr.toLatin1().constData());

             TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (editor);
             TGo4FitPanel* fitpanel = dynamic_cast<TGo4FitPanel*> (editor);
             if (panel!=0)
                panel->DropOnPad((TPad*)str, eventstr.toLatin1().constData(), cl, kind);
             else
             if (fitpanel!=0)
                fitpanel->DropOnPanel(event, eventstr.toLatin1().constData(), cl, kind);
             else
                editor->DropItem(eventstr.toLatin1().constData(), cl, kind);
         }
         break;
      }

      case QGo4Widget::service_CreateItem: {
         TClass* cl = (TClass*) par;
         int id = str!=0 ? QString(str).toInt() : 0;
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
         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());
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
         TGo4Slot* topslot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());

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
         } else
         if (cl->InheritsFrom(TGo4AnalysisStatus::Class())) {
            // use central editor, later can control many analysis at once
            EstablishAnalysisConfiguration(3);
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
         *res = MakeNewPanel(QString(str).toInt());
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
         TGo4ServerProxy** res = (TGo4ServerProxy**) par;
         *res = Browser()->FindServer(str);
         break;
      }

      case QGo4Widget::service_AddEditorLink: {
         TGo4Slot* brslot = Browser()->BrowserSlot((const char*)par);

         QByteArray ba = editor->objectName().toLatin1();

         if (str==0) str = ba.constData();

         TGo4Slot* link = fxOM->AddLink(brslot, edslot->GetFullName(),
                                        str, "link to browser item");
         if (link!=0)
            editor->SetLinkedName(link, (const char*)par);

         break;
      }

      case QGo4Widget::service_AddDirectLink: {
         //std::cout << " QGo4Widget::service_AddDirectLink " << std::endl;
         QByteArray ba = editor->objectName().toLatin1();
         if (str==0) str = ba.constData();
         TGo4Slot* slot = (TGo4Slot*) par;
         if (slot!=0) {
            //std::cout << "add link for " << str << " in edslot = " << edslot->GetFullName() << std::endl;
            fxOM->AddLink(slot, edslot->GetFullName(),
                          str, "direct link to OM slot");
            //edslot->Print("*");
         }
         break;
      }

      case QGo4Widget::service_AddLinkInSlot: {
         //std::cout << " QGo4Widget::service_AddLinkInSlot " << std::endl;

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
         TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());
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
         *res = fxMdiArea->GetActivePanel();
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

        if (strcmp(str,"CloseAnalysisSettings")==0) {
           TGo4ServerProxy* anal = Browser()->FindServer();
           if (anal!=0) {
              anal->CloseAnalysisSettings();
              anal->RefreshNamesList();
           }
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
           TerminateAnalysis(true);
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

      case QGo4Widget::service_HotStart: {
         HotStart(str);
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
         fxMdiArea->ResponseOnPanelEvent(serviceid, panel, pad);
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

   QTimer::singleShot(TGo4AbstractInterface::DelayMillisec(), this, SLOT(ProcessHotStart()));
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
      QTimer::singleShot(TGo4AbstractInterface::DelayMillisec(), this, SLOT(ProcessHotStart()));
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
   QString ext = TGo4AbstractInterface::FileExtension();
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

   TGo4Script::ProduceScript(fileName.toLatin1().constData(), this);
}



void TGo4MainWindow::AddAnalysisMacrosBar()
{
  QToolBar* AnalysisMacroBar = addToolBar("Analysis Command Buttons");
  AnalysisMacroBar->setObjectName("AnalysisCommandToolBar");

  // new: everything is in dedicated class
  TGo4UserCommands* companel = new TGo4UserCommands(this, "UserCommands");
  ConnectGo4Widget(companel);
  AnalysisMacroBar->addWidget(companel);
  AnalysisMacroBar->adjustSize();
}



void TGo4MainWindow::ProcessQtEvents()
{
   if (fApp!=0) fApp->processEvents(QEventLoop::AllEvents, TGo4AbstractInterface::DelayMillisec());
}

TGo4ViewPanel* TGo4MainWindow::FindViewPanel(const char* name)
{
   if ((name==0) || (*name==0)) return 0;

   TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (FindGo4Widget(name, false));

   if (panel!=0) return panel;

   TGo4Slot* slot = fxOM->GetSlot(fOMEditorsPath.toLatin1().constData());

   for (Int_t n=0;n<slot->NumChilds();n++) {
      TGo4Slot* widgslot = slot->GetChild(n);
      TGo4WidgetProxy* wproxy = widgslot==0 ? 0 : dynamic_cast<TGo4WidgetProxy*> (widgslot->GetProxy());
      panel = wproxy==0 ? 0 : dynamic_cast<TGo4ViewPanel*> (wproxy->GetWidget());

      if (panel!=0)
        if (strcmp(panel->GetPanelName(), name)==0) return panel;
   }

   return 0;
}
