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

#ifndef TGO4EVENTPROCESSOR_H
#define TGO4EVENTPROCESSOR_H

#include "TGo4EventSource.h"

#include <cmath>

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
class TF1;
class TCanvas;

class TGo4Condition;
class TGo4WinCond;
class TGo4PolyCond;
class TGo4ShapedCond;
class TGo4ListCond;
class TGo4RollingGraph;

#ifndef TGo4EllipseCond
#define TGo4EllipseCond TGo4ShapedCond
#endif

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
 * @interface TGo4EventProcessor
 * @author J. Adamczewski
 * @since 1/2001 */

class TGo4EventProcessor: public TGo4EventSource {
      friend class TGo4EventElement;

   public:

      TGo4EventProcessor(const char *name);

      virtual ~TGo4EventProcessor();

      /**
       * Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
      void SetInputEvent(TGo4EventElement *raw) { fxInputEvent = raw; }

      /**
       * Access to external raw event which is set as association member.
       */
      inline TGo4EventElement *GetInputEvent() { return fxInputEvent; }

      /**
       * Check if input event is valid. Tests the pointer for
       * zero, and if not zero, compares classname of the
       * input event with given string. To be used from the
       * Fill method and from the analysis method  init event classes.
       */
      virtual Bool_t CheckInputEvent(const char *classname);

      /**
       * Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
      void SetCalibration(TGo4EventCalibration *cali) { fxCalibration = cali; }

      /**
       * Access to external calibration object.
       */
      TGo4EventCalibration *GetCalibration() const { return fxCalibration; }

      /**
       * Check if calibration class  is valid. Tests the pointer for
       * zero, and if not zero, compares classname of the
       * calibration instance with given string. To be used from the
       * Fill method and from the analysis method  init event classes.
       */
      virtual Bool_t CheckCalibration(const char *classname);

      /* implemented by subclass to assign correct event structures*/
      virtual void InitEvent(TGo4EventElement *) {}

      /* implemented by subclass to do actual processing. For second step processors!*/
      virtual void FinalizeEvent() {}

      /** Register user object to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddObject(TNamed *anything, const char *subfolder = nullptr);

      /** Register histogram to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddHistogram(TH1 *his, const char *subfolder = nullptr, Bool_t replace = kTRUE);

      /** Register parameter to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddParameter(TGo4Parameter *par, const char *subfolder = nullptr);

      /** Register condition to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddAnalysisCondition(TGo4Condition *con, const char *subfolder = nullptr);

      /** Register picture to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddPicture(TGo4Picture *pic, const char *subfolder = nullptr);

      /** Register TCanvas to go4 framework. Shortcut forwarded to analysis instance */
      Bool_t AddCanvas(TCanvas *can, const char *subfolder = nullptr);

      /** UnRegister histogram from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveHistogram(const char *name);

      /** UnRegister parameter from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveParameter(const char *name);

      /** UnRegister condition from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveAnalysisCondition(const char *name);

      /** UnRegister picture from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemovePicture(const char *name);

      /** UnRegister canvas from go4 framework and delete it. Shortcut forwarded to analysis instance */
      Bool_t RemoveCanvas(const char *name);

      /** Get any object from go4 framework. Shortcut forwarded to analysis instance */
      TNamed *GetObject(const char *name, const char *folder = nullptr);

      /** Get histogram from go4 framework. Shortcut forwarded to analysis instance */
      TH1 *GetHistogram(const char *name);

      /** Get parameter from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Parameter *GetParameter(const char *name, const char *par_class = nullptr);

      /** Get condition from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Condition *GetAnalysisCondition(const char *name, const char *cond_cl = nullptr);

      /** Get picture from go4 framework. Shortcut forwarded to analysis instance */
      TGo4Picture *GetPicture(const char *name);

      /** Get TCanvas from go4 framework. Shortcut forwarded to analysis instance */
      TCanvas *GetCanvas(const char *name);

      /**
       * Returns the input event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement *GetInputEvent(const char *stepname);

      /**
       * Returns the output event structure of analysis step. Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement *GetOutputEvent(const char *stepname);

      /**
       * Display a user message. Argument pri defines message priority:
       * <0: display raw text on local terminal like plain printf
       * 0: display on local terminal only;  if debug output is enabled
       * 1: local terminal and info message in gui log panel (if gui mode)
       * 2: local terminal and warning message in gui log panel (if gui mode)
       * 3: local terminal and error message in gui log panel (if gui mode)
       */
      void Message(Int_t prio, const char *text, ...)
       #if defined(__GNUC__) && !defined(__CINT__)
         __attribute__((format(printf, 3, 4)))
       #endif
      ;

      /** Send any object as copy to gui. Shortcut forwarded to analysis instance */
      void SendObjectToGUI(TNamed *ob);

      /* We overwrite the default TNamed::Clear that would
       *  erase our name and title!
       * Implement this method in your parameter class
       * if you would like to reset any values with the
       * eraser button in the gui remote browser*/
      void Clear(Option_t *opt = "") override;

