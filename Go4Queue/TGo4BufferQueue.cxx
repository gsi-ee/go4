#include "TGo4BufferQueue.h"

#include "Riostream.h"

#include "TROOT.h"
#include "TMutex.h"
#include "TFile.h"
#include "TGo4Buffer.h"
#include "RVersion.h"

#include "TGo4Socket.h"
#include "TGo4RuntimeException.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#if ROOT_VERSION_CODE > ROOT_VERSION(4,3,2)
//#if __GO4ROOTVERSION__ > 40302
   const Int_t TGo4BufferQueue::fgiISOWNER=TBuffer::kIsOwner;
#else
   const Int_t TGo4BufferQueue::fgiISOWNER=BIT(14);
// we emulate the protected owner flag of the TBuffer class, needed for reallocation!
#endif



TGo4BufferQueue::TGo4BufferQueue()
   : TGo4Queue("Default buffer queue"),fiOverflowcount(0), fiMaxBuffers(10)
{
   TRACE((14,"TGo4BufferQueue::TGo4BufferQueue()", __LINE__, __FILE__));

   InitBuffers();
}

TGo4BufferQueue::TGo4BufferQueue(const char* name)
: TGo4Queue(name), fiOverflowcount(0), fiMaxBuffers(10)

{
   TRACE((14,"TGo4BufferQueue::TGo4BufferQueue(Text_t*)", __LINE__, __FILE__));
   InitBuffers();
}

void TGo4BufferQueue::InitBuffers()

{
   TRACE((14,"TGo4BufferQueue::InitBuffers()", __LINE__, __FILE__));
   TGo4LockGuard mainguard;
   fxBufferList=new TList; // list owning all buffers
   fxFreeList=new TList;    // list indicating the free buffers
   fxBufferIterator=fxFreeList->MakeIterator();
   fxBufferMutex=new TMutex;
   for (Int_t i=0; i< fiMaxBuffers; ++i)
      {
          TBuffer* buf=NewEntry();
          fxBufferList->Add(buf);
          fxFreeList->Add(buf);
      }
}

TGo4BufferQueue::~TGo4BufferQueue()
{
   TRACE((14,"TGo4BufferQueue::~TTGo4BufferQueue()", __LINE__, __FILE__));

   delete fxBufferIterator;
   fxBufferList->Delete();
   TCollection::EmptyGarbageCollection();
   delete fxBufferList;
   delete fxBufferMutex;
}

TBuffer * TGo4BufferQueue::WaitBuffer()
{
   TRACE((19,"TGo4BufferQueue::WaitBuffer()", __LINE__, __FILE__));
   TObject* ob = Wait();
   TBuffer* rev= dynamic_cast<TBuffer*> ( ob );
   return rev;
}


TObject * TGo4BufferQueue::WaitObjectFromBuffer()
{
TRACE((19,"TGo4BufferQueue::WaitObjectFromBuffer()", __LINE__, __FILE__));
   TObject* obj=0;
   TBuffer* buffer=WaitBuffer();
   if(buffer)
       {
            {
            TGo4LockGuard mainguard;
            // lock go4 main mutex for streaming
            TDirectory* savdir=gDirectory;
            gROOT->cd(); // be sure to be in the top directory when creating histo
            buffer->SetReadMode();
            //cout << "Reading object from buffer..."<< endl;
            buffer->Reset();
            buffer->InitMap();
            // note: root version 3.05/02 crashes again when unknown class
            // shall be read; this  was working in 3.03/09
            // therefore, we put in our own check again from revision 1.23
            TClass* cl = buffer->ReadClass();
            //cout << "buffer queue "<< GetName() <<" :waitobject, got Class: " << cl << endl;
            if(cl == (TClass*) -1)
               {
                  // case of unknown class
                  cout << " Could not receive object of unknown class on buffer queue "<<GetName() <<" !!!" <<endl;
                  obj=0;
               }
            else
               {
//                  if(cl)
//                     cout << "Classname: " << cl->GetName() << endl;
//                  cout << "Reading object from buffer..."<< endl;
                  buffer->Reset();
                  obj=buffer->ReadObject(cl);
              }
            if(savdir) savdir->cd();
            } //  TGo4LockGuard
         FreeBuffer(buffer); // give internal buffer back to queue
         } // if(buffer)
   else
     {
   //  cout << "WaitObjectFromBuffer got zero buffer in queue "<<GetName() << endl;
     }
   return obj;
}

