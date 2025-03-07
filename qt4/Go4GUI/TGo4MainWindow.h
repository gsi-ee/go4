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

#ifndef TGO4MAINWINDOW_H
#define TGO4MAINWINDOW_H

#include <QMainWindow>

class QGo4Widget;
class TGo4FitPanel;
class TGo4ParaEdit;
class TGo4HistogramInfo;
class TGo4ConditionInfo;
class TGo4EditDynEntry;
class TGo4EventInfo;
class TGo4ConditionEditor;
class TGo4AnalysisConfiguration;
class TGo4AnalysisWindow;
class TGo4AnalysisStatusMonitor;
class TGo4SetScaleValues;

class TGo4MdiArea;
class TGo4ViewPanel;
class TPad;
class TGo4Slot;
class TGo4ObjectManager;
class TGo4Style;
class TGo4AnalysisProxy;
class TGo4BrowserProxy;
class TGo4WidgetProxy;
class TGo4ServerProxy;

class QAction;
class QMenu;
class QSpinBox;
class QPushButton;
class QCloseEvent;
class QApplication;

 /** This is the Go4 Main Window
   * @author Mohammad Al-Turany, modifications J.Adamczewski, S.Linev
   * @version 1.0
   * @since 15.05.2002 */

class TGo4MainWindow : public QMainWindow {
   Q_OBJECT

   public:
      TGo4MainWindow(QApplication*);
      virtual ~TGo4MainWindow();

      void HotStart(const char *fname);
      void ProcessQtEvents();
      void StatusMessage(const QString& mess);
      const char *LastTypedPassword() const;
      TGo4ViewPanel *FindViewPanel(const char *name);
      TGo4ViewPanel *DisplayBrowserItem(const char *itemname, TGo4ViewPanel *panel, TPad *pad, bool activate, int updatelevel, const char *drawopt);

      TGo4ServerProxy *ConnectHttpServer(const char *addr = nullptr, const char *user = nullptr, const char *pass = nullptr, bool with_qt_process = false, bool get_analysis_config = false);
      void ConnectServer(bool interactive = true, const char *password = "");
      void PrepareForClientConnection(bool interactive = true);
      void LaunchClient(bool interactive = true);
      void DisconnectAnalysis(bool interactive = true);
      void ShutdownAnalysis(bool interactive = true);

      TGo4AnalysisWindow* EstablishAnalysisWindow(bool needoutput, bool withkillbnt = false, bool force_recreate = false);
      TGo4AnalysisWindow* FindAnalysisWindow();
      void EstablishAnalysisWindowForHttp();
      TGo4AnalysisConfiguration* FindAnalysisConfiguration();

      // starting editors
      TGo4ViewPanel *MakeNewPanel(int div = 0);
      TGo4FitPanel* StartFitPanel();
      TGo4ParaEdit* StartParaEdit(const char *itemname = nullptr);
      TGo4HistogramInfo* StartHistogramInfo();
      TGo4ConditionInfo* StartConditionInfo();
      TGo4EditDynEntry* StartEditDynEntry();
      TGo4EventInfo* StartEventInfo();
      TGo4ConditionEditor* StartConditionEditor();

      void CreateNewHist(int isremote = -1);
      void CreateNewCondition(bool forothereditor = false);
      void CreateNewDynEntry(bool forothereditor = false);

      TGo4ObjectManager *OM();
      TGo4BrowserProxy *Browser();

   public slots:

      void about();
      void aboutQt();
      void aboutROOT();
      void aboutDABC();
      void windowsMenuAboutToShow();
      void UserPanelSlot();
      void LoadLibrarySlot();
      void CloseAllFilesSlot();
      void OpenFileSlot();
      void OpenRemoteFileSlot();
      void ConnectDabcSlot();
      void ConnectHttpSlot();
      void ConnectHServerSlot();
      void ImportObjectSlot();
      void SaveFileSlot();
      void CascadeSubWindows();

      void InputTerminalParametersSlot();
      void LogSettingsSlot();

      void IntroHelpSlot();
      void RefHelpSlot();
      void FitHelpSlot();
      void MinAllWindows();
      void ToggleFullScreenSlot();
      void SaveSettingsSlot();
      void ChangeFontSlot();
      void ChangeTerminalFontSlot();
      void SetStyleSlot(const QString&);
      void ForseCloseSlot();

      void ChangeFetchWhenDrawSlot(bool);
      void ChangeFetchWhenCopySlot(bool);
      void ChangeFetchWhenSaveSlot(bool);
      void ChangeHideEventElement(bool);
      void ChangeDrawOnceSlot(bool);
      void ChangeWindowRubberBandSlot(bool);
      void ChangeNativeMenuBarSlot(bool);

