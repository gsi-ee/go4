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

#ifndef TGO4ANALYSISIMP_H
#define TGO4ANALYSISIMP_H

#include "TObject.h"
#include "TGo4CommandReceiver.h"
#include "TString.h"

class TH1;
class TH2;
class TTree;
class TGraph;
class TF1;
class TCanvas;
class TFolder;
class TNamed;
class TMutex;
class TStopwatch;
class TFile;

class TGo4AnalysisStepManager;
class TGo4AnalysisObjectManager;
class TGo4AnalysisObjectNames;
class TGo4AnalysisSniffer;
class TGo4AnalysisStatus;
class TGo4AnalysisWebStatus;
class TGo4AnalysisStep;
class TGo4AnalysisClient;
class TGo4HistogramStatus;
class TGo4DynamicEntry;
class TGo4Condition;
class TGo4WinCond;
class TGo4PolyCond;
class TGo4ShapedCond;
class TGo4ListCond;
class TGo4RollingGraph;
class TGo4Parameter;
class TGo4ParameterStatus;
class TGo4Picture;
class TGo4Fitter;
class TGo4ObjectStatus;
class TGo4EventStoreParameter;
class TGo4EventSourceParameter;
class TGo4EventProcessorParameter;
class TGo4EventElement;
class TGo4EventStore;
class TGo4EventSource;
class TGo4EventProcessor;
class TGo4TreeStructure;
class TGo4InterruptHandler;
class TGo4AnalysisWebStatus;
class TGo4Sniffer;
class TGo4Ratemeter;

/**
 * @brief The mother of all go4 analysis.
 * @details  Provides services to organize
 * events, histogram data, and everything. May run standalone in batch mode,
 * or may be aggregated by analysis client for threaded online control. User
 * has to derive his/her own analysis from this base class and override the
 * virtual user event function and set event factory methods.
 * @ingroup go4_major_classes
 * @author J. Adamczewski
 * @since 19-jan-2001
 */

class TGo4Analysis : public TObject, public TGo4CommandReceiver {

   friend class TGo4AnalysisStep;
   friend class TGo4AnalysisClient;
   friend class TGo4ComGetCurrentEvent;
   friend class TGo4ComGetNamesList;
   friend class TGo4ComGetEnvelope;
   friend class TGo4ComSetObject;
   friend class TGo4HisConnectorRunnable;
   friend class TGo4HistogramServer;
   friend class TGo4AnalysisWebStatus;
   friend class TGo4Sniffer;

public:

   /** Definition of default autosave interval. */
   static const Int_t fgiAUTOSAVECOUNTS;        //!

   /** This value is the number of events in between subsequent dynamic list processing */
   static const Int_t fgiDYNLISTINTERVAL;       //!

   /** Time in ms to sleep in the polling loop of the WaitForStart */
   static const Int_t fgiMACROSTARTPOLL;

   /** standard file name */
   static const char *fgcDEFAULTFILENAME;    //!

   /** standard file name for analysis settings */
   static const char *fgcDEFAULTSTATUSFILENAME; //!

   /** filename suffix for object file */
   static const char *fgcDEFAULTFILESUF;     //!

   /** leading character indicating python script execution*/
   static const char fgcPYPROMPT;

   /** default name of go4 python init script*/
   static const char *fgcPYINIT;

   /** @brief return analysis instance */
   static TGo4Analysis *Instance();

   /** @brief check if analysis instance exists */
   static Bool_t Exists();

   /** @brief is batch mode
    * @details Return kTRUE if analysis will run in batch (without connection to GUI) */
   static Bool_t IsBatchMode();

   /** @brief is client mode
    * @details Return kTRUE if analysis will run in gui mode, connected to gui as client */
   static Bool_t IsClientMode();

   /** @brief is server mode
    * @details Return kTRUE if analysis will run in server mode and gui can connect to the analysis */
   static Bool_t IsServerMode();

   /** @brief Set analysis running mode
    * @param mode defines running mode
    * - 0 - batch mode
    * - 1 - client mode
    * - 2 - server mode */
   static void SetRunningMode(int mode);

   /** @brief destructor */
   virtual ~TGo4Analysis();

   /** @brief Return analysis name */
   const char *GetName() const override { return fAnalysisName.Data(); }

   /** @brief Set analysis name */
   void SetAnalysisName(const char *name) { fAnalysisName = name; }

   /** @brief The main analysis event cycle.
    * @details This method is called by the
    * main runnable, or by RunImplicitLoop(). Processes the analysis steps
    * first, then the user event function. Finally, the dynamic list is updated. */
   Int_t MainCycle();

   /** @brief Method executed once before the main analysis event loop. This method is called by the thread prerun, or at the beginning of
    * RunImplicitLoop(). Calls virtual user preloop function. */
   Int_t PreLoop();

   /** @brief Method executed once after the main analysis event loop.
    * @details  This method is called by the thread postrun, or at the end of
    * RunImplicitLoop(). Calls virtual user postloop function. */
   Int_t PostLoop();

   /** @brief This method is meant to be called from a go4 cintserver macro
    * in an explicit event loop.
    * @details It executes any queued commands and calls MainCycle() once, protected
    * by main go4 lockguard. Returns 0 if analysis has IsRunning() state true;
    * returns negative value if IsRunning() is false. */
   Int_t Process();

   /** @brief User defined function which processes the actual analysis.
    * @details May be called explicitly from analysis client thread, or may run
    * in an implicit loop provided by method RunImplicit, if
    * analysis works in a non threaded standalone mode. To be overridden
    * in the user analysis class. */
   virtual Int_t UserEventFunc();

   /** @brief User defined function called once before processing the main
    * event loop. Optionally. */
   virtual Int_t UserPreLoop();

   /** @brief User defined function called once after processing the main
    * event loop. Optionally. */
   virtual Int_t UserPostLoop();

   /** @brief Processes the UserEventFunc in an implicit loop for "times" times.
    * @details For non-threaded usage of the analysis class.
    * @param times specified how many events should be processed, 0 - all
    * @param showrate identifies if ratemeter should be shown
    * @param process_event_interval allows regularly process ROOT system events
    * @param iswebserver specifies if loop shall be suspended at end of event source, or returns from batch  */
   Int_t RunImplicitLoop(Int_t times, Bool_t showrate = kFALSE,
                         Double_t process_event_interval = -1., Bool_t iswebserver = kFALSE);