void TGo4BufferQueue::AddBuffer(TBuffer * buffer, Bool_t clone)
{
   TRACE((19,"TGo4BufferQueue::AddBuffer(TBuffer*, Bool_t)", __LINE__, __FILE__));

   TBuffer* entry=0;
   Bool_t entryisnew=kFALSE;
   if(clone)
      {
      TGo4LockGuard qguard(fxBufferMutex);
         entry= dynamic_cast<TBuffer*>(fxFreeList->Remove(fxFreeList->First()));
                // get next free buffer
         if(entry==0)
          {
            // no buffer entry there, we create one
            TGo4Log::Debug(" Buffer Queue adding new internal buffer... ");
            //cout <<"Buffer Queue: creating new internal buffer... "<< GetName();

            entry=NewEntry();
            fxBufferList->Add(entry); // add to list of existing buffers
                // need not add new buffer to list of free buffers
            entryisnew=kTRUE;
            fiMaxBuffers++;
            //cout <<"buffers:"<<fiMaxBuffers <<", "<< fiMaxBuffers*TGo4Socket::fgiBUFINITSIZE << endl;
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
            memcpy(destination,source,destsize);
            //cout <<"))))))))))Buffer Queue: copied "<< destsize <<"bytes to buffer field" << endl;
            Int_t messlen = buffer->Length(); // compatible with root TMessage protocol
            entry->SetBufferOffset(messlen);
            entry->SetByteCount(0);
      } // if(clone)
   else
      {
         entry=buffer;
      }
   //cout <<"Bufferqueue AB adding buffer "<< entry << endl;

   try
      {
         Add(entry);
      }
   catch(TGo4RuntimeException ex)
      {
         cout << "Buffer queue "<< GetName()<<" is full, dropping new entry "<< entry <<" !!!" << endl;
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
   TRACE((12,"TGo4BufferQueue::AddBufferFromObject(TObject*)", __LINE__, __FILE__));
   if(object==0) return;
   TGo4LockGuard mainguard;
   TBuffer* entry = 0;
   entry = new TGo4Buffer(TBuffer::kWrite);
   //cout <<" Buffer  queue ABFO created buffer "<<entry << endl;
   TFile *filsav = gFile;
   gFile = 0;
   entry->WriteObject(object);
   gFile = filsav;
////////DEBUG OUTPUT
//         cout << "wrote object "<< object->GetName() <<" into message" <<  endl;
//
//         entry->SetReadMode();
//         entry->Reset();
//
//         entry->InitMap();
//         TClass* cl= entry->ReadClass();
//         entry->Reset();
//         cout << "buffer contains class "<< cl << endl;
//         if(cl)
//            {
//               cout << "classname "<< cl->GetName() << endl;
//
//               TObject* ob=entry->ReadObject(cl);
//               cout << "read object "<< ob << endl;
//               if(ob)
//                  cout << "read object "<< ob->GetName() << endl;
//            }
//////////

// do not reset, may need object length information later!
//         entry->Reset();
   try
      {
         Add(entry);
      }
   catch(TGo4RuntimeException& ex)
      {
         cout << "Buffer queue "<< GetName()<<" is full, dropping new entry "<< entry <<" !!!" << endl;
         delete entry;
      }

 }

void TGo4BufferQueue::FreeBuffer(TBuffer * buffer)
{
   TRACE((19,"TGo4BufferQueue::FreeBuffer(TBuffer*, Bool_t)", __LINE__, __FILE__));
   //
   TGo4LockGuard qguard(fxBufferMutex);
   //cout << "bufferlock acquired by bufferqueue: freebuffer"<< endl;
   // does buffer belong to our internal buffers?
   if (fxBufferList->FindObject(buffer)!=0)
            {
               // yes, put it back into the free list
                // new: we allocate the buffersize back to the initial size to
                // avoid extended memory consumption:
                Int_t memsize=buffer->BufferSize();
                if (memsize>TGo4Socket::fgiBUFINITSIZE)
                  {
                     Realloc(buffer,memsize, TGo4Socket::fgiBUFINITSIZE);
                  }
                fxFreeList->AddLast(buffer);
                //cout <<"freed buffer"<< buffer <<" of bufferqueue "<< GetName() << endl;
            }
         else
            {
               // no, we delete it to avoid leakage
               delete buffer;
               //cout <<" Buffer  queue FB deleted buffer "<<buffer << endl;
               //cout <<"deleted external buffer of bufferqueue "<< GetName() << endl;
               // TGo4Log::Debug(" Buffer Queue : deleted external buffer !!! ");

            }




}

void TGo4BufferQueue::Clear(Option_t* opt)
{
   TObject* ob=0;
   while((ob=Next()) !=0)
      {
         //cout <<"cleared entry "<<ob<<" of queue "<<GetName() << endl;
         FreeBuffer(dynamic_cast<TBuffer*> (ob) );
      }

}




void TGo4BufferQueue::Realloc(TBuffer* buffer, Int_t oldsize, Int_t newsize)
{
if(buffer==0) return;
TGo4LockGuard mainguard;
   char* memfield=buffer->Buffer();
   //buffer->Expand(newsize); // is protected! we make it by hand...
   //Int_t current = buffer->Length(); // cursor position
   Int_t extraspace=TGo4Socket::fgiBUFEXTRASPACE; // =8, constant within TBuffer
//   memfield = (char *) TStorage::ReAlloc(memfield,
//                                           (newsize + extraspace) * sizeof(char),
//                                           (oldsize+ extraspace) * sizeof(char));
   // this works only for ROOT versions > 3.02/04
   memfield = TStorage::ReAllocChar(memfield,
                                           (newsize+extraspace),
                                           (oldsize+extraspace));
   //cout << "Bufferqueue reallocating char from"<<oldsize<< " to " << newsize<< endl;
   buffer->ResetBit(fgiISOWNER);
#if ROOT_VERSION_CODE > ROOT_VERSION(5,23,2)
   buffer->SetBuffer(memfield, newsize + extraspace);
#else
   buffer->SetBuffer(memfield, newsize);
#endif
   buffer->SetBit(fgiISOWNER);
   // <- here we avoid the ownership of TBuffer for the internal buffer
   // (new feature of ROOT versions > 3.02/04)
   // problem: SetBuffer will delete previous buffer in adopt mode (isowner=true)
   // which might be at the same location as the new buffer after ReAlloc
   // ergo SetBuffer would set a buffer which it deleted before itself!
   buffer->SetBufferOffset(newsize);
}

TBuffer* TGo4BufferQueue::NewEntry()
{
   TGo4LockGuard mainguard;
   TBuffer* buf = new TGo4Buffer(TBuffer::kWrite, TGo4Socket::fgiBUFINITSIZE);
   //cout <<"nnnnnnnn BufferQueue "<<GetName()<<" made new entry "<<buf << endl;
   TNamed* dummy= new TNamed("This is a default buffer filler","GO4 is fun!");
   TFile *filsav = gFile;
   gFile = 0;
   buf->WriteObject(dummy);
   gFile = filsav;
   delete dummy;
   return buf;
}

TBuffer* TGo4BufferQueue::CreateValueBuffer(UInt_t val)
{
   TGo4Buffer* buf= new TGo4Buffer(TBuffer::kWrite);
   char* field= buf->Buffer() + sizeof(UInt_t);
   tobuf(field ,val);
   buf->SetBufferOffset( sizeof(UInt_t)+ sizeof(UInt_t) ); // set length for receiver check
   buf->SetByteCount(0); // correctly set first longword
   return buf;
}


Int_t TGo4BufferQueue::DecodeValueBuffer(TBuffer* buf)
{
if(buf==0) return -1;
UInt_t len= buf->Length();
if(len==sizeof(UInt_t)+sizeof(UInt_t)) // note: first length is length of encoded type
                                       // second length is always UInt_t
   {
     char* field=buf->Buffer();
     char* temp= field + sizeof(UInt_t); // skip length header
     UInt_t val=0;
     frombuf(temp, &val);
     //cout <<"DDDDDD DecodeValueBuffer val="<<val << endl;
     return (Int_t) val;
   }
else
   {
     return -2;
   }
}
