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

#ifndef TGO4ANALYSISIMP_H
#define TGO4ANALYSISIMP_H

#include "TGo4CommandReceiver.h"
#include "TObject.h"
#include "TString.h"

class TH1;
class TH2;
class TTree;
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
 * The mother of all go4 analysis. Provides services to organize
 * events, histogram data, and everything. May run standalone in batch mode,
 * or may be aggregated by analysis client for threaded online control. User
 * has to derive his/her own analysis from this base class and override the
 * virtual user event function and set event factory methods.
 * @author J. Adamczewski
 * @since 19-jan-2001
 */

class TGo4Analysis : public TObject, public TGo4CommandReceiver  {

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
    static const char* fgcDEFAULTFILENAME;    //!

    /** standard file name for analysis settings */
    static const char* fgcDEFAULTSTATUSFILENAME; //!

    /** filename suffix for object file */
    static const char* fgcDEFAULTFILESUF;     //!

    /** Default name of the default (toplevel) dynamic list */
    static const char* fgcTOPDYNAMICLIST;     //!

    static TGo4Analysis* Instance();

    static Bool_t Exists();

    /** Return kTRUE if analysis will run in batch (without connection to GUI) */
    static Bool_t IsBatchMode();

    /** Return kTRUE if analysis will run in gui mode, connected to gui as client */
    static Bool_t IsClientMode();

    /** Return kTRUE if analysis will run in server mode and gui can connect to the analysis */
    static Bool_t IsServerMode();

    static void SetRunningMode(int mode);

    virtual ~TGo4Analysis();

    virtual const char* GetName() const { return fAnalysisName.Data(); }

    void SetAnalysisName(const char* name) { fAnalysisName = name; }

    /** The main analysis event cycle. This method is called by the
      * main runnable, or by RunImplicitLoop(). Processes the analysis steps
      * first, then the user event function. Finally, the dynamic list is updated. */
    Int_t MainCycle();

    /** Method executed once before the main analysis event loop. This method is called by the thread prerun, or at the beginning of
      * RunImplicitLoop(). Calls virtual user preloop function. */
    Int_t PreLoop();

    /** Method executed once after the main analysis event loop. This method is called by the thread postrun, or at the end of
      * RunImplicitLoop(). Calls virtual user postloop function. */
    Int_t PostLoop();

    /** This method is meant to be called from a go4 cintserver macro
      * in an explicit event loop. It executes any queued commands and calls MainCycle() once, protected
      * by main go4 lockguard. Returns 0 if analysis has IsRunning() state true;
      * returns negative value if IsRunning() is false. */
    Int_t Process();

    /** User defined function which processes the actual analysis.
      * May be called explicitly from analysis client thread, or may run
      * in an implicit loop provided by method RunImplicit, if
      * analysis works in a non threaded standalone mode. To be overridden
      * in the user analysis class. */
    virtual Int_t UserEventFunc();

    /** User defined function called once before processing the main
      * event loop. Optionally. */
    virtual Int_t UserPreLoop();

    /** User defined function called once after processing the main
      * event loop. Optionally. */
    virtual Int_t UserPostLoop();

    /** Processes the UserEventFunc in an implicit loop for "times" times.
      * For non-threaded usage of the analysis class.
      * \param times specified how many events should be processed, 0 - all
      * \param showrate identifies if ratemeter should be shown
      * \param process_event_interval allows regularly process ROOT system events
      * \param iswebserver specifies if loop shall be suspended at end of event source, or returns from batch  */
    Int_t RunImplicitLoop(Int_t times, Bool_t showrate = kFALSE, Double_t process_event_interval = -1., Bool_t iswebserver=kFALSE);

    /** Finish the analysis run and close all event sources/storages.
      * The analysis instance and the setup of the analysis steps is not deleted. */
    virtual void CloseAnalysis();

    /** Initialization of the event class plugins which are delivered from
      * the user defined event factory. The virtual standard base class method may be changed
      * by overriding; normally, this is not necessary. */
    virtual Bool_t InitEventClasses();

    /** Returns pointer on analysis object manager */
    TGo4AnalysisObjectManager* ObjectManager() const { return fxObjectManager; }

    /** Add (create) new dynamic histogram entry which connects an existing
      * histogram with existing condition and data.
      * Dynamic entry is specified by name. Histogram histo will be searched in
      * registered histograms folder, condition in conditions folder. If
      * condition is true or not existing (condition=0), histogram will be filled
      * from the values that are found in registered eventstructure classes of
      * names  evx, evy, evz at the data members of names memx, memy, memz,
      * for the three coordinate axes, respectively. Histogram dimension is
      * checked against given values. */
    Bool_t AddDynamicHistogram(const char* name,
                               const char* histo,
                               const char* hevx, const char* hmemx,
                               const char* hevy=0, const char* hmemy=0,
                               const char* hevz=0, const char* hmemz=0,
                               const char* condition=0,
                               const char* cevx=0, const char* cmemx=0,
                               const char* cevy=0, const char* cmemy=0);

