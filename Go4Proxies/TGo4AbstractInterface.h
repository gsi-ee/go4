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

#ifndef TGO4ABSTRACTINTERFACE_H
#define TGO4ABSTRACTINTERFACE_H

#include "TObject.h"

#include "TString.h"
#include "TList.h"
#include <ostream>

class TGo4ObjectManager;
class TGo4BrowserProxy;
class TGo4AnalysisProxy;
class TGo4ServerProxy;
class TPad;
class TGo4Picture;

typedef void *ViewPanelHandle;

enum EGo4ViewPanelMode { Go4_hidden = -1, Go4_minimized = 0, Go4_normal = 1, Go4_maximized = 2 };


/** @brief Generic interface to GUI functionality.
  *
  * @details Provide methods like open file, request object from analysis and so on.
  *
  * Methods of that class can be used in arbitrary ROOT script inside/outside Go4 GUI.
  * To access these methods, one should use "go4" variable, which is exported
  * to CINT global variable space when instance of implementation of
  * TGo4AbstractInterface class is created. One can also access such instance
  * also via TGo4AbstractInterface::Instance() static method.
  *
  * There are two implementations of TGo4AbstractInterface class:
  *  - TGo4Script - used together with Go4 GUI, created automatically when GUI started
  *  - TGo4Interface - can be used in non GUI mode, must be created once by user
  * As long as these are two implementation of the same interface, macro, which
  * uses internally only go4 instance, should work similarly in both GUI and non GUI mode.
  *
  * Typically one should use interface, provided by that class, in macro,
  * executed inside GUI. Simple example of such macro:
  * ~~~{cpp}
  * {
  *    go4->OpenFile("example.root");
  *    go4->DrawItem("example.root/hist1");
  * }
  * ~~~
  *
  * Here one open file from disk and displays one histogram from that file.
  *
  * Most of the action with objects are going via object manager, where each
  * object (item) has unique name, which includes full path to that object in
  * objects structures. In our example item name is "example.root/hist1".
  * To locate object of known name in objects structure, one can use
  * FindItem() method. In that case example will look like:
  * ~~~{cpp}
  * {
  *    go4->OpenFile("example.root");
  *    go4->DrawItem(go4->FindItem("hist1"));
  * }
  * ~~~
  *
  * There are several examples of GUI macros, which can be found in
  * $GO4SYS/macros directory. Description of each method can be
  * seen further in this document. Hot start feature of go4 was implemented
  * with usage of that class, therefore one can produce hotstart files in Go4 GUI
  * and take some generated code from it directly. For instance,
  * displaying of complex viewpanels or starting and configuring of analysis.
  *
  * @ingroup go4_gui
  */

class TGo4AbstractInterface : public TObject {
   private:
      TGo4ObjectManager *fOM{nullptr};
      TGo4BrowserProxy * fBrowser{nullptr};
      TList              fxCommands;

   protected:
      /** Constructor */
      TGo4AbstractInterface();

      static TGo4AbstractInterface *fgInstance;        //!
      static TString                fInitSharedLibs;   //! list of shared libraries when application is started

      void Initialize(TGo4ObjectManager *om, TGo4BrowserProxy *br);

      virtual void ProcessEvents(Int_t timeout = -1) = 0;

      Bool_t LoadHotStart(const char *filename);
      Bool_t IsHotStart() const;
      const char *NextHotStartCmd();
      void FreeHotStartCmds();
      static void ProduceLoadLibs(std::ostream &fs);

      /** Returns default hotstart file extension */
      static const char *DefaultPicTitle();


   public:

      /** @brief method to set initial list of shared libraries,
       * which than will not be included in hotstart file */
      static void SetInitSharedLibs(const char *libs = nullptr);

      /** destructor */
      virtual ~TGo4AbstractInterface();

      /** @brief Return pointer on instance of implementation of TGo4AbstractInterface class
        * @details Normally, inside CINT script one should use "go4" variable,
        * which contains TGo4AbstractInterface::Instance() value. */
      static TGo4AbstractInterface *Instance();

      /** @brief Delete instance of interface class */
      static void DeleteInstance();

      /** @brief Returns default hotstart file extension */
      static const char *FileExtension();

      /** @brief Default delay during hotstart file extension */
      static Int_t DelayMillisec();

      /** @brief Returns pointer on object manager.
        * @details TGo4ObjectManager class should only be used when standard interface
        * does not provide required functionality. Can be accessed directly via "om" variable. */
      TGo4ObjectManager *OM() const { return fOM; }

