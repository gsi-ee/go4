#include "TGo4TaskHandler.h"

#include "Riostream.h"
#include <stdlib.h>
#include "TSystem.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Socket.h"
#include "TGo4BufferQueue.h"
#include "TGo4RuntimeException.h"
#include "TGo4DataRunnable.h"
#include "TGo4StatusRunnable.h"
#include "TGo4CommandRunnable.h"
#include "TGo4ServerTask.h"
#include "TGo4TaskHandlerStatus.h"

const UInt_t TGo4TaskHandler::fguCONNECTORPORT=5000; // port no of default connector 9229
                                                     // 5000 is the first number of ROOT portscan
const UInt_t TGo4TaskHandler::fguTRANSPORTCHECKDELAY=5000; // delay in ms for transport init check

const UInt_t TGo4TaskHandler::fguSTATUSQUEUESIZE=1000;
const UInt_t TGo4TaskHandler::fguDATAQUEUESIZE=1000;
const UInt_t TGo4TaskHandler::fguCOMMANDQUEUESIZE=1000;


const Int_t TGo4TaskHandler::fgiPORTWAITCYCLES=150;//60

const UInt_t TGo4TaskHandler::fguPORTWAITTIME=200;//500

const Int_t TGo4TaskHandler::fgiTHREADSTOPCYCLES=6;

const UInt_t TGo4TaskHandler::fguTHREADSTOPTIME=500;

const Text_t TGo4TaskHandler::fgcCONNECT[]="CONNECT-VERSION-300";
const Text_t TGo4TaskHandler::fgcDISCONNECT[]="DISCONNECT-VERSION-300";

const char* TGo4TaskHandler::fgcOK = "OK-VERSION-300";
const char* TGo4TaskHandler::fgcERROR = "ERROR-VERSION-300";

const Text_t TGo4TaskHandler::fgcMASTER[]="Master-VERSION-300";
const Text_t TGo4TaskHandler::fgcSLAVE[]="Slave-VERSION-300";

const Text_t TGo4TaskHandler::fgcCOMMANDTHREAD[]="COMMAND-";
const Text_t TGo4TaskHandler::fgcSTATUSTHREAD[]="STATUS-";
const Text_t TGo4TaskHandler::fgcDATATHREAD[]="DATA-";

TNamed TGo4TaskHandler::fgxOBSERVERACCOUNT("observer","go4view");
TNamed TGo4TaskHandler::fgxCONTROLLERACCOUNT("controller","go4ctrl");
TNamed TGo4TaskHandler::fgxADMINISTRATORACCOUNT("admin","go4super");

