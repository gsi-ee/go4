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

/** @brief Abstract event processor
 * @details Also provides number of methods to create histograms and conditions
 * @ingroup go4_major_classes
 * @ingroup go4_event
 * @author J. Adamczewski
 * @since 1/2001 */

class TGo4EventProcessor: public TGo4EventSource {
      friend class TGo4EventElement;

   public:

      TGo4EventProcessor(const char *name);

      virtual ~TGo4EventProcessor();

      /**
       * @brief Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
      void SetInputEvent(TGo4EventElement *raw) { fxInputEvent = raw; }

      /**
       * @brief Access to external raw event which is set as association member.
       */
      inline TGo4EventElement *GetInputEvent() { return fxInputEvent; }

      /**
       * @brief Check if input event is valid.
       * @details Tests the pointer for
       * zero, and if not zero, compares classname of the
       * input event with given string. To be used from the
       * Fill method and from the analysis method  init event classes.
       */
      virtual Bool_t CheckInputEvent(const char *classname);

      /**
       * @brief  Sets reference to external raw event which is used by
       * the concrete processor to unpack the interesting
       * information.
       */
      void SetCalibration(TGo4EventCalibration *cali) { fxCalibration = cali; }

      /**
       * Access to external calibration object.
       */
      TGo4EventCalibration *GetCalibration() const { return fxCalibration; }

      /**
       * @brief Check if calibration class is valid.
       * @details Tests the pointer for
       * zero, and if not zero, compares classname of the
       * calibration instance with given string. To be used from the
       * Fill method and from the analysis method  init event classes.
       */
      virtual Bool_t CheckCalibration(const char *classname);

      /** @brief implemented by subclass to assign correct event structures*/
      virtual void InitEvent(TGo4EventElement *) {}

      /** @brief implemented by subclass to do actual processing. For second step processors!*/
      virtual void FinalizeEvent() {}

      /** @brief Register user object to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddObject(TNamed *anything, const char *subfolder = nullptr);

      /** @brief Register histogram to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddHistogram(TH1 *his, const char *subfolder = nullptr, Bool_t replace = kTRUE);

      /** @brief Register parameter to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddParameter(TGo4Parameter *par, const char *subfolder = nullptr);

      /** @brief Register condition to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddAnalysisCondition(TGo4Condition *con, const char *subfolder = nullptr);

      /** @brief Register picture to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddPicture(TGo4Picture *pic, const char *subfolder = nullptr);

      /** @brief Register TCanvas to go4 framework.
       * @details Shortcut forwarded to analysis instance */
      Bool_t AddCanvas(TCanvas *can, const char *subfolder = nullptr);

      /** @brief UnRegister histogram from go4 framework and delete it.
       * @details Shortcut forwarded to analysis instance */
      Bool_t RemoveHistogram(const char *name);

      /** @brief UnRegister parameter from go4 framework and delete it.
       * @details Shortcut forwarded to analysis instance */
      Bool_t RemoveParameter(const char *name);

      /** @brief UnRegister condition from go4 framework and delete it.
       * @details Shortcut forwarded to analysis instance */
      Bool_t RemoveAnalysisCondition(const char *name);

      /** @brief UnRegister picture from go4 framework and delete it.
       * @details Shortcut forwarded to analysis instance */
      Bool_t RemovePicture(const char *name);

      /** @brief UnRegister canvas from go4 framework and delete it.
       * @details Shortcut forwarded to analysis instance */
      Bool_t RemoveCanvas(const char *name);

      /** @brief Get any object from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TNamed *GetObject(const char *name, const char *folder = nullptr);

      /** @brief Get histogram from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TH1 *GetHistogram(const char *name);

      /** @brief Get parameter from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TGo4Parameter *GetParameter(const char *name, const char *par_class = nullptr);

      /** @brief Get condition from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TGo4Condition *GetAnalysisCondition(const char *name, const char *cond_cl = nullptr);

      /** @brief Get picture from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TGo4Picture *GetPicture(const char *name);

      /** @brief Get TCanvas from go4 framework.
       * @details Shortcut forwarded to analysis instance */
      TCanvas *GetCanvas(const char *name);

