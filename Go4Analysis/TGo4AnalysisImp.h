#ifndef TGO4ANALYSISIMP_H
#define TGO4ANALYSISIMP_H

#include "TGo4CommandReceiver.h"
#include "TObject.h"
#include "TString.h"

class TH1;
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
class TGo4AnalysisStatus;
class TGo4AnalysisStep;
class TGo4AnalysisClient;
class TGo4HistogramStatus;
class TGo4DynamicEntry;
class TGo4Condition;
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

/**
 * The mother of all go4 analyses. Provides services to organize
 * events, histogram data, and everything. May run standalone in batch mode,
 * or may be aggregated by analysis client for threaded online control. User
 * has to derive his/her own analysis from this base class and override the
 * virtual user event function and set event factory methods.
 * @author J. Adamczewski
 * @since 19-jan-2001
 */

class TGo4Analysis : public TGo4CommandReceiver, public TObject  {

  friend class TGo4AnalysisStep;
  friend class TGo4AnalysisClient;
  friend class TGo4ComGetCurrentEvent;
  friend class TGo4ComGetNamesList;
  friend class TGo4ComGetEnvelope;
  friend class TGo4ComSetObject;
  friend class TGo4HisConnectorRunnable;
  friend class TGo4HistogramServer;

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

    virtual ~TGo4Analysis();

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
      * May be called explicitely from analysis client thread, or may run
      * in an implicit loop provided by method RunImplicit, if
      * analyis works in a non threaded standalone mode. To be overridden
      * in the user analysis class. */
    virtual Int_t UserEventFunc();

    /** User defined function called once before processing the main
      * event loop. Optionally. */
    virtual Int_t UserPreLoop();

    /** User defined function called once after processing the main
      * event loop. Optionally. */
    virtual Int_t UserPostLoop();

    /** Processes the UserEventFunc in an implicit loop for "times" times.
      * For non-threaded usage of the analysis class. */
    Int_t RunImplicitLoop(Int_t times);

    /** Finish the analysis run and close all event sources/storages.
      * The analysis instance and the setup of the analysis steps is not deleted. */
    virtual void CloseAnalysis();

    /** Initialization of the event class plugins which are delivered from
      * the user defined event factory. The virtual standard base class method may be changed
      * by overriding; normally, this is not necessary. */
    virtual Bool_t InitEventClasses();

    /** Add (create) new dynamic histogram entry which connects an existing
      * histogram with existing condition and data.
      * Dynamic entry is specified by name. Histogram histo will be searched in
      * registered histograms folder, condition in conditions folder. If
      * condition is true or not existing (condition=0), histogram will be filled
      * from the values that are found in registered eventstructure classes of
      * names  evx, evy, evz at the data members of names memx, memy, memz,
      * for the three coordinate axes, respectively. Histogram dimension is
      * checked against given values. */
    TGo4AnalysisObjectManager* ObjectManager() const { return fxObjectManager; }

    Bool_t AddDynamicHistogram(const Text_t* name,
                               const Text_t* histo,
                               const Text_t* hevx, const Text_t* hmemx,
                               const Text_t* hevy=0, const Text_t* hmemy=0,
                               const Text_t* hevz=0, const Text_t* hmemz=0,
                               const Text_t* condition=0,
                               const Text_t* cevx=0, const Text_t* cmemx=0,
                               const Text_t* cevy=0, const Text_t* cmemy=0);


    /** Set dynamic entry of name "name" to the values specified by external
      * dynamic entry status "state". If no dynamic entry of that name exists, a
      * new entry is created with the properties of the state, and
      * added to the current dynamic list. */
    Bool_t AddDynamicEntry(TGo4DynamicEntry* entry);

    /** Remove entry of that name from dynamic list of listname.
      * The referred objects (histograms, conditions..) are still on heap, since
      * they are owned by their special folders. */
    Bool_t RemoveDynamicEntry(const Text_t* entryname, const Text_t* listname=0);

    /** Add any external object to the user object folder.
      * Object is owned by go4 aferwards and will be saved automatically.
      * Object is accessible by name from the go4 display and from the
      * analysis itself. Subfolder of UserObjects may be specified.
      * If replace is true, old object of same name will be deleted and
      * replaced by the added one. */
    Bool_t AddObject(TNamed * anything, const Text_t* subfolder=0, Bool_t replace=kTRUE);

