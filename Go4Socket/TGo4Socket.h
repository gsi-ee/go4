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

#ifndef TGO4SOCKET_H
#define TGO4SOCKET_H

#include "Rtypes.h"

class TObject;
class TSocket;
class TServerSocket;

class TGo4Socket {
  public:
     TGo4Socket(Bool_t IsClient);
     virtual ~TGo4Socket();

     virtual Int_t Open(const char* host, Int_t port, Bool_t keepservsock=kFALSE);
     virtual Int_t Close(Option_t *opt="");

     virtual Int_t Send(TObject *obj);
     virtual Int_t Send(const char* name);
     virtual TObject* Recv(const char* name=0);
     virtual char* RecvRaw(const char* name=0);

     Bool_t IsOpen() const { return fbOpen; }

     Bool_t IsClientMode() const { return fbClientMode; }

     Int_t GetPort() const { return fiPort; }

     /**
      * Send raw buffer of external TBuffer object. Method will prepare raw buffer
      * as a TMessage, i.e. the receiving side should be able to receive it by
      * root TSocket message protocol.
      */
     Int_t SendBuffer(TBuffer* buf);

     /**
      * Receive raw buffer and fill the internal TBuffer of this socket instance
      * with it.
      */
     Int_t ReceiveBuffer();

     /**
      * Access the internal receiving buffer of this socket.
      */
     const TBuffer * GetBuffer() const { return fxBuffer; }

     /** Time to sleep between client open retries */
     static const Int_t fguOPENWAIT;

     /** Number of retries for client open */
     static const Int_t fgiOPENCYCLES;

     /** Raw transport buffer size */
     static const Int_t fgiBUFLENGTH;

     /** Initial size for object receive buffer (TBuffer) */
     static const Int_t fgiBUFINITSIZE;

     /** ROOT internal constant: extra space for reallocating object receive buffer (TBuffer) */
     static const Int_t fgiBUFEXTRASPACE;

     /** First connection string, we are told to go on... */
     static const char* fgcGOON;   //!

     /** We emulate the protected owner flag of the TBuffer class, needed for reallocation! */
     static const Int_t fgiISOWNER;

  protected:
    TGo4Socket();  // not implemented, only for compiler to avoid calling of default

    /** Reallocate the fixed input buffer if necessary */
    void ReallocBuffer(TBuffer* buffer, Int_t oldsize, Int_t newsize);

  private:

    /** True if instance runs in as client */
    Bool_t fbClientMode;

    /** True if connection is open */
    Bool_t fbOpen;

    /** root TSocket class doing the work */
    TSocket * fxSocket;

    /** Server Socket (server mode only) */
    TServerSocket * fxServerSocket;

    /** Connected port number (of server socket which opened with portnumberscan) */
    Int_t fiPort;

    char* fxLocalBuffer; //!

    /** @link aggregation
     *   @supplierCardinality 1 */
    TBuffer * fxBuffer;
};

#endif
