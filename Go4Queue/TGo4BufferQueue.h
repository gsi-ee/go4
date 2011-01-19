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

#ifndef TGO4BUFFERQUEUE_H
#define TGO4BUFFERQUEUE_H

#include "TGo4Queue.h"

class TMutex;
class TIterator;
class TList;

/**
 * Class containing a pointer queue for TBuffers. Is able to stream
 * any external TObject into a TBuffer which then is added to the queue.
 */
class TGo4BufferQueue : public TGo4Queue {

   public:

      TGo4BufferQueue() ;

      TGo4BufferQueue(const char* name);

      /**
       * Reconstruct a TObject queue entry from a given TBuffer pointer.
       * To be used for streaming the receiving socket contents into the queue.
       * Operation should be thread locked by the calling user.
       */
      void AddBufferFromObject(TObject * object);

      /**
       * Add buffer pointer to queue. We Renamed the protected baseclass method
       * to avoid user confusion. If clone is true, a clone (deep streamer copy)
       * of the given buffer will be done and this clone is added to the queue.
       */
      void AddBuffer(TBuffer * buffer, Bool_t clone=kFALSE);

      /**
       * Free internal buffer to be re-used by the AddBuffer as clone
       * method. To be used by the thread that got the
       * buffer from WaitBuffer after working on it. If buffer does not
       * belong to the internal buffers list of this queue, it is _not_
       * added to the free buffer list.
       */
      void FreeBuffer(TBuffer * buffer);

      /** Empty the queue and give free buffers back */
      virtual void Clear(Option_t* opt="");

      /**
       * Wait for buffer object from queue. We Renamed the protected baseclass method
       * to avoid user confusion.
       */
      TBuffer * WaitBuffer();

      /**
       * Wait for buffer object from queue. When buffer is received, object is
       * reconstructed out of it (thread mainlock aqcuired!). NULL in case of
       * unknown class.
       */
      TObject * WaitObjectFromBuffer();

      virtual ~TGo4BufferQueue() ;

      /**
       * Create a root buffer that contains a single value val.
       * This feature is used to wrap direct (emergency) commands
       * into a TBuffer to send it via the taskhandler queues and
       * sockets. Also used to indicate disconnect mode for
       * the taskhandler threads.
       */
      static TBuffer* CreateValueBuffer(UInt_t val);

      /**
       * Extract value from buffer that was created by
       * CreateValueBuffer method. Used to decode a
       * direct command or disconnect value that is
       * received from queue or socket connection.
       * Negative value means no valid value buffer.
       */
      static Int_t DecodeValueBuffer(TBuffer* buf);


      /**
       *.we emulate the protected owner flag of the TBuffer class, needed for reallocation!
       */
      static const Int_t fgiISOWNER;

   private:
      /**
       * Initialization of internal queue buffer.
       */
      void InitBuffers();

      /**
       * Reallocate buffer of TBuffer to newsize.
       */
      void Realloc(TBuffer* buffer, Int_t oldsize, Int_t newsize);

      /**
       * Create dummy buffer for queue.
       */
      TBuffer* NewEntry();



      /**
       * List of preallocated buffer TBuffers which
       * are used when TBuffer added to queue should
       * be "cloned". Each AddBuffer with option clone
       * will memcopy the input buffer char field to
       * the next TBuffer field in this list. Output buffer
       * after WaitBuffer is then this internal TBuffer,
       * not the input TBuffer. If all buffer buffers are in
       * use, no further buffer is allowed to be added as
       * clone.
       */
      TList* fxBufferList; //!

      /** List of buffers which are free for the next add. */
      TList* fxFreeList;   //!
      TMutex * fxBufferMutex; //!
      Int_t fiOverflowcount;

      /**
       * Number of preallocated buffer TBuffers
       * (maximum entries in fxBufferList).
       */
      Int_t fiMaxBuffers;

};

#endif //TGO4BUFFERQUEUE_H