      /**
       * @brief Returns the input event structure of analysis step.
       * @details Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement *GetInputEvent(const char *stepname);

      /**
       * @brief Returns the output event structure of analysis step.
       * @details Argument indicates
       * the step by name. Shortcut forwarded to analysis instance
       */
      TGo4EventElement *GetOutputEvent(const char *stepname);

      /**
       * @brief Display a user message.
       * @param prio defines message priority:
       * - <0: display raw text on local terminal like plain printf
       * - 0: display on local terminal only;  if debug output is enabled
       * - 1: local terminal and info message in gui log panel (if gui mode)
       * - 2: local terminal and warning message in gui log panel (if gui mode)
       * - 3: local terminal and error message in gui log panel (if gui mode)
       * @param text - message with optional formating
       */
      void Message(Int_t prio, const char *text, ...)
       #if defined(__GNUC__) && !defined(__CLING__)
         __attribute__((format(printf, 3, 4)))
       #endif
      ;

      /** @brief  Send any object as copy to gui. Shortcut forwarded to analysis instance */
      void SendObjectToGUI(TNamed *ob);

      /* We overwrite the default TNamed::Clear that would
       *  erase our name and title!
       * Implement this method in your parameter class
       * if you would like to reset any values with the
       * eraser button in the gui remote browser*/
      void Clear(Option_t *opt = "") override;

      /** @brief Set flag to use data from autosave file in Make... methods (default true) */
      void SetMakeWithAutosave(Bool_t on = kTRUE);

      /** @brief Returns kTRUE if object in last Make... call was created,
       * kFALSE when object was retrieved from autosavefile */
      Bool_t IsObjMade() const;

      /** @brief Create one dimensional histogram of specified type.
       * @details If histogram exactly with same name and type already exists in  autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
       * @param type can be:
       * - 'I', 'i' for TH1I - Int_t as bin content (default)
       * - 'F', 'f' for TH1F - Float_t as bin content
       * - 'D', 'd' for TH1D - Double_t as bin content
       * - 'S', 's' for TH1S - Short_t as bin content
       * - 'C', 'c' for TH1C - Char_t as bin content
       * - 'L', 'l' for TH1L - Long64_t as bin content
       * @param fullname specifies name of histogram (optionally with subfolder name)
       * @param title - histogram title
       * @param nbinsx - number of X bins
       * @param xlow - minimum value for X axis
       * @param xup - maximum value for X axis
       * @param xtitle - title for X axis of histogram
       * @param ytitle - title for Y axis of histogram */
      TH1 *MakeTH1(char type, const char *fullname, const char *title,
                   Int_t nbinsx, Double_t xlow, Double_t xup,
                   const char *xtitle = nullptr, const char *ytitle = nullptr);

      /** @brief Create two dimensional histogram of specified type.
       * @details If histogram exactly with same name and type already exists in  autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.
       * @param type can be:
       * 'I', 'i' for TH2I - Int_t as bin content (default)
       * 'F', 'f' for TH2F - Float_t as bin content
       * 'D', 'd' for TH2D - Double_t as bin content
       * 'S', 's' for TH2S - Short_t as bin content
       * 'C', 'c' for TH2C - Char_t as bin content
       * 'L', 'l' for TH2L - Long64_t as bin content
       * @param fullname specifies name of histogram (optionally with subfolder name)
       * @param title - histogram title
       * @param nbinsx - number of X bins
       * @param xlow - minimum value for X axis
       * @param xup - maximum value for X axis
       * @param nbinsy - number of Y bins
       * @param ylow - minimum value for Y axis
       * @param yup - maximum value for Y axis
       * @param xtitle - title for X axis of histogram
       * @param ytitle - title for Y axis of histogram
       * @param ztitle - title for Z axis of histogram */
      TH2 *MakeTH2(char type, const char *fullname, const char *title,
                   Int_t nbinsx, Double_t xlow, Double_t xup,
                   Int_t nbinsy, Double_t ylow, Double_t yup,
                   const char *xtitle = nullptr, const char *ytitle = nullptr, const char *ztitle = nullptr);