TGo4TaskHandler::TGo4TaskHandler(const char* name, TGo4ThreadManager* threadmanager, Bool_t clientmode, Bool_t mastermode,UInt_t negotiationport)
   :TNamed(name,"This is a Go4 Task Handler"),
   fbIsAborting(kFALSE), fiComPort(0),fiStatPort(0),fiDatPort(0),fiRole(kGo4ComModeController)
{
   fbClientMode=clientmode;
   fbMasterMode=mastermode;
   if(threadmanager==0)
      {
         // error
         TGo4Log::Debug(" TaskHandler -- constructor error, unspecified ThreadManager: aborting ");
         //throw TGo4RuntimeException();
      }
   else
      {
         // everything o.k.
      }

   // set port number for the client server negotiation channel:
   if(negotiationport==0)
      {
         // default: use taskhandler intrinsic port number
         fuNegPort=TGo4TaskHandler::fguCONNECTORPORT;
      }
   else
      {
         // use dynamic port number given by taskhandler owner
         fuNegPort=negotiationport;
      }

   fxThreadManager=threadmanager;
   fxThreadHandler=fxThreadManager->GetWorkHandler();
   TString namebuffer;
   fxInvoker=0;
   fxCommandQueue= new TGo4BufferQueue("Command"); // receiv commands
   fxStatusQueue= new TGo4BufferQueue("Status");   // send status buffer
   fxDataQueue= new TGo4BufferQueue("Data");         // send data

   fxCommandTransport=new TGo4Socket(IsClientMode());
   fxStatusTransport=new TGo4Socket(IsClientMode());
   fxDataTransport=new TGo4Socket(IsClientMode());
   namebuffer.Form("CommandRunnable of %s",GetName());
   // command runnable receivermode: receiving as slave and sending as master
   fxCommandRun=new TGo4CommandRunnable(namebuffer.Data(), fxThreadManager, this, !IsMasterMode());

   namebuffer.Form("StatusRunnable of %s",GetName());
   // status runnable receivermode: sending as slave and receiving as master
   fxStatusRun=new TGo4StatusRunnable(namebuffer.Data(), fxThreadManager, this, IsMasterMode());

   namebuffer.Form("DataRunnable of %s",GetName());
   // data runnable receivermode: sending as slave and receiving as master
   fxDataRun=new TGo4DataRunnable(namebuffer.Data(), fxThreadManager, this, IsMasterMode());

  // adding runnables to thread handler who takes over the responsibility...:
   namebuffer.Form("%s%s",fgcCOMMANDTHREAD,GetName());
   fxComName=namebuffer;
   fxThreadHandler->NewThread(GetComName(), fxCommandRun);
   namebuffer.Form("%s%s",fgcSTATUSTHREAD,GetName());
   fxStatName=namebuffer;
   fxThreadHandler->NewThread(GetStatName(),fxStatusRun);
   namebuffer.Form("%s%s",fgcDATATHREAD,GetName());
   fxDatName=namebuffer;
   fxThreadHandler->NewThread(GetDatName(),fxDataRun);
   if(IsClientMode())
      TGo4Log::Debug(" New TaskHandler %s in client mode ",GetName());
   else
      TGo4Log::Debug(" New TaskHandler %s in server mode ",GetName());

 // adjust queue size to our wishes
   fxCommandQueue->SetMaxEntries(TGo4TaskHandler::fguCOMMANDQUEUESIZE);
   fxDataQueue->SetMaxEntries(TGo4TaskHandler::fguDATAQUEUESIZE);
   fxStatusQueue->SetMaxEntries(TGo4TaskHandler::fguSTATUSQUEUESIZE);


}

TGo4TaskHandler::~TGo4TaskHandler()
{
   fxThreadHandler->RemoveThread(GetComName());
   fxThreadHandler->RemoveThread(GetDatName());
   fxThreadHandler->RemoveThread(GetStatName());
   delete fxCommandTransport;
   delete fxStatusTransport;
   delete fxDataTransport;
   delete fxCommandQueue;
   delete fxStatusQueue;
   delete fxDataQueue;
}

TGo4Socket* TGo4TaskHandler::ServerRequest(const char* host)
   {
   if(fbClientMode)
      {
      // we are client and want access to the server task (connector runnable)
         TGo4Socket* connector=new TGo4Socket(kTRUE); // raw socket transport
         connector->Open(host,fuNegPort); // open connection to server's connector runnable
         if(ServerLogin(connector, GetRole()))
            {
               // client and server know each other- we continue
               TString myname=fxThreadManager->GetName();
               //cout <<"ServerRequest sends name "<<myname.Data() << endl;
               connector->Send(myname.Data()); // tell server the client name
               connector->Send(gSystem->HostName()); // tell server our machine hostname
               return connector;
            } //if(!strcmp(localbuffer,fgcOK))
         else
            {
               // error: client does not match to server-- connect failed
               connector->Send(Get_fgcERROR()); // send dummy strings, server will come out of receive
               connector->Send(Get_fgcERROR()); // might check the errortext at server later
#ifdef WIN32
               gSystem->Sleep(1000);
#endif
               connector->Close();
               delete connector;
               TGo4Log::Debug(" TaskHandler %s server connection ERROR ",GetName());
               return 0;

            } // else if(!strcmp(localbuffer,fgcOK))
         return 0;

      } //if(fbClientMode)

   else
      {
         // we _are_ a server task handler , shall not request to our own Server task
         return 0;
      }
   }