   /** @brief Finish the analysis run and close all event sources/storages.
    * @details The analysis instance and the setup of the analysis steps is not deleted. */
   virtual void CloseAnalysis();

   /** @brief Initialization of the event class plugins which are delivered from
    * the user defined event factory.
    * @details The virtual standard base class method may be changed
    * by overriding; normally, this is not necessary. */
   virtual Bool_t InitEventClasses();

   /** @brief Returns pointer on analysis object manager */
   TGo4AnalysisObjectManager *ObjectManager() const { return fxObjectManager; }

   /** @brief Add (create) new dynamic histogram entry which connects an existing
    * histogram with existing condition and data.
    * @details Dynamic entry is specified by name. Histogram histo will be searched in
    * registered histograms folder, condition in conditions folder. If
    * condition is true or not existing (condition == nullptr), histogram will be filled
    * from the values that are found in registered eventstructure classes of
    * names  evx, evy, evz at the data members of names memx, memy, memz,
    * for the three coordinate axes, respectively. Histogram dimension is
    * checked against given values. */
   Bool_t AddDynamicHistogram(const char *name,
                              const char *histo,
                              const char *hevx, const char *hmemx,
                              const char *hevy = nullptr, const char *hmemy = nullptr,
                              const char *hevz = nullptr, const char *hmemz = nullptr,
                              const char *condition = nullptr,
                              const char *cevx = nullptr, const char *cmemx = nullptr,
                              const char *cevy = nullptr, const char *cmemy = nullptr);

   /** @brief Set dynamic entry of name "name" to the values specified by external
    * dynamic entry status "state".
    * @details If no dynamic entry of that name exists, a
    * new entry is created with the properties of the state, and
    * added to the current dynamic list. */
   Bool_t AddDynamicEntry(TGo4DynamicEntry *entry);

   /** @brief Remove entry of that name from dynamic list of listname.
    * @details The referred objects (histograms, conditions..) are still on heap, since
    * they are owned by their special folders. */
   Bool_t RemoveDynamicEntry(const char *entryname, const char *listname = nullptr);

   /** @brief Add any external object to the user object folder.
    * @details Object is owned by go4 afterwards and will be saved automatically.
    * Object is accessible by name from the go4 display and from the
    * analysis itself. Subfolder of UserObjects may be specified.
    * If replace is true, old object of same name will be deleted and
    * replaced by the added one. */
   Bool_t AddObject(TNamed *anything, const char *subfolder = nullptr, Bool_t replace = kTRUE);

   /** @brief Searches for object by name in all directories.
    * @details Returns pointer to object. If
    * object of that name does not exist, null is returned.
    * Optionally, folder may be specified to search in. */
   TNamed *GetObject(const char *name, const char *folder = nullptr);

   /** @brief Removes object from user object folder by name.
    * @details Returns kFALSE if no such histogram.
    * Object is deleted on heap only if del is true.
    * Otherwise, user owns the object afterwards. */
   Bool_t RemoveObject(const char *name, Bool_t del = kTRUE);

   /** @brief Delete object of name, or all objects in folder name, respectively.
    * @details Objects are only deleted if delete protection is false. Usually,
    * Objects registered from user code are delete protected by default.
    * Objects created dynamically from gui are not delete protected. */
   Bool_t DeleteObjects(const char *name);

   /** @brief Clear (reset) the specified objects.
    * @details  Method will first clear all objects in any folder if it is matching the name string.
    * If no such folder exists, object of name is searched and cleared.
    * Returns kFALSE in case of not found objects. */
   Bool_t ClearObjects(const char *name);

   /** @brief Change protection properties of object name as specified.
    * @details If object is a folder, change properties of all objects in this folder recursively.
    * Flags may contain key letters like:
    * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
    * "+D"/"-D" to enable/disable protection against object deletion
    * For example flags="+C-D", "+C+D", "-D-C", "-C"
    * Properties not appearing in flags are not changed.  */
   Bool_t ProtectObjects(const char *name, const Option_t *flags);

   /** @brief Clear previous events of backstore tree.
    * @details For reset of dynamic tree histogram
    * If called with clearflag=true, do not reset tree, but only clear
    * the backstore reset bit (done after processing of complete dynamic list)*/
   Bool_t ResetBackStores(Bool_t clearflag = kFALSE);

   /** @brief Delivers pointer to next object of the Go4 folder structure
    * with a name matching the expression expr.
    * @details If reset is true,
    * The list of matching objects will be created anew by comparing
    * all names with expr. If reset is false, the next object of a
    * previously created matching list is returned. Optionally the
    * search can be limited to a given folder. */
   TObject *NextMatchingObject(const char *expr = nullptr,
                               const char *folder = nullptr,
                               Bool_t reset = kFALSE);

   /** @brief Create a tree structure for a certain tree by name */
   TGo4TreeStructure *CreateTreeStructure(const char *treename);

   /** @brief Add external histogram to go4 histogram directory.
    * @details Histogram will be owned
    * and streamed by go4 histogram list afterwards. */
   Bool_t AddHistogram(TH1 *his, const char *subfolder = nullptr, Bool_t replace = kTRUE);

   /** @brief Add reference to a tree in the go4 folder structure.
    * @details If name of subfolder is given, tree will be assigned
    * to subfolder (e.g. subfolder with analysis step name).
    * If subfolder name is zero,
    * the tree will be put into general tree folder. */
   Bool_t AddTree(TTree *tree, const char *subfolder = nullptr);

   /** @brief Remove reference to a tree in the go4 folder structure.
    * @details If name of analysis step is given, tree will be assigned
    * to folder dedicated to that step. In stepname is zero,
    * the tree will be put into general tree folder. */
   Bool_t RemoveTree(TTree *tree, const char *stepname = nullptr);

   /** @brief Add Histogram into the dynamic list which is linked to a tree.
    * @details If Histogram of hisname already exists, this histogram will taken.
    * if not, the histogram will be created on first TTree::Draw.
    * Strings varexp and selection are used for applying cuts and variables to plot. */
   Bool_t AddTreeHistogram(const char *hisname, const char *treename, const char *varexp, const char *cutexp);

