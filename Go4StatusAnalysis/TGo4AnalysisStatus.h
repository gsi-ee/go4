#ifndef TGO4ANALYSISSTATUS_H
#define TGO4ANALYSISSTATUS_H

#include "TGo4Status.h"
#include "TString.h"

class TIterator;
class TMutex;
class TGo4AnalysisStepStatus;
class TObjArray;

/**
 * Status of the analysis instance. Can be sent to the display or controlling
 * server to check the things.
 * @since 1/2001
 */
class TGo4AnalysisStatus : public TGo4Status {

  friend class TGo4Analysis;

  public:

    /** default ctor for streamer. */
    TGo4AnalysisStatus();

    TGo4AnalysisStatus(const char* name);

    virtual ~TGo4AnalysisStatus();

    void SetStepChecking(Int_t on) { fbStepCheckingMode=on; }
    Int_t IsStepChecking() const { return fbStepCheckingMode; }

    void SetFirstStepIndex(Int_t i) { fiFirstStepIndex=i; }
    Int_t GetFirstStepIndex() const { return fiFirstStepIndex; }

    void SetLastStepIndex(Int_t i) { fiLastStepIndex=i; }
    Int_t GetLastStepIndex() const { return fiLastStepIndex; }

    void SetAutoSaveInterval(Int_t i) { fiAutoSaveInterval=i; }
    Int_t GetAutoSaveInterval() const { return fiAutoSaveInterval; }

    void SetAutoSaveCompression(Int_t i=5) { fiAutoSaveCompression=i; }
    Int_t GetAutoSaveCompression() const { return fiAutoSaveCompression; }

    void SetAutoSaveOverwrite(Bool_t over=kTRUE) { fbAutoSaveOverwrite = over ? 1 : 0; }
    Int_t IsAutoSaveOverwrite() const { return fbAutoSaveOverwrite; }

    void SetAutoFileName(const char* name) { fxAutoFileName = name; }
    const char* GetAutoFileName() const { return fxAutoFileName.Data(); }

    void SetConfigFileName(const char* name) { fxConfigFileName = name; }
    const char* GetConfigFileName() const { return fxConfigFileName.Data(); }

    void SetAutoSaveOn(Bool_t on=kTRUE) { fbAutoSaveOn=on; }
    Int_t IsAutoSaveOn() const { return fbAutoSaveOn; }

    /** Access to certain analysis step status by name. */
    TGo4AnalysisStepStatus* GetStepStatus(const char* name);

    /** Add new step status object to analysis status. */
    Bool_t AddStepStatus(TGo4AnalysisStepStatus * next);

    void ResetStepIterator();

    /** Gives next analysis step status. */
    TGo4AnalysisStepStatus * NextStepStatus();

    /** Gives number of steps. */
    Int_t GetNumberOfSteps();

    /** Gives analysis step status with provided index. */
    TGo4AnalysisStepStatus* GetStepStatus(Int_t indx);

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

  private:

    /** Array containing all analysis steps. The steplist is iterated by
      * the maincycle from start to end index to process the active steps. */
    TObjArray* fxStepArray;

    /** Mutex protecting the analysis step list. */
    TMutex * fxStepMutex; //!

    /** Iterator used for analysis step list. */
    TIterator* fxStepIterator; //!

    /** True if chain of steps is checked for consistency of input and output events.
      * Default is true. Can be disabled for steps working in non-subsequent manner.
      * i.e. intermediate steps may be disabled, event structures of subsequent steps
      * need not match.  */
    Int_t fbStepCheckingMode;

    /** Index describing the first analysis step from the steplist
      * to be processed. The first analysis step will use its own event
      * source to get input events. */
    Int_t fiFirstStepIndex;

    /** Index describing the last  analysis step from the steplist
      * to be processed. Output event of the last analysis step is
      * accessible by GetEvent method and is used
      * for user event function and dynamic list. */
    Int_t fiLastStepIndex;

    /** Implicit Loop cycles in between two autosave actions. Autosave will write
      * calibration objects of steps into event file and will save all histograms into an default
      * root file. */
    Int_t fiAutoSaveInterval;

    /** compression level for autosave file */
    Int_t fiAutoSaveCompression;

    /** overwrite mode for autosave yes or no.
      * Internally, we use Int_t for correct streamer! */
    Int_t fbAutoSaveOverwrite;

    /** True if Autosaving is enabled (default).
      * If false, calling AutoSave() will have no effect. */
    Int_t fbAutoSaveOn;

    /** Name of the autosave file. */
    TString fxAutoFileName;

    /** Name of the last configuration file. */
    TString fxConfigFileName;

  ClassDef(TGo4AnalysisStatus,2)

};

#endif //TGO4ANALYSISSTATUS_H