Bool_t TGo4TaskHandler::Connect(const char* host, TGo4Socket* connector)
// establish connection of all three channels
{
   TGo4Log::Debug(" TaskHandler %s connecting to host %s ...",GetName(),host);
   Text_t* recvchar;
   if(fbClientMode)
      {
      /////////////////// CLIENT MODE /////////////////////////////////////////////
      SetAborting(kFALSE); // reset if we reconnect after exception disconnect
      fxHostName=host;// remember hostname for later DisConnect
      if(connector==0)
         {
            // normal mode for client: we establish negotiation connection first
            connector=ServerRequest(host); // get negotiation channel from server
         }
      if(connector)
         {
            // request was successful, we keep talking:
            connector->Send(fgcCONNECT); // tell server we want to connect
            recvchar=connector->RecvRaw("dummy");
            if(recvchar==0)
               {
                  TGo4Log::Debug(" TaskHandler %s; Error on server connection, abortin... ",GetName());
                  connector->Close();
                  throw TGo4RuntimeException();
               }
            if(!strcmp(recvchar,Get_fgcERROR()))
               {
                  // server refuses to connect us, we abort
                  TGo4Log::Debug(" TaskHandler %s; Server refuses Connection",GetName());
                  connector->Send(fgcOK); // tell server we are through
#ifdef WIN32
                  gSystem->Sleep(1000);
#endif
                  connector->Close();
                  throw TGo4RuntimeException();
               }
            if(!ConnectClientChannel("Command",connector,fxCommandTransport,host))
               {
                  TGo4Log::Debug(" TaskHandler %s; negotiation error, FAILED to open Command Channel",GetName());
                  throw TGo4RuntimeException();
               }
           if(!ConnectClientChannel("Status",connector,fxStatusTransport,host))
               {
                  TGo4Log::Debug(" TaskHandler %s; negotiation error, FAILED to open Status Channel",GetName());
                  throw TGo4RuntimeException();
               }
           if(!ConnectClientChannel("Data",connector,fxDataTransport,host))
               {
                  TGo4Log::Debug(" TaskHandler %s; negotiation error, FAILED to open Data Channel",GetName());
                  throw TGo4RuntimeException();
               }
            connector->Send(fgcOK); // tell server we finish negotiation
#ifdef WIN32
            gSystem->Sleep(1000);
#endif
            connector->Close();
            TGo4Log::Debug(" TaskHandler %s closed negotiation connection ",GetName());
            delete connector;
         }
      else
         {
         // something failed
            TGo4Log::Debug(" TaskHandler %s server connection ERROR ",GetName());
            return kFALSE;
         } // if(connector)
      } //if(fbClientMode)
   else
      {
       /////////////////// SERVER MODE /////////////////////////////////////////////
      const Text_t* client=GetName();
      if(connector==0) return kFALSE;
      connector->Send(TGo4TaskHandler::fgcOK);
            // first ok to initialize client, fgcERROR would abort client
      if (!ConnectServerChannel("Command",connector, fxCommandTransport, host))
         {
            TGo4Log::Debug(" TaskHandler: Command channel connect ERROR for client %s ",client);
            return kFALSE;
         }
      if (!ConnectServerChannel("Status",connector, fxStatusTransport, host))
         {
            TGo4Log::Debug(" TaskManager: Status channel connect ERROR for client %s ",client);
            return kFALSE;
         }
      if (!ConnectServerChannel("Data",connector, fxDataTransport, host))
         {
            TGo4Log::Debug(" TaskManager: Data channel connect ERROR for client %s ",client);
            return kFALSE;
         }
      } //if(fbClientMode)

   fiComPort=WaitGetPort(fxCommandTransport); // set port numbers for runnables
   fiStatPort=WaitGetPort(fxStatusTransport);
   fiDatPort=WaitGetPort(fxDataTransport);
   StartTransportThreads();

   return kTRUE;
}