   /** @brief Search histogram in histogram list (directory). */
   TH1 *GetHistogram(const char *name);

   /** @brief Search tree in tree folder. */
   TTree *GetTree(const char *name);

   /** @brief Removes histogram from histogram dir by name.
    * @details Returns kFALSE if no
    * such index. Histogram object is deleted on heap.
    * Dynamic list entry for that histogram object is also removed. */
   Bool_t RemoveHistogram(const char *name, Bool_t del = kTRUE);

   /** @brief Puts a new analysis condition object in corresponding list.
    * @details Object is owned by list afterwards. Returns false if object
    * of that name already exists in list. */
   Bool_t AddAnalysisCondition(TGo4Condition *con, const char *subfolder = nullptr);

   /** @brief Set existing analysis condition of name to the values
    * of external condition object con.
    * @details Used to set condition by command
    * from gui, or to update the precompiled condition from the values
    * that were saved to the autosave file. */
   Bool_t SetAnalysisCondition(const char *name, TGo4Condition *con, Bool_t counter = kTRUE);

   /** @brief Retrieves an analysis condition  from list by name.
    * @details Optionally, condition could be checked if it inherits from specified class.
    * Returns 0 if no such condition found. */
   TGo4Condition *GetAnalysisCondition(const char *name, const char *cond_cl = nullptr);

   /** @brief Removes analysis condition from list by name.
    * @details Returns 0 if no such condition. Condition object is deleted on heap. */
   Bool_t RemoveAnalysisCondition(const char *name);

   /** @brief Puts a new parameter object in corresponding folder.
    * @details Object is owned by folder afterwards. Returns false if object
    * of that name already exists in list. */
   Bool_t AddParameter(TGo4Parameter *par, const char *subfolder = nullptr);

   /** @brief Set existing parameter of name to the values
    * of external parameter object par.
    * @details Used to set parameter by command
    * from gui, or to update the precompiled parameter from the values
    * that were saved to the autosave file. */
   Bool_t SetParameter(const char *name, TGo4Parameter *par);

   /** @brief Set existing parameter of name to the values
    * of external parameter status object.
    * @details Used to set parameter by command
    * from gui, or to update the precompiled parameter from the values
    * that were saved to the autosave file. */
   Bool_t SetParameterStatus(const char *name, TGo4ParameterStatus *par);

   /** @brief Retrieves a parameter object by name from the object folder.
    * @details Optionally expected class of parameter object could be specified.
    * Returns 0 if no such parameter found (or class not match). */
   TGo4Parameter *GetParameter(const char *name, const char *parameter_class = nullptr);

   /** @brief Removes parameter by name.
    * @details Returns 0 if no
    * such parameter. Parameter object is deleted on heap. */
   Bool_t RemoveParameter(const char *name);

   /** @brief Puts a new picture object in corresponding folder.
    * @details Object is owned by folder afterwards. Returns false if object
    * of that name already exists in list. */
   Bool_t AddPicture(TGo4Picture *pic, const char *subfolder = nullptr);

   /** @brief Set existing picture of name to the values
    * of external picture object pic.
    * @details Used to set picture by command
    * from gui, or to update the precompiled picture from the values
    * that were saved to the autosave file. */
   Bool_t SetPicture(const char *name, TGo4Picture *pic);

   /** @brief Retrieves a picture object by name from the object folder.
    * @details Returns 0 if no such picture. */
   TGo4Picture *GetPicture(const char *name);

   /** @brief Removes picture by name.
    * @details Returns kFALSE if no such picture. Picture object is deleted on heap. */
   Bool_t RemovePicture(const char *name);

   /** @brief Puts a new TCanvas in corresponding folder.
    * @details Object is owned by folder afterwards. Returns false if object
    * of that name already exist. */
   Bool_t AddCanvas(TCanvas *can, const char *subfolder = nullptr);

   /** @brief Retrieves a TCanvas by name from the Canvases folder.
    * @details Returns nullptr if no such object. */
   TCanvas *GetCanvas(const char *name);

   /** @brief Removes TCanvas by name.
    * @details Returns kFALSE if no such canvas. TCanvas object is deleted on heap. */
   Bool_t RemoveCanvas(const char *name);

   /** @brief Create a copy of the analysis internal state.
    * @details To be sent to the Display. */
   TGo4AnalysisStatus *CreateStatus();

   /** @brief Create a copy of the analysis internal state.
    * @details To be sent to the Web interface. */
   TGo4AnalysisWebStatus *CreateWebStatus();

   /** @brief Find Object of name in the folder structure
    * and create a complete status object of it.
    * @details Used by GUI command to get updated information on certain object. */
   TGo4ObjectStatus *CreateObjectStatus(const char *name, const char *folder = nullptr);

   /** @brief Creates a list of names (keys) of all objects in analysis directories. */
   TGo4AnalysisObjectNames *CreateNamesList();

   /** @brief Access to top level go4 folder.
    * @details For histogram server. */
   TFolder *GetObjectFolder();

   /** @brief Create a copy of the analysis internal state.
    * @details To be sent to the Display. */
   void UpdateStatus(TGo4AnalysisStatus *state);

   /** @brief Set all analysis parameters to that of given
    * status object.
    * @details Analysis will be stopped,
    * all steps are closed (deleted). Then analysis
    * steps are created due to list of analysis step
    * status objects encapsulated in the analysis status
    * object. This method does _not_ initialize the
    * analysis due to the new parameters; this is left
    * to InitEventClasses. */
   void SetStatus(TGo4AnalysisStatus *state);

   /** @brief Load Analysis Status from file and set analysis
    * to these settings. */
   Bool_t LoadStatus(const char *filename = nullptr);

   /** @brief Load analysis objects from file.
    * @details File is the autosave file by
    * default, but may be specified by name. */
   Bool_t LoadObjects(const char *filename = nullptr);

   /** @brief Save current analysis settings into file of given name. */
   Bool_t SaveStatus(const char *filename = nullptr);

   /** @brief Enable or disable check of subsequent analysis step
    * for io consistency.
    * @details Default is on, disable for none-serial type analyses. */
   void SetStepChecking(Bool_t on = kTRUE);

