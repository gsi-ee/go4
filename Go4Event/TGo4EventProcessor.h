#ifndef TGO4EVENTPROCESSOR_H
#define TGO4EVENTPROCESSOR_H

#include "TGo4EventSource.h"
#include <math.h>

class TH1;
class TH1D;
class TH1F;
class TH1S;
class TH1I;
class TH2;
class TH2D;
class TH2F;
class TH2S;
class TH2I;
class TGraph;
class TMultiGraph;
class TCanvas;

class TGo4Condition;
class TGo4WinCond;
class TGo4PolyCond;
class TGo4CondArray;
class TGo4Picture;
class TGo4Parameter;

class TGo4MbsEvent;
class TGo4EventElement;
class TGo4EventCalibration;

/** Abstract Factory used by the top level user event to unpack
 * the raw event into subevents or detector information.
 * Factory method implementations are user specific; there
 * might be more than one factory method in this class, each
 * dedicated for one special subevent. Default factory method is
 * CreateSubElement
 * @interface
 * @author J. Adamczewski
 * @since 1/2001 */

class TGo4EventProcessor : public TGo4EventSource {
  friend class TGo4EventElement;

  public:

    TGo4EventProcessor(const char* name);


   ~TGo4EventProcessor();

   /**
       * Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
   void SetInputEvent(TGo4EventElement * raw)
      {
         fxInputEvent=raw;
      }

   /**
    * Access to external raw event which is set as association member.
    */
   TGo4EventElement * GetInputEvent()
      {
         return fxInputEvent;
      }

   /**
    * Check if input event is valid. Tests the pointer for
    * zero, and if not zero, compares classname of the
    * input event with given string. To be used from the
    * Fill method and from the analysis method  init event classes.
    */
   virtual Bool_t CheckInputEvent(const char* classname);

   /**
       * Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
   void SetCalibration(TGo4EventCalibration * cali)
      {
         fxCalibration=cali;
      }

   /**
    * Access to external calibration object.
    */
   TGo4EventCalibration * GetCalibration()
      {
         return fxCalibration;
      }

   /**
       * Check if calibration class  is valid. Tests the pointer for
       * zero, and if not zero, compares classname of the
       * calibration instance with given string. To be used from the
       * Fill method and from the analysis method  init event classes.
       */
   virtual Bool_t CheckCalibration(const char* classname);


   /** Register user object to go4 framework. Shortcut forwarded to analysis instance */
   Bool_t AddObject(TNamed * anything, const Text_t* subfolder=0);

    /** Register histogram to go4 framework. Shortcut forwarded to analysis instance */
    Bool_t AddHistogram(TH1* his, const Text_t* subfolder=0, Bool_t replace=kTRUE);

    /** Register paramter to go4 framework. Shortcut forwarded to analysis instance */
    Bool_t AddParameter(TGo4Parameter* par, const Text_t* subfolder=0);

    /** Register condition to go4 framework. Shortcut forwarded to analysis instance */
    Bool_t  AddAnalysisCondition(TGo4Condition* con, const Text_t* subfolder=0);

   /** Register picture to go4 framework. Shortcut forwarded to analysis instance */
    Bool_t AddPicture(TGo4Picture* pic, const Text_t* subfolder=0);

  /** Register TCanvas to go4 framework. Shortcut forwarded to analysis instance */
    Bool_t AddCanvas(TCanvas* can, const Text_t* subfolder=0);


    /** UnRegister histogram from go4 framework and delete it. Shortcut forwarded to analysis instance */
    Bool_t RemoveHistogram(const Text_t* name);

    /** UnRegister parameter from go4 framework and delete it. Shortcut forwarded to analysis instance */
    Bool_t RemoveParameter(const Text_t* name);

    /** UnRegister condition from go4 framework and delete it. Shortcut forwarded to analysis instance */
    Bool_t RemoveAnalysisCondition(const Text_t* name);

    /** UnRegister picture from go4 framework and delete it. Shortcut forwarded to analysis instance */
    Bool_t RemovePicture(const Text_t* name);

  /** UnRegister canvas from go4 framework and delete it. Shortcut forwarded to analysis instance */
    Bool_t RemoveCanvas(const Text_t* name);

   /** Get any object from go4 framework. Shortcut forwarded to analysis instance */
   TNamed * GetObject(const Text_t* name, const Text_t* folder=0);

   /** Get histogram from go4 framework. Shortcut forwarded to analysis instance */
    TH1* GetHistogram(const Text_t* name);

    /** Get paramter from go4 framework. Shortcut forwarded to analysis instance */
     TGo4Parameter* GetParameter(const Text_t* name);

    /** Get condition from go4 framework. Shortcut forwarded to analysis instance */
    TGo4Condition*  GetAnalysisCondition(const Text_t* name);

    /** Get picture from go4 framework. Shortcut forwarded to analysis instance */
     TGo4Picture* GetPicture(const Text_t* name);

 /** Get TCanvas from go4 framework. Shortcut forwarded to analysis instance */
     TCanvas* GetCanvas(const Text_t* name);

  /**
       * Returns the input event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
   TGo4EventElement* GetInputEvent(const Text_t* stepname);

 /**
       * Returns the output event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
   TGo4EventElement* GetOutputEvent(const Text_t* stepname);

   /**
    * Display a user message. Argument pri defines message priority:
    * <0: display raw text on local terminal like plain printf
    * 0: display on local terminal only;  if debug output is enabled
    * 1: local terminal and info message in gui log panel (if gui mode)
    * 2: local terminal and warning message in gui log panel (if gui mode)
    * 3: local terminal and error message in gui log panel (if gui mode)
    */
   void Message(Int_t prio, const Text_t* text,...);

   /** Send any object as copy to gui. Shortcut forwarded to analysis instance */
    void SendObjectToGUI(TNamed* ob);

      /* We overwrite the default TNamed::Clear that would
      *  erase our name and title!
      * Implement this method in your parameter class
      * if you would like to reset any values with the
      * eraser button in the gui remote browser*/
      virtual void Clear(Option_t* opt="");

  protected:

   /** Default ctor for root streamer. */
   TGo4EventProcessor(){}

  private:

   /**@supplierCardinality 1 */
   TGo4EventElement * fxInputEvent; //!

   /** @link aggregation */
   TGo4EventCalibration * fxCalibration;
  ClassDef(TGo4EventProcessor,2)
};

#endif //TGO4EVENTPROCESSOR_H
