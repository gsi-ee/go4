// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4MAINWINDOW_H
#define TGO4MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QtGui/QCloseEvent>

class QApplication;

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

class TGo4WorkSpace;
class TGo4ViewPanel;
class TPad;
class TGo4Slot;
class TGo4ObjectManager;
class TGo4AnalysisProxy;
class TGo4BrowserProxy;
class TGo4WidgetProxy;

class QAction;
class QSignalMapper;
class QMenu;

 /** This is the Go4 Main Window
   * @author Mohammad Al-Turany, modifications J.Adamczewski, S.Linev
   * @version 1.0
   * @since 15.05.2002 */

class TGo4MainWindow : public QMainWindow {
   Q_OBJECT

   public:
      TGo4MainWindow(QApplication*);
      virtual ~TGo4MainWindow();

      void HotStart(const char* fname);
      void ProcessQtEvents();
      void StatusMessage(const QString& mess);
      const char* LastTypedPassword() const;
      TGo4ViewPanel* FindViewPanel(const char* name);
      TGo4ViewPanel* DisplayBrowserItem(const char* itemname, TGo4ViewPanel* panel, TPad* pad, bool activate, int updatelevel, const char* drawopt);

      TGo4ObjectManager* OM();
      TGo4BrowserProxy* Browser();

   public slots:

      void about();
      void aboutQt();
      void aboutROOT();
      void windowsMenuAboutToShow();
      void AddAnalysisBar();
      void AddAnalysisMenu();
      void AddFileMenu();
      void AddFileToolBar();
      void AddSettingMenu();
      void AddToolsBar();
      void AddToolsMenu();
      void UserPanelSlot();
      TGo4ViewPanel* MakeNewPanel(int div = 0);
      void LoadLibrarySlot();
      void CloseAllFilesSlot();
      void OpenFileSlot();
      void OpenRemoteFileSlot();
      void ConnectHServerSlot();
      void SaveFileSlot();

      void InputTerminalParametersSlot();
      void LogSettingsSlot();

      void IntroHelpSlot();
      void RefHelpSlot();
      void FitHelpSlot();
      void CloseAllWindows();
      void MinAllWindows();
      void ToggleFullScreenSlot();
      void SaveSettingsSlot();
      void ChangeFontSlot();
      void ChangeTerminalFontSlot();
      void SetStyleSlot(const QString&);
      void windowsMenuActivated( int id );
      void ForseCloseSlot();

      void ChangeFetchWhenDrawSlot();
      void ChangeFetchWhenCopySlot();
      void ChangeFetchWhenSaveSlot();
      void ChangeHideEventElement();
      void ChangeDrawOnceSlot();

      void CanvasColorSlot();
      void CrosshairSlot();
      void EventStatusSlot();
      void MarkerSettingsSlot();
      void OptStatsSlot();
      void ChangeCloneFlagSlot();
      void ChangeDrawTimeFlagSlot();
      void ChangeDrawDateFlagSlot();
      void ChangeDrawItemFlagSlot();

      void TH1DrawOptSlot();
      void TH2DrawOptSlot();
      void TH3DrawOptSlot();
      void TGraphDrawOptSlot();
      void GStyleStatFormatSlot();

      void PrepareForClientConnectionSlot(bool interactive = true);
      void LaunchClientSlot(bool interactive = true);
      void ConnectServerSlot(bool interactive = true, const char* password = "");
      void DisconnectAnalysisSlot(bool interactive = true);
      void ShutdownAnalysisSlot(bool interactive = true);
      void ToggleAnalysisConfiguration();
      void ToggleAnalysisWindow();
      void CloseAnalysisWindow();

      void SubmitAnalysisSettings();
      void SubmitStartAnalysisSlot();
      void StartAnalysisSlot();
      void StopAnalysisSlot();

      void CreateNewHistSlot(int isremote = -1);
      void CreateNewConditionSlot(bool forothereditor = false);
      void CreateNewDynEntrySlot(bool forothereditor = false);

      // starting editros slots
      TGo4FitPanel* StartFitPanel();
      TGo4ParaEdit* StartParaEdit(const char* itemname = 0);
      TGo4HistogramInfo* StartHistogramInfo();
      TGo4ConditionInfo* StartConditionInfo();
      TGo4EditDynEntry* StartEditDynEntry();
      TGo4EventInfo* StartEventInfo();
      TGo4ConditionEditor* StartConditionEditor();

      TGo4AnalysisConfiguration* FindAnalysisConfiguration();
      TGo4AnalysisWindow* FindAnalysisWindow();

      // slots connected to editors
      void editorServiceSlot(QGo4Widget* editor, int serviceid, const char* str, void* par);

      void ProcessHotStart();
      void StopGUIScriptSlot();
      void CreateGUIScriptSlot();
      void CheckConnectingCounterSlot();
      void checkPanelRepaintSlot();

   protected:

      virtual void closeEvent( QCloseEvent * ce );

      void ConnectGo4Widget(QGo4Widget* editor);

      void UpdateCaptionButtons();

      bool startUserGUI(const char*);
      TGo4AnalysisProxy* AddAnalysisProxy(bool isserver, bool needoutput);
      bool RemoveAnalysisProxy(int waittime = 30, bool servershutdown = false);
      TGo4AnalysisStatusMonitor* EstablishRatemeter(int level);
      TGo4AnalysisConfiguration* EstablishAnalysisConfiguration(int level);

      QGo4Widget* FindGo4Widget(const char* name, bool activate);
      TGo4Slot* GetWidgetTopSlot(QGo4Widget* widget, bool force);

      bool SaveBrowserItemToFile(const char* itemname, const char* subfolder);

      void UpdateDockAnalysisWindow();
      void TerminateAnalysis(bool interactive = true);

      void ToggleMbsMonitor(const char* nodename);
      TGo4SetScaleValues*  ToggleScaleValues();

      void SavePanelCanvas(TGo4ViewPanel* panel);

      void HelpWindow(const char* filename, const char* msg = 0);

      void UpdateBrowser();

      QApplication*      fApp;

      QMenu*             windowsMenu;
      QSignalMapper*     winMapper;
      TGo4WorkSpace*     fxWorkSpace;

      TGo4ObjectManager* fxOM;
      QString            fOMDataPath;
      QString            fOMBrowserPath;
      QString            fOMEditorsPath;

      QString            fKillCommand;

      int                fCloseCounter;
      int                fConnectingCounter;
      QString            fLastPassword;
      QString            fLastFileDir;

      bool               fbPanelTimerActive;
      bool               fbFullScreen;

      QAction*           faFetchWhenDraw;
      QAction*           faFetchWhenCopy;
      QAction*           faFetchWhenSave;
      QAction*           faHideEventElement;
      QAction*           faDrawOnce;

      QAction*           faCrosshair;
      QAction*           faEventstatus;
      QAction*           faClone;
      QAction*           faDrawTime;
      QAction*           faDrawDate;
      QAction*           faDrawItem;

      QAction*           faLaunchAnal;
      QAction*           faConnectAnal;
      QAction*           faPrepareAnal;
      QAction*           faDisconnectAnal;
      QAction*           faShutdownAnal;
      QAction*           faSumbStartAnal;
      QAction*           faStartAnal;
      QAction*           faStopAnal;
      QAction*           faAnalConfig;
      QAction*           faAnalTermin;
};

#endif
