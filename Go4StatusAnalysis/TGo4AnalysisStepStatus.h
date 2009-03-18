#ifndef TGO4ANALYSISSTEPSTATUS_H
#define TGO4ANALYSISSTEPSTATUS_H

#include "TGo4Status.h"

#include "TGo4EventProcessorParameter.h"
#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"

/**
 * Status object of one analysis step. Will be
 * aggregated in analysis status.
 * @since 1/2001
 */

class TGo4AnalysisStepStatus : public TGo4Status {

  public:
    TGo4AnalysisStepStatus();

    TGo4AnalysisStepStatus(const char* name);

    virtual ~TGo4AnalysisStepStatus();

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
   virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

    /** Sets the event source parameter which is used on intitialization
      * of this step. */
    void SetSourcePar(TGo4EventSourceParameter* kind);

    /** Returns the event source parameter which was
      * put into this status object. */
    TGo4EventSourceParameter* GetSourcePar() const { return fxSourceType; }

    /** Sets the event store parameter which is used on intitialization
      * of this step. */
    void SetStorePar(TGo4EventStoreParameter* kind);

    /** Returns the event store parameter which was
      * put into this status object. */
    TGo4EventStoreParameter * GetStorePar() const {  return fxStoreType; }

    /** Sets the event processor parameter
      * which is used on intitialization of this step. */
    void SetProcessorPar(TGo4EventProcessorParameter* kind);

    /** Returns the event processor parameter which was
      * put into this status object. */
    TGo4EventProcessorParameter * GetProcessorPar() const { return fxProcessorType; }

    /** Enables or disables the event source. */
    void SetSourceEnabled(Bool_t on=kTRUE) { fbSourceEnabled = on; }
    Bool_t IsSourceEnabled() const { return fbSourceEnabled; }

    /** Enables or disables the event store. */
    void SetStoreEnabled(Bool_t on=kTRUE) { fbStoreEnabled = on; }
    Bool_t IsStoreEnabled() const { return fbStoreEnabled; }

    /** Enables or disables the event processing. */
    void SetProcessEnabled(Bool_t on=kTRUE) { fbProcessEnabled = on; }
    Bool_t IsProcessEnabled() const { return fbProcessEnabled; }

    /** Enables or disables the stop-on-error mode. */
    void SetErrorStopEnabled(Bool_t on=kTRUE) { fbErrorStopEnabled = on; }
    /** True if analysis step shall stop on error. */
    Bool_t IsErrorStopEnabled() const { return fbErrorStopEnabled; }

    /** Enables or disables the stop-on-error mode. */
    void SetErrorStopped(Bool_t on) { fbErrorStopped = on; }
    /** True if analysis step has been stopped on error. */
    Bool_t IsErrorStopped() const { return fbErrorStopped; }

    /** Sets status value of processor, user defined. */
    void SetProcessStatus(Int_t val) { fiProcessStatus=val; }

    /** Returns status value of processor. Meaning
      * of value may be user defined. */
    Int_t GetProcessStatus() const { return fiProcessStatus; }

  private:

    /** Parameter object of next event source to be activated. Is used by
      * eventfactory as parameter for factory method. Meaning
      * is defined in actual user event factory.
      * @link aggregation */
    TGo4EventSourceParameter* fxSourceType;

    /** Parameter object of next event storage to be activated. Is used by
      * eventfactory as parameter for factory method. Meaning
      * is defined in actual user event factory.
      * @link aggregation */
    TGo4EventStoreParameter* fxStoreType;

    /** Parameter object of next event processor to be activated. Is used by
      * eventfactory as parameter for factory method. Meaning
      * is defined in actual user event factory.
      * @link aggregation */
    TGo4EventProcessorParameter* fxProcessorType;

    /** Enables event source.If true,
      * the event source is used to get the input event; otherwise
      * we use the output of the previous step. */
    Bool_t fbSourceEnabled;

    /** Enables event store. If true,
      * the event store is used as output; otherwise
      * we pass the output to the next step. */
    Bool_t fbStoreEnabled;

    /** Enables processing of the input event. If true,
      * Process method produces an output event from the input; otherwise
      * we do not fill the output. Useful if event source of this step shall be
      * used to pass the input event to another analysis step. If process is
      * disabled, also the eventstore is disabled. */
    Bool_t fbProcessEnabled;

    /** Operation mode switch. If true, the analysis
      * step process method will stop the analysis
      * if the event source returns an error code.
      * Otherwise, the event source may be reset then
      * and the analysis continues from start. */
    Bool_t fbErrorStopEnabled;

    /** True if the analysis step has been stopped
      * after input error. */
    Bool_t fbErrorStopped;

    /** Contains current analysis step status value. To be checked by framework after Process().*/
    Int_t fiProcessStatus;

  ClassDef(TGo4AnalysisStepStatus,1)
};

#endif //TGO4ANALYSISSTEPSTATUS_H