      /** @brief Returns pointer on browser proxy.
        * @details TGo4BrowserProxy class provides complete interface to Go4 browser functionality.
        * Should only be used when interface does not provide required functionality.
        * Can be accessed directly via "br" variable. */
      TGo4BrowserProxy *Browser() const { return fBrowser; }

      /** @brief Returns pointer on analysis proxy.
        * @details TGo4AnalysisProxy class provide access to analysis controlling instance in program.
        * Normally should not be used in GUI script. */
      TGo4AnalysisProxy *Analysis();

      /** @brief Returns pointer on more generic server proxy
        * @details TGo4ServerProxy class provide access to analysis controlling instance in program.
        * Normally should not be used in GUI script. */
      TGo4ServerProxy *Server();

      /** @brief Executes hotstart file, generated in go4 GUI.
        * @details Hot start files can not be executed as normal CINT scripts,
        * therefore one should use this method to activate them */
      virtual void HotStart(const char *filename) = 0;

      // general purpose

      /** @brief Load specified ROOT library */
      virtual void LoadLibrary(const char *fname);

      /** @brief Open specified file in read-only mode
        * @details File and its structure should appear in browser. */
      virtual void OpenFile(const char *fname);

      /** @brief Find item with given object name.
        * @details Item name includes object name and name of all parent folders.
        * For instance histogram of name "His1" in analysis will have
        * item name "Analysis/Histograms/His1". */
      virtual TString FindItem(const char *objname);

      /** @brief Copy item to workspace.
        * @details If there is subfolders with items, they also will be copied. */
      virtual Bool_t CopyItem(const char *itemname);

      /** @brief Delete item (if allowed).
        * @details Can be used to close file, delete memory object, close histogram server connection. */
      virtual Bool_t DeleteItem(const char *itemname);

      /** @brief Fetch item from data source.
        * @details Request item from data source, to which item corresponds to.
        * If this is file or histogram server, object will be returned immediately.
        * If this is an item from analysis, first request will be send and
        * then interface will wait "wait_time" milliseconds that object is arrived.
        * If wait_time == 0, no waiting will be done and most probably, new object will
        * be assigned to that item several seconds after method is return */
      virtual void FetchItem(const char *itemname, Int_t wait_time = 2000);

      /** @brief Returns object, assigned to specified browser item.
        * @details Parameter updatelevel specifies, how object will be requested from
        * data source, to which item correspond to.
        * Possible values for updatelevel parameter:
        *    0 - no request will be done, return last requested object
        *    1 - request to data source only first time, no waiting
        *    2 - request to data source in any case, no waiting
        *   >9 - request to data source and wait as many milliseconds as specified by updatelevel
        * Waiting required, when object requested from the analysis */
      virtual TObject *GetObject(const char *itemname, Int_t updatelevel = 1);

      /** @brief Save object in browser workspace.
        * @details Object of any type can be saved in browser. It will appear in browser folder
        * "Workspace", where local memory objects are placed.
        * If item of that name exists in browser, it will be overwritten by new object
        * Returns path to stored object in browser workspace.
        * @param  path  - subpath in "Workspace" folder, where object should be placed
        * @param  obj   - pointer ob object
        * @param  ownership - is browser becomes owner of that object */
      virtual TString SaveToMemory(const char *path, TObject *obj, Bool_t ownership = kFALSE);

      /** @brief Save specified browser item to file.
        * @details Only object, which are already fetched from the data source, will be saved.
        * Therefore, before one call this method, one probably should use
        * FetchItem() method to get objects from file/analysis/histogram server before save them.
        * If item is folder and contains sub-items, they also will be saved.
        * Only binary ROOT files (extension .root) and XML file (extension .xml) are supported. */
      virtual Bool_t SaveToFile(const char *itemname,
                                const char *filename,
                                const char *filetitle = nullptr);

      /** @brief Export browser item to different file formats.
        * @details One, probably, should use FetchItem() before calling this method.
        * If item is folder and contains sub-items, they also will be exported.
        * @param itemname - name of browser, which should be exported,
        * @param dirpath  - directory on disk, where files should be created.
        * @param format   - export file format, can be:
                  - "ASCII" - text format
                  - "Radware" - Radware format (Origin)
                  - "ROOT" - binary ROOT format
                  - "ROOT XML"  - xml ROOT format
        * @param filetitle - title of create file (only for ROOT formats) */
      virtual Bool_t ExportToFile(const char *itemname,
                                  const char *dirpath,
                                  const char *format,
                                  const char *filetitle = nullptr);

