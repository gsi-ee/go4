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

#ifndef TGO4FITTEROUTPUT_H
#define TGO4FITTEROUTPUT_H

#include "TGo4FitterAction.h"

class TVirtualPad;

/**
 * Output action
 * To add some output to actions, TGo4FitterOutput action class should be used. In constructor output command and options (if required) should be specified. Also AddOuputAction() routine of fitter can be used. Now following commands are available:
 * "Print". Possible options are: "*"- print all internal objects, "**" - print also objects in slots, "Pars" - print parameters values, "Ampls" - only amplitude parameters, "Results" - result values.
 * "Draw". Draw of the fitter data and model objects. As option name of data object and its components should be sets up. For instance, "data1,Gauss1,Gauss2". If first character will be "#", Draw command creates new canvas for output.
 */
class TGo4FitterOutput : public TGo4FitterAction {
   public:

      /**
       * Default constructor.
       */
      TGo4FitterOutput();

      /**
       * Creates TGo4FitterOutput action with provided command name and options (if required).
       */
      TGo4FitterOutput(const char *Command, const char *Options = nullptr);

      /**
       * Destroys TGo4FitterOutput object.
       */
      virtual ~TGo4FitterOutput();

      /**
       * Set command name and options (if required).
       */
      void Set(const char *Command, const char *Options = nullptr) { fxCommand = Command; fxOptions = Options; }

      /**
       * Sets pad, which can be used for command output.
       */
      void SetPad(TVirtualPad* iPad) { fxPad = iPad; }

      /**
       * Returns assigned pad.
       */
      TVirtualPad* GetPad() const { return fxPad; }

      /**
       * Return kTRUE, if pad required for output.
       */
      Bool_t NeedPad() const { return (fxCommand == "Draw") && (fxOptions.IsNull() || (fxOptions[0] != '#')); }

      /**
       * Returns command name.
       */
      const char *GetCommand() const { return fxCommand.Data(); }

      /**
       * Returns command options.
       */
      const char *GetOptions() const { return fxOptions.Data(); }

      /**
       * Sets command name.
       */
      void SetCommand(const char *Command) { fxCommand = Command; }

      /**
       * Sets command options.
       */
      void SetOptions(const char *Options) { fxOptions = Options; }

      /**
       * Execute proper output of fitter, Print() or Draw(), with specified options.
       */
      void DoAction(TGo4FitterAbstract* Fitter) override;

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;
    private:

      /**
       * Command name.
       */
      TString fxCommand;

      /**
       * Command options.
       */
      TString fxOptions;

      /**
       * TPad, which can be used for output.
       */
      TVirtualPad* fxPad{nullptr};                   //!

   ClassDefOverride(TGo4FitterOutput,1)
};

#endif // TGO4FITTEROUTPUT_H
