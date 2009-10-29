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

#ifndef TGO4OBJECTQUEUE_H
#define TGO4OBJECTQUEUE_H

#include "TGo4Queue.h"

/**
 * Class containing a pointer queue for objects. Is able to add
 * any object contained in a TBuffer into the queue using the root streamer
 */

class TGo4ObjectQueue : public TGo4Queue {

   public:

      TGo4ObjectQueue();

      TGo4ObjectQueue(const char* name);

      virtual ~TGo4ObjectQueue();

      /**
       * Reconstruct a TObject queue entry from a given TBuffer pointer.
       * To be used for streaming the receiving socket contents into the queue.
       * Operation should be thread locked by the calling user.
       */
      void AddObjectFromBuffer(TBuffer* buffer);

      /**
       * Add object pointer to queue. We Renamed the protected baseclass method
       * to avoid user confusion.
       */
      void AddObject(TObject * object, Bool_t clone=kFALSE);

      /**
       * Wait for object from queue. We Renamed the protected baseclass method
       * to avoid user confusion.
       */
      TObject* WaitObject();
};

#endif //TGO4OBJECTQUEUE_H