    /** Set dynamic entry of name "name" to the values specified by external
      * dynamic entry status "state". If no dynamic entry of that name exists, a
      * new entry is created with the properties of the state, and
      * added to the current dynamic list. */
    Bool_t AddDynamicEntry(TGo4DynamicEntry* entry);

    /** Remove entry of that name from dynamic list of listname.
      * The referred objects (histograms, conditions..) are still on heap, since
      * they are owned by their special folders. */
    Bool_t RemoveDynamicEntry(const char* entryname, const char* listname=0);

    /** Add any external object to the user object folder.
      * Object is owned by go4 afterwards and will be saved automatically.
      * Object is accessible by name from the go4 display and from the
      * analysis itself. Subfolder of UserObjects may be specified.
      * If replace is true, old object of same name will be deleted and
      * replaced by the added one. */
    Bool_t AddObject(TNamed * anything, const char* subfolder=0, Bool_t replace=kTRUE);

    /** Searches for object by name in all directories. Returns
      * pointer to object. If
      * object of that name does not exist, null is returned.
      * Optionally, folder may be specified to search in. */
    TNamed * GetObject(const char* name, const char* folder=0);

    /** Removes object from user object folder by name. Returns kFALSE if no
      * such histogram. Object is deleted on heap only if del is true.
      * Otherwise, user owns the object afterwards. */
    Bool_t RemoveObject(const char* name, Bool_t del=kTRUE);

    /** Delete object of name, or all objects in folder name, respectively.
      * Objects are only deleted if delete protection is false. Usually,
      * Objects registered from user code are delete protected by default.
      * Objects created dynamically from gui are not delete protected. */
    Bool_t DeleteObjects(const char * name);

    /** Clear (reset) the specified objects. Method will first
      * clear all objects in any folder if it is matching the namestring.
      * If no such folder exists, object of name is searched and cleared.
      * Returns kFALSE in case of not found objects. */
    Bool_t ClearObjects(const char* name);

    /** Change protection properties of object name as specified.
      * If object is a folder, change properties of all objects in this folder recursively.
      * Flags may contain key letters like:
      * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
      * "+D"/"-D" to enable/disable protection against object deletion
      * For example flags="+C-D", "+C+D", "-D-C", "-C"
      * Properties not appearing in flags are not changed.  */
    Bool_t ProtectObjects(const char* name, const Option_t* flags);

    /** Clear previous events of backstore tree. For reset of dynamic tree histogram
     * If called with clearflag=true, do not reset tree, but only clear
     * the backstore reset bit (done after processing of complete dynamic list)*/
    Bool_t ResetBackStores(Bool_t clearflag=kFALSE);

    /** Delivers pointer to next object of the Go4 folder structure
      * with a name matching the expression expr. If reset is true,
      * The list of matching objects will be created anew by comparing
      * all names with expr. If reset is false, the next object of a
      * previously created matching list is returned. Optionally the
      * search can be limited to a given folder. */
    TObject* NextMatchingObject(const char* expr=0,
                                const char* folder=0,
                                Bool_t reset=kFALSE);

    /** Create a tree structure for a certain tree by name */
    TGo4TreeStructure* CreateTreeStructure(const char* treename);

    /** Add external histogram to go4 histogram directory. Histogram will be owned
      * and streamed by go4 histogram list afterwards. */
    Bool_t AddHistogram(TH1* his , const char* subfolder=0, Bool_t replace=kTRUE);

    /** Add reference to a tree in the go4 folder structure.
      * If name of subfolder is given, tree will be assigned
      * to subfolder (e.g. subfolder with analysis step name).
      * If subfolder name is zero,
      * the tree will be put into general tree folder. */
    Bool_t AddTree(TTree* tree, const char* subfolder=0);

    /** Remove reference to a tree in the go4 folder structure.
      * If name of analysis step is given, tree will be assigned
      * to folder dedicated to that step. In stepname is zero,
      * the tree will be put into general tree folder. */
    Bool_t RemoveTree(TTree * tree, const char* stepname=0);

    /** Add Histogram into the dynamic list wich is linked to a tree.
      * If Histogram of hisname already exists, this histogram will taken.
      * if not, the histogram will be created on first TTree::Draw.
      * Strings varexp and selection are used for applying cuts and variables to plot. */
    Bool_t AddTreeHistogram(const char* hisname, const char* treename, const char* varexp, const char* cutexp);

    /** Search histogram in histogram list (directory). */
    TH1* GetHistogram(const char* name);

    /** Search tree in tree folder. */
    TTree * GetTree(const char* name);

    /** Removes histogram from histogram dir by name. Returns kFALSE if no
      * such index. Histogram object is deleted on heap.
      * Dynamic list entry for that histogram object is also removed. */
    Bool_t RemoveHistogram(const char* name, Bool_t del=kTRUE);