    /** Searches for object by name in all directories. Returns
      * pointer to object. If
      * object of that name does not exist, null is returned.
      * Optionally, folder may be specified to search in. */
    TNamed * GetObject(const Text_t* name, const Text_t* folder=0);

    /** Removes object from user object folder by name. Returns kFALSE if no
      * such histogram. Object is deleted on heap only if del is true.
      * Otherwise, user owns the object afterwords. */
    Bool_t RemoveObject(const Text_t* name, Bool_t del=kTRUE);

    /** Delete object of name, or all objects in folder name, respectively.
      * Objects are only deleted if delete protection is false. Usually,
      * Objects registered from user code are delete protected by default.
      * Objects created dynamically from gui are not delete protected. */
    Bool_t DeleteObjects(const Text_t * name);

    /** Clear (reset) the specified objects. Method will first
      * clear all objects in any folder if it is matching the namestring.
      * If no such folder exists, object of name is searched and cleared.
      * Returns kFALSE in case of not found objects. */
    Bool_t ClearObjects(const Text_t* name);

    /** Change protection properties of object name as specified.
      * If object is a folder, change properties of all objects in this folder recursively.
      * Flags may contain key letters like:
      * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
      * "+D"/"-D" to enable/disable protection against object deletion
      * For example flags="+C-D", "+C+D", "-D-C", "-C"
      * Properties not appearing in flags are not changed.  */
    Bool_t ProtectObjects(const Text_t* name, const Option_t* flags);

    /** Clear previous events of backstore tree. For reset of dynamic tree histogram
     * If called with clearflag=true, do not reset tree, but only clear
     * the backstore reset bit (done after processing of complete dynamic list)*/
    Bool_t ResetBackStores(Bool_t clearflag=kFALSE);

    /** Delivers pointer to next object of the Go4 folder structure
      * with a name matching the expression expr. If reset is true,
      * The list of matchint objects will be created anew by comparing
      * all names with expr. If reset is false, the next object of a
      * previously created matching list is returned. Optionally the
      * search can be limited to a given folder. */
    TObject* NextMatchingObject(const Text_t* expr=0,
                                const Text_t* folder=0,
                                Bool_t reset=kFALSE);

    /** Create a tree structure for a certain tree by name */
    TGo4TreeStructure* CreateTreeStructure(const Text_t* treename);

    /** Add external histogram to go4 histogram directory. Histogram will be owned
      * and streamed by go4 histogram list afterwards. */
    Bool_t AddHistogram(TH1* his , const Text_t* subfolder=0, Bool_t replace=kTRUE);

    /** Add reference to a tree in the go4 folder structure.
      * If name of subfolder is given, tree will be assigned
      * to subfolder (e.g. subfolder with analysis step name).
      * If subfolder name is zero,
      * the tree will be put into general tree folder. */
    Bool_t AddTree(TTree* tree, const Text_t* subfolder=0);

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
    TH1* GetHistogram(const Text_t* name);

    /** Search tree in tree folder. */
    TTree * GetTree(const Text_t* name);

    /** Removes histogram from histogram dir by name. Returns kFALSE if no
      * such index. Histogram object is deleted on heap.
      * Dynamic list entry for that histogram object is also removed. */
    Bool_t RemoveHistogram(const Text_t* name, Bool_t del=kTRUE);

    /** Puts a new analysis condition object in corresponding list.
      * Object is owned by list afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddAnalysisCondition(TGo4Condition * con, const Text_t* subfolder=0);

    /** Set existing analysis condition of name to the values
      * of external condition object con. Used to set condition by command
      * from gui, or to update the precompiled condition from the values
      * that were saved to the autosave file. */
    Bool_t SetAnalysisCondition(const Text_t* name, TGo4Condition* con, Bool_t counter=kTRUE);

    /** Retrieves an analysis condition  from list by name. Returns 0 if no
      * such condtion. */
    TGo4Condition * GetAnalysisCondition(const Text_t* name);

    /** Removes analysis condition from list by name. Returns 0 if no
      * such condition. Condition object is deleted on heap. */
    Bool_t RemoveAnalysisCondition(const Text_t* name);