Bool_t TGo4TaskHandler::ServerLogin(TGo4Socket* connector, Go4CommandMode_t account)
{
if(connector==0) return kFALSE;
//cout <<"ServerLogin with mode "<<account << endl;
//cout <<"observer account is "<<TGo4TaskHandler::fgxOBSERVERACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxOBSERVERACCOUNT.GetTitle() << endl;
//cout <<"controller account is "<<TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxCONTROLLERACCOUNT.GetTitle() << endl;
//cout <<"admin account is "<<TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetName()<<", "<<TGo4TaskHandler::fgxADMINISTRATORACCOUNT.GetTitle() << endl;

connector->Send(fgcOK); // tell server that we are a valid client

// tell server if we are master or slave:
if(fbMasterMode)
   connector->Send(fgcMASTER);
else
   connector->Send(fgcSLAVE);

// now send accountname and password:
switch (account)
{
   case kGo4ComModeObserver:
      connector->Send(fgxOBSERVERACCOUNT.GetName());
      connector->Send(fgxOBSERVERACCOUNT.GetTitle());
      break;

   case kGo4ComModeController:
      connector->Send(fgxCONTROLLERACCOUNT.GetName());
      connector->Send(fgxCONTROLLERACCOUNT.GetTitle());
      break;

   case kGo4ComModeAdministrator:
      connector->Send(fgxADMINISTRATORACCOUNT.GetName());
      connector->Send(fgxADMINISTRATORACCOUNT.GetTitle());
      break;

   default:
      connector->Send(Get_fgcERROR());
      connector->Send(Get_fgcERROR());
      break;
}

char * recvchar=connector->RecvRaw("dummy");// handshake back if it is ok
if(recvchar && !strcmp(recvchar,fgcOK)) return kTRUE;
return kFALSE;
}

Bool_t TGo4TaskHandler::DisConnect(Bool_t waitforclient)
{
   TGo4Log::Debug(" TaskHandler %s disconnecting ",GetName());
   //TGo4Task* task=dynamic_cast<TGo4Task*>(fxThreadManager);
   if(fbClientMode)
      {
      if(!IsAborting())
         {
         // normal DisConnect mode:
         // we are client, have to tell server to let us go...
         TGo4Socket* connector=ServerRequest(GetHostName()); // get negotiation channel from server
         if(connector)
               {
                   // request was successful, we keep talking:
//                  task->StopWorkThreads();
//                  if(task->IsMaster())
//                       fxCommandQueue->Clear();// only clear command queue on master side,
//                                              // otherwise we lose status messages from server
//                  task->WakeCommandQueue(TGo4Task::Get_fgiTERMID()); // will stop local command thread, and remote
//                  task->SendStopBuffers(); // only stop remote threads if login was successful!
                  /////
                  connector->Send(fgcDISCONNECT); // tell server we want to disconnect
                  StopTransportThreads(kTRUE);// wait until threads are really stopped
                  //cout <<"TASKHANDLER DISCONNECT closing the transports now.... " <<endl;
                  CloseChannels();
                  connector->Send(fgcOK); // tell server we finished transports
                     // server will close its transport sockets after this

                  connector->Send(fgcOK); // second ok to let server shutdown connector
#ifdef WIN32
                  gSystem->Sleep(1000);
#endif
                  connector->Close();
                  delete connector;
               } ////if(connector)
            else
               {
               // something failed
                  TGo4Log::Debug(" TaskHandler %s server disconnect login ERROR - Trying Fast DisConnect... ",GetName());
                  StopTransportThreads(kFALSE);
                  CloseChannels("force");
                  //return kFALSE;
               }
         }
      else // if(!IsAborting())
         {
            // DisConnect after exception, fast Close without negotiations
            TGo4Log::Debug(" Client Aborting mode: Fast DisConnect... ",GetName());
            StopTransportThreads(kFALSE);
            CloseChannels("force");
         }


      } ////if(fbClientMode)
   else
      {
         StopTransportThreads(waitforclient);// wait until threads are really stopped
         CloseChannels();
      }
   return kTRUE;
}

void TGo4TaskHandler::CloseChannels(Option_t* opt)
{
//TString option=opt;
//if(option=="force")
//   {
//   cout <<"sSSSSSSSSSSSS CloseChannels sending abort buffer" << endl;
//   TGo4Task* task=dynamic_cast<TGo4Task*>(fxThreadManager);
//   // provoke socket exception on receiver channels:
//   if(fbMasterMode)
//      {
//         fxCommandTransport->SendBuffer(task->GetAbortBuffer());
//      }
//   else
//      {
//         fxDataTransport->SendBuffer(task->GetAbortBuffer());
//         fxStatusTransport->SendBuffer(task->GetAbortBuffer());
//
//      }
//   }
fxDataTransport->Close(opt);
fxCommandTransport->Close(opt);
fxStatusTransport->Close(opt);
ClearQueues();

}

void TGo4TaskHandler::ClearQueues()
{
fxDataQueue->Clear();
fxCommandQueue->Clear();
fxStatusQueue->Clear();

}

