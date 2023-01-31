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

#include "TGo4BufferQueue.h"

#include <iostream>

#include "TROOT.h"
#include "TMutex.h"
#include "TFile.h"
#include "TBufferFile.h"

#include "TGo4Socket.h"
#include "TGo4RuntimeException.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"

TGo4BufferQueue::TGo4BufferQueue() :
   TGo4Queue("Default buffer queue"),
   fxBufferList(nullptr),
   fxFreeList(nullptr),
   fxBufferMutex(nullptr),
   fiMaxBuffers(10)
{
   GO4TRACE((14,"TGo4BufferQueue::TGo4BufferQueue()", __LINE__, __FILE__));

   InitBuffers();
}

TGo4BufferQueue::TGo4BufferQueue(const char *name) :
   TGo4Queue(name),
   fxBufferList(nullptr),
   fxFreeList(nullptr),
   fxBufferMutex(nullptr),
   fiMaxBuffers(10)
{
   GO4TRACE((14,"TGo4BufferQueue::TGo4BufferQueue(const char*)", __LINE__, __FILE__));

   InitBuffers();
}

void TGo4BufferQueue::InitBuffers()

{
   GO4TRACE((14,"TGo4BufferQueue::InitBuffers()", __LINE__, __FILE__));
   TGo4LockGuard mainguard;
   fxBufferList = new TList; // list owning all buffers
   fxFreeList = new TList;    // list indicating the free buffers
   fxFreeList->SetOwner(kFALSE); // JAM2018 - avoid root6 problems at shutdown???
   fxBufferMutex = new TMutex;
   for (Int_t i = 0; i < fiMaxBuffers; ++i) {
      TBuffer *buf = NewEntry();
      fxBufferList->Add(buf);
      fxFreeList->Add(buf);
   }
}

TGo4BufferQueue::~TGo4BufferQueue()
{
   GO4TRACE((14,"TGo4BufferQueue::~TTGo4BufferQueue()", __LINE__, __FILE__));

   //printf ("JAM*************** DTOR of TGo4BufferQueue %s BEGIN\n", GetName());
   //fxBufferList->Delete();// JAM2018 - avoid root6 problems at shutdown???
   TCollection::EmptyGarbageCollection();
   //printf ("JAM*************** DTOR of TGo4BufferQueue %s after EmptyGarbageCollection \n", GetName());

   delete fxFreeList; fxFreeList = nullptr;
   //printf ("JAM*************** DTOR of TGo4BufferQueue %s after delete fxFreeList \n", GetName());
   delete fxBufferList; fxBufferList = nullptr;
   //printf ("JAM*************** DTOR of TGo4BufferQueue %s after delete fxBufferList \n", GetName());
   delete fxBufferMutex; fxBufferMutex = nullptr;
   //printf ("JAM*************** DTOR of TGo4BufferQueue %s END\n", GetName());
}

TBuffer * TGo4BufferQueue::WaitBuffer()
{
   GO4TRACE((19,"TGo4BufferQueue::WaitBuffer()", __LINE__, __FILE__));
   TObject *ob = Wait();
   return dynamic_cast<TBuffer *>(ob);
}

TObject * TGo4BufferQueue::WaitObjectFromBuffer()
{
   GO4TRACE((19,"TGo4BufferQueue::WaitObjectFromBuffer()", __LINE__, __FILE__));
   TObject *obj = nullptr;
   TBuffer *buffer = WaitBuffer();
   if(buffer) {
      {
         TGo4LockGuard mainguard;
         // lock go4 main mutex for streaming
         TDirectory* savdir = gDirectory;
         gROOT->cd(); // be sure to be in the top directory when creating histo
         buffer->SetReadMode();
         buffer->Reset();
         buffer->InitMap();
         // note: root version 3.05/02 crashes again when unknown class
         // shall be read; this  was working in 3.03/09
         // therefore, we put in our own check again from revision 1.23
         TClass *cl = buffer->ReadClass();
         if(cl == (TClass*) -1)
         {
            // case of unknown class
            std::cout << " Could not receive object of unknown class on buffer queue "<<GetName() <<" !!!" <<std::endl;
            obj = nullptr;
         } else {
            buffer->Reset();
            obj = buffer->ReadObject(cl);
         }
         if(savdir) savdir->cd();
      } //  TGo4LockGuard
      FreeBuffer(buffer); // give internal buffer back to queue
   } // if(buffer)

   return obj;
}