      /** @brief Connect to GSI histogram server.
        * @details Creates appropriate entry in browser and provides access to histogram.
        * Several connections to different histogram servers are allowed
        *   @param  servername - IP server name
        *   @param  portnumber - socket port number
        *   @param  basename - histogram server base name
        *   @param  userpass - password to access histogram server
        *   @param  filter   - filter for histograms names */
      virtual void ConnectHServer(const char *servername,
                                  Int_t portnumber,
                                  const char *basename,
                                  const char *userpass,
                                  const char *filter);

      /** @brief Connect to DABC server.
       * @details Address like dabc://host:port */
      virtual void ConnectDabc(const char *servername);

      /** @brief Connect to HTTP server.
       * @details Address like http://host:port/subfolder/
       * authentication may be specified by username account and password pass*/
      virtual TGo4ServerProxy *ConnectHttp(const char *servername, const char *account = nullptr, const char *pass = nullptr) { return nullptr; }

      /** @brief Wait specified number of seconds.
        * @details Suppress macro execution, but keeps GUI functional, therefore
        * it is different from gSystem->Sleep() call. */
      virtual void Wait(double tm_sec) {}

      /** @brief Display message in GUI status line */
      virtual void Message(const char *msg) {}

      // analysis start and configuration

      /** @brief Launch analysis in client mode, using default configuration.
        * @details Can only work in standard GUI mode */
      virtual void LaunchAnalysis() {}

      /** @brief Launch analysis in client mode.
        * @param ClientName - arbitrary name of analysis, used for display
        * @param ClientDir  - directory, where analysis should be started
        * @param ClientExec - main analysis executable
        * @param ClientNode - node name, where analysis should be started
        * @param ShellMode  - shell, used to launch analysis: 1 - rsh, 2 - ssh [default]
        * @param TermMode   - terminal program: 1 - internal Qt window, 2 - xterm, 3 - KDE konsole
        * @param ExeMode    - execution mode: 0 - exe, 1 - library
        * @param UserArgs   - extra arguments for user analysis */
      virtual void LaunchAnalysis(const char *ClientName,
                          const char *ClientDir,
                          const char *ClientExec,
                          const char *ClientNode,
                          Int_t ShellMode =  2,     //   0 = exec, 1 = rsh,  2 = ssh
                          Int_t TermMode = 1,       //   1 = Qt,   2 = Xterm, 3 = konsole
                          Int_t ExeMode = 0,        //   0 = exe,  1 = lib
                          const char *UserArgs = nullptr) {}

      /** @brief Connect to running analysis server.
        * @param ServerNode - node to connect to,
        * @param ServerPort - connection port number,
        * @param UserMode   - mode of user operation : 0 - observer, 1 - controller, 2 - administrator
        * @param password   - access password, which should correspond to specified UserMode */
      virtual void ConnectAnalysis(const char *ServerNode,
                                   Int_t ServerPort,
                                   Int_t UserMode,  // 0 - observer, 1- controller, 2 - administrator
                                   const char *password = nullptr)  {}

      /** @brief Waits, until connection to analysis is established.
        * @details Method must be called before any other action like configuration,
        * start/stop can be done. If analysis is connected,
        * IsAnalysisConnected() return kTRUE. delay_sec specifies, how long
        * one should wait until analysis is connected */
      virtual void WaitAnalysis(Int_t delay_sec) {}

      /** @brief Indicate, if analysis was successfully connected */
      virtual Bool_t IsAnalysisConnected();

      /** @brief Disconnects from running analysis.
        * @details If analysis is running in client mode, analysis will be shutdown. */
      virtual void DisconnectAnalysis() {}

      /** @brief Shutdown running analysis.
        * @details If analysis is running in server mode,
        * only user with administrator privileges can do this */
      virtual void ShutdownAnalysis() {}

      /** @brief Execute one macro line on analysis side.
        * @details This allows to call any action on analysis, including
        * execution scripts with ".x userscript.C". One should not mix
        * scripts, written for analysis and scripts, written for GUI. */
      virtual void ExecuteLine(const char *remotecmd);

      /** @brief Requests current analysis configuration */
      virtual void RequestAnalysisConfig();

      /** @brief Submit configuration to analysis.
        *
        * @details Configurations can be changed with following methods:
        *    - AnalysisAutoSave();
        *    - AnalysisConfigName();
        *    - ConfigStep();
        *    - StepFileSource();
        *    - StepMbsFileSource();
        *    - StepMbsStreamSource();
        *    - StepMbsTransportSource();
        *    - StepMbsEventServerSource();
        *    - StepMbsRevServSource();
        *    - StepRandomSource();
        *    - StepUserSource();
        *    - StepFileStore();
        *    - StepBackStore().
        *
        * To understand more how these methods can be used together,
        * one should launch analysis, configure it and then generate
        * hotstart file. In this file one can find correct sequence and
        * parameters for all these methods. Timeout defines total time which
        * will be required to get response from analysis that operation was completed. */
      virtual void SubmitAnalysisConfig(int tmout = 20) {}