    /** Puts a new analysis condition object in corresponding list.
      * Object is owned by list afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddAnalysisCondition(TGo4Condition * con, const char* subfolder=0);

    /** Set existing analysis condition of name to the values
      * of external condition object con. Used to set condition by command
      * from gui, or to update the precompiled condition from the values
      * that were saved to the autosave file. */
    Bool_t SetAnalysisCondition(const char* name, TGo4Condition* con, Bool_t counter=kTRUE);

    /** Retrieves an analysis condition  from list by name.
     * Optionally, condition could be checked if it inherits from specified class.
     * Returns 0 if no such condition found. */
    TGo4Condition * GetAnalysisCondition(const char* name, const char* cond_cl = 0);

    /** Removes analysis condition from list by name. Returns 0 if no
      * such condition. Condition object is deleted on heap. */
    Bool_t RemoveAnalysisCondition(const char* name);

    /** Puts a new parameter object in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddParameter(TGo4Parameter * par, const char* subfolder=0);

    /** Set existing parameter of name to the values
      * of external parameter object par. Used to set parameter by command
      * from gui, or to update the precompiled parameter from the values
      * that were saved to the autosave file. */
    Bool_t SetParameter(const char* name, TGo4Parameter * par);

    /** Set existing parameter of name to the values
      * of external parameter status object. Used to set parameter by command
      * from gui, or to update the precompiled parameter from the values
      * that were saved to the autosave file. */
    Bool_t SetParameterStatus(const char* name, TGo4ParameterStatus* par);

    /** Retrieves a parameter object by name from the object folder.
      * Optionally expected class of parameter object could be specified.
      * Returns 0 if no such parameter found (or class not match). */
    TGo4Parameter * GetParameter(const char* name, const char* parameter_class = 0);

    /** Removes parameter by name. Returns 0 if no
      * such parameter. Parameter object is deleted on heap. */
    Bool_t RemoveParameter(const char* name);

    /** Puts a new picture object in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddPicture(TGo4Picture * pic, const char* subfolder=0);

    /** Set existing picture of name to the values
      * of external picture object pic. Used to set picture by command
      * from gui, or to update the precompiled picture from the values
      * that were saved to the autosave file. */
    Bool_t SetPicture(const char* name, TGo4Picture * pic);

    /** Retrieves a picture object by name from the object folder.
      * Returns 0 if no such picture. */
    TGo4Picture * GetPicture(const char* name);

    /** Removes picture by name. Returns 0 if no
      * such picture. Picture object is deleted on heap. */
    Bool_t RemovePicture(const char * name);

    /** Puts a new TCanvas in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exist. */
    Bool_t AddCanvas(TCanvas * can, const char* subfolder=0);

    /** Retrieves a TCanvas by name from the Canvases folder.
      * Returns 0 if no such object. */
    TCanvas * GetCanvas(const char* name);

    /** Removes TCanvas by name. Returns 0 if no
      * such canvas. TCanvas object is deleted on heap. */
    Bool_t RemoveCanvas(const char * name);

    /** Create a copy of the analysis internal state.
      * To be sent to the Display. */
    TGo4AnalysisStatus* CreateStatus();

    /** Create a copy of the analysis internal state.
    * To be sent to the Web interface. */
    TGo4AnalysisWebStatus* CreateWebStatus();

    /** Find Object of name in the folder structure
      * and create a complete status object of it.
      * Used by GUI command to get updated information on certain object. */
    TGo4ObjectStatus* CreateObjectStatus(const char * name, const char* folder=0);

    /** Creates a list of names (keys) of all objects in analysis directories. */
    TGo4AnalysisObjectNames * CreateNamesList();

    /** Access to top level go4 folder. For histogram server. */
    TFolder * GetObjectFolder();

    /** Create a copy of the analysis internal state. To be sent to the Display. */
    void UpdateStatus(TGo4AnalysisStatus* state);

    /** Set all analysis parameters to that of given
      * status object. Analysis will be stopped,
      * all steps are closed (deleted). Then analysis
      * steps are created due to list of analysis step
      * status objects encapsulated in the analysis status
      * object. This method does _not_ initialize the
      * analysis due to the new parameters; this is left
      * to InitEventClasses. */
    void SetStatus(TGo4AnalysisStatus * state);

    /** Load Analysis Status from file and set analysis
      * to these settings. */
    Bool_t LoadStatus(const char* filename=0);

    /** Load analysis objects from file. File is the autosave file by
      * default, but may be specified by name. */
    Bool_t LoadObjects(const char* filename=0);

    /** Save current analysis settings into file of given name. */
    Bool_t SaveStatus(const char* filename=0);

    /** Enable or disable check of subsequent analysis step
      * for io consistency. Default is on, disable for non
      * serial type analyses. */
    void SetStepChecking(Bool_t on=kTRUE);

