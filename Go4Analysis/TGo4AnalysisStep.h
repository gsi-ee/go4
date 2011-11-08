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

#ifndef TGO4ANALYSISSTEP_H
#define TGO4ANALYSISSTEP_H

#include "TNamed.h"
#include "TString.h"

class TFolder;

class TGo4Parameter;
class TGo4Condition;
class TGo4Fitter;

class TGo4Analysis;
class TGo4AnalysisStepStatus;
class TGo4AnalysisStatus;
class TGo4Condition;
class TGo4Fitter;
class TGo4EventProcessorParameter;
class TGo4EventStoreParameter;
class TGo4EventSourceParameter;

class TGo4EventElement;
class TGo4EventFactory;
class TGo4EventSource;
class TGo4EventStore;
//class TGo4EventProcessor;
#include "TGo4EventProcessor.h"

/**
 * Contains a single step of the analysis. Each analysis step
 * has an event source, an input event structure,
 * an output event structure (both from event element class),
 * and an event store. Input event is converted to output event using
 * an event processor instance which is referenced by the output event
 * class, but not stored with the resulting data. Analysis steps are
 * held in an analysis step list which is processed iteratively; output
 * event of an analysis step is input event of the subsequent analysis step,
 * etc. The first analysis step takes its input event from an
 * external event source, e.g. a file; the last step writes the
 * output event into its external event store. Starting and ending
 * point within the chain of analysis steps may be changed on the fly.
 * @author J. Adamczewski
 * @since 11-Jan-2001
 */
class TGo4AnalysisStep : public TNamed {
  public:

    /** default ctor for streamer. */
    TGo4AnalysisStep();
    /** Normal ctor. Note: factory and parameter objects are adopted by analysis step,
      * i.e. they are deleted by dtor. */
    TGo4AnalysisStep(const char* name,
                     TGo4EventFactory* eventfactory,
                     TGo4EventSourceParameter* sourcetype = 0,
                     TGo4EventStoreParameter* storetype = 0,
                     TGo4EventProcessorParameter* processortype = 0);

    virtual ~TGo4AnalysisStep();

    /** Sets the event source parameter which is used on initialization
      * of this step. The event source will not yet be created;
      * Creation is done by InitEventClasses() method, or by calling
      * the NewEventSource() method on the fly. */
    void SetEventSource(TGo4EventSourceParameter* kind);

    /** Return kTRUE, if event source parameter specified */
    Bool_t IsEventSourceParam() const;

    /** For lazy initialization of event source. Uses
      * the CreateEventSource method of the specialized event factory.
      * Is called from init eventclass in the timer initialization of
      * the analysis, or may called on the fly to switch the
      * source from event server to file. Parameter is passed to
      * factory method for user specification of source. */
    void NewEventSource(TGo4EventSourceParameter * kind);

    /** Return current event source parameter */
    TGo4EventSourceParameter* GetEventSource() const { return fxSourceType; }

    /** Unregister and close eventsource, if existing. */
    void CloseEventSource();

    /** Sets the event store parameter which is used on initialization
      * of this step. The event store will not yet be created;
      * Creation is done by InitEventClasses() method, or by calling
      * the NewEventStore() method on the fly. */
    void SetEventStore(TGo4EventStoreParameter* kind);

    /** Return current event store parameter */
    TGo4EventStoreParameter* GetEventStore() const { return fxStoreType; }

    /** Return kTRUE, if event store parameter specified */
    Bool_t IsEventStoreParam() const;

    /** For lazy initialization of event store. Uses
      * the CreateEventStore method of the specialized event factory.
      * Is called from initeventclasses in the timer initialization of
      * the analysis, or may called on the fly to switch the
      * store. Parameter is passed to
      * factory method for user specification of store. */
    void NewEventStore(TGo4EventStoreParameter * kind);

    /** Unregister and close eventstore, if existing. */
    void CloseEventStore();

    /** Sets the event processor parameter which is used on initialization
      * of this step. The event processor will not yet be created;
      * Creation is done by InitEventClasses() method, or by calling
      * the NewEventProcessor() method on the fly. */
    void SetEventProcessor(TGo4EventProcessorParameter* kind);