   /** @brief Sets analysis step name as first one to be processed in
    * the chain of steps.
    * @details First step will read input event from its
    * event source. If name is nullptr, takes first step in steplist. */
   Bool_t SetFirstStep(const char *name);

   /** @brief Sets analysis step name as last one to be processed in
    * the chain of steps.
    * @details Last step will write output event into its
    * event store by default, its output event is accessible in the user
    * analysis. If name is nullptr, takes last step in steplist. */
   Bool_t SetLastStep(const char *name);

   /** @brief Enables analysis step of name to write its output event into
    * its event store.
    * @details By default only the last analysis step
    * will keep the output event in the store, the intermediate
    * steps will only pass the output to the subsequent step.
    * This method lets also any intermediate step write the
    * result to its store, which can later be used to process from
    * this point of the analysis chain. However, you may also
    * switch off the storage of the last analysis step output
    * if you only want to use the histograms produced by the
    * final user event function. */
   Bool_t SetStepStorage(const char *name, Bool_t on);

   /** @brief Shuts down the old analysis step storage and creates a new
    * one specified by parameter par.
    * @details To be used in online mode to switch output files. */
   Bool_t NewStepStore(const char *name, TGo4EventStoreParameter *par);

   /** @brief Shuts down the old analysis step event source and creates a new
    * one specified by parameter par.
    * @details To be used in online mode to switch input. */
   Bool_t NewStepSource(const char *name, TGo4EventSourceParameter *par);

   /** @brief Shuts down the old analysis step processor and creates a new
    * one specified by parameter par.
    * @details To be used in online mode to switch event processing. */
   Bool_t NewStepProcessor(const char *name, TGo4EventProcessorParameter *par);

   /** @brief Set time interval (in s) for autosave action. */
   void SetAutoSaveInterval(Int_t interval = 0) { fiAutoSaveInterval = interval; }

   /** @brief Set file for autosave action. */
   void SetAutoSaveFile(const char *filename = nullptr,
                        Bool_t overwrite = kFALSE,
                        Int_t compression = 5);

   /** @brief Return kTRUE, if autosave file name set. */
   Bool_t IsAutoSaveFileName() const;

   /** @brief Lock the autosave mutex. For usage in analysis client */
   Int_t LockAutoSave();

   /** @brief UnLock the autosave mutex. For usage in analysis client */
   Int_t UnLockAutoSave();

   /** @brief Enable/disable functionality of AutoSave method */
   void SetAutoSave(Bool_t on = kTRUE) { fbAutoSaveOn = on; }

   /** @brief True if autosaving is enabled */
   Bool_t IsAutoSaveOn() const { return fbAutoSaveOn; }

   /** @brief  Set analysis in mode where autosave file switches for each new input file*/
   void SetAutoSaveFileChange(Bool_t on = kTRUE) { fbAutoSaveFileChange = on; }

   /** @brief True if autosave file is changed according to the current input file
     * of lmd file list. */
   Bool_t IsAutoSaveFileChange() const { return fbAutoSaveFileChange; }

   /** @brief True if the analysis steps are initialized */
   Bool_t IsInitDone() const { return fbInitIsDone; }

   /** @brief True if the analysis as controlled by slave process is running.
     * @details If no slave task (analysis client) is available, always true
     * (plain batch mode never stops)
     * Test: read by reference than copy value, faster?*/
   Bool_t IsRunning() const;

   /** @brief Switch slave process into running state.
    * @details To be used from analysis macro to start/stop the go4 loop on certain
    * conditions. */
   void SetRunning(Bool_t on = kTRUE);

   /** @brief Poll on the IsRunning state with sleep delay, returns number
    * of wait cycles.
    * @details For slave server controlled cint macros to
    * start/stop execution of explicit user loop. */
   Int_t WaitForStart();

   /** @brief True if current analysis step allows stop on error */
   Int_t IsErrorStopEnabled() const;

   /** @brief Set flag that input file has changed */
   void SetNewInputFile(Bool_t on = kTRUE) { fbNewInputFile = on; }

   /** @brief True if input filelist has changed to a new lmd file. */
   Bool_t IsNewInputFile() const { return fbNewInputFile; }

   /** @brief Set current input file name */
   void SetInputFileName(const char *fname) { fxCurrentInputFileName = fname; }

   /** @brief Returns current input file name */
   const char *GetInputFileName() const { return fxCurrentInputFileName.Data(); }

   /** @brief Returns default lmd test file name (in GO4SYS installation directory) */
   const char *GetDefaultTestFileName() const { return fxDefaultTestFileName.Data(); }

   /** @brief Set current dynamic list interval */
   void SetDynListInterval(Int_t val);

   /** @brief Returns current dynamic list interval */
   Int_t GetDynListInterval() const;

   /** @brief Returns the input event structure of analysis step.
    * @details Argument indicates the step by name.
    * For internal usage in the user defined event function
    * UserEventFunc, and is used by analysis steps to get result of previous steps. */
   TGo4EventElement *GetInputEvent(const char *stepname) const;

   /** @brief Returns the input event structure of analysis step.
    * @details Argument number indicates
    * the step from 0 (first) to n (last) in subsequent order.
    * For internal usage in the user defined event function
    * UserEventFunc, and is used by analysis steps to access event sources of previous steps. */
   TGo4EventElement *GetInputEvent(Int_t stepindex) const;

   /** @brief Returns the output event (detector) structure of analysis step.
    * @details Argument indicates the step by name.
    * For internal usage in the user defined event function
    * UserEventFunc, and is used by analysis steps to get result of previous steps. */
   TGo4EventElement *GetOutputEvent(const char *stepname) const;

   /** @brief Returns the output event (detector) structure of analysis step.
    * @details Argument number indicates
    * the step from 0 (first) to n (last) in subsequent
    * order. Default is the output event of the
    * currently processed step, which is set by SetEvent method.
    * For internal usage in the user defined event function
    * UserEventFunc, and is used by analysis steps to get result of previous steps. */
   TGo4EventElement *GetOutputEvent(Int_t stepindex) const;

   /** @brief Returns the output event (detector) structure of analysis step.
    * @details Is set by SetOutputEvent method.
    * For internal usage in the user defined event function
    * UserEventFunc, and is used by analysis steps to get result of previous steps. */
   TGo4EventElement *GetOutputEvent() const;

