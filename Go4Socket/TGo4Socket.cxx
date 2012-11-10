// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4Socket.h"

#include <string.h>

#include "go4iostream.h"
#include "TMutex.h"
#include "TMessage.h"
#include "TSystem.h"
#include "TSocket.h"
#include "TServerSocket.h"
#include "RVersion.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Buffer.h"
#include "TGo4SocketSignalHandler.h"

const Int_t TGo4Socket::fguOPENWAIT=200;
const Int_t TGo4Socket::fgiOPENCYCLES=6;
const Int_t TGo4Socket::fgiBUFLENGTH=256;
const Int_t TGo4Socket::fgiBUFINITSIZE=65536;
const Int_t TGo4Socket::fgiBUFEXTRASPACE=8;
const char* TGo4Socket::fgcGOON = "-I- go on";

// note: ownwership bit changed for newer root versions!!
#if ROOT_VERSION_CODE > ROOT_VERSION(4,3,2)
//#if __GO4ROOTVERSION__ > 40302
   const Int_t TGo4Socket::fgiISOWNER=TBuffer::kIsOwner;
#else
   const Int_t TGo4Socket::fgiISOWNER=BIT(14);
   // have to emulate the protected owner flag of the TBuffer class, needed for reallocation!
#endif



TGo4Socket::TGo4Socket(Bool_t IsClient) :
    fbClientMode(IsClient),
    fbOpen(kFALSE),
    fxSocket(0),
    fxServerSocket(0),
    fiPort(0),
    fxLocalBuffer(0),
    fxBuffer(0)
{
   TRACE((14,"TGo4Socket::TGo4Socket(Bool_t)", __LINE__, __FILE__));

   fxBuffer = new TGo4Buffer(TBuffer::kRead, TGo4Socket::fgiBUFINITSIZE);

   // SL: 10-11-12  no idea why this handler was set
   TGo4SocketSignalHandler::SetSigWINCH();

   // SL: 10-11-12  do not crash program when socket broken
   TGo4SocketSignalHandler::DisableSigPipe();

   fxLocalBuffer = new char [TGo4Socket::fgiBUFLENGTH];
   if(fbClientMode)
      TGo4Log::Debug(" Created new Go4Socket in client mode ");
   else
      TGo4Log::Debug(" Created new Go4Socket in server mode ");
}

TGo4Socket::~TGo4Socket()
{
   TRACE((14,"TGo4Socket::~TGo4Socket()", __LINE__, __FILE__));

   if (fxBuffer!=0) {
      // SL: 10-11-12 fix memory leak
      delete fxBuffer;
      fxBuffer = 0;
   }

   if (fxLocalBuffer) {
      delete [] fxLocalBuffer;
      fxLocalBuffer = 0;
   }

   if(fxSocket)  {
      fxSocket->Close();
      delete fxSocket;
      fxSocket = 0;
   }

   if(fxServerSocket) {
      fxServerSocket->Close();
      delete fxServerSocket;
      fxServerSocket = 0;
   }
}