      /** @brief Starts (resume) analysis execution */
      virtual void StartAnalysis() {}

      /** @brief Stop (suspend) analysis execution */
      virtual void StopAnalysis() {}

      /** @brief Sends new request to the analysis and waits for new names list */
      virtual void RefreshNamesList(int tmout = 10) {}

      /** @brief Set analysis terminal window mode.
        * @param mode indicate that terminal window:
        *   - -1 - closed,
        *   - 0 - minimized,
        *   - 1 - normal state */
      virtual void SetAnalysisTerminalMode(int mode) {}

      /** @brief Set analysis configuration window mode.
        * @param mode indicate that configuration window:
        *    - -1 - closed,
        *    - 0 - minimized,
        *    - 1 - normal state */
      virtual void SetAnalysisConfigMode(int mode) {}

      /** @brief Enable/disable monitoring of browser item.
        * @details In monitoring mode item regularly will be requested from analysis
        * and viewpanels, where item is drawn, will be updated */
      virtual void MonitorItem(const char *itemname, Bool_t on = kTRUE);

      /** @brief Start monitoring mode.
        * @details Parameter "period" specifies how often (in seconds) each
        * monitored item will be updated. */
      virtual void StartMonitoring(Int_t period = 10);

      /** @brief Stop monitoring mode. */
      virtual void StopMonitoring();

      /** @brief Configure autosave properties of analysis */
      virtual void AnalysisAutoSave(const char *filename,
                                    Int_t interval,
                                    Int_t compression,
                                    Bool_t enabled,
                                    Bool_t overwrite) {}

      /** @brief Configure name of file, where analysis configuration will be saved */
      virtual void AnalysisConfigName(const char *filename) {}

      // steps configuration

      /** @brief Set basic step properties */
      virtual void ConfigStep(const char *stepname,
                              Bool_t enableprocess,
                              Bool_t enablesource,
                              Bool_t enablestore) {}

      /** @brief Set file source as step data source */
      virtual void StepFileSource(const char *stepname,
                                  const char *sourcename,
                                  int timeout) {}

      /** @brief Set MBS file source as step data source */
      virtual void StepMbsFileSource(const char *stepname,
                                     const char *sourcename,
                                     int timeout,
                                     const char *TagFile,
                                     int start = 0,
                                     int stop = 0,
                                     int interval = 0) {}

      /** @brief Set MBS stream server as step data source */
      virtual void StepMbsStreamSource(const char *stepname,
                                       const char *sourcename,
                                       int timeout,
                                       int start = 0,
                                       int stop = 0,
                                       int interval = 0) {}

      /** @brief Set MBS transport server as step data source */
      virtual void StepMbsTransportSource(const char *stepname,
                                          const char *sourcename,
                                          int timeout,
                                          int start = 0,
                                          int stop = 0,
                                          int interval = 0) {}

      /** @brief Set MBS event server as step data source */
      virtual void StepMbsEventServerSource(const char *stepname,
                                    const char *sourcename,
                                    int timeout,
                                    int start = 0,
                                    int stop = 0,
                                    int interval = 0) {}

      /** @brief Set MBS remote event server as step data source */
      virtual void StepMbsRevServSource(const char *stepname,
                                const char *sourcename,
                                int timeout,
                                int port = 0,
                                int start = 0,
                                int stop = 0,
                                int interval = 0) {}

      /** @brief Set event selection for MBS source */
      virtual void StepMbsSelection(const char *stepname,
                                    int start,
                                    int stop,
                                    int interval) {}

      /** @brief Set random generator as step data source */
      virtual void StepRandomSource(const char *stepname,
                            const char *sourcename,
                            int timeout) {}

      /** @brief Set custom port number for MBS source like stream or transport server */
      virtual void StepMbsPort(const char *stepname,
                              int port) {}

      /** @brief Set retry number for MBS source like stream or transport server */
      virtual void StepMbsRetryCnt(const char *stepname,
                                   int cnt) {}

      /** @brief Set user data source as step data source */
      virtual void StepUserSource(const char *stepname,
                          const char *sourcename,
                          int timeout,
                          int port,
                          const char *expr) {}

      /** @brief Set hdf5 data source as step data source */
      virtual void StepHDF5Source(const char *stepname,
                                  const char *sourcename,
                                  int timeout) {}

      /** @brief Set file as step data storage */
      virtual void StepFileStore(const char *stepname,
                         const char *storename,
                         bool overwrite,
                         int bufsize,
                         int splitlevel,
                         int compression,
                         int autosaveperiod = 0) {}

