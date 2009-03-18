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


