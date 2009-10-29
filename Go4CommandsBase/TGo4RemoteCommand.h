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

#ifndef TGO4REMOTECOMMAND_H
#define TGO4REMOTECOMMAND_H

#include "TGo4Command.h"
#include "TString.h"

#define __REMCOMPARS__ 5
/**
 * This command is a container for commands to be send from
 * master to slave via inter-process transport. Master
 * Only has to know this command (Go4CommandsBase package),
 * thus decoupling from Slave libraries
 * @author J. Adamczewski, gsi
 * @since 27-Apr-2005
 **/
class TGo4RemoteCommand : public TGo4Command {

   public:

      TGo4RemoteCommand();

      TGo4RemoteCommand(const char* comname);

      virtual ~TGo4RemoteCommand();

      /** Remote command will execute on the command invoker at the
       * slave side to create and execute the correct command from
       * the contained information*/
      Int_t ExeCom();

      /** Wrap any object ob into this command container. May
       * be any parameter, condition, or other status object to
       * be used for command execution.*/
      void SetAggregate(TObject* ob);

      TObject* GetAggregate(Bool_t chown=kTRUE);

      /** Set Go4 Name of command to be executed on the
       * command receiver side*/
      void SetCommandName(const char* txt) { fxCommandName=txt; }

      const char* GetCommandName() const { return fxCommandName.Data(); }

      /** Specify string parameter txt for index i
       * to be used in executed command.
       * Index 0 is usually reserved for object name to be modified.
       * higher indices are used variably depending on command */
      void SetString(const char* txt,Int_t i);

      const char* GetString(Int_t i) const;

      /** Specify integer parameters to be used in executed command.
       * Indices are used depending on command. To be applied also
       * for boolean values! */
      void SetValue(Int_t num, Int_t i);

      Int_t GetValue(Int_t i) const;

      static const char* Get_fgxREMCOMEMPTYSTRING();

   private:

      void InitPars();

      /** Name of command to be executed */
      TString fxCommandName;

      /** String parameters. May contain object name, folder name,
       * passwords, etc.. */
      TString fxParameter[__REMCOMPARS__];

      /** Numerical parameters for command, otpional*/
      Int_t fiValue[__REMCOMPARS__];

      /** Optional aggregated parameter object. May be a condition,
        * status, canvas etc. to apply on slave node. */
      TObject* fxAggregate;

      static const char* fgxREMCOMEMPTYSTRING; //!


   ClassDef(TGo4RemoteCommand,1)
};

#endif //TGO4REMOTECOMMAND_H
