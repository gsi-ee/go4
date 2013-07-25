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

#include "TGo4TaskManager.h"

#include "Riostream.h"
#include "TObjArray.h"
#include "TMutex.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Thread.h"
#include "TGo4Socket.h"
#include "TGo4TaskHandler.h"
#include "TGo4TerminateException.h"
#include "TGo4ServerTask.h"

const Int_t TGo4TaskManager::fgiDISCONCYCLES=360; // wait cycles 180

const UInt_t TGo4TaskManager::fguDISCONTIME=500; // time in ms 1000

TGo4TaskManager::TGo4TaskManager(const char* name,
                                 TGo4ServerTask * server,
                                 UInt_t negotiationport,
                                 Bool_t createconnector)
: TNamed(name,"This is a Go4TaskManager"),
   fuTaskCount(0),fuNegotiationPort(0), fbClientIsRemoved(kFALSE), fbHasControllerConnection(kFALSE)
{
   fxServer=server;
 // set port number for the client server negotiation channel:
   if(negotiationport==0)
      {
         // default: use taskhandler intrinsic port number
         fuNegotiationPort=TGo4TaskHandler::fguCONNECTORPORT;
      }
   else
      {
         // use dynamic port number given by main program
         fuNegotiationPort=negotiationport;
      }

   fxListMutex=new TMutex(kTRUE);
   fxTaskList=new TObjArray;
   fxTaskIter=fxTaskList->MakeIterator();
   fxTransport=0;
   if(createconnector)
      {
       // this mode is for server task created on the fly
       // connector should be available immediately, independent of timer connect!
       TGo4Log::Debug("TaskManager: Created negotiation channel in ctor");
       fxTransport=new TGo4Socket(kFALSE); // use raw transport for negotiations
       fxTransport->Open( "Server mode does not need hostname", 0, kTRUE);
       // note: Open() return value is not 0 here, since we do not have
       // accept finished yet! but portnumber is ready after this...
      }
}

TGo4TaskManager::~TGo4TaskManager()
{
   if(fxTransport!=0) {
      fxTransport->Close();
      delete fxTransport;
      fxTransport=0;
   }
   delete fxTaskIter;
   delete fxTaskList;
   delete fxListMutex;
}