Int_t TGo4Socket::Open(const char* host, Int_t port, Bool_t keepservsock)
{
   TRACE((12,"TGo4Socket::Open(const char* host, Int_t port)", __LINE__, __FILE__));

   if(fbOpen) return 1;

   Int_t rev=0;

   if(fbClientMode) {
      // client socket
      fxSocket = new TSocket(host, port);
      Int_t maxcounter=0;
      while(!fxSocket->IsValid())
      {
         if(++maxcounter> TGo4Socket::fgiOPENCYCLES)
         {
            TGo4Log::Debug(" Socket: Open timeout!!");
            break;
         }
         else
         {
            TGo4Log::Debug(" Socket: Open retry %d ", maxcounter);
            delete fxSocket;
            gSystem->Sleep(TGo4Socket::fguOPENWAIT);
            fxSocket = new TSocket(host, port);
         }
      } // while()
      if(!fxSocket->IsValid())
      {
         TGo4Log::Debug(" Socket: Open(const char* host, Int_t port ) as Client failed ");
         fiPort=0;
         return -8;
      }
      else
      {
         fxSocket->SetOption(kNoBlock,0); // set blocking
         fiPort=fxSocket->GetLocalPort(); // success, get real port number
      }
      char str[32];
      fxSocket->Recv(str, 32);
      if(!strcmp(str,TGo4Socket::fgcGOON))
      {
         //cout << "-I- We are told to go on !\n";
         fbOpen = kTRUE;
         rev=0;
         TGo4Log::Debug(" Socket: Connection Established ");
      }
      else
      {
         TGo4Log::Debug(" Socket: !!! Received unknown string !!! ");
      }
   }
   else // if(fbClientMode)
   {
      // server socket:
      if(!fxServerSocket)
      {
         fxServerSocket = new TServerSocket(port, kFALSE); // for portscan
         fxServerSocket->SetOption(kNoBlock,1);
      }
      else
      {
         // do nothing, we poll existing server socket
      }
      if(!fxServerSocket->IsValid())
      {
         TGo4Log::Debug(" Socket: Open(const char* host,  Int_t port) as Server failed ");
         fiPort=0;
         return -8;
      }
      else
      {
         fiPort=fxServerSocket->GetLocalPort(); // success, get real port number
         //cout << " ---- Go4 Socket got local port "<< fiPort  << endl;
      }
      fxSocket = 0;
      while(1)
      {
         if(!fxSocket)
         {
            fxSocket = fxServerSocket->Accept();
            if(!fxSocket || fxSocket==(TSocket*) (-1) )
            {
               fxSocket=0;
               rev=-1;
               break;
               // no connection request, leave loop
            }
            else
            {
               fxSocket->SetOption(kNoBlock,0); // server socket was non blocking, but socket shall block in wait!
               fxSocket->Send(TGo4Socket::fgcGOON);
               if(!keepservsock)
               {
                  fxServerSocket->Close();
                  delete fxServerSocket;
                  fxServerSocket = 0;
               }
               else
               {
                  // do not delete serversocket in "keep" mode
                  // more performant for servers that should be
                  // idle quickly for repeated client requests
               }// if(!keepservsock)
               fbOpen = kTRUE;
               rev=0;
               break;
            } // if(!fxSocket || fxSocket==(TSocket*) (-1) )
         }
         else
         {
            rev=-2; // TSocket already existing
            break;
         } // if(!fxSocket)
      } // while(1)
   } // end if(fbClientMode)
   return rev;
}


Int_t TGo4Socket::Close(Option_t *opt)
{
   TRACE((12,"TGo4Socket::Close()", __LINE__, __FILE__));
   if(!fbOpen) return 1;

   fxSocket->Close(opt);
   fbOpen = kFALSE;
   return 0;
}

Int_t TGo4Socket::SendBuffer(TBuffer* buf)
{
   TRACE((12,"TGo4Socket::SendBuffer(TBuffer*)", __LINE__, __FILE__));

   if(!buf) {
      TGo4Log::Debug(" !!! Socket: SendBuffer() ERROR : no buffer  !!! ");
      return -2;
   }

   if(!IsOpen()) {
      TGo4Log::Debug(" !!! Socket: SendBuffer() ERROR : socket not open  !!! ");
      return -1;
   }

   if(!fxSocket) {
      TGo4Log::Debug(" !!! Socket: SendBuffer() ERROR : no TSocket !!! ");
      return -5;
   }  // end if(fxSocket)


   UInt_t len = buf->Length();
   char* field = buf->Buffer();

   if(field==0) {
      TGo4Log::Debug(" !!! Socket: SendBuffer() ERROR : no data field !!! ");
      return -5;
    }

   char* temp = field;
   tobuf(temp, (UInt_t)(len - sizeof(UInt_t))); // tobuf changes pointer!
   //set length into first word of buffer (see TMessage)
   ////// debug://///////////////////////////////////////////////////////////////////////
   //                        cout << "))))))))) socket send: buffer length in buffer ";
   //                        UInt_t dummy=0;
   //                        temp=field;
   //                        frombuf(temp,&dummy);
   //                        cout << dummy << endl;
   //////////////////////////////////////////////////////////////////////////////////////

   Int_t rev = fxSocket->SendRaw(field,len);
   // raw send complete buffer

   if(rev>0) return 0;

   // error on send
   TGo4Log::Debug(" !!! Socket: SendBuffer() ERROR # %d !!! ",rev);
   return rev;
}