void TGo4BufferQueue::AddBuffer(TBuffer * buffer, Bool_t clone)
{
   GO4TRACE((19,"TGo4BufferQueue::AddBuffer(TBuffer *, Bool_t)", __LINE__, __FILE__));

   TBuffer *entry = nullptr;
   Bool_t entryisnew = kFALSE;
   if(clone)
      {
      TGo4LockGuard qguard(fxBufferMutex);
         entry = dynamic_cast<TBuffer *>(fxFreeList->Remove(fxFreeList->First()));
         // get next free buffer
         if(!entry)
          {
            // no buffer entry there, we create one
            TGo4Log::Debug(" Buffer Queue adding new internal buffer... ");

            entry = NewEntry();

            fxBufferList->Add(entry); // add to list of existing buffers
                // need not add new buffer to list of free buffers
            entryisnew=kTRUE;
            fiMaxBuffers++;
          }
         else
           {
             // ok, we have a buffer to clone
           }

           Int_t srcsize=buffer->BufferSize();
           Int_t destsize=entry->BufferSize();
           if (srcsize>destsize)
                  {
                     Realloc(entry,destsize,srcsize);
                     destsize=srcsize;
                  }
               else
                  {
                     // buffer big enough, proceed
                  }
           char* source= buffer->Buffer();
           char* destination= entry->Buffer();
            memcpy(destination,source,srcsize);
            Int_t messlen = buffer->Length(); // compatible with root TMessage protocol
            entry->SetBufferOffset(messlen);
            entry->SetByteCount(0);
      } // if(clone)
   else
      {
         entry=buffer;
      }

   try
      {
         Add(entry);
      }
   catch(TGo4RuntimeException &)
      {
         std::cout << "Buffer queue "<< GetName()<<" is full, dropping new entry "<< entry <<" !!!" << std::endl;
         if(entryisnew)
            {
               TGo4LockGuard qguard(fxBufferMutex);
               fxBufferList->Remove(entry); // remove new entry again from pool
               delete entry;
            }
      }
}

void TGo4BufferQueue::AddBufferFromObject(TObject * object)
{
   GO4TRACE((12,"TGo4BufferQueue::AddBufferFromObject(TObject*)", __LINE__, __FILE__));
   if(!object) return;
   TGo4LockGuard mainguard;
   TBuffer *entry = new TBufferFile(TBuffer::kWrite);
   TFile *filsav = gFile;
   gFile = nullptr;
   entry->WriteObject(object);
   gFile = filsav;
////////DEBUG OUTPUT
//         std::cout << "wrote object "<< object->GetName() <<" into message" <<  std::endl;
//
//         entry->SetReadMode();
//         entry->Reset();
//
//         entry->InitMap();
//         TClass *cl= entry->ReadClass();
//         entry->Reset();
//         std::cout << "buffer contains class "<< cl << std::endl;
//         if(cl)
//            {
//               std::cout << "classname "<< cl->GetName() << std::endl;
//
//               TObject *ob=entry->ReadObject(cl);
//               std::cout << "read object "<< ob << std::endl;
//               if(ob)
//                  std::cout << "read object "<< ob->GetName() << std::endl;
//            }
//////////

// do not reset, may need object length information later!
//         entry->Reset();
   try
      {
         Add(entry);
      }
   catch(TGo4RuntimeException &)
      {
         std::cout << "Buffer queue "<< GetName()<<" is full, dropping new entry "<< entry <<" !!!" << std::endl;
         delete entry;
      }
}

void TGo4BufferQueue::FreeBuffer(TBuffer *buffer)
{
   GO4TRACE((19, "TGo4BufferQueue::FreeBuffer(TBuffer *, Bool_t)", __LINE__, __FILE__));
   TGo4LockGuard qguard(fxBufferMutex);
   // does buffer belong to our internal buffers?
   if (fxBufferList->FindObject(buffer)) {
      // yes, put it back into the free list
      // new: we allocate the buffersize back to the initial size to
      // avoid extended memory consumption:
      Int_t memsize = buffer->BufferSize();
      if (memsize > TGo4Socket::fgiBUFINITSIZE) {
         Realloc(buffer, memsize, TGo4Socket::fgiBUFINITSIZE);
      }
      fxFreeList->AddLast(buffer);
   } else {
      // no, we delete it to avoid leakage
      delete buffer;
   }
}

void TGo4BufferQueue::Clear(Option_t *)
{
   while(auto ob = Next())
      FreeBuffer(dynamic_cast<TBuffer *>(ob));
}

void TGo4BufferQueue::Realloc(TBuffer *buffer, Int_t oldsize, Int_t newsize)
{
   if(!buffer) return;
   TGo4LockGuard mainguard;

   buffer->Expand(newsize); // JAM2021- always use framework method to avoid recent check byte count problems
}

TBuffer *TGo4BufferQueue::NewEntry()
{
   TGo4LockGuard mainguard;
   TBuffer *buf = new TBufferFile(TBuffer::kWrite, TGo4Socket::fgiBUFINITSIZE);
   TNamed *dummy = new TNamed("This is a default buffer filler","GO4 is fun!");
   TFile *filsav = gFile;
   gFile = nullptr;
   buf->WriteObject(dummy);
   gFile = filsav;
   delete dummy;
   return buf;
}

TBuffer *TGo4BufferQueue::CreateValueBuffer(UInt_t val)
{
   TBuffer *buf = new TBufferFile(TBuffer::kWrite);
   char *field= buf->Buffer() + sizeof(UInt_t);
   tobuf(field ,val);
   buf->SetBufferOffset( sizeof(UInt_t)+ sizeof(UInt_t) ); // set length for receiver check
   buf->SetByteCount(0); // correctly set first longword
   return buf;
}


Int_t TGo4BufferQueue::DecodeValueBuffer(TBuffer *buf)
{
   if(!buf) return -1;
   UInt_t len = buf->Length();
   if(len != (sizeof(UInt_t)+sizeof(UInt_t))) return -2;
      // note: first length is length of encoded type
                                       // second length is always UInt_t
   char *field = buf->Buffer();
   char *temp = field + sizeof(UInt_t); // skip length header
   UInt_t val = 0;
   frombuf(temp, &val);
   return (Int_t) val;
}