    /** Sets analysis step name as first one to be processed in
      * the chain of steps. First step will read input event from its
      * event source. If name is nullpointer, takes first step in steplist. */
    Bool_t SetFirstStep(const char* name);

    /** Sets analysis step name as last one to be processed in
      * the chain of steps. Last step will write output event into its
      * event store by default, its output event is accessible in the user
      * analysis. If name is nullpointer, takes last step in steplist. */
    Bool_t SetLastStep(const char* name);

    /** Enables analysis step of name to write its output event into
      * its event store. By default only the last analysis step
      * will keep the output event in the store, the intermediate
      * steps will only pass the output to the subsequent step.
      * This method lets also any intermediate step write the
      * result to its store, which can later be used to process from
      * this point of the analysis chain. However, you may also
      * switch off the storage of the last analysis step output
      * if you only want to use the histograms produced by the
      * final user event function. */
    Bool_t SetStepStorage(const char* name, Bool_t on);

    /** Shuts down the old analysis step storage and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch output files. */
    Bool_t NewStepStore(const char* name, TGo4EventStoreParameter* par);

    /** Shuts down the old analysis step event source and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch input. */
    Bool_t NewStepSource(const char* name, TGo4EventSourceParameter * par);

    /** Shuts down the old analysis step processor and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch event processing. */
    Bool_t NewStepProcessor(const char* name, TGo4EventProcessorParameter * par);

    /** Set time interval (in s) for autosave action. */
    void SetAutoSaveInterval(Int_t interval=0) { fiAutoSaveInterval=interval; }

    /** Set file for autosave action. */
    void SetAutoSaveFile(const char* filename=0,
                         Bool_t overwrite=kFALSE,
                         Int_t compression=5);

    /** Return kTRUE, if autosave file name set. */
    Bool_t IsAutoSaveFileName() const;

    /** Lock the autosave mutex. For usage in analysis client */
    Int_t LockAutoSave();

    /** UnLock the autosave mutex. For usage in analysis client */
    Int_t UnLockAutoSave();

    /**  enable/disable functionality of AutoSave method */
    void SetAutoSave(Bool_t on=kTRUE) { fbAutoSaveOn=on; }

    /** True if autosaving is enabled */
    Bool_t IsAutoSaveOn() const { return fbAutoSaveOn; }

    /**  Set analysis in mode where autosave file switches for each new input file*/
    void SetAutoSaveFileChange(Bool_t on=kTRUE) {fbAutoSaveFileChange=on;}

    /** True if autosave file is changed according to the current input file
      * of lmd file list. */
    Bool_t IsAutoSaveFileChange() const { return fbAutoSaveFileChange; }

    /** True if the analysis steps are initialized */
    Bool_t IsInitDone() const { return fbInitIsDone; }

    /** True if the analysis as controlled by slave process is running.
      * if no slave task (analysis client) is available, always true
      * (plain batch mode never stops)
      * Test: read by reference than copy value, faster?*/
    Bool_t IsRunning();

    /** Switch slave process into running state. To be used
     * from analysis macro to start/stop the go4 loop on certain
     * conditions. */
    void SetRunning(Bool_t on=kTRUE);

    /** Poll on the IsRunning state with sleep delay, returns number
      * of wait cycles. For slave server controlled cint macros to
      * start/stop execution of explicit user loop. */
    Int_t WaitForStart();

    /** True if current analysis step allows stop on error */
    Int_t IsErrorStopEnabled();

    /** Set flag that input file has changed */
    void SetNewInputFile(Bool_t on=kTRUE) { fbNewInputFile=on; }

    /** True if input filelist has changed to a new lmd file. */
    Bool_t IsNewInputFile() const { return fbNewInputFile; }

    /** Set current input file name */
    void SetInputFileName(const char* fname) { fxCurrentInputFileName = fname; }

    /** Returns current input file name */
    const char* GetInputFileName() const { return fxCurrentInputFileName.Data(); }

    void SetDynListInterval(Int_t val);

    Int_t GetDynListInterval();

    /** Returns the input event structure of analysis step. Argument indicates
      * the step by name. For internal usage in the user defined event function
      * UserEventFunc, and is used by analysis steps to get result of previous steps. */
    TGo4EventElement* GetInputEvent(const char* stepname);

    /** Returns the input event structure of analysis step. Argument number indicates
      * the step from 0 (first) to n (last) in subsequent order.
      * For internal usage in the user defined event function
      * UserEventFunc, and is used by analysis steps to access event sources of previous steps. */
    TGo4EventElement* GetInputEvent(Int_t stepindex);

    /** Returns the output event (detector) structure of analysis step. Argument indicates
      * the step by name.
      * For internal usage in the user defined event function
      * UserEventFunc, and is used by analysis steps to get result of previous steps. */
    TGo4EventElement* GetOutputEvent(const char* stepname);