Int_t TGo4Socket::ReceiveBuffer()
{
   TRACE((12,"TGo4Socket::ReceiveBuffer()", __LINE__, __FILE__));

   if(!IsOpen()) {
       TGo4Log::Debug(" !!! Socket: ReceiveBuffer() ERROR : not open  !!! ");
       return -11;
   }

   if(!fxSocket) {
      TGo4Log::Debug(" !!! Socket: ReceiveBuffer() ERROR : no TSocket !!! ");
      return -10;
   }  // end if(fxSocket)

   UInt_t len = 0;
   // first receive length of following buffer
   Int_t rev = fxSocket->RecvRaw(&len, sizeof(UInt_t));
   if(rev<= 0) {
      // error on receive
      //TGo4Log::Debug(" !!! Socket: ReceiveBuffer() -- receive length ERROR # %d !!! ",rev);
      // no output here, we will redirect cout client runnable (command)
      return -55;
   }
   // check if length exceeds receive buffer
   len = net2host(len);  //from network to host byte order
   Int_t messlen = len + sizeof(UInt_t); // compatible with root TMessage protocol
   //               cout << "))))))))) socket receive: buffer length "<< len << endl;
   //               cout << "))))))))) socket receive: messlen "<< messlen << endl;
   Int_t oldsize = fxBuffer->BufferSize();
   Int_t newsize = messlen;
   if(newsize>oldsize) {
      ReallocBuffer(fxBuffer, oldsize, newsize);
   } else
   if(newsize<oldsize && oldsize>TGo4Socket::fgiBUFINITSIZE) {
      //cout << "))))))))) socket receive shorter messlen "<< messlen << endl;
      if(newsize < TGo4Socket::fgiBUFINITSIZE)
         newsize = TGo4Socket::fgiBUFINITSIZE;
      ReallocBuffer(fxBuffer, oldsize, newsize);
   }
   // read object buffer into receive buffer:
   char* buf = fxBuffer->Buffer() +sizeof(UInt_t);
   // skip first word, see TMessage transport
   rev = fxSocket->RecvRaw((void*) buf, len);
   if(rev <= 0) {
      // error on receive
      TGo4Log::Debug(" !!! Socket: ReceiveBuffer() ERROR # %d !!! ",rev);
      return -56;
   }
      //                     cout << "socket: received raw "<< rev << " bytes" << endl;
   // set root byte count for correct object reconstruction:
   fxBuffer->SetBufferOffset(messlen);
   fxBuffer->SetByteCount(0);
   return 0;
}

Int_t TGo4Socket::Send(TObject *obj)
{
   TRACE((12,"TGo4Socket::Send(TObject *obj)", __LINE__, __FILE__));

   Int_t rev=0;
   if(IsOpen())
   {
      {
         TGo4LockGuard init; // initialize main mutex
      }
      TGo4LockGuard::LockMainMutex();
      TMessage mess(kMESS_OBJECT);
      mess.WriteObject(obj);
      TGo4LockGuard::UnLockMainMutex();
      if(fxSocket)
      {
         rev = fxSocket->Send(mess);
      }
      else
      {
         rev=-16;
      } // end if(fxSocket)
   }    // if(IsOpen())
   else {
      // do not send on closed socket
      rev=-32;
   } // end   if(IsOpen() && IsActive())

   if(rev < 0)
      TGo4Log::Debug(" !!! Socket: Send(TObject*) ERROR # %d !!! ",rev);

   return rev;
}