   /** @brief Search reference to event structure in  folder. */
   TGo4EventElement *GetEventStructure(const char *name) const;

   /** @brief Display a user message.
    * @param prio defines message priority:
    *  - <0: display text as info on local terminal and in logfile anyway, independent of ignorelevel
    *  - 0: local terminal only; if debug level is enabled (TGo4Log::SetIgnorelevel(0))
    *  - 1: local terminal and info message in gui log panel (if gui mode)
    *  - 2: local terminal and warning message in gui log panel (if gui mode)
    *  - 3: local terminal and error message in gui log panel (if gui mode)
    * @param text is printed text with optional text formating arguments */
   void Message(Int_t prio, const char *text, ...)
   #if defined(__GNUC__) && !defined(__CLING__)
   __attribute__((format(printf, 3, 4)))
   #endif
   ;

   /** @brief User May send any named object via data channel to the gui.
    * @details In batch mode, this method is disabled. */
   void SendObjectToGUI(TObject *ob);

   /** @brief Start the object (histogram) server.
    * @details Existing server will be stopped and replaced
    * by new server with different basename and password. */
   void StartObjectServer(const char *basename, const char *passwd);

   /** @brief Stop the object (histogram) server. */
   void StopObjectServer();

   /** @brief Specify password for administrator account.
    * @details For login from remote gui into analysis server*/
   void SetAdministratorPassword(const char *passwd);

   /** @brief Specify password for controller account.
    * @details For login from remote gui into analysis server*/
   void SetControllerPassword(const char *passwd);

   /** @brief Specify password for observer account.
    * @details For login from remote gui into analysis server*/
   void SetObserverPassword(const char *passwd);

   /** @brief Printout the analysis (step-) setup. */
   void Print(Option_t *opt = "") const override;

   /** @brief Printout of all histogram statistic counters on the terminal. */
   void PrintHistograms(const char *expression = nullptr);

   /** @brief Print all condition counters to the terminal. */
   void PrintConditions(const char *expression = nullptr);

   /** @brief Printout of all parameters on the terminal. */
   void PrintParameters(const char *expression = nullptr);

   /** @brief Print entries of current dynamic list. */
   void PrintDynamicList();

   /** @brief Printout event content of single current event to analysis terminal
    * (using ROOT TTree::Show method).
    * @details Event is searched by name of analysis step. If isoutput true, then
    * output event of that step is used, otherwise input event.
    * If analysis step not found, event of stepname is searched in registered folders. */
   void ShowEvent(const char *stepname, Bool_t isoutput = kTRUE);

   /** @brief Write object obj into eventstore of the step specified by name.
    * @details Current event number will be added to object name in the file.
    * Returns -1 in case of not active eventstore. Returns +1 in case
    * of unknown step. */
   Int_t StoreParameter(const char *name, TGo4Parameter *par);

   /** @brief Write condition con into eventstore of the step specified by name.
    * @details Current event number will be added to parameter name in the file.
    * Returns -1 in case of not active eventstore. Returns +1 in case
    * of unknown step. */
   Int_t StoreCondition(const char *name, TGo4Condition *con);

   /** @brief Write fitter fit into eventstore of the step specified by name.
    * @details Current event number will be added to parameter name in the file.
    * Returns -1 in case of not active eventstore. Returns +1 in case
    * of unknown step. */
   Int_t StoreFitter(const char *name, TGo4Fitter *fit);

   /** @brief Write folder into eventstore of the step specified by name.
    * @details Current event number will be added to parameter name in the file.
    * Returns -1 in case of not active eventstore. Returns +1 in case
    * of unknown step. */
   Int_t StoreFolder(const char *stepname, TFolder *folder);

   /** @brief Write registered folder of foldername to the step store of stepname.
    * @details Returns -1 in case of not active eventstore. Returns +1 in case
    * of unknown step, +2 in case of unknown folder*/
   Int_t StoreFolder(const char *stepname, const char *foldername);

   /** @brief Perform autosave of all active stores, save existing calibration objects,
    * write known objects into user file. */
   void AutoSave();

   /** @brief Method for user analysis constructor to setup the list
    * of analysis steps.
    * @details Each call of this method will add a new
    * analysis step at the end of the list. Caution: user has to take
    * care that output and input event types of subsequent steps
    * are matching! Analysis steps cannot be removed, since the
    * steplist is not intended to be dynamic. This is a pure creation method.
    * Analysis steps are owned by the steplist once they have been added. */
   Bool_t AddAnalysisStep(TGo4AnalysisStep *next);

   /** @brief Access to certain analysis step by name. If name == nullptr,
    * this will return the first active step. */
   TGo4AnalysisStep *GetAnalysisStep(const char *name);

   /** @brief Access to certain analysis step by number. */
   TGo4AnalysisStep *GetAnalysisStepNum(Int_t number);

   /** @brief Define passwords, which should be used for analysis access when analysis runs in server mode */
   void DefineServerPasswords(const char *admin, const char *controller, const char *observer);

   /** @brief Set number of event loop count, executed in batch mode */
   void SetBatchLoopCount(Int_t cnt = -1) { fBatchLoopCount = cnt; }

   /** @brief Set flag to use data from autosave file in Make... methods (default true) */
   void SetMakeWithAutosave(Bool_t on = kTRUE) { fbMakeWithAutosave = on; }

   /** @brief Returns kTRUE if object in last Make... call was created,
     * kFALSE when object was retrieved from autosavefile */
   Bool_t IsObjMade() const { return fbObjMade; }

   /** @brief Create one dimensional histogram of specified type.
    * @details If histogram exactly with same name and type already exists in  autosave file,
    * it will be return. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
    * @param type can be:
    * - 'I', 'i' for TH1I - Int_t as bin content (default)
    * - 'F', 'f' for TH1F - Float_t as bin content
    * - 'D', 'd' for TH1D - Double_t as bin content
    * - 'S', 's' for TH1S - Short_t as bin content
    * - 'C', 'c' for TH1C - Char_t as bin content
    * - 'L', 'l' for TH1L - Long64_t as bin content
    * @param fullname specifies name of histogram (optionally with subfolder name)
    * @param title - histogram title
    * @param nbinsx - number of bins for X axis
    * @param xlow - minium for X axis
    * @param xup - maximum for X axis
    * @param xtitle - title for X axis of histogram
    * @param ytitle - title for Y axis of histogram */
   TH1 *MakeTH1(char type, const char *fullname, const char *title,
                Int_t nbinsx, Double_t xlow, Double_t xup,
                const char *xtitle = nullptr, const char *ytitle = nullptr);