    /** Returns the output event (detector) structure of analysis step. Argument number indicates
      * the step from 0 (first) to n (last) in subsequent
      * order. Default is the output event of the
      * currently processed step, which is set by SetEvent method.
      * For internal usage in the user defined event function
      * UserEventFunc, and is used by analysis steps to get result of previous steps. */
    TGo4EventElement* GetOutputEvent(Int_t stepindex);

    /** Returns the output event (detector) structure of analysis step.  Is set by
      * SetOutputEvent method. For internal usage in the user defined event function
      * UserEventFunc, and is used by analysis steps to get result of previous steps. */
    TGo4EventElement* GetOutputEvent();

    /** Search reference to event structure in  folder. */
    TGo4EventElement * GetEventStructure(const char * name);

    /** Display a user message. Argument pri defines message priority:
      * <0: display text as info on local terminal and in logfile anyway, independent of ignorelevel
      * 0: local terminal only; if debug level is enabled (TGo4Log::SetIgnorelevel(0))
      * 1: local terminal and info message in gui log panel (if gui mode)
      * 2: local terminal and warning message in gui log panel (if gui mode)
      * 3: local terminal and error message in gui log panel (if gui mode) */
    void Message(Int_t prio, const char* text,...);

    /** User May send any named object via data channel to the gui.
      * In batch mode, this method is disabled. */
    void SendObjectToGUI(TObject* ob);

    /** Start the object (histogram) server. Existing server will be stopped and replaced
      * by new server with different basename and password. */
    void StartObjectServer(const char* basename,  const char* passwd);

    /** Stop the object (histogram) server. */
    void StopObjectServer();

   /** Specify password for administrator account. For login from
      * remote gui into analysis server*/
   void SetAdministratorPassword(const char* passwd);

     /** Specify password for controller account. For login from
      * remote gui into analysis server*/
   void SetControllerPassword(const char* passwd);

   /** Specify password for observer account. For login from
      * remote gui into analysis server*/
   void SetObserverPassword(const char* passwd);


    /** Printout the analysis (step-) setup. */
    virtual void Print(Option_t* opt="") const;

    /** Printout of all histogram statistic counters on the terminal. */
    void PrintHistograms(const char* expression = 0);

    /** Print all condition counters to the terminal. */
    void PrintConditions(const char* expression = 0);

    /** Printout of all parameters on the terminal. */
    void PrintParameters(const char* expression = 0);

    /** Print entries of current dynamic list. */
    void PrintDynamicList();

    /** Printout event content of single current event to analysis terminal
      * (using ROOT TTree::Show method).
      * Event is searched by name of analysis step. If isoutput true, then
      * output event of that step is used, otherwise input event.
      * If analysis step not found, event of stepname is searched in registered folders. */
    void ShowEvent(const char* stepname, Bool_t isoutput=kTRUE);

    /** Write object obj into eventstore of the step specified by name.
      * Current event number will be added to object name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreParameter(const char * name, TGo4Parameter* par);

    /** Write condition con into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreCondition(const char * name, TGo4Condition* con);

    /** Write fitter fit into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreFitter(const char * name, TGo4Fitter* fit);

    /** Write folder into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreFolder(const char * stepname, TFolder* folder);

    /** Write registered folder of foldername to the step store of stepname.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step, +2 in case of unknown folder*/
    Int_t StoreFolder(const char * stepname, const char* foldername);

    /** Perform autosave of all active stores, save existing calibration objects,
      * write known objects into user file. */
    void AutoSave();

    /** Method for user analysis constructor to setup the list
      * of analysis steps. Each call of this method will add a new
      * analysis step at the end of the list. Caution: user has to take
      * care that output and input event types of subsequent steps
      * are matching! Analysis steps cannot be removed, since the
      * steplist is not intended to be dynamic. This is a pure creation method.
      * Analysis steps are owned by the steplist once they have been added. */
    Bool_t AddAnalysisStep(TGo4AnalysisStep* next);

    /** Access to certain analysis step by name. If name=0,
      * this will return the first active step. */
    TGo4AnalysisStep* GetAnalysisStep(const char* name);

    /** Access to certain analysis step by number. */
    TGo4AnalysisStep* GetAnalysisStepNum(Int_t number);

    /** Define passwords, which should be used for analysis access when analysis runs in server mode */
    void DefineServerPasswords(const char* admin, const char* controller, const char* observer);

    /** Set number of event loop count, executed in batch mode */
    void SetBatchLoopCount(Int_t cnt = -1) { fBatchLoopCount = cnt; }

    /** Set flag to use data from autosave file in Make... methods (default true) */
    void SetMakeWithAutosave(Bool_t on = kTRUE) { fbMakeWithAutosave = on; }

    /** Returns kTRUE if object in last Make... call was created,
     * kFALSE when object was retrieved from autosavefile */
    Bool_t IsObjMade() { return fbObjMade; }

