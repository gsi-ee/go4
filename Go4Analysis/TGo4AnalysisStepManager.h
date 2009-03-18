#ifndef TGO4ANALYSISSTEPMANAGER_H
#define TGO4ANALYSISSTEPMANAGER_H

#include "TNamed.h"

class TIterator;
class TMutex;
class TObjArray;
class TFolder;

class TGo4EventProcessorParameter;
class TGo4EventSourceParameter;
class TGo4EventStoreParameter;
class TGo4EventElement;
class TGo4AnalysisStep;
class TGo4AnalysisStatus;
class TGo4Parameter;
class TGo4Condition;
class TGo4Fitter;

/**
 * This object is responsible for the
 * organisation of analysis steps. Keeps lists of steps. Is aggregate of TGo4Analysis.
 * @author J. Adamczewski
 * @since 07-May-2002
 */
class TGo4AnalysisStepManager : public TNamed {
   public:
      TGo4AnalysisStepManager(const char* name);

      virtual ~TGo4AnalysisStepManager();

      /** Uses the chain of analysis steps to process the first input
        * event into an output event which then is accessible at
        * the output event member. Returns number of
        * steps run. For internal usage in the MainCycle and
        * GetEvent methods; may also be called from UserEventFunc directly. */
      Int_t ProcessAnalysisSteps();

      /** Access to certain analysis step by name. For analysis internal usage. */
      TGo4AnalysisStep* GetAnalysisStep(const char* name);

      /** Method for user analysis constructor to setup the list
        * of analyisis steps. Each call of this method will add a new
        * analysis step at the end of the list. Caution: user has to take
        * care that output and input event types of subsequent steps
        * are matching! Analyisis steps cannot be removed, since the
        * steplist is not intended to be dynamic. This is a pure creational method.
        * Analysis steps are owned by the steplist once they have been added. */
      Bool_t AddAnalysisStep(TGo4AnalysisStep* next);

      /** Finish the analysis run and close all event sources/storages.
        * The analysis instance and the setup of the analysis steps is not deleted. */
      void CloseAnalysis();

      /** Initialization of the event class plugins which are delivered from
        * the user defined event factory. The virtual standard base class method may be changed
        * by overriding; normally, this is not necessary. */
      virtual Bool_t InitEventClasses();

      /** Update step specific part of analysis status object */
      void UpdateStatus(TGo4AnalysisStatus* state);

      /** Set step specific part of analysis status object to steplist */
      void SetStatus(TGo4AnalysisStatus * state);

      /** Save step specific parts. */
      void AutoSave();

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

      /**
       * Shuts down the old analysis step storage and creates a new
       * one specified by parameter par. To be used in online mode to
       * switch output files.
       */
      Bool_t NewStepStore(const Text_t * name, TGo4EventStoreParameter* par);

      /**
       * Shuts down the old analysis step event source and creates a new
       * one specified by parameter par. To be used in online mode to
       * switch input.
       */
      Bool_t NewStepSource(const Text_t * name, TGo4EventSourceParameter * par);

      /**
       * Shuts down the old analysis step processor and creates a new
       * one specified by parameter par. To be used in online mode to
       * switch event processing.
       */
      Bool_t NewStepProcessor(const Text_t * name, TGo4EventProcessorParameter * par);

      /**
       * Write object obj into eventstore of the step specified by name.
       * Current event number will be added to object name in the file.
       * Returns -1 in case of not active eventstore. Returns +1 in case
       * of unknown step.
       */
      Int_t Store(const Text_t * name, TGo4Parameter* obj);

      /**
       * Write condition con into eventstore of the step specified by name.
       * Current event number will be added to parameter name in the file.
       * Returns -1 in case of not active eventstore. Returns +1 in case
       * of unknown step.
       */
      Int_t Store(const Text_t * name, TGo4Condition* con);