TGo4TaskHandlerStatus * TGo4TaskHandler::CreateStatus()
{
   TGo4TaskHandlerStatus* state= new TGo4TaskHandlerStatus(GetName());
   // we are friend of our status class, may use private setters:
   state->SetFlags(fbIsAborting);
   state->SetPorts(fuNegPort, fiComPort, fiStatPort, fiDatPort);
   state->SetNames(GetComName(),GetStatName(),GetDatName(),GetHostName());
   return state;
}



Bool_t TGo4TaskHandler::ConnectServerChannel(const char* name, TGo4Socket* negotiator, TGo4Socket* channel, const char* host)
{
Text_t* revchar=0;
Int_t waitresult=0;
UInt_t port=0;
TGo4ServerTask* server=dynamic_cast<TGo4ServerTask*>(fxThreadManager);
if(server==0)
   {
     TGo4Log::Debug(" TaskHandler: Channel %s open ERROR: no server task ",name);
     return kFALSE;
   }
if(negotiator==0 || !negotiator->IsOpen())
   {
     TGo4Log::Debug(" TaskHandler: Channel %s open ERROR: no negotiation channel ",name);
     return kFALSE;
   }
if(channel==0)
   {
     TGo4Log::Debug(" TaskHandler: Channel %s open ERROR: no TGo4Socket instance ",name);
     return kFALSE;
   }
const Text_t* client=GetName(); // taskhandler name is client name
// in server mode, we connect by the connector thread:
// need timer mechanism for proper registration of ROOT sockets (timer is main thread)
// only root sockets connected in main application thread will be cleaned up
server->SetConnect(channel, host,0,kTRUE);
   // tell the ServerTask timer we want to connect; portscan. we keep server socket open for windows
waitresult=server->WaitForOpen(); // wait for the server Open() call by timer
if(waitresult<0)
   {
      // open timeout
      TGo4Log::Debug(" TaskHandler: Channel %s open TIMEOUT for client %s ",name, client);
      return kFALSE;
   }
else
   {
      // ok, proceed
   }
port=WaitGetPort(channel);
if (port<0)
   {
      TGo4Log::Debug(" TaskHandler: Channel %s getport TIMEOUT for client %s ",name, client);
      return kFALSE;
   }
else {}
negotiator->Send(TGo4TaskHandler::fgcOK); // tell client we are ready to connect
TString localbuffer;
localbuffer.Form("%d",port);
negotiator->Send(localbuffer.Data()); // tell client the port number;
//cout <<"------- ConnectServerChannel offers portnumber "<< localbuffer<< " for Channel "<< name << endl;
revchar=negotiator->RecvRaw("dummy"); // wait for client connection ok
if(revchar && !strcmp(revchar,TGo4TaskHandler::fgcOK))
   {
      // o.k., client tells us connection is open, continue
   }
else
   {
      // something went wrong, no ok
      TGo4Log::Debug(" TaskHandler: Negotiation ERROR after Channel %s open for client %s ",
               name, client);
      return kFALSE;
      //throw TGo4RuntimeException();
   }

waitresult=server->WaitForConnection(); // we also check ourselves if timer has returned from server open
if(waitresult<0)
   {
      // connect timeout
      TGo4Log::Debug(" TaskHandler: Channel %s connect TIMEOUT for client %s ", name, client);
      return kFALSE;
   }
else
   {
      // ok, proceed
   }
TGo4Log::Debug(" TaskHandler: Channel %s for client %s open!",name, client);
return kTRUE;
}

Bool_t TGo4TaskHandler::ConnectClientChannel(const char* name, TGo4Socket * negotiator, TGo4Socket * channel, const char* host)
{
//
Text_t* recvchar=0;
Int_t port=0;
if(negotiator==0 || !negotiator->IsOpen())
   {
     TGo4Log::Debug(" TaskHandler: Channel %s open ERROR: no negotiation channel ",name);
     return kFALSE;
   }
if(channel==0)
   {
     TGo4Log::Debug(" TaskHandler: Channel %s open ERROR: no TGo4Socket instance ",name);
     return kFALSE;
   }

recvchar=negotiator->RecvRaw("dummy");// get OK from server to connect first channel
if(recvchar && !strcmp(recvchar,fgcOK))
   {
      // get portnumber  from server:
      recvchar=negotiator->RecvRaw("dummy");
      TString localbuffer = recvchar;
      port=atoi(localbuffer.Data());
//      cout <<"------- TaskHandler::Connect client tries port  "<< port << "for Channel "<< name << endl;
      channel->Open(host,port);  // in client mode, we connect directly (main thread!)
      TGo4Log::Debug(" TaskHandler %s: Channel %s open!",GetName(), name );
      negotiator->Send(fgcOK); // tell server that open is ready
      return kTRUE;
   }
else
   {
      TGo4Log::Debug(" TaskHandler %s; negotiation error, FAILED to open Channel %s ",
               GetName(), name);
      return kFALSE;
      //throw TGo4RuntimeException();
   } // if(!strcmp(recvchar,fgcOK))
}
Int_t TGo4TaskHandler::WaitGetPort(TGo4Socket* sock)