    /** Create one dimensional histogram of specified type. Type can be:
     * 'I', 'i' for TH1I - Int_t as bin content (default)
     * 'F', 'f' for TH1F - Float_t as bin content
     * 'D', 'd' for TH1D - Double_t as bin content
     * 'S', 's' for TH1S - Short_t as bin content
     * 'C', 'c' for TH1C - Char_t as bin content
     * fullname specifies name of histogram (optionally with subfolder name)
     * title - histogram title
     * nbinsx, xlow, xup - range arguments for X axis
     * xtitle, ytitle - title for X and Y axis of histogram
     * If histogram exactly with same name and type already exists in  autosave file,
     * it will be return. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
     */
    TH1* MakeTH1(char type, const char* fullname, const char* title,
                 Int_t nbinsx, Double_t xlow, Double_t xup,
                 const char* xtitle = 0, const char* ytitle = 0);

    /** Create two dimensional histogram of specified type. Type can be:
     * 'I', 'i' for TH2I - Int_t as bin content (default)
     * 'F', 'f' for TH2F - Float_t as bin content
     * 'D', 'd' for TH2D - Double_t as bin content
     * 'S', 's' for TH2S - Short_t as bin content
     * 'C', 'c' for TH1C - Char_t as bin content
     * fullname specifies name of histogram (optionally with subfolder name)
     * title - histogram title
     * nbinsx, xlow, xup - range arguments for X axis
     * nbinsy, ylow, yup - range arguments for Y axis
     * xtitle, ytitle, ztitle - title for X, Y and Z axis of histogram
     * If histogram exactly with same name and type already exists in  autosave file,
     * it will be return. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
     */
    TH2* MakeTH2(char type, const char* fullname, const char* title,
                 Int_t nbinsx, Double_t xlow, Double_t xup,
                 Int_t nbinsy, Double_t ylow, Double_t yup,
                 const char* xtitle = 0, const char* ytitle = 0, const char* ztitle = 0);

    /** Create 1D window condition.
     * fullname specifies name of condition (optionally with subfolder name)
     * xmin, xmax - condition range
     * HistoName - name of histogram, to which condition is assigned
     */
    TGo4WinCond* MakeWinCond(const char* fullname,
                             Double_t xmin, Double_t xmax,
                             const char* HistoName = 0);

    /** Create 2D window condition.
     * fullname specifies name of condition (optionally with subfolder name)
     * xmin, xmax - X condition range
     * ymin, ymax - Y condition range
     * HistoName - name of histogram, to which condition is assigned
     */
    TGo4WinCond* MakeWinCond(const char* fullname,
                             Double_t xmin, Double_t xmax,
                             Double_t ymin, Double_t ymax,
                             const char* HistoName = 0);

    /** Create polygon condition.
     * fullname specifies name of condition (optionally with subfolder name)
     * npoints - number of points in polygon condition
     * points - (X,Y) points
     * HistoName - name of histogram, to which condition is assigned
     * shapedcond - create a freestyle TGo4ShapedCond
     * To use method, array should be declared as following:
     * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
     * cond = MakePolyCond("Folder/CondName", 4, points);
     */
    TGo4PolyCond* MakePolyCond(const char* fullname,
                               Int_t npoints,
                               Double_t (*points) [2],
                               const char* HistoName = 0,
                               Bool_t shapedcond=kFALSE);



    /** Create ellipse shaped polygon condition.
         * fullname specifies name of condition (optionally with subfolder name)
         * npoints - number of points in ellipse condition, 0 for default resolution
         * cx,cy - center coordinates of ellipse
         * a1,a2 - width of ellipse half axes
         * theta - ellipse tilt angle
         * HistoName - name of histogram, to which condition is assigned
         */
    TGo4ShapedCond* MakeEllipseCond(const char* fullname,
        Int_t npoints,
        Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta=0,
        const char* HistoName = 0);

    /** Create circular shaped polygon condition.
             * fullname specifies name of condition (optionally with subfolder name)
             * npoints - number of points in shaped condition, 0 for default resolution
             * cx,cy   - circle center coordinates
             * r       - circle radius
             * HistoName - name of histogram, to which condition is assigned
             */
    TGo4ShapedCond* MakeCircleCond(const char* fullname,
            Int_t npoints, Double_t cx, Double_t cy, Double_t r,
            const char* HistoName = 0);

    /** Create tilted rectangular box shaped polygon condition.
            * fullname specifies name of condition (optionally with subfolder name)
            * cx,cy - center coordinates of box
            * a1,a2 - width of box half axes
            * theta - tilt angle
            * HistoName - name of histogram, to which condition is assigned
            */
    TGo4ShapedCond* MakeBoxCond(const char* fullname, Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta,
        const char* HistoName = 0);

    /** Create free shaped (polygon) condition.
     * in contrast to plain TGo4PolyCond, this one can be converted later
     * to different dedicated shape types (ellipse, box, etc.)
         * fullname specifies name of condition (optionally with subfolder name)
         * npoints - number of points in polygon condition
         * points - (X,Y) points
         * HistoName - name of histogram, to which condition is assigned
         * To use method, array should be declared as following:
         * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
         * cond = MakePolyCond("Folder/CondName", 4, points);
         */
    TGo4ShapedCond* MakeFreeShapeCond(const char* fullname,
          Int_t npoints,
          Double_t (*points) [2],
          const char* HistoName = 0);