      /** @brief Set back store as step data storage */
      virtual void StepBackStore(const char *stepname,
                         const char *storename,
                         int bufsize,
                         int splitlevel) {}

      /** @brief Set user store as step data storage */
      virtual void StepUserStore(const char *stepname,
                         const char *storename) {}

      /** @brief Set hdf5 data source as step data storage*/
      virtual void StepHDF5Store(const char *stepname,
                                  const char *storename,
                                  int flags) {}

      // windows management

      /** @brief Set state of all tool boxes in main window */
      virtual void SetMainWindowState(int qtversion, const char *val) {}

      /** @brief Set geometry of main window */
      virtual void SetMainWindowGeometry(int qtversion, const char *val) {}

      /** @brief Create new view panel.
        * @details Handle, returned by this method, must be used for other operation,
        * like DivideViewPanel() or SelectPad() */
      virtual ViewPanelHandle StartViewPanel() { return nullptr; }

      /** @brief Create new viewpanel with specified parameters.
        * @details Returns handle of newly created viewpanel.
        * To understand usage of this method together with TGo4Picture class,
        * one can create and configure viewpanel and then create hotstart file.
        * This file will include complete TGo4Picture configuration, which
        * contains attributes like colors, ranges, pad divisions and so on.
        * @param x - X coordinate of view panel;
        * @param y - Y coordinate of view panel;
        * @param width - panel width;
        * @param height - panel height;
        * @param mode  - display mode for view panel (0 - minimized, 1 - normal, 2 - maximized);
        * @param pic - TGo4Picture object, which includes configuration of viewpanel. */
      virtual ViewPanelHandle StartViewPanel(int x, int y, int width, int height, int mode = 1, TGo4Picture *pic = nullptr) { return nullptr; }

      /** @brief Return name of viewpanel. */
      virtual TString GetViewPanelName(ViewPanelHandle panel) { return TString(); }

      /** @brief Return handle on viewpanel with specified name. */
      virtual ViewPanelHandle FindViewPanel(const char *name) { return nullptr; }

      /** @brief Change name of viewpanel. */
      virtual Bool_t SetViewPanelName(ViewPanelHandle panel, const char *newname) { return kFALSE; }

      /** @brief Return handle of last active viewpanel. */
      virtual ViewPanelHandle GetActiveViewPanel() { return nullptr; }

      /** @brief Forces of panel redraw. */
      virtual void RedrawPanel(ViewPanelHandle panel) {}

      /** @brief Divide viewpanel on subpads. */
      virtual void DivideViewPanel(ViewPanelHandle panel, Int_t numX, Int_t numY) {}

      /** @brief Set active pad on viewpanel.
        * @details If number == 0, main pad (canvas) will be set as active,
        * otherwise one of subpads will be activated.
        * Most of methods, working with viewpanel, acting with active pad of this panel. */
      virtual TPad *SelectPad(ViewPanelHandle panel, Int_t number = 0) { return nullptr; }

      /** @brief Returns pad options for pad with specified number */
      virtual TGo4Picture *GetPadOptions(ViewPanelHandle panel, Int_t padnumber = 0) { return nullptr; }

      /** @brief Set superimpose flag for active pad of viewpanel. */
      virtual void SetSuperimpose(ViewPanelHandle panel, Bool_t on = kTRUE) {}

      /** @brief Set apply to all flag for active pad of viewpanel. */
      virtual void SetApplyToAll(ViewPanelHandle panel, Bool_t on = kTRUE) {}

      /** @brief Draw browser item on specified viewpanel.
        * @param itemname - browser item name,
        * @param panel    - viewpanel, if panel == nullptr, new viewpanel will be created.
        * @param drawopt  - draw options, used in obj->Draw() operation */
      virtual Bool_t DrawItem(const char *itemname, ViewPanelHandle panel = nullptr, const char *drawopt = nullptr) { return kFALSE; }

      /** @brief Redraw item of given name on all viewpanels/editors.
        * @details Useful for the case, when content of object (histogram, for example)
        * changed directly in script and after that should be updated in viewpanel. */
      virtual void RedrawItem(const char *itemname);

      /** @brief Provide item name, drawn in the view panel.
       *  @details Optional cnt parameter could specify sequence number in panel, including all subpads */
      virtual const char *GetDrawnItemName(ViewPanelHandle panel, int cnt = 0) { return nullptr; }

      /** @brief Starts fit panel */
      virtual void StartFitPanel() {}

   ClassDefOverride(TGo4AbstractInterface, 0);
};


#endif