    /** Puts a new parameter object in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddParameter(TGo4Parameter * par, const Text_t* subfolder=0);

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

    /** Retrieves a parameter object by name from the object folder. Returns 0 if no
      * such paramter. */
    TGo4Parameter * GetParameter(const Text_t* name);

    /** Removes parameter by name. Returns 0 if no
      * such parameter. Parameter object is deleted on heap. */
    Bool_t RemoveParameter(const Text_t* name);

    /** Puts a new picture object in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exists in list. */
    Bool_t AddPicture(TGo4Picture * pic, const Text_t* subfolder=0);

    /** Set existing picture of name to the values
      * of external picture object pic. Used to set picture by command
      * from gui, or to update the precompiled picture from the values
      * that were saved to the autosave file. */
    Bool_t SetPicture(const Text_t* name, TGo4Picture * pic);

    /** Retrieves a picture object by name from the object folder.
      * Returns 0 if no such picture. */
    TGo4Picture * GetPicture(const Text_t* name);

    /** Removes picture by name. Returns 0 if no
      * such picture. Picture object is deleted on heap. */
    Bool_t RemovePicture(const Text_t * name);

    /** Puts a new TCanvas in corresponding folder.
      * Object is owned by folder afterwards. Returns false if object
      * of that name already exist. */
    Bool_t AddCanvas(TCanvas * can, const Text_t* subfolder=0);

    /** Retrieves a TCanvas by name from the Canvases folder.
      * Returns 0 if no such object. */
    TCanvas * GetCanvas(const Text_t* name);

    /** Removes TCanvas by name. Returns 0 if no
      * such canvas. TCanvas object is deleted on heap. */
    Bool_t RemoveCanvas(const Text_t * name);

    /** Create a copy of the analysis internal state.
      * To be sent to the Display. */
    TGo4AnalysisStatus* CreateStatus();

    /** Find Object of name in the folder structure
      * and create a complete status object of it.
      * Used by GUI command to get updated information on certain object. */
    TGo4ObjectStatus* CreateObjectStatus(const Text_t * name, const Text_t* folder=0);

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
    Bool_t SetFirstStep(const Text_t* name);

    /** Sets analysis step name as last one to be processed in
      * the chain of steps. Last step will write output event into its
      * event store by default, its output event is accessible in the user
      * analysis. If name is nullpointer, takes last step in steplist. */
    Bool_t SetLastStep(const Text_t* name);

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
    Bool_t SetStepStorage(const Text_t* name, Bool_t on);

    /** Shuts down the old analysis step storage and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch output files. */
    Bool_t NewStepStore(const Text_t* name, TGo4EventStoreParameter* par);

    /** Shuts down the old analysis step event source and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch input. */
    Bool_t NewStepSource(const Text_t* name, TGo4EventSourceParameter * par);

    /** Shuts down the old analysis step processor and creates a new
      * one specified by parameter par. To be used in online mode to
      * switch event processing. */
    Bool_t NewStepProcessor(const Text_t* name, TGo4EventProcessorParameter * par);

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
    void SetAutoSave(Bool_t on=kTRUE) {fbAutoSaveOn=on;}

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

    /** Called by interrupt routine for normal exit from WaitForStart() function */
    void StopWaiting();

    /** True if current analysis step allows stop on error */
    Int_t IsErrorStopEnabled();

    /** Set flag that input file has changed */
    void SetNewInputFile(Bool_t on=kTRUE) {fbNewInputFile=on;}

    /** True if input filelist has changed to a new lmd file. */
    Bool_t IsNewInputFile() const { return fbNewInputFile; }

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

    /** Search reference to event stucture in  folder. */
    TGo4EventElement * GetEventStructure(const Text_t * name);

    /** Display a user message. Argument pri defines message priority:
      * <0: display text as info on local terminal and in logfile anyway, independent of ignorelevel
      * 0: local terminal only; if debug level is enabled (TGo4Log::SetIgnorelevel(0))
      * 1: local terminal and info message in gui log panel (if gui mode)
      * 2: local terminal and warning message in gui log panel (if gui mode)
      * 3: local terminal and error message in gui log panel (if gui mode) */
    void Message(Int_t prio, const Text_t* text,...);

    /** User May send any named object via data channel to the gui.
      * In batch mode, this method is disabled. */
    void SendObjectToGUI(TNamed* ob);