     /**
       * Write fitter fit into eventstore of the step specified by name.
       * Current event number will be added to parameter name in the file.
       * Returns -1 in case of not active eventstore. Returns +1 in case
       * of unknown step.
       */
      Int_t Store(const Text_t * name, TGo4Fitter* fit);

    /**
       * Write folder  into eventstore of the step specified by name.
       * Current event number will be added to parameter name in the file.
       * Returns -1 in case of not active eventstore. Returns +1 in case
       * of unknown step.
       */
      Int_t Store(const Text_t * name, TFolder* folder);

      /**
          * Returns the input event structure of analysis step. Argument indicates
          * the step by name.
          * For internal usage in the user defined event function
          * UserEventFunc, and is used by analysis steps to get result of previous steps.
          */
      TGo4EventElement* GetInputEvent(const char* stepname);

      /**
       * Returns the input event structure of analysis step. Argument number indicates
       * the step from 0 (first) to n (last) in subsequent
       * order.
       * For internal usage in the user defined event function
       * UserEventFunc, and is used by analysis steps to access event sources of previous steps.
       */
      TGo4EventElement* GetInputEvent(Int_t stepindex);

      /**
       * Returns the output event (detector) structure of analysis step. Argument indicates
       * the step by name.
       * For internal usage in the user defined event function
       * UserEventFunc, and is used by analysis steps to get result of previous steps.
       */
      TGo4EventElement* GetOutputEvent(const char* stepname);

      /**
       * Returns the output event (detector) structure of analysis step. Argument number indicates
       * the step from 0 (first) to n (last) in subsequent
       * order. Default is the output event of the
       * currently processed step, which is set by
       * SetEvent method.
       * For internal usage in the user defined event function
       * UserEventFunc, and is used by analysis steps to get result of previous steps.
       */
      TGo4EventElement* GetOutputEvent(Int_t stepindex);

      /**
       * Returns the output event (detector) structure of analysis step.  Is set by
       * SetOutputEvent method. For internal usage in the user defined event function
       * UserEventFunc, and is used by analysis steps to get result of previous steps.
       */
      TGo4EventElement* GetOutputEvent() { return fxOutputEvent; }

      void SetOutputEvent(TGo4EventElement * event) {  fxOutputEvent=event; }

      Int_t IsErrorStopEnabled();

      void SetStepChecking(Bool_t on=kTRUE) { fbStepCheckingMode=on; }

      Bool_t IsStepChecking() { return fbStepCheckingMode; }

   private:

      /** Array containing all analysis steps. The steplist is iterated by
        * the maincycle from start to end index to process the active steps. */
      TObjArray* fxStepList;

      /** Mutex protecting the analysis step list. */
      TMutex* fxStepMutex;

      /** Iterator used for analysis step list. */
      TIterator* fxStepIterator; //!

      /** Index describing the first analysis step from the steplist
        * to be processed. The first analysis step will use its own event
        * source to get input events. */
      Int_t fiFirstStepIndex;

      /** Index describing the last  analysis step from the steplist
        * to be processed. Output event of the last analysis step is
        * accessible by GetEvent method and is used
        * for user event function and dynamic list. */
      Int_t fiLastStepIndex;

      /** Index describing the current  analysis step.
        * Used to check the first and last step to be processed. */
      Int_t fiCurrentStepIndex;

     /** True if chain of steps is checked for consistency of input and output events.
       * Default is true. Can be disabled for steps working in non-subsequent manner.
       * i.e. intermediate steps may be disabled, event structures of subsequent steps
       * need not match. */
      Bool_t fbStepCheckingMode;

      /** Points to the currently active analysis step.
       * @supplierCardinality 1 */
      TGo4AnalysisStep * fxCurrentStep;

      /** Link to the last output event produced by the event analysis steps.
        * Can be processed further by user event function and dynamic list. */
      TGo4EventElement* fxOutputEvent;
};

#endif //TGO4ANALYSISSTEPMANAGER_H
