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

#ifndef TGO4QUEUE_H
#define TGO4QUEUE_H

#include "TNamed.h"

class TCondition;
class TList;
class TMutex;

/**
 * Base class for all threadsafe pointer queues. TObject pointers are
 * transported in queue without streaming. Subclasses may specialize the
 * behaviour on adding and waiting.
 * @author J. Adamczewski
 * @since 13-Jul-2001
 */

class TGo4Queue : public TNamed {
   public:
      TGo4Queue(const char *name = nullptr);

      virtual ~TGo4Queue();

      void Clear(Option_t *opt = "") override;

      virtual void Wake();

      Bool_t IsEmpty() const;

      Int_t GetEntries() const { return fiEntries; }

      void SetMaxEntries(Int_t max) { fiMaxEntries = max; }

   protected:
      TObject *Next();
      TObject *Wait();
      void Add(TObject *ob);

   private:
      TMutex* fxMutex{nullptr};          //!
      TCondition* fxCondition{nullptr};  //!
      TList *fxList{nullptr};            //!
      Int_t fiEntries{0};                //!
      Int_t fiMaxEntries{0};             //!
      Bool_t fbWakeUpCall{kFALSE};       //!
};

#endif //TGO4QUEUE_H