      /** @brief Create a TGraph with initial values as specified by points, xvalues and yvalues.
       * @details If theses parameters are missing, an empty graph is created to be specified by the user.
       * If a graph of this name already exists in the autosave file,
       * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGraph *MakeGraph(const char *fullname, const char *title, Int_t points = 0, Double_t *xvalues = nullptr,
                        Double_t *yvalues = nullptr);

      /** @brief Create a TGraph with values initialized by a function object TF1l.
        * @details If a graph of this name already exists in the autosave file,
        * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGraph *MakeGraph(const char *fullname, const char *title, TF1 *function);

      /** @brief Create a go4 rolling graph (generic trending plot) with properties points and average.
        * @details If theses parameters are missing, an empty graph is created to be specified by the user.
        * If a rolling graph of this name already exists in the autosave file,
        * it will be returned. With SetMakeWithAutosave(kFALSE) one can exclude data from autosave.*/
      TGo4RollingGraph *MakeRollingGraph(const char *fullname, const char *title, Int_t points = 0, Int_t average = 1);

      /** @brief Create 1D window condition.
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param xmin - X min condition range
       * @param xmax - X max condition range
       * @param HistoName - name of histogram, to which condition is assigned */
      TGo4WinCond *MakeWinCond(const char *fullname,
                               Double_t xmin, Double_t xmax,
                               const char *HistoName = nullptr);

      /** @brief Create 2D window condition.
        * @param fullname specifies name of condition (optionally with subfolder name)
        * @param xmin - X min condition range
        * @param xmax - X max condition range
        * @param ymin - Y min condition range
        * @param ymax - Y max condition range
        * @param HistoName - name of histogram, to which condition is assigned */
      TGo4WinCond *MakeWinCond(const char *fullname,
                               Double_t xmin, Double_t xmax,
                               Double_t ymin, Double_t ymax,
                               const char *HistoName = nullptr);

      /** @brief Create polygon condition.
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param npoints - number of points in in polygon condition
       * @param points - (X,Y) points
       * @param HistoName - name of histogram, to which condition is assigned
       * To use method, array should be declared as following:
       * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
       * cond = MakePolyCond("Folder/CondName", 4, points); */
      TGo4PolyCond *MakePolyCond(const char *fullname,
                                 Int_t npoints,
                                 Double_t (*points) [2],
                                 const char *HistoName = nullptr);

      /** @brief Create ellipse shaped polygon condition.
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param npoints - number of points in ellipse condition, 0 for default resolution
       * @param cx - center X coordinate of ellipse
       * @param cy - center Y coordinate of ellipse
       * @param a1 - width of ellipse X half axes
       * @param a2 - width of ellipse Y half axes
       * @param theta - ellipse tilt angle
       * @param HistoName - name of histogram, to which condition is assigned  */
      TGo4ShapedCond *MakeEllipseCond(const char *fullname,
                                      Int_t npoints,
                                      Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta = 0,
                                      const char *HistoName = nullptr);

      /** @brief Create circular shaped polygon condition.
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param npoints - number of points in shaped condition, 0 for default resolution
       * @param cx - circle center X coordinate
       * @param cy - circle center Y coordinate
       * @param r  - circle radius
       * @param HistoName - name of histogram, to which condition is assigned */
      TGo4ShapedCond *MakeCircleCond(const char *fullname,
                                     Int_t npoints, Double_t cx, Double_t cy, Double_t r,
                                     const char *HistoName = nullptr);

      /** @brief Create tilted rectangular box shaped polygon condition.
       *  @param fullname specifies name of condition (optionally with subfolder name)
       *  @param cx - center X coordinates of box
       *  @param cy - center Y coordinates of box
       *  @param a1 - width of box half axes
       *  @param a2 - height of box half axes
       *  @param theta - tilt angle
       *  @param HistoName - name of histogram, to which condition is assigned */
      TGo4ShapedCond *MakeBoxCond(const char *fullname, Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta,
                                  const char *HistoName = nullptr);

      /** @brief Create free shaped (polygon) condition.
       * @details in contrast to plain TGo4PolyCond, this one can be converted later
       * to different dedicated shape types (ellipse, box, etc.)
       * To use method, array should be declared as following:
       * ~~~{cpp}
       * Double_t points[4][2] = { {10, 0}, {10, 10}, {5, 15}, {5, 5} };
       * auto cond = MakePolyCond("Folder/CondName", 4, points);
       * ~~~
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param npoints - number of points in polygon condition
       * @param points - (X,Y) points
       * @param HistoName - name of histogram, to which condition is assigned */
      TGo4ShapedCond *MakeFreeShapeCond(const char *fullname,
                                        Int_t npoints,
                                        Double_t (*points) [2],
                                        const char *HistoName = nullptr);