{
   Int_t count=0;
   Int_t port=0;
   while(port==0)
   {
      port=sock->GetPort(); // get dynamically bound port number of server socket
//      cout <<"------- WaitGetPort has next portnumber "<< port << endl;
      if(count>fgiPORTWAITCYCLES)
            {
               return -1;
            }
      else if(fxThreadManager->IsTerminating())
            {
               return -2;
            }
      else
            {
              TGo4Thread::Sleep(fguPORTWAITTIME);
              ++count;
            }
   }
   return port;

}

void TGo4TaskHandler::StartTransportThreads()
{
fxThreadHandler->Start(GetComName());         // start runnables
fxThreadHandler->Start(GetStatName());
fxThreadHandler->Start(GetDatName());
}

Bool_t TGo4TaskHandler::StopTransportThreads(Bool_t wait)
{
Bool_t rev=kTRUE;
fxThreadHandler->Stop(GetComName());
if(IsMasterMode())
   {
   TGo4BufferQueue* comq= dynamic_cast<TGo4BufferQueue*>(GetCommandQueue());
   if(comq)
      {
         //cout <<"SSSSSStopTransportThreads Waking command queue" << endl;
         comq->Wake();
      }
   }
fxThreadHandler->Stop(GetStatName());
fxThreadHandler->Stop(GetDatName());
if(wait)
   {
         rev&=WaitThreadStop(GetComName());
         rev&=WaitThreadStop(GetStatName());
         rev&=WaitThreadStop(GetDatName());
   }
return rev;
}

Bool_t TGo4TaskHandler::WaitThreadStop(const char* name)
{
if(name==0) return kFALSE;
TGo4Thread* thread=fxThreadHandler->GetThread(name);
if(thread==0) return kFALSE;
Int_t t=0;
Bool_t timeout=kFALSE;
while(!thread->IsWaiting())
   {
      TGo4Log::Debug(" TaskHandler Disconnect --  waiting for runnable %s to stop... ",name);
      TGo4Thread::Sleep(TGo4TaskHandler::fguTHREADSTOPTIME);
      if((t++>=TGo4TaskHandler::fgiTHREADSTOPCYCLES))
         {
            timeout=kTRUE;
            break;
         }
   }
return (!timeout);
}

void TGo4TaskHandler::SetAdminAccount(const char* name, const char* passwd)
{
if(name) fgxADMINISTRATORACCOUNT.SetName(name);
if(passwd) fgxADMINISTRATORACCOUNT.SetTitle(passwd);
}

void TGo4TaskHandler::SetCtrlAccount(const char* name, const char* passwd)
{
if(name) fgxCONTROLLERACCOUNT.SetName(name);
if(passwd) fgxCONTROLLERACCOUNT.SetTitle(passwd);
}

void TGo4TaskHandler::SetObservAccount(const char* name, const char* passwd)
{
if(name) fgxOBSERVERACCOUNT.SetName(name);
if(passwd) fgxOBSERVERACCOUNT.SetTitle(passwd);
}

const char* TGo4TaskHandler::Get_fgcOK()
{
   return fgcOK;
}

const char* TGo4TaskHandler::Get_fgcERROR()
{
   return fgcERROR;
}

UInt_t TGo4TaskHandler::Get_fguPORTWAITTIME()
{
   return fguPORTWAITTIME;
}

Int_t TGo4TaskHandler::Get_fgiPORTWAITCYCLES()
{
   return fgiPORTWAITCYCLES;
}
