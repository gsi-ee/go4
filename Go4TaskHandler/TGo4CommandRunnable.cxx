#include "TGo4CommandRunnable.h"

#include <signal.h>
#include <sstream>
using namespace std;

#include "Riostream.h"
#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4BufferQueue.h"
#include "TGo4Socket.h"
#include "TGo4SocketSignalHandler.h"
#include "TGo4CommandInvoker.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerAbortException.h"
#include "TGo4ClientTask.h"

#include "TGo4ComQuit.h"

TGo4CommandRunnable::TGo4CommandRunnable(const char* name,
                                         TGo4ThreadManager* man,
                                         TGo4TaskHandler* hand,
                                         Bool_t receivermode)
   :TGo4TaskHandlerRunnable(name,man,hand,receivermode)
{
   fxBufferQueue=dynamic_cast<TGo4BufferQueue*> (fxTaskHandler->GetCommandQueue() );
   fxTransport=fxTaskHandler->GetCommandTransport();
   fxInvoker=TGo4CommandInvoker::Instance();
}


TGo4CommandRunnable::~TGo4CommandRunnable()
{
}

Int_t TGo4CommandRunnable::Run(void* ptr)
{
   if(!CheckTransportOpen()) return 0;
   if(fbReceiverMode)
      {
      // wait for something from socket
      Int_t rev=fxTransport->ReceiveBuffer();
      if(rev>=0)
         {
          TBuffer* buffer=const_cast<TBuffer*> (fxTransport->GetBuffer());
          Int_t val=0;
          if(CheckStopBuffer(buffer,&val)) return 0; // stop for disconnect mode
          if(val>=0)
             {
             fxTransport->Send(TGo4TaskHandler::Get_fgcOK()); //acknowledge before execute
             TGo4Task* cli = dynamic_cast<TGo4Task*>(fxManager);
               // we have a direct command, execute this here:
             TGo4ComQuit* qcommand=0;
             Go4EmergencyCommand_t comvalue= (Go4EmergencyCommand_t) (val);
             // test here for different command values
             switch(comvalue)
                {
                case kComQuit:
                   if(fxTaskHandler->GetRole()==kGo4ComModeObserver) break;
                   TGo4Log::Debug(" Command runnable executing direct command QUIT... ");
                   //cli->Quit();
                   // note: need execution of quit in local command thread,
                   // because we are inside thread to be cancelled...
                   qcommand = new TGo4ComQuit;
                   cli->SubmitLocalCommand(qcommand);
                   break;
                case kComKillMain:
                   if(fxTaskHandler->GetRole()==kGo4ComModeObserver) break;
                   TGo4Log::Debug(" Command runnable executing direct command KILL MAIN... ");
                   cli->KillMain();
                   break;
                case kComRestartMain:
                   if(fxTaskHandler->GetRole()==kGo4ComModeObserver) break;
                   TGo4Log::Debug(" Command runnable executing direct command RESTART MAIN... ");
                   cli->RestartMain();
                   break;
                case kComCloseInput:
                     // this case should be treated by CheckStopBuffer, so:
                     TGo4Log::Debug("NEVER COME HERE: Command runnable has direct command CLOSE INPUT");
   //                TGo4Log::Debug(" Command runnable executing direct command CLOSE INPUT... ");
   //                GetThread()->Stop();
                   break;
                default:
                   TGo4Log::Debug(" Command runnable direct command value %d UNKNOWN! ",
                            comvalue);
                   break;


                }
            // end direct command section
             } // if(val>0)
      else
         {
         fxBufferQueue->AddBuffer(buffer, kTRUE);
         fxTransport->Send(TGo4TaskHandler::Get_fgcOK());
         }  // end if((val>0))
      } // if(rev>=0)
   else
      {
         if (TGo4SocketSignalHandler::GetLastSignal() == SIGWINCH)
            {
               // TSocket error because of window resize, do not abort!
               TGo4Log::Debug(" %s: caught SIGWINCH ", GetName());
               TGo4SocketSignalHandler::SetLastSignal(0); // reset
            }
         else if(fxManager->IsTerminating())
            {
               //cout <<"Receive error in "<<GetName()<< " while threadmanager is terminating. Ignored!" << endl;
               TGo4Log::Debug("Receive Error in %s during threadmanager termination. Ignored.",GetName());
               GetThread()->Stop();

            }
         else
            {
               if(fxTaskHandler->IsClientMode()) RedirectIO(); // only suppress output for analysis clients (correct shutdown if run in QtWindow!)
               TGo4Log::Debug("Receive Error in %s, aborting taskhandler...",GetName());
               throw TGo4TaskHandlerAbortException(this);
            }
       } // end if (rev>=0)
   }  //end if(fbReceiverMode)
   else
      {
         // get next command from queue or wait for it
        TBuffer* buf= fxBufferQueue->WaitBuffer();
            if (buf)
               // check if there is really an object from queue
               {
                    Bool_t stopmode=CheckStopBuffer(buf);
                     // send over inter task transport
                     fxTransport->SendBuffer(buf);
   //                  cout << "command runnable: sending buffer via transport" << endl;
                     fxBufferQueue->FreeBuffer(buf);
                     if(stopmode) return 0; // no handshake after stop buffer
                     Text_t* revchar=fxTransport->RecvRaw("dummy"); // wait for o.k. string
                     if(revchar==0)
                        {
                           // error, received garbage
                            if (TGo4SocketSignalHandler::GetLastSignal() == SIGWINCH)
                            //if (TGo4SocketSignalHandler::fgxLastSignal == kSigWindowChanged)
                              {
                                 // TSocket error because of window resize, do not abort!
                                 TGo4Log::Debug(" %s: caught SIGWINCH ",GetName());
                                 TGo4SocketSignalHandler::SetLastSignal(0); // reset
                                 //TGo4SocketSignalHandler::fgxLastSignal = (ESignals) 0;
                              }
                           else
                              {
                                 //RedirectIO(); // do not redirect output for command sender, will not terminate!
                                 TGo4Log::Debug(" !!!Connection Error -1 in CommandRunnable ''%s''!!!",GetName());
                                 throw TGo4TaskHandlerAbortException(this);
                              }
                        }
                     else
                        {
                            // we received something, proceed
                        } // end if(revchar==0)

                     if(!strcmp(revchar,TGo4TaskHandler::Get_fgcOK()) )
                        {
                           // fine, command has reached its destination, anyhow..
                        }
                     else if(!strcmp(revchar,TGo4TaskHandler::Get_fgcERROR()))
                        {
                           // client signals any kind of error with last command
                           TGo4Log::Debug(" CommandRunnable ''%s'' received command error string!!!",GetName());
                        }
                     else
                        {
                           // error, received something else
                           TGo4Log::Debug(" !!!Connection Error -3 in CommandRunnable ''%s''!!!",GetName());
                           //throw TGo4TerminateException(this);
                        }
               }
            else
               {
                  if(!GetThread()->IsRunning())
                        {
                           // when gui client is disconnected by slave server itself
                           // (server shutdown from other gui), we have to react on Queue wakeup
                           // in command queue from the TGo4TaskHandler::StopTransportThreads
                           //cout <<"Command runnable gets null from queue and is stopped!!!!" << endl;
                           return 0;
                        }
                  // do nothing, return for next wait
               }
     }   // end else if(fbReceiverMode)
   return 0;
}


void TGo4CommandRunnable::RedirectIO()
{
// note: this method is only called in case of aborting,
// so we do not care about our memory leak...JA
ostringstream* strout = new ostringstream;
//streambuf* cout_buffer = cout.rdbuf(); // would need it to recover cout
cout.rdbuf(strout->rdbuf());
ostringstream* strerr = new ostringstream;
//streambuf* cerr_buffer = cerr.rdbuf(); // would need to rcover cerr later
cerr.rdbuf(strerr->rdbuf());
}
