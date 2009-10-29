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

#ifndef TGO4FITMINUIT_H
#define TGO4FITMINUIT_H

#include "TGo4FitterAction.h"

#include "TObjArray.h"

class TGo4FitMinuitResult;

/**
 * Minuit minimization action
 * Now only TGo4FitMinuit class, provided general minimization routine, is available. It uses standard ROOT TMinuit class [3]. TGo4FitMinuit class includes Minuit commands list, which will be executed during minimizatione. There are several methods of TGo4FitMinuit class to operate with command list:
 * AddCommand() - add command to commands list;
 * GetNumCommands() - get number of commands in list;
 * GetCommand() - get command from list;
 * ClearCommands() - clear commands list.
 * To get full description of Minuit commands, see Minuit reference manual [4].
 * In additional to standard Minuit commands, one adds result command, which get status and results values from Minuit and store them as TGo4FitMinuitResult objects in TGo4FitMinuit results list. The syntax of command is
 *
 * result [xxxx [result_name]]
 *
 * where "result"- identifier of this command, "xxxx" - flags field (default - "1000"), "result_name"- optional name of result object (default - "Result"). The each "x" in flags field can be: "0" - option switched off or "1" - switched on. The meanings of flags are:
 * 1.   Storing of current parameters values and errors (ParValues and ParError arrays of doubles, TArrayD class).
 * 2.   Storing result of Minos error analysis (EPLUS, EMINUS, EPARAB & GLOBCC arrays of doubles). Normally should be used after "MINOs" command of Minuit.
 * 3.   Storing error matrice estimations to ERRORMATRIX (TMatrix class). Columns and strings in matrix, corresponds to fixed elements, will be set to 0.
 * 4.   Storing contour plot in CONTOX, CONTOY (both are arrays of doubles) and CONTOCH (array of char, TArrayC). Normally should be switched on after "MNContour" command of Minuit.
 * Result object always store status values of Minuit (see MNSTAT command in Minuit reference manual [4]):
 * FMIN - the best function value found so far;
 * FEDM - the estimated vertical distance remaining to minimum;
 * ERRDEF - the value of UP defining parameter uncertainties;
 * NPARI - number of currently variable parameters;
 * NPARX - the highest (external) parameter number defined by user;
 * ISTAT - a status integer indicating how good is the covariance matrix.
 * Several result commands can be present in Minuit command list and the same number of TGo4FitMinuitResult object will be present in TGo4FitMinuit object after minimization is finished. Results can be accessed via index, using GetNumResults() and GetResult() methods or via result name, using FindResult() method. The results objects always owned by TGo4FitMinuit object and stored together with it. Thus, if TGo4FitMinuit object will be saved together with fitter, the TGo4FitMinuitResult objects also will be stored and can be accessed later, then fitter will be loaded.
 */
class TGo4FitMinuit : public TGo4FitterAction {
   public:

      /**
       * Default constructor.
       */
      TGo4FitMinuit();

      /**
       * Create TGo4FitMinuit object (action) with given name.
       */
      TGo4FitMinuit(const char* Name);

      /**
       * Destroys TGo4FitMinuit object.
       */
      virtual ~TGo4FitMinuit();

      /**
       * Add Minuit command to command list.
       * To get a full description of possible Minuit commands, see Minuit documentation.
       */
      void AddCommand(const char* iCommand);

      /**
       * Returns number of commands in list.
       */
      Int_t GetNumCommands() { return fxCommands.GetLast()+1; }

      /**
       * Return command with given index.
       */
      const char* GetCommand(Int_t n);

      /**
       * Clear Minuit commands list.
       */
      void ClearCommands() { fxCommands.Clear(); }

      /**
       * Returns number of result objects.
       */
      Int_t GetNumResults() { return fxResults.GetLast()+1; }

      /**
       * Get TGo4FitMinuitResult object with given index.
       */
      TGo4FitMinuitResult* GetResult(Int_t indx);

      /**
       * Find result object with provided name.
       */
      TGo4FitMinuitResult* FindResult(const char* ResName);

      /**
       * Add result object to results list.
       */
      void AddResult(TGo4FitMinuitResult* res);

      /**
       * Remove result from results list and delete it.
       */
      void RemoveResult(TGo4FitMinuitResult* res);

      /**
       * Delete all results objects.
       */
      void ClearResults() { fxResults.Clear(); }

      virtual Bool_t NeedBuffers() const { return kTRUE; }

      /**
       * Execute all Minuit commands.
       */
      virtual void DoAction(TGo4FitterAbstract* Fitter);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * List of minuit commands, presented by array of TObjString.
       */
      TObjArray fxCommands;

      /**
       * List of TGo4FitMinuitResult objects.
       */
      TObjArray fxResults;

   private:

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label TObjArray*/
      /*#  TGo4FitMinuitResult ResultArray; */

   ClassDef(TGo4FitMinuit,1)
};

#endif // TGO4FITMINUIT_H
