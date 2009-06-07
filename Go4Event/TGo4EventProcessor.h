#ifndef TGO4EVENTPROCESSOR_H
#define TGO4EVENTPROCESSOR_H

#include "TGo4EventSource.h"

#include <math.h>

class TH1;
class TH1D;
class TH1F;
class TH1S;
class TH1I;
class TH1C;
class TH2;
class TH2D;
class TH2C;
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
      Bool_t AddObject(TNamed * anything, const char* subfolder=0);

      /** Register histogram to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddHistogram(TH1* his, const char* subfolder=0, Bool_t replace=kTRUE);

      /** Register parameter to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddParameter(TGo4Parameter* par, const char* subfolder=0);

      /** Register condition to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t  AddAnalysisCondition(TGo4Condition* con, const char* subfolder=0);

      /** Register picture to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddPicture(TGo4Picture* pic, const char* subfolder=0);

      /** Register TCanvas to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddCanvas(TCanvas* can, const char* subfolder=0);


      /** UnRegister histogram from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveHistogram(const char* name);

      /** UnRegister parameter from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveParameter(const char* name);

      /** UnRegister condition from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveAnalysisCondition(const char* name);

      /** UnRegister picture from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemovePicture(const char* name);

      /** UnRegister canvas from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveCanvas(const char* name);

      /** Get any object from go4 framework. Shortcut forwarded to analysis instance */
      TNamed * GetObject(const char* name, const char* folder=0);

      /** Get histogram from go4 framework. Shortcut forwarded to analysis instance */
      TH1* GetHistogram(const char* name);

      /** Get parameter from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Parameter* GetParameter(const char* name);

      /** Get condition from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Condition*  GetAnalysisCondition(const char* name);

      /** Get picture from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Picture* GetPicture(const char* name);

      /** Get TCanvas from go4 framework. Shortcut forwarded to analysis instance */
      TCanvas* GetCanvas(const char* name);

      /**
       * Returns the input event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement* GetInputEvent(const char* stepname);

      /**
       * Returns the output event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement* GetOutputEvent(const char* stepname);

      /**
       * Display a user message. Argument pri defines message priority:
       * <0: display raw text on local terminal like plain printf
       * 0: display on local terminal only;  if debug output is enabled
       * 1: local terminal and info message in gui log panel (if gui mode)
       * 2: local terminal and warning message in gui log panel (if gui mode)
       * 3: local terminal and error message in gui log panel (if gui mode)
       */
      void Message(Int_t prio, const char* text,...);

      /** Send any object as copy to gui. Shortcut forwarded to analysis instance */
      void SendObjectToGUI(TNamed* ob);

      /* We overwrite the default TNamed::Clear that would
       *  erase our name and title!
       * Implement this method in your parameter class
       * if you would like to reset any values with the
       * eraser button in the gui remote browser*/
      virtual void Clear(Option_t* opt="");

      /** Set flag to use data from autosave file in Make... methods (default true) */
      void SetMakeWithAutosave(Bool_t on = kTRUE) { fbMakeWithAutosave = on; }

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
      TH1* MakeH1(char type, const char* fullname, const char* title,
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
       * xtitle, ytitle - title for X and Y axis of histogram
       * If histogram exactly with same name and type already exists in  autosave file,
       * it will be return. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
       */
      TH2* MakeH2(char type, const char* fullname, const char* title,
                   Int_t nbinsx, Double_t xlow, Double_t xup,
                   Int_t nbinsy, Double_t ylow, Double_t yup,
                   const char* xtitle = 0, const char* ytitle = 0);

      /** Create window condition.
       * fullname specifies name of condition (optionally with subfolder name)
       * xmin, xmax - condition range
       * HistoName - name of histogram, to which condition is assigned
       */
      TGo4WinCond* MakeWinCond(const char* fullname,
                               Double_t xmin, Double_t xmax,
                               Double_t ymin = 0., Double_t ymax = 0.,
                               const char* HistoName = 0);

      /** Create polygon condition.
       * fullname specifies name of condition (optionally with subfolder name)
       * npoints - number of points in in polygon condition
       * points - (X,Y) points
       * HistoName - name of histogram, to which condition is assigned
       * To use method, array should be declared as following:
       * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
       * cond = MakePolyCond("Folder/CondName", 4, points);
       */
      TGo4PolyCond* MakePolyCond(const char* fullname,
                                   Int_t npoints,
                                   Double_t (*points) [2],
                                   const char* HistoName = 0);

   protected:

      /** Default ctor for root streamer. */
      TGo4EventProcessor(){}

   private:

      /**@supplierCardinality 1 */
      TGo4EventElement * fxInputEvent; //!

      /** @link aggregation */
      TGo4EventCalibration * fxCalibration;

      /** If false, do not use data from autosave file in Make... methods */
      Bool_t fbMakeWithAutosave; //!

   ClassDef(TGo4EventProcessor,2)
};

#endif //TGO4EVENTPROCESSOR_H
