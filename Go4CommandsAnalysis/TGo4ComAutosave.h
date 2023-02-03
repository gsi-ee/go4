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

#ifndef TGO4COMAUTOSAVE_H
#define TGO4COMAUTOSAVE_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Write objects into autosave file now.
 */
class TGo4ComAutosave : public TGo4AnalysisCommand {
   public:

      TGo4ComAutosave();

      Int_t ExeCom() override;

      /** Set internals of this command from specification of external
       * remote command (command container). Must be overloaded
       * for each specific command! */
      void Set(TGo4RemoteCommand *remcon) override;

      void SetAutoSaveCompression(Int_t i = 5) { fiAutoSaveCompression = i; }

      void SetAutoSaveOverwrite(Bool_t over = kTRUE) { fbAutoSaveOverwrite = over ? 1 : 0; }

      void SetAutoFileName(const char *name) { fxAutoFileName = name; }

      const char *GetAutoFileName() const { return fxAutoFileName.Data(); }

   private:

      /** compression level for autosave file */
      Int_t fiAutoSaveCompression{0};

      /** overwrite mode for autosave (true) or append mode (false).*/
      Int_t fbAutoSaveOverwrite{0};

      /** Name of the autosave file. Replaces previously set name */
      TString fxAutoFileName;

   ClassDefOverride(TGo4ComAutosave,1)
};

#endif //TGO4COMAUTOSAVE_H