   /** @brief Create two dimensional histogram of specified type.
    * @details If histogram exactly with same name and type already exists in  autosave file,
    * it will be return. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
    * @param type can be:
    * - 'I', 'i' for TH2I - Int_t as bin content (default)
    * - 'F', 'f' for TH2F - Float_t as bin content
    * - 'D', 'd' for TH2D - Double_t as bin content
    * - 'S', 's' for TH2S - Short_t as bin content
    * - 'C', 'c' for TH1C - Char_t as bin content
    * - 'L', 'l' for TH2L - Long64_t as bin content
    * @param fullname specifies name of histogram (optionally with subfolder name)
    * @param title - histogram title
    * @param nbinsx - number of bins for X axis
    * @param xlow - minium for X axis
    * @param xup - maximum for X axis
    * @param nbinsy - number of bins for X axis
    * @param ylow - minium for X axis
    * @param yup - maximum for X axis
    * @param xtitle - title for X axis of histogram
    * @param ytitle - title for Y axis of histogram
    * @param ztitle - title for Z axis of histogram */
   TH2 *MakeTH2(char type, const char *fullname, const char *title,
                Int_t nbinsx, Double_t xlow, Double_t xup,
                Int_t nbinsy, Double_t ylow, Double_t yup,
                const char *xtitle = nullptr, const char *ytitle = nullptr, const char *ztitle = nullptr);

   /** @brief Create 1D window condition.
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param xmin - X min condition range
    * @param xmax - X max condition range
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4WinCond *MakeWinCond(const char *fullname,
                            Double_t xmin, Double_t xmax,
                            const char *HistoName = nullptr);

   /** @brief Create 2D window condition.
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param xmin - X min condition range
    * @param xmax - X max condition range
    * @param ymin - Y min condition range
    * @param ymax - Y max condition range
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4WinCond *MakeWinCond(const char *fullname,
                            Double_t xmin, Double_t xmax,
                            Double_t ymin, Double_t ymax,
                            const char *HistoName = nullptr);

   /** @brief Create polygon condition.
    * @details To use method, array should be declared as following:
    * ~~~{cpp}
    * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
    * auto cond = MakePolyCond("Folder/CondName", 4, points);
    * ~~~
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param npoints - number of points in polygon condition
    * @param points - (X,Y) points
    * @param HistoName - name of histogram, to which condition is assigned
    * @param shapedcond - create a freestyle TGo4ShapedCond */
   TGo4PolyCond *MakePolyCond(const char *fullname,
                              Int_t npoints,
                              Double_t (*points) [2],
                              const char *HistoName = nullptr,
                              Bool_t shapedcond = kFALSE);

   /** @brief Create ellipse shaped polygon condition.
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param npoints - number of points in ellipse condition, 0 for default resolution
    * @param cx - center X coordinate of ellipse
    * @param cy - center Y coordinate of ellipse
    * @param a1 - width of ellipse X half axes
    * @param a2 - width of ellipse Y half axes
    * @param theta - ellipse tilt angle
    * @param HistoName - name of histogram, to which condition is assigned  */
   TGo4ShapedCond *MakeEllipseCond(const char *fullname,
                                   Int_t npoints,
                                   Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta = 0.,
                                   const char *HistoName = nullptr);

   /** @brief Create circular shaped polygon condition.
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param npoints - number of points in shaped condition, 0 for default resolution
    * @param cx - circle center X coordinate
    * @param cy - circle center Y coordinate
    * @param r  - circle radius
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4ShapedCond *MakeCircleCond(const char *fullname,
                                  Int_t npoints, Double_t cx, Double_t cy, Double_t r,
                                  const char *HistoName = nullptr);

   /** @brief Create tilted rectangular box shaped polygon condition.
    *  @param fullname specifies name of condition (optionally with subfolder name)
    *  @param cx - center X coordinates of box
    *  @param cy - center Y coordinates of box
    *  @param a1 - width of box half axes
    *  @param a2 - height of box half axes
    *  @param theta - tilt angle
    *  @param HistoName - name of histogram, to which condition is assigned */
   TGo4ShapedCond *MakeBoxCond(const char *fullname, Double_t cx, Double_t cy,
                               Double_t a1, Double_t a2, Double_t theta,
                               const char *HistoName = nullptr);

   /** @brief Create free shaped (polygon) condition.
    * @details In contrast to plain TGo4PolyCond, this one can be converted later
    * to different dedicated shape types (ellipse, box, etc.)
    * To use method, array should be declared as following:
    * ~~~{cpp}
    * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
    * auto cond = MakePolyCond("Folder/CondName", 4, points);
    * ~~~
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param npoints - number of points in polygon condition
    * @param points - (X,Y) points
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4ShapedCond *MakeFreeShapeCond(const char *fullname, Int_t npoints,
                                     Double_t (*points)[2], const char *HistoName = nullptr);

   /** @brief Create "whitlelist" condition with separate values to test against
    * condition is true if any of the values matches
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param num - number of values in array
    * @param values - 1d array with values
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4ListCond *MakeListCond(const char *fullname, const Int_t num,
                              const Int_t *values, const char *HistoName = nullptr);

   /** @brief Create "whitlelist" condition with separate values to test against
    * condition is true if any of the values matches
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param start - first value in list
    * @param stop - last value in list
    * @param step - distance between list entries
    * @param HistoName - name of histogram, to which condition is assigned */
   TGo4ListCond *MakeListCond(const char *fullname, const Int_t start,
                              const Int_t stop, const Int_t step = 1, const char *HistoName = nullptr);

   /** @brief Create "whitlelist" condition with separate values to tes against
    * condition is true if any of the values matches
    * @details This method creates empty list condition of specified name and title to be set by the user,
    * or already defined condition from previous autosave*/
   TGo4ListCond *MakeListCond(const char *fullname, const char *title,
                              const char *HistoName = nullptr);