      void CanvasColorSlot();
      void SuperimposeSlot(bool);
      void CrosshairSlot(bool);
      void EventStatusSlot(bool);
      void MarkerSettingsSlot();
      void OptStatsSlot();
      void ChangeCloneFlagSlot(bool);
      void ChangeDrawTimeFlagSlot(bool);
      void ChangeDrawDateFlagSlot(bool);
      void ChangeDrawItemFlagSlot(bool);
      void ChangeSaveWhiteBackgroundSlot(bool);

      void ChangeTerminalTimeStampSlot(bool);
      void ChangeTerminalTimeStampFormatSlot();

      void DrawLineWidthSlot();
      void DrawFillColorSlot();
      void DrawFillStyleSlot();

      void TH1DrawOptSlot();
      void TH2DrawOptSlot();
      void TH3DrawOptSlot();
      void TGraphDrawOptSlot();
      void GStyleStatFormatSlot();
      void PaletteSettingsSlot();

      void ToggleAnalysisConfiguration();
      void ToggleAnalysisWindow();
      void CloseAnalysisWindow();

      bool SubmitAnalysisSettings();
      void SubmitStartAnalysisSlot();
      void StartAnalysisSlot();
      void StopAnalysisSlot();

      // modify default high-dpi scaling
      void ScaleFactorSlot();

      // slots connected to editors
      void editorServiceSlot(QGo4Widget* editor, int serviceid, const char *str, void *par);

      void ProcessHotStart();
      void StopGUIScriptSlot();
      void CreateGUIScriptSlot();
      void CheckConnectingCounterSlot();
      void checkPanelRepaintSlot();

   protected:

      void ShowAboudDialog(const QString &title, const QString &text, const QString &icon);

      void AddAnalysisBar();
      void AddAnalysisMenu();
      void AddFileMenu();
      void AddFileToolBar();
      void AddSettingMenu();
      void AddToolsBar();
      void AddToolsMenu();

      void AddAnalysisMacrosBar();

      void closeEvent(QCloseEvent *ce) override;

      void ConnectGo4Widget(QGo4Widget *editor);

      void CascadeMdiPosition(QWidget *sub);

      void WindowActivated(int id);

      void UpdateCaptionButtons();

      bool startUserGUI(const char *);
      TGo4AnalysisProxy *AddAnalysisProxy(bool isserver, bool needoutput);
      bool RemoveAnalysisProxy(int waittime = 30, bool servershutdown = false);
      TGo4AnalysisStatusMonitor *EstablishRatemeter(int level);
      TGo4AnalysisConfiguration *EstablishAnalysisConfiguration(int level);

      QGo4Widget *FindGo4Widget(const char *name, bool activate);
      TGo4Slot *GetWidgetTopSlot(QGo4Widget *widget, bool force);

      bool SaveBrowserItemToFile(const char *itemname, const char *subfolder);

      void UpdateDockAnalysisWindow();
      void TerminateAnalysis(bool interactive = true);

      void ToggleMbsMonitor(const char *nodename);
      TGo4SetScaleValues *ToggleScaleValues();

      void SavePanelCanvas(TGo4ViewPanel *panel);

      void HelpWindow(const char *filename, const char *msg = nullptr);

      void UpdateBrowser();

      QApplication*      fApp{nullptr};

      QMenu*             windowsMenu{nullptr};
      TGo4MdiArea*       fxMdiArea{nullptr};
      TGo4Style*         fxStyle{nullptr};
      TGo4ObjectManager *fxOM{nullptr};
      QString            fOMDataPath;
      QString            fOMBrowserPath;
      QString            fOMEditorsPath;

      QString            fKillCommand;

      int                fNewWidgetX{0};
      int                fNewWidgetY{0};

      int                fCloseCounter{0};
      int                fConnectingCounter{0}; // counter to establish connection
      bool               fbGetAnalysisConfig{false}; // flag to decide if we want to have analysis config after connect
      QString            fConnectingHttp;   // http address (set when doing connection to http)
      QString            fLastPassword;
      QString            fLastFileDir;

      bool               fbPanelTimerActive{false};
      bool               fbFullScreen{false};

      QAction*           faWebCanvas{nullptr};
      QAction*           faSuperimpose{nullptr};
      QAction*           faCrosshair{nullptr};
      QAction*           faEventstatus{nullptr};
      QAction*           faDrawTime{nullptr};
      QAction*           faDrawDate{nullptr};
      QAction*           faDrawItem{nullptr};

      QAction*           faLaunchAnal{nullptr};
      QAction*           faConnectAnal{nullptr};
      QAction*           faPrepareAnal{nullptr};
      QAction*           faDisconnectAnal{nullptr};
      QAction*           faShutdownAnal{nullptr};
      QAction*           faSumbStartAnal{nullptr};
      QAction*           faStartAnal{nullptr};
      QAction*           faStopAnal{nullptr};
      QAction*           faAnalConfig{nullptr};
      QAction*           faAnalTermin{nullptr};
};

#endif