    /** For lazy initialization of event processor. Uses
      * the CreateEventProcessor method of the specialized event factory.
      * Is called from initeventclasses in the timer initialization of
      * the analysis, or may called on the fly to switch the
      * processing algorithm. Parameter is passed to
      * factory method for user specification of processor. */
    void NewEventProcessor(TGo4EventProcessorParameter * kind);

    /** Unregister and close eventprocessor, if existing. */
    void CloseEventProcessor();

    /** create input event object */
    void NewInputEvent();

    /** Delete input event object */
    void DeleteInputEvent();

    /** create input event object */
    void NewOutputEvent();

    /** Delete input event object */
    void DeleteOutputEvent();

    /** Initialization of the event class plugins which are delivered from
      * the user defined event factory. Parameters are ids for the implementation kind */
    virtual void InitEventClasses();

    /** Enables or disables the event source. */
    void SetSourceEnabled(Bool_t on=kTRUE) { fbSourceEnabled=on; }

    /** Enables or disables the event store. */
    void SetStoreEnabled(Bool_t on=kTRUE) { fbStoreEnabled=on; }

    /** Enables or disables the event processing. */
    void SetProcessEnabled(Bool_t on=kTRUE) { fbProcessEnabled=on; }

    /** Enables or disables the stop-on-error mode. */
    void SetErrorStopEnabled(Bool_t on) { fbErrorStopEnabled=on; }

    /** Reset the stopped-after-error state of this
      * analysis step object. May be used to restart
      * analysis after clearing the situation. */
    void ResetErrorStop() { fbErrorStopped=0; }

    /** Sets reference to external input event. The processor will use
      * this input event at the next call of process. I this analysis
      * step is the first in the chain, the input event will be taken from
      * the event source of this step. */
    void SetInputEvent(TGo4EventElement* in) { fxInputEvent=in; }

    /** Sets reference to previous analysis step. */
    void SetPreviousStep(TGo4AnalysisStep * pre) { fxPrevious=pre; }

    /** Get previous analysis step. Returns 0 if there is none. */
    TGo4AnalysisStep* GetPreviousStep() const { return fxPrevious; }

    /** Access to the output event which has been filled last. Is used to
      * pass the output to the next analysis step, or to the main analysis
      * user event function. */
    TGo4EventElement* GetOutputEvent() const { return fxOutputEvent; }

    /** Access to the input event of this step. Useful to
      * pass incoming event from event source to a
      * subsequent step or to user event function,
      * without processing it. */
    TGo4EventElement* GetInputEvent() const { return fxInputEvent; }

    /** Access to the event processor.
      * Useful if processor has own data to be displayed, e.g. histograms. */
    TGo4EventProcessor * GetEventProcessor() const { return fxEventProcessor; }

    /** Access to step factory.
      * Useful in setup macros */
    TGo4EventFactory * GetStepFactory() const { return fxEventFactory; }

    Bool_t IsStoreEnabled() const { return fbStoreEnabled; }

    Bool_t IsStoreImplemented() const { return fbStoreImplemented; }

    Bool_t IsSourceImplemented() const { return fbSourceImplemented; }

    Bool_t IsSourceEnabled() const { return fbSourceEnabled; }

    Bool_t IsProcessEnabled() const { return fbProcessEnabled; }


    Bool_t IsKeepInputEvent() { return fxEventProcessor->IsKeepInputEvent();}

    Bool_t IsKeepOutputEvent() { return fxEventProcessor->IsKeepOutputEvent();}

    /** True if analysis step has been stopped on error. */
    Bool_t IsErrorStopped() const { return fbErrorStopped; }

    /** True if analysis step allows stop on error. */
    Bool_t IsErrorStopEnabled() const { return fbErrorStopEnabled; }

    /** Status of the last event. */
    Int_t GetProcessStatus() const { return fiProcessStatus; }

    /** Status message of last Process() call. */
    const char* GetStatusMessage() const { return fcStatusMessage.Data(); }