      /** Set flag to use data from autosave file in Make... methods (default true) */
      void SetMakeWithAutosave(Bool_t on = kTRUE);

      /** Returns kTRUE if object in last Make... call was created,
       * kFALSE when object was retrieved from autosavefile */
      Bool_t IsObjMade() const;

      /** Create one dimensional histogram of specified type. Type can be:
       * 'I', 'i' for TH1I - Int_t as bin content (default)
       * 'F', 'f' for TH1F - Float_t as bin content
       * 'D', 'd' for TH1D - Double_t as bin content
       * 'S', 's' for TH1S - Short_t as bin content
       * 'C', 'c' for TH1C - Char_t as bin content
       * 'L', 'l' for TH1L - Long64_t as bin content
       * \param fullname specifies name of histogram (optionally with subfolder name)
       * \param title - histogram title
       * \param nbinsx, \param xlow, \param xup - range arguments for X axis
       * \param xtitle, \param ytitle - title for X and Y axis of histogram
       * If histogram exactly with same name and type already exists in  autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
       */
      TH1 *MakeTH1(char type, const char *fullname, const char *title,
                   Int_t nbinsx, Double_t xlow, Double_t xup,
                   const char *xtitle = nullptr, const char *ytitle = nullptr);

      /** Create two dimensional histogram of specified type. Type can be:
       * 'I', 'i' for TH2I - Int_t as bin content (default)
       * 'F', 'f' for TH2F - Float_t as bin content
       * 'D', 'd' for TH2D - Double_t as bin content
       * 'S', 's' for TH2S - Short_t as bin content
       * 'C', 'c' for TH2C - Char_t as bin content
       * 'L', 'l' for TH2L - Long64_t as bin content
       * \param fullname specifies name of histogram (optionally with subfolder name)
       * \param title - histogram title
       * \param nbinsx, \param xlow, \param xup - range arguments for X axis
       * \param nbinsy, \param ylow, \param yup - range arguments for Y axis
       * \param xtitle, \param ytitle, \param ztitle - title for X, Y and Z axis of histogram
       * If histogram exactly with same name and type already exists in  autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
       */
      TH2 *MakeTH2(char type, const char *fullname, const char *title,
                   Int_t nbinsx, Double_t xlow, Double_t xup,
                   Int_t nbinsy, Double_t ylow, Double_t yup,
                   const char *xtitle = nullptr, const char *ytitle = nullptr, const char *ztitle = nullptr);

      /** Create a TGraph with initial values as specified by points, xvalues and yvalues.
       * If theses parameters are missing, an empty graph is created to be specified by the user.
       * If a graph of this name already exists in the autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGraph *MakeGraph(const char *fullname, const char *title, Int_t points = 0, Double_t *xvalues = nullptr,
                        Double_t *yvalues = nullptr);

      /** Create a TGraph with values initialized by a function object TF1l.
        * If a graph of this name already exists in the autosave file,
        * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGraph *MakeGraph(const char *fullname, const char *title, TF1 *function);

      /** Create a go4 rolling graph (generic trending plot) with properties points and average.
        * If theses parameters are missing, an empty graph is created to be specified by the user.
        * If a rolling graph of this name already exists in the autosave file,
        * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGo4RollingGraph *MakeRollingGraph(const char *fullname, const char *title, Int_t points = 0, Int_t average = 1);

      /** Create 1D window condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param xmin, \param xmax - condition range
       * \param HistoName - name of histogram, to which condition is assigned */
      TGo4WinCond *MakeWinCond(const char *fullname,
                               Double_t xmin, Double_t xmax,
                               const char *HistoName = nullptr);

      /** Create 2D window condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param xmin, \param xmax - X condition range
       * \param ymin, \param ymax - Y condition range
       * \param HistoName - name of histogram, to which condition is assigned */
      TGo4WinCond *MakeWinCond(const char *fullname,
                               Double_t xmin, Double_t xmax,
                               Double_t ymin, Double_t ymax,
                               const char *HistoName = nullptr);

      /** Create polygon condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param npoints - number of points in in polygon condition
       * \param points - (X,Y) points
       * \param HistoName - name of histogram, to which condition is assigned
       * To use method, array should be declared as following:
       * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
       * cond = MakePolyCond("Folder/CondName", 4, points);
       */
      TGo4PolyCond *MakePolyCond(const char *fullname,
                                 Int_t npoints,
                                 Double_t (*points) [2],
                                 const char *HistoName = nullptr);

      /** Create ellipse shaped polygon condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param npoints - number of points in ellipse condition, 0 for default resolution
       * \param cx, \param cy - center coordinates of ellipse
       * \param a1, \param a2 - width of ellipse half axes
       * \param theta - ellipse tilt angle
       * \param HistoName - name of histogram, to which condition is assigned
       */
      TGo4ShapedCond *MakeEllipseCond(const char *fullname,
                                      Int_t npoints,
                                      Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta = 0,
                                      const char *HistoName = nullptr);