      /** @brief Create "whitlelist" condition with separate values to test against
       * condition is true if any of the values matches
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param num - number of values in array
       * @param values - 1d array with values
       * @param HistoName - name of histogram, to which condition is assigned
       */
      TGo4ListCond *MakeListCond(const char *fullname, const Int_t num, const Int_t * values,  const char *HistoName = nullptr);

      /** @brief Create "whitlelist" condition with separate values to test against
       * condition is true if any of the values matches
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param start - first value in list
       * @param stop - last value in list
       * @param step - distance between list entries
       * @param HistoName - name of histogram, to which condition is assigned */
      TGo4ListCond *MakeListCond(const char *fullname, const Int_t start, const Int_t stop, const Int_t step = 1, const char *HistoName = nullptr);

      /** @brief Create "whitlelist" condition with separate values to tes against
        * condition is true if any of the values matches
        * @details This method creates empty list condition of specified name and title to be set by the user,
        * or already defined condition from previous autosave*/
      TGo4ListCond *MakeListCond(const char *fullname, const char *title, const char *HistoName = nullptr);

      /** @brief Create parameter of specified class,
       * @param fullname specifies name of condition (optionally with subfolder name)
       * @param classname - name of required parameter class, it should be known to ROOT.
       * @param newcmd - optional argument, can be used for two purposes:
       *    1. as new command to create parameter like `new UserParameter(%s, 1000, 2000)`,
       *       where %s is place for parameter name.  Should be specified, if parameter
       *       constructor contains more parameters as only parameter name. Such argument
       *       should be always started with "new "" command.
       *    2. macro name to set parameter value. Macro executed immediately after parameter
       *       creation (or loading from auto-save file) and thus overwrites parameters value.
       *       Macro name should be always starting with "set_" like "set_RocPar.C"
       */
      TGo4Parameter *MakeParameter(const char *fullname,
                                   const char *classname,
                                   const char *newcmd = nullptr);

      /** @brief Executes ROOT script.
       * @return -1 when script was not found or result of script execution */
      Long64_t ExecuteScript(const char *script_name);

      /** @brief Executes Python script in ROOT interpreter.
       * @details Will bind TGo4Analysis object to python go4 Symbol
       * Errcode may be used to check ROOT interpreter error code.*/
      Long64_t ExecutePython(const char *macro_name, Int_t *errcode = nullptr);

      /** @brief Process ROOT command line.
       * @details Optionally provide pyroot binding:
          *  a leading '$' will try to load and execute python script.
          * Errcode may be used to check ROOT interpreter error code.
          * Return value is result of command execution*/
      Long64_t ExecuteLine(const char *command, Int_t *errcode = nullptr);

      /** @brief Is marked to keep input event
       * @details if true, processor wants to continue with current content of input event
       * event is not refilled from event source, and all previous analysis steps are suspended*/
      Bool_t IsKeepInputEvent() const { return fbKeepInputEvent; }

      /** @brief  Mark current input event for reprocessing in next cycle
       * @details In this case, all previous analysis steps are skipped until this is done*/
      void SetKeepInputEvent(Bool_t val = kTRUE) { fbKeepInputEvent = val; }

      /** @brief Is marked to keep output event
       * @details if true, processor wants to continue filling current output event
       * Event is not stored, and all subsequent analysis steps are skipped*/
      Bool_t IsKeepOutputEvent() const { return fbKeepOutputEvent; }

      /** @brief Mark current input event for reprocessing in next cycle
       *  @details In this case, all previous analysis steps are skipped until this is done*/
      void SetKeepOutputEvent(Bool_t val = kTRUE) { fbKeepOutputEvent = val; }

      /** @brief Method called when analysis is created */
      virtual void UserPreLoop() {}

      /** @brief Method called when analysis is stopping */
      virtual void UserPostLoop() {}

   protected:

      TGo4EventProcessor() {}

   private:

      /** @supplierCardinality 1 */
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
