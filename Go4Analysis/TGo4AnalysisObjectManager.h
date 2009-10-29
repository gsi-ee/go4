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

#ifndef TGO4ANALYSISOBJECTMANAGER_H
#define TGO4ANALYSISOBJECTMANAGER_H

#include "TNamed.h"
#include "Htypes.h"

class TCanvas;
class TTree;
class TMutex;
class TFolder;
class TH1;
class TH2;
class TFile;
class TDirectory;
class TIterator;

class TGo4EventElement;
class TGo4EventProcessor;
class TGo4EventSource;
class TGo4EventStore;
class TGo4ObjectStatus;
class TGo4TreeStructure;
class TGo4CompositeEvent;
class TObjArray;
class TGo4Condition;
class TGo4WinCond;
class TGo4PolyCond;
class TGo4AnalysisObjectNames;
class TGo4DynamicEntry;
class TGo4MbsHist;
class TGo4Picture;
class TGo4Parameter;
class TGo4ParameterStatus;

/**
 * This object is responsible for the
 * organisation of analysis objects. Keeps Folder structures and lists. Aggregate of TGo4Analysis.
 * @author J. Adamczewski
 * @since 07-May-2002
 */
class TGo4AnalysisObjectManager : public TNamed
   {
   public:

      TGo4AnalysisObjectManager();

      TGo4AnalysisObjectManager(const char* name);

      ~TGo4AnalysisObjectManager();

      /**
       * Add any external object to the user object folder.
       * Object is owned by go4 aferwards and will be saved automatically.
       * Object is accessible by name from the go4 display and from the
       * analysis itself. Subfolder of UserObjects may be specified.
       * If replace is true, old object of same name will be deleted and
       * replaced by the added one.
       */
      Bool_t AddObject(TNamed * anything, const char* subfolder=0, Bool_t replace=kTRUE);

      /**
       * Searches for object by name in all go4 folders. Returns
       * pointer to object. If
       * object of that name does not exist, null is returned
       * Optionally, name of folder may be specified; object is
       * only searched within that folder then. D
       */
      TNamed * GetObject(const char * name, const char* folder=0);

     /**
       * Removes object from user object folder by name. Returns kFALSE if no
       * such histogram. Object is deleted on heap only if del is true.
       * Otherwise, user owns the object afterwords.
       */
      Bool_t RemoveObject(const char * name, Bool_t del=kTRUE);



      /**
       * Delete object of name, or all objects in folder name, respectively.
       * Objects are only deleted if delete protection is false. Usuallay,
       * Objects registered from user code are delete protected by default.
       * Objects created dynamically from gui are not delete protected.
       */
      Bool_t DeleteObjects(const char * name);

  /**
    * Delivers pointer to next object of the Go4 folder structure
    * with a name matching the expression expr. If reset is true,
    * The list of matchint objects will be created anew by comparing
    * all names with expr. If reset is false, the next object of a
    * previously created matching list is returned. Optionally the
    * search can be limited to a given folder.
    */
    TObject* NextMatchingObject(const char* expr,
                                const char* folder,
                                Bool_t reset);

      /**
        * Create a folder with subfolders from a list of TBranchElements. For each
        * subbranch a subfolder will be created, until a TBranchElement
        * without subbranches occur. The latter will be put into a
        * TGo4ObjectStatus class.
        * Flag istopbranch is used to indicate top level call for composite event.
        */
      TFolder * CreateBranchFolder(TObjArray* branchlist,
                                   const char* name,
                                   const char* title,
                                   Bool_t istopbranch=kFALSE);

      /**
        * Create a folder with subfolders from a list of TBranchElements
        * that belongs to a TGo4CompositeEvent. For each
        * subbranch a subfolder will be created, until a TBranchElement
        * without subbranches occur. The latter will be put into a
        * TGo4ObjectStatus class.
        */
      TFolder * CreateCompositeBranchFolder(TObjArray* branchlist,
                                            TGo4CompositeEvent* compevent,
                                            Int_t startindex, Int_t* skip,
                                            const char* name, const char* title);

      /**
       * Create a tree structure
       * object that maps the TBranchelements into a TFolder hierarchy
       */
      TGo4TreeStructure * CreateTreeStructure(TTree * thetree);

      /** Create a tree structure for a certain tree by name */
      TGo4TreeStructure * CreateTreeStructure(const char* treename);

      /** Create a folder with members of this class */
      TFolder* CreateMembersFolder(const char* membrfoldername, TClass* cl);


      /**
       * Add external histogram to go4 histogram directory. Histogram will be owned
       * and streamed by go4 histogram list afterwards.
       */
      Bool_t AddHistogram(TH1 * his, const char* subfolder=0, Bool_t replace=kTRUE);

     /**
       * Search histogram in histogram list (directory).
       */
      TH1* GetHistogram(const char * name);

     /**
       * Removes histogram from histogram dir by name. Returns kFALSE if no
       * such histogram. Histogram object is deleted on heap only if del is true.
       * Otherwise, user owns the histogram afterwords.
       * Dynamic list entry for that histogram object is also removed.
       */
      Bool_t RemoveHistogram(const char * name, Bool_t del=kTRUE);

      /**
        * Create 1-dim histogram in histogram folder.
        * If histogram exists in autosave file, it will be taken from autosavefile.
        * If type of existing histogram does not match with provided, old histogram
        * will be removed and new proper will be created.
        * Parameters are:
        *    histotype = "C", "D", "F", "I", "S" corresonds to different TH1* types
        *    folderame - name of folder, where histogram should be created
        *    histoname - name of histogram
        *    nbinsx    - number of histogram bins
        *    xlow      - histogram lower limit
        *    xup       - histogram upper limit
        *    title     - histogram tille [optional]
        *    xtitle    - title on x axis [optional]
        *    ytitle    - title on y axis [optional]
        */

      TH1* MakeTH1(const char* histotype,
                  const char* foldername,
                  const char* histoname,
                  Int_t         nbinsx,
                  Axis_t        xlow,
                  Axis_t        xup,
                  const char* title = 0,
                  const char* xtitle = 0,
                  const char* ytitle = 0);

      /**
        * Create 2-dim histogram in histogram folder.
        * If histogram exists in autosave file, it will be taken from autosavefile.
        * If type of existing histogram does not match with provided, old histogram
        * will be removed and new proper will be created.
        * Parameters are:
        *    histotype = "C", "D", "F", "I", "S" corresonds to different TH1* types
        *    folderame - name of folder, where histogram should be created
        *    histoname - name of histogram
        *    nbinsx    - number of bins on x axis
        *    xlow      - lower limit on x axis
        *    xup       - upper limit on x axis
        *    nbinsy    - number of bins on y axis
        *    ylow      - lower limit on y axis
        *    yup       - upper limit on y axis
        *    title     - histogram tille [optional]
        *    xtitle    - title on x axis [optional]
        *    ytitle    - title on y axis [optional]
        */

      TH2* MakeTH2(const char* histotype,
                  const char* foldername,
                  const char* histoname,
                  Int_t         nbinsx,
                  Axis_t        xlow,
                  Axis_t        xup,
                  Int_t         nbinsy,
                  Axis_t        ylow,
                  Axis_t        yup,
                  const char* title = 0,
                  const char* xtitle = 0,
                  const char* ytitle = 0);

      /**
       * Add reference to a tree in the go4 folder structure.
       * If name of subfolder is given, tree will be assigned
       * to subfolder, e.g. dedicated to analysis step. If subfolder is zero,
       * the tree will be put into general tree folder.
       */
      Bool_t AddTree(TTree* tree, const char* subfolder=0);

      /**
       * Search tree in tree folder.
       */
      TTree * GetTree(const char * name);

      /**
       * Remove reference to a tree in the go4 folder structure.
       * If name of analysis step is given, tree will be assigned
       * to folder dedicated to that step. In stepname is zero,
       * the tree will be put into general tree folder.
       */
      Bool_t RemoveTree(TTree * tree, const char* stepname=0);

     /**
       * Puts a new analysis condition object in corresponding list.
       * Object is owned by list afterwards. Returns false if object
       * of that name already exists in list.
       */
      Bool_t AddAnalysisCondition(TGo4Condition * con, const char* subfolder=0);

      /**
        * Set existing analysis condition of name to the values
        * of external condition object con. Used to set condition by command
        * from gui, or to update the precompiled condition from the values
        * that were saved to the autosave file. optional parent folder can be
        * specified. By default, condition is searched relative to Conditions folder.
        */
      Bool_t SetAnalysisCondition(const char * name, TGo4Condition* con,
                                      Bool_t counter=kTRUE, TFolder* parent=0);

      /**
       * Retrieves an analysis condition  from list by name. Returns 0 if no
       * such condtion.
       */
      TGo4Condition * GetAnalysisCondition(const char * name);

      /**
       * Removes analysis condition from list by name. Returns 0 if no
       * such condition. Condition object is deleted on heap.
       */
      Bool_t RemoveAnalysisCondition(const char * name);

       /**
        * Create 1-dim window condition in conditions folder.
        * If conditions exists in autosave file, it will be taken from autosave file.
        * If type of existing condition does not match, condition
        * will be removed and new will be created.
        * Parameters are:
        *    folderame      - name of folder, where condition should be created
        *    conditionsname - name of condition
        *    xlow           - lower limit of condition
        *    xup            - upper limit on condition
        *    bindhistogram  - name of histogram, connected to condition [optional]
        *    invert         - invert condition
        */

      TGo4WinCond* MakeWindowCond(const char* foldername,
                                  const char* conditionname,
                                  Double_t      xlow,
                                  Double_t      xup,
                                  const char* bindhistogram = 0,
                                  Bool_t        invert = kFALSE);

       /**
        * Create 2-dim window condition in conditions folder.
        * If conditions exists in autosave file, it will be taken from autosave file.
        * If type of existing condition does not match, condition
        * will be removed and new will be created.
        * Parameters are:
        *    folderame      - name of folder, where condition should be created
        *    conditionsname - name of condition
        *    xlow           - lower limit on x axis
        *    xup            - upper limit on x axis
        *    ylow           - lower limit on y axis
        *    yup            - upper limit on y axis
        *    bindhistogram  - name of histogram, connected to condition [optional]
        *    invert         - invert condition
        */

      TGo4WinCond* MakeWindowCond(const char* foldername,
                                  const char* conditionname,
                                  Double_t      xlow,
                                  Double_t      xup,
                                  Double_t      ylow,
                                  Double_t      yup,
                                  const char* bindhistogram = 0,
                                  Bool_t        invert = kFALSE);

       /**
        * Create polygon 2-dim condition in conditions folder.
        * If conditions exists in autosave file, it will be taken from autosave file.
        * If type of existing condition does not match, condition
        * will be removed and new will be created.
        * Points of polygon are taken from two dimentional array,
        * which can be declared in program like:
        *   Float_t points[4][2] =
        *           {{    0.,    0.},
        *            {    0., 4000.},
        *            {40000., 4000.},
        *            {40000.,    0.}};
        * If first and last points will be connected, if they didnot match each other
        *
        * Parameters are:
        *    folderame      - name of folder, where condition should be created
        *    conditionsname - name of condition
        *    size           - number of points in polygon
        *    points         - array of (x,y) coordinate of polygon
        *    bindhistogram  - name of histogram, connected to condition [optional]
        *    invert         - invert condition
        */

      TGo4PolyCond* MakePolyCond(const char* foldername,
                                 const char* conditionname,
                                 Int_t         size,
                                 Float_t       (*points)[2],
                                 const char* bindhistogram = 0,
                                 Bool_t        invert = kFALSE);
     /**
    * Puts a new parameter object in corresponding folder.
    * Object is owned by folder afterwards. Returns false if object
    * of that name already exists in list.
    */
      Bool_t AddParameter(TGo4Parameter * par, const char* subfolder=0);

    /**
     * Set existing parameter of name to the values
     * of external parameter object par. Used to set parameter by command
     * from gui, or to update the precompiled parameter from the values
     * that were saved to the autosave file. Optional parent folder can be specified.
     * by default, parameter is searched relative to Parameters folder.
     */
      Bool_t SetParameter(const char * name, TGo4Parameter * par, TFolder* parent=0);

    /**
     * Set existing parameter of name to the values
     * of external parameter object par. Used to set parameter by command
     * from gui, or to update the precompiled parameter from the values
     * that were saved to the autosave file. Optional parent folder can be specified.
     * by default, parameter is searched relative to Parameters folder.
     */
      Bool_t SetParameterStatus(const char* name, TGo4ParameterStatus* par, TFolder* parent=0);

   /**
    * Retrieves a parameter object by name from the object folder. Returns 0 if no
    * such paramter.
    */
      TGo4Parameter * GetParameter(const char * name);

  /**
    * Removes parameter by name. Returns 0 if no
    * such parameter. Parameter object is deleted on heap.
    */
      Bool_t RemoveParameter(const char * name);

   /**
    * Puts a new picture object in corresponding folder.
    * Object is owned by folder afterwards. Returns false if object
    * of that name already exists in list.
    */
      Bool_t AddPicture(TGo4Picture * pic, const char* subfolder=0);

    /**
     * Set existing picture of name to the values
     * of external picture object pic. Used to set picture by command
     * from gui, or to update the precompiled picture from the values
     * that were saved to the autosave file. Optional parent folder can be specified.
     * by default, picture is searched relative to Pictures folder.
     */
      Bool_t SetPicture(const char* name, TGo4Picture * pic, TFolder* parent=0);

   /**
    * Retrieves a picture object by name from the object folder. Returns 0 if no
    * such picture.
    */
      TGo4Picture * GetPicture(const char * name);

  /**
    * Removes picture by name. Returns 0 if no
    * such picture. Picture object is deleted on heap.
    */
      Bool_t RemovePicture(const char * name);


  /**
    * Puts a new TCanvas in corresponding folder.
    * Object is owned by folder afterwards. Returns false if object
    * of that name already exists in list.
    */
      Bool_t AddCanvas(TCanvas * can, const char* subfolder=0);

   /**
    * Retrieves a TCanvas by name from the Canvases folder. Returns 0 if no
    * such object.
    */
      TCanvas * GetCanvas(const char * name);

  /**
    * Removes TCanvas by name. Returns 0 if no
    * such canvas. TCanvas object is deleted on heap.
    */
      Bool_t RemoveCanvas(const char * name);

      /**
       * Find Object of name in the folder structure and create a complete status object of it. Used by GUI command to get updated information on certain object.
       * Optionally, name of search folder may be given.
       */
      TGo4ObjectStatus * CreateObjectStatus(const char * name, const char* folder=0);

      /** Create status object for object ob. If fullinfo flag is false,
        * skip time consuming parts of the status (case of nameslist) */
      TGo4ObjectStatus * CreateObjectStatus(TObject* ob, Bool_t fullinfo=kTRUE);


      /**
       * Creates a list of names (keys) of all objects in analysis
       * directories.
       */
      TGo4AnalysisObjectNames * CreateNamesList();

      /**
       * Conversion of the TFolder of objects into a TFolder containing
       * the object names as TObjectStrings. Subfolders are translated to
       * subfolders containing subfolder names.
       */
      TFolder * CreateNamesFolder(TFolder * objectfolder);

     /**
      * Access to top folder of all objects.
      */
      TFolder * GetObjectFolder();

      /**
       * Add reference to event store object to Go4 Folder structure. To be used by framework only!
       */
      Bool_t AddEventStore(TGo4EventStore * store);

      /**
        * Remove reference to event store from go4 folder structure.
        */
      Bool_t RemoveEventStore(TGo4EventStore * store);

      /**
       * Add reference to event source object to Go4 Folder structure. To be used by framework only!
       */
      Bool_t AddEventSource(TGo4EventSource * source);

      /**
       * Remove reference to event source from go4 folder structure.
       */
      Bool_t RemoveEventSource(TGo4EventSource* source);

      /**
       * Add reference to event processor object to Go4 Folder structure. To be used by framework only!
       */
      Bool_t AddEventProcessor(TGo4EventProcessor * pro);

      /**
       * Remove reference to event processor from go4 folder structure.
       */
      Bool_t RemoveEventProcessor(TGo4EventProcessor * pro);

      /**
        * Add reference to event structure object to Go4 Folder structure. To be used by framework only!
        */
      Bool_t AddEventStructure(TGo4EventElement * ev);

      /**
       * Remove reference to event structure from go4 folder structure.
       */
      Bool_t RemoveEventStructure(TGo4EventElement * ev);

      /**
        * Search reference to event stucture in  folder.
        */
      TGo4EventElement * GetEventStructure(const char * name);

      /**
       * Reset all registered backstore instances.Called by dynamic list processing.
       */
      Bool_t ResetBackStores(Bool_t clearflag=kFALSE);

      /**
       * Cleanups required when analysis is closed.
       */
      void CloseAnalysis();

     /** Save folder structure to given file. */
     void SaveObjects(TFile* file);

     /**
       * Load objects from file. File should be opened and closed by the
       * caller.
       */
      Bool_t LoadObjects(TFile* statusfile);

      /** Adds entry to object manager */
      Bool_t AddDynamicEntry(TGo4DynamicEntry* entry);

      /** Reset the current dynamic list. */
      void ResetCurrentDynList();

      /** Loop over the dynamic list and
        * process the actions linked to the entries. */
      void ProcessDynamicList();

      void SetDynListInterval(Int_t val) { fiDynListInterval=val; }

      Int_t GetDynListInterval() const { return fiDynListInterval; }

      /**
       * Add (create) new dynamic histogram entry which connects an existing
       * histogram with existing condition and data.
       * Dynamic entry is specified by name. Histogram histo will be searched in
       * registered histograms folder, condition in conditions folder. If
       * condition is true or not existing (condition=0), histogram will be filled
       * from the values that are found in registered eventstructure classes of
       * names  hevx, hevy, hevz at the data members of names hmemx, hmemy, hmemz,
       * for the three coordinate axes, respectively. Histogram dimension is
       * checked against given values. Condition is tested regarding the
       * events cevx, cevy with members cmemx, cmemy
       */
      Bool_t AddDynamicHistogram(const char* name,
                                 const char* histo,
                                 const char* hevx, const char* hmemx,
                                 const char* hevy=0, const char* hmemy=0,
                                 const char* hevz=0, const char* hmemz=0,
                                 const char* condition=0,
                                 const char* cevx=0, const char* cmemx=0,
                                 const char* cevy=0, const char* cmemy=0);



   /**
    * Add Histogram into the dynamic list wich is linked to a tree.
    * If Histogram of hisname already exists, this histogram will taken.
    * if not, the histogram will be created on first TTree::Draw.
    * Strings varexp and selection are used for applying cuts and variables
    * to plot.
    */
   Bool_t AddTreeHistogram(const char* hisname, const char* treename, const char* varexp, const char* cutexp);


  /**
    * Printout of all histogram statistic counters on the terminal.
    * Expression may filter only objects with names matching.
    */
   void PrintHistograms(const char* expression=0);
   /**
    * Print all condition counters to the terminal.
    * Expression may filter only objects with names matching.
    */
   void PrintConditions(const char* expression=0);
/**
    * Print entries of current dynamic list.
    */
   void PrintDynamicList();


/**
    * Clear (reset) the specified objects. Method will first
    * clear all objects in any folder if it is matching the namestring.
    * If no such folder exists, object of name is searched and cleared.
    * Returns kFALSE in case of not found objects.
    */
   Bool_t ClearObjects(const char* name);


    /**
    * Change protection properties of object name as specified.
    * Flags may contain key letters like:
    * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
    * "+D"/"-D" to enable/disable protection against object deletion
    * For example flags="+C-D", "+C+D", "-D-C"
    * Properties not appearing in flags are not changed.
    */
   Bool_t ProtectObjects(const char* name, const Option_t* flags);

  /**
    * Remove entry of that name from dynamic list of listname.
    * The referred objects (histograms, conditions..) are still on heap, since
    * they are owned by their special folders. If listname is not given, the current
    * dynamic list is used.
    */
       Bool_t RemoveDynamicEntry(const char* entryname);


        /**
        * Get pointer to subfolder of parent specified by name. If create is true, create it
        */
      TFolder* FindSubFolder(TFolder* parent, const char* subfolder, Bool_t create=kTRUE);


      Bool_t CreatedInMake() const { return fbCreatedinMake; }

      /** Default name of the default (toplevel) dynamic list  */
      static const char* fgcTOPDYNAMICLIST;

      /** Top level folder name */
      static const char* fgcTOPFOLDER;

      /** Histograms folder name */
      static const char* fgcHISTFOLDER;

      /** Dynamic lists folder name */
      static const char* fgcDYNFOLDER;

      /** Analysis conditions folder name */
      static const char* fgcCONDFOLDER;

     /** Analysis user parameters folder name */
      static const char* fgcPARAFOLDER;

      /** Tree reference folder name */
      static const char* fgcTREEFOLDER;

      /** Picture objects folder name */
      static const char* fgcPICTFOLDER;

      /** TCanvas folder name */
      static const char* fgcCANVFOLDER;

      /** top analysis objects reference folder name */
      static const char* fgcANALYSISFOLDER;

      /** Event references folder name */
      static const char* fgcEVENTFOLDER;

      /** Event source reference folder name */
      static const char* fgcSRCFOLDER;

      /** Event store reference folder name */
      static const char* fgcSTOREFOLDER;

      /** Event processor reference folder name */
      static const char* fgcPROCFOLDER;

      /** User objects folder name */
      static const char* fgcUSRFOLDER;

      /** Temporary dummy folder name */
      static const char* fgcTMPFOLDER;

   private:

      /**
        * Top level Go4 Directory (root folder)
        */
      TFolder * fxGo4Dir;


      /**
       * Directory containing all histogram objects to be used by dynamic list
       * and user analysis.
       */
      TFolder * fxHistogramDir;

      /**
       * Directory containing all condition objects to be used by dynamic list
       * and by user analysis.
       */
      TFolder * fxConditionDir;

      /**
       * Directory containing user parameter objects
       */
      TFolder * fxParameterDir;

      /**
       * Directory containing all action objects to be used by dynamic list
       * and by user analysis.
       */
      TFolder * fxDynListDir;

      /**
       * Directory containing all user objects.
       */
      TFolder * fxUserDir;

      /**
       * Directory containing references to trees.
       */
      TFolder * fxTreeDir;

     /**
       * Directory containing the pictures.
       */
      TFolder * fxPictureDir;

 /**
       * Directory containing TCanvases.
       */
      TFolder * fxCanvasDir;

      /**
       * Directory containing references to event stores.
       */
      TFolder * fxStoreDir;

      /**
       * Directory containing references to event sources.
       */
      TFolder * fxSourceDir;

      /**
       * Directory containing references to event processors.
       */
      TFolder * fxProcessorDir;

      /**
       * Directory containing references to event structures.
       */
      TFolder * fxEventDir;

      /**
       * Top Directory for all references to event classes.
       */
      TFolder * fxAnalysisDir;

      /**
        * Temporary folder for nameslist objects.
        */
      TFolder * fxTempFolder;

      /**
          * Mutex protecting the object directories.
          */
      TMutex * fxDirMutex;


     /**
          * List of matching objects for name expression.
          */
      TList * fxMatchList;//!

      /** Iterator for list of matching objects. */
      TIterator * fxMatchIterator; //!


      /**
          * Event counter for dynamic list processing.
          * After each fgiDYNLISTINTERVAL counts the entries in the dynamic list
          * are updated with the previous events (e.g. loop over a tree for this range)
          */
      Int_t fiDynListCount;

      /**
        * Interval for dynamic list processing
        */
      Int_t fiDynListInterval;

     /**
       * Boolean flag indicates, if object was created
       * in last calls of one of Make* functions
       */
      Bool_t fbCreatedinMake;    //!

       /**
       * If this is set to true, any found histogram
       * will not be loaded from the file current TDirectory.
       * To be used to avoid Picture referenced Histograms to
       * appear in the Pictures folder
       */
      Bool_t fbSuppressLoadHistograms;    //!

    /**
       * Add any named object to a folder. Used by AddHistogram, AddParameter etc. methods.
       * If replace is true, old object of same name will be removed first.
       * If uniquename is true, there are no two objects of same name allowed even
       * in different subfolders (important for histograms registered to gROOT).
       * if resetbits is true, go4 deletion and reset protection bits are set to defaults.
       * Otherwise, object bits of ob are not touched.
       */
      Bool_t AddObjectToFolder(TObject * ob,
                               TFolder* fold,
                               const char* subfolder=0,
                               Bool_t replace=kTRUE,
                               Bool_t uniquename=kFALSE,
                               Bool_t resetbits=kTRUE);

  /**
       * Remove object specified by full name ("subfolder/subssubfolder/name") from folder fold.
       * If isDel flag is kTRUE, object will be deleted.
       * Return kTRUE if object was found and removed, otherwise kFALSE.
       * Used by RemoveHistogram etc. methods.
       */
      Bool_t RemoveObjectFromFolder(const char* fullname, TFolder* fold, Bool_t isDel);

  /**
       * Update folder destination with the content of the folder source. Subfolder
       * structure of source is recreated in destination, but existing objects are not
       * replaced/deleted unless replace==kTRUE
       */
      Bool_t LoadFolder(TFolder* source, TFolder* destination, Bool_t replace=kFALSE);

/**
       * Update folder destination with the content of the directory source. Subdirectory
       * structure of source is converted into subfolder of destination, but existing objects are not
       * replaced/deleted unless replace==kTRUE
       */
      Bool_t LoadFolder(TDirectory* source, TFolder* destination, Bool_t replace=kFALSE);

/**
       * Method used by both LoadFolder variants to assign object ob into destination
       * folder by object type. If replace is kTRUE, previous objects of same name
       * are replaced by ob.
       */
      Bool_t PutToFolder(TObject* ob, TFolder* destination, Bool_t replace=kFALSE);



 /**
       * Save this folder as a new subdirectory (TDirectory) of the current directory
       * i.e. the currently open file.
       * This is most likely more performant than streaming top folder completely into file
       */
      Bool_t SaveFolder(TFolder* source);

/**
       * Remove all objects in folder fold from directory dir, recursively.
       * Prevents the deletion of all objects in memory folders after SaveFolder,
       * when dir is closed.
       */
      void RemoveFromDir(TFolder* fold, TDirectory* dir);

/**
       * Append object ob to directory dir. If object of same name
       * already existed there, it is replaced.
       */
    void AppendToDir(TObject* ob, TDirectory* dir);


 /**
    * Printout all objects in folder fold on the terminal. Option for root Print can be specified.
    * Optionally, a filtering expression can be defined. Return value
    * gives total size of all printed objects in bytes (including subfolders).
    */
   Int_t PrintFolder(TFolder* fold, Option_t* opt, const char* expression=0);

/**
    * Clear (reset) all objects in folder fold, e.g. zero the histograms.
    * Objects are not deleted!
    */
   Bool_t ClearFolder(TFolder* fold);

/**
    * Clear (reset) the specified object. Depending on
    * type, different actions may happen (clear histo, zero graph)
    */
   Bool_t ClearObject(TObject* ob);


    /**
    * Delete all objects in folder fold only if the kGo4CanDelete bit
    * is set. Usually, only objects created from the gui may be deleted.
    */
   Bool_t DeleteFolder(TFolder* fold);

    /**
    * Delete the specified object if the kGo4CanDelete bit
    * is set. Usually, only objects created from the gui may be deleted.
    */
   Bool_t DeleteObject(TObject* ob);


    /**
    * Change protection properties of all objects in this folder as specified.
    * Flags may contain key letters like:
    * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
    * "+D"/"-D" to enable/disable protection against object deletion
    * For example flags="+C-D", "+C+D", "-D-C"
    * Properties not appearing in flags are not changed.
    */
    Bool_t ProtectFolder(TFolder* fold, const Option_t* flags);


   /**
    * Change protection properties of object name as specified.
    * Flags may contain key letters like:
    * "+C"/"-C" to enable/disable protection against Clear()(histogram zeroing etc).
    * "+D"/"-D" to enable/disable protection against object deletion
    * For example flags="+C-D", "+C+D", "-D-C"
    * Properties not appearing in flags are not changed.
    */
   Bool_t ProtectObject(TObject* ob, const Option_t* flags);


    /**
       * Remove reference to object from all dynamic lists.Object type is noticed automatically.
       */
     void CleanupDynamicLists(TObject* oldobject);

    /** Create a list of objects which names are matching expression expr.
      * optionally, the search can be limited to given subfolder of
      * Go4. By default, all registered objects are compared.*/
    TList* CreateObjectList(const char* expr, const char* folder=0);

    /** For recursive search for objects in folder fold that match expression. */
    TList* CreateObjectList(const char* expr, TFolder* fold);

    /** Finds out if  string is matching the expression */
    Bool_t IsMatching(const char* string, const char* expression);

    /** Serachs in folder for object with specified name
      * Uses fxDirMutex until search is working
      */
    TObject* FindObjectInFolder(TFolder* folder, const char* fullname);

    /** Test, if object exists in provided folder.
      * If object exists and corresponds to provided class,
      * it will be returned, otherwise it will be destroyed
      * Used by different Make* functions
      */

    TObject* TestObject(TFolder* folder,
                        const char* &pathname,
                        const char* objectname,
                        const TClass* cl);

};

#endif //TGO4ANALYSISOBJECTMANAGER_H
