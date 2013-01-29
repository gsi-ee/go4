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

#include "TGo4ObjClient.h"

#include <signal.h>
#include "Riostream.h"

#include "TROOT.h"
#include "TClass.h"
#include "TDirectory.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Socket.h"
#include "TGo4SocketSignalHandler.h"
#include "TGo4TaskHandler.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4HistogramServer.h"

TGo4ObjClient::TGo4ObjClient() :
   TNamed(),
   fxServerName(),
   fxServerPass(),
   fxHostname(),
   fiPort(0),
   fxTransport(0)
{
}

TGo4ObjClient::TGo4ObjClient(const char* name,
                                 const char* base,
                                 const char* passwd,
                                 const char* host,
                                 Int_t port) :
   TNamed(name, "This is a Go4 object client"),
   fxServerName(),
   fxServerPass(),
   fxHostname(),
   fiPort(0),
   fxTransport(0)
{
   SetParms(base,passwd,host,port);
   fxTransport = new TGo4Socket(kTRUE);
}

TGo4ObjClient::~TGo4ObjClient()
{
   delete fxTransport;
}

void TGo4ObjClient::SetParms(const char* base,
               const char* passwd,
               const char* host,
               Int_t port)
{
   if(base) SetBase(base);
   if(passwd) SetPasswd(passwd);
   if(host) SetHost(host);
   if(port) SetPort(port);
}



TGo4AnalysisObjectNames * TGo4ObjClient::RequestNamesList(const char* base,
                                               const char* passwd,
                                               const char* host,
                                               Int_t port)
{
   //
   TGo4AnalysisObjectNames* nameslist=0;
   SetParms(base,passwd,host,port);
   if(ConnectServer()==0)
   {
      // connection successful, go on
      SendCommand(TGo4HistogramServer::fgcCOMGETLIST); // send nameslist request
      TObject* obj=ReceiveObject(); // get incoming buffer and stream object
      if(obj && obj->InheritsFrom(TGo4AnalysisObjectNames::Class()))
      {
         nameslist=dynamic_cast<TGo4AnalysisObjectNames*>(obj);
      }
      else
      {
         TGo4Log::Error("error receiving nameslist !!!");
      }
   }
   else
   {
      TGo4Log::Error("error on connection in RequestNamesList");
      nameslist=0;
   }
   DisconnectServer();
   return nameslist;
}


TObject* TGo4ObjClient::RequestObject(const char* objectname,
                                       const char* base,
                                       const char* passwd,
                                       const char* host,
                                       Int_t port)
{
   TObject* obj=0;
   SetParms(base,passwd,host,port);
   if(ConnectServer()==0)
   {
      // connection successful, go on
      SendCommand(objectname); // command is name of object to get
      obj=ReceiveObject(); // get incoming buffer and stream object
   }
   else
   {
      TGo4Log::Error("error on connection in RequestObject");
      obj=0;
   }
   DisconnectServer();
   return obj;
}


TObject* TGo4ObjClient::ReceiveObject()
{
   TObject* obj=0;
   // check for OK signal (object is existing on server)
   char* recvchar=fxTransport->RecvRaw("dummy");
   if(recvchar && !strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
   {
      TBuffer* buffer=ReceiveBuffer();
      if(buffer)
      {
         // reconstruct object from buffer
         {
            TGo4LockGuard mainguard;
            // lock go4 main mutex for streaming
            TDirectory* savdir=gDirectory;
            gROOT->cd(); // be sure to be in the top directory when creating histo
            buffer->SetReadMode();
            //cout << "Reading object from buffer..."<< endl;
            buffer->Reset();
            obj=buffer->ReadObject(0); // ReadObject(cl)
            if(obj) std::cout <<"read object of class"<<obj->ClassName() << std::endl;
            savdir->cd();
         } //  TGo4LockGuard
      }
      else
      {
         TGo4Log::Error("error receiving object");
      } // if(buffer)
   }
   else
   {
      TGo4Log::Error("Server did not send object, probably unknown!");
   } // if(!strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
   fxTransport->Send(TGo4TaskHandler::Get_fgcOK()); // final OK
   return obj;
}

void TGo4ObjClient::SendCommand(const char* com)
{
   if (fxTransport) fxTransport->Send(com);
}


Int_t TGo4ObjClient::ConnectServer()
{
char* recvchar=0;
Int_t openok=fxTransport->Open(GetHost(),GetPort());
if(openok!=0) return 1;
// send basename:
fxTransport->Send(GetBase());
recvchar=fxTransport->RecvRaw("dummy");
if(recvchar==0) return 1;
if(strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
{
   TGo4Log::Debug(" Wrong basename for object server (host %s ,port %d)", GetHost(), GetPort());
   return 2;
}
// send password:
fxTransport->Send(GetPasswd());
recvchar=fxTransport->RecvRaw("dummy");
if(recvchar==0) return 1;
if(strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
{
   TGo4Log::Debug(" Wrong password for object server (host %s ,port %d)", GetHost(), GetPort());
   return 4;
}
return 0;
}

Int_t TGo4ObjClient::DisconnectServer()
{
   fxTransport->Close();
return 0;
}

TBuffer* TGo4ObjClient::ReceiveBuffer()
{

TBuffer* rev=0;
Int_t state=fxTransport->ReceiveBuffer();
if(state>=0)
   {
      rev=const_cast<TBuffer*> (fxTransport->GetBuffer());
      // currently, we pass out the internal buffer of the TGo4Socket
      // maybe we have to put a queue of length 1 in between?
      //fxBufferQueue->AddBuffer(buf, kTRUE);
   } //// if(rev>=0)
else
   {
      // error
      if (TGo4SocketSignalHandler::IsLastSignalWINCH())
        {
            // TSocket error because of window resize, do not abort!
            TGo4Log::Debug(" %s: caught SIGWINCH ",GetName());
            TGo4SocketSignalHandler::SetLastSignal(0); // reset
            rev = 0;
         }
      else
         {
            TGo4Log::Debug(" !!!Receive Error in Object Client %s!!!",GetName());
            rev=0;  // here we might throw some exception later....
          }
   } // end if(rev>=0)
return rev;
}