   /** @brief Create a TGraph with initial values as specified by points, xvalues and yvalues.
    * @details If theses parameters are missing, an empty graph is created to be specified by the user.
    * If a graph of this name already exists in the autosave file,
    * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
   TGraph *MakeGraph(const char *fullname, const char *title, Int_t points = 0,
                     Double_t *xvalues = nullptr, Double_t *yvalues = nullptr);

   /** @brief Create a TGraph with values initialized by a function object TF1l.
    * @details If a graph of this name already exists in the autosave file,
    * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
   TGraph *MakeGraph(const char *fullname, const char *title, TF1 *function);

   /** @brief Create a go4 rolling graph (generic trending plot) with properties points and average.
    * @details If theses parameters are missing, an empty graph is created to be specified by the user.
    * If a rolling graph of this name already exists in the autosave file,
    * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
   TGo4RollingGraph *MakeRollingGraph(const char *fullname, const char *title, Int_t points = 0, Int_t average = 1);

   /** @brief Create parameter of specified class,
    * @param fullname specifies name of condition (optionally with subfolder name)
    * @param classname - name of required parameter class, it should be known to ROOT.
    * @param cmd - optional argument, can be used for two purposes:
    *   1. as new command to create parameter like "new UserParameter(%s, 1000, 2000)",
    *      where %s is place for parameter name.  Should be specified, if parameter
    *      constructor contains more parameters as only parameter name. Such argument
    *      should be always started with 'new ' command.
    *   2. macro name to set parameter value. Macro executed immediately after parameter
    *      creation (or loading from auto-save file) and thus overwrites parameters value.
    *      Macro name should be always starting with "set_" */
   TGo4Parameter *MakeParameter(const char *fullname,
                                 const char *classname,
                                 const char *cmd = nullptr);

   /** @brief Configure sorting order for newly created sub-folders */
   void SetSortedOrder(Bool_t on = kTRUE);

   /** @brief Returns true if sub-folders will be created in sorted order */
   Bool_t IsSortedOrder() const;

   /** @brief Executes ROOT script.
    * @returns -1 when script was not found, or result of script execution */
   Long64_t ExecuteScript(const char *script_name);

   /** @brief Executes Python script in ROOT interpreter.
    * @details Will bind TGo4Analysis object to python go4 Symbol
    * Errcode may be used to check ROOT interpreter error code.*/
   Long64_t ExecutePython(const char *script_name, Int_t *errcode = nullptr);

   /** @brief Process ROOT command line.
    * @details Optionally provide pyroot binding:
    *  a leading '$' will try to load and execute python script.
    * Errcode may be used to check ROOT interpreter error code.
    * Return value is result of command execution*/
   Long64_t ExecuteLine(const char *command, Int_t *errcode = nullptr);

   /** @brief Method called from Ctrl-C handler */
   void ProcessCrtlCSignal();

   /** @brief Set pointer on sniffer object */
   void SetSniffer(TGo4AnalysisSniffer *sniff) { fSniffer = sniff; }

   /** @brief Returns pointer on current sniffer */
   TGo4AnalysisSniffer *GetSniffer() const { return fSniffer; }

   /** @brief Returns pointer on analysis client */
   TGo4AnalysisClient *GetAnalysisClient() const { return fxAnalysisSlave; }

   /** @brief Stop analysis event loop, works in both batch and gui-controlled mode */
   void StopAnalysis();

   /** @brief (Re)Start analysis event loop, works in both batch and gui-controlled mode */
   void StartAnalysis();

   /** @brief Method should be called to process extra events in the analysis thread context */
   void ProcessEvents();

protected:

   /** True if analysis framework has been initialized and
    * the event classes are created. */
   Bool_t fbInitIsDone{kFALSE};

   /** True if Autosaving is enabled (default).
    * If false, calling AutoSave() will have no effect. */
   Bool_t fbAutoSaveOn{kFALSE};

   /** @brief Protected constructor for singleton instance. */
   TGo4Analysis(const char *name = nullptr);

   /** @brief Another variant for analysis constructor. */
   TGo4Analysis(int argc, char **argv);

   /** @brief Method to create all necessary object in the real constructor */
   void Constructor();

   /** @brief Process steps
    * @details Uses the chain of analysis steps to process the first input
    * event into an output event which then is accessible at
    * the output event member. Returns number of
    * steps run. For internal usage in the MainCycle and
    * GetEvent methods; may also be called from UserEventFunc directly. */
   Int_t ProcessAnalysisSteps();

private:

   /** @brief Helper function to figure out object name and folder path from full path.
    * @details Returns false if something is wrong with given fullname*/
   Bool_t EvaluateFolderpath(const char *fullname, TString &object, TString &folder);

   /** @brief Create a tree that is filled with one single event sample.
    * @details Event is taken from analysis step of name. If isoutput
    * is kTRUE, the output event is used, otherwise the input event.
    * If no analysis step of that name exists, the event is
    * searched by object name in the eventstructure folder
    * of the object manager. */
   TTree *CreateSingleEventTree(const char *name, Bool_t isoutput = kTRUE);

   /** @brief Create a tree that is filled with one single event sample. To be sent to the gui. */
   TTree *CreateSingleEventTree(TGo4EventElement *event);

   /** @brief Update the internal list of object names. To be used internally and by analysis client. */
   void UpdateNamesList();

   /** @brief Returns the nameslist of the go4 object directory structure.
    * @details Is used by analysis client to send the current object names to the gui. */
   TGo4AnalysisObjectNames *GetNamesList() const { return fxObjectNames; }

   /** @brief Sets the current output event (detector) structure.
    * @details For internal usage by the chain of analysis steps, since
    * TGo4AnalysisStep is friend. */
   void SetOutputEvent(TGo4EventElement *event);

   /** @brief Add reference to event store object to Go4 Folder structure.
    * @warning To be used by framework only! */
   Bool_t AddEventStore(TGo4EventStore *store);

   /** @brief Remove reference to event store from go4 folder structure. */
   Bool_t RemoveEventStore(TGo4EventStore *store);

   /** @brief Add reference to event source object to Go4 Folder structure.
    * @warning To be used by framework only!  */
   Bool_t AddEventSource(TGo4EventSource *source);