      /** Create circular shaped polygon condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param npoints - number of points in ellipse condition, 0 for default resolution
       * \param cx, \param cy - circle center coordinates
       * \param r - circle radius
       * \param HistoName - name of histogram, to which condition is assigned  */
      TGo4ShapedCond *MakeCircleCond(const char *fullname,
                                     Int_t npoints, Double_t cx, Double_t cy, Double_t r,
                                     const char *HistoName = nullptr);

      /** Create tilted rectangular box shaped polygon condition.
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param npoints - number of points in ellipse condition, 0 for default resolution
       * \param cx, \param cy - center coordinates of ellipse
       * \param a1, \param a2 - width of ellipse half axes
       * \param theta - tilt angle
       * \param HistoName - name of histogram, to which condition is assigned
       */
      TGo4ShapedCond *MakeBoxCond(const char *fullname, Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta,
                                  const char *HistoName = nullptr);

      /** Create free shaped (polygon) condition.
       * in contrast to plain TGo4PolyCond, this one can be converted later
       * to different dedicated shape types (ellipse, box, etc.)
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param npoints - number of points in polygon condition
       * \param points - (X,Y) points
       * \param HistoName - name of histogram, to which condition is assigned
       * To use method, array should be declared as following:
       * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
       * cond = MakePolyCond("Folder/CondName", 4, points);
       */
      TGo4ShapedCond *MakeFreeShapeCond(const char *fullname,
                                        Int_t npoints,
                                        Double_t (*points) [2],
                                        const char *HistoName = nullptr);

      /** Create "whitlelist" condition with separate values to test against
       * condition is true if any of the values matches
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param num - number of values in array
       * \param values - 1d array with values
       * \param HistoName - name of histogram, to which condition is assigned
       */
      TGo4ListCond *MakeListCond(const char *fullname, const Int_t num, const Int_t * values,  const char *HistoName = nullptr);

      /** Create "whitlelist" condition with separate values to test against
       * condition is true if any of the values matches
       * \param fullname specifies name of condition (optionally with subfolder name)
       * \param start - first value in list
       * \param stop - last value in list
       * \param step - distance between list entries
       * \param HistoName - name of histogram, to which condition is assigned */
      TGo4ListCond *MakeListCond(const char *fullname, const Int_t start, const Int_t stop, const Int_t step = 1, const char *HistoName = nullptr);

      /** Create "whitlelist" condition with separate values to tes against
        * condition is true if any of the values matches
        * This method creates empty list condition of specified name and title to be set by the user,
        * or already defined condition from previous autosave*/
      TGo4ListCond *MakeListCond(const char *fullname, const char *title, const char *HistoName = nullptr);

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
       *       Macro name should be always starting with "set_" like "set_RocPar.C"
       */
      TGo4Parameter *MakeParameter(const char *fullname,
                                   const char *classname,
                                   const char *newcmd = nullptr);

      /** Executes ROOT script.
       * Returns -1 when script was not found or result of script execution */
      Long64_t ExecuteScript(const char *script_name);

      /** Executes Python script in ROOT interpreter.
       * Will bind TGo4Analysis object to python go4 Symbol
       * Errcode may be used to check ROOT interpreter error code.*/
      Long64_t ExecutePython(const char *macro_name, Int_t *errcode = nullptr);

      /** Process ROOT command line. Optionally provide pyroot binding:
          *  a leading '$' will try to load and execute python script.
          * Errcode may be used to check ROOT interpreter error code.
          * Return value is result of command execution*/
      Long64_t ExecuteLine(const char *command, Int_t *errcode = nullptr);

      /* if true, processor wants to continue with current content of input event
       * event is not refilled from event source, and all previous analysis steps are suspended*/
      Bool_t IsKeepInputEvent() const { return fbKeepInputEvent; }

      /* Mark current input event for reprocessing in next cycle
       * In this case, all previous analysis steps are skipped until this is done*/
      void SetKeepInputEvent(Bool_t val = kTRUE) { fbKeepInputEvent = val; }

      /* if true, processor wants to continue filling current output event
       * Event is not stored, and all subsequent analysis steps are skipped*/
      Bool_t IsKeepOutputEvent() const { return fbKeepOutputEvent; }

      /* Mark current input event for reprocessing in next cycle
       * In this case, all previous analysis steps are skipped until this is done*/
      void SetKeepOutputEvent(Bool_t val = kTRUE) { fbKeepOutputEvent = val; }

      /** Method called when analysis is created */
      virtual void UserPreLoop() {}

      /** Method called when analysis is stopping */
      virtual void UserPostLoop() {}

   protected:

      TGo4EventProcessor() {}

   private:

      /**@supplierCardinality 1 */
      TGo4EventElement *fxInputEvent{nullptr}; //!

      /** @link aggregation */
      TGo4EventCalibration *fxCalibration{nullptr};

      /** flag to steer re-processing of same input event */
      Bool_t fbKeepInputEvent{kFALSE}; //!

      /** flag to steer continue filling of same output event */
      Bool_t fbKeepOutputEvent{kFALSE}; //!

      ClassDefOverride(TGo4EventProcessor,3)
};

#endif //TGO4EVENTPROCESSOR_H