Int_t TGo4TaskManager::ServeClient()
{
   //std::cout <<"EEEEEEEEEEEnter TGo4TaskManager::ServeClient()" << std::endl;
   Int_t rev=0;
   char* recvchar=0;
   TString cliname, hostname;
   // open connection in server mode with default port as raw Socket, wait for client
   if(!fxTransport)
      {
         //std::cout << "+++++TaskManager creating new negotiation transport server instance" << std::endl;
         fxTransport=new TGo4Socket(kFALSE); // use raw transport for negotiations
      }
   // we delegate the actual TSocket open to the taskconnector timer:
   fxServer->SetConnect(fxTransport, "Server mode does not need hostname", 0, kTRUE);
      // for portscan, we keep exisiting server socket (keepserv=kTRUE)
   Int_t waitresult=fxServer->WaitForOpen(); // wait for the server Open() call by timer
   if(waitresult<0)
   {
      // open timeout
      TGo4Log::Debug(" TaskManager: Negotiation channel open TIMEOUT");
      std::cerr <<" TaskManager TIMEOUT ERROR opening socket connection !!! Terminating..." << std::endl;
      throw TGo4TerminateException(fxServer);
      //return kFALSE;
   }
   Int_t count=0;
   while(GetNegotiationPort()==0)
   {
      if(count>TGo4TaskHandler::Get_fgiPORTWAITCYCLES())
            {
               TGo4Log::Debug(" TaskManager: Negotiation port getter TIMEOUT");
               std::cerr <<" TaskManager TIMEOUT ERROR retrieving port number  !!! Terminating..." << std::endl;
               throw TGo4TerminateException(fxServer);
               //return kFALSE;
            }
      else if(fxServer->IsTerminating())
            {
              //std::cout << "TTTTTT  ServeClient sees terminating state and returns -1" << std::endl;
               return -1;
            }
      else
            {
              TGo4Thread::Sleep(TGo4TaskHandler::Get_fguPORTWAITTIME());
              ++count;
            }
   }
   std::cout << " Waiting for client connection on PORT: "<< fuNegotiationPort << std::endl;
   TGo4Log::Debug(" TaskManager is waiting to serve client request on port %d ... ",
            fuNegotiationPort);
   Int_t connectwaitseconds=fxServer->WaitForConnection(); // timer tells us by flag when the transport is opened

   if(connectwaitseconds<0)
      {
         // case of threadmanager termination:
         // connector runnable shall stop on return from ServeClient method
         return connectwaitseconds;
      }
   else
      {
         // just proceed to the client server negotiations
      }

   // check connected client: we expect correct ok string
//   recvchar=fxTransport->RecvRaw("dummy");
//   if(recvchar && !strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
     Go4CommandMode_t account=ClientLogin();
     if(account!=kGo4ComModeRefused)
         {
         // client knows task handler, we keep talking
         //
         fxTransport->Send(TGo4TaskHandler::Get_fgcOK()); // handshake to assure the client
         recvchar = fxTransport->RecvRaw("dummy");
         cliname = recvchar; // get the client name
         recvchar = fxTransport->RecvRaw("dummy");
         hostname = recvchar; // get the host name
         //
         // check for connect or disconnect:
         //
         recvchar=fxTransport->RecvRaw("dummy"); // get string to tell us what to do...
         if(recvchar && !strcmp(recvchar,TGo4TaskHandler::fgcCONNECT))
            {
               // request to connect a new client
               rev  = ConnectClient(cliname,hostname,account);
            }
         else if(recvchar && !strcmp(recvchar,TGo4TaskHandler::fgcDISCONNECT))
            {
               // request to disconnect an existing client
             rev = DisConnectClient(cliname);
            }
         else
            {
               // unknown request
               rev =0;
            }
      }
   else
      {
         // no valid client
         //
      fxTransport->Send(TGo4TaskHandler::Get_fgcERROR());
      TGo4Log::Debug(" TaskManager: ServeClient received invalid login, closing negotiation port ", cliname.Data());
      fxServer->SetDisConnect(fxTransport); // timer shall do the Close() of negotiation
      //TGo4Log::Debug(" TaskManager: Waiting for timer Close() of negotiation port ... ", cliname);
      fxServer->WaitForClose(); // poll until timer has returned from close
//      delete fxTransport;
//      fxTransport=0;
//      TGo4Log::Debug(" TaskManager: Closed and deleted negotiation port");
      TGo4Log::Debug(" TaskManager: Finished negotiations with client %s ", cliname.Data());

      return 0;
      }

   // finally, we close the channel again...
   recvchar=fxTransport->RecvRaw("dummy"); // get exit message
   if(recvchar && !strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
      {
      fxServer->SetDisConnect(fxTransport); // timer shall do the Close() of negotiation
      TGo4Log::Debug(" TaskManager: Waiting for timer Close() of negotiation to client %s ... ", cliname.Data());
      fxServer->WaitForClose(); // poll until timer has returned from close
      TGo4Log::Debug(" TaskManager: Finished negotiations with client %s ", cliname.Data());
      }
   else // if (!strcmp(revchar,TGo4TaskHandler::Get_fgcOK()))
      {
      TGo4Log::Debug(" TaskManager: ERROR on closing down negotiation channel, client %s   ", cliname.Data());
      throw TGo4RuntimeException();
      }
   return rev;
   }

Go4CommandMode_t TGo4TaskManager::ClientLogin()
{
if(fxTransport==0) return kGo4ComModeRefused;
TString purpose;
TString account;
TString passwd;
char* recvchar=fxTransport->RecvRaw("dummy"); // first receive OK string
if(recvchar && !strcmp(recvchar,TGo4TaskHandler::Get_fgcOK()))
{
	//return kGo4ComModeController; // old protocol: no password
   ///
   TGo4Log::Debug(" TaskManager::ClientLogin getting login...");
   recvchar=fxTransport->RecvRaw("dummy"); // get purpose of client (master or slave)
   if(recvchar==0) return kGo4ComModeRefused;
   purpose=recvchar;
   //std::cout <<"ClientLogin got purpose "<<purpose.Data() << std::endl;
   recvchar=fxTransport->RecvRaw("dummy"); // login account
   if(recvchar==0) return kGo4ComModeRefused;
   account=recvchar;
   //std::cout <<"ClientLogin got account "<<account.Data() << std::endl;
   recvchar=fxTransport->RecvRaw("dummy"); // login password
   if(recvchar==0) return kGo4ComModeRefused;
   passwd=recvchar;
//   std::cout <<"ClientLogin got passwd "<<passwd.Data() << std::endl;
//   std::cout <<"observer account is "<<TGo4TaskHandler::fgxOBSERVERACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxOBSERVERACCOUNT.GetTitle() << std::endl;
//   std::cout <<"controller account is "<<TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetTitle() << std::endl;
//   std::cout <<"admin account is "<<TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetTitle() << std::endl;

   // first check if client matches our own purpose:
   Bool_t matching=kFALSE;
   if(fxServer->IsMaster())
      {
           if(purpose==TGo4TaskHandler::fgcSLAVE) matching=kTRUE;
      }
   else
      {
           if(purpose==TGo4TaskHandler::fgcMASTER) matching=kTRUE;
      }
   if(!matching)
      {
         TGo4Log::Debug(" TaskManager: Client does not match Server, Login failed!!!");
         return kGo4ComModeRefused;
      }

   // check password and account:
   if(account==TGo4TaskHandler::fgxOBSERVERACCOUNT.GetName()
      && passwd==TGo4TaskHandler::fgxOBSERVERACCOUNT.GetTitle())
      {
         TGo4Log::Debug(" TaskManager: Client logged in as observer");
         return kGo4ComModeObserver;
      }
   else if(account==TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetName()
         && passwd==TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetTitle())
      {
         // avoid multiple controllers at this server:
         if(fbHasControllerConnection)
            {
               TGo4Log::Debug(" TaskManager: Client logged in as 2nd controller, will be observer");
               return kGo4ComModeObserver;
            }
         else
            {
               TGo4Log::Debug(" TaskManager: Client logged in as controller");
               return kGo4ComModeController;
            }
      }
   else if(account==TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetName()
         && passwd==TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetTitle())
      {
         // avoid multiple controllers at this server:
         if(fbHasControllerConnection)
            {
               TGo4Log::Warn(" TaskManager: Client logged in as 2nd controller, will be observer");
               return kGo4ComModeObserver;
            }
         else
            {
               TGo4Log::Debug(" TaskManager: Client logged in as administrator");
               return kGo4ComModeAdministrator;
            }
      }

   else
      {
         TGo4Log::Debug(" TaskManager: Client Login failed!!!");
         return kGo4ComModeRefused;
      }
}
return kGo4ComModeRefused;
}

Int_t TGo4TaskManager::ConnectClient(const char* client, const char* host, Go4CommandMode_t role)
{
   Int_t rev=0;
   // check first if client of that name already exists:
   TString cliname=client;
   if (!AddClient(cliname.Data(),host,role)) rev=1;
   return rev;
}


Int_t TGo4TaskManager::DisConnectClient(const char* name, Bool_t clientwait)
{
   TGo4Log::Info("TaskManager is disconnecting client %s %s ...", name, clientwait ? "with waiting" : "with no wait!" );
   //TGo4LockGuard listguard(fxListMutex);
      // this mutex
      // might deadlock between connector thread and local command thread
      // in case of timeout: command thread inits disconnect by client request
      // but if this fails, connector thread itself wants to finish disconnection hard
   Int_t rev=0;
   TGo4TaskHandler* han=GetTaskHandler(name);
   rev=DisConnectClient(han,clientwait);
   return rev;
}

Int_t TGo4TaskManager::DisConnectClient(TGo4TaskHandler * taskhandler, Bool_t clientwait)
{
Int_t rev=0;
if(taskhandler!=0)
   {
      fbClientIsRemoved=kFALSE; // reset the flag for waiting commander thread
      TString tname=taskhandler->GetName();
      Bool_t iscontrollertask=(taskhandler->GetRole()>kGo4ComModeObserver);
      fxServer->SendStopBuffers(tname); // suspend remote threads from socket receive
      if(clientwait)
         {
         // wait for OK string sent by client over connector negotiation port
         char* revchar = fxTransport->RecvRaw("dummy"); // wait for client close ok
         if(!(revchar && !strcmp(revchar,TGo4TaskHandler::Get_fgcOK())))
            {
               TGo4Log::Debug(" TaskManager %s; negotiation ERROR after client disconnect!",GetName());
               rev+=1;
               //throw TGo4RuntimeException();
            }
         } // if(clientwait)
      if(!taskhandler->DisConnect(clientwait))rev+=1;
      if (!RemoveTaskHandler(tname.Data()))   rev+=2;
      if (rev==0)
         {
            // all right, we reset flags
            fuTaskCount--;            // set number of still connected client tasks
            if(iscontrollertask) fbHasControllerConnection=kFALSE;
            fbClientIsRemoved=kTRUE; // this flag tells the main thread we are done
            TGo4Log::Debug(" TaskManager: client %s has been disconnected.  ", tname.Data());
         }
      else
         {
            // something went wrong, warn the user
            TGo4Log::Debug(" TaskManager: client %s disconnect ERROR %d occured !! ", tname.Data(),rev);
         }

   }

else //// if(taskhandler!=0)
   {
      // no such client
      TGo4Log::Debug(" TaskManager: FAILED to disonnect client -- no such client! ");
      rev=-1;
   }
return rev;
}

Bool_t TGo4TaskManager::AddClient(const char* client, const char* host, Go4CommandMode_t role)
{
  TGo4TaskHandler* han=NewTaskHandler(client);
  if (han==0)
   {
       TGo4Log::Warn(" !!! TaskManager::AddClient ERROR: client of name %s is already exisiting !!! ",client);
       fxTransport->Send(TGo4TaskHandler::Get_fgcERROR()); // tell client we refuse connection
       return kFALSE;
   }

if(han->Connect(host,fxTransport))
   {
      // successful connection:
      TGo4Log::Info(" TaskManager: Succesfully added new client %s (host %s, ports %d,%d,%d) ",
               client, host, han->GetComPort(), han->GetStatPort(), han->GetDatPort());
      fuTaskCount++;
      han->SetRole(role);
      if(role>kGo4ComModeObserver) fbHasControllerConnection=kTRUE;
      fxServer->SetCurrentTask(client); // this will set the direct link to the new client handler
      fxServer->SendStatusMessage(1,kTRUE,"%s::Client %s is logged in at %s as %s",
         client,client,fxServer->GetName(),TGo4Command::GetModeDescription(han->GetRole()) );
      return kTRUE;
    }
else
   {
      TGo4Log::Error(" TaskManager: ERROR on connecting new client %s (host %s)", client, host);
      RemoveTaskHandler(client);
      return kFALSE;
   }
return kFALSE;
}


Bool_t TGo4TaskManager::AddTaskHandler(TGo4TaskHandler* han)
{
   Bool_t rev=kFALSE;
   {
   TGo4LockGuard listguard(fxListMutex);
      if(fxTaskList->FindObject(han)==0)
         // is taskhandler already in list?
            {
               //no, add the new taskhandler
               fxTaskList->AddLast(han);
               rev=kTRUE;
            }
         else
            {
               // yes, do nothing
               rev=kFALSE;
            }
   } //  TGo4LockGuard
   return rev;
}


TGo4TaskHandler* TGo4TaskManager::NewTaskHandler(const char* name)
{
   TGo4TaskHandler* han=new TGo4TaskHandler(name,fxServer,kFALSE, fxServer->IsMaster());
   if(AddTaskHandler(han))
      {
         // success, taskhandler was not already existing
         return han;
      }
   else
      {
      // error, taskhandler of this name was already there
      delete han;
      return 0;

      }
   return 0;
}

Bool_t TGo4TaskManager::RemoveTaskHandler(const char* name)
{
   Bool_t rev=kTRUE;
   TGo4TaskHandler* taskhandler;
   {
   TGo4LockGuard listguard(fxListMutex);
      TObject* obj=fxTaskList->FindObject(name);
      taskhandler= (TGo4TaskHandler*) fxTaskList->Remove(obj);
            // Remove will do nothing if obj==0; on success, it returns pointer to
            // removed object
   } //TGo4LockGuard
   if(taskhandler!=0)
      {
         // test if we have removed the currently active taskhandler
         TGo4TaskHandler* currenttaskhandler=fxServer->GetCurrentTaskHandler();
         if(taskhandler==currenttaskhandler)
            {
               // yes, then set current task to the next in list
               fxServer->SetCurrentTask(0); // will also start the work threads again
            }
         else // if (taskhandler==currenttaskhandler)
            {
               // no, the current task remains
               fxServer->StartWorkThreads(); // but need to start the work threads
            }
         delete taskhandler;
   } // if (taskhandler!=0)
   else
      {
         // no such handler, do nothing
         rev=kFALSE;
      }
   return rev;
}



TGo4TaskHandler* TGo4TaskManager::GetTaskHandler(const char* name)
{
   TGo4TaskHandler* th=0;
    {
      TGo4LockGuard listguard(fxListMutex);
      th= (TGo4TaskHandler*) fxTaskList->FindObject(name);
    } //TGo4LockGuard
   return th;
}

TGo4TaskHandler* TGo4TaskManager::GetLastTaskHandler()
{
   TGo4TaskHandler* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      th= (TGo4TaskHandler*) fxTaskList->Last();
    } //TGo4LockGuard
   return th;
}

TGo4TaskHandler* TGo4TaskManager::NextTaskHandler(Bool_t reset)
{
   TGo4LockGuard listguard(fxListMutex);
   if(reset) fxTaskIter->Reset();
   TGo4TaskHandler* th=dynamic_cast<TGo4TaskHandler*>(fxTaskIter->Next());
   return th;
}


Int_t TGo4TaskManager::WaitForClientRemoved()

{
   Int_t count=0;
   while(!fbClientIsRemoved)
      {
         if(count>TGo4TaskManager::fgiDISCONCYCLES)
            {
               return -1;
            }
         else if(fxServer->IsTerminating())
            {
               return -2;
            }
         else
            {
               TGo4Thread::Sleep(TGo4TaskManager::fguDISCONTIME);
               ++count;
            }
      }
   fbClientIsRemoved=kFALSE; //  reset for next time
   return count;

}

UInt_t TGo4TaskManager::GetNegotiationPort()
{
   if(fxTransport)
      {
         fuNegotiationPort = fxTransport->GetPort();
      }
//   std::cout << "...........Taskmanager found negotiation port "<< fuNegotiationPort << std::endl;
   return fuNegotiationPort;
}
