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

#ifndef TGO4FILESTOREPARAMETER_H
#define TGO4FILESTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

class TGo4FileStoreParameter : public TGo4EventStoreParameter {

   public:
      TGo4FileStoreParameter();

      TGo4FileStoreParameter(const char *name,
                             Int_t splitlevel=99,
                             Int_t bufsize=32000,
                             Int_t compression=5,
                             Int_t autosave=10000);

      virtual ~TGo4FileStoreParameter();

      void Print(Option_t* = "") const override;

      /** update contents of parameter class with external object. */
      Bool_t UpdateFrom(TGo4Parameter* rhs) override;

      Int_t GetSplitlevel() const { return fiSplit; }
      void SetSplitlevel(Int_t split) { fiSplit=split; }

      Int_t GetBufsize() const { return fiBufsize; }
      void SetBufsize(Int_t bufsize) { fiBufsize=bufsize; }

      Int_t GetCompression() const { return fiCompression; }
      void SetCompression(Int_t comp) { fiCompression=comp; }

      Int_t GetAutosaveSize() const { return fiAutosavesize; }
      void SetAutosaveSize(Int_t interval) { fiAutosavesize=interval; }

      Int_t IsOverwriteMode() const { return fbOverwrite; }
      void SetOverwriteMode(Bool_t over=kTRUE) { fbOverwrite = over ? 1 : 0; }

   private:

      /** Splitlevel for Tree. Default is 1.*/
      Int_t fiSplit{99};

      /** Size of the root io buffer. */
      Int_t fiBufsize{64000};

      /** Compression level of the root file. Default is 5. */
      Int_t fiCompression{5};

      /** Autosave interval for the Tree. */
      Int_t fiAutosavesize{10000};

      /** True if old filestore of same name shall be overwritten.
        * False if old filestore should be opened in append (update) mode.
        * internally, we use Int_t for root streamer problems.*/
      Int_t fbOverwrite{1};

   ClassDefOverride(TGo4FileStoreParameter,1)
};

#endif //TGO4FILESTOREPARAMETER_H