   /** @brief Remove reference to event source from go4 folder structure. */
   Bool_t RemoveEventSource(TGo4EventSource *source);

   /** @brief Add reference to event processor object to Go4 Folder structure.
    * @warning To be used by framework only! */
   Bool_t AddEventProcessor(TGo4EventProcessor *pro);

   /** @brief Remove reference to event processor from go4 folder structure. */
   Bool_t RemoveEventProcessor(TGo4EventProcessor *pro);

   /** @brief Add reference to event structure object to Go4 Folder structure.
    * @warning To be used by framework only! */
   Bool_t AddEventStructure(TGo4EventElement *ev);

   /** @brief Remove reference to event structure from go4 folder structure. */
   Bool_t RemoveEventStructure(TGo4EventElement *ev);

   /** @brief Register analysis client if running in gui mode. */
   void SetAnalysisClient(TGo4AnalysisClient *cli) { fxAnalysisSlave = cli; }

   /** @brief Send message string in a status object to the gui.
    * @details Will be displayed in status window. Level indicates info (1), warning (2) or error(3),
    * if printout is true, message will also be printed on terminal.
    * In non-GUI mode, this method is equivalent to TGo4Log::Message */
   void SendMessageToGUI(Int_t level, Bool_t printout, const char *text);

   /** @brief Open the autosave file with the current settings. */
   void OpenAutoSaveFile(bool for_writing = false);

   /** @brief Close the autosave file if existing. */
   void CloseAutoSaveFile();

   /** @brief Called by interrupt routine for normal exit from program (in batch mode) */
   void StopWorking() { fxDoWorkingFlag = flagStop; }

   /** @brief Returns true when event loop execution was stopped, only in batch mode */
   Bool_t IsStopWorking() const { return fxDoWorkingFlag == flagStop; }

   /** @brief for signal handler to shutdown analysis server, if existing*/
   void ShutdownServer();

   /** Static Pointer to the analysis singleton instance. */
   static TGo4Analysis *fxInstance;             //!

   /** This is used to check from outside if an analysis is
    * already there. Useful if one wants to prevent creating
    * a complete analysis instance in the gui task... */
   static Bool_t fbExists; //!

   /** This is used to identify running mode of analysis:
    * 0 - batch, 1 - client, 2 - server */
   static Int_t fiRunningMode; //!

   /** Optional backpointer to analysis client if running in gui mode.*/
   TGo4AnalysisClient *fxAnalysisSlave{nullptr};            //!

   /** @link aggregationByValue
    *   @supplierCardinality 1 */
   TGo4AnalysisStepManager *fxStepManager{nullptr};      //!

   /** @link aggregationByValue
    *   @supplierCardinality 1 */
   TGo4AnalysisObjectManager *fxObjectManager{nullptr};  //!

   /** Event counter for autosave mechanism. */
   Int_t fiAutoSaveCount{0};                       //!

   /** Clock for autosave by time mechanism. */
   TStopwatch *fxAutoSaveClock{nullptr};                //!

   /** Implicit Loop cycles in between two autosave actions. Autosave will write
    * calibration objects of steps into event file and will save all histograms into an default
    * root file. */
   Int_t fiAutoSaveInterval{0};                    //!

   /** compression level for autosave file */
   Int_t fiAutoSaveCompression{0};                 //!

   /** Mutex protecting the autosave file access. On Termination, both the
    * main thread and  the appcontrol timer might try to autosave or delete on
    * the same file. */
   TMutex *fxAutoSaveMutex{nullptr};                     //!

   /** Optional file for autosave of the important objects. */
   TFile *fxAutoFile{nullptr};                           //!

   /** Name of the autosave file. */
   TString fxAutoFileName;                      //!

   /** True if autosave file is in overwrite mode */
   Bool_t fbAutoSaveOverwrite{kFALSE};                  //!

   /** name of configuration file used for this setup */
   TString fxConfigFilename;                    //!

   /** True if input file of multiple file input has changed.
    *  User analysis may check this and read mbs buffer infos then. */
   Bool_t fbNewInputFile{kFALSE};                       //!

   /** Contains current input file name */
   TString fxCurrentInputFileName;               //!

   /** True if autosave name shall be changed when input file of multiple file input has changed.
    *  To be set in user analysis; will be evaluated in TGo4MbsFile */
   Bool_t fbAutoSaveFileChange{kFALSE};                 //!

   /** this holds event sample for the SingleEventTree. Must be clone
    * of the event in use, since TTree::Show() will modify it! */
   TGo4EventElement *fxSampleEvent{nullptr};             //!

   /** @link aggregation
    *   @supplierCardinality 1 */
   TGo4AnalysisObjectNames *fxObjectNames{nullptr};     //!

   /** Indicates analysis runstate */
   enum { flagInit, flagRunning, flagPause, flagClosed, flagStop  } fxDoWorkingFlag; //!

   /** use to treat Ctrl-C interrupts */
   TGo4InterruptHandler *fxInterruptHandler{nullptr}; //!

   /** name of analysis object */
   TString fAnalysisName;

   /** exact number of loop counts in batch mode */
   Int_t fBatchLoopCount{0};

   /** pre-configured password for administrator access */
   TString fServerAdminPass; //!

   /** pre-configured password for controller access */
   TString fServerCtrlPass; //!

   /** pre-configured password for observer access */
   TString fServerObserverPass; //!

   /** If false, do not use data from autosave file in Make... methods */
   Bool_t fbMakeWithAutosave{kFALSE}; //!

   /** indicate if object was created by last Make... operation */
   Bool_t fbObjMade{kFALSE}; //!

   /** If true, pyroot has already bound TGo4Analysis object. For lazy initialization at first call of .py macro*/
   Bool_t fbPythonBound{kFALSE}; //!

   /** number Ctrl-C handler called */
   Int_t fNumCtrlC{0}; //!

   /** Sniffer object */
   TGo4AnalysisSniffer *fSniffer{nullptr}; //!

   /** optional ratemeter object */
   TGo4Ratemeter *fxRate{nullptr}; //!

   /** default file name for distributed test lmd file*/
   TString fxDefaultTestFileName; //!

   ClassDefOverride(TGo4Analysis,5)
};

#endif