    /** Set Status message of last Process() call. */
    void SetStatusMessage(const char* txt) { fcStatusMessage = txt; }

    /** Check if an analysis step is matching as a previous step to this
      * step. Will compare by the classnames of input and output events
      * as created by the event factory. To be used at the end of
      * InitEventClasses method of analysis to ensure that chain of
      * analysis steps would be working. */
    Bool_t IsMatchingPrevious();

    /** Access to name of currently active event store. Used to avoid collisions between
      * store and source (files) of subsequent steps. Returns 0 pointer if there is no store implemented. */
    const char* GetEventStoreName();

    /** Access to name of currently active event source.
     * in case of listmode list, this should give currently open file!*/
    const char* GetEventSourceName();

    /** Store the current calibration object of the
      * event processor into the event store instance. */
    void StoreCalibration();

    /** Saves a parameter correlated with the current event into the storage. */
    Int_t Store(TGo4Parameter* cali);

    /** Saves a condition correlated with the current event into the storage. */
    Int_t Store(TGo4Condition* conny);

    /** Saves a fitter correlated with the current event into the storage. */
    Int_t Store(TGo4Fitter* fitter);

    /** Saves a folder correlated with the current event into the storage. */
    Int_t Store(TFolder* fold);

    /** The main analysis action which is invoked by the external
      * go4 analysis. Reads input event from source (if first step),
      * fills the output event by means of the event processor, and
      * stores the output event (if last step). */
    void Process();

    /** Closes the analysis step. Closes all files and connections
      * and frees memory. */
    void Close();

    /** Set all analysis step parameters to that of given status object. */
    void SetStatus(TGo4AnalysisStepStatus * state);

    /** Create a copy of the analysis step internal state. To be sent to the Display. */
    TGo4AnalysisStepStatus * CreateStatus();

  private:

    /** Points back to the analysis wich keeps this step. */
    TGo4Analysis* fxOwner;                       //!

    /** Points to the previous analysis step. Useful to check
      * the corresponding input and output event types, and the event source state. */
    TGo4AnalysisStep * fxPrevious;               //!

    /** The abstract factory implementation which provides the event classes.
      * @link aggregationByValue
      *   @supplierCardinality 1 */
    TGo4EventFactory * fxEventFactory;

    /** The storage instance for the unpacked events (detector) structures.
      * @link aggregation
      *   @supplierCardinality 1 */
     TGo4EventStore * fxEventStore;

    /** The source of the input events. Can be a remote event server, or
      * a root file, or anything else.
      * @link aggregation
      *   @supplierCardinality 1 */
    TGo4EventSource * fxEventSource;

    /** The processing class which works on the input event and
      * fills the output event.
      * @link aggregation
      *   @supplierCardinality 1 */
    TGo4EventProcessor * fxEventProcessor;

    /** points to the last input event delivered from the event source
     * @supplierCardinality 1
     * @link aggregation*/
    TGo4EventElement * fxInputEvent;

    /** The unpacked event (detector) structure that has been filled  by the
     * analysis step .
     * @supplierCardinality 1
     * @link aggregation*/
    TGo4EventElement * fxOutputEvent;

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

    /** Flag for the previous analysis step. If true, the event source instance exists, i.e. a file may be
      * open. Usage is optional so far. */
    Bool_t fbSourceImplemented;

    /** Enables event store. If true,
      * the event store is used as output; otherwise
      * we pass the output to the next step. */
    Bool_t fbStoreEnabled;

    /** Flag for the subsequent analysis step.
      * If true, the event store instance exists, i.e. a file may be open. */
    Bool_t fbStoreImplemented;

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

    /** True if the analysis step has been stopped after input error. */
    Bool_t fbErrorStopped;

    /** Contains current analysis step status value. To be checked by framework after Process().*/
    Int_t fiProcessStatus;

    /** Most recent status message. May be delivered by eventsource or eventstore. */
    TString fcStatusMessage;   //!

  ClassDef(TGo4AnalysisStep,1)
};

#endif //TGO4ANALYSISSTEP_H