    /** Start the object (histogram) server. Existing server will be stopped and replaced
      * by new server with different basename and password. */
    void StartObjectServer(const Text_t* basename,  const Text_t* passwd);

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
    void PrintHistograms(const Text_t* expression=0);

    /** Print all condition counters to the terminal. */
    void PrintConditions(const Text_t* expression=0);

    /** Print entries of current dynamic list. */
    void PrintDynamicList();

    /** Printout event content of single current event to analysis terminal
      * (using ROOT TTree::Show method).
      * Event is searched by name of analyis step. If isoutput true, then
      * output event of that step is used, otherwise input event.
      * If analysis step not found, event of stepname is searched in registered folders. */
    void ShowEvent(const Text_t* stepname, Bool_t isoutput=kTRUE);

    /** Write object obj into eventstore of the step specified by name.
      * Current event number will be added to object name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreParameter(const Text_t * name, TGo4Parameter* par);

    /** Write condition con into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreCondition(const Text_t * name, TGo4Condition* con);

    /** Write fitter fit into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreFitter(const Text_t * name, TGo4Fitter* fit);

    /** Write folder into eventstore of the step specified by name.
      * Current event number will be added to parameter name in the file.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step. */
    Int_t StoreFolder(const Text_t * stepname, TFolder* folder);

    /** Write registered folder of foldername to the step store of stepname.
      * Returns -1 in case of not active eventstore. Returns +1 in case
      * of unknown step, +2 in case of unknown folder*/
    Int_t StoreFolder(const Text_t * stepname, const Text_t* foldername);

    /** Perform autosave of all active stores, save existing calibration objects,
      * write known objects into user file. */
    void AutoSave();

    /** Method for user analysis constructor to setup the list
      * of analyisis steps. Each call of this method will add a new
      * analysis step at the end of the list. Caution: user has to take
      * care that output and input event types of subsequent steps
      * are matching! Analysis steps cannot be removed, since the
      * steplist is not intended to be dynamic. This is a pure creational method.
      * Analysis steps are owned by the steplist once they have been added. */
    Bool_t AddAnalysisStep(TGo4AnalysisStep* next);

    /** Access to certain analysis step by name. If name=0,
      * this will return the first active step. */
    TGo4AnalysisStep* GetAnalysisStep(const char* name);

  protected:

    /** True if analysis framework has been initialized and
      * the event classes are created. */
    Bool_t fbInitIsDone;

    /** True if Autosaving is enabled (default).
      * If false, calling AutoSave() will have no effect. */
    Bool_t fbAutoSaveOn;

    /** Protected constructor for singleton instance. */
    TGo4Analysis();

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
    TTree* CreateSingleEventTree(const Text_t* name, Bool_t isoutput=kTRUE);

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
    void SetAnalysisClient(TGo4AnalysisClient* cli) { fxAnalysisSlave=cli; }

    /** Send message string in a status object to the gui.
      * Will be displayed in status window. Level indicates info (1), warning (2) or error(3),
      * if printout is true, message will also be printed on terminal.
      * In non-GUI mode, this method is equivalent to TGo4Log::Message */
    void SendMessageToGUI(Int_t level, Bool_t printout, const char* text);

    /** Open the autosave file with the current settings. */
    void OpenAutoSaveFile();

    /** Close the autosave file if existing. */
    void CloseAutoSaveFile();

    /** Static Pointer to the analysis singleton instance. */
    static TGo4Analysis* fxInstance;             //!

    /** This is used to check from outsid if an analysis is
      * already there. Useful if one wants to prevent creating
      * a complete analysis instance in the gui task... */
    static Bool_t fbExists; //!

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

    /** True if autosave name shall be changed when input file of multiple file input has changed.
      *  To be set in user analysis; will be evaluated in TGo4MbsFile */
    Bool_t fbAutoSaveFileChange;                 //!

    /** this holds event sample for the SingleEventTree. Must be clone
      * of the event in use, since TTree::Show() will modify it! */
    TGo4EventElement* fxSampleEvent;             //!

    /** @link aggregation
      *   @supplierCardinality 1 */
    TGo4AnalysisObjectNames * fxObjectNames;     //!

    /** used for exit from WaitForStart() routine by interrupt */
    Bool_t fbStopWatingFlag; //!

  ClassDef(TGo4Analysis,3)
};

#endif