Int_t TGo4Socket::Send(const char* name)
{
   TRACE((12,"TGo4Socket::Send(const char* name)", __LINE__, __FILE__));

   Int_t rev=0;
   if(IsOpen())
      {
         if(fxSocket)
            {
               strncpy(fxLocalBuffer,name, TGo4Socket::fgiBUFLENGTH-1);
               rev = fxSocket->SendRaw(fxLocalBuffer,TGo4Socket::fgiBUFLENGTH);
            }
         else
            {
               rev=-16;
            } // end if(fxSocket)
      }    // if(IsOpen())
   else
      {
         // do not send on closed socket
         rev=-32;
      } // end   if(IsOpen())

   if(rev < 0)
      {
         TGo4Log::Debug(" !!! Socket: Send(const char*) ERROR # %d !!! ",rev);
      }
   return rev;
}

char* TGo4Socket::RecvRaw(const char* name)
{
// note: optional parameter const char* name is left for compatibility, has no effect!
   TRACE((12,"TGo4Socket::RecvRaw(const char* name)", __LINE__, __FILE__));

   char* revchar;
   if(IsOpen())
      {
         if(fxSocket)
            {
               Int_t rev=fxSocket->RecvRaw(fxLocalBuffer, TGo4Socket::fgiBUFLENGTH);
               if(rev<= 0)
                  {
                     // error on receive
                     TGo4Log::Debug(" !!! Socket: RecvRaw(const char*) ERROR # %d !!! ",rev);
                     revchar=0;
                  }
               else
                  {
                     revchar=fxLocalBuffer;
                  }
             } // if(fxSocket)
         else
            {
               TGo4Log::Debug(" !!! Socket: Recv(const char*) ERROR : no TSocket !!! ");
               revchar=0;
            }  // end if(fxSocket)
      }
   else // if(IsOpen())
      {
         TGo4Log::Debug(" !!! Socket: Recv(const char*) ERROR : not open or not active !!! ");
         revchar=0;
      }
   return revchar;
}



TObject* TGo4Socket::Recv(const char* name)
{
   // note: optional parameter const char* name is left for compatibility, has no effect!
   TRACE((12,"TGo4Socket::Recv(const char* name)", __LINE__, __FILE__));

   TObject* obj=0;
   Int_t rev=0;
   if(IsOpen())
   {
      TMessage *mess;
      if(fxSocket)
      {
         rev=fxSocket->Recv(mess);
         if(mess == 0)
         {
            //cout << "TGo4SocketTransportImp: zero mess" << endl;
            obj=0;
         }
         else
         {
            TGo4LockGuard socker;
            if(mess->What() == kMESS_OBJECT)
            {
               obj = mess->ReadObject(mess->GetClass());
            }
            delete mess;
         } // end if(mess == NULL)

      } // if (fxSocket)
      else
      {
         TGo4Log::Debug(" !!! Socket: Recv(TMessage*) ERROR : no TSocket! ");
      } // end if (fxSocket)
   }
   else // if(IsOpen())
   {
      TGo4Log::Debug(" !!! Socket: Recv(TMessage*) ERROR : not open or not active! ");
   }
   return obj;
}


void TGo4Socket::ReallocBuffer(TBuffer* buffer, Int_t oldsize, Int_t newsize)
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
   // this works only for ROOT versions > 3.02/04:
   memfield = TStorage::ReAllocChar(memfield,
         (newsize+extraspace),
         (oldsize+extraspace));
   //cout << "Socket reallocating char receive buffer from "<<oldsize<< " to " << newsize<< endl;
   buffer->ResetBit(fgiISOWNER);
   //#if ROOT_VERSION_CODE > ROOT_VERSION(5,23,2)
   //   buffer->SetBuffer(memfield, newsize + extraspace);
   //#else
   buffer->SetBuffer(memfield, newsize);
   //#endif
   buffer->SetBit(fgiISOWNER);
   // <- here we avoid the ownership of TBuffer for the internal buffer
   // (new feature of ROOT versions > 3.02/04)
   // problem: SetBuffer will delete previous buffer in adopt mode (isowner=true)
   // which might be at the same location as the new buffer after ReAlloc
   // ergo SetBuffer would set a buffer which it deleted before itself!
   buffer->SetBufferOffset(newsize);
}