    /** Create parameter of specified class,
     * fullname specifies name of condition (optionally with subfolder name)
     * classname - name of required parameter class, it should be known to ROOT.
     * cmd - optional argument, can be used for two purposes:
     *    1) as new command to create parameter like "new UserParameter(%s, 1000, 2000)",
     *       where %s is place for parameter name.  Should be specified, if parameter
     *       constructor contains more parameters as only parameter name. Such argument
     *       should be always started with 'new ' command.
     *    2) macro name to set parameter value. Macro executed immediately after parameter
     *       creation (or loading from auto-save file) and thus overwrites parameters value.
     *       Macro name should be always starting with "set_"
     */
    TGo4Parameter* MakeParameter(const char* fullname,
                                 const char* classname,
                                 const char* cmd = 0);

    /** Executes ROOT script.
     * Returns -1 when script was not found or result of script execution */
    Long_t ExecuteScript(const char* script_name);

    /** Method called from Ctrl-C handler */
    void ProcessCrtlCSignal();

    /** Set pointer on sniffer object */
    void SetSniffer(TGo4AnalysisSniffer* sniff) { fSniffer = sniff; }

    /** Returns pointer on current sniffer */
    TGo4AnalysisSniffer* GetSniffer() const { return fSniffer; }

    /** Returns pointer on analysis client */
    TGo4AnalysisClient* GetAnalysisClient() const { return fxAnalysisSlave; }

    /** Stop analysis event loop, works in both batch and gui-controlled mode */
    void StopAnalysis();

    /** (Re)Start analysis event loop, works in both batch and gui-controlled mode */
    void StartAnalysis();

    /** Method should be called to process extra events in the analysis thread context */
    void ProcessEvents();

  protected:

    /** True if analysis framework has been initialized and
      * the event classes are created. */
    Bool_t fbInitIsDone;

    /** True if Autosaving is enabled (default).
      * If false, calling AutoSave() will have no effect. */
    Bool_t fbAutoSaveOn;

    /** Protected constructor for singleton instance. */
    TGo4Analysis(const char* name = 0);

    /** Another variant for analysis constructor. */
    TGo4Analysis(int argc, char** argv);

    /** Method to create all necessary object in the real constructor */
    void Constructor();


    /** Uses the chain of analysis steps to process the first input
      * event into an output event which then is accessible at
      * the output event member. Returns number of
      * steps run. For internal usage in the MainCycle and
      * GetEvent methods; may also be called from UserEventFunc directly. */
    Int_t ProcessAnalysisSteps();

  private:

    /** Create a tree that is filled with one single event sample.
      * Event is taken from analysis step of name. If isoutput
      * is kTRUE, the output event is used, otherwise the input event.
      * If no analysis step of that name exists, the event is
      * searched by object name in the eventstructure folder
      * of the object manager. */
    TTree* CreateSingleEventTree(const char* name, Bool_t isoutput=kTRUE);

    /** Create a tree that is filled with one single event sample. To be sent to the gui. */
    TTree* CreateSingleEventTree(TGo4EventElement* event);

    /** Update the internal list of object names. To be used internally and by analysis client. */
    void UpdateNamesList();

    /** Returns the nameslist of the go4 object directory structure. Is used
      * by analysis client to send the current object names to the gui. */
    TGo4AnalysisObjectNames * GetNamesList() const { return fxObjectNames; }

    /** Sets the current output event (detector) structure.
      * For internal usage by the chain of analysis steps, since
      * TGo4AnalysisStep is friend. */
    void SetOutputEvent(TGo4EventElement * event);

    /** Add reference to event store object to Go4 Folder structure.
      * To be used by framework only! */
    Bool_t AddEventStore(TGo4EventStore * store);

    /** Remove reference to event store from go4 folder structure. */
    Bool_t RemoveEventStore(TGo4EventStore * store);

    /** Add reference to event source object to Go4 Folder structure.
      * To be used by framework only!  */
    Bool_t AddEventSource(TGo4EventSource * source);

    /** Remove reference to event source from go4 folder structure. */
    Bool_t RemoveEventSource(TGo4EventSource* source);

    /** Add reference to event processor object to Go4 Folder structure.
      * To be used by framework only! */
    Bool_t AddEventProcessor(TGo4EventProcessor * pro);

    /** Remove reference to event processor from go4 folder structure. */
    Bool_t RemoveEventProcessor(TGo4EventProcessor * pro);

    /** Add reference to event structure object to Go4 Folder structure.
      * To be used by framework only! */
    Bool_t AddEventStructure(TGo4EventElement * ev);

    /** Remove reference to event structure from go4 folder structure. */
    Bool_t RemoveEventStructure(TGo4EventElement * ev);

    /** Register analysis client if running in gui mode. */
    void SetAnalysisClient(TGo4AnalysisClient* cli) { fxAnalysisSlave = cli; }

    /** Send message string in a status object to the gui.
      * Will be displayed in status window. Level indicates info (1), warning (2) or error(3),
      * if printout is true, message will also be printed on terminal.
      * In non-GUI mode, this method is equivalent to TGo4Log::Message */
    void SendMessageToGUI(Int_t level, Bool_t printout, const char* text);

    /** Open the autosave file with the current settings. */
    void OpenAutoSaveFile(bool for_writing = false);

    /** Close the autosave file if existing. */
    void CloseAutoSaveFile();

    /** Called by interrupt routine for normal exit from program (in batch mode) */
    void StopWorking() { fxDoWorkingFlag = flagStop; }

    /** Returns true when event loop execution was stopped, only in batch mode */
    Bool_t IsStopWorking() const { return fxDoWorkingFlag == flagStop; }

    /* for signal handler to shutdown analysis server, if existing*/
    void ShutdownServer();

    /** Static Pointer to the analysis singleton instance. */
    static TGo4Analysis* fxInstance;             //!

    /** This is used to check from outside if an analysis is
      * already there. Useful if one wants to prevent creating
      * a complete analysis instance in the gui task... */
    static Bool_t fbExists; //!

    /** This is used to identify running mode of analysis:
      * 0 - batch, 1 - client, 2 - server */
    static Int_t fiRunningMode; //!

    /** Optional backpointer to analysis client if running in gui mode.*/
    TGo4AnalysisClient* fxAnalysisSlave;            //!

    /** @link aggregationByValue
      *   @supplierCardinality 1 */
    TGo4AnalysisStepManager* fxStepManager;      //!

    /** @link aggregationByValue
      *   @supplierCardinality 1 */
    TGo4AnalysisObjectManager* fxObjectManager;  //!

    /** Event counter for autosave mechanism. */
    Int_t fiAutoSaveCount;                       //!

    /** Clock for autosave by time mechanism. */
    TStopwatch* fxAutoSaveClock;                //!

    /** Implicit Loop cycles in between two autosave actions. Autosave will write
      * calibration objects of steps into event file and will save all histograms into an default
      * root file. */
    Int_t fiAutoSaveInterval;                    //!

    /** compression level for autosave file */
    Int_t fiAutoSaveCompression;                 //!

    /** Mutex protecting the autosave file access. On Termination, both the
      * main thread and  the appcontrol timer might try to autosave or delete on
      * the same file. */
    TMutex* fxAutoSaveMutex;                     //!

    /** Optional file for autosave of the important objects. */
    TFile* fxAutoFile;                           //!

    /** Name of the autosave file. */
    TString fxAutoFileName;                      //!

    /** True if autosave file is in overwrite mode */
    Bool_t fbAutoSaveOverwrite;                  //!

    /** name of configuration file used for this setup */
    TString fxConfigFilename;                    //!

    /** True if input file of multiple file input has changed.
      *  User analysis may check this and read mbs buffer infos then. */
    Bool_t fbNewInputFile;                       //!

    /** Contains current input file name */
    TString fxCurrentInputFileName;               //!

    /** True if autosave name shall be changed when input file of multiple file input has changed.
      *  To be set in user analysis; will be evaluated in TGo4MbsFile */
    Bool_t fbAutoSaveFileChange;                 //!

    /** this holds event sample for the SingleEventTree. Must be clone
      * of the event in use, since TTree::Show() will modify it! */
    TGo4EventElement* fxSampleEvent;             //!

    /** @link aggregation
      *   @supplierCardinality 1 */
    TGo4AnalysisObjectNames * fxObjectNames;     //!

    /** Indicates analysis runstate */
    enum { flagInit, flagRunning, flagPause, flagClosed, flagStop  } fxDoWorkingFlag; //!

    /** use to treat Ctrl-C interrupts */
    TGo4InterruptHandler* fxInterruptHandler; //!

    /** name of analysis object */
    TString fAnalysisName;

    /** exact number of loop counts in batch mode */
    Int_t fBatchLoopCount;

    /** preconfigured password for administrator access */
    TString fServerAdminPass; //!

    /** preconfigured password for controller access */
    TString fServerCtrlPass; //!

    /** preconfigured password for observer access */
    TString fServerObserverPass; //!

    /** If false, do not use data from autosave file in Make... methods */
    Bool_t fbMakeWithAutosave; //!

    /** indicate if object was created by last Make... operation */
    Bool_t fbObjMade; //!

    /** number Ctrl-C handler called */
    Int_t fNumCtrlC; //!

    /** Sniffer object */
    TGo4AnalysisSniffer* fSniffer; //!

    /** optional ratemeter object */
    TGo4Ratemeter* fxRate; //!

  ClassDef(TGo4Analysis,4)
};

#endif
